#include "TGo4TreeHistogramEntry.h"

#include "Riostream.h"

#include "TTree.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4DynamicListException.h"

const char* TGo4TreeHistogramEntry::fgcENTRYSUF="-DL";

TGo4TreeHistogramEntry::TGo4TreeHistogramEntry() :
   TGo4DynamicEntry(),
   fxHistogramName(),
   fxVarExp(),
   fxCutExp(),
   fiDynListInterval(1000),
   fbNewHistogram(kTRUE),
   fiLastEvent(0)
{
   TRACE((15,"TGo4TreeHistogramEntry::TGo4TreeHistogramEntry()",__LINE__, __FILE__));
   fxTreeName="Go4Tree";
   EnableProcessing(kTRUE);
}

TGo4TreeHistogramEntry::TGo4TreeHistogramEntry(const char* histogramname,
                                               const char* treename,
                                               const char* varexp,
                                               const char* cutexp) :
   TGo4DynamicEntry(histogramname),
   fxHistogramName(histogramname),
   fxTreeName(treename),
   fxVarExp(varexp),
   fxCutExp(cutexp),
   fiDynListInterval(1000),
   fbNewHistogram(kTRUE),
   fiLastEvent(0)
{
   TRACE((15,"TGo4TreeHistogramEntry::TGo4TreeHistogramEntry(const Text_t*, const Text_t*, const Text_t*, TCut)",__LINE__, __FILE__));
   SetName(Form("%s%s",histogramname,fgcENTRYSUF)); // histogram name is different from entryname!
   EnableProcessing(kTRUE);

   Reset();
}

TGo4TreeHistogramEntry::~TGo4TreeHistogramEntry()
{
   TRACE((15,"TGo4TreeHistogramEntry::~TGo4TreeHistogramEntry()",__LINE__, __FILE__));
}

void TGo4TreeHistogramEntry::Reset()
{
   TRACE((12,"TGo4TreeHistogramEntry::Reset()",__LINE__, __FILE__));
   TGo4DynamicEntry::Reset();
   fiLastEvent=0;
   fbNewHistogram=kTRUE;
}

void TGo4TreeHistogramEntry::ProcessTreeNew(TTree* tree, Int_t times)
{
   if(times<=0) times=1;

   Int_t lastentrynumber = static_cast<Int_t> (tree->GetEntries());
   if(tree->TestBit(TGo4Status::kGo4BackStoreReset))
      fiLastEvent=0; // start again with first entry of backstore tree

   if(fiLastEvent==lastentrynumber)
      throw TGo4DynamicListException(this,
                     "Tree Histogram Entry: %s Reached end of tree %s. Draw() is stopped. ",
                     GetName(), tree->GetName());
   Int_t startindex= lastentrynumber -times;
   //cout << "\t -- init startindex: "<< startindex<< endl;
   if(startindex<fiLastEvent)
      {
         // we have to check if some of the events were not filled in the tree,
         // in this case we take all from the last processed event to the current:
         if(startindex>0)
            {
               // normal case in continuous processed tree
               startindex=fiLastEvent;
            }
            else
            {
               // this can happen when tree was Reset() in the meantime
               // e.g. tree of TGo4BackStore, then fiLastEvent is wrong
               startindex=0;
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
      cout <<"Passing over end of tree - NEVER COME HERE!!!" << endl;
      fiLastEvent=lastentrynumber;
      times=fiLastEvent-startindex;
      cout << "\t -- startindex: "<< startindex<< endl;
      cout << "\t -- times: "<< times<< endl;
      cout << "\t -- treeentries: "<< lastentrynumber<< endl;
   }

   TString drawexp = GetVarExp();
   drawexp += ">>+";
   drawexp += GetHistogramName();

   TDirectory* savdir = gDirectory;
   gROOT->cd(); // be sure to be in the top directory when processing
   tree->Draw(drawexp.Data(), fxCutExp.Data(), "goff", times, startindex);
   savdir->cd();
}


void TGo4TreeHistogramEntry::Print(Option_t* dummy) const
{
   // this trick is needed since root defines Print as const function...
   TROOT::IndentLevel();
   cout <<"-Dynamic TreeDraw Entry " << GetName() << " :" << endl;
   TROOT::IncreaseDirLevel();
   if(IsEnabledProcessing()) cout <<"\t-- Enabled --" << endl;
                        else cout <<"\t-- Disabled --" << endl;

   TROOT::IndentLevel();
   cout <<"\tHistogram:\t"<< GetHistogramName()<<endl;
   TROOT::IndentLevel();
   cout <<"\tTree:\t"<< GetTreeName()<<endl;
   TROOT::IndentLevel();
   cout <<"\tDraw Expression:\t" << GetVarExp() <<endl;
   TROOT::IndentLevel();
   cout <<"\tCut Expression:\t" << GetCutExp() << endl;
   TROOT::DecreaseDirLevel();
   TROOT::IndentLevel();
   cout <<"-End "<<GetName()<<"-----------" << endl;
}
