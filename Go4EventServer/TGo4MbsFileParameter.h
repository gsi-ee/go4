#ifndef TGO4MBSFILEPARAMETER_H
#define TGO4MBSFILEPARAMETER_H

#include "TGo4EventSourceParameter.h"

#include "TString.h"

class TGo4MbsFileParameter : public TGo4EventSourceParameter {
  public:
    TGo4MbsFileParameter() ;

    TGo4MbsFileParameter(const char* name);

    virtual ~TGo4MbsFileParameter();

    /** Define the Start index for the NextEvent */
    void SetStartEvent(UInt_t firstindex) { fuStartEvent=firstindex; }

    /** Returns the Start index for the NextEvent */
    UInt_t GetStartEvent() const { return fuStartEvent; }

    /** Name of the Tagfile */
    const char* GetTagName() const { return fxTagFile.Data(); }

    /** Name of the Tagfile */
    void SetTagName(const char* name) { fxTagFile = name; }

    /** Define the index of Stop event */
    void SetStopEvent(UInt_t lastindex) { fuStopEvent=lastindex; }

    /** Returns the index of Stop event */
    UInt_t GetStopEvent() const { return fuStopEvent; }

    /** Defines event interval */
    void SetEventInterval(UInt_t skipinterval) { fuEventInterval=skipinterval; }

    /** Returns event interval */
    UInt_t GetEventInterval() const { return fuEventInterval; }

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    /** update contents of paramter class with external object. */
    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

  private:

    /** Index of first event to process. */
    UInt_t fuStartEvent;

    /** Index of last event to process. */
    UInt_t fuStopEvent;

    /** Number of events to skip from file in between two read events  */
    UInt_t fuEventInterval;

    /** Name of the tagfile associated with the listmode event file. */
    TString fxTagFile;

  ClassDef(TGo4MbsFileParameter,1)

};

#endif //TGO4MBSFILEPARAMETER_H
