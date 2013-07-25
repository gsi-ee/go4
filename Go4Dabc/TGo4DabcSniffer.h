#ifndef TGO4DABCSNIFFER_H
#define TGO4DABCSNIFFER_H

#include "dabc_root/RootSniffer.h"

#include "TGo4AnalysisSniffer.h"


class TGo4DabcSniffer : public dabc_root::RootSniffer,
                    public TGo4AnalysisSniffer {

   protected:

      virtual void InitializeHierarchy();

   public:
      TGo4DabcSniffer(const std::string& name, dabc::Command cmd = 0);

      virtual ~TGo4DabcSniffer();

      virtual const char* ClassName() const { return "TGo4DabcSniffer"; }

      virtual void* ScanRootHierarchy(dabc::Hierarchy& h, const char* searchpath = 0);

      virtual void RatemeterUpdate(TGo4Ratemeter*);

};

#endif
