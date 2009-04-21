#include "TGo4RevServ.h"

#include "TGo4Log.h"
#include "TGo4RevServParameter.h"
#include "TGo4EventErrorException.h"

const Int_t TGo4RevServ::fgiDEFAULTPORT=6003;

TGo4RevServ::TGo4RevServ(const char* name) :
   TGo4MbsSource(name , GETEVT__REVSERV),
   fiPort(0)
{
   TRACE((15,"TGo4RevServ::TGo4RevServ(Text_t*)",__LINE__, __FILE__));
   TGo4Log::Debug(" New Event Source RevServ %s:  ",name);
   SetTimeout(fgiTIMEOUTDEFAULT);
   Open();
}

TGo4RevServ::TGo4RevServ(TGo4RevServParameter* par) :
   TGo4MbsSource(par, GETEVT__REVSERV),
   fiPort(par->GetPort())
{
   TRACE((15,"TGo4RevServ::TGo4RevServ(TGo4RevServParameter*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source RevServ %s:  ",GetName());

   Open();
}

TGo4RevServ::TGo4RevServ()
//: TGo4MbsSource("Default RevServ" , GETEVT__REVSERV)
{
   TRACE((15,"TGo4RevServ::TGo4RevServ()",__LINE__, __FILE__));
}

TGo4RevServ::~TGo4RevServ()
{
  TRACE((15,"TGo4RevServ::~TGo4RevServ()",__LINE__, __FILE__));
  Close();
}

Int_t TGo4RevServ::Open()
{
   TRACE((12,"TGo4RevServFile::Open()",__LINE__, __FILE__));
   if(fbIsOpen) return -1;

   if(fiPort==0)
      fiPort=TGo4RevServ::fgiDEFAULTPORT;

   Int_t status  = f_evt_rev_port(fiPort);
   SetCreateStatus(status);
   if(GetCreateStatus() !=GETEVT__SUCCESS) {
       Text_t buffer[TGo4EventSource::fguTXTLEN];
       f_evt_error(GetCreateStatus(), buffer, 1); // provide text message for later output
       SetErrMess(buffer);
       fbIsOpen = kFALSE;
       throw TGo4EventErrorException(this);
    }
    return TGo4MbsSource::Open();
}
