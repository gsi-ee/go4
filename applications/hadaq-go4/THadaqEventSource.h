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
////////////////////////////////////
// Event Source for Hadaq data format
// V 0.3 12-Jul-2011
// Joern Adamczewski-Musch, GSI Darmstadt
// j.adamczewski@gsi.de



#ifndef THadaqEVENTSOURCE_H
#define THadaqEVENTSOURCE_H

#include "TGo4EventSource.h"
#include <fstream>
#include <stdint.h>

#include "TGo4MbsEvent.h"

#define Hadaq_BUFSIZE 0x80000

class THadaqRawEvent;
class TGo4UserSourceParameter;





#pragma pack(push, 1)




//Description of the Event Structure
//
//An event consists of an event header and of varying number of subevents, each with a subevent header. The size of the event header is fixed to 0x20 bytes
//
//Event header
//evtHeader
//evtSize 	evtDecoding 	evtId 	evtSeqNr 	evtDate 	evtTime 	runNr 	expId
//
//    * evtSize - total size of the event including the event header, it is measured in bytes.
//    * evtDecoding - event decoding type: Tells the analysis the binary format of the event data, so that it can be handed to a corresponding routine for unpacking into a software usable data structure. For easier decoding of this word, the meaning of some bits is already predefined:
//      evtDecoding
//      msB 	--- 	--- 	lsB
//      0 	alignment 	decoding type 	nonzero
//          o The first (most significant) byte is always zero.
//          o The second byte contains the alignment of the subevents in the event. 0 = byte, 1 = 16 bit word, 2 = 32 bit word...
//          o The remaining two bytes form the actual decoding type, e.g. fixed length, zero suppressed etc.
//          o The last byte must not be zero, so the whole evtDecoding can be used to check for correct or swapped byte order.
//
//It is stated again, that the whole evtDecoding is one 32bit word. The above bit assignments are merely a rule how to select this 32bit numbers.
//
//    * evtId - event identifier: Tells the analysis the semantics of the event data, e.g. if this is a run start event, data event, simulated event, slow control data, end file event, etc..
//      evtId
//      31 	30 - 16 	15 - 12 	11 - 8 	5 - 7 	4 	3- 0
//      error bit 	reserved 	version 	reserved 	MU decision 	DS flag 	ID
//          o error bit - set if one of the subsystems has set the error bit
//          o version - 0 meaning of event ID before SEP03; 1 event ID after SEP03
//          o MU decision - 0 = negative LVL2 decision; >0 positive LVL2 decision
//            MU trigger algo result
//            1 	negative decision
//            2 	positive decision
//            3 	positive decision due to too many leptons or dileptons
//            4 	reserved
//            5 	reserved
//          o DS flag - LVL1 downscaling flag; 1 = this event is written to the tape independent on the LVL2 trigger decision
//          o ID - defines the trigger code
//            ID before SEP03 	description
//            0 	simulation
//            1 	real
//            2,3,4,5,6,7,8,9 	calibration
//            13 	beginrun
//            14 	endrun
//
//            ID after SEP03 	description
//            0 	simulation
//            1,2,3,4,5 	real
//            7,9 	calibration
//            1 	real1
//            2 	real2
//            3 	real3
//            4 	real4
//            5 	real5
//            6 	special1
//            7 	offspill
//            8 	special3
//            9 	MDCcalibration
//            10 	special5
//            13 	beginrun
//            14 	endrun
//    * evtSeqNr - event number: This is the sequence number of the event in the file. The pair evtFileNr/evtSeqNr
//
//is unique throughout all events ever acquired by the system.
//
//    * evtDate - date of event assembly (filled by the event builder, rough precision):
//      evtDate 	ISO-C date format
//      msB 	--- 	--- 	lsB
//      0 	year 	month 	day
//
//   1. The first (most significant) byte is zero
//   2. The second byte contains the years since 1900
//   3. The third the months since January [0-11]
//   4. The last the day of the month [1-31]
//
//    * evtTime - time of assembly (filled by the event builder, rough precision):
//      evtTime 	ISO-C time format
//      msB 	--- 	--- 	lsB
//      0 	hour 	minute 	second
//
//   1. The first (most significant) byte is zero
//   2. The second byte contains the hours since midnight [0-23]
//   3. The third the minutes after the hour [0-59]
//   4. The last the seconds after the minute [0-60]
//
//    * runNr - file number: A unique number assigned to the file. The runNr is used as key for the RTDB.
//    * evtPad - padding: Makes the event header a multiple of 64 bits long.



