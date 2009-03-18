#include "TYYYRawEvent.h"

#include "Riostream.h"

#include "TGo4Log.h"

#include "TYYYEventSource.h"
#include "TGo4TreeSource.h"
#include "TGo4FileSource.h"

TYYYRawEvent::TYYYRawEvent()
:TGo4EventElement("YYYRawEvent"), fxUserSource(0), fxTreeSource(0), fxFileSource(0)
{
   fiColumns=0;
   fdData=0; // default ctor is for streamer only, avoid heap objects here!

}

TYYYRawEvent::TYYYRawEvent(const char* name)
:TGo4EventElement(name), fxUserSource(0), fxTreeSource(0), fxFileSource(0)
{
   fiColumns=10;
   fdData=new Double_t[fiColumns];

}

TYYYRawEvent::~TYYYRawEvent()
{

delete [] fdData;

}

Int_t TYYYRawEvent::Fill()
{
Int_t rev=0;
   if(fxUserSource)
      {
         Clear();
         // fill from our source
         rev=fxUserSource->NextEvent();
         if(rev!=0)
            {
               cout<<"YYY Raw Event --  !!! NextEvent() error:";
               cout<<fxUserSource->GetErrMess()<<endl;
               return rev;
            }
         else
            {
               fxUserSource->BuildYYYEvent(this);
               return rev;
            }
      }

   else if(fxTreeSource)
      {
         Clear();
         if(fxTreeSource->BuildEvent(this))
            {
               return 0;
            }
         else
            {
               // error, may be end of tree..
               return 1;
            }

      }
else if(fxFileSource)
      {
         Clear();
         if(fxFileSource->BuildEvent(this))
            {
               return 0;
            }
         else
            {
               // error, may be end of tree..
               return 1;
            }

      }

   else
      {

         TGo4Log::Debug(" !!! YYYEvent: Fill ERROR:  unknown event source !!! ");
         return 1;
      }

   return 0;
}

Int_t TYYYRawEvent::Init()
{
   // Check event sources here and downcast the correct one:
   // will be called once before event processing is done
   if(CheckEventSource("TYYYEventSource"))
      {
         fxUserSource = dynamic_cast<TYYYEventSource*> (GetEventSource());
         fxTreeSource=0;
         fxFileSource=0;
      }
   else if(CheckEventSource("TGo4TreeSource"))
      {
         fxTreeSource = dynamic_cast<TGo4TreeSource*> (GetEventSource());
         fxUserSource=0;
         fxFileSource=0;
      }
   else if(CheckEventSource("TGo4FileSource"))
      {
         fxFileSource = dynamic_cast<TGo4FileSource*> (GetEventSource());
         fxTreeSource=0;
         fxUserSource=0;
      }
   else
      {
         cout<<" !!! YYYEvent: Init ERROR:  unknown event source !!!"<<endl;
         return 1;
      }
   return 0;
}



void TYYYRawEvent::Clear(Option_t *t)
{
for(Int_t t=0; t<fiColumns;++t)
{
   fdData[t]=0;
}

}

void TYYYRawEvent::ReAllocate(Int_t newsize)
{
   if( newsize <= fiColumns )
      {
         // newsize is smaller, we do not reallocate
      }
   else
      {
        cout <<"*** YYYRawEvent reallocating from "<<fiColumns<<" to "<<newsize << endl;
         Double_t* narray=new Double_t[newsize];
         for(Int_t i=0;i<fiColumns;++i)
            {
             narray[i]=fdData[i];
            }
         delete [] fdData;
         fdData=narray;
         fiColumns=newsize;
      }

}

void TYYYRawEvent::PrintEvent()
{
   TGo4EventElement::PrintEvent();
   cout<<" YYY Event printout: "<<endl;
   for(Int_t t=0; t<fiColumns;++t)
   {
      cout <<"\t dat("<<t<<")="<<fdData[t]<<endl;
   }
}
