// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4Log.h"

#ifndef _MSC_VER
#include <unistd.h>
#include <fcntl.h>
#endif

#include <stdarg.h>
#include <fstream>
#include <iostream>

#include "TDatime.h"
#include "TDataType.h"
#include "TMutex.h"
#include "TSystem.h"


const char *TGo4Log::fgcLEFT = "GO4-";
//const char *TGo4Log::fgcRIGHT = " <GO4";
const char *TGo4Log::fgcRIGHT = " ";
const char *TGo4Log::fgcDEBUG = "d";
const char *TGo4Log::fgcINFO = "*";
const char *TGo4Log::fgcWARN = "#";
const char *TGo4Log::fgcERR = "!";
const char *TGo4Log::fgcDEFAULTLOG = "go4logfile.txt";

TString TGo4Log::fgsGO4SYS = "";

char TGo4Log::fgcMessagetext[__MESSAGETEXTLENGTH__];
Int_t TGo4Log::fgiIgnoreLevel = 1;
Bool_t TGo4Log::fgbOutputEnabled = kTRUE;
Bool_t TGo4Log::fgbLogfileEnabled = kFALSE;
Bool_t TGo4Log::fgbAutoMode = kFALSE;
void *TGo4Log::fgxLogfile = nullptr;
TMutex *TGo4Log::fgxMutex = nullptr;
TGo4Log *TGo4Log::fgxInstance = nullptr;

TString TGo4Log::fgxLogName = TGo4Log::fgcDEFAULTLOG;

TNamed* TGo4Log::fgSniffer = nullptr;

int TGo4Log::fgStdPipe[2] = {-1, -1};
int TGo4Log::fgStdSave = -1;

class TLogTimer : public TTimer {
   public:
      TLogTimer(Int_t millisec) : TTimer(millisec) {}

      Bool_t Notify() override { TGo4Log::ProcessRedirection(); return kTRUE; }
};


TLogTimer* TGo4Log::fgTimer = nullptr;

TGo4Log::TGo4Log()
{
   if (!fgxMutex) fgxMutex = new TMutex(kTRUE);


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
   if(!fgxInstance)
      fgxInstance = new TGo4Log();

   return fgxInstance;
}

void TGo4Log::EnableRedirection()
{
   if (fgStdSave > 0) return;

#ifndef _MSC_VER

   fflush(stdout);

   fgStdSave = dup(STDOUT_FILENO);  /* save stdout for display later */

   if(pipe(fgStdPipe) != 0) return; /* make a pipe */

   dup2(fgStdPipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
   long flags = fcntl(fgStdPipe[0], F_GETFL);
   flags |= O_NONBLOCK;
   fcntl(fgStdPipe[0], F_SETFL, flags);

   if (!fgTimer) {
      fgTimer = new TLogTimer(200);
      fgTimer->Start(200);
   }
#endif
}


void TGo4Log::ProcessRedirection(int kind)
{
   if (fgStdSave < 0) return;

#ifndef _MSC_VER

   if (kind >= 0) {

      std::cout.flush();
      fflush(stdout);

      char buffer[20000];
      memset(buffer, 0, sizeof(buffer));
      int len = read(fgStdPipe[0], buffer, sizeof(buffer)-1); /* read from pipe into buffer */

      // when nothing to read, than nothing to redirect
      if (len == 0) return;

      dup2(fgStdSave, STDOUT_FILENO);  /* reconnect stdout for realoutput */

      printf("%s", buffer);
      fflush(stdout);

      if (fgSniffer) fgSniffer->SetTitle(buffer);
   }

   if (kind <= 0) {
      dup2(fgStdPipe[1], STDOUT_FILENO); // redirect again
   }

#endif
}

void TGo4Log::SetSniffer(TNamed* sniff)
{
   fgSniffer = sniff;
}

const char *TGo4Log::GO4SYS()
{
   if (fgsGO4SYS.Length()>0) return fgsGO4SYS.Data();
   const char *go4sys = gSystem->Getenv("GO4SYS");
#ifdef COMP_GO4SYS
   if (!go4sys || (strlen(go4sys) == 0)) go4sys = COMP_GO4SYS;
#endif
   if (!go4sys || (strlen(go4sys) == 0)) return "";

   fgsGO4SYS = go4sys;
   if (fgsGO4SYS.Length() > 0) {
#ifdef _MSC_VER
      char lastsymbol = '\\';
      fgsGO4SYS.ReplaceAll("/","\\"); // while in cygpath we cannot deliver windows-like slashes
#else
      char lastsymbol = '/';
#endif
      if (fgsGO4SYS[fgsGO4SYS.Length() - 1] != lastsymbol) fgsGO4SYS += lastsymbol;
   }

   return fgsGO4SYS.Length()>0 ? fgsGO4SYS.Data() : "";
}

const char *TGo4Log::GO4INCPATH()
{
   #ifdef COMP_GO4INC
   return COMP_GO4INC;
   #else
   return "";
   #endif
}


TString TGo4Log::subGO4SYS(const char *subdir)
{
   const char *go4sys = GO4SYS();

   if (!subdir || (strlen(subdir) == 0)) return TString(go4sys);

   TString res = go4sys;

#ifdef _MSC_VER
   res += TString(subdir).ReplaceAll("/","\\");
#else
   res += subdir;
#endif

   return res;
}

const char *TGo4Log::Message(Int_t prio, const char *text,...)
{
   Instance();
   //TGo4LockGuard(fxMutex);
   if(prio>-1 && prio<fgiIgnoreLevel) return nullptr;
   char txtbuf[fguMESLEN-20];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN-20, text, args);
   va_end(args);
   const char *prefix = fgcINFO;
   switch(prio) {
      // info output independent of current ignorelevel
      case -1: prefix = fgcINFO;  break;
      case  0: prefix = fgcDEBUG; break;
      case  1: prefix = fgcINFO;  break;
      case  2: prefix = fgcWARN;  break;
      case  3: prefix = fgcERR;   break;
   } // switch()

   if(fgbLogfileEnabled) {
      // message format for logfile is different:
      int resf = snprintf(fgcMessagetext, __MESSAGETEXTLENGTH__, "%s %s", prefix, txtbuf);
      WriteLogfile(fgcMessagetext);

      (void) resf; // prevent compiler warnings
   }

   // we compose the full messagetext anyway, for further use outside
   int res = snprintf(fgcMessagetext, __MESSAGETEXTLENGTH__, "%s%s> %s %s", fgcLEFT, prefix, txtbuf, fgcRIGHT);
   (void) res; // prevent compiler warnings

   if(fgbOutputEnabled) {

      ProcessRedirection(1); // disable redirection

      std::cout << fgcMessagetext << std::endl;

      std::cout.flush();

      if (fgSniffer) fgSniffer->SetTitle(fgcMessagetext);

      ProcessRedirection(-1); // enable again
   }

   return fgcMessagetext;
}

