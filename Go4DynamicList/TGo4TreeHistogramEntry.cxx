// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4TreeHistogramEntry.h"

#include <iostream>

#include "TTree.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4DynamicListException.h"

const char *TGo4TreeHistogramEntry::fgcENTRYSUF = "-DL";

TGo4TreeHistogramEntry::TGo4TreeHistogramEntry() :
   TGo4DynamicEntry(),
   fxHistogramName(),
   fxVarExp(),
   fxCutExp(),
   fiDynListInterval(1000),
   fbNewHistogram(kTRUE),
   fiLastEvent(0)
{
   GO4TRACE((15,"TGo4TreeHistogramEntry::TGo4TreeHistogramEntry()",__LINE__, __FILE__));
   fxTreeName = "Go4Tree";
   EnableProcessing(kTRUE);
}

TGo4TreeHistogramEntry::TGo4TreeHistogramEntry(const char *histogramname,
                                               const char *treename,
                                               const char *varexp,
                                               const char *cutexp) :
   TGo4DynamicEntry(histogramname),
   fxHistogramName(histogramname),
   fxTreeName(treename),
   fxVarExp(varexp),
   fxCutExp(cutexp),
   fiDynListInterval(1000),
   fbNewHistogram(kTRUE),
   fiLastEvent(0)
{
   GO4TRACE((15,"TGo4TreeHistogramEntry::TGo4TreeHistogramEntry(const char *, const char *, const char *, const char *)",__LINE__, __FILE__));
   SetName(TString::Format("%s%s",histogramname,fgcENTRYSUF).Data()); // histogram name is different from entryname!
   EnableProcessing(kTRUE);

   Reset();
}

TGo4TreeHistogramEntry::~TGo4TreeHistogramEntry()
{
   GO4TRACE((15,"TGo4TreeHistogramEntry::~TGo4TreeHistogramEntry()",__LINE__, __FILE__));
}

void TGo4TreeHistogramEntry::Reset()
{
   GO4TRACE((12,"TGo4TreeHistogramEntry::Reset()",__LINE__, __FILE__));
   TGo4DynamicEntry::Reset();
   fiLastEvent = 0;
   fbNewHistogram = kTRUE;
}

void TGo4TreeHistogramEntry::ProcessTreeNew(TTree *tree, Int_t times)
{
   if(times <= 0) times = 1;

   Int_t lastentrynumber = static_cast<Int_t> (tree->GetEntries());
   if(tree->TestBit(TGo4Status::kGo4BackStoreReset))
      fiLastEvent = 0; // start again with first entry of backstore tree

   if(fiLastEvent==lastentrynumber)
      throw TGo4DynamicListException(this,
                 TString::Format("Tree Histogram Entry: %s Reached end of tree %s. Draw() is stopped. ", GetName(), tree->GetName()).Data());
   Int_t startindex = lastentrynumber - times;
   if(startindex < fiLastEvent)
      {
         // we have to check if some of the events were not filled in the tree,
         // in this case we take all from the last processed event to the current:
         if(startindex>0)
            {
               // normal case in continuous processed tree
               startindex = fiLastEvent;
            }
            else
            {
               // this can happen when tree was Reset() in the meantime
               // e.g. tree of TGo4BackStore, then fiLastEvent is wrong
               startindex = 0;
            }
         times=lastentrynumber-startindex;
      }
   else
      {
         // this happens when dynamic list entry started after the tree filling has begun
         // switch this on for cint tree processing:
         startindex=fiLastEvent;
         //times=lastentrynumber-startindex;
         // <- this would cause to process all events in tree since the last time this
         // dynamic entry was active. might be dangerous though, not useful...
      }

   fiLastEvent=startindex+times; // for cint mode

   if(fiLastEvent>lastentrynumber) {
      std::cout <<"Passing over end of tree - NEVER COME HERE!!!" << std::endl;
      fiLastEvent=lastentrynumber;
      times=fiLastEvent-startindex;
      std::cout << "\t -- startindex: "<< startindex<< std::endl;
      std::cout << "\t -- times: "<< times<< std::endl;
      std::cout << "\t -- treeentries: "<< lastentrynumber<< std::endl;
   }

   TString drawexp = GetVarExp();
   drawexp += ">>+";
   drawexp += GetHistogramName();

   TDirectory *savdir = gDirectory;
   gROOT->cd(); // be sure to be in the top directory when processing
   tree->Draw(drawexp.Data(), fxCutExp.Data(), "goff", times, startindex);
   savdir->cd();
}


void TGo4TreeHistogramEntry::Print(Option_t *) const
{
   // this trick is needed since root defines Print as const function...
   TROOT::IndentLevel();
   std::cout <<"-Dynamic TreeDraw Entry " << GetName() << " :" << std::endl;
   TROOT::IncreaseDirLevel();
   if(IsEnabledProcessing()) std::cout <<"\t-- Enabled --" << std::endl;
                        else std::cout <<"\t-- Disabled --" << std::endl;

   TROOT::IndentLevel();
   std::cout <<"\tHistogram:\t"<< GetHistogramName()<<std::endl;
   TROOT::IndentLevel();
   std::cout <<"\tTree:\t"<< GetTreeName()<<std::endl;
   TROOT::IndentLevel();
   std::cout <<"\tDraw Expression:\t" << GetVarExp() <<std::endl;
   TROOT::IndentLevel();
   std::cout <<"\tCut Expression:\t" << GetCutExp() << std::endl;
   TROOT::DecreaseDirLevel();
   TROOT::IndentLevel();
   std::cout <<"-End "<<GetName()<<"-----------" << std::endl;
}
