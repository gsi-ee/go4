
#include "THadaqEventSource.h"

#include "TClass.h"
#include "Riostream.h"
#include <stdlib.h>

#include "TGo4EventErrorException.h"
#include "TGo4EventEndException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4UserSourceParameter.h"
#include "TGo4MbsEvent.h"
#include "TGo4Log.h"

THadaqEventSource::THadaqEventSource(const char* name,
                                 const char* args,
                                 Int_t port) :
   TGo4EventSource(name),
   fbIsOpen(kFALSE),
   fxArgs(args),
   fiPort(port),
   fxFile(0),
   fxBuffer(0),
   fxHadEvent(0),
   fxBufsize(0),
   fxCursor(0),
   fxBufEnd(0)
{
   Open();
}

THadaqEventSource::THadaqEventSource(TGo4UserSourceParameter* par) :
   TGo4EventSource(" "),
   fbIsOpen(kFALSE),
    fxArgs(""),
    fiPort(0),
    fxFile(0),
    fxBuffer(0),
    fxHadEvent(0),
    fxBufsize(0),
    fxCursor(0),
    fxBufEnd(0)
{
   if(par) {
      SetName(par->GetName());
      SetPort(par->GetPort());
      SetArgs(par->GetExpression());
      Open();
   } else {
      cout <<"THadaqEventSource constructor with zero parameter!" << endl;
   }
}

THadaqEventSource::THadaqEventSource() :
   TGo4EventSource("default Hadaq source"),
   fbIsOpen(kFALSE),
    fxArgs(""),
    fiPort(0),
    fxFile(0),
    fxBuffer(0),
    fxHadEvent(0),
    fxBufsize(0),
    fxCursor(0),
    fxBufEnd(0)
{
}

THadaqEventSource::~THadaqEventSource()
{
   Close();
}

Bool_t THadaqEventSource::CheckEventClass(TClass* cl)
{
   return cl->InheritsFrom(TGo4MbsEvent::Class());
}

std::streamsize THadaqEventSource::ReadFile(Char_t* dest, size_t len)
{
	fxFile->read(dest, len);
	if(fxFile->eof() || !fxFile->good())
			{
					SetCreateStatus(1);
					SetErrMess(Form("End of input file %s", GetName()));
					SetEventStatus(1);
					throw TGo4EventEndException(this);
			}
	//cout <<"ReadFile reads "<< (hex) << fxFile->gcount()<<" bytes to 0x"<< (hex) <<(int) dest<< endl;
	return fxFile->gcount();
}



Bool_t THadaqEventSource::NextBuffer()
{
	// each buffer contains one full event:
	// first read event header:
	ReadFile(fxBuffer, sizeof(Hadaq_Event));
	fxHadEvent=(Hadaq_Event*)(fxBuffer);

	// then read rest of buffer from file
	size_t evlen=fxHadEvent->GetSize();
	// account padding of events to 8 byte boundaries:
	 while((evlen % 8) !=0)
		 {
			 evlen++;
			 //cout <<"Next Buffer extends for padding the length to "<<evlen<< endl;
		 }
	//cout <<"Next Buffer reads event of size:"<<evlen<< endl;
	if(evlen > Hadaq_BUFSIZE)
		{
			SetErrMess(Form("Next event length 0x%x bigger than read buffer limit 0x%x", evlen, Hadaq_BUFSIZE));
			SetEventStatus(1);
			throw TGo4EventEndException(this);

		}
	ReadFile(fxBuffer+sizeof(Hadaq_Event), evlen - sizeof(Hadaq_Event));
	fxBufsize=evlen;
	fxBufEnd=(Short_t*)( fxBuffer+ evlen);
	return kTRUE;

}

Bool_t THadaqEventSource::NextEvent(TGo4MbsEvent* target)
{
	// we fill complete hadaq event into one mbs subevent, not to lose header information
	// decomposing hades subevents is left to the user analysis processor!

	fiEventLen=(fxBufsize/sizeof(Short_t)) ; // data payload of mbs is full hades event in buffer
	fxSubevHead.fiFullid=fxHadEvent->GetId(); // set mbs subevent id to original hades event id
	int evcounter=fxHadEvent->GetSeqNr();
    //cout <<"Next Event is filling mbs event, fullid:"<<fxSubevHead.fiFullid<<", nr:"<<evcounter<< endl;
	target->AddSubEvent(fxSubevHead.fiFullid, (Short_t*) fxBuffer, fiEventLen + 2, kTRUE);
	target->SetDlen(fiEventLen+2+4); // total length of pseudo mbs event
	//cout <<"Next Event setting event length:"<<fiEventLen+2+4<< endl;

	target->SetCount(evcounter);

	//return kFALSE; // this means we need another file buffer for this event, what is never the case since buffer contains just one event
	return kTRUE; // event is ready
}


Bool_t THadaqEventSource::BuildEvent(TGo4EventElement* dest)
{
   TGo4MbsEvent* evnt = dynamic_cast<TGo4MbsEvent*> (dest);
   if (evnt==0) return kFALSE;
   // this generic loop is intended to handle buffers with several events. we keep it here,
   // although our "buffers" consists of single events.
   do {
		   NextBuffer(); // note: next buffer will throw exception on end of file
	   }
   while (!NextEvent(evnt));


   return kTRUE;
}

Int_t THadaqEventSource::Open()
{
   if(fbIsOpen) return -1;
   cout << "Open of THadaqEventSource"<< endl;
   // open connection/file
   fxFile = new std::ifstream(GetName(), ios::binary);
   if((fxFile==0) || !fxFile->good()) {
      delete fxFile; fxFile = 0;
      SetCreateStatus(1);
      SetErrMess(Form("Eror opening user file:%s",GetName()));
      throw TGo4EventErrorException(this);
   }
   fxBuffer=new Char_t[Hadaq_BUFSIZE];




   fbIsOpen=kTRUE;
   return 0;
}

Int_t THadaqEventSource::Close()
{
   if(!fbIsOpen) return -1;
   cout << "Close of THadaqEventSource"<< endl;
   delete fxBuffer;
   Int_t status=0; // closestatus of source
   delete fxFile;
   fbIsOpen=kFALSE;
   return status;
}
