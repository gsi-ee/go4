// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
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
#include "TGo4AnalysisClientImp.h"
#include "TGo4Ratemeter.h"
#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"

#include "dabc/Hierarchy.h"

#include "dabc/Factory.h"
#include "dabc/Command.h"


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
   sub.SetField("_status","GO4.DrawAnalysisStatus");

   sub.CreateHChild("State").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("Message").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("EventRate").SetField(dabc::prop_kind, "rate");

   sub.CreateHChild("AverRate").SetField(dabc::prop_kind, "rate");

   sub.CreateHChild("EventCount").SetField(dabc::prop_kind, "log");
   sub.CreateHChild("RunTime").SetField(dabc::prop_kind, "log");

   sub.CreateHChild("DebugOutput").SetField(dabc::prop_kind, "log");

   dabc::Hierarchy cmd = sub.CreateHChild("CmdClear");
   cmd.SetField(dabc::prop_kind, "DABC.Command");
   cmd.SetField("_fastcmd","/go4sys/icons/clear.png");
   cmd = sub.CreateHChild("CmdStart");
   cmd.SetField(dabc::prop_kind, "DABC.Command");
   cmd.SetField("_fastcmd","/go4sys/icons/start.png");
   cmd = sub.CreateHChild("CmdStop");
   cmd.SetField(dabc::prop_kind, "DABC.Command");
   cmd.SetField("_fastcmd","/go4sys/icons/Stop.png");
   cmd = sub.CreateHChild("CmdRestart");
   cmd.SetField(dabc::prop_kind, "DABC.Command");
   cmd.SetField("_fastcmd","/go4sys/icons/restart.png");

   // JAM add here special commands to set/get object values: DEPRECATED- to be removed
//   cmd = sub.CreateHChild("CmdSetCondition");
//   cmd.SetField(dabc::prop_kind, "DABC.Command");
//   cmd = sub.CreateHChild("CmdGetCondition"); // maybe we dont need this?
//   cmd.SetField(dabc::prop_kind, "DABC.Command");
//   cmd = sub.CreateHChild("CmdSetParameter");
//   cmd.SetField(dabc::prop_kind, "DABC.Command");
//   cmd = sub.CreateHChild("CmdGetParameter");
//   cmd.SetField(dabc::prop_kind, "DABC.Command");



   sub.EnableHistory(200, true);
}


void TGo4DabcPlayer::RatemeterUpdate(TGo4Ratemeter* r)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   bool running = an ? an->IsRunning() : false;

   dabc::LockGuard lock(fHierarchy.GetHMutex());

   fHierarchy.GetHChild("Status/State").SetField("value", running ? "Running" : "Stopped");

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

