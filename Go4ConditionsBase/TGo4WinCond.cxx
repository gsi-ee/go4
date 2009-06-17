#include "TGo4WinCond.h"

#include "Riostream.h"

#include "TH1.h"
#include "snprintf.h"

#include "TGo4WinCondPainter.h"
#include "TGo4Log.h"

// -----------------------------------------------
// Constructors
// -----------------------------------------------
TGo4WinCond::TGo4WinCond() :
   TGo4Condition(),
   fLow1(0),
   fUp1(0),
   fLow2(0),
   fUp2(0),
   fiSaveXMin(0),
   fiSaveXMax(0),
   fiSaveYMin(0),
   fiSaveYMax(0)
{
   TRACE((15,"TGo4WinCond::TGo4WinCond()",__LINE__, __FILE__));
}
// -----------------------------------------------
TGo4WinCond::TGo4WinCond(const char* name, const char* title) :
   TGo4Condition(name,title),
   fLow1(0),
   fUp1(0),
   fLow2(0),
   fUp2(0),
   fiSaveXMin(0),
   fiSaveXMax(0),
   fiSaveYMin(0),
   fiSaveYMax(0)
{
TRACE((15,"TGo4WinCond::TGo4WinCond(name,title)",__LINE__, __FILE__));
}

// -----------------------------------------------
TGo4WinCond::~TGo4WinCond()
{
TRACE((15,"TGo4WinCond::~TGo4WinCond()",__LINE__, __FILE__));
}

// -----------------------------------------------
Bool_t TGo4WinCond::Test(Double_t v1, Double_t v2){
IncCounts();
if(!IsEnabled()){
  if(FixedResult()) IncTrueCounts();
  return FixedResult();
}
if(v1 <  fLow1) return IsFalse();
if(v1 >= fUp1)  return IsFalse();
if(v2 <  fLow2) return IsFalse();
if(v2 >= fUp2)  return IsFalse();
IncTrueCounts();
return IsTrue();
}
// -----------------------------------------------
Bool_t TGo4WinCond::Test(Double_t v1){
IncCounts();
if(!IsEnabled()){
  if(FixedResult()) IncTrueCounts();
  return FixedResult();
}
if(v1 <  fLow1) return IsFalse();
if(v1 >= fUp1)  return IsFalse();
IncTrueCounts();
return IsTrue();
}
// -----------------------------------------------
void TGo4WinCond::SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2)
{
  fLow1 = low1;
  fUp1  = up1;
  fLow2 = low2;
  fUp2  = up2;
  SetDimension(2);
}
// -----------------------------------------------
void TGo4WinCond::SetValues(Double_t low1, Double_t up1)
{
   fLow1 = low1;
   fUp1  = up1;
   SetDimension(1);
}
// -----------------------------------------------
void TGo4WinCond::GetValues(Int_t & dim, Double_t & x1, Double_t & y1, Double_t & x2, Double_t & y2)
{
   x1  = fLow1;
   y1  = fUp1;
   x2  = fLow2;
   y2  = fUp2;
   dim = GetDimension();
}

Bool_t TGo4WinCond::IsPolygonType()
{
   return kFALSE;
}
// -----------------------------------------------
void TGo4WinCond::PrintCondition(Bool_t limits){
   TGo4Condition::PrintCondition();
if(limits)
{
Text_t line[128];
if(GetDimension()==1) snprintf(line,127,"[%8.2f,%8.2f]",fLow1,fUp1);
else         snprintf(line,127,"[%8.2f,%8.2f][%8.2f,%8.2f]",fLow1,fUp1,fLow2,fUp2);
cout << line     << endl;
}}


Double_t TGo4WinCond::GetIntegral(TH1* histo, Option_t* opt)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
result=histo->Integral(opt);
RestoreHistogramRanges(histo);
return result;
}

Double_t TGo4WinCond::GetMean(TH1* histo, Int_t axis)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
result=histo->GetMean(axis);
RestoreHistogramRanges(histo);
return result;
}
Double_t TGo4WinCond::GetRMS(TH1* histo, Int_t axis)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
result=histo->GetRMS(axis);
RestoreHistogramRanges(histo);
return result;
}
Double_t TGo4WinCond::GetXMax(TH1* histo)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
TAxis* xax=histo->GetXaxis();
Int_t maxbin=histo->GetMaximumBin();
if(histo->GetDimension()==1)
   {
   result=xax->GetBinCenter(maxbin);

   }
