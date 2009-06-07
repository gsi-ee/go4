#include "TGo4EventProcessor.h"

#include <snprintf.h>
#include <string.h>

#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "TArrayD.h"

#include "TGo4Log.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4EventElement.h"
#include "TGo4EventCalibration.h"
#include "TGo4AnalysisImp.h"

TGo4EventProcessor::TGo4EventProcessor(const char* name)
: TGo4EventSource(name), fxInputEvent(0), fxCalibration(0)
{
TRACE((15,"TGo4EventProcessor::TGo4EventProcessor()",__LINE__, __FILE__));
}

TGo4EventProcessor::~TGo4EventProcessor()
{
TRACE((15,"TGo4EventProcessor::~TGo4EventProcessor()",__LINE__, __FILE__));
}

Bool_t TGo4EventProcessor::CheckInputEvent(const char* classname)
{
   TRACE((12,"TGo4EventProcessor::CheckInputEvent()",__LINE__, __FILE__));
   if(fxInputEvent==0)
      {
         return kFALSE;
      }
   else
      {
         if(!strcmp(fxInputEvent->ClassName(),classname))
            {
               return kTRUE;
            }
         else
            {
               return kFALSE;
            }

     }

}

Bool_t TGo4EventProcessor::CheckCalibration(const char* classname)
{
   TRACE((12,"TGo4EventProcessor::CheckCalibration()",__LINE__, __FILE__));
   if(fxCalibration==0)
      {
         return kFALSE;
      }
   else
      {
         if(!strcmp(fxCalibration->ClassName(),classname))
            {
               return kTRUE;
            }
         else
            {
               return kFALSE;
            }
     }
}

Bool_t TGo4EventProcessor::AddObject(TNamed* any, const char* subfolder)
{
   return (TGo4Analysis::Instance()->AddObject(any,subfolder));
}

Bool_t TGo4EventProcessor::AddHistogram(TH1* his, const char* subfolder, Bool_t replace)
{
   return (TGo4Analysis::Instance()->AddHistogram(his,subfolder,replace));
}

Bool_t TGo4EventProcessor::AddParameter(TGo4Parameter* par,const char* subfolder)
{
  return (TGo4Analysis::Instance()->AddParameter(par,subfolder));
}

Bool_t TGo4EventProcessor::AddPicture(TGo4Picture* pic,const char* subfolder)
{
  return (TGo4Analysis::Instance()->AddPicture(pic,subfolder));
}

Bool_t TGo4EventProcessor::AddCanvas(TCanvas* can, const char* subfolder)
{
  return (TGo4Analysis::Instance()->AddCanvas(can,subfolder));
}

Bool_t  TGo4EventProcessor::AddAnalysisCondition(TGo4Condition* con,const char* subfolder)
{
  return (TGo4Analysis::Instance()->AddAnalysisCondition(con,subfolder));
}

Bool_t TGo4EventProcessor::RemoveHistogram(const char* name)
{
  return (TGo4Analysis::Instance()->RemoveHistogram(name));
}

Bool_t TGo4EventProcessor::RemoveParameter(const char* name)
{
  return (TGo4Analysis::Instance()->RemoveParameter(name));
}

Bool_t TGo4EventProcessor::RemovePicture(const char* name)
{
  return (TGo4Analysis::Instance()->RemovePicture(name));
}

Bool_t TGo4EventProcessor::RemoveCanvas(const char* name)
{
  return (TGo4Analysis::Instance()->RemoveCanvas(name));
}

Bool_t TGo4EventProcessor::RemoveAnalysisCondition(const char* name)
{
  return (TGo4Analysis::Instance()->RemoveAnalysisCondition(name));
}

TNamed* TGo4EventProcessor::GetObject(const char* name, const char* folder)
{
   return (TGo4Analysis::Instance()->GetObject(name,folder));
}


TH1* TGo4EventProcessor::GetHistogram(const char* name)
{
   return (TGo4Analysis::Instance()->GetHistogram(name));
}

TGo4Parameter* TGo4EventProcessor::GetParameter(const char* name)
{
   return (TGo4Analysis::Instance()->GetParameter(name));
}

TGo4Picture* TGo4EventProcessor::GetPicture(const char* name)
{
   return (TGo4Analysis::Instance()->GetPicture(name));
}

TCanvas* TGo4EventProcessor::GetCanvas(const char* name)
{
   return (TGo4Analysis::Instance()->GetCanvas(name));
}

TGo4Condition*  TGo4EventProcessor::GetAnalysisCondition(const char* name)
{
   return (TGo4Analysis::Instance()->GetAnalysisCondition(name));
}