void TGo4Log::Printf(Bool_t _stdout, const char *text)
{
   ProcessRedirection(1); // disable redirection

   if (_stdout) {
      fprintf(stdout, "%s", text);
      fflush(stdout);
   } else {
      fprintf(stderr, "%s", text);
      fflush(stderr);
   }

   ProcessRedirection(-1); // enable again
}


void TGo4Log::PrintRate(ULong64_t cnt, double rate)
{
   ProcessRedirection(1); // disable redirection

   int width = (rate > 1e4) ? 0 : (rate < 1. ? 3 : 1);
   fprintf(stdout, "\rCnt = %llu  Rate = %5.*f Ev/s", cnt, width, rate);
   fflush(stdout);

   ProcessRedirection(-1); // enable again
}

void TGo4Log::Debug(const char *text,...)
{
   if(fgiIgnoreLevel>0) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(0, "%s", txtbuf);
}

void TGo4Log::Info(const char *text,...)
{
   if(fgiIgnoreLevel>1) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(1, "%s", txtbuf);
}

void TGo4Log::Warn(const char *text,...)
{
   if(fgiIgnoreLevel>2) return;
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(2, "%s", txtbuf);
}

void TGo4Log::Error(const char *text,...)
{
   Instance();
   //TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(3, "%s", txtbuf);
}

void TGo4Log::SetIgnoreLevel(Int_t level)
{
   //TGo4LockGuard(fxMutex);
   fgiIgnoreLevel = level;
}

Int_t TGo4Log::GetIgnoreLevel()
{
   return fgiIgnoreLevel;
}

const char *TGo4Log::GetLogname()
{
   return fgxLogName.Data();
}

const char *TGo4Log::GetDefaultLogname()
{
   return fgcDEFAULTLOG;
}

void TGo4Log::OutputEnable(Bool_t on)
{
   fgbOutputEnabled = on;
}

Bool_t TGo4Log::IsOutputEnabled()
{
   return fgbOutputEnabled;
}

void TGo4Log::LogfileEnable(Bool_t on)
{
   fgbLogfileEnabled = on;
}

Bool_t TGo4Log::IsLogfileEnabled()
{
   return fgbLogfileEnabled;
}

void TGo4Log::AutoEnable(Bool_t on)
{
   fgbAutoMode = on;
}

Bool_t TGo4Log::IsAutoEnabled()
{
   return fgbAutoMode;
}

void TGo4Log::OpenLogfile(const char *name, const char *headercomment, Bool_t appendmode)
{
   //TGo4LockGuard(fxMutex);
   try
   {
      CloseLogfile();
      char txtbuf[fguMESLEN];
      if(!name)
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

void TGo4Log::WriteLogfile(const char *text, Bool_t withtime)
{
   //TGo4LockGuard(fxMutex);
   if(!text || !fgbLogfileEnabled || !fgxLogfile) return;
   try {
      std::ofstream *lf = static_cast<std::ofstream*>(fgxLogfile);
      if (lf->is_open()) {
         if(withtime) {
            TDatime now;
            *lf << now.AsSQLString() << ": ";
         }
         *lf << text << std::endl;
      }
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
   if(fgxLogfile)
   {
      try
      {
         delete (std::ofstream*)fgxLogfile;
         fgxLogfile = nullptr;
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
