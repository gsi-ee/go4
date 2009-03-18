#ifndef TGO4MEMBERSTATUS_H
#define TGO4MEMBERSTATUS_H

#include "TGo4ObjectStatus.h"

class TGo4MemberStatus : public TGo4ObjectStatus {
   public:
      TGo4MemberStatus();

      /**
       * Create status information about event datamemeber.
       * For reconstruction of eventstructure for display in remote browser.
       */
      TGo4MemberStatus(const char* name, const char* datatypename);

      virtual ~TGo4MemberStatus();

      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);


   ClassDef(TGo4MemberStatus,1)
};

#endif //TGO4MEMBERSTATUS_H
