#include "TGo4MbsStream.h"

#include "TGo4Log.h"
#include "TGo4MbsStreamParameter.h"

TGo4MbsStream::TGo4MbsStream(const char* name) :
   TGo4MbsSource(name , GETEVT__STREAM)
{
TRACE((15,"TGo4MbsStream::TGo4MbsStream(Text_t*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsStream %s:  ",name);
   SetTimeout(fgiTIMEOUTDEFAULT);
   Open();
}

TGo4MbsStream::TGo4MbsStream(TGo4MbsStreamParameter* par) :
   TGo4MbsSource(par , GETEVT__STREAM)
{
   TRACE((15,"TGo4MbsStream::TGo4MbsStream(TGo4MbsStreamParameter**)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsStream %s:  ",GetName());
   Open();
}

TGo4MbsStream::TGo4MbsStream()
{
   TRACE((15,"TGo4MbsStream::TGo4MbsStream()",__LINE__, __FILE__));
}

TGo4MbsStream::~TGo4MbsStream()
{
TRACE((15,"TGo4MbsStream::~TGo4MbsStream()",__LINE__, __FILE__));
   Close();
}