struct Hadaq_Event
{
Int_t   evtSize;
Int_t   evtDecoding;
Int_t    evtId;
Int_t evtSeqNr;
Int_t evtDate;
Int_t evtTime;
Int_t runNr;
Int_t evtPad;


/* msb of decode word is always non zero...?*/
Bool_t IsSwapped()
{
	return  (evtDecoding > 0xffffff);
}

/* swapsave access to any data stolen from hadtu.h*/
Int_t Value(Int_t *member)
{

	if (IsSwapped()) {
		uint32_t tmp0;
		uint32_t tmp1;

		tmp0 = *member;
		((char *) &tmp1)[0] = ((char *) &tmp0)[3];
		((char *) &tmp1)[1] = ((char *) &tmp0)[2];
		((char *) &tmp1)[2] = ((char *) &tmp0)[1];
		((char *) &tmp1)[3] = ((char *) &tmp0)[0];
		return tmp1;
	} else {
		return *member;
	}
}

size_t GetSize()
{
	return (size_t) (Value(&evtSize));
}

Int_t GetId()
{
	return Value(&evtId);
}

Int_t GetSeqNr()
{
	return Value(&evtSeqNr);
}


};


//Subevent
//
//Every event contains zero to unspecified many subevents. As an empty event is allowed, data outside of any subevent are not allowed. A subevents consists out of a fixed size subevent header and a varying number of data words.
//
//    * The subevent header
//      subEvtHeader
//      subEvtSize 	subEvtDecoding 	subEvtId 	subEvtTrigNr
//          o subEvtSize - size of the subevent: This includes the the subevent header, it is measured in bytes.
//          o subEvtDecoding - subevent decoding type: Tells the analysis the binary format of the subevent data, so that it can be handed to a corresponding routine for unpacking into a software usable data structure. For easier decoding of this word, the meaning of some bits is already predefined:
//            subEvtDecoding
//            msB 	--- 	--- 	lsB
//            0 	data type 	decoding type 	nonzero
//                + The first (most significant) byte is always zero
//                + The second byte contains the word length of the subevent data. 0 = byte, 1 = 16 bit word, 2 = 32 bit word...
//                + The remaining two bytes form the actual decoding type, e.g. fixed length, zero suppressed etc.
//                + The last byte must not be zero, so the whole subEvtDecoding can be used to check for correct or swapped byte order. It is stated again, that the whole subEvtDecoding is one 32bit word. The above bit assignments are merely a rule how to select this 32bit numbers.
//          o subEvtId - subevent identifier: Tells the analysis the semantics of the subevent data, e.g. every subevent builder may get its own subEvtId. So the data structure can be analyzed by the corresponding routine after unpacking.
//            1-99 	DAQ
//            100-199 	RICH
//            200-299 	MDC
//            300-399 	SHOWER
//            400-499 	TOF
//            500-599 	TRIG
//            600-699 	SLOW
//            700-799 	TRB_RPC 	common TRB, but contents is RPC
//            800-899 	TRB_HOD 	pion-hodoscope
//            900-999 	TRB_FW 	forward wall
//            1000-1099 	TRB_START 	start detector
//            1100-1199 	TRB_TOF 	TOF detector
//            1200-1299 	TRB RICH 	RICH detector
//
//Additionally, all subEvtIds may have the MSB set. This indicates a sub event of the corresponding id that contains broken data (e.g. parity check failed, sub event was too long etc.).
//
//    *
//          o subEvtTrigNr - subevent Trigger Number: This is the event tag that is produced by the trigger system and is used for checking the correct assembly of several sub entities. In general, this number is not counting sequentially. The lowest significant byte represents the trigger tag generated by the CTU and has to be same for all detector subsystems in one event. The rest is filled by the EB.
//    * The data words: The format of the data words (word length, compression algorithm, sub-sub-event format) is defined by the subEvtDecoding and apart from this it is completely free. The meaning of the data words (detector, geographical position, error information) is defined by the subEvtId and apart from this it is completely unknown to the data acquisition system.


