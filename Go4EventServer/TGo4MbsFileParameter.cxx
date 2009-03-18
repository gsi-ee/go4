#include "TGo4MbsFileParameter.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

const char* TGo4MbsFile__fgcNOTAGFILE = "GO4-NOLMDTAG";

TGo4MbsFileParameter::TGo4MbsFileParameter() :
    TGo4EventSourceParameter("default MBS File", GO4EV_MBS_FILE),
    fuStartEvent(0),
    fuStopEvent(0),
    fuEventInterval(0),
    fxTagFile()
{
   TRACE((14,"TGo4MbsFileParameter::TGo4MbsFileParameter()", __LINE__, __FILE__));
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::TGo4MbsFileParameter(const char* name) :
   TGo4EventSourceParameter(name, GO4EV_MBS_FILE),
   fuStartEvent(0),
   fuStopEvent(0),
   fuEventInterval(0),
   fxTagFile()
{
   TRACE((14,"TGo4MbsFileParameter::TGo4MbsFileParameter(const char*,...)", __LINE__, __FILE__));
   SetTagName(TGo4MbsFile__fgcNOTAGFILE);
}

TGo4MbsFileParameter::~TGo4MbsFileParameter()
{
   TRACE((14,"TGo4MbsFileParameter::~TGo4MbsFileParameter()", __LINE__, __FILE__));
}

Int_t TGo4MbsFileParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4MbsFileParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;

   Int_t delta=TGo4EventSourceParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+= delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Startevent: \t%d",GetStartEvent());
   current=TGo4Status::PrintBuffer(current,restlen, "\t Stopevent: \t%d \n",GetStopEvent());
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Event interval: \t%d",GetEventInterval());
   current=TGo4Status::PrintBuffer(current,restlen, "\t Tagfile: %s \n",GetTagName());
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

Bool_t TGo4MbsFileParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4MbsFileParameter::UpdateFrom()",__LINE__, __FILE__));
   if((rhs!=0) && rhs->InheritsFrom(TGo4MbsFileParameter::Class()))
      {
         TGo4MbsFileParameter* mbspar=dynamic_cast<TGo4MbsFileParameter*>(rhs);
         if(!mbspar) return kFALSE;
         if(!TGo4EventSourceParameter::UpdateFrom(rhs)) return kFALSE;
         SetStartEvent(mbspar->GetStartEvent());
         SetStopEvent(mbspar->GetStopEvent());
         SetEventInterval(mbspar->GetEventInterval());
         SetTagName(mbspar->GetTagName());
         return kTRUE;
      }
   else
      {
          return kFALSE;
      }
   return kFALSE;
}
