#include "TGo4TaskHandlerStatus.h"

#include "Riostream.h"

#include "TGo4Log.h"

TGo4TaskHandlerStatus::TGo4TaskHandlerStatus() :
   TGo4Status(),
   fxComName(),
   fxStatName(),
   fxDatName(),
   fxHostName(),
   fuNegPort(0),
   fiComPort(0),
   fiStatPort(0),
   fiDatPort(0),
   fbIsAborting(kFALSE)
{
}

TGo4TaskHandlerStatus::TGo4TaskHandlerStatus(const char* name) :
   TGo4Status(name),
   fxComName(),
   fxStatName(),
   fxDatName(),
   fxHostName(),
   fuNegPort(0),
   fiComPort(0),
   fiStatPort(0),
   fiDatPort(0),
   fbIsAborting(kFALSE)

{
}

TGo4TaskHandlerStatus::~TGo4TaskHandlerStatus()
{
}

Int_t TGo4TaskHandlerStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=4096;
   Text_t localbuf[4096];
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t size=0;
   current=PrintBuffer(current,restlen, "G-OOOO-> TaskHandler Status Class %s Printout: <-OOOO-G\n",GetName());
   current=PrintBuffer(current,restlen, "G-OOOO-> --------------------------------------------------------------------- <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> Remote Server: %s\n",GetHostName());
   current=PrintBuffer(current,restlen, "G-OOOO-> Negotiation Port: %d\n",fuNegPort);
   current=PrintBuffer(current,restlen, "G-OOOO-> Command Thread: %s\n",GetComName());
   current=PrintBuffer(current,restlen, "G-OOOO-> Command Port: %d\n",fiComPort);
   current=PrintBuffer(current,restlen, "G-OOOO-> Data Thread: %s\n",GetDatName());
   current=PrintBuffer(current,restlen, "G-OOOO-> Data Port: %d\n",fiDatPort);
   current=PrintBuffer(current,restlen, "G-OOOO-> Status Thread: %s\n",GetStatName());
   current=PrintBuffer(current,restlen, "G-OOOO-> Status Port: %d\n",fiStatPort);
   if(fbIsAborting)
      {
         current=PrintBuffer(current,restlen, "G-OOOO-> Aborting State: TRUE\n");
      }
   else
      {
         current=PrintBuffer(current,restlen, "G-OOOO-> Aborting State: FALSE\n");
      }
   current=PrintBuffer(current,restlen, "G-OOOO-> END TaskHandler Status Class Printout END <-OOOO-G\n");
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

void TGo4TaskHandlerStatus::SetFlags(Bool_t isaborting)
{
   fbIsAborting=isaborting;
}

void TGo4TaskHandlerStatus::SetPorts(UInt_t neg, Int_t com, Int_t stat, Int_t dat)
{
   fuNegPort=neg;
   fiComPort=com;
   fiStatPort=stat;
   fiDatPort=dat;
}

void TGo4TaskHandlerStatus::SetNames(const char* com, const char* stat, const char* dat, const char* host)
{
   fxComName = com;
   fxStatName = stat;
   fxDatName = dat;
   fxHostName = host;
}
