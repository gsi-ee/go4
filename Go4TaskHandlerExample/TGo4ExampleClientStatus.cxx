#include "TGo4ExampleClientStatus.h"

#include "Riostream.h"

#include "TH1.h"

#include "TGo4Log.h"

TGo4ExampleClientStatus::TGo4ExampleClientStatus(const char* name) : TGo4ClientStatus(name)
{
  TRACE((12,"TGo4ExampleClientStatus::TGo4ExampleClientStatus(const char*)",__LINE__, __FILE__));
}

void TGo4ExampleClientStatus::SetHistoStatus(TH1* histogram)
{
   TRACE((12,"TGo4ClientStatus::SetHistoStatus(TH1*)",__LINE__, __FILE__));
   fxHistogramName = histogram->GetName();
   // here the complete internal histogram information may be stored; we leave that
   // as an exercise for the reader!
}

void TGo4ExampleClientStatus::SetNames(const char* main, const char* watch)
{
   TRACE((12,"TGo4ClientStatus::SetNames(const char*,...)",__LINE__, __FILE__));
   fxMainName = main;
   fxWatchName = watch;
}

Int_t TGo4ExampleClientStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4ExampleClientStatus::PrintStatus()",__LINE__, __FILE__));
   //
   if(buflen<=0 && buffer!=0)return 0;
   Int_t locallen=4096;
   Text_t localbuf[4096];
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;

   current=PrintBuffer(current,restlen, "G-OOOO->    ExampleClient Status Class %s Printout:  <-OOOO-G\n",GetName());
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> Main Runnable Name: %s\n",fxMainName.Data());
   current=PrintBuffer(current,restlen, "G-OOOO-> Watch Runnable Name: %s\n",fxWatchName.Data());
   current=PrintBuffer(current,restlen, "G-OOOO-> Demo Histogram Name: %s\n",fxHistogramName.Data());
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   Int_t delta=TGo4ClientStatus::PrintStatus(current,restlen); // gives us the superclass data
   restlen-=delta;
   current+=delta;
   current=PrintBuffer(current,restlen, "G-OOOO-> END Example Client Status Class Printout END <-OOOO-G\n");
   if(buffer==0)
      {
         cout << localbuf << endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1) size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

TGo4ExampleClientStatus::~TGo4ExampleClientStatus()
{
  TRACE((12,"TGo4ExampleClientStatus::~TGo4ExampleClientStatus()",__LINE__, __FILE__));
}
