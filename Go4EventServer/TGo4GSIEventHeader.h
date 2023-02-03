// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4GSIEVENTHEADER_H
#define TGO4GSIEVENTHEADER_H

#include "TObject.h"

/**
 * Standard GSI event/subevent header structure. Little Endian
 * representation (intel linux).
 */
class TGo4GSIEventHeader : public TObject {
  public:

    TGo4GSIEventHeader() : TObject() {}

    virtual ~TGo4GSIEventHeader() {}

    /** Reset all members to zero. */
    void Clear(Option_t * = "") override
    {
       fiDlen = 0;
       fsType = 0;
       fsSubtype = 0;
    }

    /** Length (16 bit words) of the data field following this header.
      * (i.e. datalenth+2 for events, datalength+4 for subevents).
      * Except for GSIBuffer, here occurs the length of data
      * after the _buffer header_ (bufhe) */
    Int_t fiDlen{0};

    /** Type number */
    Short_t fsType{0};

    /** Subtype */
    Short_t fsSubtype{0};

  ClassDefOverride(TGo4GSIEventHeader,1)
};

#endif //TGO4GSIEVENTHEADER_H
