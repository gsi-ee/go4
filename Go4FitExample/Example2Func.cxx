#include <math.h>

extern "C" double gaussian(double *axis, double *pars)
{
   double pos = pars[0];
   double width = pars[1];
   double x = axis[0];
   return exp(-0.5*(x-pos)*(x-pos)/width/width);
}


