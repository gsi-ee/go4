#include "TYYYEventSource.h"

#include "Riostream.h"
#include <stdlib.h>
#include "snprintf.h"

#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4UserSourceParameter.h"
#include "TYYYRawEvent.h"

TYYYEventSource::TYYYEventSource(const char* name,
                                 const char* args,
                                 Int_t port) :
   TGo4EventSource(name),
   fbIsOpen(kFALSE),
   fxArgs(args),
   fiPort(port),
   fxFile(0)
{
   Open();
}

TYYYEventSource::TYYYEventSource(TGo4UserSourceParameter* par) :
   TGo4EventSource(" "),
   fbIsOpen(kFALSE),
   fxArgs(" "),
   fiPort(0),
   fxFile(0)
{
   if(par)
      {
        SetName(par->GetName());
        SetPort(par->GetPort());
        SetArgs(par->GetExpression());
        Open();
      }
   else
      {
        cout <<"TYYYEventSource constructor with zero parameter!" << endl;
      }
}

TYYYEventSource::TYYYEventSource() :
   TGo4EventSource("default YYY source"),
   fbIsOpen(kFALSE),
   fxArgs(" "),
   fiPort(0),
   fxFile(0)
{
}

TYYYEventSource::~TYYYEventSource()
{
   Close();
}

void TYYYEventSource::BuildYYYEvent(TYYYRawEvent* target)
{
   Int_t status=1;
   // process on event information in our buffer
   // scan the last input line for values:
   Text_t buffer[TGo4EventSource::fguTXTLEN];
   Int_t scanresult=0;
   Int_t numval=0;
   const char* cursor = fxNextline.Data();
   do{
      target->ReAllocate(numval+1); // check if realloc necessary
      scanresult = sscanf(cursor,"%s",buffer);
      //cout <<"BuildYYYEvent got buffer:"<<buffer<<", scanresult:";
      //cout << scanresult << endl;
      if(scanresult!=0 && scanresult!=-1)
         {
           target->fdData[numval]=atof(buffer);
           //cout <<"filled data:"<<target->fdData[numval] << endl;
            status=0; // only ok if at least one value scanned
         }
      numval++;
      cursor+=strlen(buffer)+1;
      //cout <<"cursor set to:"<<cursor << endl;
   } while( scanresult!=0 && scanresult!=-1);



   // test here for error in input event

   if(status==0)
      {
      target->SetValid(kTRUE); // reset target if previously was set to false
      // here build event from source!
      }
   else
      {
         target->SetValid(kFALSE);
         // somethings wrong, display error message from f_evt_error()
            SetErrMess("YYY Event Source --  ERROR !!!");
            throw TGo4EventErrorException(this);
      }
}

Int_t TYYYEventSource::NextEvent()
{
// read another event from open file into our buffer
   do {
        fxFile->getline(const_cast<Text_t*>(fxNextline.Data()),
                        TGo4EventSource::fguTXTLEN,
                        '\n' ); // read whole line
        if(fxFile->eof() || !fxFile->good())
          {
              // reached last line or read error?
              SetCreateStatus(1);
              SetErrMess(Form("End of input file %s", GetName()));
              throw TGo4EventErrorException(this);
          }
      //cout <<"read line:"<<fxNextline.Data() << endl;
   }while(strstr(fxNextline.Data(),"#") || strstr(fxNextline.Data(),"!") ); // skip any comments

   return 0;
}

Int_t TYYYEventSource::Open()
{
   if(fbIsOpen) return -1;
   cout << "Open of TYYYEventSource"<< endl;
   // open connection/file
   fxNextline.Capacity(TGo4EventSource::fguTXTLEN);
   fxFile = new std::ifstream(GetName());
   if((fxFile==0) || !fxFile->good()) {
      delete fxFile; fxFile = 0;
      SetCreateStatus(1);
      SetErrMess(Form("Eror opening user file:%s",GetName()));
      throw TGo4EventErrorException(this);
   }
   fbIsOpen=kTRUE;
   return 0;
}

Int_t TYYYEventSource::Close()
{
   if(!fbIsOpen) return -1;
   cout << "Close of TYYYEventSource"<< endl;
   Int_t status=0; // closestatus of source
   delete fxFile;
   fbIsOpen=kFALSE;
   return status;
}
