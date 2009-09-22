#include "TYYYRawEvent.h"

#include "Riostream.h"

#include "TGo4Log.h"

TYYYRawEvent::TYYYRawEvent() :
   TGo4EventElement()
{
   fiColumns=0;
   fdData=0; // default ctor is for streamer only, avoid heap objects here!

}

TYYYRawEvent::TYYYRawEvent(const char* name) :
   TGo4EventElement(name)
{
   fiColumns = 10;
   fdData = new Double_t[fiColumns];
}

TYYYRawEvent::~TYYYRawEvent()
{
   if (fdData) delete [] fdData;
}

void TYYYRawEvent::Clear(Option_t *t)
{
   for(Int_t t=0; t<fiColumns;++t)
      fdData[t] = 0.;
}

void TYYYRawEvent::ReAllocate(Int_t newsize)
{
   if (newsize > fiColumns) {
      // cout <<"*** YYYRawEvent reallocating from "<<fiColumns<<" to "<<newsize << endl;
      Double_t* narray=new Double_t[newsize];
      for(Int_t i=0;i<fiColumns;++i)
         narray[i] = fdData[i];
      delete [] fdData;
      fdData = narray;
      fiColumns = newsize;
   } else {
      if (newsize == 0) {
         delete [] fdData;
         fdData = 0;
      }
      fiColumns = newsize;
   }
}

void TYYYRawEvent::PrintEvent()
{
   TGo4EventElement::PrintEvent();
   cout<<" YYY Event printout: "<<endl;
   for(Int_t t=0; t<fiColumns;++t)
      cout <<"\t dat("<<t<<")="<<fdData[t]<<endl;
}
