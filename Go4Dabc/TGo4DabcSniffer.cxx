#include "TGo4DabcSniffer.h"

#include "TFolder.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4Ratemeter.h"

#include "dabc/Hierarchy.h"

TGo4DabcSniffer::TGo4DabcSniffer(const std::string& name, dabc::Command cmd) :
   dabc_root::RootSniffer(name, cmd),
   TGo4AnalysisSniffer("dabc_sniffer")
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(this);
}

TGo4DabcSniffer::~TGo4DabcSniffer()
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(0);
}

void TGo4DabcSniffer::InitializeHierarchy()
{
   dabc::Hierarchy sub = fHierarchy.CreateChild("Status");
   sub.SetPermanent();

   dabc::Hierarchy item = sub.CreateChild("EventRate");
   item.Field(dabc::prop_kind).SetStr("rate");
   item.EnableHistory(100,"value");
}


void* TGo4DabcSniffer::ScanRootHierarchy(dabc::Hierarchy& h, const char* searchpath)
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

void TGo4DabcSniffer::RatemeterUpdate(TGo4Ratemeter* r)
{
   dabc::LockGuard lock(fHierarchyMutex);

   fHierarchy.FindChild("Status/EventRate").Field("value").SetStr(dabc::format("%4.2f", r->GetRate()));
}
