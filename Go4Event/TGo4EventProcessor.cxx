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

#include "TGo4EventProcessor.h"

#include <snprintf.h>
#include <string.h>

#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "TArrayD.h"
#include "TAxis.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4EventElement.h"
#include "TGo4EventCalibration.h"
#include "TGo4AnalysisImp.h"

TGo4EventProcessor::TGo4EventProcessor(const char* name) :
   TGo4EventSource(name),
   fxInputEvent(0),
   fxCalibration(0)
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
   if(fxInputEvent==0) return kFALSE;

   return !strcmp(fxInputEvent->ClassName(),classname);
}

Bool_t TGo4EventProcessor::CheckCalibration(const char* classname)
{
   TRACE((12,"TGo4EventProcessor::CheckCalibration()",__LINE__, __FILE__));
   if(fxCalibration==0) return kFALSE;

   return !strcmp(fxCalibration->ClassName(),classname);
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

void TGo4EventProcessor::SetMakeWithAutosave(Bool_t on)
{
   TGo4Analysis::Instance()->SetMakeWithAutosave(on);
}

Bool_t TGo4EventProcessor::IsObjMade()
{
   return TGo4Analysis::Instance()->IsObjMade();
}


TH1* TGo4EventProcessor::MakeTH1(char type, const char* fullname, const char* title,
                                 Int_t nbinsx, Double_t xlow, Double_t xup,
                                 const char* xtitle, const char* ytitle)
{
   return TGo4Analysis::Instance()->MakeTH1(type, fullname, title,
                                            nbinsx, xlow, xup,
                                            xtitle, ytitle);
}

TH2* TGo4EventProcessor::MakeTH2(char type, const char* fullname, const char* title,
                                 Int_t nbinsx, Double_t xlow, Double_t xup,
                                 Int_t nbinsy, Double_t ylow, Double_t yup,
                                 const char* xtitle, const char* ytitle, const char* ztitle)
{
   return TGo4Analysis::Instance()->MakeTH2(type, fullname, title,
                                            nbinsx, xlow, xup,
                                            nbinsy, ylow, yup,
                                            xtitle, ytitle, ztitle);
}

TGo4WinCond* TGo4EventProcessor::MakeWinCond(const char* fullname,
                                             Double_t xmin, Double_t xmax,
                                             const char* HistoName)
{
   return TGo4Analysis::Instance()->MakeWinCond(fullname, xmin, xmax, HistoName);
}

TGo4WinCond* TGo4EventProcessor::MakeWinCond(const char* fullname,
                                             Double_t xmin, Double_t xmax,
                                             Double_t ymin, Double_t ymax,
                                             const char* HistoName)
{
   return TGo4Analysis::Instance()->MakeWinCond(fullname,
                                                xmin, xmax,
                                                ymin, ymax,
                                                HistoName);
}

TGo4PolyCond* TGo4EventProcessor::MakePolyCond(const char* fullname,
                                               Int_t npoints,
                                               Double_t (*points) [2],
                                               const char* HistoName)
{
   return TGo4Analysis::Instance()->MakePolyCond(fullname, npoints, points, HistoName);
}

TGo4Parameter* TGo4EventProcessor::MakeParameter(const char* fullname,
                                                 const char* classname,
                                                 const char* newcmd)
{
   return TGo4Analysis::Instance()->MakeParameter(fullname, classname, newcmd);
}
