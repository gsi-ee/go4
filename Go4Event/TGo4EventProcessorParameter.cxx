#include "TGo4EventProcessorParameter.h"

#include "Riostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"

TGo4EventProcessorParameter::TGo4EventProcessorParameter(const char* name, Int_t id)
: TGo4Parameter (name), fiID(id)
{
   TRACE((12,"TGo4EventProcessorParameter::TGo4EventProcessorParameter(const char*)",__LINE__, __FILE__));
}

TGo4EventProcessorParameter::TGo4EventProcessorParameter()
: TGo4Parameter ("Default Event Processor Parameter"), fiID(0)
{
   TRACE((12,"TGo4EventProcessorParameter::TGo4EventProcessorParameter()",__LINE__, __FILE__));
}

TGo4EventProcessorParameter::~TGo4EventProcessorParameter()
{
   TRACE((12,"TGo4EventProcessorParameter::~TGo4EventProcessorParameter()",__LINE__, __FILE__));
}

Int_t TGo4EventProcessorParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4EventProcessorParameter::PrintParameter()",__LINE__, __FILE__));
    Int_t locallen=64000;
   Text_t localbuf[64000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "EventProcessor: %s \n",ClassName());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Name: \t%s \n",GetName());
//   current=TGo4Status::PrintIndent(current,restlen);
//   current=TGo4Status::PrintBuffer(current,restlen, "  ID: \t%d \n",GetID());
   if(buffer==0)
      {
          cout << localbuf << endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
            size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

Bool_t TGo4EventProcessorParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4EventProcessorParameter::UpdateFrom()",__LINE__, __FILE__));
   TGo4EventProcessorParameter* procpar=dynamic_cast<TGo4EventProcessorParameter*>(rhs);
   if(procpar==0) return kFALSE;
   SetName(procpar->GetName());
   fiID=procpar->GetID();
   return kTRUE;
}