TGo4EventElement* TGo4EventProcessor::GetInputEvent(const char* stepname)
{
   return (TGo4Analysis::Instance()->GetInputEvent(stepname) );
}

TGo4EventElement* TGo4EventProcessor::GetOutputEvent(const char* stepname)
{
   return (TGo4Analysis::Instance()->GetOutputEvent(stepname) );
}

void TGo4EventProcessor::Message(Int_t prio, const char* text,...)
{
   char txtbuf[__MESSAGETEXTLENGTH__];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, __MESSAGETEXTLENGTH__, text, args);
   va_end(args);
   TGo4Analysis::Instance()->Message(prio,txtbuf);
}

void TGo4EventProcessor::SendObjectToGUI(TNamed* ob)
{
   return (TGo4Analysis::Instance()->SendObjectToGUI(ob ) );
}


void TGo4EventProcessor::Clear(Option_t* opt)
{
 cout <<"Default Clear of eventprocessor "<< GetName() << endl;
 // dummy clear, may be implemented by user
}


TH1* TGo4EventProcessor::MakeH1(char type, const char* fullname, const char* title,
                                     Int_t nbinsx, Double_t xlow, Double_t xup,
                                     const char* xtitle, const char* ytitle)
{
   TString foldername, histoname;

   if ((fullname==0) || (strlen(fullname)==0)) {
      cout << "Histogram name not specified, can be a hard error" << endl;
      return 0;
   }
   const char* separ = strrchr(fullname, '/');
   if (separ!=0) {
      histoname = separ + 1;
      foldername.Append(fullname, fullname - separ - 1);
   } else
      histoname = fullname;

   int itype = 0;
   const char* stype = "TH1I";
   switch (type) {
      case 'I': case 'i': itype = 0; stype = "TH1I"; break;
      case 'F': case 'f': itype = 1; stype = "TH1F"; break;
      case 'D': case 'd': itype = 2; stype = "TH1D"; break;
      case 'S': case 's': itype = 3; stype = "TH1S"; break;
      case 'C': case 'c': itype = 4; stype = "TH1C"; break;
      default:
         cout << "There is no histogram with type:" << type << ", use I instead" << endl;
   }

   TH1* oldh = GetHistogram(fullname);

   if (oldh!=0) {
      if (oldh->InheritsFrom(stype) && fbMakeWithAutosave) {
         if (title) oldh->SetTitle(title);
         if (xtitle) oldh->GetXaxis()->SetTitle(xtitle);
         if (ytitle) oldh->GetYaxis()->SetTitle(ytitle);
         return oldh;
      }
      cout << "There is histogram " << fullname << " with type " << oldh->ClassName() << " other than specified "
           << stype << " rebuild" << endl;
   }

   TH1* newh = 0;

   switch (itype) {
      case 0: newh = new TH1I(histoname, title, nbinsx, xlow, xup); break;
      case 1: newh = new TH1F(histoname, title, nbinsx, xlow, xup); break;
      case 2: newh = new TH1D(histoname, title, nbinsx, xlow, xup); break;
      case 3: newh = new TH1S(histoname, title, nbinsx, xlow, xup); break;
      case 4: newh = new TH1C(histoname, title, nbinsx, xlow, xup); break;
   }

   if (xtitle) newh->GetXaxis()->SetTitle(xtitle);
   if (ytitle) newh->GetYaxis()->SetTitle(ytitle);

   if (oldh) {
      if ((oldh->GetDimension()==1) && fbMakeWithAutosave) newh->Add(oldh);
      RemoveHistogram(fullname);
   }

   if (foldername.Length() > 0)
      AddHistogram(newh, foldername.Data());
   else
      AddHistogram(newh);

   return newh;
}

