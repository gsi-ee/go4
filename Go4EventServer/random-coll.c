// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/*----------------------------------------------------------------------
   DISCLAIMER

   This code is provided ``as is'' without express or implied
   warranty. The authors do not and cannot warrant the performance
   of the code provided, or the results that may be obtained by
   its use or its fitness for any specific use. In no event shall
   the authors be liable for any loss or damages that arise
   from the use of this code.

   BUGS:
   Report to: moudgill@cs.cornell.edu

----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
   This file contains the C implementation of various random number
   generators as described in
   Seminumerical Algorithms, Vol. 2, The Art of Computer Programming,
   by D.E. Knuth.

   Included are:
      Beta
      Bionomial
      Exponential
      Gamma
      Geometric
      Normal
      Uniform

   This was implemented by Mayan Moudgill, Tushar Deepak Chandra
   and Sandeep Jain in 1984 as part of the project requirement
   for the degree of Bachelor of Technology in Computer Science
   and Engineering at the Indian Institute of Technology, Kanpur.

----------------------------------------------------------------------*/

/*----------------------------------------------------------------------

   This file ccntains the beta random number generator.
   The algorithm used depends on the size of the inputs;
   for small a and b, we use the method of M.D. Johnk
   (Metrica 8, 1964, pg 5-15), in which we repeatedly
   take two uniformly distributed numbers till the sum of
   their 1/a and 1/b powers is less than 1. Then we return
   one number divided by their sum.
   Otherwise, we generate two gamma distributed numbers,
   with orders a and b and return one number divided by their
   sum.

   Reference: The Art of Computer Programming, D.E. Knuth 2nd ed., 3.4.1e

----------------------------------------------------------------------*/

/*----------------------------------------------------------------------

   p_dBeta() returns beta distributed random numbers
   with mean parameters a and b.

   Input:
      a,b   : positive doubles.
      seed  : a pointer to an unsigned integer. This is used to
         store previous uniform variate calculated for
         calculation of next uniform variate.
         It must not be changed by calling program.

   Output: an beta distributed double, with parameters a and b.

----------------------------------------------------------------------*/

extern double p_dUniform ();
extern double pow ();
extern double p_dGammaGen ();
#define BETA_TURNING_POINT 2

double   p_dBeta (a,b,seed)
double   a,b;
unsigned int   *seed;
{
   if (a*b < BETA_TURNING_POINT)
   {
      double   u1,u2,p1,p2,x;

      p1 = 1/a;
      p2 = 1/b;
      do
      {
         u1 = pow (p_dUniform(seed),p1);
         u2 = pow (p_dUniform(seed),p2);
      } while ((x=u1+u2) > 1);
      return (u1/x);
   }
   else
   {
      double   x,y;

      x = p_dGammaGen (a,seed);
      y = p_dGammaGen (b,seed);
      return (x/(x+y));
   }
}

/*------------------------------------------------------------------------------

   This file contains the binomial random number generator.
   Unfortunately, here I actually have to simulate it for want of
   a better method. Thus if a binomial is required with
   mean .3 and n=50,fifty uniform variables are required!

   Reference: The Art of Computer Programming, D.E. Knuth 2nd ed., 3.4.1f

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   p_iBionamial() returns bionomially distributed random numbers
   with n trials of events with a probability p.

   Input:
      n     : the numer of trials
      p     : the probability of success in each trial
      seed  : a pointer to an unsigned integer. This is used to
         store previous uniform variate calculated for
         calculation of next uniform variate.
         It must not be changed by calling program.

   Output: a bionomially distributed integer, with n trials of
      events with probability p.

------------------------------------------------------------------------------*/

extern double   p_dUniform ();

int   p_iBinomial (p,n,seed)
unsigned int   *seed;
int   n;
double   p;
{
   int   i,ans=0;

   for (i=0;i<n;i++)
      if (p_dUniform (seed) <= p)
         ans++;

   return (ans);
}
/*------------------------------------------------------------------------------

   This file contains exponential random number function.The method
   is to get a uniform random number U in (0,1] and return -ln(U).
   Care is taken to aviod U=0;

   Reference: The Art of Computer Programming, D.E. Knuth 2nd ed., 3.4.1d

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   p_dExponential() returns exponentially distributed random numbers
   with mean lambda.

   Input:
      lambda: the mean.
      seed  : a pointer to an unsigned integer. This is used to
         store previous uniform variate calculated for
         calculation of next uniform variate.
         It must not be changed by calling program.

   Output: an exponentially distributed double, with mean lambda

------------------------------------------------------------------------------*/

