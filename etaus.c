/* etaus.c - random number generator Version 1.0.0                    */
/* Copyright (C) 2016 aquila62 at github.com                         */

/* This program is free software; you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License as    */
/* published by the Free Software Foundation; either version 2 of    */
/* the License, or (at your option) any later version.               */

/* This program is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of    */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/* GNU General Public License for more details.                      */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to:                        */

   /* Free Software Foundation, Inc.                                 */
   /* 59 Temple Place - Suite 330                                    */
   /* Boston, MA 02111-1307, USA.                                    */

/********************************************************/
/* Thanks to Dr. Robert G. Brown at Duke University     */
/* for many suggestions leading to this RNG.            */
/********************************************************/

/********************************************************/
/* etaus: "extended taus"                               */
/* This subroutine produces a random unsigned           */
/* 32-bit integer using the taus algorithm plus a       */
/* Bays-Durham shuffle.  The previous two cycles are    */
/* xor'd together with the current cycle.               */
/* The speed of etaus is 1/3 faster than the GSL        */
/* subroutine gsl_rng_get(r).                           */
/********************************************************/

#include "etaus.h"

unsigned int etaus(etfmt *et)
   {
   unsigned int *p;           /* state array pointer */
   unsigned int tmp;          /* used for Bays-Durham shuffle */
   /**********************************************************/
   /* The offset into the state array is not related         */
   /* to the current state.                                  */
   /**********************************************************/
   /* calculate a 10-bit offset into the state array */
   et->ofst  = et->pprev >> 22;   /* offset into state array */
   et->pprev = et->prev;   /* prev prev <== prev  */
   et->prev  = et->out;    /* prev <== current  */
   /* calculate new current state */
   /* using the taus algorithm */
   et->s1 = (((et->s1&0xfffffffe)<<12)
      ^(((et->s1<<13)^et->s1)>>19));
   et->s2 = (((et->s2&0xfffffff8)<< 4)
      ^(((et->s2<< 2)^et->s2)>>25));
   et->s3 = (((et->s3&0xfffffff0)<<17)
      ^(((et->s3<< 3)^et->s3)>>11));
   /* XOR the past two results with the current result */
   et->out   = et->pprev ^ et->prev ^ et->s1 ^ et->s2 ^ et->s3;
   /********************************************************/
   /* Bays-Durham shuffle of state array                   */
   /* 1024! = 5.41e+2639 base 10 rounded down              */
   /* The period length of the state array is theoretical  */
   /* and cannot be proven with empirical testing.         */
   /********************************************************/
   /* point to a state array element */
   p       = (unsigned int *) et->state + et->ofst;
   /* swap the current output with the member of the state array */
   tmp     = *p;
   *p      = et->out;
   et->out = tmp;
   /********************************************************/
   /* return the output of the etaus cycle after the swap  */
   return(et->out);
   } /* etaus subroutine */
