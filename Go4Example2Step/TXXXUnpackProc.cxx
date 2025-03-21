// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXUnpackProc.h"

#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TCutG.h"
#include "TMath.h"
#include "TLatex.h"

#include "TGo4Log.h"
#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4ShapedCond.h"
#include "TGo4ListCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"
#include "TGo4RollingGraph.h"

#include "TXXXParameter.h"
#include "TXXXUnpackEvent.h"

//-----------------------------------------------------------
TXXXUnpackProc::TXXXUnpackProc() :
   TGo4EventProcessor()
{
   for (int n = 0; n < XXX_NUM_CHAN; n++) {
      fCr1Ch[n] = nullptr;
      fCr2Ch[n] = nullptr;
   }
}

//-----------------------------------------------------------
TXXXUnpackProc::TXXXUnpackProc(const char *name) :
   TGo4EventProcessor(name)
{
   for (int n = 0; n < XXX_NUM_CHAN; n++) {
      fCr1Ch[n] = nullptr;
      fCr2Ch[n] = nullptr;
   }

   TGo4Log::Info("TXXXUnpackProc: Create");

   //// init user analysis objects:
   // create and load parameter here
   // set_Par.C macro executed after parameter load from auto-save file
   fParam = (TXXXParameter*) MakeParameter("XXXParameter", "TXXXParameter", "set_Par.C");

   // this one is created in TXXXAnalysis, because it is used in both steps
   fWinCon1 = (TGo4WinCond *) GetAnalysisCondition("wincon1", "TGo4WinCond");

   if(fParam->fbHisto){

      TGo4Log::Info("TXXXUnpackProc: Produce histograms");

      for (int i = 0; i < XXX_NUM_CHAN; i++) {
         TString hname = TString::Format("Crate1/Cr1Ch%02d", i + 1);
         TString htitle = TString::Format("Crate 1 channel %2d", i+1);
         fCr1Ch[i] = MakeTH1('I', hname.Data(), htitle.Data(), 5000, 1., 5001.);
         hname = TString::Format("Crate2/Cr2Ch%02d",i+1);
         htitle = TString::Format("Crate 2 channel %2d",i+1);
         fCr2Ch[i] = MakeTH1('I', hname.Data(), htitle.Data(), 5000, 1., 5001.);
      }

      fCr1Ch1x2 = MakeTH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 1., 5001., 200, 1., 5001.);
      fHis1 = MakeTH1('I', "His1","Condition histogram", 5000, 1., 5001.);
      fHis2 = MakeTH1('I', "His2","Condition histogram", 5000, 1., 5001.);
      fHis1gate = MakeTH1('I', "His1g","Gated histogram", 5000, 1., 5001.);
      fHis2gate = MakeTH1('I', "His2g","Gated histogram", 5000, 1., 5001.);


      fRollingGraph1 = MakeRollingGraph("myrollinggraphs/rolling1","Test of a rolling graph 1",10000,10);
      fRollingGraph2 = MakeRollingGraph("myrollinggraphs/rolling2","Test of a rolling graph 2",1000,100);
      fGraph = MakeGraph("sinus","graph from a function",new TF1("sinus","sin(x)",0, 4*TMath::Pi()));

      TGo4Log::Info("TXXXUnpackProc: Produce conditions");
      // fWinCon1->PrintCondition(true);

      fWinCon2 = MakeWinCond("wincon2", 50, 70, 90, 120);
      fconHis1 = MakeWinCond("cHis1", 100, 2000, "His1");
      fconHis2 = MakeWinCond("cHis2", 100, 2000, "His2");

      Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
      fPolyCon1 = MakePolyCond("polycon", 3, cutpnts, "Cr1Ch1x2");

      fEllipseCond = MakeEllipseCond("ellipsecond",0,3000,3000,300,800, 45, "Cr1Ch1x2");
                              //Int_t npoints, Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta, const char *HistoName

      fCircleCond = MakeCircleCond("circlecond",24,300,200,100, "Cr1Ch1x2");
                          //Int_t npoints, Double_t cx, Double_t cy, Double_t r, const char *HistoName

      fBoxCond = MakeBoxCond("boxcond",2000,2000,400,100, 30, "Cr1Ch1x2");

      fFreestyleCond = MakeFreeShapeCond("freecon", 3, cutpnts, "Cr1Ch1x2");

      fWhitelistCon = MakeListCond("Whitelist",1,42,2,"His1");

      fConArr1 = (TGo4CondArray*)GetAnalysisCondition("winconar");
      if (!fConArr1) {
         fConArr1 = new TGo4CondArray("winconar",30,"TGo4WinCond");
         fConArr1->SetValues(100,500);
         fConArr1->Disable(true);
         ((*fConArr1)[0])->SetValues(200,400);
         ((*fConArr1)[1])->SetValues(700,1000);
         ((*fConArr1)[2])->SetValues(1500,2000);
         fConArr1->SetHistogram("Sum3");
         AddAnalysisCondition(fConArr1);
      } else {
         fConArr1->ResetCounts();
      }

      fConArr2 = (TGo4CondArray*)GetAnalysisCondition("polyconar");
      if(!fConArr2) {
         // This is example how to create condition array
         TGo4Log::Info("TXXXUnpackProc: Create condition polyconar");
         Double_t xvalues[4] = { 1000, 2000, 1500, 1000 };
         Double_t yvalues[4] = { 1000, 1000, 3000, 1000 };
         TCutG *mycut = new TCutG("cut2", 4, xvalues, yvalues);
         fConArr2 = new TGo4CondArray("polyconar",4,"TGo4PolyCond");
         fConArr2->SetValues(mycut);
         fConArr2->Disable(true);   // means: condition check always returns true
         delete mycut; // mycat has been copied into the conditions
         AddAnalysisCondition(fConArr2);
      } else {
         TGo4Log::Info("TXXXUnpackProc: Restore condition polyconar from autosave");
         fConArr2->ResetCounts();
      }

      // connect histograms to conditions. will be drawn when condition is edited.
      fWinCon1->Enable();
      fWinCon2->Disable(true); // return always true
      fWinCon2->Invert(kTRUE);
      fWinCon1->PrintCondition(true);
      fconHis1->PrintCondition(true);
      fconHis2->PrintCondition(true);
      fPolyCon1->Enable();
      fPolyCon1->PrintCondition(true);
      ((*fConArr2)[0])->Enable();
      ((*fConArr2)[1])->Enable(); // 2 and 3 remain disabled

      fcondSet = GetPicture("condSet");
      if (!fcondSet) {
         // in the upper two pads, the condition limits can be set,
         // in the lower two pads, the resulting histograms are shown
         fcondSet = new TGo4Picture("condSet","Set conditions");
         fcondSet->SetDivision(2,2);
         fcondSet->Pic(0,0)->AddObject(fHis1);
         fcondSet->Pic(0,1)->AddObject(fHis2);
         fcondSet->Pic(0,0)->AddCondition(fconHis1);
         fcondSet->Pic(0,1)->AddCondition(fconHis2);
         fcondSet->Pic(1,0)->AddObject(fHis1gate);
         fcondSet->Pic(1,1)->AddObject(fHis2gate);
         fcondSet->Pic(1,0)->SetFillAtt(4, 1001); // solid
         fcondSet->Pic(1,0)->SetLineAtt(4,1,1);
         fcondSet->Pic(1,1)->SetFillAtt(9, 1001); // solid
         fcondSet->Pic(1,1)->SetLineAtt(9,1,1);
         fcondSet->Pic(0,0)->SetTitleAttr(0.05, 0.85, 0.8, 0.95);

         /** this is example how arbitrary objects can be add to
          * the picture and than displayed in the gui */

//       TF1 *fa1 = new TF1("fa1","1000*sin(x)/x",0,1000);
//       fcondSet->Pic(0,0)->AddSpecialObject(fa1, "same");

//       TArc* arc = new TArc(0,0, 1000);
//       fcondSet->Pic(0,1)->AddSpecialObject(arc);

         AddPicture(fcondSet);
      }

      fLaText = new TLatex(0.5,0.5,"-- demo text --");
      fLaText->SetName("LatexObjectDemo");
      fLaText->SetTextAlign(22);
      fLaText->SetNDC();
      AddObject(fLaText); // will replace old one of same name

      fPicture1 = GetPicture("Picture1");
      if (!fPicture1) {
         fPicture1 = new TGo4Picture("Picture1","Picture example");
         fPicture1->SetLinesDivision(3, 2,3,1); // three rows, cols per row
         // top row
         fPicture1->LPic(0,0)->AddObject(fCr1Ch[0]);
         fPicture1->LPic(0,0)->SetFillAtt(5, 3001); // pattern
         fPicture1->LPic(0,0)->SetLineAtt(5,1,1);
         fPicture1->LPic(0,0)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(0,0)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(0,0)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(0,0)->SetHisTitle(false);
         fPicture1->LPic(0,0)->SetTitleAttr(0.1,0.75,0.7,0.9);

         fPicture1->LPic(0,0)->AddObject(fLaText);

         fPicture1->LPic(0,1)->AddObject(fCr1Ch[1]);
         fPicture1->LPic(0,1)->SetFillAtt(4, 3001); // pattern
         fPicture1->LPic(0,1)->SetLineAtt(4,1,1);
         fPicture1->LPic(0,1)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(0,1)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(0,1)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(0,1)->SetHisTitle(false);
         fPicture1->LPic(0,1)->SetTitleAttr(0.1,0.75,0.7,0.9);
         // middle row
         fPicture1->LPic(1,0)->AddObject(fCr1Ch[2]);
         fPicture1->LPic(1,0)->SetFillAtt(6, 1001); // solid
         fPicture1->LPic(1,0)->SetLineAtt(6,1,1);
         fPicture1->LPic(1,0)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(1,0)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,0)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,0)->SetHisTitle(false);
         fPicture1->LPic(1,0)->SetTitleAttr(0.1,0.75,0.7,0.9);

         fPicture1->LPic(1,1)->AddObject(fCr1Ch[3]);
         fPicture1->LPic(1,1)->SetFillAtt(7, 1001); // solid
         fPicture1->LPic(1,1)->SetLineAtt(7,1,1);
         fPicture1->LPic(1,1)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(1,1)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,1)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,1)->SetHisTitle(false);
         fPicture1->LPic(1,1)->SetTitleAttr(0.1,0.75,0.7,0.9);

         fPicture1->LPic(1,2)->AddObject(fCr1Ch[4]);
         fPicture1->LPic(1,2)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(1,2)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,2)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(1,2)->SetHisTitle(false);
         fPicture1->LPic(1,2)->SetTitleAttr(0.1,0.75,0.7,0.9);
         // bottom row
         fPicture1->LPic(2,0)->AddObject(fCr1Ch1x2);
         fPicture1->LPic(2,0)->SetDrawOption("CONT");
         fPicture1->LPic(2,0)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
         fPicture1->LPic(2,0)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(2,0)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
         fPicture1->LPic(2,0)->SetHisTitle(false);
         fPicture1->LPic(2,0)->SetTitleAttr(0.1,0.75,0.7,0.9);
         AddPicture(fPicture1);
      }
   } // create histograms

   //ExecuteLine("$blabla.py"); // JAM test internal python call
   //ExecutePython("blabla.py");

}