struct Hadaq_Subevent
{
Int_t   subEvtSize;
Int_t   subEvtDecoding;
Int_t   subEvtId;
Int_t   subEvtTrigNr;

/* msb of decode word is always non zero...?*/
Bool_t IsSwapped()
{
	return  (subEvtDecoding > 0xffffff);
}


unsigned Alignment()
{
	return 1 << ( GetDecoding() >> 16 & 0xff);
}

/* swapsave access to any member, stolen from hadtu.h*/
Int_t Value(Int_t *member)
{

	if (IsSwapped()) {
		uint32_t tmp0;
		uint32_t tmp1;

		tmp0 = *member;
		((char *) &tmp1)[0] = ((char *) &tmp0)[3];
		((char *) &tmp1)[1] = ((char *) &tmp0)[2];
		((char *) &tmp1)[2] = ((char *) &tmp0)[1];
		((char *) &tmp1)[3] = ((char *) &tmp0)[0];
		return tmp1;
	} else {
		return *member;
	}
}


size_t GetSize()
{
	return (size_t) (Value(&subEvtSize));
}

Int_t GetDecoding()
{
	return Value(&subEvtDecoding);
}

Int_t GetId()
{
	return Value(&subEvtId);
}

Int_t GetTrigNr()
{
	return Value(&subEvtTrigNr);
}



/* swapsave access to any data. stolen from hadtu.h*/
Int_t Data(unsigned idx)
{
	const void* my= (char*) (this) + sizeof(Hadaq_Subevent);
	//const void* my= (char*) (this) + 16;
	Int_t val;

		switch (Alignment()) {
		case 4:
			if (IsSwapped()) {
				uint32_t tmp0;
				uint32_t tmp1;
				tmp0 = ((uint32_t *) my)[idx];
				((char *) &tmp1)[0] = ((char *) &tmp0)[3];
				((char *) &tmp1)[1] = ((char *) &tmp0)[2];
				((char *) &tmp1)[2] = ((char *) &tmp0)[1];
				((char *) &tmp1)[3] = ((char *) &tmp0)[0];
				val = tmp1;
			} else {
				val = ((uint32_t *) my)[idx];
			}
			break;
		case 2:
			if (IsSwapped()) {
				uint16_t v;
				//swab(((uint16_t *) (my)[idx]), &v, 2);
				swab(((uint16_t *) (my)) + idx, &v, 2);
				val = v;
			} else {
				val = ((uint16_t *) my)[idx];
			}
			break;
		default:
			val = ((uint8_t *) my)[idx];
			break;
		}
		return val;



	}



};





#pragma pack(pop)

class THadaqEventSource : public TGo4EventSource {
   public:

      THadaqEventSource();

      /** Create source specifying values directly */
      THadaqEventSource(const char* name, const char* args, Int_t port);

      /** Creat source from setup within usersource parameter object */
      THadaqEventSource(TGo4UserSourceParameter* par);

      virtual ~THadaqEventSource();


      /** Open the file or connection. */
      virtual Int_t Open();

      /** Close the file or connection. */
      virtual Int_t Close();


      /** This method checks if event class is suited for the source */
      virtual Bool_t CheckEventClass(TClass* cl);

      /** This methods actually fills the target event class which is passed as pointer.
        * Uses the latest event which is referenced
        * by fxEvent or fxBuffer. Does _not_ fetch a new event
        * from source, therefore one source event may be used
        * to fill several THadaqEvent classes. To get a new
        * event call NextEvent() before this method.*/
      virtual Bool_t BuildEvent(TGo4EventElement* dest);

      const char* GetArgs() const { return fxArgs.Data(); }

      void SetArgs(const char* arg) { fxArgs=arg; }

      Int_t GetPort() const { return fiPort; }

      void SetPort(Int_t val) { fiPort=val; }

   protected:

      /* read next buffer from file into memory*/
      Bool_t NextBuffer();

      /* Fill next xsys event from buffer location into output structure*/
      Bool_t NextEvent(TGo4MbsEvent* target);

      /* Read from input file and check*/
      std::streamsize ReadFile(Char_t* dest, size_t len);

   private:

      Bool_t fbIsOpen;

      /** Optional argument string */
      TString fxArgs;

      /** Optional port number  */
      Int_t fiPort;

      /** file that contains the data in ascii format. */
      std::ifstream* fxFile; //!



      /* true if next buffer needs to be read from file*/
      //Bool_t fbNeedNewBuffer;

      /* setup subevent ids for mbs container*/
      TGo4SubEventHeader10 fxSubevHead;


      /* working buffer*/
      Char_t* fxBuffer; //!

      /* points to current event header in buffer*/
      Hadaq_Event* fxHadEvent; //!

      /* actual size of bytes read into buffer most recently*/
      std::streamsize fxBufsize; //!

      /* cursor in buffer*/
      Short_t* fxCursor; //!

      /* points to end of buffer*/
      Short_t* fxBufEnd; //!


      /* Length of current event data in shorts*/
      Int_t fiEventLen;





   ClassDef(THadaqEventSource, 1)
};

#endif //TGO4MBSSOURCE_H