TH2* TGo4EventProcessor::MakeH2(char type, const char* fullname, const char* title,
                                     Int_t nbinsx, Double_t xlow, Double_t xup,
                                     Int_t nbinsy, Double_t ylow, Double_t yup,
                                     const char* xtitle, const char* ytitle)
{
   TString foldername, histoname;

   if ((fullname==0) || (strlen(fullname)==0)) {
      cout << "Histogram name not specified, can be a hard error" << endl;
      return 0;
   }
   const char* separ = strrchr(fullname, '/');
   if (separ!=0) {
      histoname = separ + 1;
      foldername.Append(fullname, fullname - separ - 1);
   } else
      histoname = fullname;

   int itype = 0;
   const char* stype = "TH2I";
   switch (type) {
      case 'I': case 'i': itype = 0; stype = "TH2I"; break;
      case 'F': case 'f': itype = 1; stype = "TH2F"; break;
      case 'D': case 'd': itype = 2; stype = "TH2D"; break;
      case 'S': case 's': itype = 3; stype = "TH2S"; break;
      case 'C': case 'c': itype = 4; stype = "TH2C"; break;
      default:
         cout << "There is no histogram with type:" << type << ", use I instead" << endl;
   }

   TH1* oldh = GetHistogram(fullname);

   if (oldh!=0) {
      if (oldh->InheritsFrom(stype) && fbMakeWithAutosave) {
         if (title) oldh->SetTitle(title);
         if (xtitle) oldh->GetXaxis()->SetTitle(xtitle);
         if (ytitle) oldh->GetYaxis()->SetTitle(ytitle);
         return (TH2*) oldh;
      }
      cout << "There is histogram " << fullname << " with type " << oldh->ClassName() << " other than specified "
           << stype << " rebuild" << endl;
   }

   TH2* newh = 0;

   switch (itype) {
      case 0: newh = new TH2I(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 1: newh = new TH2F(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 2: newh = new TH2D(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 3: newh = new TH2S(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 4: newh = new TH2C(histoname, title, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
   }

   if (xtitle) newh->GetXaxis()->SetTitle(xtitle);
   if (ytitle) newh->GetYaxis()->SetTitle(ytitle);

   if (oldh) {
      if ((oldh->GetDimension()==2) && fbMakeWithAutosave) newh->Add(oldh);
      RemoveHistogram(fullname);
   }

   if (foldername.Length() > 0)
      AddHistogram(newh, foldername.Data());
   else
      AddHistogram(newh);

   return newh;
}

TGo4WinCond* TGo4EventProcessor::MakeWinCond(const char* fullname,
                                                   Double_t xmin, Double_t xmax,
                                                   Double_t ymin, Double_t ymax,
                                                   const char* HistoName)
{
   TString foldername, condname;

   if ((fullname==0) || (strlen(fullname)==0)) {
      cout << "Condition name not specified, can be a hard error" << endl;
      return 0;
   }
   const char* separ = strrchr(fullname, '/');
   if (separ!=0) {
      condname = separ + 1;
      foldername.Append(fullname, fullname - separ - 1);
   } else
      condname = fullname;

   TGo4Condition* cond = GetAnalysisCondition(fullname);

   if (cond!=0) {
      if (cond->InheritsFrom(TGo4WinCond::Class()) && fbMakeWithAutosave)
         return (TGo4WinCond*) cond;
      RemoveAnalysisCondition(fullname);
   }

   TGo4WinCond* wcond = new TGo4WinCond(condname);
   if ((ymin!=0.) || (ymax!=0.))
      wcond->SetValues(xmin, xmax, ymin, ymax);
   else
      wcond->SetValues(xmin, xmax);
   wcond->SetHistogram(HistoName);
   wcond->Enable();

   if (foldername.Length() > 0)
      AddAnalysisCondition(wcond, foldername.Data());
   else
      AddAnalysisCondition(wcond);

   return wcond;
}

TGo4PolyCond* TGo4EventProcessor::MakePolyCond(const char* fullname,
                                                     Int_t npoints,
                                                     Double_t (*points) [2],
                                                     const char* HistoName)
{
   TString foldername, condname;

   if ((fullname==0) || (strlen(fullname)==0)) {
      cout << "Condition name not specified, can be a hard error" << endl;
      return 0;
   }
   const char* separ = strrchr(fullname, '/');
   if (separ!=0) {
      condname = separ + 1;
      foldername.Append(fullname, fullname - separ - 1);
   } else
      condname = fullname;

   TGo4Condition* cond = GetAnalysisCondition(fullname);

   if (cond!=0) {
      if (cond->InheritsFrom(TGo4PolyCond::Class()) && fbMakeWithAutosave)
         return (TGo4PolyCond*) cond;
      RemoveAnalysisCondition(fullname);
   }

   TArrayD fullx(npoints+1), fully(npoints+1);

   for (int i=0;i<npoints;i++) {
      fullx[i] = points[i][0];
      fully[i] = points[i][1];
   }

   // connect first and last points
   if ((fullx[0]!=fullx[npoints-1]) || (fully[0]!=fully[npoints-1])) {
       fullx[npoints] = fullx[0];
       fully[npoints] = fully[0];
       npoints++;
    }

   TCutG mycat("initialcut", npoints, fullx.GetArray(), fully.GetArray());

   TGo4PolyCond* pcond = new TGo4PolyCond(condname);
   pcond->SetValues(&mycat);
   pcond->Enable();

   pcond->SetHistogram(HistoName);

   if (foldername.Length() > 0)
      AddAnalysisCondition(pcond, foldername.Data());
   else
      AddAnalysisCondition(pcond);

   return pcond;
}