//-----------------------------------------------------------
TXXXUnpackProc::~TXXXUnpackProc()
{
   TGo4Log::Info("TXXXUnpackProc: Delete");
   if(fParam->fbHisto) {
      fWinCon1->PrintCondition(true);
      fPolyCon1->PrintCondition(true);
   }
}

//-----------------------------------------------------------
Bool_t TXXXUnpackProc::BuildEvent(TGo4EventElement *dest)
{
   Bool_t isValid = kFALSE; // validity of output event

   auto inp_evt = static_cast<TGo4MbsEvent *>(GetInputEvent()); // from this
   auto out_evt = static_cast<TXXXUnpackEvent *>(dest);

   if (!inp_evt) {
      TGo4Log::Error("XXXUnpackProc: no input event !");
      out_evt->SetValid(isValid); // to store or not to store
      // default calling Fill method will set validity of out_evt to return value!
      return isValid;
   }
   isValid = kTRUE;

	////////////////////////////////////////////////////
	// Some examples how to skip event processing or stop analysis by exception
	// for convenience, we provide  GO4_ macros to invoke appropriate exception throws
	// NOTE: You need to //#include "TGo4UserException.h" for this feature
   //	 static UInt_t count = 0;
	//	 if((count++ % 100000) == 0 && count > 1) // user may put a real analysis condition here
	//		 {
	//			 // this macro will skip event and subsequent analysis steps and send specified message to gui log window:
	//			 // GO4_SKIP_EVENT_MESSAGE("Skipped Event %d",count-1)
	//
	//			 // this macro will skip event and subsequent analysis steps without message:
	//			 GO4_SKIP_EVENT
	//
	//			// this macro will stop analysis and send specified message to gui log window:
	//			 //GO4_STOP_ANALYSIS_MESSAGE("Stopped after Event %d",count-1)
	//
	//			// this macro will stop analysis without message
	//			//GO4_STOP_ANALYSIS
	//		 }
	////////////////////////////////////////////////////


   /////////////////////////////////////////////////////////////
   ////// use this if you want access to the mbs file header data:
   //      s_filhe* head=inp_evt->GetMbsSourceHeader();
   //      if(head)
   //         {
   //            std::cout <<"found filhe structure:" << std::endl;
   //            std::cout <<"\tdatalen: "<<head->filhe_dlen << std::endl;
   //            std::cout <<"\tfilename_l: "<<head->filhe_file_l << std::endl;
   //            std::cout <<"\tfilename: "<<head->filhe_file << std::endl;
   //            std::cout <<"\ttype: "<<head->filhe_type << std::endl;
   //            std::cout <<"\tsubtype: "<<head->filhe_subtype << std::endl;
   //            std::cout <<"\t#commentlines: "<<head->filhe_lines << std::endl;
   //         }
   //      else
   //         {
   //            std::cout <<"zero file header" << std::endl;
   //         }
   //////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////
   ////// use this if you want access to the mbs buffer header data:
   //      s_bufhe* head=inp_evt->GetMbsBufferHeader();
   //      if(head) {
   //            std::cout <<"\nfound bufhe structure:" << std::endl;
   //            std::cout <<"\tbuffernumber: "<<head->l_buf << std::endl;
   //            std::cout <<"\tdatalen: "<<head->l_dlen << std::endl;
   //            std::cout <<"\ttime lo: "<<head->l_time[0] << std::endl; // seconds since epoch 1970
   //            std::cout <<"\ttime hi: "<<head->l_time[1] << std::endl; // microseconds since time lo
   //            char sbuf[1000]; f_ut_utime(head->l_time[0], head->l_time[1], sbuf);
   //            std::cout <<"\ttimestring: " << sbuf << std::endl;
   //            std::cout <<"\ttype: "<<head->i_type << std::endl;
   //            std::cout <<"\tsubtype: "<<head->i_subtype << std::endl;
   //         }
   //      else
   //         {
   //            std::cout <<"zero buffer header" << std::endl;
   //         }
   //////////////////////////////////////////////////////////////////


   inp_evt->ResetIterator();
   while (auto psubevt = inp_evt->NextSubEvent()) // subevent loop
   {
      if( psubevt->GetSubcrate() == 1)
      {
         Int_t *pdata = psubevt->GetDataField();
         Int_t lwords = psubevt->GetIntLen();
         if(lwords > XXX_NUM_CHAN) lwords = XXX_NUM_CHAN; // take only first 8 lwords
         Int_t lastvalue = 0;
         for(Int_t i = 0; i<lwords; ++i)
         {
            // Int_t index =  *pdata&0xfff;      // in case low word is index
            // Int_t value = (*pdata>>16)&0xfff; // in case high word is data
            if(*pdata != 0)
            {
               out_evt->fiCrate1[i] = *pdata; // fill output event

               if (fCr1Ch[i]) fCr1Ch[i]->Fill(*pdata);

               if(fParam->fbHisto) { // fill histograms
                  if(i == 0) // fill first channel
                  {
                     if(fconHis1->Test((Double_t) *pdata)) fHis1gate->Fill(*pdata);
                     fHis1->Fill(*pdata);
                     fWhitelistCon->Test(*pdata);
                     fRollingGraph1->Fill(*pdata);
                     fRollingGraph2->Fill(*pdata);
                  }
                  if(i == 1)
                  {
                     if(fconHis2->Test((Double_t) *pdata)) fHis2gate->Fill(*pdata);
                     fHis2->Fill(*pdata);
                     // fill Cr1Ch1x2 for three polygons:
                     if(fPolyCon1->Test(*pdata,lastvalue))        fCr1Ch1x2->Fill(*pdata, lastvalue);
                     if(((*fConArr2)[0])->Test(*pdata,lastvalue)) fCr1Ch1x2->Fill(*pdata, lastvalue);
                     if(((*fConArr2)[1])->Test(*pdata,lastvalue)) fCr1Ch1x2->Fill(*pdata, lastvalue);

                     if(fEllipseCond->Test(*pdata,lastvalue)) fCr1Ch1x2->Fill(*pdata, lastvalue);
                     fCircleCond->Test(*pdata,lastvalue);
                  }
               }
            }
            lastvalue = *pdata; // save for 2d histogram
            pdata++;
         } // for SEW LW
      } // if (subcrate)
      if( psubevt->GetSubcrate() == 2)
      {
         Int_t *pdata = psubevt->GetDataField();
         Int_t lwords = psubevt->GetIntLen();
         if(lwords > XXX_NUM_CHAN) lwords=XXX_NUM_CHAN;
         for(Int_t i = 0; i<lwords; ++i) {
            if(*pdata != 0) {
               out_evt->fiCrate2[i] = *pdata;
               if(fCr2Ch[i]) fCr2Ch[i]->Fill((Float_t)(*pdata));
            }
            pdata++;
         } // for SEW LW
      } // if (subcrate)
   }  // while

   if (fLaText) {
      TString lbl = TString::Format("#scale[3.0]{#color[2]{Event number:%d}}",inp_evt->GetCount());
      fLaText->SetText(0.5,0.5, lbl.Data());
   }

   out_evt->SetValid(isValid); // to store or not to store
   // default calling Fill method will set validity of out_evt to return value!

   return isValid; // this will overwrite out_evt->SetValid
   // except one would have a Fill method implemented in the output event class!
   // In this case the return value can be handled differently there.
}
