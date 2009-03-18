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
 if(fgxMutex==0)
      {
          fgxMutex= new TMutex(kTRUE); // we use recursive mode for cascading lockguards
      }
OpenLogfile(fgcDEFAULTLOG,"--- This is the default Go4 Message logfile ---");
LogfileEnable(kFALSE); // after first write disable logfile
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

const char* TGo4Log::Message(Int_t prio, const char* text,...)
{
   Instance();
   TGo4LockGuard(fxMutex);
   if(prio>-1 && prio<fgiIgnoreLevel) return 0;
   Text_t txtbuf[fguMESLEN];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, fguMESLEN, text, args);
   va_end(args);
   const Text_t* prefix;
   switch(prio)
     {
        case -1:
          // info output independent of current ignorelevel
          prefix=fgcINFO;
          break;
        case 0:
          prefix=fgcDEBUG;
          break;
        case 1:
          prefix=fgcINFO;
          break;
        case 2:
          prefix=fgcWARN;
          break;
        case 3:
          prefix=fgcERR;
          break;
        default:
           prefix=fgcINFO;
           break;
   } // switch()

   if(fgbLogfileEnabled)
   {
      // message format for logfile is different:
      snprintf(fgcMessagetext,fguMESLEN,"%s %s",prefix,txtbuf);
      WriteLogfile(fgcMessagetext);
   } else {}

   // we compose the full messagetext anyway, for further use outside
   snprintf(fgcMessagetext,fguMESLEN, "%s%s> %s %s",
                fgcLEFT, prefix, txtbuf,fgcRIGHT);
   if(fgbOutputEnabled)
      {
         cout << fgcMessagetext << endl;
      } else {}

  return fgcMessagetext;
}

void TGo4Log::Debug(const char* text,...)
{
   if(fgiIgnoreLevel>0) return;
   Instance();
   TGo4LockGuard(fxMutex);
   Text_t txtbuf[fguMESLEN];
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
   Text_t txtbuf[fguMESLEN];
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
   Text_t txtbuf[fguMESLEN];
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
   Text_t txtbuf[fguMESLEN];
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
   Text_t txtbuf[fguMESLEN];
   if(name==0)
      {
         // default: encode pid into filename
         Int_t pid=gSystem->GetPid();
         snprintf(txtbuf,fguMESLEN,"go4log-%d.txt",pid);
      }
   else
      {
         snprintf(txtbuf,fguMESLEN,"%s",name);
      }

   if(appendmode)
      fgxLogfile = new std::ofstream(txtbuf,ios::app);
   else
      fgxLogfile = new std::ofstream(txtbuf);
   if(((std::ofstream*)fgxLogfile)->fail())
      {
       LogfileEnable(kFALSE);
       cerr <<"TGo4Log::OpenLogfile() - Error opening logfile "<<name << endl;
      }
   else
      {
       fgxLogName=txtbuf; // remember our last filename
   }
   // write headercomment into the first lines:
   if(headercomment) WriteLogfile(headercomment,kFALSE);
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
if(text==0) return;
if((fgxLogfile!=0) && fgbLogfileEnabled)
{
try
   {
   if(withtime)
      {
         Text_t buffer[fguMESLEN];
         TDatime now;
         snprintf(buffer,fguMESLEN,"%s: %s",
                     now.AsSQLString(),
                        text);
         *((std::ofstream*)fgxLogfile) << buffer << endl;
      }
   else
      {
         *((std::ofstream*)fgxLogfile) << text << endl;
      }
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
