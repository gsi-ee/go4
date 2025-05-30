// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4MbsRandom.h"


#include "TRandom.h"
#include <cmath>

#include "TGo4Log.h"
#include "TGo4MbsRandomParameter.h"


extern "C"
{
#include "random-coll.h"
}

TGo4MbsRandom::TGo4MbsRandom() :
   TGo4MbsSource(),
   fiDLen(0),
   fiNumSub(0),
   fiNumDat(0)
{
   GO4TRACE((15,"TGo4MbsRandom::TGo4MbsRandom()",__LINE__, __FILE__));
}


TGo4MbsRandom::TGo4MbsRandom(TGo4MbsRandomParameter* par) :
   TGo4MbsSource(par, 1),
   fiDLen(0),
   fiNumSub(0),
   fiNumDat(0)
{
   GO4TRACE((15,"TGo4MbsRandom::TGo4MbsRandom(TGo4MbsRandomParameter*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsRandom %s:  ",GetName());

   Open();
}

TGo4MbsRandom::TGo4MbsRandom(const char *name) :
   TGo4MbsSource(name, 1),
   fiDLen(0),
   fiNumSub(0),
   fiNumDat(0)
{
   GO4TRACE((15,"TGo4MbsRandom::TGo4MbsRandom(const char *)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsRandom %s:  ",name);
   Open();
}


TGo4MbsRandom::~TGo4MbsRandom()
{
   GO4TRACE((15,"TGo4MbsRandom::~TGo4MbsRandom()",__LINE__, __FILE__));
   Close();
}

Int_t TGo4MbsRandom::NextEvent()
{
   GO4TRACE((12,"TGo4MbsRandom::NextEvent()",__LINE__, __FILE__));
   // here we fill mbs event structure at fxEvent ptr.
   void *clearfield = (void *) (fxEvent+1); // we skip header and clear the rest
   size_t clearlen = fiDLen*2 - 8;
   memset(clearfield, 0, clearlen); // clear old values
   fxEvent->l_count++;
   // now fill in some new values:
   fxEvent->l_dlen = 0;
   s_ves10_1* subevt = (s_ves10_1*) clearfield;
   for (Int_t i = 0; i < fiNumSub; ++i) {
      int l_val_num = (int)(get_int(1., 7.)+0.5); // random number for number of data longwords
      if(l_val_num>fiNumDat) l_val_num=fiNumDat; // never exceed allocated field
      // setup subevent header:
      subevt->i_type=10;
      subevt->i_subtype=1;
      subevt->h_subcrate=i+1; // set subevent id numbers:
      subevt->h_control=2*i;
      subevt->i_procid=4*i;
      //subevt->l_dlen=fiNumDat*2+2; // length in short units + 2
      subevt->l_dlen=l_val_num*2+2; // subevent length in short units + 2
      fxEvent->l_dlen+=(l_val_num*sizeof(Int_t)) / 2 ; // add datalength to total length in shorts

      Int_t *subdata = (Int_t *) (subevt+1); // data starts after subevt
      for (Int_t j = 0; j < l_val_num; ++j) {
         *(subdata + j) = rand_event(j + 1); // later use random generator here
      }                                        // for (... numdat ...)
      subevt=(s_ves10_1*) (subdata+l_val_num); // next subheader after last data
   }                                           // for(...numsub)
   fxEvent->l_dlen+=(sizeof(s_ve10_1)-sizeof(s_evhe)+fiNumSub*sizeof(s_ves10_1))/2;
   // finally, add length of headers  to totalevent length
   return 0;
}


Int_t TGo4MbsRandom::Open()
{
   GO4TRACE((12,"TGo4MbsRandom::Open()",__LINE__, __FILE__));
   //
   if(fbIsOpen) return -1;

   get_rand_seed();
   fiNumSub = 2; // number of subevents, fix
   fiNumDat = 16; // maximum allocated data longs per subevent
   fiDLen = (sizeof(s_ve10_1)-sizeof(s_evhe)+fiNumSub*(sizeof(s_ves10_1)+fiNumDat*sizeof(Int_t))) / 2 ;
   // fiDLen is not in char (=size_t), but short units
   fxEventMem = new Short_t[fiDLen+sizeof(s_evhe)];
   fxEvent = (s_ve10_1*) fxEventMem;
   fxEvent->l_dlen = fiDLen;
   fxEvent->i_subtype = 1;
   fxEvent->i_type = 10;
   fxEvent->i_trigger = 1;
   fxEvent->l_count = 0;

   fbIsOpen = kTRUE;

   return 0;
}


Int_t TGo4MbsRandom::Close()
{
   GO4TRACE((12,"TGo4MbsRandom::Close()",__LINE__, __FILE__));
   if(!fbIsOpen) return -1;
   delete [] fxEventMem;
   fxEventMem = nullptr;
   fxEvent = nullptr;
   fbIsOpen = kFALSE;
   return 0;
}

////////// functions for random generation:

void TGo4MbsRandom::get_rand_seed()                         /* obtains seed for generator      */
{
//  struct timeval tv;
//  struct timezone tz;

//  gettimeofday(&tv, &tz);          /* may not be supported in your C  */
//  fuSeed=tv.tv_sec + tv.tv_usec;     /* if not, replace with clock()    */
//  srand48(fuSeed);

  gRandom->SetSeed(0);
}


double TGo4MbsRandom::gauss_rnd(double mean, double sigma)
{
  static int iset = 0;
  static double gset;
  double v1, v2, s, u1;

  if (sigma < 0.) {
     v1 = gRandom->Rndm(); // drand48();
     return (log(1 - v1) / sigma + mean);
  } else {
     if (iset == 0) {
        do {
           // v1 = 2.*drand48()-1.;
           // v2 = 2.*drand48()-1.;

           v1 = 2. * gRandom->Rndm() - 1.;
           v2 = 2. * gRandom->Rndm() - 1.;

           s = v1 * v1 + v2 * v2;
        } while (s >= 1.0 || s == 0.0);

        u1 = sigma * v1 * (sqrt(-2. * log(s) / s)) + mean;
        gset = u1;
        iset = 1;

        return (sigma * v2 * (sqrt(-2. * log(s) / s)) + mean);
     } else {
        iset = 0;
        return gset;
     }
  }
}

double TGo4MbsRandom::get_int(double low, double high)
{
//   return ((high-low)*drand48()+low);
   return ((high-low)*gRandom->Rndm()+low);
}


long TGo4MbsRandom::rand_event(long choice)
{
   static double fgdPeak[]   = { 200., 302., 653., 1024., 2800.};
   static double fgdSigma[]  = {  10.,  22., 153.,  104.,   38.};
   static double fgdPeak2[]  = { 300., 402., 853., 2000., 3024.};
   static double fgdSigma2[] = {  22., 153., 104.,  100.,   20.};

   int cnt;
   switch(choice) {
      case 1:
         cnt = (int)(get_int(0., (double)NUM_PEAK));
         return ((long)(gauss_rnd(fgdPeak[cnt], fgdSigma[cnt])));
      case 2:
         cnt = (int)(get_int(0., (double)NUM_PEAK));
         return ((long)(p_dNormal(fgdPeak2[cnt], fgdSigma2[cnt], &fuSeed)));
      case 3:
         return ((long)(4096*p_dUniform(&fuSeed)));
      case 4:
         return ((long)(gauss_rnd(0., -.001)));
      case 5:
         return ((long)(p_dExponential(100., &fuSeed)));
      case 6:
         cnt = (int)(get_int(0., (double)NUM_PEAK));
         return ((long)((p_dExponential(200., &fuSeed)) + gauss_rnd(fgdPeak[cnt], fgdSigma[cnt])));
      case 7:
         cnt = (int)(get_int(3., (double)NUM_PEAK));
         return ((long)((4096*p_dUniform(&fuSeed)) + gauss_rnd(fgdPeak[cnt], fgdSigma[cnt])));
   }

   return 0;
}

s_bufhe *TGo4MbsRandom::GetBufferHeader()
{
   return nullptr;
}
