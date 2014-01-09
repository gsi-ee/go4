#ifndef TGO4DABCSNIFFER_H
#define TGO4DABCSNIFFER_H

#include "TRootSniffer.h"

class TGo4Sniffer : public TRootSniffer {
   public:
      TGo4Sniffer(const char* name, Int_t comp);

      virtual ~TGo4Sniffer() {}

      virtual void ScanRoot(TRootSnifferScanRec& rec);
};


#endif
