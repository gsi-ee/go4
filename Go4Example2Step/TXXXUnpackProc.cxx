#include "TXXXUnpackProc.h"

#include "Riostream.h"
#include <time.h>

#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "snprintf.h"

#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"

#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"

#include "TXXXParameter.h"
#include "TXXXUnpackEvent.h"

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
  cout << "**** TXXXUnpackProc: Create" << endl;

  Text_t chis[16];
  Text_t chead[64];
  Int_t i;

  //// init user analysis objects:

  fParam1   = (TXXXParameter *)   GetParameter("XXXPar1");
  fParam2   = (TXXXParameter *)   GetParameter("XXXPar2");
  fParam1->PrintParameter(0,0);
  fParam2->PrintParameter(0,0);

  // Creation of histograms:
  if(GetHistogram("Crate1/Cr1Ch01")==0)
    {
      for(i =0;i<8;i++)
   {
     snprintf(chis,15,"Cr1Ch%02d",i+1);
     snprintf(chead,63,"Crate 1 channel %2d",i+1);
          fCr1Ch[i] = new TH1I (chis,chead,5000,1,5000);
          AddHistogram(fCr1Ch[i],"Crate1");
     snprintf(chis,15,"Cr2Ch%02d",i+1);
     snprintf(chead,63,"Crate 2 channel %2d",i+1);
          fCr2Ch[i] = new TH1I (chis,chead,5000,1,5000);
          AddHistogram(fCr2Ch[i],"Crate2");
   }
      fCr1Ch1x2 = new TH2I("Cr1Ch1x2","Crate 1 channel 1x2",200,1,5000,200,1,5000);
      AddHistogram(fCr1Ch1x2);
      fHis1 = new TH1I ("His1","Condition histogram",5000,1,5000);
      AddHistogram(fHis1);
      fHis2 = new TH1I ("His2","Condition histogram",5000,1,5000);
      AddHistogram(fHis2);
      fHis1gate = new TH1I ("His1g","Gated histogram",5000,1,5000);
      AddHistogram(fHis1gate);
      fHis2gate = new TH1I ("His2g","Gated histogram",5000,1,5000);
      AddHistogram(fHis2gate);
    }
  else // got them from autosave file
    {
      for(i =0;i<8;i++)
   {
     snprintf(chis,15,"Crate1/Cr1Ch%02d",i+1);
          fCr1Ch[i]=(TH1I*)GetHistogram(chis);
     snprintf(chis,15,"Crate2/Cr2Ch%02d",i+1);
          fCr2Ch[i]=(TH1I*)GetHistogram(chis);
   }
      fCr1Ch1x2=(TH2I*)GetHistogram("Cr1Ch1x2");
      fHis1=(TH1I*)GetHistogram("His1");
      fHis2=(TH1I*)GetHistogram("His2");
      fHis1gate=(TH1I*)GetHistogram("His1g");
      fHis2gate=(TH1I*)GetHistogram("His2g");
      cout << "Unpack: Restored histograms from autosave" << endl;
    }
  // Creation of conditions:
  if(GetAnalysisCondition("wincon1")==0)
    {
      fWinCon1= new TGo4WinCond("wincon1");
      fWinCon1->SetValues(50,2000);
      fWinCon1->Disable(true); // return always true

      fWinCon2= new TGo4WinCond("wincon2");
      fWinCon2->SetValues(50,70,90,120);
      fWinCon2->Disable(true);
      fWinCon2->Invert(kTRUE);

      fconHis1= new TGo4WinCond("cHis1");
      fconHis2= new TGo4WinCond("cHis2");
      fconHis1->SetValues(100,2000);
      fconHis2->SetValues(100,2000);

      fConArr1= new TGo4CondArray("winconar",30,"TGo4WinCond");
      fConArr1->SetValues(100,500);
      fConArr1->Disable(true);
      ((*fConArr1)[0])->SetValues(200,400);
      ((*fConArr1)[1])->SetValues(700,1000);
      ((*fConArr1)[2])->SetValues(1500,2000);
      fConArr1->SetHistogram("Sum3");

      Double_t xvalues[4]={400,700,600,400};
      Double_t yvalues[4]={800,900,1100,800};
      TCutG* mycut= new TCutG("cut1",4,xvalues,yvalues);
      fPolyCon1= new TGo4PolyCond("polycon");
      fPolyCon1->SetValues(mycut); // copies mycat into fPolyCon1
      fPolyCon1->Disable(true);
      delete mycut; // mycat has been copied into the conditions

      xvalues[0]=1000;xvalues[1]=2000;xvalues[2]=1500;xvalues[3]=1000;
      yvalues[0]=1000;yvalues[1]=1000;yvalues[2]=3000;yvalues[3]=1000;
      mycut= new TCutG("cut2",4,xvalues,yvalues);
      fConArr2= new TGo4CondArray("polyconar",5,"TGo4PolyCond");
      fConArr2->SetValues(mycut);
      fConArr2->Disable(true);
      delete mycut; // mycat has been copied into the conditions

      AddAnalysisCondition(fWinCon1);
      AddAnalysisCondition(fWinCon2);
      AddAnalysisCondition(fPolyCon1);
      AddAnalysisCondition(fConArr1);
      AddAnalysisCondition(fConArr2);
      AddAnalysisCondition(fconHis1);
      AddAnalysisCondition(fconHis2);
    }
  else // got them from autosave file
    {
      fWinCon1  = (TGo4WinCond*)  GetAnalysisCondition("wincon1");
      fWinCon2  = (TGo4WinCond*)  GetAnalysisCondition("wincon2");
      fPolyCon1 = (TGo4PolyCond*) GetAnalysisCondition("polycon");
      fConArr1  = (TGo4CondArray*)GetAnalysisCondition("winconar");
      fConArr2  = (TGo4CondArray*)GetAnalysisCondition("polyconar");
      fconHis1  = (TGo4WinCond*)  GetAnalysisCondition("cHis1");
      fconHis2  = (TGo4WinCond*)  GetAnalysisCondition("cHis2");
      fconHis1->ResetCounts();
      fconHis2->ResetCounts();
      fWinCon1->ResetCounts();
      fWinCon2->ResetCounts();
      fPolyCon1->ResetCounts();
      fConArr1->ResetCounts();
      fConArr2->ResetCounts();

      cout << "Unpack: Restored conditions from autosave" << endl;
    }
  // connect histograms to conditions. will be drawn when condition is edited.
  fconHis1->SetHistogram("His1");
  fconHis2->SetHistogram("His2");
  fconHis1->Enable();
  fconHis2->Enable();
  fconHis1->PrintCondition(true);
  fconHis2->PrintCondition(true);

  if (GetPicture("Picture1")==0)
    {
      fcondSet = new TGo4Picture("condSet","Set conditions");
      fcondSet->SetDrawHeader(kTRUE);
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

      Picture1 = new TGo4Picture("Picture1","Picture example");
      Picture1->SetLinesDivision(3, 2,3,1);
      Picture1->LPic(0,0)->AddObject(fCr1Ch[0]);
      Picture1->LPic(0,0)->SetFillAtt(5, 3001); // pattern
      Picture1->LPic(0,0)->SetLineAtt(5,1,1);
      Picture1->LPic(0,1)->AddObject(fCr1Ch[1]);
      Picture1->LPic(0,1)->SetFillAtt(4, 3001); // pattern
      Picture1->LPic(0,1)->SetLineAtt(4,1,1);
      Picture1->LPic(1,0)->AddObject(fCr1Ch[2]);
      Picture1->LPic(1,0)->SetFillAtt(6, 1001); // solid
      Picture1->LPic(1,0)->SetLineAtt(6,1,1);
      Picture1->LPic(1,1)->AddObject(fCr1Ch[3]);
      Picture1->LPic(1,1)->SetFillAtt(7, 1001); // solid
      Picture1->LPic(1,1)->SetLineAtt(7,1,1);
      Picture1->LPic(1,2)->AddObject(fCr1Ch[4]);
      Picture1->LPic(3,0)->AddObject(fCr1Ch1x2);
      Picture1->LPic(3,0)->SetDrawOption("CONT");

      AddPicture(Picture1);

    }
  else
    {
      Picture1 = GetPicture("Picture1");
      fcondSet = (TGo4Picture *)GetPicture("condSet");
    }

  fWinCon1->Enable();
  fWinCon1->PrintCondition(true);
  fPolyCon1->Enable();
  fPolyCon1->PrintCondition(true);
  ((*fConArr2)[0])->Enable();
  ((*fConArr2)[1])->Enable();
}
//***********************************************************
TXXXUnpackProc::~TXXXUnpackProc()
{
  fWinCon1->PrintCondition(true);
  fPolyCon1->PrintCondition(true);
}
//***********************************************************

