#include "TGo4DabcPlayer.h"

#include "TFolder.h"
#include "TCollection.h"
#include "TTimer.h"
#include "Riostream.h"

#include "TGo4Sniffer.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4Ratemeter.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#include "dabc/Hierarchy.h"

#include "dabc/Factory.h"


class TGo4DabcFactory : public dabc::Factory {
   public:
      TGo4DabcFactory(const std::string& name) : dabc::Factory(name) {}

       virtual dabc::Reference CreateObject(const std::string& classname, const std::string& objname, dabc::Command cmd)
       {
          if (classname=="TGo4DabcPlayer")
             return new TGo4DabcPlayer(objname, cmd);

          return dabc::Factory::CreateObject(classname, objname, cmd);
       }
};


dabc::FactoryPlugin go4dabc_factory(new TGo4DabcFactory("go4"));

// ================================================================================


// Timer used to invoke commands in the main process

class TExecDabcCmdTimer : public TTimer {
   public:
      dabc::Command fCmd;
      TGo4DabcPlayer* fSniffer;

      TExecDabcCmdTimer(dabc::Command cmd, TGo4DabcPlayer* sniff) :
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

TGo4DabcPlayer::TGo4DabcPlayer(const std::string& name, dabc::Command cmd) :
   root::Player(name, cmd),
   TGo4AnalysisSniffer("dabc_sniffer")
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(this);

   // SetObjectSniffer(new TGo4Sniffer("go4_dabc", fCompression));

   TGo4Log::SetSniffer(this);
}

TGo4DabcPlayer::~TGo4DabcPlayer()
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(0);

   TGo4Log::SetSniffer(0);
}

void TGo4DabcPlayer::InitializeHierarchy()
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


void TGo4DabcPlayer::RatemeterUpdate(TGo4Ratemeter* r)
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

void TGo4DabcPlayer::StatusMessage(int level, const TString& msg)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   dabc::Hierarchy item = fHierarchy.FindChild("Status/Message");
   item.SetField("value", msg.Data());
   item.SetFieldModified("value");
   item.SetField("level", level);

   fHierarchy.FindChild("Status").MarkChangedItems();
}

void TGo4DabcPlayer::SetTitle(const char* title)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.FindChild("Status/DebugOutput").SetField("value", title ? title : "");

   fHierarchy.FindChild("Status").MarkChangedItems();
}


int TGo4DabcPlayer::ExecuteCommand(dabc::Command cmd)
{
   if (cmd.IsName("CmdClear") ||
       cmd.IsName("CmdStart") ||
       cmd.IsName("CmdStop")) { new TExecDabcCmdTimer(cmd, this); return dabc::cmd_postponed; }

   return root::Player::ExecuteCommand(cmd);
}

int TGo4DabcPlayer::ProcessGetBinary(TRootSniffer* sniff, dabc::Command cmd)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   // when working with the client, timer is processed in other thread and we need to lock main go4 mutex
   if (cli!=0) {
      TGo4LockGuard mainlock;
      return root::Player::ProcessGetBinary(sniff, cmd);
   } else {
      return root::Player::ProcessGetBinary(sniff, cmd);
   }
}

