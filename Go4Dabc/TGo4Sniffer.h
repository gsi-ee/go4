#ifndef TGO4SNIFFER_H
#define TGO4SNIFFER_H

#include "dabc_root/RootSniffer.h"

class TGo4Sniffer : public dabc_root::RootSniffer {

   protected:

   public:
      TGo4Sniffer(const std::string& name, dabc::Command cmd = 0);

      virtual ~TGo4Sniffer();

      virtual const char* ClassName() const { return "TGo4Sniffer"; }

      virtual void* ScanRootHierarchy(dabc::Hierarchy& h, const char* searchpath = 0);
};

#endif
