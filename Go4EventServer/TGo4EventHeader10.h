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

#ifndef TGO4EVENTHEADER10_H
#define TGO4EVENTHEADER10_H

#include "TObject.h"

#include "TGo4GSIEventHeader.h"

/**
 * @brief Standard GSI (goosy, mbs) event header implementation.
 * @details For Little Endian machines (linux)
 * @ingroup go4_event
 * @since 25-jan-2001
 * @author J. Adamczewski
 * @author M. Hemberger*/

class TGo4EventHeader10 : public TObject {
   public:

      TGo4EventHeader10() :
         TObject(),
         fxGSIHeader(),
         fsDummy(0),
         fsTrigger(0),
         fiCount(0)
      {}

      virtual ~TGo4EventHeader10() {}

      /** Reset all members to zero. */
      void Clear(Option_t * = "") override
      {
         fxGSIHeader.Clear();
         fsDummy = 0;
         fsTrigger = 0;
         fiCount = 0;
      }

      /** @link aggregationByValue */
      TGo4GSIEventHeader fxGSIHeader;

      /** not used yet. */
      Short_t fsDummy{0};

      /** Trigger number */
      Short_t fsTrigger{0};

      Int_t fiCount{0};

   ClassDefOverride(TGo4EventHeader10,1)
};

#endif // TGO4EVENTHEADER10_H