//-----------------------------------------------------------
void TXXXUnpackProc::XXXUnpack(TXXXUnpackEvent* poutevt)
{
  TGo4MbsSubEvent* psubevt;
  Int_t index=0;
  Int_t value=0;
  Int_t lwords;
  Int_t *pdata;
  fInput    = (TGo4MbsEvent* ) GetInputEvent(); // from this
  if(fInput)
    {
      /////////////////////////////////////////////////////////////
      ////// use this if you want access to the mbs file header data:
      //      s_filhe* head=fInput->GetMbsSourceHeader();
      //      if(head)
      //         {
      //            cout <<"found filhe structure:" << endl;
      //            cout <<"\tdatalen: "<<head->filhe_dlen << endl;
      //            cout <<"\tfilename_l: "<<head->filhe_file_l << endl;
      //            cout <<"\tfilename: "<<head->filhe_file << endl;
      //            cout <<"\ttype: "<<head->filhe_type << endl;
      //            cout <<"\tsubtype: "<<head->filhe_subtype << endl;
      //            cout <<"\t#commentlines: "<<head->filhe_lines << endl;
      //         }
      //      else
      //         {
      //            cout <<"zero file header" << endl;
      //         }
      //////////////////////////////////////////////////////////////////

      /////////////////////////////////////////////////////////////
      ////// use this if you want access to the mbs buffer header data:
      //      s_bufhe* head=fInput->GetMbsBufferHeader();
      //      if(head)
      //         {
      //            cout <<"\nfound bufhe structure:" << endl;
      //            cout <<"\tbuffernumber: "<<head->l_buf << endl;
      //            cout <<"\tdatalen: "<<head->l_dlen << endl;
      //            cout <<"\ttime lo: "<<head->l_time[0] << endl; // seconds since epoch 1970
      //            cout <<"\ttime hi: "<<head->l_time[1] << endl; // microseconds since time lo
      //            cout <<"\ttimestring: "<<ctime((const time_t*) &(head->l_time[0]));
      //            cout << "\t\t + "<<head->l_time[1] << " µs"<<endl;
      //            cout <<"\ttype: "<<head->i_type << endl;
      //            cout <<"\tsubtype: "<<head->i_subtype << endl;
      //         }
      //      else
      //         {
      //            cout <<"zero buffer header" << endl;
      //         }
      //////////////////////////////////////////////////////////////////

      fInput->ResetIterator();
      while ((psubevt = fInput->NextSubEvent()) != 0) // subevent loop
   {
     if( psubevt->GetSubcrate() == 1)
       {
         pdata=psubevt->GetDataField();
         lwords= psubevt->GetIntLen();
         if(lwords >= 8) lwords=8; // take only first 8 lwords
         for(Int_t i = 0; i<lwords; ++i)
      {
        index =  *pdata&0xfff;      // in case low word is index
        //value = (*pdata>>16)&0xfff; // in case high word is data
        if(*pdata != 0)
          {
            fCr1Ch[i]->Fill((Float_t)(*pdata));
            poutevt->fiCrate1[i] = *pdata; // fill output event
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
                if(fPolyCon1->Test(*pdata,value))       fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)value);
                if(((*fConArr2)[0])->Test(*pdata,value))fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)value);
                if(((*fConArr2)[1])->Test(*pdata,value))fCr1Ch1x2->Fill((Float_t)(*pdata),(Float_t)value);
         }
          }
        value = *pdata; // save for 2d histogram
        pdata++;
      } // for SEW LW
       } // if (subcrate)
     if( psubevt->GetSubcrate() == 2)
       {
         pdata=psubevt->GetDataField();
         lwords= (psubevt->GetDlen() -2) * sizeof(Short_t)/sizeof(Int_t);
         if(lwords >= 8) lwords=8;
         for(Int_t i = 0; i<lwords; ++i)
      {
        index=*pdata&0xfff;
        value=(*pdata>>16)&0xfff;
        if(*pdata != 0)
          {
            poutevt->fiCrate2[i] = *pdata;
            fCr2Ch[i]->Fill((Float_t)(*pdata));
          }
        pdata++;
      } // for SEW LW
       } // if (subcrate)
   }  // while
      poutevt->SetValid(kTRUE); // to store
    } // if(fInput)
  else    cout << "XXXUnpackProc: no input event !"<< endl;
}
