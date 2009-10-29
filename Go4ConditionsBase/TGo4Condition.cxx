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

#include "TGo4Condition.h"

#include "Riostream.h"

#include "TH1.h"
#include "TROOT.h"
#include "TList.h"
#include "TCutG.h"
#include "TVirtualPad.h"
#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4ConditionPainter.h"

const Double_t TGo4Condition::fgdUPDATEEPSILON=0.01;

Bool_t TGo4Condition::fgbLABELDRAW=kTRUE;
Bool_t TGo4Condition::fgbLIMITSDRAW=kTRUE;
Bool_t TGo4Condition::fgbINTDRAW=kTRUE;
Bool_t TGo4Condition::fgbXMEANDRAW=kTRUE;
Bool_t TGo4Condition::fgbXRMSDRAW=kTRUE;
Bool_t TGo4Condition::fgbYMEANDRAW=kFALSE;
Bool_t TGo4Condition::fgbYRMSDRAW=kFALSE;
Bool_t TGo4Condition::fgbXMAXDRAW=kTRUE;
Bool_t TGo4Condition::fgbYMAXDRAW=kFALSE;
Bool_t TGo4Condition::fgbCMAXDRAW=kTRUE;


// -----------------------------------------------
// Constructors
// ---------------------------------------------------------
TGo4Condition::TGo4Condition() :
   TNamed(),
   TAttLine(),
   TAttFill(),
   fxPainter(0),
   fxCutHis(0),
   fxHisto(0),
   fiIsChanged(0)
{
   TRACE((15,"TGo4Condition::TGo4Condition()",__LINE__, __FILE__));
   fiDim=0;
   fbEnabled = false;
   fbResult  = true;
   fbTrue    = true;
   fbFalse   = false;
   fbMarkReset   = false;
   fiCounts  = 0;
   fiTrueCounts= 0;
   fbVisible=true;
   fbMultiEdit= true;
   fbHistogramLink=false;
   fdUpdateEpsilon=fgdUPDATEEPSILON;
   fbIsPainted=kFALSE;
   fbOwnedByEditor=kFALSE;
   fbStreamedCondition=kTRUE;
   InitLabelStyle();
}

// -----------------------------------------------
TGo4Condition::TGo4Condition(const char* name, const char* title) :
   TNamed(name, title),
   TAttLine(),
   TAttFill(),
   fxPainter(0),
   fxCutHis(0),
   fxHisto(0),
   fiIsChanged(0)
{
   TRACE((15,"TGo4Condition::TGo4Condition(const char*)",__LINE__, __FILE__));
   fiDim=0;
   fbEnabled = false;
   fbResult  = true;
   fbTrue    = true;
   fbFalse   = false;
   fbMarkReset   = false;
   fiCounts  = 0;
   fiTrueCounts = 0;
   fbVisible=true;
   fbMultiEdit= true;
   fbHistogramLink=false;
   fdUpdateEpsilon=fgdUPDATEEPSILON;
   fbIsPainted=kFALSE;
   fbOwnedByEditor=kFALSE;
   fbStreamedCondition=kFALSE;
   InitLabelStyle();
}

// ---------------------------------------------------------
TGo4Condition::~TGo4Condition()
{
   TRACE((15,"TGo4Condition::~TGo4Condition()",__LINE__, __FILE__));

   UnDraw("reset");

   if(fxPainter) {
      delete fxPainter;
      fxPainter = 0;
   }
   if(fxCutHis) {
      delete fxCutHis;
      fxCutHis = 0;
   }
}
// ---------------------------------------------------------
Bool_t TGo4Condition::Test()
{
   fiCounts++;
   return true;
}
// ---------------------------------------------------------

