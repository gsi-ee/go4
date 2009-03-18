#include "TGo4PolyCond.h"

#include "Riostream.h"
#include "RVersion.h"

#include "TMath.h"
#include "TROOT.h"
#include "TH2.h"
#include "TList.h"
#include "TCutG.h"

#include "TGo4PolyCondPainter.h"
#include "TGo4Log.h"

TString TGo4PolyCond::NextAvailableName()
{
   TString res;
   Int_t cnt = 0;
   do {
      res.Form("CutG_%d",cnt++);
   } while (gROOT->GetListOfSpecials()->FindObject(res)!=0);
   return res;
}

// ----------------------------------------------------------
TGo4PolyCond::TGo4PolyCond() :
   TGo4Condition(),
   fxCut(0)
{
   SetDimension(2);
}
// ----------------------------------------------------------
TGo4PolyCond::TGo4PolyCond(const char* name, const char* title) :
   TGo4Condition(name,title),
   fxCut(0)
{
   SetDimension(2);
}
// ----------------------------------------------------------
TGo4PolyCond::~TGo4PolyCond()
{
   if(fxCut != 0)
       delete fxCut;
}

Double_t TGo4PolyCond::GetXLow()
{
   if(fxCut==0) return 0;
   Int_t n=fxCut->GetN();
   Double_t* xarr=fxCut->GetX();
   fxCut->SetBit(kMustCleanup,0);
   Int_t nxmin=TMath::LocMin(n,xarr);
   return (xarr[nxmin]);
}
Double_t TGo4PolyCond::GetXUp()
{
   if(fxCut==0) return 0;
   Int_t n=fxCut->GetN();
   Double_t* xarr=fxCut->GetX();
   Int_t nxmax=TMath::LocMax(n,xarr);
   return (xarr[nxmax]);
}
Double_t TGo4PolyCond::GetYLow()
{
   if(fxCut==0) return 0;
   Int_t n=fxCut->GetN();
   Double_t* yarr=fxCut->GetY();
   Int_t nymin=TMath::LocMin(n,yarr);
   return (yarr[nymin]);
}
Double_t TGo4PolyCond::GetYUp()
{
   if(fxCut==0) return 0;
   Int_t n=fxCut->GetN();
   Double_t* yarr=fxCut->GetY();
   Int_t nymax=TMath::LocMax(n,yarr);
   return (yarr[nymax]);
}

Bool_t TGo4PolyCond::IsPolygonType()
{
   return kTRUE;
}

// ----------------------------------------------------------
TCutG* TGo4PolyCond::GetCut(Bool_t changeowner)
{
   TCutG* tempcut = fxCut;

   if(changeowner) {
      fxCut = 0;
      delete fxCutHis;
      fxCutHis=0;
   }
   return tempcut;
}

// ----------------------------------------------------------
TCutG * TGo4PolyCond::CloneCut(TGo4PolyCond * source)
{
  TCutG * tempcut = source->GetCut(false); // get fxCut pointer
  if(tempcut) return (TCutG *)tempcut->Clone(GetName());
         else return 0;
}
// ----------------------------------------------------------
void TGo4PolyCond::SetValues(TCutG * newcut)
{
   if(newcut==0) return;
   if(fxCut!=0) delete fxCut;

   fxCut = (TCutG*) newcut->Clone(NextAvailableName());

   // when updated from view, we store graphical attributes:
   SetLineColor(newcut->GetLineColor());
   SetLineWidth(newcut->GetLineWidth());
   SetLineStyle(newcut->GetLineStyle());
   SetFillColor(newcut->GetFillColor());
   SetFillStyle(newcut->GetFillStyle());

   delete fxCutHis; // fxCut changed, so discard previous fxCut histogram
   fxCutHis=0;
//cout << "Set fxCut " << fxCut << " from " << newcut << endl;
}
// ----------------------------------------------------------
void TGo4PolyCond::SetValuesDirect(TCutG * newcut)
{
   if(newcut==0) return;
   if(fxCut!=0) delete fxCut;

   fxCut = newcut;

   // when updated from view, we store graphical attributes:
   SetLineColor(newcut->GetLineColor());
   SetLineWidth(newcut->GetLineWidth());
   SetLineStyle(newcut->GetLineStyle());
   SetFillColor(newcut->GetFillColor());
   SetFillStyle(newcut->GetFillStyle());

   delete fxCutHis; // fxCut changed, so discard previous fxCut histogram
   fxCutHis=0;
//cout << "Set fxCut " << fxCut << " from " << newcut << endl;
}


