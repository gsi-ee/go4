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

#include <stdlib.h>

#define TYPE__SHORT  0
#define TYPE__INT    1
#define TYPE__FLOAT  2
#define TYPE__DOUBLE 3

#define UP 1
#define DOWN -1
#define HORI 0
#define PRINT 0

int go4fit_position(int l_len,double *pa_data,double *pr_pos,double *pr_sig, double *pr_sum);

void go4fit_find_peaks(
        void   *pfData,     // pointer to data
        int     lType,      // data type: 0=w 1=l 2=f 3=d
        int     lFirstChan, // first channel
        int     lPoints,    // number of points
        int     lAver,      // channels to sum up
        double  dDeltaFact, // noise factor
        double  dDeltaMin,  // noise minimum
        double *dNoise,     // noise array
        int     lPeaksMax,  // Maximum number of peaks
        int    *plPeaks,    // returns number of peaks found
        int    *plMinima,   // pointer to list of minima
        double *pdMinima,   // pointer to list of minima values
        int    *plMean,   // pointer to list of maxima
        int    *plWidth,    // pointer to list of width
        double *pdIntegral, // pointer to list of integrals
        int    *plLeft,     // pointer to array of left RMS index
        int    *plMaximum,     // pointer to array of maximum index
        int    *plRight,     // pointer to array of right RMS index
        float  *pfLowNoise,   // pointer to data array of low noise
        float  *pfHighNoise)  // pointer to data array of high noise
{
  short *pw;
  int lMaxIndex,lMinIndex,lIndexMax,lDirection,lIndex,lLastUp,lLastDown,ii,i,lMax,lLeft,lRight,
    *plMinimaL,*plMeanL,*plMinimaR,*plMeanR,lPrint,*pl;
  float *pf;
  double dValue,dLow,dHigh,dDelta,dAver,*pdData,*pdNoise,dMax,dMin,dPos,dSig,dSum,*pd;

  *plPeaks=0;
  pdNoise=NULL;
  pdData = (double *)malloc(lPoints*8);
  pdNoise = (double *)malloc(lPoints*8);
  memset(pdData,0,lPoints*8);
  memset(pdNoise,0,lPoints*8);
  lPrint=PRINT;
  dMax=0.;
  dMin=1e20;
  pw = ((short  *)pfData) + lFirstChan;
  pl = ((int    *)pfData) + lFirstChan;
  pf = ((float  *)pfData) + lFirstChan;
  pd = ((double *)pfData) + lFirstChan;
  if(lAver == 0)lAver=1;
  lPoints=lPoints/lAver;
  dAver = (double) lAver;
  // copy the data field into local double field
  switch(lType)
    {
    case TYPE__SHORT:  for(ii=0;ii<lPoints;ii++)
      {  *(pdData+ii) = (double)*(pw+lAver*ii);
      for(i=1;i<lAver;i++)*(pdData+ii) += (double)*(pw+lAver*ii+i);
      *(pdData+ii) = *(pdData+ii)/dAver;
      if(*(pdData+ii) > dMax) dMax=*(pdData+ii);
      if(*(pdData+ii) < dMin) dMin=*(pdData+ii);} break;
    case TYPE__INT:    for(ii=0;ii<lPoints;ii++)
      {  *(pdData+ii) = (double)*(pl+lAver*ii);
      for(i=1;i<lAver;i++)*(pdData+ii) += (double)*(pl+lAver*ii+i);
      *(pdData+ii) = *(pdData+ii)/dAver;
      if(*(pdData+ii) > dMax) dMax=*(pdData+ii);
      if(*(pdData+ii) < dMin) dMin=*(pdData+ii);} break;
    case TYPE__FLOAT:  for(ii=0;ii<lPoints;ii++)
      {  *(pdData+ii) = (double)*(pf+lAver*ii);
      for(i=1;i<lAver;i++)*(pdData+ii) += (double)*(pf+lAver*ii+i);
      *(pdData+ii) = *(pdData+ii)/dAver;
      if(*(pdData+ii) > dMax) dMax=*(pdData+ii);
      if(*(pdData+ii) < dMin) dMin=*(pdData+ii);} break;
    case TYPE__DOUBLE: for(ii=0;ii<lPoints;ii++)
      {  *(pdData+ii) = (double)*(pd+lAver*ii);
      for(i=1;i<lAver;i++)*(pdData+ii) += (double)*(pd+lAver*ii+i);
      *(pdData+ii) = *(pdData+ii)/dAver;
      if(*(pdData+ii) > dMax) dMax=*(pdData+ii);
      if(*(pdData+ii) < dMin) dMin=*(pdData+ii);} break;
    default: printf("Data type %d not supported!\n",lType);
      printf("Type 0: short, 1: int, 2: float, 3: double\n");
      free(pdData);
      free(pdNoise);
      return;
    }
  if(dNoise == NULL)
    {// calculate from square root
      for(ii=0;ii<lPoints;ii++)
   {
     if(*(pdData+ii) == 0.0) *(pdNoise+ii) = dDeltaFact;
     else                    *(pdNoise+ii) = sqrt(*(pdData+ii)) * dDeltaFact;
     if(dDeltaMin > *(pdNoise+ii)) *(pdNoise+ii)=dDeltaMin;
   }
    }else {// sum up bins from noise input array
   for(ii=0;ii<lPoints;ii++)
     {  *(pdNoise+ii) = *(dNoise+lFirstChan+lAver*ii);
        for(i=1;i<lAver;i++)*(pdNoise+ii) += *(dNoise+lFirstChan+lAver*ii+i);
        *(pdNoise+ii) = *(pdNoise+ii)/dAver;
     }
      }
  lMaxIndex=0;
  lMinIndex=0;
  lIndexMax=lPoints-1;
  plMinimaL = (int *)malloc(lPeaksMax*16);
  plMeanL = (int *)(plMinimaL+lPeaksMax);
  plMinimaR = (int *)(plMinimaL+2*lPeaksMax);
  plMeanR = (int *)(plMinimaL+3*lPeaksMax);
  memset(plMinimaL,0,lPeaksMax*16);

  dDelta= *pdNoise;
  dHigh = *pdData + dDelta;
  dLow  = *pdData - dDelta;
  if(pfLowNoise)*pfLowNoise=dLow;
  if(pfHighNoise)*pfHighNoise=dHigh;
  //-----------------------------------------------------
  // loop: get points of minima and maxima
  lDirection=HORI;
  lIndex=0;
  lLastDown=0;
  lLastUp=0;
  while ((lIndex <= lIndexMax) & (lMaxIndex < lPeaksMax-2))
    {
      dValue = *(pdData+lIndex);
      if(dValue > dHigh)
   {// direction goes upstairs
     if (lDirection != UP)
       { // direction was downstairs: minimum found
         if(lPrint)printf("%5d - %5d - %5d d %8.1f\n",
                lMinIndex,
                lLastDown,
                lIndex,
                dDelta);
         *(plMinimaR+lMinIndex)=lIndex;
         *(plMinimaL+lMinIndex)=lLastDown;
         lMinIndex++;
       }
     lLastUp=lIndex;
     lDirection=UP;// direction goes upstairs
   }
      if(dValue < dLow)
   {// direction goes downstairs
     if (lDirection == UP)
       { // direction was upstairs: maximum found
         if(lPrint)printf("%5d + %5d - %5d d %8.1f\n",
                lMaxIndex,
                lLastUp,
                lIndex,
                dDelta);
         *(plMeanR+lMaxIndex)=lIndex;
         *(plMeanL+lMaxIndex)=lLastUp;
         lMaxIndex++;
       }
     lLastDown=lIndex;
     lDirection=DOWN;// direction goes downstairs
   }
      // calculate new limits if noise band has been left
      if((dValue < dLow)|(dValue > dHigh))
   {
          dDelta= *(pdNoise+lIndex);
     dHigh = dValue + dDelta;
     dLow  = dValue - dDelta;
   }
      lIndex++;
      if(pfLowNoise)*(pfLowNoise+lIndex)=dLow;
      if(pfHighNoise)*(pfHighNoise+lIndex)=dHigh;
    }
  if(lDirection == DOWN)
    {
      *(plMinimaL+lMinIndex)=lLastDown;
      *(plMinimaR+lMinIndex)=lIndexMax;
      if(lPrint)printf("%5d - %5d - %5d\n",
             lMinIndex,
             lLastDown,
             lIndexMax);
      lMinIndex++;
    }
  //-----------------------------------------------------
  if(lPrint)printf("-------------------------\n");
  if(lPrint)printf("Minima %d Maxima %d  \n",lMinIndex,lMaxIndex);
  // for each peak there must be a minimum left and right
  if(lMinIndex != (lMaxIndex+1))
    {
      free(plMinimaL);
      free(pdData);
      free(pdNoise);
      *plPeaks=0;
      if(lPrint)printf("Error, wrong minima\n");
      return;
    }
  // calculate mean values of minima L&R, mean content, peaks
  // start with first minimum
  if(lPrint)printf("  [ Left   Right ]  \n");
  *plMinima = (*plMinimaL + *plMinimaR)/2;
  *pdMinima=0.; // average content at mean minimum channel
  for(ii = *plMinimaL;ii <= *plMinimaR;ii++) *pdMinima += *(pdData+ii);
  *pdMinima = *pdMinima/(double)(*plMinimaR - *plMinimaL + 1);
  if(lPrint)printf("- [%6d %6d] %6d %6.1f\n",
         *plMinimaL,
         *plMinimaR,
         *plMinima,
         *pdMinima);
  for(lIndex=0;lIndex < lMaxIndex;lIndex++)
    {
      // simple mean peak value, overwritten later
      *(plMean+lIndex)=(*(plMeanL+lIndex)+*(plMeanR+lIndex))/2;
      if(lPrint)printf("+ [%6d %6d] %6d\n",
             *(plMeanL+lIndex),
             *(plMeanR+lIndex),
             *(plMean+lIndex));
      // mean minima values (channel and content)
      *(plMinima+lIndex+1) = (*(plMinimaL+lIndex+1) + *(plMinimaR+lIndex+1))/2;
      *(pdMinima+lIndex+1)=0.;
      for(ii = *(plMinimaL+lIndex+1);ii <= *(plMinimaR+lIndex+1);ii++)
   *(pdMinima+lIndex+1) += *(pdData+ii);
      *(pdMinima+lIndex+1) = *(pdMinima+lIndex+1)/(double)(*(plMinimaR+lIndex+1) - *(plMinimaL+lIndex+1)+1);
      if(lPrint)printf("- [%6d %6d] %6d %6.1f\n",
             *(plMinimaL+lIndex+1),
             *(plMinimaR+lIndex+1),
             *(plMinima+lIndex+1),
             *(pdMinima+lIndex+1));
      // Peak positions, width and sum by momenta
      ii = *(plMinimaL+lIndex+1) - *(plMinimaR+lIndex) + 1;
      if(ii > 1)
   {
     go4fit_position(ii,pdData + *(plMinimaR+lIndex),&dPos,&dSig,&dSum);

     *(plMean+*plPeaks)=(int)((dPos+(double)(*(plMinimaR+lIndex)))*dAver)+lFirstChan;
     *(plWidth+*plPeaks) =(int)(dSig*dAver + 0.5);
     if(*(pdMinima+lIndex) < *(pdMinima+lIndex+1))
       dSum=dSum-((double)ii * *(pdMinima+lIndex)+ (double)ii * (*(pdMinima+lIndex+1)-*(pdMinima+lIndex))/2.);
          else
       dSum=dSum-((double)ii * *(pdMinima+lIndex)- (double)ii * (*(pdMinima+lIndex)-*(pdMinima+lIndex+1))/2.);
     *(pdIntegral+*plPeaks) = dSum*dAver;
          (*plPeaks)++;
   }
      else
   {
     free(plMinimaL);
     free(pdData);
      free(pdNoise);
      *plPeaks=0;
     if(lPrint)printf("Error, negative interval\n");
     return;
   }
    }
  for(i=0;i<*plPeaks;i++)
    {
      dMax=0; // search local maximum in interval
      dMin=1000000000; // local minimum
      pd=pdData+*(plMinima+i);
      for(ii=*(plMinima+i);ii<*(plMinima+i+1);ii++){if(dMax < *pd)dMax = *pd; if(dMin > *pd)dMin = *pd; pd++;}
      dMin=dMin+(dMax-dMin)/2.; // threashold for RMS
      pd=pdData+*(plMinima+i); // search channel of local maximum
      for(lMax=*(plMinima+i);lMax<*(plMinima+i+1);lMax++){if(dMax == *pd)break; pd++;} // l_max is now channel of local maximum
      lLeft=*(plMinima+i);
      lRight=*(plMinima+i+1);
      *(plMaximum+i)=lMax;
      pd=pdData+lMax;
      for(ii=lMax;ii<lRight;ii++){if(dMin > *pd){lRight=ii;break;}pd++;}
      pd=pdData+lMax;
      for(ii=lMax;ii>lLeft;ii--){if(dMin > *pd){lLeft=ii;break;}pd--;}
      *(plLeft+i)=lLeft;
      *(plRight+i)=lRight;
    }
  *(plMinima) = (int)((double)(*(plMinima))*dAver + dAver/2.)+lFirstChan;
  for(i=0;i<*plPeaks;i++)
    {
      // calculate channel numbers of original data
      *(plMaximum+i)  = (int)((double)(*(plMaximum+i))*dAver + dAver/2.)+lFirstChan;
      *(plLeft+i)     = (int)((double)(*(plLeft+i))*dAver + dAver/2.)+lFirstChan;
      *(plRight+i)    = (int)((double)(*(plRight+i))*dAver + dAver/2.)+lFirstChan;
      *(plMinima+i+1) = (int)((double)(*(plMinima+i+1))*dAver + dAver/2.)+lFirstChan;
    }
  if(lPrint)printf("peaks found %d\n",*plPeaks);

  free(plMinimaL);
  free(pdData);
  free(pdNoise);
}
//**********************************************************************************
int go4fit_position(int l_len,double *pa_data,double *pr_pos,double *pr_sig, double *pr_sum)
{

#define LOW  2
#define WIDTH  8

  double r_sig_f     ;
//SL  int I,J,K,L     ;
  int J;
  double d_sum_prod  ;
//SL  double l_max_chan  ;
  double *pl_data,d_max    ;

  /* 2.0E2*SQRT(2.0E0*LOG(2.0E0))/100. */;
  r_sig_f      = 55.4518;
  r_sig_f      = 2.;
  *pr_sum   = 0;
  d_sum_prod   = 0;
  *pr_sig      = 0.;
  *pr_pos      = 0.;
  d_max=0.0;

  /* get maximum channel content and integral */
  /*  pl_data = pa_data;
      for(J = 0; J < l_len; J++)
      {
      if(d_max < *pl_data)
      {
      d_max = *pl_data;
      l_max_chan = J;
      }
      pl_data++;
      }
  */
  /* Calculate first momentum */
  pl_data = pa_data;
  for(J = 1; J <= l_len; J++)
    {
      *pr_sum = *pr_sum +     *pl_data;
      d_sum_prod = d_sum_prod + J * *pl_data;
      pl_data++;
    }
  /* Calculate second momentum */
  if(*pr_sum > 0)
    {
      *pr_pos = (d_sum_prod/ *pr_sum + 0.5);
      pl_data = pa_data;
      for(J = 1; J <= l_len; J++)
   {
     *pr_sig = *pr_sig + ((double)J - *pr_pos) * ((double)J - *pr_pos) * *pl_data;
     pl_data++;
   }
      *pr_sig = r_sig_f * sqrt(*pr_sig/ *pr_sum);
    }
  else return(1);
  *pr_pos = *pr_pos -1.0;
  return(0);
}
/** END f_position C Procedure ******************************************/
