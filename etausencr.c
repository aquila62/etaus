/************************************************************************/
/* etausencr.c  Version 1.0.0                                           */
/* Encrypt an input stream with the etaus algorithm                     */
/* Copyright (C) 2016  Russell Stevens                                  */

/* This program is free software; you can redistribute it and/or        */
/* modify it under the terms of the GNU General Public License as       */
/* published by the Free Software Foundation; either version 2 of       */
/* the License, or (at your option) any later version.                  */

/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */

/* You should have received a copy of the GNU General Public License    */
/* along with this program; if not, write to:                           */

	/* Free Software Foundation, Inc.         */
	/* 59 Temple Place - Suite 330            */
	/* Boston, MA  02111-1307, USA.           */
/************************************************************************/

/**********************************************************/
/* The CRC algorithm in this program was copied from      */
/* http://www.hackersdelight.org/hdcodetxt/crc.c.txt      */
/**********************************************************/

/**********************************************************/
/* This program initializes the random number generator   */
/* state by overriding the state with a CRC hash based    */
/* on an input password parameter                         */
/* The algorithm used is a CRC feedback hash.             */
/**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "etaus.h"

/* length of input/output buffer */
#define BUFSZ (1024*1024)

/* print the command syntax */
void putstx(char *pgm)
   {
   fprintf(stderr,"Usage: %s password\n", pgm);
   fprintf(stderr,"Where password is an ASCII string\n");
   fprintf(stderr,"Example: %s abcd\n", pgm);
   fprintf(stderr,"Example: %s \"abcd efgh\"\n", pgm);
   fprintf(stderr,"Example: %s 'abcd efgh'\n", pgm);
   exit(1);
   } /* putstx */

/**************************************/
/* read one block of input from stdin */
/* return the length of the block     */
/* a partial block may be returned    */
/* maximum blocksize is BUFSZ         */
/* input may be binary data           */
/**************************************/
int getblk(unsigned char *blk)
   {
   int len;
   len = read(0,blk,BUFSZ);
   if (!len) return(EOF);
   if (len < 0)
      {
      fprintf(stderr,"getblk: read error\n");
      exit(1);
      } /* read error */
   return(len);
   } /* getblk */

/***************************************/
/* write one block of output to stdout */
/* maximum blocksize is determined by  */
/* the calling routine.                */
/* output is binary data               */
/***************************************/
void putblk(unsigned char *blk, int len)
   {
   int rslt;
   rslt = write(1,blk,len);
   if (rslt != len)
      {
      fprintf(stderr,"putblk: write error\n");
      exit(1);
      } /* write error */
   } /* putblk */

/* generate the CRC table */
/* See citation above for the CRC algorithm */
void bldtbl(unsigned int *table)
   {
   int j;
   unsigned int byte;
   unsigned int crc;
   unsigned int mask;
   for (byte = 0; byte <= 255; byte++)
      {
      crc = byte;
      for (j = 7; j >= 0; j--)
         {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
         } /* for each bit in byte */
      table[byte] = crc;
      } /* for each of 256 bytes in table */
   } /* bldtbl */

/* calculate the 32-bit CRC of a block of data */
/* This CRC calculation uses a CRC table */
/* See citation above for the CRC algorithm */
unsigned int crc32c(unsigned char *message, unsigned int *table)
   {
   int i;
   unsigned int byte, crc;
   i = 0;
   crc = 0xFFFFFFFF;
   while ((byte = message[i]) != 0) {
      crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
      i = i + 1;
   }
   return ~crc;
   } /* crc32c */

int main(int argc, char **argv)
   {
   int pswlen;                 /* length of password parameter */
   /* The CRC is followed by 4 bytes of zeros */
   /* to turn the CRC into a 4 byte message */
   /* terminated by zero */
   /* The CRC is a 32-bit little endian integer */
   /* No effort is made to eliminate zero bytes within */
   /* the CRC itself */
   unsigned int crc;           /* calculated CRC */
   unsigned int zero;          /* 4 bytes of zeros following the CRC */
   unsigned int tbl[256];      /* CRC table */
   unsigned int *statep;       /* pointer into et->state */
   unsigned int *stateq;       /* pointer for end of et->state */
   unsigned char psw[128];     /* password text */
   unsigned char *crcp;        /* pointer for 4 byte CRC */
   unsigned char *buf;         /* input/output buffer */
   etfmt *et;                  /* etaus RNG structure */
   if (argc != 2) putstx(*argv);    /* must have password parameter */
   /*******************************/
   /* validate length of password */
   /*******************************/
   pswlen = strlen(*(argv+1));    /* validate length of password */
   if (pswlen > 64)
      {
      fprintf(stderr,"main: password overflow\n");
      fprintf(stderr,"password is 1-64 bytes long\n");
      putstx(*argv);
      } /* password overflow */
   strcpy((char *) psw,*(argv+1));     /* save password */
   /*****************************************************/
   /* Alloocate memory for the input/output buffer      */
   /*****************************************************/
   buf = (unsigned char *) malloc(BUFSZ);
   if (buf == NULL)
      {
      fprintf(stderr,"main: out of memory "
         "allocating buf\n");
      exit(1);
      } /* out of memory */
   /*****************************************************/
   et = (etfmt *) etausinit();    /* initialize etaus RNG */
   bldtbl(tbl);      /* initialize CRC table */
   crc = zero = 0;         /* treat the CRC as a 4 byte string */
   crcp = (unsigned char *) &crc;   /* point to CRC string */
   /*****************************************************/
   /* override the state of the random number generator */
   /* with the password parameter hashed by the CRC     */
   /* feedback algorithm.                               */
   /*****************************************************/
   crc = crc32c(psw,tbl);
   et->s1 = crc;
   crc = crc32c(crcp,tbl);
   et->s2 = crc;
   crc = crc32c(crcp,tbl);
   et->s3 = crc;
   crc = crc32c(crcp,tbl);
   et->out = crc;
   crc = crc32c(crcp,tbl);
   et->prev = crc;
   crc = crc32c(crcp,tbl);
   et->pprev = crc;
   statep = (unsigned int *) et->state;
   stateq = (unsigned int *) et->state + 1024;
   while (statep < stateq)
      {
      crc = crc32c(crcp,tbl);
      *statep++ = crc;
      } /* for each state in et->state array */
   /*****************************************************/
   /* encrypt the stdin data stream                     */
   /* write encrypted data to stdout                    */
   /* loop terminated by end of input stream            */
   /*****************************************************/
   while (1)
      {
      int blklen;
      unsigned char *p,*q;
      blklen = getblk(buf);
      if (blklen == EOF) break;
      p = (unsigned char *) buf;
      q = (unsigned char *) buf + blklen;
      while (p < q)
         {
         *p = *p ^ etauspwr(et,8);
	 p++;
	 } /* for each byte in the buffer */
      putblk(buf,blklen);
      } /* for each block encrypted */
   free(buf);
   free(et->state);
   free(et);
   return(0);
   } /* main */
