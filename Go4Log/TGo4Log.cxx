// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4Log.h"

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#endif

#include "Riostream.h"
#include "TDataType.h"
#include "TDatime.h"
#include "TMutex.h"
#include "TSystem.h"
#include "snprintf.h"

#include "TGo4LockGuard.h"


const char* TGo4Log::fgcLEFT = "GO4-";
//const char* TGo4Log::fgcRIGHT = " <GO4";
const char* TGo4Log::fgcRIGHT = " ";
const char* TGo4Log::fgcDEBUG = "d";
const char* TGo4Log::fgcINFO = "*";
const char* TGo4Log::fgcWARN = "#";
const char* TGo4Log::fgcERR = "!";
const char* TGo4Log::fgcDEFAULTLOG = "go4logfile.txt";

TString TGo4Log::fgsGO4SYS = "";


char TGo4Log::fgcMessagetext[__MESSAGETEXTLENGTH__];
Int_t TGo4Log::fgiIgnoreLevel = 1;
Bool_t TGo4Log::fgbOutputEnabled = kTRUE;
Bool_t TGo4Log::fgbLogfileEnabled = kFALSE;
Bool_t TGo4Log::fgbAutoMode = kFALSE;
void* TGo4Log::fgxLogfile = 0;
TMutex* TGo4Log::fgxMutex = 0;
TGo4Log* TGo4Log::fgxInstance = 0;

TString TGo4Log::fgxLogName = TGo4Log::fgcDEFAULTLOG;

TNamed* TGo4Log::fgSniffer = 0;

int TGo4Log::fgStdPipe[2] = {-1, -1};
int TGo4Log::fgStdSave = -1;

class TLogTimer : public TTimer {
   public:
      TLogTimer(Int_t millisec) : TTimer(millisec) {}

      virtual Bool_t Notify() { TGo4Log::ProcessRedirection(); return kTRUE; }
};


TLogTimer* TGo4Log::fgTimer = 0;

TGo4Log::TGo4Log()
{
   if (fgxMutex == 0) fgxMutex = new TMutex(kTRUE);


   // initialization at first time we call logger
   // we use recursive mode for cascading lockguards
   if (IsLogfileEnabled()) {
      OpenLogfile(fgcDEFAULTLOG,"--- This is the default Go4 Message logfile ---");
      LogfileEnable(kFALSE); // after first write disable logfile
   }
}

TGo4Log::~TGo4Log()
{
   CloseLogfile();
}

TGo4Log *TGo4Log::Instance()
{
   if(fgxInstance == 0)
      fgxInstance = new TGo4Log();

   return fgxInstance;
}

