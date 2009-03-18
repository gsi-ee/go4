#ifndef TGO4GSIEVENTHEADER_H
#define TGO4GSIEVENTHEADER_H

#include "TObject.h"

/**
 * Standard GSI event/subevent header structure. Little Endian
 * representation (intel linux).
 */
class TGo4GSIEventHeader : public TObject {
  public:

    TGo4GSIEventHeader() : fiDlen(0), fsType(0), fsSubtype(0) { }

    virtual ~TGo4GSIEventHeader() { }

    /** Reset all members to zero. */
    virtual void Clear(Option_t* opt="")
      {
         fiDlen=0;
         fsType=0;
         fsSubtype=0;
      }

    /** Length (16 bit words) of the data field following this header.
      * (i.e. datalenth+2 for events, datalength+4 for subevents).
      * Except for GSIBuffer, here occurs the length of data
      * after the _buffer header_ (bufhe) */
    Int_t fiDlen;

    /** Type number */
    Short_t fsType;

    /** Subtype */
    Short_t fsSubtype;

  ClassDef(TGo4GSIEventHeader,1)
};

#endif //TGO4GSIEVENTHEADER_H



