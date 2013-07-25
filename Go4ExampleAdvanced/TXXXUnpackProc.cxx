// $Id: TXXXUnpackProc.cxx 478 2009-10-29 12:26:09Z linev $
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

#include "TXXXUnpackProc.h"
#include "TGo4EventEndException.h"

#include <time.h>

#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TLine.h"

#include "TGo4Log.h"
#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"

#include "TXXXParameter.h"
#include "TXXXUnpackEvent.h"
#include "TXXXCalibPar.h"

extern "C" {
   #include "s_filhe_swap.h"
   #include "s_bufhe_swap.h"
   #include "f_ut_utime.h"
}

//***********************************************************
TXXXUnpackProc::TXXXUnpackProc() :
   TGo4EventProcessor()
{
}
//***********************************************************
// this one is used in TXXXUnpackFact.cxx
TXXXUnpackProc::TXXXUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
   TGo4Log::Info("TXXXUnpackProc: Create %s", name);

   //// init user analysis objects:
   fParam1   = (TXXXParameter *)   GetParameter("XXXPar1");
   fParam2   = (TXXXParameter *)   GetParameter("XXXPar2");
   fParam1->PrintParameter(0,0);

   fParam2->PrintParameter(0,0);

   TGo4Log::Info("TXXXProc: Produce histograms");

   for(int i=0;i<8;i++) {
      fCr1Ch[i] = 0;
      fCr2Ch[i] = 0;
   }

   CreateRawHsitograms(5000, 1., 5001.);
   fEvCount = 0;

   fCr1Ch1x2 = MakeTH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 1., 5001., 200, 1., 5001.);
   fHis1 = MakeTH1('I', "His1","Condition histogram", 5000, 1., 5001.);
   fHis2 = MakeTH1('I', "His2","Condition histogram", 5000, 1., 5001.);
   fHis1gate = MakeTH1('I', "His1g","Gated histogram", 5000, 1., 5001.);
   fHis2gate = MakeTH1('I', "His2g","Gated histogram", 5000, 1., 5001.);

   TGo4Log::Info("TXXXProc: Produce conditions");
   fWinCon1 = MakeWinCond("wincon1", 50, 2000);
   fWinCon2 = MakeWinCond("wincon2", 50, 70, 90, 120);
   fconHis1 = MakeWinCond("cHis1", 100, 2000, "His1");
   fconHis2 = MakeWinCond("cHis2", 100, 2000, "His2");

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyCon1 = MakePolyCond("polycon", 3, cutpnts);

   fConArr1 = (TGo4CondArray*)GetAnalysisCondition("winconar");
   if (fConArr1==0) {
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
   if(fConArr2==0) {
      // This is example how to create condition array
      TGo4Log::Info("TXXXProc: Create condition");
      Double_t xvalues[4] = { 1000, 2000, 1500, 1000 };
      Double_t yvalues[4] = { 1000, 1000, 3000, 1000 };
      TCutG* mycut = new TCutG("cut2", 4, xvalues, yvalues);
      fConArr2 = new TGo4CondArray("polyconar",4,"TGo4PolyCond");
      fConArr2->SetValues(mycut);
      fConArr2->Disable(true);   // means: condition check always returns true
      delete mycut; // mycat has been copied into the conditions
      AddAnalysisCondition(fConArr2);
   } else {
      TGo4Log::Info("TXXXProc: Restore condition from autosave");
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
   if (fcondSet==0) {
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
      AddPicture(fcondSet);
   }

   fPicture1 = GetPicture("Picture1");
   if (fPicture1 == 0) {
      fPicture1 = new TGo4Picture("Picture1","Picture example");
      fPicture1->SetLinesDivision(3, 2,3,1);
      fPicture1->LPic(0,0)->AddObject(fCr1Ch[0]);
      fPicture1->LPic(0,0)->SetFillAtt(5, 3001); // pattern
      fPicture1->LPic(0,0)->SetLineAtt(5,1,1);
      fPicture1->LPic(0,1)->AddObject(fCr1Ch[1]);
      fPicture1->LPic(0,1)->SetFillAtt(4, 3001); // pattern
      fPicture1->LPic(0,1)->SetLineAtt(4,1,1);
      fPicture1->LPic(1,0)->AddObject(fCr1Ch[2]);
      fPicture1->LPic(1,0)->SetFillAtt(6, 1001); // solid
      fPicture1->LPic(1,0)->SetLineAtt(6,1,1);
      fPicture1->LPic(1,1)->AddObject(fCr1Ch[3]);
      fPicture1->LPic(1,1)->SetFillAtt(7, 1001); // solid
      fPicture1->LPic(1,1)->SetLineAtt(7,1,1);
      fPicture1->LPic(1,2)->AddObject(fCr1Ch[4]);
      fPicture1->LPic(3,0)->AddObject(fCr1Ch1x2);
      fPicture1->LPic(3,0)->SetDrawOption("CONT");
      fPicture1->LPic(3,0)->AddSpecialObject(new TArrow(3500., 3500, 2000., 1500., 0.02));
      fPicture1->LPic(3,0)->AddSpecialObject(new TLatex(1500, 3500, "Example of text label in the picture"));
//      fPicture1->LPic(3,0)->AddSpecialObject(new TLine(2500., 2500, 3500., 1500.));
      AddPicture(fPicture1);
   }
}
//***********************************************************
TXXXUnpackProc::~TXXXUnpackProc()
{
   fWinCon1->PrintCondition(true);
   fPolyCon1->PrintCondition(true);
}
//***********************************************************

//-----------------------------------------------------------
void TXXXUnpackProc::CreateRawHsitograms(int nbins, double xmin, double xmax)
{
   for(int i=0;i<8;i++) {

      if (fCr1Ch[i]!=0) {
         RemoveHistogram(fCr1Ch[i]->GetName());
         fCr1Ch[i] = 0;
      }

      if (fCr2Ch[i]!=0) {
         RemoveHistogram(fCr2Ch[i]->GetName());
         fCr2Ch[i] = 0;
      }

      fCr1Ch[i] = MakeTH1('I', Form("Crate1/Cr1Ch%02d",i+1), Form("Crate 1 channel %2d",i+1), nbins, xmin, xmax);
      fCr2Ch[i] = MakeTH1('I', Form("Crate2/Cr2Ch%02d",i+1), Form("Crate 2 channel %2d",i+1), nbins, xmin, xmax);
   }

   // change histogram pointer, used in parameter
   TXXXCalibPar* par = (TXXXCalibPar*) GetParameter("CaliPar");
   if (par!=0) par->SetCalibSpectrum(fCr1Ch[0]);


}

//-----------------------------------------------------------
Bool_t TXXXUnpackProc::BuildEvent(TGo4EventElement* dest)
{
   TGo4MbsEvent* inp_evt = (TGo4MbsEvent* ) GetInputEvent(); // from this
   TXXXUnpackEvent* out_evt = (TXXXUnpackEvent*) dest;

   if (inp_evt==0) {
      TGo4Log::Error("XXXUnpackProc: no input event !");
      return kFALSE;
   }

   fEvCount++;
   if (fEvCount % 1000000 == 0) {
      // this is demonstration how one can time to time recreate histogram with other ranges

      Long_t loop = (fEvCount / 1000000) % 5;

      CreateRawHsitograms(5000 + loop*200, 1. - loop*100, 5001 + loop*100);

      // TGo4Log::Info("Histograms recreated");
   }

   /////////////////////////////////////////////////////////////
   ////// use this if you want access to the mbs file header data:
//         s_filhe* head=inp_evt->GetMbsSourceHeader();
//         if(head)
//            {
//               std::cout <<"found filhe structure:" << std::endl;
//               std::cout <<"\tdatalen: "<<head->filhe_dlen << std::endl;
//               std::cout <<"\tfilename_l: "<<head->filhe_file_l << std::endl;
//               std::cout <<"\tfilename: "<<head->filhe_file << std::endl;
//               std::cout <<"\ttype: "<<head->filhe_type << std::endl;
//               std::cout <<"\tsubtype: "<<head->filhe_subtype << std::endl;
//               std::cout <<"\t#commentlines: "<<head->filhe_lines << std::endl;
//            }
//         else
//            {
//               std::cout <<"zero file header" << std::endl;
//            }
   //////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////
   ////// use this if you want access to the mbs buffer header data:
//         s_bufhe* head=inp_evt->GetMbsBufferHeader();
//         if(head) {
//               std::cout <<"\nfound bufhe structure:" << std::endl;
//               std::cout <<"\tbuffernumber: "<<head->l_buf << std::endl;
//               std::cout <<"\tdatalen: "<<head->l_dlen << std::endl;
//               std::cout <<"\ttime lo: "<<head->l_time[0] << std::endl; // seconds since epoch 1970
//               std::cout <<"\ttime hi: "<<head->l_time[1] << std::endl; // microseconds since time lo
//               char sbuf[1000]; f_ut_utime(head->l_time[0], head->l_time[1], sbuf);
//               std::cout <<"\ttimestring: " << sbuf << std::endl;
//               std::cout <<"\ttype: "<<head->i_type << std::endl;
//               std::cout <<"\tsubtype: "<<head->i_subtype << std::endl;
//            }
//         else
//            {
//               std::cout <<"zero buffer header" << std::endl;
//
//            }
   //////////////////////////////////////////////////////////////////


   inp_evt->ResetIterator();
   TGo4MbsSubEvent* psubevt(0);
   while ((psubevt = inp_evt->NextSubEvent()) != 0) // subevent loop
   {
	 Int_t* pdata = psubevt->GetDataField();
	 Int_t lwords = psubevt->GetIntLen();
	 Int_t subcrate=psubevt->GetSubcrate();
	 if(subcrate<0 || subcrate> XXX_NUM_CRATES)
	 	 {
	       TGo4Log::Info("XXXUnpackProc: skip invalid subcrate %d", subcrate);
		 	 continue; // try next subevent if any
	 	 }

	 if(lwords >= 8) lwords=8; // take only first 8 lwords
	 Int_t lastvalue = 0;
	 for(Int_t i = 0; i<lwords; ++i)
	 {
		// Int_t index =  *pdata&0xfff;      // in case low word is index
		// Int_t value = (*pdata>>16)&0xfff; // in case high word is data


		   // first copy data of crate and channel into corresponding module:
		   TXXXUnpackEvent& ev=*out_evt; // ref instead pointer for array syntax:
		   TXXXModule* mod=dynamic_cast<TXXXModule*>(&ev[subcrate][i]); // 2d array with composite event operator[]
		   if(mod)
		   {
			   if(*pdata != 0)
				   mod->SetData(*pdata); // zero suppressed data
			   else
				   mod->SetAux(i); // account count zero value data channel for each subevent

			   mod->SetTest(*pdata); // record any data here
		   }
			else
			{
			   TGo4Log::Info("XXXUnpackProc: WARNING: no output event module for crate %d, channel %d", subcrate, i);

			}
		   if(*pdata != 0)
		   		{
				   // process crate specific histograming here:
				   if( subcrate == 1)
						{
						   fCr1Ch[i]->Fill((Float_t)(*pdata));

						   if(i == 0) // fill first channel
						   {
							  if(fconHis1->Test(*pdata))fHis1gate->Fill((Float_t)(*pdata));
							  fHis1->Fill((Float_t)(*pdata));
						   }
						   if(i == 1)
						   {
							  if(fconHis2->Test(*pdata))fHis2gate->Fill((Float_t)(*pdata));
							  fHis2->Fill((Float_t)(*pdata));
							  // fill Cr1Ch1x2 for three polygons:
							  if(fPolyCon1->Test(*pdata,lastvalue))       fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)lastvalue);
							  if(((*fConArr2)[0])->Test(*pdata,lastvalue))fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)lastvalue);
							  if(((*fConArr2)[1])->Test(*pdata,lastvalue))fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)lastvalue);
						   }
						   lastvalue = *pdata; // save for 2d histogram
						}
				   else if ( subcrate == 2)
						{
							fCr2Ch[i]->Fill((Float_t)(*pdata));
						}
		   		}
		pdata++;
	 }// for SEW LW
   }  // while
   out_evt->SetValid(kTRUE); // to store
   // throwing this exception stops the event loop
   // Note that subsequent steps are not executed!
   //   throw TGo4EventEndException(this);

   return kTRUE;
}
