// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------


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


TGo4DabcPlayer::TGo4DabcPlayer(const std::string& name, dabc::Command cmd) :
   root::Monitor(name, cmd),
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
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   dabc::Hierarchy sub = fHierarchy.CreateHChild("Status");
   sub.SetPermanent();

   sub.CreateHChild("Message").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("EventRate").SetField(dabc::prop_kind, "rate");

   sub.CreateHChild("AverRate").SetField(dabc::prop_kind, "rate");

   sub.CreateHChild("EventCount").SetField(dabc::prop_kind, "log");
   sub.CreateHChild("RunTime").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("DebugOutput").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("CmdClear").SetField(dabc::prop_kind, "DABC.Command");
   sub.CreateHChild("CmdStart").SetField(dabc::prop_kind, "DABC.Command");
   sub.CreateHChild("CmdStop").SetField(dabc::prop_kind, "DABC.Command");

   sub.EnableHistory(200, true);
}


void TGo4DabcPlayer::RatemeterUpdate(TGo4Ratemeter* r)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.GetHChild("Status/EventRate").SetField("value", r->GetRate());
   fHierarchy.GetHChild("Status/EventRate").SetFieldModified("value");

   fHierarchy.GetHChild("Status/AverRate").SetField("value", r->GetAvRate());
   fHierarchy.GetHChild("Status/AverRate").SetFieldModified("value");

   fHierarchy.GetHChild("Status/EventCount").SetField("value", dabc::format("%lu", (long unsigned) r->GetCurrentCount()));

   fHierarchy.GetHChild("Status/RunTime").SetField("value", dabc::format("%3.1f", r->GetTime()));

   fHierarchy.MarkChangedItems();
}

void TGo4DabcPlayer::StatusMessage(int level, const TString& msg)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   dabc::Hierarchy item = fHierarchy.GetHChild("Status/Message");
   item.SetField("value", msg.Data());
   item.SetFieldModified("value");
   item.SetField("level", level);

   fHierarchy.GetHChild("Status").MarkChangedItems();
}

void TGo4DabcPlayer::SetTitle(const char* title)
{
   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.GetHChild("Status/DebugOutput").SetField("value", title ? title : "");

   fHierarchy.GetHChild("Status").MarkChangedItems();
}


int TGo4DabcPlayer::ProcessGetBinary(TRootSniffer* sniff, dabc::Command cmd)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   // when working with the client, timer is processed in other thread and we need to lock main go4 mutex
   if (cli!=0) {
      TGo4LockGuard mainlock;
      return root::Monitor::ProcessGetBinary(sniff, cmd);
   } else {
      return root::Monitor::ProcessGetBinary(sniff, cmd);
   }
}

bool TGo4DabcPlayer::ProcessHCommand(const std::string& cmdname, dabc::Command)
{
   TGo4Analysis* an = TGo4Analysis::Instance();

   if (cmdname == "Status/CmdClear") {
      if (an) {
         an->ClearObjects("Histograms");
         StatusMessage(0, "Clear Histograms folder");
         std::cout << "web: Clear Histograms folder" << std::endl;
      }
      return true;
   }
   if (cmdname == "Status/CmdStart") {
      if (an) {
         an->StartAnalysis();
         StatusMessage(0, "Resume analysis loop");
         std::cout << "web: Resume analysis loop" << std::endl;
      }
      return true;
   }
   if (cmdname == "Status/CmdStop") {
      if (an) {
         an->StopAnalysis();
         StatusMessage(0, "Suspend analysis loop");
         std::cout << "web: Suspend analysis loop" << std::endl;
      }
      return true;
   }
   return false;
}

