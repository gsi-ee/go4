// $Id$
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

#ifndef TGO4MBSSOURCE_H
#define TGO4MBSSOURCE_H

#include "TGo4EventSource.h"

#ifdef __CINT__

struct s_filhe;
struct s_bufhe;
struct s_evt_channel;
struct s_ve10_1;

#else

extern "C"
{
  #include "f_evt.h"
  #include "s_filhe_swap.h"
  #include "s_bufhe_swap.h"
}

#endif

class TGo4MbsEvent;
class TGo4MbsSubEvent;
class TGo4MbsSourceParameter;

/** data structure to keep parameters for f_evt_type (SetPrintEvent) */
class TGo4MbsSourcePrintPar : public TObject {

  public:
    TGo4MbsSourcePrintPar() : TObject(), fiNum(0), fiSid(0), fiLong(0), fiHex(0), fiData(0) {}
    virtual ~TGo4MbsSourcePrintPar() {}
    /** Counter for events to be printed */
    Int_t fiNum;
    /** subevent id (-1 is all) */
    Int_t fiSid;
    /** output longwords  */
    Int_t fiLong;
    /** output hex longwords*/
    Int_t fiHex;
    /** output data*/
    Int_t fiData;
  ClassDef(TGo4MbsSourcePrintPar, 1)
};


class TGo4MbsSource : public TGo4EventSource {
  public:

    /** default for streamer */
    TGo4MbsSource();

    TGo4MbsSource(const char* name, Int_t mode);

    TGo4MbsSource(TGo4MbsSourceParameter* par, Int_t mode);

    virtual ~TGo4MbsSource();

    /** Check that filled event is a proper class */
    virtual Bool_t CheckEventClass(TClass* cl);

    /** Fill the destination event \param dest */
    virtual Bool_t BuildEvent(TGo4EventElement* dest);

    /** Close the file or connection.  */
    virtual Int_t Close();

    /** Open the file or connection. */
    virtual Int_t Open();

    /** Requests the next event from mbs source. Returns event status value. */
    virtual Int_t NextEvent();

    /** This methods actually fills the target event class which is passed as pointer. Uses the latest event which is referenced
      * by fxEvent or fxBuffer. Does _not_ fetch a new event
      * from source, therefore one source event may be used
      * to fill several TGo4MbsEvent classes. To get a new
      * event call NextEvent() before this method.*/
    void BuildMbsEvent(TGo4MbsEvent * target);

    /** Auxiliary function to fill datalength words (Short_t) from source
     * into the next subevent of target with matching header fullID longword.
     * Returns handle to this subevent. */
    TGo4MbsSubEvent* BuildMbsSubEvent(TGo4MbsEvent * target, Int_t fullID, Short_t* source, Int_t datalength);

    /** Define the Start index for the NextEvent */
    void SetStartEvent(UInt_t firstindex) { fuStartEvent=firstindex; }

    void SetStopEvent(UInt_t lastindex) { fuStopEvent=lastindex; }

    void SetEventInterval(UInt_t skipinterval) { fuEventInterval=skipinterval; }

    void SetTimeout(Int_t time) { fiTimeout=time; }

    void SetPort(Int_t port) { fiPort=port; }

    /** User access to info header. May contain additional run information */
    s_filhe * GetInfoHeader() { return fxInfoHeader; }

    /** User access to current event buffer header. */
    virtual s_bufhe * GetBufferHeader();

    /** ratio sizeof(long) by sizeof(char). */
    static const UInt_t fguLONGBYCHAR;//!

    /** ratio sizeof(short) by sizeof(char). */
    static const UInt_t fguSHORTBYCHAR;//!

    /** ratio sizeof(long) by sizeof(short). */
    static const UInt_t fguLONGBYSHORT;//!

    /** ratio sizeof(s_evhe) by sizeof(char). */
    static const UInt_t fguEVHEBYCHAR;//!

    /** Set mbs source into printout (verbose) mode for the next num
      * events. Subevent id sid may be specific (-1 is all subevents).
      * Parameters longw, hexw, dataw switch output data formats.
      * See MbsAPI function f_evt_type().*/
    void SetPrintEvent(Int_t num=1, Int_t sid=-1, Int_t longw=1, Int_t hexw=1, Int_t dataw=0);

    void SetDataCopyMode(Bool_t on) { fbDataCopyMode=on; }

  private:

    /** Mode of operation   */
    Int_t fiMode;

    Int_t fiRetryCnt;   //! number of allowed reconnect retries

    Int_t GetMode() const { return fiMode; }

  protected:

    /** Event channel structure used by event source. */
    s_evt_channel* fxInputChannel; //!

    s_ve10_1* fxEvent; //!

    /** Points to the current gsi buffer structure filled by the event source. */
    s_bufhe * fxBuffer; //!

    /** Reference to header info delivered by source. */
    s_filhe * fxInfoHeader; //!

    Bool_t fbIsOpen;

    /** switches buildmbs event mode: if true, data buffer is copied
    * to own buffer of subevent. If false (default), mbs subevent uses
    * reference to external data buffer of eventapi. */
    Bool_t fbDataCopyMode;

    /** Current event index counter */
    ULong_t fuEventCounter;

    /** Indicates if first event should be extracted */
    Bool_t fbFirstEvent;

    /** Index of first event to process. */
    ULong_t fuStartEvent;

    /** Index of last event to process. */
    ULong_t fuStopEvent;

    /** Number of events to skip from file in between two read events  */
    ULong_t fuEventInterval;

    /** Timeout in seconds for mbs getevent. If -1, no timeout (default)  */
    Int_t fiTimeout;

    /** Optional port parameter, if non-positive default port number for transport will be used  */
    Int_t fiPort;

    /** This structure keeps parameters for printevent mode */
    TGo4MbsSourcePrintPar fxPrEventPar; //!

  ClassDef(TGo4MbsSource, 3)

};

#endif //TGO4MBSSOURCE_H
