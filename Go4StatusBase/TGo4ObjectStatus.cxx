#include "TGo4ObjectStatus.h"

#include "Riostream.h"

#include "TDatime.h"
#include "TClass.h"

#include "TGo4Log.h"

TGo4ObjectStatus::TGo4ObjectStatus() :
   TGo4Status(),
   fiObjectSize(0),
   fbResetProtect(kFALSE),
   fbDeleteProtect(kTRUE),
   fxObjectClass(),
   fxStatusTime()
{
}

TGo4ObjectStatus::TGo4ObjectStatus(TObject* object, Bool_t withtime) :
   TGo4Status("Initname","InitTitle"),
   fiObjectSize(0),
   fbResetProtect(kFALSE),
   fbDeleteProtect(kTRUE),
   fxObjectClass(),
   fxStatusTime()
{
   TRACE((15,"TGo4ObjectStatus::TGo4ObjectStatus(TNamed*)",__LINE__, __FILE__));
   if(object) {
      SetName(object->GetName());
      SetTitle(object->GetTitle());
      fxObjectClass=object->ClassName();
      fiObjectSize=object->IsA()->Size();
      fbDeleteProtect=   !object->TestBit(TGo4Status::kGo4CanDelete);
      fbResetProtect =   object->TestBit(TGo4Status::kGo4NoReset);
   } else {
      SetName("Unknown Name");
      SetTitle("Unknown Title");
      fxObjectClass="Unknown Class";
   }

   if(withtime) {
       TDatime now;
       fxStatusTime=now.AsSQLString();
   } else {
      //fxStatusTime="creation time unknown";
   }
// we might suppress time information for substatus objects
// for performance reason
}

TGo4ObjectStatus::~TGo4ObjectStatus()
{
   TRACE((15,"TGo4ObjectStatus::~TGo4ObjectStatus()",__LINE__, __FILE__));

}

Int_t TGo4ObjectStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4ObjectStatus::PrintStatus()",__LINE__, __FILE__));
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=2048;
   Text_t localbuf[2048];
   Text_t* current=localbuf;
   Int_t size=0;
   Int_t restlen=locallen;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen,"G-OOOO-> Object Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object name: \t\t%s\n",GetName());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object title: \t\t%s\n",GetTitle());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object class: \t\t%s\n",GetObjectClass());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Object size: \t\t%d byte\n",GetObjectSize());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Clear protection: \t\t%d\n",IsResetProtect());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Delete protection: \t\t%d\n",IsDeleteProtect());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Status creation time: \t%s\n",GetTimeString());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
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
