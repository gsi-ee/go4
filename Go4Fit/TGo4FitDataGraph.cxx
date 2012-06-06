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

#include "TGo4FitDataGraph.h"

#include "go4iostream.h"

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TArrayD.h"

TGo4FitDataGraph::TGo4FitDataGraph() :
   TGo4FitData(),
   fxGraph(this, TGraph::Class())
{
}

TGo4FitDataGraph::TGo4FitDataGraph(const char* iName, TGraph *iGraph, Bool_t iGraphOwned, Bool_t AddAmpl) :
   TGo4FitData(iName,"TGraph and derived classes", dtGraph, AddAmpl),
   fxGraph("Graph","Data object of TGraph class",this, TGraph::Class(), kTRUE, iGraph, iGraphOwned)
{
   SetExcludeLessThen(-1e50);
}

TGo4FitDataGraph::~TGo4FitDataGraph() {
}

TGo4FitDataIter* TGo4FitDataGraph::MakeIter()
{
   return new TGo4FitDataGraphIter(this);
}

void TGo4FitDataGraph::SetGraph(TGraph *iGraph, Bool_t iGraphOwned)
{
   fxGraph.SetObject(iGraph,iGraphOwned);
}

void TGo4FitDataGraph::FillSlotList(TSeqCollection* list)
{
   TGo4FitData::FillSlotList(list);
   list->Add(&fxGraph);
}

void TGo4FitDataGraph::Print(Option_t* option) const
{
   TGo4FitData::Print(option);
   cout << "   ";
   fxGraph.Print(option);
}

// *********************************************************************************

TGo4FitDataGraphIter::TGo4FitDataGraphIter() :
   TGo4FitDataIter(),
   fxData(0),
   fiNumPoints(0)
{
}

TGo4FitDataGraphIter::TGo4FitDataGraphIter(TGo4FitDataGraph* Data) :
   TGo4FitDataIter(),
   fxData(Data),
   fiNumPoints(0)
{
}

TGo4FitDataGraphIter::~TGo4FitDataGraphIter()
{
}

Bool_t TGo4FitDataGraphIter::StartReset()
{
  if ((fxData==0) || (fxData->GetGraph()==0)) return kFALSE;

  fiNumPoints = fxData->GetGraph()->GetN();

  return ReserveArrays(1, 1, kFALSE);
}

Bool_t TGo4FitDataGraphIter::ReadCurrentPoint()
{
   if (fxData==0) return kFALSE;
   TGraph* gr = fxData->GetGraph();
   if (gr==0) return kFALSE;

   Double_t* xx = gr->GetX();
   Double_t* yy = gr->GetY();
   if ((xx==0) || (yy==0)) return kFALSE;

   Double_t xvalue = xx[fxIndexes[0]];
   fdValue = yy[fxIndexes[0]];

   if (!GetDeviation()) {
      Double_t zn = gr->GetErrorY(fxIndexes[0]);
      if (zn>0) fdStandardDeviation = zn*zn;
           else fdStandardDeviation = 1.;
   }

   return ProduceScales(fxIndexes.GetArray(), &xvalue, 0);
}

Bool_t TGo4FitDataGraphIter::ShiftToNextPoint()
{
   fxIndexes[0]+=1;
   return (fxIndexes[0]<fiNumPoints);
}
