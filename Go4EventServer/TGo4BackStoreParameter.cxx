#include "TGo4BackStoreParameter.h"

#include "Riostream.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4BackStoreParameter::TGo4BackStoreParameter(const char* name,
                                              Int_t splitlevel,
                                              Int_t bufsize)
: TGo4EventStoreParameter(name, GO4EV_BACK),
   fiSplit(splitlevel), fiBufsize(bufsize)
{
   TRACE((14,"TGo4FileStoreParameter::TGo4FileStoreParameter(const char*,...)", __LINE__, __FILE__));
   SetTitle(name);
}

TGo4BackStoreParameter::TGo4BackStoreParameter()
: TGo4EventStoreParameter("Default Go4 FileStore", GO4EV_BACK),
   fiSplit(99), fiBufsize(64000)
{
   TRACE((14,"TGo4FileStoreParameter::TGo4FileStoreParameter()", __LINE__, __FILE__));
   SetTitle("Go4FileStore-Tree");
}

TGo4BackStoreParameter::~TGo4BackStoreParameter()
{
   TRACE((14,"TGo4FileStoreParameter::~TGo4FileStoreParameter()", __LINE__, __FILE__));
}

Int_t TGo4BackStoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
 TRACE((12,"TGo4FileStoreParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0) return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   Int_t delta=TGo4EventStoreParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Split level: \t%d \n",fiSplit);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Buffer size: \t%d bytes \n",fiBufsize);
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

Bool_t TGo4BackStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4BackStoreParameter::UpdateFrom()",__LINE__, __FILE__));
   TGo4BackStoreParameter* backpar=dynamic_cast<TGo4BackStoreParameter*>(rhs);
   if(backpar==0) return kFALSE;
   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetSplitlevel(backpar->GetSplitlevel());
   SetBufsize(backpar->GetBufsize());
   return kTRUE;
}
