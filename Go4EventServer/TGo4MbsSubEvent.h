#ifndef TGO4MBSSUBEVENT_H
#define TGO4MBSSUBEVENT_H

#include "TGo4EventElement.h"

#include "TGo4SubEventHeader10.h"

/** Subevent class for gsi mbs data. */
class TGo4MbsSubEvent : public TGo4EventElement {

  friend class TGo4MbsSource;


  public:

    // only for streaming
    TGo4MbsSubEvent();

    TGo4MbsSubEvent(Int_t datasize);

    virtual ~TGo4MbsSubEvent();

    virtual Int_t Fill(){return -1; }// method not used

    virtual void Clear(Option_t *t="");

    void PrintEvent();

    void Set(Int_t dlen, Short_t type=10, Short_t subtype=1,
             Short_t procid=0, Char_t subcrate=0, Char_t ctrl=0);

    void  SetDlen(Int_t dlen) { fxHeader.fxGSIHeader.fiDlen = dlen; }
    Int_t  GetDlen() const { return fxHeader.fxGSIHeader.fiDlen; }
    Int_t  GetIntLen() const { return (GetDlen()-2)*sizeof(Short_t)/sizeof(Int_t); }

    void  SetType(Short_t type) { fxHeader.fxGSIHeader.fsType  = type; }
    Short_t GetType() const { return fxHeader.fxGSIHeader.fsType; }

    void  SetSubtype(Short_t subtype) { fxHeader.fxGSIHeader.fsSubtype = subtype; }
    Short_t GetSubtype() const { return fxHeader.fxGSIHeader.fsSubtype; }

    void  SetProcid(Short_t procid) { fxHeader.fsProcid = procid; }
    Short_t GetProcid() const { return fxHeader.fsProcid; }

    void  SetSubcrate(Char_t subcrate) { fxHeader.fcSubcrate = subcrate; }
    Char_t GetSubcrate() const { return fxHeader.fcSubcrate; }

    void  SetControl(Char_t control) { fxHeader.fcControl = control; }
    Char_t GetControl() const { return fxHeader.fcControl; }

    /** Return length of allocated memory for data field. */
    Int_t GetAllocatedLength() { return fiAllocLen; }

    /** Direct access to the fiData field pointer. User has to care
      * about allocated range as specified in fiAllocLen. */
    Int_t* GetDataField() { return fiData; }

    /** Returns the value at position i in the fiData field.
      * If Index i is out of DLen range, zero is returned. */
    Int_t Data(Int_t i)
     { return ((i<0) || (i>=GetAllocatedLength())) ? 0 : fiData[i]; }

    /** true if this subevent was filled since the last Clear() */
     Bool_t IsFilled() { return fbIsFilled; }

  private:

    /** Allocate the data field with newsize. Free the previously
      * allocated field before, and set new fiAllocLen value. */
    void ReAllocate(Int_t newsize);

    /** True if this subevent has already been filled in this event cycle.
      * For subsequent subevents of identical header ids, prevents overwriting. */
    Bool_t fbIsFilled;

    /** True if subevent owns data field.*/
    Bool_t fbIsDataOwner;

    /** @link aggregationByValue */
    TGo4SubEventHeader10 fxHeader;

    /** Length of the allocated data field. Is _not_ equal to the
      * subevent data length which can be smaller. On filling the
      * subevent, it is tested if the source data fits into the
      * already allocated field; if the source data is larger,
      * we will re-allocate the field with the larger size
      * before filling it. If the source data is smaller, we
      * leave the data field as is and use only the first
      * DLen words. */
    Int_t fiAllocLen;

    /** Pointer to external integer field containing the
      * subevent data. Length of this field may be changed
      * dynamically depending on the maximum length
      * of the incoming subevent of this type. */
    Int_t* fiData; //[fiAllocLen]

  ClassDef(TGo4MbsSubEvent,1)
};

#endif //TGO4MBSSUBEVENT_H
