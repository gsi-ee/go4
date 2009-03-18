#include "TGo4MbsTransport.h"

#include "TGo4Log.h"
#include "TGo4MbsTransportParameter.h"


TGo4MbsTransport::TGo4MbsTransport(const char* name)
: TGo4MbsSource(name , GETEVT__TRANS)
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport(Text_t*)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source MbsTransport %s:  ",name);
   SetTimeout(fgiTIMEOUTDEFAULT);
   Open();
}

TGo4MbsTransport::TGo4MbsTransport(TGo4MbsTransportParameter* par)
: TGo4MbsSource(par->GetName() , GETEVT__TRANS)
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport(TGo4MbsTransportParameter**)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source MbsTransport %s:  ",GetName());
   SetTimeout(par->GetTimeout());
   Open();
}


TGo4MbsTransport::TGo4MbsTransport()
//: TGo4MbsSource("Default MbsTransport" , GETEVT__TRANS)
{
   TRACE((15,"TGo4MbsTransport::TGo4MbsTransport()",__LINE__, __FILE__));
}


TGo4MbsTransport::~TGo4MbsTransport()
{
   TRACE((15,"TGo4MbsTransport::~TGo4MbsTransport()",__LINE__, __FILE__));
   Close();
}
