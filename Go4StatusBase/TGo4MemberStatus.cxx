#include "TGo4MemberStatus.h"

#include "Riostream.h"

TGo4MemberStatus::TGo4MemberStatus() :
   TGo4ObjectStatus()
{
}

TGo4MemberStatus::TGo4MemberStatus(const char* name, const char* datatypename) :
   TGo4ObjectStatus(0,kFALSE)
{
   SetName(name);
   SetTitle(datatypename);
   fxObjectClass = datatypename;
}

TGo4MemberStatus::~TGo4MemberStatus()
{
}

Int_t TGo4MemberStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=128000;
   Text_t localbuf[128000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta   = TGo4ObjectStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> Datamember Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
  if(buffer==0)
      {
          cout << localbuf << endl;
      }
   else
      {
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}