void TGo4Condition::AddCondition(TGo4Condition* next)
{
TRACE((14,"TGo4Condition::AddCondition(TGo4Condition*)",__LINE__, __FILE__));
}
// ---------------------------------------------------------
void TGo4Condition::IncTrueCounts()
{
TRACE((12,"TGo4Condition::IncTrueCounts()",__LINE__, __FILE__));
   fiTrueCounts++;
}
// ---------------------------------------------------------
void TGo4Condition::IncCounts()
{
TRACE((12,"TGo4Condition::IncCounts()",__LINE__, __FILE__));
   fiCounts++;
}
// ---------------------------------------------------------
Int_t TGo4Condition::Counts()
{
TRACE((12,"TGo4Condition::Counts()",__LINE__, __FILE__));
   return fiCounts;
}
// ---------------------------------------------------------
Int_t TGo4Condition::TrueCounts()
{
TRACE((12,"TGo4Condition::TrueCounts()",__LINE__, __FILE__));
   return fiTrueCounts;
}
// ---------------------------------------------------------
void TGo4Condition::ResetCounts()
{
TRACE((12,"TGo4Condition::ResetCounts()",__LINE__, __FILE__));
   fiTrueCounts=0;
   fiCounts=0;
}
// ---------------------------------------------------------
void TGo4Condition::SetCounts(Int_t truecounts, Int_t counts)
{
TRACE((12,"TGo4Condition::SetCounts()",__LINE__, __FILE__));
   fiTrueCounts=truecounts;
   fiCounts=counts;
}


// ---------------------------------------------------------
void TGo4Condition::Invert(Bool_t on)
{
TRACE((12,"TGo4Condition::Invert",__LINE__, __FILE__));
fbTrue  = on ^ true;
fbFalse = on ^ false;
}

