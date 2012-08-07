

#define HAD_TIME_CHANNELS 32
#define HAD_TIME_NUMTDC 4
#define HAD_TIME_NUMBOARDS 10
#define HAD_TIME_NUMMCP  4

void set_HadaqCond()
{
#ifndef __GO4ANAMACRO__
   cout << "Macro set_HadaqCondd can execute only in analysis" << endl;
   return;
#endif
   TGo4Log::Info("Set conditions from set_HadaqCond.C");
   TString obname;
   for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b) {
      if (!THadaqUnpackEvent::AssertTRB(b))
         continue;
      for (int t = 0; t < HAD_TIME_NUMTDC; ++t) {
         obname.Form("LeadingDeltaCalTimeGate_%02d_%02d", b, t);
         TGo4WinCond* leadconny = (TGo4WinCond*) go4->GetAnalysisCondition(obname.Data(), "TGo4WinCond");
         if (leadconny == 0) {
            TGo4Log::Error("Could not find condition %s", obname.Data());
            continue;
         }

         leadconny->SetValues(106500, 107500); // reasonable init for mainz beam?
         TGo4Log::Info("Set condition %s to (%f,%f)", obname.Data(),leadconny->GetXLow(), leadconny->GetXUp());
         obname.Form("TrailingDeltaCalTimeGate_%02d_%02d", b, t);
         TGo4WinCond* trailconny = (TGo4WinCond*) go4->GetAnalysisCondition(obname.Data(), "TGo4WinCond");
         if (trailconny == 0) {
            TGo4Log::Error("Could not find condition %s", obname.Data());
            continue;
         }
         trailconny->SetValues(106500, 107500); // reasonable init for mainz beam?
         TGo4Log::Info("Set condition %s to (%f,%f)", obname.Data(),trailconny->GetXLow(), trailconny->GetXUp());

         // DEBUG PRINT CONDITIONS BELOW:
         obname.Form("LeadingDeltaDebugTimeGate_%02d_%02d", b, t);
         TGo4WinCond* dbgleadconny = (TGo4WinCond*) go4->GetAnalysisCondition(
               obname.Data(), "TGo4WinCond");
         if (dbgleadconny == 0) {
            TGo4Log::Error("Could not find condition %s", obname.Data());
            continue;
         }

         dbgleadconny->SetValues(106500, 107500); // reasonable init for mainz beam?
         TGo4Log::Info("Set condition %s to (%f,%f)", obname.Data(),
               dbgleadconny->GetXLow(), dbgleadconny->GetXUp());
         obname.Form("TrailingDeltaDebugTimeGate_%02d_%02d", b, t);
         TGo4WinCond* dbgtrailconny = (TGo4WinCond*) go4->GetAnalysisCondition(
               obname.Data(), "TGo4WinCond");
         if (dbgtrailconny == 0) {
            TGo4Log::Error("Could not find condition %s", obname.Data());
            continue;
         }
         dbgtrailconny->SetValues(106500, 107500); // reasonable init for mainz beam?
         dbgtrailconny->Disable(kFALSE); // to enable/disable debug output
         TGo4Log::Info("Set condition %s to (%f,%f)", obname.Data(),
               dbgtrailconny->GetXLow(), dbgtrailconny->GetXUp());





      }

   }


}
