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

#ifndef TGO4COMPOSITEEVENT_H
#define TGO4COMPOSITEEVENT_H

#include "TGo4EventElement.h"

class TObjArray;
class TTree;

/** Base type for object composition */
class TGo4CompositeEvent : public TGo4EventElement {
  protected:

    /** Number of Elements in composite */
    Short_t fNElements{0};

    /** Sub-Events list for this event */
    TObjArray *fEventElements{nullptr}; //!

    /** Maximum index (i.e. array size to provide when reading back tree) */
    Short_t fMaxIndex{0};

  public:
     TGo4CompositeEvent();
     TGo4CompositeEvent(const char* aName, const char* aTitle, Short_t aBaseCat=0);
     virtual ~TGo4CompositeEvent();

     TGo4EventElement* GetChild(const char* name) override;

     void makeBranch(TBranch *parent) override;

     Int_t activateBranch(TBranch *branch, Int_t init = 0, TGo4EventElement** var_ptr = nullptr) override;

     void deactivate() override;
     void activate() override;

     Bool_t addEventElement(TGo4EventElement* aElement, Bool_t reading = kFALSE);

     TTree* CreateSampleTree(TGo4EventElement** sample = nullptr) override;

     TGo4EventElement *getEventElement(Int_t idx);
     TGo4EventElement *getEventElement(const char* name,Int_t final=0);
     void Clear(Option_t *opt="") override;
     Short_t getNElements() const { return fNElements; }
     Short_t getMaxIndex() const { return fMaxIndex; }
     Bool_t isComposed() override { return kTRUE; }
     TObjArray* getElements() { return fEventElements;}
     TObjArray* getListOfComposites(Bool_t toplevel=kTRUE);
     TGo4EventElement& operator[](Int_t i) override;

  protected:

     void ProvideArray();


  ClassDefOverride(TGo4CompositeEvent,2)
};

#endif