// ----------------------------------------------------------
void TGo4PolyCond::SetValues(Double_t * x, Double_t * y, Int_t len)
{
   if(fxCut != 0) delete fxCut;
   fxCut = new TCutG(NextAvailableName(), len, x, y);
   fxCut->SetBit(kMustCleanup);
   //cout << "Set new fxCut " << fxCut << endl;
   delete fxCutHis; // discard previous fxCut histogram
   fxCutHis=0;
}
// ----------------------------------------------------------
Bool_t TGo4PolyCond::Test(Double_t x, Double_t y)
{
   IncCounts();
   if((!IsEnabled()) || (fxCut == 0)){
      if(FixedResult()) IncTrueCounts();
      return FixedResult();
   }
   Bool_t outside = (fxCut->IsInside(x,y) == 0);
   if(outside) return IsFalse();
   IncTrueCounts();
   return IsTrue();
}
// ----------------------------------------------------------
void TGo4PolyCond::PrintCondition(Bool_t points){
TGo4Condition::PrintCondition();
//cout << "fxCut " << fxCut << endl;
if(points){
   if(fxCut == 0){
//cout << "Cut " << fxCut->GetName() << endl;
cout << "No polygon specified!" << endl;
   }
else fxCut->Print(0);
}}
// -----------------------------------------------
// PrintBar switch is handled in condition baseclass now.
//void TGo4PolyCond::Print(Option_t* opt) const{
//// this trick is needed since root defines Print as const function...
//   TGo4PolyCond* const localthis= const_cast<TGo4PolyCond* const>(this);
//   localthis->PrintBar();
//}
// ----------------------------------------------------------
Bool_t TGo4PolyCond::UpdateFrom(TGo4Condition * cond, Bool_t counts)
{
  if(!TGo4Condition::UpdateFrom(cond,counts)) return kFALSE;
  if(cond->InheritsFrom(TGo4PolyCond::Class()))
     {
       TCutG * temp = CloneCut((TGo4PolyCond*)cond);  // get clone from source, still valid there!
       CleanupSpecials(); // remove all references to cloned TCutG from list of specials
       //cout << "Update " << GetName() << " from " << temp << endl;
       if(temp != 0)
          {
             //cout << "Delete fxCut " << fxCut << endl;
             if(fxCut != 0) delete fxCut;
             fxCut = temp;
             delete fxCutHis;
             fxCutHis=0;
             //cout << "Update new fxCut " << fxCut << endl;
             return kTRUE;
          }
       else
          {
             return kFALSE;
          }
    }
 else
   {
      cout << "Cannot update " << GetName() << " from " << cond->ClassName() << endl;
      return kFALSE;
   }
}

Double_t TGo4PolyCond::GetIntegral(TH1* histo, Option_t* opt)
{
//// root >4.00/08 only:
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,0,8)
//#if __GO4ROOTVERSION__ >= 40008
   //cout <<"Integral with new root" << endl;
   if(fxCut)
      return (fxCut->Integral(dynamic_cast<TH2*>(histo),opt));
   else
      return 0;
#else
   //cout <<"Integral with old root" << endl;
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      return (fxCutHis->Integral(opt));
   else
      return-1;
#endif // 40008
}

