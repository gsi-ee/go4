#include "TGo4MbsEventServer.h"

#include "TGo4Log.h"
#include "TGo4MbsEventServerParameter.h"


TGo4MbsEventServer::TGo4MbsEventServer(const char* name) :
   TGo4MbsSource(name , GETEVT__EVENT)
{
   TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer(Text_t*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsEventServer %s:  ",name);
   SetTimeout(fgiTIMEOUTDEFAULT);
   Open();
}

TGo4MbsEventServer::TGo4MbsEventServer(TGo4MbsEventServerParameter* par) :
   TGo4MbsSource(par, GETEVT__EVENT)
{
   TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer(TGo4MbsEventServerParameter**)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsEventServer %s:  ",GetName());

   Open();
}

TGo4MbsEventServer::TGo4MbsEventServer() :
   TGo4MbsSource()
{
   TRACE((15,"TGo4MbsEventServer::TGo4MbsEventServer()",__LINE__, __FILE__));
}

TGo4MbsEventServer::~TGo4MbsEventServer()
{
   TRACE((15,"TGo4MbsEventServer::~TGo4MbsEventServer()",__LINE__, __FILE__));
   Close();
}
