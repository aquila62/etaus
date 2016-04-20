/* etausinit.c - Extended taus initialization Version 1.0.0          */
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

/* This is the initialization routine for extended taus    */
/* This initialization routine is based on date/time/ticks */
/* The caller is welcome to override the following:        */
/* the state array                                         */
/* the two previous outputs                                */
/* the current output                                      */
/* the seed for the GSL taus2 subroutine                   */
/* the three states for the taus algorithm                 */
/*                                                         */
/* The GSL version of taus2 is used to initialize etaus    */
/* with random values based on date and time               */
/* The period length of etaus is estimated to be many      */
/* magnitudes longer than regular taus.                    */
/* The speed of etaus is 1/3 faster than the GSL version   */
/* of taus2.                                               */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include "etaus.h"

/* size of the state array is 2^10 */
#define STATES 1024

etfmt *etausinit(void)
   {
   unsigned int dttk;          /* combined date, time, and #ticks */
   unsigned int *stp,*stq;     /* pointer into state array */
   time_t now;                 /* current date and time */
   clock_t clk;                /* current number of ticks */
   struct tms t;               /* structure used by times() */
   gsl_rng *r;                 /* GSL RNG structure */
   etfmt *et;                  /* etaus structure */

   /***************************************************/
   /* allocate memory for etaus structure */
   /***************************************************/
   et = (etfmt *) malloc(sizeof(etfmt));
   if (et == NULL)
      {
      fprintf(stderr,"etausinit: out of memory "
      "allocating etaus structure et\n");
      exit(1);
      } /* out of memory */
   et->states = STATES; /* save the number of etaus states */

   /***************************************************/
   /* allocate memory for etaus state array           */
   /***************************************************/
   et->state = (unsigned int *)
      malloc(sizeof(unsigned int) * STATES);
   if (et->state == NULL)
      {
      fprintf(stderr,"etausinit: out of memory "
      "allocating et->state state array\n");
      exit(1);
      } /* out of memory */

   /***************************************************/
   /* Randomize the seeds and states                  */
   /***************************************************/
   /* declare the GSL random number generator as taus2     */
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_taus2);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL taus2 random number generator      */
   /* to date,time,#ticks                                  */
   gsl_rng_set(r, dttk);

   /***************************************************/
   /* initialize the state array to random values     */
   /***************************************************/
   stp = (unsigned int *) et->state;
   stq = (unsigned int *) et->state + STATES;
   while (stp < stq)
      {
      *stp++ = gsl_rng_get(r);     /* set to random UINT */
      } /* for each member in et->state array */

   /***************************************************/
   /* initialize out, prev, and prev prev             */
   /* and taus s1,s2,s3                               */
   /* to random values                                */
   /***************************************************/
   et->pprev = gsl_rng_get(r);   /* set to random UINT */
   et->prev  = gsl_rng_get(r);   /* set to random UINT */
   et->out   = gsl_rng_get(r);   /* set to random UINT */
   et->s1    = gsl_rng_get(r);   /* set to random UINT */
   et->s2    = gsl_rng_get(r);   /* set to random UINT */
   et->s3    = gsl_rng_get(r);   /* set to random UINT */

   /***************************************************/
   /* after this subroutine you may initialize the    */
   /* state array to your own values, if you wish     */
   /* to do regression testing.                       */
   /* Use the above 12 instructions as an example of  */
   /* how to initialize the generator                 */
   /***************************************************/
   /* return the etaus structure                      */
   /***************************************************/
   return(et);
   } /* etausinit subroutine */