Double_t TGo4PolyCond::GetMean(TH1* histo, Int_t axis)
{
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      return (fxCutHis->GetMean(axis));
   else
      return-1;
}
Double_t TGo4PolyCond::GetRMS(TH1* histo, Int_t axis)
{
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      return (fxCutHis->GetRMS(axis));
   else
      return -1;
}
Double_t TGo4PolyCond::GetXMax(TH1* histo)
{
   Double_t result=0;
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      {
          TAxis* xax=fxCutHis->GetXaxis();
          Int_t maxbin=fxCutHis->GetMaximumBin();
          Int_t xmaxbin=maxbin%(fxCutHis->GetNbinsX()+2);
          result=xax->GetBinCenter(xmaxbin);
      }
   else
      {
         result=-1;
      }
   return result;
}
Double_t TGo4PolyCond::GetYMax(TH1* histo)
{
   Double_t result=0;
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      {
         TAxis* yax=fxCutHis->GetYaxis();
         Int_t maxbin=fxCutHis->GetMaximumBin();
         Int_t maxybin=maxbin/(fxCutHis->GetNbinsX()+2);
         result=yax->GetBinCenter(maxybin);
      }
   else
      {
         result=-1;
      }
   return result;
}
Double_t TGo4PolyCond::GetCMax(TH1* histo)
{
   if(fxCutHis==0) fxCutHis=CreateCutHistogram(histo);
   if(fxCutHis!=0)
      return(fxCutHis->GetMaximum());
   else
      return -1;
}

void TGo4PolyCond::SetPainter(TGo4ConditionPainter* painter)
{
if(painter==0) return;
if(painter->InheritsFrom(TGo4PolyCondPainter::Class()))
   {
      if(fxPainter) delete fxPainter;
      fxPainter=painter;
      fxPainter->SetCondition(this);
   }
else
   {
      TGo4Log::Warn("Could not set painter of class %s for TGo4PolyCond %s",
         painter->ClassName(),GetName());
   }


}


TGo4ConditionPainter* TGo4PolyCond::CreatePainter()
{
   TGo4ConditionPainter* painter=new TGo4PolyCondPainter(GetName());
   painter->SetCondition(this);
   return painter;
}

TH2* TGo4PolyCond::CreateCutHistogram(TH1* source)
{
   TH2* his=dynamic_cast<TH2*>(source);
   if(his==0) return 0;
   TH2* work= (TH2*) his->Clone();
   Int_t nx=work->GetNbinsX();
   Int_t ny=work->GetNbinsY();
   TAxis* xaxis = work->GetXaxis();
   TAxis* yaxis = work->GetYaxis();
   xaxis->SetRange(0,0); // expand work histogram to full range
   yaxis->SetRange(0,0);
   // set all bins outside fxCut to zero:
   for(Int_t i=0; i<nx;++i)
      {
       Double_t x = xaxis->GetBinCenter(i);
       for(Int_t j=0; j<ny;++j)
         {
            Double_t y = yaxis->GetBinCenter(j);
            if(fxCut && !(fxCut->IsInside(x,y)))
               work->SetBinContent(i,j,0);
         }
      }
   // prepare statistics:
   Stat_t s[11]={0}; // dimension is kNstat of TH1.cxx
   work->PutStats(s); // reset previous stats
   work->GetStats(s); // recalculate
   work->PutStats(s); // put back
   work->SetDirectory(0); // important for first draw from marker setup file!
   return work;
}

// ----------------------------------------------------------
void TGo4PolyCond::CleanupSpecials()
{
   TSeqCollection* specials=gROOT->GetListOfSpecials();
   TIter iter(specials);
   TObject* ob=0;
   while((ob = iter())!=0) {
     if(ob->InheritsFrom(TCutG::Class())) {
        specials->Remove(ob);
        //cout <<">>>>>>>>>> removed fxCut" <<ob<<" :" << ob->GetName() <<" from list of specials "<< endl;
     }
   }//while
}

Int_t TGo4PolyCond::GetMemorySize()
{
   Int_t size = sizeof(*this);
   if (GetName()!=0) size+=strlen(GetName());
   if (GetTitle()!=0) size+=strlen(GetTitle());
   if (fxCut!=0) {
      size += sizeof(*fxCut);
#if ROOT_VERSION_CODE > ROOT_VERSION(4,0,8)
      size += fxCut->GetMaxSize()*2*sizeof(Double_t);
#else
      size += fxCut->GetN()*2*sizeof(Double_t);
#endif
   }
   return size;
}