extern double p_dUniform();
extern double log();

double   p_dExponential (lambda,seed)
double   lambda;
unsigned int   *seed;
{
   double   u= p_dUniform( seed);

   while (u == (double)0)
      u= p_dUniform( seed);

   return (-lambda*log(u));
}
/*----------------------------------------------------------------------

   The standard Gamma distribution has been implemented using
   two functions:
      <1> For integer orders
      <2> For order in range (0..1)

   A gamma distribution of order X+Y is the same as that of the
   sum of two independent gamma distributions of orders X and Y
   respectivley. Hence, for general orders, we split the order
   into it's integer and fractional parts, calculate their gamma
   variates, and sum the variates obtained.

   Reference: The Art of Computer Programming, D.E. Knuth 2nd ed., 3.4.1e
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------

   There are 2 methods for Gamma with integer A. The fisrt is to
   get the sum of A independent exponential variates, is quick,
   but grows linearly with A. The second method gets a closed
   form and is more or less independent of A in speed.  Therefore
   for A>GAMMA_TURNING_POINT we should switch methods.

   GAMMA_TURNING_POINT is 7 for a Sun SS-10.

   Reference: D.E.D.E. Knuth,The art of computer programming,Vol 2
      Chapter 3.4.1E algo A (p.129)

----------------------------------------------------------------------*/

#define   GAMMA_TURNING_POINT   7
extern double   p_dUniform ();
extern double   log ();
extern double   exp ();
extern double   sqrt ();
extern double   tan ();
extern double   pow();
extern double   floor();

double   p_dGammaInt (a,seed)
int   a;
unsigned int   *seed;
{
   if (a>GAMMA_TURNING_POINT)
   {
      double   x,y,v,sqrt2a1,asub1,y2;

      sqrt2a1 = sqrt ((double)(2*a - 1));
      asub1 = a-1;
      while (1)
      {
         y = tan (3.1415926536*p_dUniform (seed));
         y2 = y*sqrt2a1;
         if ((x = y2 + asub1) <= 0)
            continue;
         v = p_dUniform (seed);
         if (v <= (1+y*y)*exp((asub1)*log(x/asub1)-y2))
            return (x);
      }
   }
   else
   {
      double ans=1;
      while (a--)
         ans *= p_dUniform (seed);
      return (-log(ans));
   }
}

/*----------------------------------------------------------------------

   GammaGen() takes any order A>0, and generates a gamma variate
   as described above.

   The function incorporated in GammaGen() for generating gamma
   variates in range (0..1) was given by Fishman. It is taken
   from:
   W.J.Kennedy,Jr & James E. Gentle, Statistical Computing, pg214

----------------------------------------------------------------------*/

double p_dGammaGen (A,seed)
double A;
unsigned *seed;
{
if (A <= 5)
   {
   double b,p,inva,res=0;
   double EXP= 2.71828182;
   double a,f;
   int g;

   f= floor( A);
   a= A-f;
   g= (int) f;

   if( a!=0)
   {
   inva= 1/a;
   b= 1.0 + a/EXP;

   while(1)
   {
   p= b*p_dUniform(seed);
   if( p<=1.0 )
   {
   res= pow(p, inva);
   if( p_dUniform(seed) <= exp( -res)) break;
   }
   else
   {
   res= - log( (b-p)/a);
   if( p_dUniform(seed) <= 1.0/pow(res, 1-a)) break;
   }
   }
   }

   if ( g>0)
   res += p_dGammaInt(g,seed);

   return(res );
   }
else /*Use the same function as integer*/
{
   double   x,y,v,sqrt2a1,asub1,y2;

   sqrt2a1 = sqrt (2*A - 1);
   asub1 = A-1;
   while (1)
   {
      y = tan (3.1415926536*p_dUniform (seed));
      y2 = y*sqrt2a1;
      if ((x = y2 + asub1) <= 0)
         continue;
      v = p_dUniform (seed);
      if (v <= (1+y*y)*exp((asub1)*log(x/asub1)-y2))
         return (x);
   }
}
}
/*------------------------------------------------------------------------------

   This file contains functions to generate geometric random numbers.
      P(1) = p
      P(2) = p(1-p)
      P(i) = p(1-p)^(i-1)
   Keep generating a uniform random number till it is lesser than p.
      OR
   Take ceil (log (U)/log (p)).Though this is slower,we will select this
   if p < .20 because as p falls,the iterative algo tends to slow
down and
   they are equally fast at p = GEOMETRIC_TURNING_POINT.
   Note that when p is large,the first method is nearly twice as
fast as
   the second but when it can be made arbitarily slow by reducing p.

   GEOMETRIC_TURNING_POINT is 0.38 for a Sun SS-10.

   ERROR HANDLING: if p>1 or p<=0 then we have an error

------------------------------------------------------------------------------*/

