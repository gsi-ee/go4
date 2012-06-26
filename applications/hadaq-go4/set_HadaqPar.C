

#define HAD_TIME_CHANNELS 32
#define HAD_TIME_NUMTDC 4
#define HAD_TIME_NUMBOARDS 10
#define HAD_TIME_NUMMCP  4

void set_HadaqPar()
{
#ifndef __GO4ANAMACRO__
   cout << "Macro set_HadaqPar can execute only in analysis" << endl;
   return;
#endif
   THadaqParam* param0 = (THadaqParam*) go4->GetParameter("HadaqPar");

   if (param0==0) {
      TGo4Log::Error("Could not find parameter HadaqPar of class THadaqParam");
      return;
   }

   TGo4Log::Info("Set parameter HadaqPar (pointer %x) from set_HadaqPar.C",param0);

   param0->fillRaw = kTRUE;
   param0->printEvent = kFALSE;
   param0->generalCalibration = kTRUE;
   param0->continuousCalibration = kFALSE;
   param0->calibrationPeriod = 10000;

//   for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b) {
//       if (!THadaqUnpackEvent::AssertTRB(b))
//          continue;
//       for (int t = 0; t < HAD_TIME_NUMTDC; ++t) {
//          for (int i = 0; i < HAD_TIME_CHANNELS; ++i) {
//             param0->deltaTDC[b][t][i] = t; // by default, channels refer to self tdc
//             param0->deltaTRB[b][t][i] = b; // by default, channels refer to self trb
//             param0->deltaChannels[b][t][i] = 0; // reference channel is 0
//          }
//       }
//    }
//
//
//    // here mcp mappings (JAM 1-June2012):
//   Int_t trbids[HAD_TIME_NUMMCP]={3,5,8,9}; // assign mcps 0..3 to trbs
//
//    for (Int_t mcp = 0; mcp < HAD_TIME_NUMMCP; ++mcp) {
//       Int_t trb = trbids[mcp];
//       if (!THadaqUnpackEvent::AssertTRB(trb))
//              continue;
//       for (Int_t tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc) {
//          Int_t row = 1;
//          Int_t deltarow = +1;
//          Int_t col = 2 * (tdc + 1);
//          for (Int_t ch = 0; ch < HAD_TIME_CHANNELS; ++ch) {
//             if (ch == 0 || (ch % 2) != 0)
//                continue;
//             param0->imageMCP[trb][tdc][ch] = mcp;
//             param0->imageRow[trb][tdc][ch] = row;
//             param0->imageCol[trb][tdc][ch] = col;
//             printf(" **** Mapped trb %d, tdc %d, ch %d to mcp %d row %d col %d \n",trb,tdc,ch,mcp,row,col);
//             printf(" **** array content is: mcp %d row %d col %d \n",
//                   param0->imageMCP[trb][tdc][ch],
//                   param0->imageRow[trb][tdc][ch],
//                   param0->imageCol[trb][tdc][ch]);
//
//             row += deltarow;
//             if (row > 8) {
//                row = 8;
//                deltarow = -1;
//                col -= 1;
//             }
//          }
//       }
//    } // mcp

   // condition block:
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
         TGo4Log::Info("Set condition %s", obname.Data());
         obname.Form("TrailingDeltaCalTimeGate_%02d_%02d", b, t);
         TGo4WinCond* trailconny = (TGo4WinCond*) go4->GetAnalysisCondition(obname.Data(), "TGo4WinCond");
         if (trailconny == 0) {
            TGo4Log::Error("Could not find condition %s", obname.Data());
            continue;
         }
         trailconny->SetValues(106500, 107500); // reasonable init for mainz beam?
         TGo4Log::Info("Set condition %s", obname.Data());
      }

   }


}
