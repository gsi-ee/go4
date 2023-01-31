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

#ifndef TGO4POLYCONDVIEW_H
#define TGO4POLYCONDVIEW_H

#include "TCutG.h"
#include "TGo4PolyCond.h"

class TGo4PolyCondView : public TCutG {

public:

    TGo4PolyCondView(TCutG* source);
    TGo4PolyCondView();
    virtual ~TGo4PolyCondView();

    void Paint(Option_t* opt) override;
    void ExecuteEvent(Int_t event, Int_t px, Int_t py) override;

    /** Check if cut on pad is changed compared with condition */
    Bool_t IsCutChanged() const;
    /**Copy modified view values back to condition */
    virtual void UpdateCondition();

    void SetCondition(TGo4PolyCond *condition) { fxPolyCondition = condition; }

    TGo4PolyCond *GetCondition() { return fxPolyCondition; }

    /** Set this view to values of cut source*/
    void SetCut(TCutG* source);

    /** create plain cutg from this view. */
    TCutG* CreateCut();

    const char *GetName() const override;
    void SetName(const char *nam) override; // *MENU*
     /** Delete condition instead of view: */
    void DeleteRegion(); // *MENU*

    void SetLabelDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetLabelDraw(on); } // *TOGGLE* *GETTER=IsLabelDraw
    void SetLimitsDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetLimitsDraw(on); } // *TOGGLE* *GETTER=IsLimitsDraw
    void SetIntDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetIntDraw(on); } // *TOGGLE* *GETTER=IsIntDraw
    void SetXMeanDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetXMeanDraw(on); } // *TOGGLE* *GETTER=IsXMeanDraw
    void SetXRMSDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetXRMSDraw(on); } // *TOGGLE* *GETTER=IsXRMSDraw
    void SetYMeanDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetYMeanDraw(on);  } // *TOGGLE* *GETTER=IsYMeanDraw
    void SetYRMSDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetYRMSDraw(on); } // *TOGGLE* *GETTER=IsYRMSDraw
    void SetXMaxDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetXMaxDraw(on); } // *TOGGLE* *GETTER=IsXMaxDraw
    void SetYMaxDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetYMaxDraw(on); } // *TOGGLE* *GETTER=IsYMaxDraw
    void SetCMaxDraw(Bool_t on) { if(fxPolyCondition) fxPolyCondition->SetCMaxDraw(on); } // *TOGGLE* *GETTER=IsCMaxDraw

    Bool_t IsLabelDraw() { return fxPolyCondition ? fxPolyCondition->IsLabelDraw() : kFALSE; }
    Bool_t IsLimitsDraw() { return fxPolyCondition ? fxPolyCondition->IsLimitsDraw() : kFALSE; }
    Bool_t IsIntDraw() { return fxPolyCondition ? fxPolyCondition->IsIntDraw() : kFALSE; }
    Bool_t IsXMeanDraw() { return fxPolyCondition ? fxPolyCondition->IsXMeanDraw() : kFALSE; }
    Bool_t IsXRMSDraw() { return fxPolyCondition ? fxPolyCondition->IsXRMSDraw() : kFALSE; }
    Bool_t IsYMeanDraw() { return fxPolyCondition ? fxPolyCondition->IsYMeanDraw() : kFALSE; }
    Bool_t IsYRMSDraw() { return fxPolyCondition ? fxPolyCondition->IsYRMSDraw() : kFALSE; }
    Bool_t IsXMaxDraw() { return fxPolyCondition ? fxPolyCondition->IsXMaxDraw() : kFALSE; }
    Bool_t IsYMaxDraw() { return fxPolyCondition ? fxPolyCondition->IsYMaxDraw() : kFALSE; }
    Bool_t IsCMaxDraw() { return fxPolyCondition ? fxPolyCondition->IsCMaxDraw() : kFALSE; }

    /** This will save draw flags into  static default setup */
    void SaveLabelStyle();// *MENU*
    /** Will reset label position to defaults */
    void ResetLabel();// *MENU*

    using TCutG::RemovePoint;
    Int_t InsertPoint() override; // *MENU*
    Int_t RemovePoint() override; // *MENU*

    Bool_t IsAtExecuteMouseEvent() const { return fbExecutesMouseEvent; }
    Bool_t IsAtExecuteMouseMenu() const { return fbExecutesMouseMenu; }

private:

    /** JAM this one will suppress condition repaint during execution of mouse events*/
    Bool_t fbExecutesMouseEvent{kFALSE};

    /** JAM this one will suppress condition repaint during mouse menu execution*/
    Bool_t fbExecutesMouseMenu{kFALSE};

    /** pointer to viewed condition for update after graphical action */
    TGo4PolyCond *fxPolyCondition{nullptr}; //!

    ClassDefOverride(TGo4PolyCondView,7)
};

#endif //TGO4POLYCONDVIEW_H
