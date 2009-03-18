#include "TGo4WinCondPainter.h"

#include "Riostream.h"

#include "TVirtualPad.h"
#include "RVersion.h"
#include "TList.h"
#include "TROOT.h"

#include "TGo4WinCond.h"
#include "TGo4WinCondView.h"

TGo4WinCondPainter::TGo4WinCondPainter() :
   TGo4ConditionPainter(),
   fxBox(0)
{
}

TGo4WinCondPainter::TGo4WinCondPainter(const char* name, const char* title) :
   TGo4ConditionPainter(name,title),
   fxBox(0)
{
}

TGo4WinCondPainter::~TGo4WinCondPainter()
{
   UnPaintCondition();
   if (fxBox!=0) {
      delete fxBox;
      fxBox = 0;
   }
}


void TGo4WinCondPainter::PaintCondition(Option_t* opt)
{
if(gPad==0) return;
//cout <<"WinCondPainter PaintCondition on gPad "<<gPad << endl;
double xpmin=0;
double xpmax=0;
double ypmin=0;
double ypmax=0;
int dim=0;
TGo4WinCond* wconny=dynamic_cast<TGo4WinCond*>(fxCondition);
if(wconny && wconny->IsVisible())
   {
      wconny->GetValues(dim, xpmin, xpmax, ypmin, ypmax);

      // for loaded window conditions, we have to correct limits
      // otherwise, this would yield wrong statistics from work histogram
      // root would correct box limits only on first edge resize!
      if(xpmin>xpmax)
            {
               if(wconny->GetDimension()>1) // keep dimension!
                  wconny->SetValues(xpmax, xpmin, ypmin, ypmax );
               else
                  wconny->SetValues(xpmax, xpmin);
            }
      if(ypmin>ypmax)
            {
               if(wconny->GetDimension()>1) // keep dimension!
                  wconny->SetValues(xpmin, xpmax, ypmax, ypmin );
            }

//      cout <<"\nBefore toPad:"<< endl;
//      cout <<" xpmin="<<xpmin << endl;
//      cout <<" xpmax="<<xpmax << endl;
//      cout <<" ypmin="<<ypmin << endl;
//      cout <<" ypmax="<<ypmax << endl;

// note: newer root versions treat log scale correctly in TBox
#if ROOT_VERSION_CODE < ROOT_VERSION(4,3,2)

// #if __GO4ROOTVERSION__ < 40302

      xpmin=gPad->XtoPad(xpmin); // for case of log scale
      xpmax=gPad->XtoPad(xpmax);
      if(dim==1)
         {
            ypmin =gPad->GetUymin();
            ypmax =gPad->GetUymax();
         }
      else
         {
            ypmin=gPad->YtoPad(ypmin);
            ypmax=gPad->YtoPad(ypmax);
         }

#else
      if(dim==1)
         {
            ypmin=gPad->PadtoY(gPad->GetUymin());
            ypmax=gPad->PadtoY(gPad->GetUymax());
         }
#endif //__GO4ROOTVERSION__ < 40302

      if(fxBox==0 || gPad->GetListOfPrimitives()->FindObject(fxBox)==0)
      // user might have deleted box from pad by mouse even if fxBox!=0
         {
            fxBox= new TGo4WinCondView(xpmin,ypmin,xpmax, ypmax);
         }
      else
         {
             fxBox->SetX1(xpmin);
             fxBox->SetY1(ypmin);
             fxBox->SetX2(xpmax);
             fxBox->SetY2(ypmax);
         }
      fxBox->SetCondition(wconny);
      if(!strcmp(opt,"fixstyle"))
         {
            // reproduce condition colors always
            // this mode will prevent the user from changing box color interactively
            fxBox->SetLineWidth(wconny->GetLineWidth());
            fxBox->SetLineColor(wconny->GetLineColor());
            fxBox->SetLineStyle(wconny->GetLineStyle());
            fxBox->SetFillColor(wconny->GetFillColor());
            fxBox->SetFillStyle(wconny->GetFillStyle());
         }
      if(gPad->GetListOfPrimitives()->FindObject(fxBox)==0)
         {
            fxBox->SetLineWidth(wconny->GetLineWidth());
            fxBox->SetLineColor(wconny->GetLineColor());
            fxBox->SetLineStyle(wconny->GetLineStyle());
            fxBox->SetFillColor(wconny->GetFillColor());
            fxBox->SetFillStyle(wconny->GetFillStyle());
            fxBox->AppendPad(); // only append to pad if not already there
                                // this is necessary for the Pop() in array painter
            //cout <<"TGo4WinCondPainter::PaintCondition appended box for condition "<<wconny->GetName() << endl;
         }
        fxBox->Paint(); // for condarrays
        //cout <<"------WinCondPainter "<< GetName() <<"PaintCondition has painted box." << endl;

   }// if(wconny && wconny->IsVisible())
else
   {
      UnPaintCondition();
   }
}

void TGo4WinCondPainter::UnPaintCondition(Option_t* opt)
{
   gROOT->GetListOfCanvases()->RecursiveRemove(fxBox);
   // we do not delete view, but restore graphics properties though invisible
   TString option(opt);
   if(option.Contains("reset")) {
      // case of reset option: discard old label geometry
      delete fxBox;
      fxBox=0;
   }
}

void TGo4WinCondPainter::DisplayToFront(Option_t* opt)
{
   if(fxBox) fxBox->Pop(); // box to front
   TGo4LabelPainter::DisplayToFront(opt); // label itself will be frontmost
}