// ---------------------------------------------------------
void TGo4Condition::PrintCondition(Bool_t full)
{
   Float_t perc;
   char line[128];
   char num[64];
   if(fiCounts==0)perc=0.0;
   else perc=100.0/fiCounts*fiTrueCounts;
   cout << "Name:" << GetName()
        << " type:" << ClassName()
        << " title:" << GetTitle() << endl;
   if(fbHistogramLink)
      cout << "Connected to histogram " << fxHistoName << endl;
   if(fbEnabled)  strcpy(line,"Is Checked   ");
   else{
      if(fbResult) strcpy(line,"Always True  ");
      else         strcpy(line,"Always False ");
   }
   if(fbTrue) strcat(line,"normal  ");
   else       strcat(line,"inverse ");
   snprintf(num,63,", tested: %8d true: %8d is %3.0f%%",fiCounts,fiTrueCounts,perc);
   strcat(line,num);
   cout << line     << endl;
}
// ---------------------------------------------------------
void TGo4Condition::PrintBar()
{
   Float_t perc;
   char line[128];
   char num[64];
   strcpy(num,"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
   Char_t *pc;
   if(fiCounts==0) perc=0.0;
   else            perc=100.0/fiCounts*fiTrueCounts;
   pc=num + (Int_t)perc/2;
   *pc=0;
   snprintf(line,127,"%-24s %8d %3.0f%% |%-50s|",GetName(),fiCounts,perc,num);
   *pc='+';
   cout << line     << endl;
}
// -----------------------------------------------
void TGo4Condition::Print(Option_t* opt) const{
   //cout <<"MyPrint:"<<GetName() << endl;
TGo4Condition* localthis=const_cast<TGo4Condition*>(this);
TString option=opt;
option.ToLower();
if(option.IsNull() || option=="*")
   {
       // old default: we print bar graphics to cout
      localthis->PrintBar();
   }
else
   {
      // new printout of condition with different options:
   TString textbuffer="\nCondition ";
   textbuffer+=localthis->GetName();
   if(localthis->IsPolygonType())
      {
         textbuffer+=" (Polygon type, 2-dim)";
      }
   else
      {
         textbuffer+=" (Window type,";
         if(localthis->GetActiveCondition()->GetDimension()>1)
            textbuffer+=" 2-dim)";
         else
            textbuffer+=" 1-dim)";
      }


   //textbuffer+="\n";
   if(option.Contains("limits"))
      textbuffer +=
         Form("\n!  Xlow: \t\tXup: \t\tYlow: \t\tYup:\n   %.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t",
               localthis->GetXLow(),localthis->GetXUp(),localthis->GetYLow(),localthis->GetYUp());

   if(option.Contains("flags"))
      textbuffer +=
         Form("\n!  Status:\n!  Enab.: \tVis.: \tRes.: \tTrue: \tCnts: \tTrueCnts:\n   %d\t\t%d\t%d\t%d\t%d\t%d",
               localthis->fbEnabled, localthis->IsVisible(), localthis->fbResult, localthis->fbTrue,
               localthis->Counts(), localthis->TrueCounts());

   if(option.Contains("stats"))
      {
         // output of region statistics

         textbuffer+="\n!  with";
         TH1* hist=localthis->GetWorkHistogram();
         if(hist)
            {
               textbuffer+=" histogram: ";
               textbuffer+=hist->GetName();
               textbuffer +=
                  Form("\n!   Int:\t\tXmax:\t\tYmax:\t\tCmax:\t\tXmean:\t\tYmean:\t\tXrms:\t\tYrms:\n    %.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f",
                        localthis->GetIntegral(hist), localthis->GetXMax(hist),localthis->GetYMax(hist), localthis->GetCMax(hist),
                        localthis->GetMean(hist,1), localthis->GetMean(hist,2), localthis->GetRMS(hist,1), localthis->GetRMS(hist,2));
            }
         else
            {
               textbuffer+="out histogram";
            }
      }
   // now check output mode:
   if(option.Contains("go4log"))
      {
         TGo4Log::Message(1,textbuffer.Data());
      }
   else
      {
         cout << textbuffer.Data() << endl;
      }
   } //if(option.IsNull())
}
// ---------------------------------------------------------
Bool_t TGo4Condition::UpdateFrom(TGo4Condition * cond, Bool_t counts){
    fbTrue    = cond->TGo4Condition::IsTrue();
    fbFalse   = cond->TGo4Condition::IsFalse();
    fbResult  = cond->TGo4Condition::FixedResult();
    fbEnabled = cond->TGo4Condition::IsEnabled();
    fbMarkReset = cond->fbMarkReset;
    fbVisible=cond->TGo4Condition::IsVisible();
    fbLabelDraw=cond->TGo4Condition::IsLabelDraw();
    fbMultiEdit=cond->TGo4Condition::IsMultiEdit();
    fbIntDraw= cond->TGo4Condition::IsIntDraw();
    fbXMeanDraw= cond->TGo4Condition::IsXMeanDraw();
    fbXRMSDraw= cond->TGo4Condition::IsXRMSDraw();
    fbYMeanDraw= cond->TGo4Condition::IsYMeanDraw();
    fbYRMSDraw= cond->TGo4Condition::IsYRMSDraw();
    fbXMaxDraw= cond->TGo4Condition::IsXMaxDraw();
    fbYMaxDraw= cond->TGo4Condition::IsYMaxDraw();
    fbCMaxDraw= cond->TGo4Condition::IsCMaxDraw();
    fbHistogramLink=cond->TGo4Condition::IsHistogramLink();
    fxHistoName=cond->fxHistoName;
    fdUpdateEpsilon= cond->fdUpdateEpsilon;
    fiDim=cond->GetDimension();
    if(counts){
      fiCounts = cond->TGo4Condition::Counts();
      fiTrueCounts = cond->TGo4Condition::TrueCounts();
      fbMarkReset = false;
    }
    if(fbMarkReset){
       ResetCounts();
       fbMarkReset = false;
    }

return kTRUE;
}

void TGo4Condition::GetValues(Int_t & dim, Double_t & xmin, Double_t & xmax, Double_t & ymin, Double_t & ymax)
{
   xmin  = GetXLow();
   xmax  = GetXUp();
   ymin  = GetYLow();
   ymax  = GetYUp();
   dim = GetDimension();

}
Double_t TGo4Condition::GetXLow(){return 0;}
Double_t TGo4Condition::GetXUp(){return 0;}
Double_t TGo4Condition::GetYLow(){return 0;}
Double_t TGo4Condition::GetYUp(){return 0;}
TCutG* TGo4Condition::GetCut(Bool_t owner){ return 0;}
Bool_t TGo4Condition::IsPolygonType(){return kFALSE;}
Bool_t TGo4Condition::IsArrayType(){return kFALSE;}
TGo4Condition* TGo4Condition::GetActiveCondition(){return this;}

void TGo4Condition::SetCurrentIndex(Int_t) { }
Int_t TGo4Condition::GetCurrentIndex()
{
   return 0;
}

Int_t TGo4Condition::GetNumberOfConditions()
{
   return 1;
}

Int_t TGo4Condition::GetMemorySize()
{
   Int_t size = sizeof(*this);
   if (GetName()!=0) size+=strlen(GetName());
   if (GetTitle()!=0) size+=strlen(GetTitle());
   return size;
}

void TGo4Condition::MarkReset(Bool_t on)
{
   fbMarkReset = on;
}
void TGo4Condition::Clear(Option_t* opt){
ResetCounts();
}

void TGo4Condition::GetFlags(Bool_t* enabled, Bool_t* lastresult, Bool_t* markreset,
                   Bool_t* result, Bool_t* truevalue, Bool_t* falsevalue)
{
  *enabled=fbEnabled;
  *lastresult=fbLastResult;
  *markreset=fbMarkReset;
  *result=fbResult;
  *truevalue=fbTrue;
  *falsevalue=fbFalse;
}
void TGo4Condition::SetFlags(Bool_t enabled, Bool_t lastresult, Bool_t markreset,
                   Bool_t result, Bool_t truevalue, Bool_t falsevalue)
{
  fbEnabled=enabled;
  fbLastResult=lastresult;
  fbMarkReset=markreset;
  fbResult=result;
  fbTrue=truevalue;
  fbFalse=falsevalue;
}



Double_t TGo4Condition::GetIntegral(TH1* histo, Option_t* opt)
{
   return 0;
}

Double_t TGo4Condition::GetMean(TH1* histo, Int_t axis)
{
   return 0;
}
Double_t TGo4Condition::GetRMS(TH1* histo, Int_t axis)
{
   return 0;
}
Double_t TGo4Condition::GetXMax(TH1* histo)
{
   return 0;
}
Double_t TGo4Condition::GetYMax(TH1* histo)
{
   return 0;
}
Double_t TGo4Condition::GetCMax(TH1* histo)
{
   return 0;
}

void TGo4Condition::Disable(Bool_t result)
{
  fbEnabled=kFALSE;
  fbResult=result;
}

void TGo4Condition::Enable()
{
   fbEnabled=kTRUE;
}


void TGo4Condition::SetVisible(Bool_t on)
{
   fbVisible=on;
}

Bool_t TGo4Condition::IsVisible()
{
   return fbVisible;
}

void TGo4Condition::SetHistogramLink(Bool_t on)
{
  fbHistogramLink=on;
}

Bool_t TGo4Condition::IsHistogramLink()
{
   return (Bool_t) fbHistogramLink;
}

void TGo4Condition::SetLabelDraw(Bool_t on)
{
fbLabelDraw=on;
}

Bool_t TGo4Condition::IsLabelDraw()
{
return fbLabelDraw;
}

void TGo4Condition::SetLimitsDraw(Bool_t on)
{
fbLimitsDraw=on;
}

Bool_t TGo4Condition::IsLimitsDraw()
{
return fbLimitsDraw;
}

void TGo4Condition::SetIntDraw(Bool_t on)
{
fbIntDraw=on;
}
Bool_t TGo4Condition::IsIntDraw()
{
return fbIntDraw;
}
void TGo4Condition::SetXMeanDraw(Bool_t on)
{
fbXMeanDraw=on;
}
Bool_t TGo4Condition::IsXMeanDraw()
{
return fbXMeanDraw;
}
void TGo4Condition::SetXRMSDraw(Bool_t on)
{
fbXRMSDraw=on;
}
Bool_t TGo4Condition::IsXRMSDraw()
{
return fbXRMSDraw;
}
void TGo4Condition::SetYMeanDraw(Bool_t on)
{
fbYMeanDraw=on;
}
Bool_t TGo4Condition::IsYMeanDraw()
{
return fbYMeanDraw;
}
void TGo4Condition::SetYRMSDraw(Bool_t on)
{
fbYRMSDraw=on;
}
Bool_t TGo4Condition::IsYRMSDraw()
{
return fbYRMSDraw;
}
void TGo4Condition::SetXMaxDraw(Bool_t on)
{
fbXMaxDraw=on;
}
Bool_t TGo4Condition::IsXMaxDraw()
{
return fbXMaxDraw;
}
void TGo4Condition::SetYMaxDraw(Bool_t on)
{
fbYMaxDraw=on;
}
Bool_t TGo4Condition::IsYMaxDraw()
{
return fbYMaxDraw;
}
void TGo4Condition::SetCMaxDraw(Bool_t on)
{
fbCMaxDraw=on;
}
Bool_t TGo4Condition::IsCMaxDraw()
{
   return fbCMaxDraw;
}

void TGo4Condition::SetHistogram(const char* name)
{
   if ((name==0) || (*name==0)) {
      fxHistoName = "";
      fbHistogramLink = false;
   } else {
      fxHistoName = name;
      fbHistogramLink = true;
   }
}

const char* TGo4Condition::GetLinkedHistogram()
{
   return fxHistoName.Data();
}

void TGo4Condition::SetPainter(TGo4ConditionPainter*)
{
// delete old painter, replace by the new one
// overwritten method in subclass may check if painter is correct type

}


void TGo4Condition::Paint(Option_t* opt)
{
 /////// check for streamed canvas markers that were not Draw()n:
if(fbStreamedCondition) {
    SetPainted(kTRUE);
    fbStreamedCondition=kFALSE;
}

if(!IsPainted()) return;
if(fxPainter==0) fxPainter=CreatePainter();
// condition subclass may not provide a real painter, then we skip painting:
if(fxPainter!=0)
   {
      fxPainter->PaintCondition(opt);
      fxPainter->PaintLabel(opt);
   }
}

void TGo4Condition::Draw(Option_t* opt)
{
   if(TGo4Condition::IsVisible()) {
      if(gPad && gPad->GetListOfPrimitives()->FindObject(this)==0) {
         UnDraw();
         AppendPad(opt);
      }
      SetPainted(kTRUE);
   } else
      UnDraw(opt);
}

void TGo4Condition::UnDraw(Option_t* opt)
{
   SetPainted(kFALSE);
   gROOT->GetListOfCanvases()->RecursiveRemove(this);

   if(fxPainter==0) fxPainter=CreatePainter();
   // condition subclass may not provide a real painter, then we skip unpainting:
   if(fxPainter!=0) {
      fxPainter->UnPaintCondition(opt);
      //if(strcmp(opt,"keeplabel"))
      fxPainter->UnPaintLabel();
   }
}

void TGo4Condition::Pop()
{
   if(fxPainter!=0) fxPainter->DisplayToFront();
}

TGo4ConditionPainter* TGo4Condition::CreatePainter()
{
   return 0;
}

void TGo4Condition::SaveLabelStyle()
{
   TGo4Condition::fgbLABELDRAW=fbLabelDraw;
   TGo4Condition::fgbLIMITSDRAW=fbLimitsDraw;
   TGo4Condition::fgbINTDRAW=fbIntDraw;
   TGo4Condition::fgbXMEANDRAW=fbXMeanDraw;
   TGo4Condition::fgbXRMSDRAW=fbXRMSDraw;
   TGo4Condition::fgbYMEANDRAW=fbYMeanDraw;
   TGo4Condition::fgbYRMSDRAW=fbYRMSDraw;
   TGo4Condition::fgbXMAXDRAW=fbXMaxDraw;
   TGo4Condition::fgbYMAXDRAW=fbYMaxDraw;
   TGo4Condition::fgbCMAXDRAW=fbCMaxDraw;
}

void TGo4Condition::InitLabelStyle()
{
   fbLabelDraw=TGo4Condition::fgbLABELDRAW;
   fbLimitsDraw=TGo4Condition::fgbLIMITSDRAW;
   fbIntDraw=TGo4Condition::fgbINTDRAW;
   fbXMeanDraw=TGo4Condition::fgbXMEANDRAW;
   fbXRMSDraw=TGo4Condition::fgbXRMSDRAW;
   fbYMeanDraw=TGo4Condition::fgbYMEANDRAW;
   fbYRMSDraw=TGo4Condition::fgbYRMSDRAW;
   fbXMaxDraw=TGo4Condition::fgbXMAXDRAW;
   fbYMaxDraw=TGo4Condition::fgbYMAXDRAW;
   fbCMaxDraw=TGo4Condition::fgbCMAXDRAW;
}

void TGo4Condition::ResetLabel(Option_t* opt)
{
   if(fxPainter) {
      fxPainter->UnPaintLabel(opt);
      fxPainter->PaintLabel();
   }
}

void TGo4Condition::SetWorkHistogram(TH1* histo)
{
   fxHisto=histo;
   delete fxCutHis; // discard internal cut histogram
   fxCutHis=0;
}

void TGo4Condition::DeletePainter()
{
   if (fxPainter!=0) {
      delete fxPainter;
      fxPainter = 0;
   }
}

const char* TGo4Condition::MakeScript(ostream& out, const char* varname, Option_t* opt, const char* arrextraargs)
{
   Bool_t savemacro = (opt!=0) && (strstr(opt,"savemacro")!=0);
   Bool_t saveprefix = savemacro;

   const char* subname = strstr(opt, "name:");
   if (subname != 0) { varname = subname + 5; saveprefix = kFALSE; }

   if (saveprefix) {
      out << Form("   %s* %s = (%s*) go4->GetObject(\"%s\",\"Go4\");",
                   ClassName(), varname, ClassName(), GetName()) << endl;
      out << Form("   if (%s==0) {", varname) << endl;
      out << Form("      cout << \"Could not find %s\" << endl;", GetName()) << endl;
      out << Form("      return kFALSE;") << endl;
      out << Form("   }") << endl << endl;
      out << Form("   if (strcmp(%s->ClassName(), \"%s\")) {", varname, ClassName()) << endl;
      out << Form("      cout << \"Condition %s has wrong class \" << %s->ClassName() << endl;", GetName(), varname) << endl;
      out << Form("      return kFALSE;") << endl;
      out << Form("   }") << endl << endl;
      out << Form("   cout << \"Set condition %s as saved at %s\" << endl;",
                         GetName(),TDatime().AsString()) << endl << endl;
   } else
   if (!savemacro && ((opt==0) || (strstr(opt, "nocreate")==0))) {
      out << Form("   %s* %s = new %s(\"%s\"%s);", ClassName(), varname, ClassName(), GetName(), (arrextraargs ? arrextraargs : "")) << endl << endl;
   }

   if (arrextraargs==0) {

      Bool_t enabled,last,mark,result,vtrue,vfalse;
      GetFlags(&enabled, &last, &mark, &result, &vtrue, &vfalse);

      out << "   // SetFlags(enabled,last,mark,result,vtrue,vfalse);" << endl;

      out << Form("   %s%s->SetFlags(%s, %s, %s, %s, %s, %s);",
                  savemacro ? "if (flags) " : "", varname,
                  enabled ? "kTRUE" : "kFALSE",
                  last ? "kTRUE" : "kFALSE",
                  mark ? "kTRUE" : "kFALSE",
                  result ? "kTRUE" : "kFALSE",
                  vtrue ? "kTRUE" : "kFALSE",
                  vfalse ? "kTRUE" : "kFALSE") << endl;

      out << Form("   %s%s->SetCounts(%d, %d);",
                 savemacro ? "if (counters) " : "", varname,
                 TrueCounts(), Counts()) << endl;

      if (savemacro)
         out << Form("   if (reset) %s->ResetCounts();", varname) << endl;
   }

   return varname;
}