void TGo4Log::EnableRedirection()
{
   if (fgStdSave > 0) return;

#ifndef WIN32

   fflush(stdout);

   fgStdSave = dup(STDOUT_FILENO);  /* save stdout for display later */

   if(pipe(fgStdPipe) != 0) return; /* make a pipe */

   dup2(fgStdPipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
   long flags = fcntl(fgStdPipe[0], F_GETFL);
   flags |= O_NONBLOCK;
   fcntl(fgStdPipe[0], F_SETFL, flags);

   if (fgTimer==0) {
      fgTimer = new TLogTimer(200);
      fgTimer->Start(200);
   }
#endif
}


void TGo4Log::ProcessRedirection(int kind)
{
   if (fgStdSave<0) return;

#ifndef WIN32

   if (kind>=0) {

      std::cout.flush();
      fflush(stdout);

      char buffer[20000];
      memset(buffer, 0, sizeof(buffer));
      int len = read(fgStdPipe[0], buffer, sizeof(buffer)-1); /* read from pipe into buffer */

      // when nothing to read, than nothing to redirect
      if (len==0) return;

      dup2(fgStdSave, STDOUT_FILENO);  /* reconnect stdout for realoutput */

      printf("%s", buffer);
      fflush(stdout);

      if (fgSniffer!=0) fgSniffer->SetTitle(buffer);
   }

   if (kind<=0) {
      dup2(fgStdPipe[1], STDOUT_FILENO); // redirect again
   }

#endif
}

void TGo4Log::SetSniffer(TNamed* sniff)
{
   fgSniffer = sniff;
}

const char* TGo4Log::GO4SYS()
{
   if (fgsGO4SYS.Length()>0) return fgsGO4SYS.Data();
   const char* go4sys = gSystem->Getenv("GO4SYS");
#ifdef COMP_GO4SYS
   if ((go4sys==0) || (strlen(go4sys)==0)) go4sys = COMP_GO4SYS;
#endif
   if ((go4sys==0) || (strlen(go4sys)==0)) return "";

   fgsGO4SYS = go4sys;
   if (fgsGO4SYS.Length() > 0) {
#ifdef WIN32
      char lastsymbol = '\\';
      fgsGO4SYS.ReplaceAll("/","\\"); // while in cygpath we cannot deliver windows-like slashes
#else
      char lastsymbol = '/';
#endif
      if (fgsGO4SYS[fgsGO4SYS.Length() - 1] != lastsymbol) fgsGO4SYS += lastsymbol;
   }

   return fgsGO4SYS.Length()>0 ? fgsGO4SYS.Data() : "";
}

TString TGo4Log::subGO4SYS(const char* subdir)
{
   const char* go4sys = GO4SYS();

   if ((subdir==0) || (strlen(subdir)==0)) return TString(go4sys);

   TString res = go4sys;

#ifdef WIN32
   res += TString(subdir).ReplaceAll("/","\\");
#else
   res += subdir;
#endif

   return res;
}

const char* TGo4Log::Message(Int_t prio, const char* text,...)
{
   Instance();
   //TGo4LockGuard(fxMutex);
   if(prio>-1 && prio<fgiIgnoreLevel) return 0;
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   const char* prefix(fgcINFO);
   switch(prio) {
      // info output independent of current ignorelevel
      case -1: prefix=fgcINFO;  break;
      case  0: prefix=fgcDEBUG; break;
      case  1: prefix=fgcINFO;  break;
      case  2: prefix=fgcWARN;  break;
      case  3: prefix=fgcERR;   break;
   } // switch()

   if(fgbLogfileEnabled) {
      // message format for logfile is different:
      snprintf(fgcMessagetext,fguMESLEN,"%s %s",prefix,txtbuf);
      WriteLogfile(fgcMessagetext);
   }

   // we compose the full messagetext anyway, for further use outside
   snprintf(fgcMessagetext,fguMESLEN, "%s%s> %s %s",
         fgcLEFT, prefix, txtbuf,fgcRIGHT);

   if(fgbOutputEnabled) {

      ProcessRedirection(1); // disable redirection

      std::cout << fgcMessagetext << std::endl;

      std::cout.flush();

      if (fgSniffer) fgSniffer->SetTitle(fgcMessagetext);

      ProcessRedirection(-1); // enable again
   }

   return fgcMessagetext;
}

void TGo4Log::Debug(const char* text,...)
{
   if(fgiIgnoreLevel>0) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(0,txtbuf);
}

void TGo4Log::Info(const char* text,...)
{
   if(fgiIgnoreLevel>1) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(1,txtbuf);
}

void TGo4Log::Warn(const char* text,...)
{
   if(fgiIgnoreLevel>2) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(2,txtbuf);
}

void TGo4Log::Error(const char* text,...)
{
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(3,txtbuf);
}


void TGo4Log::SetIgnoreLevel(Int_t level)
{
   //TGo4LockGuard(fxMutex);
   fgiIgnoreLevel=level;
}

Int_t TGo4Log::GetIgnoreLevel()
{
   return fgiIgnoreLevel;
}

const char* TGo4Log::GetLogname()
{
   return fgxLogName.Data();
}

const char* TGo4Log::GetDefaultLogname()
{
   return fgcDEFAULTLOG;
}

const char* TGo4Log::GetPrintfArg(Int_t type_id)
{
   switch (type_id) {
      case kInt_t: return "%d";
      case kUInt_t: return "%u";
      case kLong_t: return "%ld";
      case kULong_t: return "%lu";
      case kLong64_t: return sizeof(long long int)==8 ? "%lld" : "%ld";
      case kULong64_t: return sizeof(long long unsigned)==8 ? "%llu" : "%lu";
   }

   return "%d";
}


void TGo4Log::OutputEnable(Bool_t on)
{
   //TGo4LockGuard(fxMutex);
   fgbOutputEnabled=on;
}

Bool_t TGo4Log::IsOutputEnabled()
{
   return fgbOutputEnabled;
}

void TGo4Log::LogfileEnable(Bool_t on)
{
   //TGo4LockGuard(fxMutex);
   fgbLogfileEnabled=on;
}

Bool_t TGo4Log::IsLogfileEnabled()
{
   return fgbLogfileEnabled;
}

void TGo4Log::AutoEnable(Bool_t on)
{
   //TGo4LockGuard(fxMutex);
   fgbAutoMode=on;
}

Bool_t TGo4Log::IsAutoEnabled()
{
   return fgbAutoMode;
}

void TGo4Log::OpenLogfile(const char* name, const char* headercomment, Bool_t appendmode)
{
   //TGo4LockGuard(fxMutex);
   try
   {
      CloseLogfile();
      char txtbuf[fguMESLEN];
      if(name==0)
         // default: encode pid into filename
         snprintf(txtbuf,fguMESLEN,"go4log-%d.txt", gSystem->GetPid());
      else
         snprintf(txtbuf,fguMESLEN,"%s",name);

      std::ofstream* lf = new std::ofstream(txtbuf, appendmode ? std::ios::app : std::ios::out);

      if(lf->fail()) {
         LogfileEnable(kFALSE);
         delete lf;
         std::cerr <<"TGo4Log::OpenLogfile() - Error opening logfile "<< name << std::endl;
      } else {
         fgxLogfile = lf;
         fgxLogName = txtbuf; // remember our last filename
      }
      // write headercomment into the first lines:
      if(headercomment) WriteLogfile(headercomment, kFALSE);
   } // try

   catch(std::exception& ex) // treat standard library exceptions
   {
      std::cerr <<"standard exception "<<ex.what()<<"in TGo4Log::OpenLogfile" << std::endl;
   }
   catch(...)
   {
      std::cerr <<"!!! Unexpected exception in TGo4Log::OpenLogfile !!!" << std::endl;
   } // catch
}

void TGo4Log::WriteLogfile(const char* text, Bool_t withtime)
{
   //TGo4LockGuard(fxMutex);
   if((text==0) || !fgbLogfileEnabled || (fgxLogfile==0)) return;
   try {
      if(withtime) {
         TDatime now;
         *((std::ofstream*)fgxLogfile) << now.AsSQLString() << ": ";
      }
      *((std::ofstream*)fgxLogfile) << text << std::endl;
   }// try
   catch(std::exception& ex) // treat standard library exceptions
   {
      std::cerr <<"standard exception "<<ex.what()<<"in TGo4Log::WriteLogfile" << std::endl;
   }
   catch(...)
   {
      std::cerr <<"!!! Unexpected exception in TGo4Log::WriteLogfile !!!" << std::endl;
   } // catch
}

void TGo4Log::CloseLogfile()
{
   //TGo4LockGuard(fxMutex);
   if(fgxLogfile!=0)
   {
      try
      {
         delete (std::ofstream*)fgxLogfile;
         fgxLogfile=0;
      }
      catch(std::exception& ex) // treat standard library exceptions
      {
         std::cerr <<"standard exception "<<ex.what()<<"in TGo4Log::CloseLogfile" << std::endl;
      }
      catch(...)
      {
         std::cerr <<"!!! Unexpected exception in TGo4Log::CloseLogfile !!!" << std::endl;
      } //
   }
}
