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

#ifndef TGO4TREEHISTOGRAMENTRY_H
#define TGO4TREEHISTOGRAMENTRY_H

#include "TGo4DynamicEntry.h"

#include "TString.h"

class TTree;

/**
 * Dynamic list entry which links a histogram to a certain tree.
 * Method Process performs a single event TTree::Draw on this
 * tree and directs the output to the histogram. String Parameters
 * Varexp and Selection of Draw() are stored in the entry and are
 * used for each Draw. This dynamic list entry does _not_ use
 * the Go4 Conditions and event index scheme!
 * @author J. Adamczewski
 * @since 1/2001
 */

class TGo4TreeHistogramEntry : public TGo4DynamicEntry {

    friend class TGo4DynamicList;

  public:
    /** default ctor for streamer. */
    TGo4TreeHistogramEntry();

    TGo4TreeHistogramEntry(const char* histogramname,
                           const char* treename,
                           const char* varexp,
                           const char* cutexp);

    virtual ~TGo4TreeHistogramEntry();

    /** Set name of the external histogram to be filled. */
    void SetHistogramName(const char* name) { fxHistogramName = name; }
    /** Get name of the external histogram to be filled. */
    const char* GetHistogramName() const { return fxHistogramName.Data(); }

    void SetTreeName(const char* name) { fxTreeName = name; }
    const char* GetTreeName() const { return fxTreeName.Data(); }

    void SetVarExp(const char* exp) { fxVarExp = exp; }
    const char* GetVarExp() const { return fxVarExp.Data(); }

    void SetCutExp(const char* cut) { fxCutExp = cut; }
    const char* GetCutExp() const { return fxCutExp.Data(); }

    void SetDynListInterval(Int_t value) { fiDynListInterval = value; }
    Int_t GetDynListInterval() const { return fiDynListInterval; }

    /** Resets this entry to an initial status. */
    virtual void Reset();

    virtual void Print(Option_t * dummy="") const;

    /** suffix for entryname to differ from histogram name */
    static const char* fgcENTRYSUF; //!

  protected:
     void ProcessTreeNew(TTree* tree, Int_t times);

  private:

   /** The target histogram name to be filled */
   TString fxHistogramName;

   /** The Treename to find the tree */
   TString fxTreeName;

   /** The varexp string needed by TTree:Draw() */
   TString fxVarExp;

   /** The selection string needed by TTree:Draw() */
   TString fxCutExp;

   Int_t fiDynListInterval;

   /** True before Process is called the first time. Then the histogram
     * created by the TTree::Draw is added to the histogram folder once. */
   Bool_t fbNewHistogram; //!

   /** index of last tree entry of the previous Process() call */
   Int_t fiLastEvent;     //!

  ClassDef(TGo4TreeHistogramEntry,3)

};

#endif //TGO4TREEHISTOGRAMENTRY_H
