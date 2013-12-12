#include "TGo4DabcSniffer.h"

#include "TFolder.h"
#include "TCollection.h"
#include "TTimer.h"
#include "Riostream.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4Ratemeter.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#include "dabc/Hierarchy.h"

// Timer used to invoke commands in the main process

class TExecDabcCmdTimer : public TTimer {
   public:
      dabc::Command fCmd;
      TGo4DabcSniffer* fSniffer;

      TExecDabcCmdTimer(dabc::Command cmd, TGo4DabcSniffer* sniff) :
         TTimer(0),
         fCmd(cmd),
         fSniffer(sniff)
      {
         Add();
      }

      virtual Bool_t Notify()
      {
         Remove();

         if (!fCmd.null()) {
            // DOUT0("MY COMMAND!!");

            TGo4Analysis* an = TGo4Analysis::Instance();

            if (fCmd.IsName("CmdClear")) {
               if (an) {
                  an->ClearObjects("Histograms");
                  fSniffer->StatusMessage(0, "Clear Histograms folder");
                  std::cout << "web: Clear Histograms folder" << std::endl;
               }
            } else
            if (fCmd.IsName("CmdStart")) {
               if (an) {
                  an->StartAnalysis();
                  fSniffer->StatusMessage(0, "Resume analysis loop");
                  std::cout << "web: Resume analysis loop" << std::endl;
               }
            } else
            if (fCmd.IsName("CmdStop")) {
               if (an) {
                  an->StopAnalysis();
                  fSniffer->StatusMessage(0, "Suspend analysis loop");
                  std::cout << "web: Suspend analysis loop" << std::endl;
               }
            }

            fCmd.ReplyBool(true);
         }

         delete this;

         return kFALSE;
      }
};

// =================================================================================

TGo4DabcSniffer::TGo4DabcSniffer(const std::string& name, dabc::Command cmd) :
   dabc_root::RootSniffer(name, cmd),
   TGo4AnalysisSniffer("dabc_sniffer")
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(this);

   TGo4Log::SetSniffer(this);
}

TGo4DabcSniffer::~TGo4DabcSniffer()
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(0);

   TGo4Log::SetSniffer(0);
}

void TGo4DabcSniffer::InitializeHierarchy()
{
   dabc::Hierarchy sub = fHierarchy.CreateChild("Status");
   sub.SetPermanent();

   sub.CreateChild("Message").SetField(dabc::prop_kind, "log");

   sub.CreateChild("EventRate").SetField(dabc::prop_kind, "rate");

   sub.CreateChild("AverRate").SetField(dabc::prop_kind, "rate");

   sub.CreateChild("EventCount").SetField(dabc::prop_kind, "log");
   sub.CreateChild("RunTime").SetField(dabc::prop_kind, "log");

   sub.CreateChild("DebugOutput").SetField(dabc::prop_kind, "log");

   sub.CreateChild("CmdClear").SetField(dabc::prop_kind, "DABC.Command");
   sub.CreateChild("CmdStart").SetField(dabc::prop_kind, "DABC.Command");
   sub.CreateChild("CmdStop").SetField(dabc::prop_kind, "DABC.Command");

   sub.EnableHistory(200, true);
}


void TGo4DabcSniffer::ScanRoot(ScanRec& rec)
{
   TGo4AnalysisObjectManager* om(0);
   TGo4AnalysisClient* cli(0);
   if (TGo4Analysis::Instance()) {
      om = TGo4Analysis::Instance()->ObjectManager();
      cli = TGo4Analysis::Instance()->GetAnalysisClient();
   }

   if (om==0) {
      dabc_root::RootSniffer::ScanRoot(rec);
      return;
   }

   rec.SetField(dabc::prop_kind, "GO4.Analysis");

   TFolder* main = om->GetObjectFolder();

/*   TGo4LockGuard mainlock;
   ScanCollection(rec, main->GetListOfFolders());
*/
   TFolder* hist_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcHISTFOLDER));
   TFolder* par_fold  = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcPARAFOLDER));
   TFolder* tree_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcTREEFOLDER));
   TFolder* canv_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcCANVFOLDER));
   TFolder* anal_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcANALYSISFOLDER));
   TFolder* even_fold = dynamic_cast<TFolder*> (anal_fold->FindObject(TGo4AnalysisObjectManager::fgcEVENTFOLDER));
   TFolder* user_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcUSRFOLDER));

   TGo4LockGuard mainlock;
   ScanCollection(rec, hist_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcHISTFOLDER);
   ScanCollection(rec, par_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcPARAFOLDER, mask_MarkExpand);
   ScanCollection(rec, tree_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcTREEFOLDER);
   ScanCollection(rec, canv_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcCANVFOLDER);
   ScanCollection(rec, even_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcEVENTFOLDER, mask_MarkExpand);
   ScanCollection(rec, user_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcUSRFOLDER);

//   printf("EVENTS folder %s %p %d \n", even_fold->GetName(), even_fold, even_fold->GetListOfFolders()->GetSize());
//   dabc::Hierarchy h = rec.top.FindChild("Events/MbsEvent101");
//   printf("MBS %p \n%s\n", h(), rec.top.SaveToXml().c_str());
}

void TGo4DabcSniffer::RatemeterUpdate(TGo4Ratemeter* r)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.FindChild("Status/EventRate").SetField("value", r->GetRate());
   fHierarchy.FindChild("Status/EventRate").SetFieldModified("value");

   fHierarchy.FindChild("Status/AverRate").SetField("value", r->GetAvRate());
   fHierarchy.FindChild("Status/AverRate").SetFieldModified("value");

   fHierarchy.FindChild("Status/EventCount").SetField("value", dabc::format("%lu", (long unsigned) r->GetCurrentCount()));

   fHierarchy.FindChild("Status/RunTime").SetField("value", dabc::format("%3.1f", r->GetTime()));

   fHierarchy.MarkChangedItems();
}

void TGo4DabcSniffer::StatusMessage(int level, const TString& msg)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   dabc::Hierarchy item = fHierarchy.FindChild("Status/Message");
   item.SetField("value", msg.Data());
   item.SetFieldModified("value");
   item.SetField("level", level);

   fHierarchy.FindChild("Status").MarkChangedItems();
}

void TGo4DabcSniffer::SetTitle(const char* title)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.FindChild("Status/DebugOutput").SetField("value", title ? title : "");

   fHierarchy.FindChild("Status").MarkChangedItems();
}


int TGo4DabcSniffer::ExecuteCommand(dabc::Command cmd)
{
   if (cmd.IsName("CmdClear") ||
       cmd.IsName("CmdStart") ||
       cmd.IsName("CmdStop")) { new TExecDabcCmdTimer(cmd, this); return dabc::cmd_postponed; }

   return dabc_root::RootSniffer::ExecuteCommand(cmd);
}

int TGo4DabcSniffer::ProcessGetBinary(dabc::Command cmd)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   // when working with the client, timer is processed in other thread and we need to lock main go4 mutex
   if (cli!=0) {
      TGo4LockGuard mainlock;
      return dabc_root::RootSniffer::ProcessGetBinary(cmd);
   } else {
      return dabc_root::RootSniffer::ProcessGetBinary(cmd);
   }
}