extern double p_dUniform();
extern double log();
extern double ceil();
#define   GEOMETRIC_TURNING_POINT  0.38

int   p_iGeometric (p,seed)
double   p;
unsigned int   *seed;
{
   int   i=1;

   if (p < GEOMETRIC_TURNING_POINT)
      return ((int) ceil(log (p_dUniform(seed))/log (1-p)));

   while (p_dUniform(seed) > p)
      i++;
   return (i);
}

/*------------------------------------------------------------------------------

   This file contains functions to generate normally distributed
   variables. It uses the polar method developed by G.E.P. Box, M.E.
   Muller and G. Marsaglia.

   Reference: D.E.D.E. Knuth, The Art of Computer Programming Ed. 2, 3.4.1C

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   p_dNormal() returns a normally distributed double variable with
mean
   mean, standard deviation  sigma.


------------------------------------------------------------------------------*/

extern double p_dUniform();
extern double log();
extern double sqrt();

double p_dNormal (mean,sigma,seed)
double   mean,sigma;
unsigned int   *seed;
{

double u1, u2, v1, v2, s;
static char flag= 0;
static double x1, x2;

if( flag )
{
flag=0;
return(x2*sigma + mean);
}

do {
u1= p_dUniform(seed);
u2= p_dUniform(seed);

v1= 2* u1 -1;
v2= 2* u2 -1;
s= v1*v1 + v2*v2;
} while( s>=1);

s= sqrt( -2*log(s)/s);
x1= v1* s;
x2= v2* s;

flag= 1;
return( x1*sigma + mean);

}
extern double exp();
extern double p_dUniform();

int p_iPoisson(mu,seed)
double mu;
unsigned *seed;
{
double total=1, till;
int count= -1   ;

till= exp( -mu);
while( total > till)
{
total *= p_dUniform(seed);
count++;
}

return(count);
}

/*------------------------------------------------------------------------------

   This file contains functions for the generation of uniformly
   distributed random numbbers. The linear congruential method is
   used. Function used is

      X[n+1]= ( a * X[n] + c ) mod m

   with
      a= 1664525 ,
      c= 1 ,
      m= 2^32 .

   Reference : D.E. Knuth, The Art of Computer Programming, Chapter 3.

------------------------------------------------------------------------------*/


#define      _Mult   1664525
#define      _Cons   1
#define    _Mask   0xFFFF
#define      _Lo(X)   (X&_Mask)      /* the 16 LSB of X */
#define      _Hi(X)   ((X>>16)&_Mask)      /* the 16 MSB of X (if 32 bit)*/


/*------------------------------------------------------------------------------

   p_iUniform() and p_dUniform generate uniformly distributed random
   numbers, the first returning an unsigned integer in range [0,2^33-1],
   and the other a double in range [0.0, 1.0)

   Input: the pointer to an unsigned number; this is X[n]. This
      location is used to store the value of the old seed,
      hence should not be changed by calling routine.

   Output: a random number, X[n+1] calculated from Input using given
      formula, converted into correct type (unsigned or double).


------------------------------------------------------------------------------*/

unsigned p_iUniform ( seed)
unsigned *seed;
{
unsigned lo, hi;

lo= _Lo(_Lo(*seed) * _Lo(_Mult) + _Cons);
hi= _Lo(_Hi(*seed) * _Lo(_Mult))+_Lo(_Hi(_Mult) * _Lo(*seed))+
    _Hi(_Lo(*seed) * _Lo(_Mult) + _Cons);

*seed= (hi<<16 | lo);
return( *seed );
}


static unsigned   Scal=0xFFFFFFFF;
double p_dUniform( seed)
unsigned * seed;
{
unsigned lo, hi;

lo= _Lo(_Lo(*seed) * _Lo(_Mult) + _Cons);
hi= _Lo(_Hi(*seed) * _Lo(_Mult))+_Lo(_Hi(_Mult) * _Lo(*seed))+
    _Hi(_Lo(*seed) * _Lo(_Mult) + _Cons);

*seed= (hi<<16 | lo);
return( ((double)*seed)/Scal );
}
/*

--
--------------------------------------------------------------------------
Mark Feldman                               \\_==_o    Skydivers do
E-mail : u914097@student.canberra.edu.au        \\    it at 120mph....
--------------------------------------------------------------------------
*/

