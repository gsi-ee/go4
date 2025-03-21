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

#ifndef TGO4SUBEVENTHEADER10_H
#define TGO4SUBEVENTHEADER10_H

#include "TObject.h"

#include "TGo4GSIEventHeader.h"

#include "typedefs.h"

/**
 * @brief Standard GSI subevent header implementation.
 * @details For Little Endian machines (linux)
 * @ingroup go4_event
 * @author J. Adamczewski
 * @author M. Hemberger
 */
class TGo4SubEventHeader10 : public TObject {
  public:

    /** Reset all members to zero. */
    void Clear(Option_t * = "") override
     {
        fxGSIHeader.Clear();
        fxGSIHeader.fsType = 10;
        //fsProcid = 0; // these three values are all unique identifiers
        //fcSubcrate = 0;
        //fcControl = 0;
     }

    /** @link aggregationByValue */
    TGo4GSIEventHeader fxGSIHeader;

    /** union to correctly access full id as one int value */
   union {
      /** full id number of procid, subcrate and ctrl */
      Int_t fiFullid;

      struct {

      #if MBS_ENDIAN == 1

         /** Processor type code. */
         Char_t fcControl;

         /** Subcrate number */
          Char_t fcSubcrate;

         /** Processor ID [from setup] */
         Short_t fsProcid;

      #else

         /** Processor ID [from setup] */
         Short_t fsProcid;

         /** Subcrate number */
         Char_t fcSubcrate;

         /** Processor type code. */
         Char_t fcControl;

      #endif

      };
   };

    TGo4SubEventHeader10() : TObject(), fxGSIHeader(), fsProcid(0), fcSubcrate(0), fcControl(0) { }

    virtual ~TGo4SubEventHeader10() {}

  ClassDefOverride(TGo4SubEventHeader10,2)
};

#endif // TGO4SUBEVENTHEADER10_H
