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

#include "TGo4HistogramStatus.h"

#include "go4iostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "TGo4Log.h"

TGo4HistogramStatus::TGo4HistogramStatus(TH1* his, Bool_t allstatistics)
: TGo4ObjectStatus(his, allstatistics),
fiDim(0),fiNbinX(0), fiNbinY(0), fiNbinZ(0),
fdXmin(0), fdXmax(0), fdYmin(0), fdYmax(0), fdZmin(0), fdZmax(0),
fdEntries(0), fdXmean(0), fdYmean(0), fdZmean(0),
fdXrms(0), fdYrms(0), fdZrms(0)
{
   TRACE((14,"TGo4HistogramStatus::TGo4HistogramStatus(TH1*)",__LINE__, __FILE__));

   if(his!=0)
      {
         fiDim=his->GetDimension();
         fiNbinX=his->GetNbinsX();
         fiNbinY=his->GetNbinsY();
         fiNbinZ=his->GetNbinsZ();
         TAxis* xax=his->GetXaxis();
         if(xax)
            {
               fdXmin=xax->GetXmin();
               fdXmax=xax->GetXmax();
            }
         else
            { // values already initialized as zero
            }
         TAxis* yax=his->GetYaxis();
         if(yax)
            {
               fdYmin=yax->GetXmin();
               fdYmax=yax->GetXmax();
            }
         else
            { // values already initialized as zero
            }
         TAxis* zax=his->GetZaxis();
         if(zax)
            {
               fdZmin=zax->GetXmin();
               fdZmax=zax->GetXmax();
            }
         else
            { // values already initialized as zero
            }
         fdEntries = his->GetEntries();
         //////////////////////////////////////////////////////
         ///// correct object size for real size of data array:
         Int_t fieldsize=0;
         // because of multiple inheritance of histogram types, we
         // must check for all types separately to downcast the correct TArray:
         if(his->InheritsFrom(TH1D::Class()))
            {
                TH1D* dhis=dynamic_cast<TH1D*>(his);
                fieldsize=sizeof(Double_t)*dhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH1F::Class()))
            {
                TH1F* fhis=dynamic_cast<TH1F*>(his);
                fieldsize=sizeof(Float_t)*fhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH1I::Class()))
            {
                TH1I* ihis=dynamic_cast<TH1I*>(his);
                fieldsize=sizeof(Int_t)*ihis->GetSize(); // method of TArray baseclass
            }
          else if(his->InheritsFrom(TH1S::Class()))
            {
                TH1S* shis=dynamic_cast<TH1S*>(his);
                fieldsize=sizeof(Short_t)*shis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH1C::Class()))
            {
                TH1C* chis=dynamic_cast<TH1C*>(his);
                fieldsize=sizeof(Char_t)*chis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH2D::Class()))
            {
                TH2D* dhis=dynamic_cast<TH2D*>(his);
                fieldsize=sizeof(Double_t)*dhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH2F::Class()))
            {
                TH2F* fhis=dynamic_cast<TH2F*>(his);
                fieldsize=sizeof(Float_t)*fhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH2I::Class()))
            {
                TH2I* ihis=dynamic_cast<TH2I*>(his);
                fieldsize=sizeof(Int_t)*ihis->GetSize(); // method of TArray baseclass
            }
          else if(his->InheritsFrom(TH2S::Class()))
            {
                TH2S* shis=dynamic_cast<TH2S*>(his);
                fieldsize=sizeof(Short_t)*shis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH2C::Class()))
            {
                TH2C* chis=dynamic_cast<TH2C*>(his);
                fieldsize=sizeof(Char_t)*chis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH3D::Class()))
            {
                TH3D* dhis=dynamic_cast<TH3D*>(his);
                fieldsize=sizeof(Double_t)*dhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH3F::Class()))
            {
                TH3F* fhis=dynamic_cast<TH3F*>(his);
                fieldsize=sizeof(Float_t)*fhis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH3I::Class()))
            {
                TH3I* ihis=dynamic_cast<TH3I*>(his);
                fieldsize=sizeof(Int_t)*ihis->GetSize(); // method of TArray baseclass
            }
          else if(his->InheritsFrom(TH3S::Class()))
            {
                TH3S* shis=dynamic_cast<TH3S*>(his);
                fieldsize=sizeof(Short_t)*shis->GetSize(); // method of TArray baseclass
            }
         else if(his->InheritsFrom(TH3C::Class()))
            {
                TH3C* chis=dynamic_cast<TH3C*>(his);
                fieldsize=sizeof(Char_t)*chis->GetSize(); // method of TArray baseclass
            }
         fiObjectSize+=fieldsize; // add heap field to object stack memory size
         //////////////////////////////////////////////////////////////////

         if(allstatistics)
         {
         // note: we do not calculate statistic for nameslist
         // slows down when processing empty 2d histos!
         //cout <<"\t" <<GetName()<<" calculating mean/rms..." << endl;
         fdXmean=his->GetMean(1);
         fdYmean=his->GetMean(2);
         fdZmean=his->GetMean(3);
         //cout <<"\t"<<GetName()<<" finished mean." << endl;
         fdXrms=his->GetRMS(1);
         fdYrms=his->GetRMS(2);
         fdZrms=his->GetRMS(3);
         //cout <<"\t"<<GetName()<<" finished rms." << endl;
         }
      }
}



TGo4HistogramStatus::TGo4HistogramStatus() : TGo4ObjectStatus(),
fiDim(0),fiNbinX(0), fiNbinY(0), fiNbinZ(0),
fdXmin(0), fdXmax(0), fdYmin(0), fdYmax(0), fdZmin(0), fdZmax(0),
fdEntries(0), fdXmean(0), fdYmean(0), fdZmean(0),
fdXrms(0), fdYrms(0), fdZrms(0)
{
TRACE((14,"TGo4HistogramStatus::TGo4HistogramStatus()",__LINE__, __FILE__));

}


TGo4HistogramStatus::~TGo4HistogramStatus()
{
TRACE((14,"TGo4HistogramStatus::TGo4HistogramStatus()",__LINE__, __FILE__));

}




Int_t TGo4HistogramStatus::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4HistogramStatus::PrintStatus()",__LINE__, __FILE__));
   //
   if(buflen<=0 && buffer!=0)
      return 0;
   Int_t locallen=128000;
   Text_t localbuf[128000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta= TGo4ObjectStatus::PrintStatus(current,restlen);
   restlen-=delta;
   current+=delta;
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> Histogram Status Class Printout <-OOOO-G\n");
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   // put printout of histogram infos here:
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Dimension: \t%d\n",GetDimension());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " Bins X: \t%d\n",GetXbins());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " X min: \t\t%f\n",GetXmin());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " X max: \t\t%f\n",GetXmax());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " X mean: \t%f\n",GetXmean());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " X rms: \t\t%f\n",GetXrms());
   if(GetDimension()>1)
      {
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Bins Y: \t%d\n",GetYbins());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Y min: \t\t%f\n",GetYmin());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Y max: \t\t%f\n",GetYmax());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Y mean: \t%f\n",GetYmean());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Y rms: \t\t%f\n",GetYrms());
      }
   else
      { }
   if(GetDimension()>2)
      {
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Bins Z: \t%d\n",GetZbins());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Z min: \t\t%f\n",GetZmin());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Z max: \t\t%f\n",GetZmax());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Z mean: \t%f\n",GetZmean());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen, " Z rms: \t\t%f\n",GetZrms());
      }
   else
      { }
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, " N Entries: \t%f\n",GetEntries());
   current=PrintIndent(current,restlen);
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
  if(buffer==0)
      {
          cout << localbuf << endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}