else if (histo->GetDimension()==2)
   {
      Int_t xmaxbin=maxbin%(histo->GetNbinsX()+2);
      result=xax->GetBinCenter(xmaxbin);
   }
else
   {
      result=0; // no support for 3d histos at the moment!
   }
RestoreHistogramRanges(histo);
return result;
}
Double_t TGo4WinCond::GetYMax(TH1* histo)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
if(histo->GetDimension()==1)
   {
      result=histo->GetMaximum();
   }
else if (histo->GetDimension()==2)
   {
      TAxis* yax=histo->GetYaxis();
      Int_t maxbin=histo->GetMaximumBin();
      Int_t maxybin=maxbin/(histo->GetNbinsX()+2);
      result=yax->GetBinCenter(maxybin);
   }
else
   {
      result=0; // no support for 3d histos at the moment!
   }
RestoreHistogramRanges(histo);
return result;
}
Double_t TGo4WinCond::GetCMax(TH1* histo)
{
if(histo==0) return 0;
Double_t result=0;
SetHistogramRanges(histo);
result=histo->GetMaximum();
RestoreHistogramRanges(histo);
return result;
}

void TGo4WinCond::SetHistogramRanges(TH1* histo)
{
if(histo==0) return;
Double_t xmin=fLow1;
Double_t xmax=fUp1;
Double_t ymin=fLow2;
Double_t ymax=fUp2;
TAxis* xax=histo->GetXaxis();
fiSaveXMin=xax->GetFirst();
fiSaveXMax=xax->GetLast();
Int_t xminbin=xax->FindBin(xmin);
Int_t xmaxbin=xax->FindBin(xmax);
Int_t yminbin=0;
Int_t ymaxbin=0;
TAxis* yax=histo->GetYaxis();
if(yax && histo->GetDimension()>1)
{
   fiSaveYMin=yax->GetFirst();
   fiSaveYMax=yax->GetLast();
   yminbin=yax->FindBin(ymin);
   ymaxbin=yax->FindBin(ymax);
}
    // set histo range to condition limits
xax->SetRange(xminbin,xmaxbin);
if(yax&& histo->GetDimension()>1)
   yax->SetRange(yminbin,ymaxbin);
}

void TGo4WinCond::RestoreHistogramRanges(TH1* histo)
{
if(histo==0) return;
TAxis* xax=histo->GetXaxis();
TAxis* yax=histo->GetYaxis();
xax->SetRange(fiSaveXMin,fiSaveXMax);
if(yax&& histo->GetDimension()>1)
   yax->SetRange(fiSaveYMin,fiSaveYMax);


}



// -----------------------------------------------
Bool_t TGo4WinCond::UpdateFrom(TGo4Condition * cond, Bool_t counts)
{
   if(!TGo4Condition::UpdateFrom(cond,counts)) return kFALSE;
   if(!cond->InheritsFrom(TGo4WinCond::Class())) {
       cout << "Cannot update " << GetName() << " from " << cond->ClassName() << endl;
       return kFALSE;
   }
   Int_t dimension=0;
   ((TGo4WinCond*)cond)->GetValues(dimension,fLow1,fUp1,fLow2,fUp2);  // get limits from source
   SetDimension(dimension);
   return kTRUE;
}

void TGo4WinCond::SetPainter(TGo4ConditionPainter* painter)
{
// delete old painter, replace by the new one
// overwritten method in subclass may check if painter is correct type
if(painter==0) return;
if(painter->InheritsFrom(TGo4WinCondPainter::Class()))
   {
      if(fxPainter) delete fxPainter;
      fxPainter=painter;
      fxPainter->SetCondition(this);
   }
else
   {
      TGo4Log::Warn("Could not set painter of class %s for TGo4WinCond %s",
         painter->ClassName(),GetName());
   }
}

TGo4ConditionPainter* TGo4WinCond::CreatePainter()
{
   TGo4ConditionPainter* painter=new TGo4WinCondPainter(GetName());
   painter->SetCondition(this);
   return painter;
}

Int_t TGo4WinCond::GetMemorySize()
{
   Int_t size = sizeof(*this);
   if (GetName()!=0) size+=strlen(GetName());
   if (GetTitle()!=0) size+=strlen(GetTitle());
   return size;
}



