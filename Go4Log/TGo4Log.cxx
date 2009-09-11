#include "TGo4Log.h"

#include "Riostream.h"
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
Int_t TGo4Log::fgiIgnoreLevel=1;
Bool_t TGo4Log::fgbOutputEnabled=kTRUE;
Bool_t TGo4Log::fgbLogfileEnabled=kTRUE;
Bool_t TGo4Log::fgbAutoMode=kFALSE;
void* TGo4Log::fgxLogfile=0;
TMutex* TGo4Log::fgxMutex=0;
TGo4Log* TGo4Log::fgxInstance=0;

TString TGo4Log::fgxLogName=TGo4Log::fgcDEFAULTLOG;

TGo4Log::TGo4Log()
{
   // initialization at first time we call logger
   // we use recursive mode for cascading lockguards
   if(fgxMutex==0) fgxMutex = new TMutex(kTRUE);
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
   TGo4LockGuard(fxMutex);
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
      cout << fgcMessagetext << endl;
   }

   return fgcMessagetext;
}

void TGo4Log::Debug(const char* text,...)
{
   if(fgiIgnoreLevel>0) return;
   Instance();
   TGo4LockGuard(fxMutex);
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
   TGo4LockGuard(fxMutex);
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
   TGo4LockGuard(fxMutex);
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
   TGo4LockGuard(fxMutex);
   char txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   Message(3,txtbuf);
}


void TGo4Log::SetIgnoreLevel(Int_t level)
{
   TGo4LockGuard(fxMutex);
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

void TGo4Log::OutputEnable(Bool_t on)
{
   TGo4LockGuard(fxMutex);
   fgbOutputEnabled=on;

}

Bool_t TGo4Log::IsOutputEnabled()
{
   return fgbOutputEnabled;
}

void TGo4Log::LogfileEnable(Bool_t on)
{
   TGo4LockGuard(fxMutex);
   fgbLogfileEnabled=on;
}

Bool_t TGo4Log::IsLogfileEnabled()
{
   return fgbLogfileEnabled;
}

void TGo4Log::AutoEnable(Bool_t on)
{
   TGo4LockGuard(fxMutex);
   fgbAutoMode=on;
}

Bool_t TGo4Log::IsAutoEnabled()
{
   return fgbAutoMode;
}


void TGo4Log::OpenLogfile(const char* name,
      const char* headercomment,
      Bool_t appendmode)
{
   TGo4LockGuard(fxMutex);
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
         cerr <<"TGo4Log::OpenLogfile() - Error opening logfile "<< name << endl;
      } else {
         fgxLogfile = lf;
         fgxLogName = txtbuf; // remember our last filename
      }
      // write headercomment into the first lines:
      if(headercomment) WriteLogfile(headercomment, kFALSE);
   } // try

   catch(std::exception& ex) // treat standard library exceptions
   {
      cerr <<"standard exception "<<ex.what()<<"in TGo4Log::OpenLogfile" << endl;
   }
   catch(...)
   {
      cerr <<"!!! Unexpected exception in TGo4Log::OpenLogfile !!!" << endl;
   } // catch
}

void TGo4Log::WriteLogfile(const char* text, Bool_t withtime)
{
   TGo4LockGuard(fxMutex);
   if((text==0) || !fgbLogfileEnabled || (fgxLogfile==0)) return;
   try {
      if(withtime) {
         TDatime now;
         *((std::ofstream*)fgxLogfile) << now.AsSQLString() << ": ";
      }
      *((std::ofstream*)fgxLogfile) << text << endl;
   }// try
   catch(std::exception& ex) // treat standard library exceptions
   {
      cerr <<"standard exception "<<ex.what()<<"in TGo4Log::WriteLogfile" << endl;
   }
   catch(...)
   {
      cerr <<"!!! Unexpected exception in TGo4Log::WriteLogfile !!!" << endl;
   } // catch
}



void TGo4Log::CloseLogfile()
{
   TGo4LockGuard(fxMutex);
   if(fgxLogfile!=0)
   {
      try
      {
         delete (std::ofstream*)fgxLogfile;
         fgxLogfile=0;
      }
      catch(std::exception& ex) // treat standard library exceptions
      {
         cerr <<"standard exception "<<ex.what()<<"in TGo4Log::CloseLogfile" << endl;
      }
      catch(...)
      {
         cerr <<"!!! Unexpected exception in TGo4Log::CloseLogfile !!!" << endl;
      } //
   }

}
