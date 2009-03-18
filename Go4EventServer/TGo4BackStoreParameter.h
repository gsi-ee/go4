#ifndef TGO4BACKSTOREPARAMETER_H
#define TGO4BACKSTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

class TGo4BackStoreParameter : public TGo4EventStoreParameter {

   public:
      TGo4BackStoreParameter() ;

      TGo4BackStoreParameter(const char* name,
                              Int_t splitlevel=99,
                              Int_t bufsize=32000);

      virtual ~TGo4BackStoreParameter() ;

      /** basic method to printout status information
        * on stdout; to be overridden by specific subclass  */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

      Int_t GetSplitlevel() const { return fiSplit; }
      void SetSplitlevel(Int_t split) { fiSplit=split; }

      Int_t GetBufsize() const { return fiBufsize; }
      void SetBufsize(Int_t bufsize) { fiBufsize=bufsize; }

   private:

      /** Splitlevel for Tree. Default is 1. */
      Int_t fiSplit;

      /** Size of the root io buffer. */
      Int_t fiBufsize;

   ClassDef(TGo4BackStoreParameter,1)
};

#endif //TGO4BACKSTOREPARAMETER_H
