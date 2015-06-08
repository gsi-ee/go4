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

#ifndef TGO4COMPOSITEEVENT_H
#define TGO4COMPOSITEEVENT_H

#include "TGo4EventElement.h"

class TObjArray;
class TTree;

/** Base type for object composition */
class TGo4CompositeEvent : public TGo4EventElement {
  protected:

    /** Number of Elements in composite */
    Short_t fNElements;

    /** Sub-Events list for this event */
    TObjArray *fEventElements; //!

    /** Maximum index (i.e. array size to provide when reading back tree) */
    Short_t fMaxIndex;

  public:
     TGo4CompositeEvent();
     TGo4CompositeEvent(const char* aName, const char* aTitle, Short_t aBaseCat=0);
     virtual ~TGo4CompositeEvent();

     virtual TGo4EventElement* GetChild(const char* name);

     virtual void makeBranch(TBranch *parent);

     virtual Int_t activateBranch(TBranch *branch, Int_t init = 0, TGo4EventElement** var_ptr = 0);

     virtual void deactivate();
     virtual void activate();

     Bool_t addEventElement(TGo4EventElement* aElement, Bool_t reading = kFALSE);

     virtual TTree* CreateSampleTree(TGo4EventElement** sample = 0);

     TGo4EventElement *getEventElement(Int_t idx);
     TGo4EventElement *getEventElement(const char* name,Int_t final=0);
     void Clear(Option_t *opt="");
     Short_t getNElements() const { return fNElements; }
     Short_t getMaxIndex() const { return fMaxIndex; }
     virtual Bool_t isComposed() { return kTRUE; }
     TObjArray* getElements() { return fEventElements;}
     TObjArray* getListOfComposites(Bool_t toplevel=kTRUE);
     TGo4EventElement& operator[]( Int_t i );

  protected:

     void ProvideArray();


  ClassDef(TGo4CompositeEvent,2)
};

#endif
