#include "TGo4Sniffer.h"

#include "TFolder.h"

#include "TGo4Analysis.h"
#include "TGo4AnalysisObjectManager.h"

TGo4Sniffer::TGo4Sniffer(const std::string& name, dabc::Command cmd) :
   dabc_root::RootSniffer(name, cmd)
{
}

TGo4Sniffer::~TGo4Sniffer()
{
}

void* TGo4Sniffer::ScanRootHierarchy(dabc::Hierarchy& h, const char* searchpath)
{
   TGo4AnalysisObjectManager* om(0);
   if (TGo4Analysis::Instance())
      om = TGo4Analysis::Instance()->ObjectManager();

   if (om==0) return dabc_root::RootSniffer::ScanRootHierarchy(h, searchpath);

   void *res = 0;
   if (h.null()) return res;

   if (searchpath==0) h.Field(dabc::prop_kind).SetStr("GO4.Analysis");

   if (!res) res = ScanListHierarchy(h, searchpath, om->GetObjectFolder()->GetListOfFolders(), 0);

   return res;
}
