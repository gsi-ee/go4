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

#ifndef TGO4FITGUIARROW_H
#define TGO4FITGUIARROW_H

#include "TArrow.h"

class TGo4FitModel;
class TGo4FitComponent;
class QFitItem;
class TGo4FitPanel;

class TGo4FitGuiArrow : public TArrow {

public:

   enum { at_none, at_pos, at_width, at_range };

   TGo4FitGuiArrow();
   TGo4FitGuiArrow(Float_t arrowsize, Option_t* option);
   TGo4FitGuiArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Float_t arrowsize, Option_t* option);
   virtual ~TGo4FitGuiArrow();

   void SetForPosition(TGo4FitModel* model, TGo4FitGuiArrow* forwidth, Double_t iLineAmpl);
   void SetForWidth(TGo4FitModel* model, TGo4FitGuiArrow* forposition);
   void SetForRange(TGo4FitComponent* Comp, Int_t num, Double_t rangey, Double_t shiftx);
   void SetItem(QFitItem* item, TGo4FitPanel* panel);

   QFitItem* GetItem() { return fxItem; }

   Bool_t Locate();

   Int_t GetType() { return fxType; }
   TGo4FitModel* GetModel() { return fxModel; }
   TGo4FitComponent* GetComp() { return fxComp; }

   Bool_t IsAssignTo(TObject* obj);

   void ClearOther() { fxOther = 0; }
   virtual void Delete(Option_t* option="");   //  *MENU*

   virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

#ifdef __GO4WEB__
   virtual Bool_t Notify();
#endif

private:
   Int_t fxType;                    //!
   TGo4FitModel* fxModel;           //!
   TGo4FitGuiArrow* fxOther;        //!
   Double_t fdLineAmpl;             //!
   TGo4FitComponent* fxComp;        //!
   Int_t fiRangeNum;                //!
   Double_t fdRangeY;               //!
   Double_t fdShiftX;               //!
   QFitItem* fxItem;                //!
   TGo4FitPanel* fxPanel;           //!

   void TestNewPosition();
};




#endif
