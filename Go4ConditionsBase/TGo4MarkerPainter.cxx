#include "TGo4MarkerPainter.h"

#include "Riostream.h"

#include "TROOT.h"
#include "TList.h"
#include "TVirtualPad.h"
#include "snprintf.h"

#include "TGo4Marker.h"
#include "TH1.h"

TGo4MarkerPainter::TGo4MarkerPainter() :
   TGo4LabelPainter(),
   fxMarker(0),
   fxConnector(0),
   fbIsConStreamed(kTRUE)
{

}

TGo4MarkerPainter::TGo4MarkerPainter(const char* name, const char* title) :
    TGo4LabelPainter(name, title),
    fxMarker(0),
    fxConnector(0),
    fbIsConStreamed(kFALSE)
{

}

TGo4MarkerPainter::~TGo4MarkerPainter()
{
   if (fxConnector!=0) {
      delete fxConnector;
      fxConnector = 0;
   }
}



void TGo4MarkerPainter::PaintLabel(Option_t* opt)
{
if(gPad==0) return;
if(fxMarker && fxMarker->HasLabel())
   {
      //------ find out initial coordinates for labels near marker boundaries:
      Double_t xoff=0.015*(gPad->GetUxmax()-gPad->GetUxmin());
      Double_t yoff=0.015*(gPad->GetUymax()-gPad->GetUymin()); // offset is in pad coordinates
      Double_t xpmin=gPad->PadtoX(gPad->XtoPad(fxMarker->GetLabelX())+xoff);
      Double_t ypmin=gPad->PadtoY(gPad->YtoPad(fxMarker->GetLabelY())+yoff);
      Bool_t drx=fxMarker->IsXDraw();
      Bool_t dry=fxMarker->IsYDraw();
      Bool_t drxbin=fxMarker->IsXbinDraw();
      Bool_t drybin=fxMarker->IsYbinDraw();
      Bool_t drcont=fxMarker->IsContDraw();
      SetX0(xpmin);
      SetY0(ypmin); // initial coordinates are real axis coordinates
      SetLineColor(fxMarker->GetMarkerColor());
      //SetTextColor(fxMarker->GetMarkerColor());
      TString cap=fxMarker->GetName();
      TH1* his=fxMarker->GetHistogram();
      if(his)
         {
            cap+=":";
            cap+=his->GetName();
         }
      SetCaption(cap.Data());
      TGo4LabelPainter::PaintLabel();// this creates new label
      if(drx)
         AddToLabel(Form("X    = %.4E",fxMarker->GetX()));
      if(dry)
         AddToLabel(Form("Y    = %.4E",fxMarker->GetY()));
      if(drxbin)
         AddToLabel(Form("Xbin = %d",fxMarker->GetXbin()));
      if(drybin)
         AddToLabel(Form("Ybin = %d",fxMarker->GetYbin()));
      if(drcont)
         AddToLabel(Form("C    = %d",fxMarker->GetCont()));
      RePaintLabel();
      PaintConnector();
   }//if(fxMarker && fxMarker->HasLabel())
else
   {
      UnPaintLabel();
   }
}

void TGo4MarkerPainter::UnPaintLabel(Option_t* opt)
{
if(gPad==0) return;
TGo4LabelPainter::UnPaintLabel(opt);
UnPaintConnector(opt);
}


Bool_t TGo4MarkerPainter::CheckConnector()
{
   if(fbIsConStreamed && fxConnector!=0) {
       // case of Connector was streamed from file: not yet in cleanup list!
       fxConnector->SetOwner(this);
       fbIsConStreamed=kFALSE;
       return kTRUE;
   }
    if(TGo4LabelConnector::fxLastDeleted==(char*) fxConnector)
      {
         // our label was deleted by user mouse menu just before
         TGo4LabelConnector::fxLastDeleted=0;
         fxConnector=0; // reset reference, will re-create label on next paint
         return kFALSE;
      }


   return fxConnector!=0;
}

