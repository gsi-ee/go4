#ifndef TGO4EVENTHEADER10_H
#define TGO4EVENTHEADER10_H

#include "TObject.h"

#include "TGo4GSIEventHeader.h"

/**
 * Standard GSI (goosy, mbs) event header implementation.
 * For Little Endian machines (linux)
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
      virtual void Clear(Option_t* ="")
      {
         fxGSIHeader.Clear();
         fsDummy=0;
         fsTrigger=0;
         fiCount=0;
      }

      /** @link aggregationByValue */
      TGo4GSIEventHeader fxGSIHeader;

      /** not used yet. */
      Short_t fsDummy;

      /** Trigger number */
      Short_t fsTrigger;

      Int_t fiCount;

   ClassDef(TGo4EventHeader10,1)
};

#endif // TGO4EVENTHEADER10_H
