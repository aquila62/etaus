/* mcsph.c - Monte Carlo Calculation of Pi/6  Version 1.0.0          */
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

/**********************************************************/
/* mcsph estimates the value of Pi/6 through a Monte      */
/* Carlo simulation.                                      */
/* Three uniform random numbers are generated 0-1         */
/* 100 million times                                      */
/* If x^2 + y^2 + z^2 < 1 then tally a counter            */
/* The ratio of the counter total / 100 million           */
/* is approximately Pi/6 to 3 or 4 decimal places         */
/* or approximately 0.5236...                             */
/**********************************************************/
/* In this source, mt19937 refers to the Mersenne Twister */
/**********************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include "etaus.h"

int main(void)
   {
   int i;                     /* loop counter                 */
   int etauselap;             /* elapsed time for etaus       */
   int mtelap;                /* elapsed time for mt19937     */
   int ranlxelap;             /* elapsed time for ranlxd2     */
   unsigned int dttk;         /* combined date, time #ticks   */
   double tot;                /* total points within a sphere */
   double bot;                /* 100 million                  */
   double ratio;              /* estimated pi/6               */
   time_t now;                /* current date and time        */
   clock_t clk;               /* current number of ticks      */
   clock_t etausstart;        /* start time for etaus         */
   clock_t etausfin;          /* end   time for etaus         */
   clock_t mtstart;           /* start time for mt19937       */
   clock_t mtfin;             /* end   time for mt19937       */
   clock_t ranlxstart;        /* start time for ranlxd2       */
   clock_t ranlxfin;          /* end   time for ranlxd2       */
   struct tms t;              /* structure used by times()    */
   gsl_rng *r;                /* GSL RNG structure            */
   etfmt *et;                 /* etaus structure              */
   et = (etfmt *) etausinit();     /* initialize the etaus structure */
   bot = 100000000.0;         /* set to 100 million           */
   /************************************************************/
   tot = 0.0;                 /* initialize total points in a sphere */
   i = (int) bot;             /* set loop counter */
   /* get clock ticks since boot                           */
   etausstart = times(&t);    /* start time for etaus      */
   while (i--)                /* loop 100 million times    */
      {
      double x;               /* horizontal value             */
      double y;               /* vertical   value             */
      double z;               /* depth      value             */
      double sum;             /* sum = x*x + y*y + z*z        */
      x = etausunif(et);      /* uniform number 0-1           */
      y = etausunif(et);      /* uniform number 0-1           */
      z = etausunif(et);      /* uniform number 0-1           */
      sum = (x*x) + (y*y) + (z*z);   /* calculate the vector length  */
      if (sum < 1.0) tot += 1.0;   /* if within a unit sphere, tally */
      } /* for each point inside or outside a sphere */
   ratio = tot / bot;         /* calculate est. pi/6   */
   etausfin = times(&t);      /* finish time for etaus */
   printf("Monte Carlo Calculation of Pi/6\n");
   printf("      n = 100 million\n");
   printf("      etaus Pi/6 %18.15f\n", ratio);
   printf("     Actual Pi/6 %18.15f\n", M_PI / 6.0);
   /************************************************************/
   /* allocate the mt19937 random number generator */
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_mt19937);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL ranlxd2 random number generator      */
   /* to date,time,#ticks                                     */
   gsl_rng_set(r, dttk);      /* initialize ranlxd2 seed      */
   tot = 0.0;                 /* initialize total points in a sphere */
   i = (int) bot;             /* set loop counter */
   /* get clock ticks since boot                              */
   mtstart = times(&t);       /* start time for GSL mt19937   */
   while (i--)                /* loop 100 million times       */
      {
      double x;               /* horizontal value             */
      double y;               /* vertical   value             */
      double z;               /* depth      value             */
      double sum;             /* sum = x*x + y*y + z*z        */
      /* use the mt19937 random number generator this time    */
      x = gsl_rng_uniform(r);      /* uniform number 0-1      */
      y = gsl_rng_uniform(r);      /* uniform number 0-1      */
      z = gsl_rng_uniform(r);      /* uniform number 0-1      */
      sum = (x*x) + (y*y) + (z*z);   /* calculate the vector length  */
      if (sum < 1.0) tot += 1.0;   /* if within a unit sphere, tally */
      } /* for each point inside or outside a sphere */
   ratio = tot / bot;         /* calculate est. pi/6           */
   mtfin = times(&t);         /* finish time for GSL mt19937   */
   printf("GSL mt19937 Pi/6 %18.15f\n", ratio);
   printf("     Actual Pi/6 %18.15f\n", M_PI / 6.0);
   etauselap = etausfin - etausstart;
   mtelap    = mtfin    - mtstart;
   printf("      etaus ticks %6d\n", etauselap);
   printf("GSL mt19937 ticks %6d\n", mtelap);
   /************************************************************/
   /* allocate the ranlxd2 random number generator */
   r = (gsl_rng *) gsl_rng_alloc(gsl_rng_ranlxd2);
   /* get clock ticks since boot                           */
   clk = times(&t);
   /* get date & time                                      */
   time(&now);
   /* combine date, time, and ticks into a single UINT     */
   dttk = (unsigned int) (now ^ clk);
   /* initialize the GSL ranlxd2 random number generator      */
   /* to date,time,#ticks                                     */
   gsl_rng_set(r, dttk);      /* initialize ranlxd2 seed      */
   tot = 0.0;                 /* initialize total points in a sphere */
   i = (int) bot;             /* set loop counter */
   /* get clock ticks since boot                              */
   ranlxstart = times(&t);    /* start time for GSL ranlxd2   */
   while (i--)                /* loop 100 million times       */
      {
      double x;               /* horizontal value             */
      double y;               /* vertical   value             */
      double z;               /* depth      value             */
      double sum;             /* sum = x*x + y*y + z*z        */
      /* use the ranlxd2 random number generator this time    */
      x = gsl_rng_uniform(r);      /* uniform number 0-1      */
      y = gsl_rng_uniform(r);      /* uniform number 0-1      */
      z = gsl_rng_uniform(r);      /* uniform number 0-1      */
      sum = (x*x) + (y*y) + (z*z);   /* calculate the vector length  */
      if (sum < 1.0) tot += 1.0;   /* if within a unit sphere, tally */
      } /* for each point inside or outside a sphere */
   ratio = tot / bot;         /* calculate est. pi/6           */
   ranlxfin = times(&t);      /* finish time for GSL ranlxd2   */
   printf("GSL ranlxd2 Pi/6 %18.15f\n", ratio);
   printf("     Actual Pi/6 %18.15f\n", M_PI / 6.0);
   etauselap = etausfin - etausstart;
   ranlxelap = ranlxfin - ranlxstart;
   printf("      etaus ticks %6d\n", etauselap);
   printf("GSL ranlxd2 ticks %6d\n", ranlxelap);
   return(0);
   } /* main */