void TGo4MarkerPainter::PaintConnector(Option_t* opt)
{
   if(gPad==0) return;
   if(fxMarker && fxMarker->HasConnector()) {
      Double_t xmark=0;
      Double_t ymark=0;
      Double_t xmark0=fxMarker->GetX();
      Double_t ymark0=fxMarker->GetY();
      Double_t xlab,ylab;
      Double_t xlablo=GetLabelXlo();
      Double_t ylablo=GetLabelYlo();
      Double_t xlabup=GetLabelXup();
      Double_t ylabup=GetLabelYup();
      Double_t xlabmid=0.5*(xlabup+xlablo);
      Double_t ylabmid=0.5*(ylabup+ylablo);
      xlablo=gPad->PadtoX(xlablo);
      ylablo=gPad->PadtoY(ylablo);
      xlabup=gPad->PadtoX(xlabup);
      ylabup=gPad->PadtoY(ylabup);
      xlabmid=gPad->PadtoX(xlabmid);
      ylabmid=gPad->PadtoY(ylabmid);
      // evaluate connection to nearest label edge or side centre:
      if(TMath::Abs(xlablo-xmark0) < TMath::Abs(xlabup-xmark0))
         {
            if(TMath::Abs(xlablo-xmark0) < TMath::Abs(xlabmid-xmark0))
               xlab=xlablo;
            else
               xlab=xlabmid;
         }
      else
         {
            if(TMath::Abs(xlabup-xmark0) < TMath::Abs(xlabmid-xmark0))
               xlab=xlabup;
            else
               xlab=xlabmid;
         }

      if(TMath::Abs(ylablo-ymark0) < TMath::Abs(ylabup-ymark0))
         {
             if(TMath::Abs(ylablo-ymark0) < TMath::Abs(ylabmid-ymark0))
               ylab=ylablo;
             else
               ylab=ylabmid;
         }
      else
         {
            if(TMath::Abs(ylabup-ymark0) < TMath::Abs(ylabmid-ymark0))
               ylab=ylabup;
            else
               ylab=ylabmid;
         }
      // offset from marker center:
      Double_t f=0.92; // fraction of direct distance between marker and label
      // we calculate connector geometry in pad coordinates for log scale!
      Double_t xmark_pad=0;
      Double_t ymark_pad=0;
      Double_t xlab_pad=gPad->XtoPad(xlab);
      Double_t ylab_pad=gPad->YtoPad(ylab);
      Double_t xmark0_pad=gPad->XtoPad(xmark0);
      Double_t ymark0_pad=gPad->YtoPad(ymark0);
//      if(gPad->GetLogx() || gPad->GetLogy())
//         {
//            //cout <<"using percentage for f" << endl;
//            f=0.95;
//         }
//      else
//         {
//            // note: this method only works if we have equal axis scaling in x and y!
//            //cout <<"using roff for f" << endl;
//            Double_t dr=TMath::Sqrt(TMath::Power(xlab-xmark0,2) + TMath::Power(ylab-ymark0,2));
//            Size_t msize=fxMarker->GetMarkerSize();
//            Double_t xoff=0.008*msize*(gPad->GetUxmax()-gPad->GetUxmin());
//            Double_t yoff=0.008*msize*(gPad->GetUymax()-gPad->GetUymin());
//            Double_t roff= TMath::Sqrt(TMath::Power(xoff,2) + TMath::Power(yoff,2));
//            if(dr) f=1- roff/dr;// we do it relative to marker size
//         }
//      cout <<"f="<<f << endl;
      if(xlab_pad>xmark0_pad)
            xmark_pad=xlab_pad-f*(xlab_pad-xmark0_pad);
      else
            xmark_pad=xlab_pad+f*(xmark0_pad-xlab_pad);
      if(ylab_pad>ymark0_pad)
            ymark_pad=ylab_pad-f*(ylab_pad-ymark0_pad);
      else
            ymark_pad=ylab_pad+f*(ymark0_pad-ylab_pad);

      xmark=gPad->PadtoX(xmark_pad); // back to real axis coordinates
      ymark=gPad->PadtoY(ymark_pad);
      if(!CheckConnector()) {
         // connection line not existing or deleted by root:
         fxConnector=new TGo4LabelConnector(xmark,ymark,xlab,ylab);
         fxConnector->SetOwner(this);
         fxConnector->SetLineStyle(4);
         fxConnector->AppendPad();
      } else {
         // connection line is still exisiting:
         if (gPad->GetListOfPrimitives()->FindObject(fxConnector)==0)
            fxConnector->AppendPad();// connection line was removed from pad, we redraw it:
         fxConnector->SetX1(xmark);
         fxConnector->SetY1(ymark);
         fxConnector->SetX2(xlab);
         fxConnector->SetY2(ylab);
      }
      fxConnector->SetLineColor(fxMarker->GetMarkerColor());
      //fxConnector->Paint(); // for direct update of view
   }
else
   {
      UnPaintConnector(opt);
   }
}

void TGo4MarkerPainter::UnPaintConnector(Option_t* opt)
{
   TString option=opt;
   if(option.Contains("reset")) {
      // case of reset option: discard old connector line style
      if(CheckConnector()) {
         delete fxConnector;
         fxConnector=0;
      }
   }
}

void TGo4MarkerPainter::DisplayToFront(Option_t* opt)
{
UnPaintConnector();
PaintConnector(); // pop line to front if enabled
if(fxMarker) fxMarker->TObject::Pop(); // then pop marker
TGo4LabelPainter::DisplayToFront(opt); // label itself will be frontmost
}


