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

  public:
     TGo4CompositeEvent();
     TGo4CompositeEvent(const char* aName, const char* aTitle, Short_t aBaseCat=0);
     ~TGo4CompositeEvent();

     void makeBranch(TBranch *parent);
     void synchronizeWithTree(TTree *tree, Int_t splitLevel=0);
     Int_t activateBranch(TBranch *branch, Int_t splitLevel=0,Int_t init=0);

     virtual void deactivate();
     virtual void activate();

     void addEventElement(TGo4EventElement* aElement, Int_t reading =0);
     TGo4EventElement *getEventElement(Int_t idx);
     TGo4EventElement *getEventElement(const char* name,Int_t final=0);
     void Clear(Option_t *opt="");
     void clearAll(Int_t level);
     Short_t getNElements() const { return fNElements; }

     virtual Bool_t isComposed() { return kTRUE; }
     TObjArray* getElements(){ return fEventElements;}
     TObjArray* getListOfComposites(Bool_t toplevel=kTRUE);
     TGo4EventElement& operator[]( Int_t i );

  ClassDef(TGo4CompositeEvent,1)
};

#endif