bool TGo4DabcPlayer::ProcessHCommand(const std::string& cmdname, dabc::Command com)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   if (cmdname == "Status/CmdClear") {
      if (an) {
         an->ClearObjects("Histograms");
         an->ClearObjects("Conditions");
         StatusMessage(0, "Clear Histograms and Conditions folder");
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

   if (cmdname == "Status/CmdRestart")
  {
    if (cli != 0)
    {
      cli->Stop();
      an->CloseAnalysis();
      an->InitEventClasses();
      cli->Start();
    }
    else if (an)
    {
      an->StopAnalysis();
      an->PostLoop();
      an->CloseAnalysis();
      an->InitEventClasses();
      an->PreLoop();
      an->StartAnalysis();
    }

    StatusMessage(0, "Restart analysis loop");
    std::cout << "web: Restart analysis loop" << std::endl;

    return true;
  }

////////// alternative implementation, deprecated (to be removed)
//  if (cmdname == "Status/CmdSetCondition")
//  {
//    if (an)
//    {
//      TString message;
//      TString name = com.GetStr("name", "");
//      message.Form("Set Condition %s:", name.Data());
//
//      // we do not stream all condition internals, just modes that can be set from editor
//      // these will be used to invoke corresponding methods
//      // negative defaults to decide if url actually has delivered changed values
//      Int_t resultmode = com.GetInt("resultmode", -1);
//      Int_t invertmode = com.GetInt("invertmode", -1);
////               fbTrue    = cond->TGo4Condition::IsTrue();
////               fbFalse   = cond->TGo4Condition::IsFalse();
////               fbResult  = cond->TGo4Condition::FixedResult();
////               fbEnabled = cond->TGo4Condition::IsEnabled();
//
//// DO NOT SUPPORT               fbMarkReset = cond->fbMarkReset;
//
//      Int_t visible = com.GetInt("visible", -1);
////               fbVisible=cond->TGo4Condition::IsVisible();
//      Int_t labeldraw = com.GetInt("labeldraw", -1);
////               fbLabelDraw=cond->TGo4Condition::IsLabelDraw();
//
//      Int_t limitsdraw = com.GetInt("limitsdraw", -1);
//// DO NOT SUPPORT               fbMultiEdit=cond->TGo4Condition::IsMultiEdit();
//
//      Int_t integraldraw = com.GetInt("intdraw", -1);
////               fbIntDraw= cond->TGo4Condition::IsIntDraw();
//      Int_t xmeandraw = com.GetInt("xmeandraw", -1);
////               fbXMeanDraw= cond->TGo4Condition::IsXMeanDraw();
//      Int_t xrmsdraw = com.GetInt("xrmsdraw", -1);
////               fbXRMSDraw= cond->TGo4Condition::IsXRMSDraw();
//      Int_t ymeandraw = com.GetInt("ymeandraw", -1);
////               fbYMeanDraw= cond->TGo4Condition::IsYMeanDraw();
//      Int_t yrmsdraw = com.GetInt("yrmsdraw", -1);
////               fbYRMSDraw= cond->TGo4Condition::IsYRMSDraw();
//      Int_t xmaxdraw = com.GetInt("xmaxdraw", -1);
////               fbXMaxDraw= cond->TGo4Condition::IsXMaxDraw();
//      Int_t ymaxdraw = com.GetInt("ymaxdraw", -1);
////               fbYMaxDraw= cond->TGo4Condition::IsYMaxDraw();
//      Int_t cmaxdraw = com.GetInt("cmaxdraw", -1);
////               fbCMaxDraw= cond->TGo4Condition::IsCMaxDraw();
//
//      Int_t resetcounts = com.GetInt("resetcounts", -1);
//
////  DO NOT SUPPORT             fbHistogramLink=cond->TGo4Condition::IsHistogramLink();
////   DO NOT SUPPORT            fxHistoName=cond->fxHistoName;
////  DO NOT SUPPORT             fdUpdateEpsilon= cond->fdUpdateEpsilon;
////   DO NOT SUPPORT            fiDim=cond->GetDimension();
//
////               if(counts){
////   DO NOT SUPPORT              fiCounts = cond->TGo4Condition::Counts();
////   DO NOT SUPPORT              fiTrueCounts = cond->TGo4Condition::TrueCounts();
//
////                 fbMarkReset = false;
////               }
////               if(fbMarkReset){
////                  ResetCounts();
////                  fbMarkReset = false;
////               }
//// From TGo4Condition UpdateFrom
//
//      TGo4Condition* cond = an->GetAnalysisCondition(name.Data());
//      if (cond == 0)
//      {
//        message.Append(TString::Format("Condition not found!"));
//      }
//      else
//      {
//        // first set base class properties if delivered by url:
//
//        if (resultmode >= 0)
//        {
//          // same as in Go4 GUI condition editor:
//          switch (resultmode)
//          {
//            case 0:
//              cond->Enable();
//              break;
//            case 1:
//              cond->Disable(kTRUE);
//              break;
//            case 2:
//              cond->Disable(kFALSE);
//              break;
//            default:
//              cond->Enable();
//              break;
//          };
//        }
//
//        if (invertmode >= 0)
//        {
//          // same as in Go4 GUI condition editor:
//          cond->Invert(invertmode == 1);
//        }
//
////        cond->SetLabelDraw(on); break;
////     case 1: cond->SetLimitsDraw(on); break;
////     case 2: cond->SetIntDraw(on);  break;
////     case 3: cond->SetXMeanDraw(on);  break;
////     case 4: cond->SetXRMSDraw(on);  break;
////     case 5: cond->SetYRMSDraw(on);  break;
////     case 6: cond->SetYMeanDraw(on);  break;
////     case 7: cond->SetXMaxDraw(on);  break;
////     case 8: cond->SetYMaxDraw(on);  break;
////     case 9: cond->SetCMaxDraw(on);  break;
////     case 10: cond->SetVisible(on); break;
//
//        if (visible >= 0)
//        {
//          cond->SetVisible(visible == 1);
//          message.Append(TString::Format(", visible=%d", visible));
//        }
//        if (labeldraw >= 0)
//        {
//          cond->SetLabelDraw(labeldraw == 1);
//          message.Append(TString::Format(", labeldraw=%d", labeldraw));
//        }
//        if (limitsdraw >= 0)
//                {
//                  cond->SetLimitsDraw(limitsdraw == 1);
//                  message.Append(TString::Format(", limitsdraw=%d", limitsdraw));
//                }
//        if (integraldraw >= 0)
//        {
//          cond->SetIntDraw(integraldraw == 1);
//          message.Append(TString::Format(", intdraw=%d", integraldraw));
//        }
//        if (xmeandraw >= 0)
//        {
//          cond->SetXMeanDraw(xmeandraw == 1);
//          message.Append(TString::Format(", xmeandraw=%d", xmeandraw));
//        }
//        if (xrmsdraw >= 0)
//        {
//          cond->SetXRMSDraw(xrmsdraw == 1);
//          message.Append(TString::Format(", xrmsdraw=%d", xrmsdraw));
//        }
//        if (ymeandraw >= 0)
//        {
//          cond->SetYMeanDraw(ymeandraw == 1);
//          message.Append(TString::Format(", ymeandraw=%d", ymeandraw));
//        }
//        if (yrmsdraw >= 0)
//        {
//          cond->SetYRMSDraw(yrmsdraw == 1);
//          message.Append(TString::Format(", yrmsdraw=%d", yrmsdraw));
//        }
//        if (xmaxdraw >= 0)
//        {
//          cond->SetXMaxDraw(xmaxdraw == 1);
//          message.Append(TString::Format(", xmaxdraw=%d", xmaxdraw));
//        }
//        if (ymaxdraw >= 0)
//        {
//          cond->SetYMaxDraw(ymaxdraw == 1);
//          message.Append(TString::Format(", ymaxdraw=%d", ymaxdraw));
//        }
//        if (cmaxdraw >= 0)
//        {
//          cond->SetCMaxDraw(cmaxdraw == 1);
//          message.Append(TString::Format(", cmaxdraw=%d", cmaxdraw));
//        }
//
//        // evaluate subclass properties:
//        TGo4WinCond* winny = dynamic_cast<TGo4WinCond*>(cond);
//        TGo4PolyCond* polly = dynamic_cast<TGo4PolyCond*>(cond);
//        if (winny)
//        {
//
//          if (com.HasField("xmin"))
//          {
//            Double_t xmin = com.GetDouble("xmin", winny->GetXLow());
//            Double_t xmax = com.GetDouble("xmax", winny->GetXUp());
//            Double_t ymin = com.GetDouble("ymin", winny->GetYLow());
//            Double_t ymax = com.GetDouble("ymax", winny->GetYUp());
//            std::cout << "web: Setting condition " << name.Data() << "to xmin=" << xmin << ", xmax=" << xmax;
//            std::cout << ", ymin=" << ymin << ", ymax=" << ymax << std::endl;
//
//            Int_t dim = winny->GetDimension();
//            switch (dim)
//            {
//              case 1:
//                winny->SetValues(xmin, xmax);
//                message.Append(TString::Format(", set limits to (%f, %f)", xmin, xmax));
//                break;
//              case 2:
//                winny->SetValues(xmin, xmax, ymin, ymax);
//                message.Append(TString::Format(", set limits to (%f, %f) (%f, %f)", xmin, xmax, ymin, ymax));
//                break;
//              default:
//                message.Append(TString::Format(" !wrong condition dimension %d, NEVER COME HERE", dim));
//                break;
//            };
//
//          }
//          else
//          {
//            std::cout <<"DEBUG- no limits to change received" << std::endl;
//          }
//
//        }
//        else if (polly)
//        {
//          message.Append(TString::Format(" - setting Polygon condition Points not yet supported!"));
//        }
//        else
//        {
//          message.Append(TString::Format(" ! not suported type: %s!", cond->ClassName()));
//        }
//
//      }
//
//      StatusMessage(0, message);
//    }    // if analysis
//
//    return true;
//  }    //CmdSetCondition


   return false;
}

