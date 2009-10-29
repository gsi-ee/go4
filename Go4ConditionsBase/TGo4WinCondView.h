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

#ifndef TGO4WINCONDVIEW_H
#define TGO4WINCONDVIEW_H

#include "TBox.h"

class TGo4WinCond;

class TGo4WinCondView : public TBox {

public:

    TGo4WinCondView(Double_t x1,Double_t y1,Double_t x2,Double_t y2);
    TGo4WinCondView();

    virtual ~TGo4WinCondView();

    virtual void Paint(Option_t* opt="");
    virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

    void SetCondition(TGo4WinCond* condition) { fxWinCondition=condition; }
    TGo4WinCond* GetCondition() { return fxWinCondition; }
    const char* GetName() const;
    void SetName(const char* nam); // *MENU*
    /** Delete condition instead of view: */
    //void DeleteRegion(); // *MENU*
    void SetToLimits(Double_t x1, Double_t x2,Double_t y1=0, Double_t y2=0); // *MENU*
    void SetLabelDraw(Bool_t on); // *MENU*
    void SetLimitsDraw(Bool_t on); // *MENU*
    void SetIntDraw(Bool_t on); // *MENU*
    void SetXMeanDraw(Bool_t on); // *MENU*
    void SetXRMSDraw(Bool_t on); // *MENU*
    void SetYMeanDraw(Bool_t on); // *MENU*
    void SetYRMSDraw(Bool_t on); // *MENU*
    void SetXMaxDraw(Bool_t on); // *MENU*
    void SetYMaxDraw(Bool_t on); // *MENU*
    void SetCMaxDraw(Bool_t on); // *MENU*
    /** This will save draw flags into  static default setup */
    void SaveLabelStyle();// *MENU*
    /** Will reset label position to defaults */
    void ResetLabel();// *MENU*

private:

    /** pointer to viewed condition for update after graphical action */
    TGo4WinCond* fxWinCondition; //!

    ClassDef(TGo4WinCondView,6)
};
#endif //TGO4WINCONDVIEW_H
