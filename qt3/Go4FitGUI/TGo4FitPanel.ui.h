// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------


#include "Riostream.h"


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
};

// ********************************************************************

TGo4FitGuiArrow::TGo4FitGuiArrow() :
    TArrow(), fxType(at_none), fxModel(0), fxOther(0), fdLineAmpl(0.), fxComp(0), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(0), fxPanel(0)
{
}

TGo4FitGuiArrow::TGo4FitGuiArrow(Float_t arrowsize, Option_t* option) :
    TArrow(0.,0.,0.,0.,arrowsize,option), fxType(at_none), fxModel(0), fxOther(0), fdLineAmpl(0.), fxComp(0), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(0), fxPanel(0)
{
}

TGo4FitGuiArrow::TGo4FitGuiArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Float_t arrowsize, Option_t* option) :
    TArrow(x1,y1,x2,y2,arrowsize,option), fxType(at_none), fxModel(0), fxOther(0), fdLineAmpl(0.), fxComp(0), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(0), fxPanel(0)
{
}

TGo4FitGuiArrow::~TGo4FitGuiArrow()
{
}

void TGo4FitGuiArrow::SetForPosition(TGo4FitModel* model, TGo4FitGuiArrow* forwidth, Double_t iLineAmpl)
{
  fxType = at_pos;
  fxModel = model;
  fxOther = forwidth;
  fdLineAmpl = iLineAmpl;
}

void TGo4FitGuiArrow::SetForWidth(TGo4FitModel* model, TGo4FitGuiArrow* forposition)
{
  fxType = at_width;
  fxModel = model;
  fxOther = forposition;
}

void TGo4FitGuiArrow::SetForRange(TGo4FitComponent* Comp, Int_t num, Double_t rangey, Double_t shiftx) {
  fxType = at_range;
  fxComp = Comp;
  fiRangeNum = num;
  fdRangeY = rangey;
  fdShiftX = shiftx;
}

void TGo4FitGuiArrow::SetItem(QFitItem* item, TGo4FitPanel* panel)
{
   fxItem = item;
   fxPanel = panel;
}

Bool_t TGo4FitGuiArrow::Locate()
{
  Double_t x1 = GetX1(), x2 = GetX2();
  Double_t y1 = GetY1(), y2 = GetY2();

  switch (fxType) {
    case at_pos:
      Double_t pos;
      if (fxModel->GetPosition(0,pos)) {
         SetX1(pos); SetX2(pos); SetY1(fdLineAmpl/2); SetY2(fdLineAmpl);
      }
      break;
    case at_width:
      Double_t width, ppp;
      if ((fxModel->GetWidth(0,width)) && (fxModel->GetPosition(0,ppp))) {
         SetX1(ppp-width); SetX2(ppp+width);
         SetY1(fxOther->GetY1());
         SetY2(fxOther->GetY1());
      }
      break;
    case at_range:
      Int_t typ, naxis;
      Double_t left, right;
      fxComp->GetRangeCondition(fiRangeNum, typ, naxis, left, right);
      switch (typ) {
        case 0: SetX1(left); SetX2(right); break;
        case 1: SetX1(left); SetX2(right); break;
        case 2: SetX1(left); SetX2(left+fdShiftX); break;
        case 3: SetX1(right-fdShiftX); SetX2(right); break;
      }
      SetY1(fdRangeY); SetY2(fdRangeY);

      break;
  }

  return (x1 != GetX1()) || (x2 != GetX2()) || (y1 != GetY1()) || (y2 != GetY2());
}

Bool_t TGo4FitGuiArrow::IsAssignTo(TObject* obj)
{
   return (obj!=0) && ((obj==fxModel) || (obj==fxComp));
}

void TGo4FitGuiArrow::Delete(Option_t* option)
{
  if ((fxType==at_pos) && (fxPanel!=0))
    fxPanel->DeleteModelWithPrimit(this);
}

void TGo4FitGuiArrow::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    TArrow::ExecuteEvent(event,px,py);
    if (event!=kButton1Up) return;

    switch (fxType) {

      case at_pos: {
         fxModel->SetPosition(0, GetX2());
         fdLineAmpl = (GetY1()>GetY2()) ? GetY1() : GetY2();
         if (fxPanel) fxPanel->ArrowChanged(this);
         break; }

      case at_width: {
        Double_t width = 0;
        fxModel->GetWidth(0,width);
        Double_t pos = fxOther->GetX2();

        Double_t w0 = (GetX2()-GetX1())/2;
        Double_t w1 = pos-GetX1();
        Double_t w2 = GetX2()-pos;

        if (TMath::Abs(w0-width)>1e-10) {
           Double_t w = width;
           if (TMath::Abs(w1-width)>1e-10) w=w1; else w=w2;
           if (w>0) {
             width=w;
             fxModel->SetWidth(0,width);
             if (fxPanel) fxPanel->ArrowChanged(this);
           }
        }
        break; }

      case at_range: {
        Int_t typ, naxis;
        Double_t left, right;
        fxComp->GetRangeCondition(fiRangeNum,typ,naxis,left,right);

        left = GetX1()<GetX2() ? GetX1() : GetX2();
        right = GetX1()>GetX2() ? GetX1() : GetX2();

        fxComp->SetRangeCondition(fiRangeNum,typ,naxis,left,right);

        if (fxPanel) fxPanel->ArrowChanged(this);

        break; }

    }
}

// *************************************************************************


static const char* SelectedXPM[]={
"12 12 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"############",
"#aaaaaaaaaa#",
"#aaaaaaa##a#",
"#aaaaaaa##a#",
"#aaaaaa##aa#",
"#a##aaa##aa#",
"#a##aa##aaa#",
"#aa##a##aaa#",
"#aa####aaaa#",
"#aaa##aaaaa#",
"#aaaaaaaaaa#",
"############"};

static const char* UnselectedXPM[]={
"12 12 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"############",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"#aaaaaaaaaa#",
"############"};

void TGo4FitPanel::init()
{
    // fitstatus part
//    fbNeedConfirmation = false;
//    fbShowPrimitives = true;
    fbFreezeMode = false;
    fiPanelMode = FitGui::pm_Wizard;
    fbParsWidgetShown = false;
//    fbDrawModels = true;
//    fbDrawComponents = false;
//    fbDrawBackground = false;
//    fbUseSamePanelForDraw = true;
//    fbDrawInfoOnPad = false;
//    fiIntegralMode = 0;
//    fbRecalculateGaussWidth = true;
//    fiBuffersUsage = 1;
//    fbSaveWithReferences = false;
//    fbUseCurrentRange = true;

   fbNeedConfirmation = go4sett->getBool("/FitPanel/NeedConfirmation", false);
   fbShowPrimitives = go4sett->getBool("/FitPanel/ShowPrimitives", true);
   fbSaveWithReferences = go4sett->getBool("/FitPanel/SaveWithReferences", false);
   fbUseCurrentRange = go4sett->getBool("/FitPanel/UseCurrentRange", true);
   fbDrawModels = go4sett->getBool("/FitPanel/DrawModels", true);
   fbDrawComponents = go4sett->getBool("/FitPanel/DrawComponents", false);
   fbUseSamePanelForDraw = go4sett->getBool("/FitPanel/UseSamePanelForDraw",true);
   fbDrawBackground = go4sett->getBool("/FitPanel/DrawBackground", false);
   fbDrawInfoOnPad = go4sett->getBool("/FitPanel/DrawInfoOnPad", false);
   fbRecalculateGaussWidth = go4sett->getBool("/FitPanel/RecalculateGaussWidth", true);
   fiIntegralMode = go4sett->getInt("/FitPanel/IntegralMode", 0);
   fiBuffersUsage = go4sett->getInt("/FitPanel/BuffersUsage", 1);

    // fit options part
    fxWizDataName = "";
    fxWizModelName = "";
    fbWizShowAllModels = true;
    fiWizPageIndex = 0;
    fbUseAmplEstim = false;
    fiNumMigradIter = 0;

    fxCurrentItem = 0;

    fxActivePanel = 0;
    fxActivePad = 0;

    fxDrawNewPanel = 0;
    fbDrawPanelCreation = false;

    TGo4WorkSpace* ws = TGo4WorkSpace::Instance();
    connect(ws, SIGNAL(panelSignal(TGo4ViewPanel*, TPad*, int)),
            this, SLOT(panelSlot(TGo4ViewPanel*, TPad*, int)));

    MenuBar = new QMenuBar( this, "menubar" );
    MenuBar->setFrameShape(QFrame::NoFrame);

    FitterMenu = new QPopupMenu( this );
    MenuBar->insertItem( "&Fitter",FitterMenu);
    connect(FitterMenu, SIGNAL(aboutToShow()), this, SLOT(AboutToShowFitterMenu()) );

    ViewMenu = new QPopupMenu( this );
    MenuBar->insertItem( "&Tools",ViewMenu);
    connect(ViewMenu, SIGNAL(aboutToShow()), this, SLOT(AboutToShowViewMenu()) );

    SettMenu = new QPopupMenu( this );
    MenuBar->insertItem( "&Settings", SettMenu);
    connect(SettMenu, SIGNAL(aboutToShow()), this, SLOT(AboutToShowSettMenu()) );

    SettMenu->insertItem("&Confirmation", this, SLOT(ChangeSettings(int)), 0, 1);
    SettMenu->insertItem("&Show primitives", this, SLOT(ChangeSettings(int)), 0, 2);
    SettMenu->insertItem("&Freeze mode", this, SLOT(ChangeSettings(int)), 0, 3);
    SettMenu->insertItem("&Save with objects", this, SLOT(ChangeSettings(int)), 0, 4);

    SettMenu->insertSeparator();

    SettMenu->insertItem("&Use current range", this, SLOT(ChangeSettings(int)), 0, 10);
    SettMenu->insertItem("&Draw model", this, SLOT(ChangeSettings(int)), 0, 11);
    SettMenu->insertItem("Draw &background", this, SLOT(ChangeSettings(int)), 0, 14);
    SettMenu->insertItem("Dra&w components", this, SLOT(ChangeSettings(int)), 0, 12);
    SettMenu->insertItem("Draw on same &pad", this, SLOT(ChangeSettings(int)), 0, 13);
    SettMenu->insertItem("Draw &info on pad", this, SLOT(ChangeSettings(int)), 0, 15);

    SettMenu->insertSeparator();

    SettMenu->insertItem("&No &integral", this, SLOT(ChangeSettings(int)), 0, 17);
    SettMenu->insertItem("&Counts", this, SLOT(ChangeSettings(int)), 0, 18);
    SettMenu->insertItem("&Integral", this, SLOT(ChangeSettings(int)), 0, 19);
    SettMenu->insertItem("&Gauss integral", this, SLOT(ChangeSettings(int)), 0, 20);

    SettMenu->insertItem("Recalculate gauss &widths", this, SLOT(ChangeSettings(int)), 0, 16);

    SettMenu->insertSeparator();

    SettMenu->insertItem("&Do not use buffers", this, SLOT(ChangeSettings(int)), 0, 21);
    SettMenu->insertItem("&Only for data", this, SLOT(ChangeSettings(int)), 0, 22);
    SettMenu->insertItem("&For data and models", this, SLOT(ChangeSettings(int)), 0, 23);
    SettMenu->insertItem("&Individual settings", this, SLOT(ChangeSettings(int)), 0, 24);

    ItemMenu = 0;

    fxWizPars = new TObjArray();
    fxWizSlots = new TObjArray();
    fxParsTableList = new TObjArray();

    fiPaintMode = 0;

    TGo4FitPanelLayout->setMargin(3);
    TGo4FitPanelLayout->setSpacing(1);

    UpdateActivePage();
}

void TGo4FitPanel::destroy()
{
   // discards messages from deleted items
   fxCurrentItem = 0;
   FitList->clear();

   delete fxParsTableList;
   delete fxWizSlots;
   delete fxWizPars;

   RemovePrimitives();
   RemoveDrawObjects();
   CloseDrawPanel();

   ClearObjectReferenceInSlots();
}

// *********************** overloaded methods of QGo4Widget **************************


bool TGo4FitPanel::IsAcceptDrag( const char* itemname, TClass * cl, int kind )
{
   if (cl==0) return false;

   return (cl->InheritsFrom(TGo4Fitter::Class())) ||
          (cl->InheritsFrom(TH1::Class())) ||
          (cl->InheritsFrom(TGraph::Class()));
}

void TGo4FitPanel::DropOnPanel( QDropEvent* event, const char * itemname, TClass * cl, int kind)
{
   if (cl==0) return;

   if (cl->InheritsFrom(TGo4Fitter::Class())) {
      WorkWithFitter(itemname, 0, 0);
      return;
   }

   if (event==0) return;

   QWidget* w = childAt(event->pos(), TRUE);

   if (w == Wiz_DataSlotsTable->viewport()) {
      QPoint pnt = Wiz_DataSlotsTable->viewport()->mapFrom(this, event->pos());
      pnt = Wiz_DataSlotsTable->viewportToContents(pnt);
      int nrow = Wiz_DataSlotsTable->rowAt(pnt.y());

      if ((nrow<0) || (nrow>fxWizSlots->GetLast())) return;

      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (fxWizSlots->At(nrow));

      if (slot==0) return;
      if (!slot->IsSuitableClass(cl)) return;

      CreateFitSlotLink(slot, itemname);

      UpdateActivePage();
   } else

   if(w == (QWidget*)FitList->viewport()) {
      QPoint pnt = FitList->viewport()->mapFrom(this, event->pos());
      QFitItem* item = dynamic_cast<QFitItem*> (FitList->itemAt(pnt));
      if ((item==0) || (item->ObjectType()!=FitGui::ot_slot)) return;

      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*>(item->Object());
      if (slot==0) return;
      if (!slot->IsSuitableClass(cl)) return;

      CreateFitSlotLink(slot, itemname);

      UpdateActivePage();
   }
}

void TGo4FitPanel::linkedObjectUpdated( const char * linkname, TObject * obj )
{
   UpdateObjectReferenceInSlots();

   UpdateActivePage();
}

void TGo4FitPanel::RemoveFitterLink()
{
   RemoveLink("Fitter");
}

void TGo4FitPanel::linkedObjectRemoved( const char * linkname )
{
   if (strcmp(linkname,"Fitter")==0) {
      RemoveAllLinks();
      fxActivePanel = 0;
      fxActivePad = 0;
   } else {
      UpdateObjectReferenceInSlots();
   }

   UpdateActivePage();
}

void TGo4FitPanel::linkedRemoved(TGo4Slot* slot, TObject* obj)
{
}


void TGo4FitPanel::WorkWithFitter(const char* itemname, TGo4ViewPanel* panel, TPad* pad)
{
   RemovePrimitives();
   RemoveDrawObjects();
   CloseDrawPanel();
   ClearObjectReferenceInSlots();

   fxActivePanel = panel;
   fxActivePad = pad;

   RemoveFitterLink();
   AddLink(itemname, "Fitter");

   TObject* obj = GetLinked("Fitter",2);

   UpdateObjectReferenceInSlots();

   UpdateActivePage();
}

void TGo4FitPanel::WorkWithFitterInSlot(TGo4Slot* slot)
{
   RemovePrimitives();
   RemoveDrawObjects();
   CloseDrawPanel();
   ClearObjectReferenceInSlots();

   fxActivePanel = 0;
   fxActivePad = 0;

   RemoveFitterLink();
   AddLink(slot, "Fitter");

   TObject* obj = GetLinked("Fitter",0);

   UpdateObjectReferenceInSlots();

   UpdateActivePage();
}

// **************************** interface to MainGUI ************************

void TGo4FitPanel::panelSlot(TGo4ViewPanel* panel, TPad* pad, int signalid)
{
   switch (signalid) {
      case panel_Activated: {
         if ((panel==0) || (pad==0)) return;

         if (fbDrawPanelCreation) return;
         if ((panel==fxDrawNewPanel) || fbDrawPanelCreation) return;

         bool update = fbFreezeMode;

         if (!update && (GetFitter()!=0) && (GetFitter()->GetNumData()>1))
            update = true;

         if (WorkingWithPanel() &&
             (panel==ActivePanel()) &&
             (pad==ActivePad()) ) update = true;

         if (update) {
            UpdateObjectReferenceInSlots();
            if (fiPanelMode==FitGui::pm_Expert) UpdateItemsOfType(FitGui::ot_slot, 0);
            if (fiPanelMode==FitGui::pm_Wizard) UpdateWizStackWidget();
            return;
         }

         RemovePrimitives();
         RemoveDrawObjects();
         CloseDrawPanel();
         ClearObjectReferenceInSlots();

         RemoveFitterLink();

         fxActivePanel = panel;
         fxActivePad = pad;

         if ((panel!=0) && (pad!=0)) {
            TGo4Slot* slot = panel->GetDrawObjectSlot(pad, "::Fitter");
            AddLink(slot, "Fitter");
         }

         UpdateObjectReferenceInSlots();

         UpdateActivePage();

         break;
      }

      case panel_Modified: {
         if (!WorkingWithPanel() || (panel!=ActivePanel())) return;
         RemovePrimitives();
         CloseDrawPanel();
         UpdateObjectReferenceInSlots();
         if (fiPanelMode==FitGui::pm_Wizard) UpdateWizStackWidget();
         if (fiPanelMode==FitGui::pm_Expert) UpdateItemsOfType(FitGui::ot_slot, 0);
         break;
      }

      case panel_Deleted: {
         if (panel==0) return;
         if (panel==fxDrawNewPanel) { fxDrawNewPanel = 0; return; }
         if (WorkingWithPanel() && (panel==ActivePanel())) {
            ClearObjectReferenceInSlots();
            RemoveFitterLink();
            fxActivePanel = 0;
            fxActivePad = 0;
         }

         UpdateActivePage();
         break;
      }
   }
}

// ************************ functionality of fitslots classes *****************************


bool TGo4FitPanel::WorkingWithPanel()
{
   return (fxActivePanel!=0) && (fxActivePad!=0);
}

bool TGo4FitPanel::WorkingWithOnlyPad()
{
  return WorkingWithPanel() ? !ActivePanel()->IsPadHasSubPads(ActivePad()) : false;
}

TGo4ViewPanel* TGo4FitPanel::ActivePanel()
{
   return fxActivePanel;
}

TPad* TGo4FitPanel::ActivePad()
{
   return fxActivePad;
}

TGo4Fitter* TGo4FitPanel::GetFitter()
{
   return dynamic_cast<TGo4Fitter*>(GetLinked("Fitter",0));
}

void TGo4FitPanel::SetFitter(TGo4Fitter* fitter)
{
   ClearObjectReferenceInSlots();
   RemoveFitterLink();
   if (!WorkingWithPanel()) {
      if (fitter!=0) {
         QString itemname = SaveObjectInMemory("FitPanel", fitter);
         AddLink(itemname, "Fitter");
      }
   } else {
      ActivePanel()->DeleteDrawObject(ActivePad(),"::Fitter");
      TGo4Slot* slot = ActivePanel()->AddDrawObject(
                         ActivePad(),
                         TGo4ViewPanel::kind_Fitter,
                         "::Fitter",
                         fitter, true, 0);
      if (slot!=0)
         AddLink(slot, "Fitter");
   }
   UpdateObjectReferenceInSlots();
   fbParsWidgetShown = false;
}

TGo4Fitter* TGo4FitPanel::CloneFitter()
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return 0;

   fitter->SetSaveFlagForObjects(fbSaveWithReferences);

   return dynamic_cast<TGo4Fitter*> (fitter->Clone());
}

void TGo4FitPanel::Fitter_New()
{
  if (WorkingWithPanel()) {
     ClearObjectReferenceInSlots();
     fxActivePanel = 0;
     fxActivePad = 0;
  }

  TGo4Fitter* fitter = new TGo4Fitter("Fitter","Fitter object");
  SetFitter(fitter);

  UpdateActivePage();
}


void TGo4FitPanel::Fitter_NewForActivePad(bool overwrite)
{
   TGo4ViewPanel* panel = LastActivePanel();
   if ((panel==0) || (panel==fxDrawNewPanel)) return;
   TPad* pad = panel->GetActivePad();

   ClearObjectReferenceInSlots();

   RemoveFitterLink();

   fxActivePanel = panel;
   fxActivePad = pad;

   if ((panel!=0) && (pad!=0)) {
      TGo4Slot* fitterslot = panel->GetDrawObjectSlot(pad, "::Fitter");

      if (!overwrite && (fitterslot!=0))
        AddLink(fitterslot, "Fitter");
   }

   if (overwrite || (GetFitter()==0))
      SetFitter(CreateFitterFor(panel, pad, "Fitter"));

   UpdateObjectReferenceInSlots();
   UpdateActivePage();
}

void TGo4FitPanel::Fitter_Delete()
{
  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete fitter"),
         QString("Are you sure?"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  RemovePrimitives();
  RemoveDrawObjects();
  CloseDrawPanel();

  ClearObjectReferenceInSlots();

  SetFitter(0);

  if (WorkingWithPanel()) {
    fxActivePanel = 0;
    fxActivePad = 0;
  }

  UpdateActivePage();
}

void TGo4FitPanel::Fitter_UseWorkspace()
{
  RemovePrimitives();
  CloseDrawPanel();
  ClearObjectReferenceInSlots();

  fxActivePanel = 0;
  fxActivePad = 0;

  UpdateObjectReferenceInSlots();
  UpdateActivePage();
}


void TGo4FitPanel::Fitter_UpdateReferences()
{
   UpdateObjectReferenceInSlots();
   UpdateActivePage();
}

void TGo4FitPanel::Fitter_SaveToBrowser()
{
   TGo4LockGuard lock;

   TGo4Fitter* clone = CloneFitter();

   if (clone!=0) SaveObjectInMemory("", clone);
}

void TGo4FitPanel::Fitter_PrintParameters()
{
   QTable* tab = ParsTable;
   if (tab==0) return;

   TGo4Log::Info("Printout of fitter parameters table");

   QString str("Name");
   for(int ncol=0;ncol<tab->numCols();ncol++) {
      str+="\t";
      str+=tab->horizontalHeader()->label(ncol);
   }
   TGo4Log::Info(str.latin1());

   for(int nrow=0;nrow<tab->numRows();nrow++) {
      str = tab->verticalHeader()->label(nrow);
      for(int ncol=0;ncol<tab->numCols();ncol++) {
        str+="\t";
        str+=tab->text(nrow, ncol);
      }
      TGo4Log::Info(str.latin1());
   }
}

void TGo4FitPanel::Fitter_RollbackParameters()
{
   TGo4Fitter* fitter = GetFitter();
   fitter->RememberPars();

   UpdateStatusBar(0);
   UpdateActivePage();
}

void TGo4FitPanel::Button_WorkWithPanel()
{
   TGo4LockGuard lock;

   TGo4Fitter* fitter = GetFitter();
   if (fitter!=0) {
      TGo4WorkSpace* space = TGo4WorkSpace::Instance();
      if (space==0) return;
      TGo4ViewPanel* panel = LastActivePanel();
      if ((panel==0) || (panel==fxDrawNewPanel)) return;
      if ( (panel!=ActivePanel()) || (panel->GetActivePad()!=ActivePad())) {
         TGo4Fitter* clone = (TGo4Fitter*) fitter->Clone();
         fxActivePanel = panel;
         fxActivePad = panel->GetActivePad();
         SetFitter(clone);
         UpdateObjectReferenceInSlots();
         UpdateActivePage();
      }
   } else
      Fitter_NewForActivePad(false);
}

void TGo4FitPanel::Button_SimpleFit(int nmodel)
{
  if (!WorkingWithOnlyPad()) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  if (fitter->GetNumData()!=1) { cout << " Incorrect data" << endl; return; }

  if (!UpdateObjectReferenceInSlots()) return;

  TGo4FitData* data = fitter->GetData(0);
  if (data==0) return;

  data->ClearRanges();
  data->SetRange(0, ActivePad()->GetUxmin(), ActivePad()->GetUxmax());

  fitter->SetMemoryUsage(1);

  int NumDim = data->DefineDimensions();

  TGo4FitModel* model = 0;

  switch (nmodel) {
    case 0:
      fitter->AddPolynoms(data->GetName(), "Pol", Smp_PolOrderSpin->value(), NumDim);
      break;

    case 1:
      model  = CreateModel(20, 0, fitter, data);
      break;

    case 2:
      model  = CreateModel(24, 0, fitter, data);
      break;

    case 3:
      model  = CreateModel(25, 0, fitter, data);
      break;
  }

  if (model && ((nmodel==1) || (nmodel==2))) LocateModel(model, data, FALSE);

  fbUseAmplEstim = true;
  fiNumMigradIter = 500;

  Button_PerformFit();
}

void TGo4FitPanel::Button_SimpleClear()
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  fitter->DeleteAllModels();

  RemovePrimitives();
  RemoveDrawObjects();
}

void TGo4FitPanel::Button_PeakFinder()
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return;

   TGo4FitPeakFinder* finder = GetPeakFinder(true);
   if (finder==0) return;

   TGo4FitData* data = 0;
   if (fiPanelMode==FitGui::pm_Wizard) data = Wiz_SelectedData();
                         else data = fitter->GetData(0);
   if (data==0) return;

   RemovePrimitives();
   RemoveDrawObjects();

   if (fbUseCurrentRange)
     Wiz_TakeCurrentRange();

   finder->SetDataName(data->GetName());

   finder->SetClearModels(kTRUE);

   fitter->SetMemoryUsage(fiBuffersUsage);

   UpdateStatusBar("Start peak finder ... ");

   fitter->DoAction(finder);

   fbUseAmplEstim = true;
   fiNumMigradIter = 300;

   UpdateStatusBar(0);

   fiPaintMode = 1;
   UpdateActivePage();
}

void TGo4FitPanel::Button_PerformFit()
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return;

   if (fiPanelMode!=FitGui::pm_Expert) {

       if (fbUseAmplEstim || (fiNumMigradIter>0)) {
          TGo4FitPeakFinder* finder = GetPeakFinder(false);
          Int_t n=0;
          while (n<fitter->GetNumActions()) {
             TGo4FitterAction* action = fitter->GetAction(n);
             if (action==finder) n++;
                            else fitter->DeleteAction(action);
          }

          if (fbUseAmplEstim) fitter->AddAmplEstimation();
          if (fiNumMigradIter > 0) {
            TGo4FitMinuit* minuit = new TGo4FitMinuit("Minuit");
            QString cmd("MIGRAD ");
            cmd+=QString::number(fiNumMigradIter);
            cmd+=" 1";
            minuit->AddCommand(cmd.latin1());
            fitter->AddAction(minuit);
          }
       }

       if (fitter->GetNumActions() == 0)
         fitter->AddStandardActions();
   }

   if (fbUseCurrentRange)
      Wiz_TakeCurrentRange();

   fitter->SetMemoryUsage(fiBuffersUsage);

   fitter->MemorizePars();

   UpdateStatusBar("Start fitting ... ");

   fitter->DoActions();

   Button_FitterDraw(0);

   if (fiPanelMode==FitGui::pm_Simple) {
      RemovePrimitives();
      if (!fbShowPrimitives) return;
      TGo4FitData* data = fitter->GetData(0);
      PaintModelsFor(fitter, data, 0, TRUE);
      UpdateStatusBar(0);
   } else
      UpdateActivePage();
}

void TGo4FitPanel::Button_FitterDraw(TGo4FitData* selecteddata)
{
   TGo4Fitter* fitter = GetFitter();
   if ((fitter==0) ||
       (!fbDrawModels && !fbDrawComponents && !fbDrawBackground)) {
           RemoveDrawObjects();
           return;
        }

   bool UseSamePads = fbUseSamePanelForDraw;
   if (UseSamePads)
     for(Int_t n=0;n<fitter->GetNumData();n++) {
       TGo4FitData* data = fitter->GetData(n);
       if (selecteddata && (data!=selecteddata)) continue;

       if (data->IsAnyDataTransform())
          UseSamePads = FALSE;

       if (FindPadWhereData(data)==0)
          UseSamePads = FALSE;
     }

   int NeedPads = 1;
   if(!UseSamePads) {
     if (fxDrawNewPanel==0) {
        fbDrawPanelCreation = kTRUE;
        fxDrawNewPanel = CreateViewPanel();
        fbDrawPanelCreation = kFALSE;
        if (fxDrawNewPanel==0) return;
     } else fxDrawNewPanel->ClearCanvas();

     if (selecteddata==0) NeedPads = fitter->GetNumData();
                     else NeedPads = 1;

     if (NeedPads>1) {
        int DivX = (int)TMath::Sqrt(NeedPads*1.);
        int DivY = DivX;
        while (DivX*DivY<NeedPads) DivY++;
        fxDrawNewPanel->Divide(DivX, DivY);
     }
   }

   for(Int_t n=0;n<fitter->GetNumData();n++) {
       TGo4FitData* data = fitter->GetData(n);

       if (selecteddata && (data!=selecteddata)) continue;

       const char* drawopt = 0;
       if (data->GetDataType()==TGo4FitData::dtGraph)
         drawopt = "L";

       TGo4ViewPanel* panel = 0;
       TPad* pad = 0;

       if (UseSamePads) {
          panel = ActivePanel();
          pad = FindPadWhereData(data);
       } else {
          panel = fxDrawNewPanel;
          pad = fxDrawNewPanel->GetSubPad(0, n, true);
       }
       if ((panel==0) || (pad==0)) continue;

       panel->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitModels);

       // disable autoscaling while THStack is not able to automatically
       // scale only visible region
       if (UseSamePads) panel->SetAutoScale(false, pad);

       // panel->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitInfo);
       // panel->SetPadSuperImpose(pad, true);

       panel->ShootRepaintTimer();

       if (!UseSamePads) {
           TObject* obj = fitter->CreateDrawObject("Data", data->GetName(), kFALSE);
           panel->AddDrawObject(pad, TGo4ViewPanel::kind_FitModels, "::Data", obj, true, drawopt);
       }

       if (!fbDrawInfoOnPad)
          panel->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitInfo);
       else {
          Double_t x2 = 0.98;
          Double_t y2 = 0.5;
          Double_t x1 = x2 - gStyle->GetStatW();
          Double_t y1 = y2 - gStyle->GetStatH();
          TPaveStats* stats = dynamic_cast<TPaveStats*> (pad->GetPrimitive("stats"));
          if (stats) {
            x1 = stats->GetX1NDC();
            x2 = stats->GetX2NDC();
            y2 = stats->GetY1NDC() - 0.05;
            y1 = y2 - (stats->GetY2NDC() - stats->GetY1NDC());
            if (y1<0.02) y1 = 0.02;
          }

          TPaveStats* info = dynamic_cast<TPaveStats*>
              (panel->GetDrawObject(pad, "::FitterPars"));

          bool infocreated = false;

          if (info!=0) info->Clear("*"); else {
             infocreated = true;
             info = new TPaveStats(x1,y1,x2,y2,"brNDC");
             info->SetFillColor(gStyle->GetStatColor());
             info->SetFillStyle(gStyle->GetStatStyle());
             info->SetBorderSize(gStyle->GetStatBorderSize());
             info->SetTextFont(gStyle->GetStatFont());
             if (gStyle->GetStatFont()%10 > 2)
               info->SetTextSize(gStyle->GetStatFontSize());
             info->SetFitFormat(gStyle->GetFitFormat());
             info->SetStatFormat(gStyle->GetStatFormat());
             info->SetTextColor(gStyle->GetStatTextColor());
             info->SetTextAlign(12);
             info->SetName("fitinfo");
          }

          info->AddText(data->GetName());
          info->AddLine(0,0,0,0);

          char t[500], tt[500];

          if (LineParsChk->isChecked())
            info->AddText("Line | Ampl | Pos & Width");

          for(Int_t m=0;m<fitter->GetNumModel();m++) {
            TGo4FitModel* model = fitter->GetModel(m);
            if (!model->IsAssignTo(data->GetName())) continue;

            if (LineParsChk->isChecked()) {
               Double_t ampl, pos, width;

               ampl = model->GetAmplValue();
               if (!model->GetPosition(0,pos)) pos = 0.;
               if (!model->GetWidth(0,width)) width = 0.;

               snprintf(tt,500,"%7s | %s%s | %s%s  %s%s",
                        model->GetName(),
                        "%",gStyle->GetStatFormat(),
                        "%",gStyle->GetStatFormat(),
                        "%",gStyle->GetStatFormat());

               snprintf(t,500,tt,ampl,pos,width);
               info->AddText(t);
            } else {
               int maxparlen = 7;
               for(Int_t np=0;np<model->NumPars();np++) {
                  int len = strlen(model->GetParFullName(np));
                  if (len>maxparlen) maxparlen = len;
               }
               snprintf(tt,500,"%s%ds = %s%s","%",maxparlen,"%",gStyle->GetStatFormat());

               for(Int_t np=0;np<model->NumPars();np++) {
                 snprintf(t,500,tt,model->GetParFullName(np), model->GetPar(np)->GetValue());
                 info->AddText(t);
               }
            }
          }

          if (infocreated)
            panel->AddDrawObject(pad, TGo4ViewPanel::kind_FitInfo, "::FitterPars", info, true, 0);
        }

       if (fbDrawModels) {
           TObject* model = fitter->CreateDrawObject("Model", data->GetName(), kTRUE);
           panel->AddDrawObject(pad, TGo4ViewPanel::kind_FitModels, "::DataModel", model, true, drawopt);
       }

       if (fbDrawBackground) {
           TObject* model = fitter->CreateDrawObject("Background", data->GetName(), kTRUE, "Background");
           panel->AddDrawObject(pad, TGo4ViewPanel::kind_FitModels, "::Background", model, true, drawopt);
       }

       if (fbDrawComponents)
         for(Int_t m=0;m<fitter->GetNumModel();m++) {
            TGo4FitModel* model = fitter->GetModel(m);
            if (!model->IsAssignTo(data->GetName())) continue;
            if (model->GetGroupIndex()==0) continue;
            TObject* comp = fitter->CreateDrawObject(model->GetName(), data->GetName(), kTRUE, model->GetName());
            if (comp==0) continue;

            panel->AddDrawObject(pad, TGo4ViewPanel::kind_FitModels, "::FitComponent", comp, true, drawopt);
         }
   } // data
}


void TGo4FitPanel::Cmd_CreateFitter()
{
  TGo4Fitter* fitter = new TGo4Fitter("Fitter","Fitter object");
  SetFitter(fitter);

  UpdateActivePage();
}

void TGo4FitPanel::Cmd_CreateAppropriateFitter()
{
   TGo4Fitter* fitter = CreateFitterFor(ActivePanel(), ActivePad(), "Fitter");
   if (fitter) {
      SetFitter(fitter);
      UpdateActivePage();
   }
}

void TGo4FitPanel::Cmd_DeleteFitter()
{
  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Clear fitter"),
         QString("Are you sure?"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  RemovePrimitives();

  SetFitter(0);

  UpdateActivePage();
}

void TGo4FitPanel::Cmd_ClearFitter()
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Clear fitter"),
         QString("Are you sure?"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   fitter->Clear();

   UpdateActivePage();
}

void TGo4FitPanel::Cmd_SaveFitter(bool ask)
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  QString fname(fitter->GetName());
  fname+=".root";

  if(ask) {
    fname = QFileDialog::getSaveFileName(fname,
                                         QString("Root file (*.root)"),
                                         0,
                                         QString("save fitter dialog"),
                                         QString("Set file name to save fitter"));
    if (fname==QString::null) return;
  }

  fitter->SetSaveFlagForObjects(fbSaveWithReferences);

  TFile f(fname,"recreate");
  fitter->Write();
}

void TGo4FitPanel::Cmd_ItemPrint(QFitItem* item)
{
   TObject* obj = item->Object();
   if (obj==0) return;

   QString str("*");
   bool ok = TRUE;
   if (obj->InheritsFrom(TGo4Fitter::Class()))
      str = QInputDialog::getText(QString("Go4 Fit Panel"),
                                  QString("Enter parameters for Print() command"),
                                  QLineEdit::Normal, str, &ok, 0);
   if (!ok) return;

   RemoveItemWidget();
   QFitPrintWidget* widget = new QFitPrintWidget(0, QString("Print ")+obj->GetName());

   ListStack->addWidget(widget, FitGui::ListStackId);

   widget->SetDrawOption(str);
   widget->SetItem(this, item);
   ListStack->raiseWidget(widget);
}

void TGo4FitPanel::Cmd_DrawData(QFitItem * item)
{
  TGo4FitData* data = dynamic_cast<TGo4FitData*> (item->Object());
  if (data) Button_FitterDraw(data);
}

void TGo4FitPanel::Cmd_DeleteData(QFitItem * item)
{
  TGo4FitData* data = dynamic_cast<TGo4FitData*> (item->Object());
  TGo4Fitter* fitter = GetFitter();

  if ((data==0) || (fitter==0)) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete data"),
         QString("Are you sure to delete ") + data->GetName(),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  fitter->RemoveData(data->GetName(),kTRUE);

  UpdateObjectReferenceInSlots();

  UpdateItemsOfType(FitGui::ot_allslots, 0);

  UpdateItem(item->Parent(), TRUE);

  UpdateStatusBar(0);
}

void TGo4FitPanel::Cmd_DeleteAllData(QFitItem * item)
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete all data"),
         QString("Are you sure to delete all data objects"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  fitter->DeleteAllData();

  UpdateObjectReferenceInSlots();

  UpdateItemsOfType(FitGui::ot_allslots, 0);

  UpdateItem(item, TRUE);

  UpdateStatusBar(0);
}

void TGo4FitPanel::Cmd_DeleteAssosiatedModels(QFitItem * item)
{
  if ((item==0) || (item->Parent()==0)) return;
  TGo4FitData* data = dynamic_cast<TGo4FitData*> (item->Object());
  TGo4Fitter* fitter = GetFitter();

  if ((data==0) || (fitter==0)) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete models"),
         QString("Are you sure to delete models, assosiated to ") + data->GetName(),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  fitter->DeleteModelsAssosiatedTo(data->GetName());

  QFitItem* mlist = FindItem(0, FitGui::ot_modellist, 0);
  if (mlist) UpdateItem(mlist, TRUE);

  UpdateItemsOfType(FitGui::ot_allslots, 0);

  ShowItem(item, false);

  UpdateStatusBar(0);
}

void TGo4FitPanel::Cmd_DeleteModel(QFitItem * item)
{
   TGo4FitModel* model =  dynamic_cast<TGo4FitModel*> (item->Object());

   Cmd_RemoveModel(model);
}

void TGo4FitPanel::Cmd_RemoveModel(TGo4FitModel* model)
{
  TGo4Fitter* fitter = GetFitter();
  if ((model==0) || (fitter==0)) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
       QString("Delete model"),
       QString("Are you sure to delete ") + model->GetName(),
       QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  fitter->RemoveModel(model->GetName(), kTRUE);

  UpdateObjectReferenceInSlots();

  UpdateActivePage();
}

void TGo4FitPanel::Cmd_DeleteModels(QFitItem * item)
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete all models"),
         QString("Are you sure to delete all models objects"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  fitter->DeleteAllModels();

  UpdateObjectReferenceInSlots();

  UpdateItem(item, TRUE);
  UpdateItemsOfType(FitGui::ot_allslots, 0);
  UpdateStatusBar(0);
}

void TGo4FitPanel::Cmd_CloneModel(QFitItem * item)
{
   TGo4FitModel* model =  dynamic_cast<TGo4FitModel*> (item->Object());

   TGo4Fitter* fitter = GetFitter();
   if ((fitter==0) || (model==0)) return;

   fitter->CloneModel(model->GetName());

   UpdateObjectReferenceInSlots();

   UpdateActivePage();
}

void TGo4FitPanel::Cmd_AddNewData(QFitItem * item, int id)
{
  if (item->ObjectType()!=FitGui::ot_datalist) item = item->Parent();
  if (item->ObjectType()!=FitGui::ot_datalist) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  TGo4FitData* data = CreateData(id, 0);
  if (data==0) return;

  fitter->AddData(data);

  UpdateObjectReferenceInSlots();

  UpdateItem(item, TRUE);

  UpdateItemsOfType(FitGui::ot_allslots, 0);

  QFitItem* curr = FindItem(data, FitGui::ot_data, item);
  if (curr!=0)
    FitList->setSelected(curr, TRUE);
  UpdateStatusBar(0);
}

void  TGo4FitPanel::Cmd_AddNewModel(QFitItem * item, int id)
{
  if (item->ObjectType()!=FitGui::ot_modellist) item = item->Parent();
  if (item->ObjectType()!=FitGui::ot_modellist) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  TGo4FitModel* model = CreateModel(id, 0, fitter, 0);
  LocateModel(model, 0, true);

  UpdateObjectReferenceInSlots();

  UpdateItem(item, TRUE);

  UpdateItemsOfType(FitGui::ot_allslots, 0);

  QFitItem* curr = FindItem(model, FitGui::ot_model, item);
  if (curr!=0)
    FitList->setSelected(curr, TRUE);
  UpdateStatusBar(0);
}

void TGo4FitPanel::Cmd_ClearAssigment(QFitItem* item)
{
  if (item==0) return;
  TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Parent()->Object());
  if (model==0) return;

  model->ClearAssignmentTo(item->text(0).latin1());

  UpdateItemsOfType(FitGui::ot_parslist, item->Parent()->Parent());

  UpdateItem(item->Parent(), TRUE);
}

void TGo4FitPanel::Cmd_ClearAssigments(QFitItem* item)
{
   TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
   if (model==0) return;

   model->ClearAssignments();

   UpdateItemsOfType(FitGui::ot_parslist, item->Parent());

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_AssignModelToAllData(QFitItem* item)
{
   TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
   TGo4Fitter* fitter = GetFitter();
   if ((model==0) || (fitter==0)) return;

   fitter->AssignModelTo(model->GetName(), 0);

   UpdateItemsOfType(FitGui::ot_parslist, item->Parent());

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_AssignModelToData(QFitItem* item, int id)
{
   TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
   TGo4Fitter* fitter = GetFitter();
   if ((model==0) || (fitter==0)) return;

   fitter->AssignModelTo(model->GetName(), fitter->GetDataName(id));

   UpdateItemsOfType(FitGui::ot_parslist, item->Parent());

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_RemoveRangeCondition(QFitItem* item)
{
   if (item==0) return;

   Int_t typ = item->ObjectType();

   if ((typ!=FitGui::ot_range) && (typ!=FitGui::ot_rangecut)) return;

   int num = item->Tag();

   if (num<0) return;

   if(fbNeedConfirmation)
       if ( QMessageBox::information(0,
            QString("Remove condition"),
            QString("Are you sure to delete condition ") + item->text(0),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   QFitItem* parent = item->Parent();
   TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (parent->Object());

   if (typ==FitGui::ot_range) comp->RemoveRangeCondition(num);
                         else comp->RemoveRangeCut(num);

   UpdateItem(parent, TRUE);
}

void TGo4FitPanel::Cmd_RemoveRangeConditions(QFitItem* item)
{
   if ((item==0) || (item->ObjectType()!=FitGui::ot_rangelist)) return;

   if(fbNeedConfirmation)
       if ( QMessageBox::information(0,
            QString("Delete conditions"),
            QString("Are you sure to delete all conditions"),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (item->Object());
   comp->ClearRanges();

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_AddRangeCondition(QFitItem* item, int id)
{
   if ((item==0) || (item->ObjectType()!=FitGui::ot_rangelist)) return;

   TGo4FitData* data = 0;

   if (item->Object()->InheritsFrom(TGo4FitData::Class()))
     data = dynamic_cast<TGo4FitData*> (item->Object());
   else {
     TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
     TGo4Fitter* fitter = GetFitter();
     if ((model!=0) && (fitter!=0))
        for (Int_t n=0;n<model->NumAssigments();n++) {
          data = fitter->FindData(model->AssignmentName(n));
          if (data!=0) break;
        }

   }
   Double_t minx = 0., maxx = 100., miny = 0., maxy = 100;

   TPad* pad = FindPadWhereData(data);
   if (pad) {
      minx = pad->GetUxmin(); maxx = pad->GetUxmax();
      miny = pad->GetUymin(); maxy = pad->GetUymax();
   } else
   if (data!=0) {
     data->DefineScaleMinMax(0,minx,maxx);
     data->DefineScaleMinMax(1,miny,maxy);
     Double_t ddx = 0.1*(maxx-minx);
     minx += ddx;
     maxx -= ddx;
     Double_t ddy = 0.1*(maxy-miny);
     miny += ddy;
     maxy -= ddy;
   }

   TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (item->Object());

   if (id==0) comp->SetRange(0, minx, maxx); else
   if (id==1) comp->ExcludeRange(0, minx, maxx); else
   if (id==2) comp->SetRangeMin(0, minx);  else
   if (id==3) comp->SetRangeMax(0, maxx); else
   if (id==4) {
      TString name("Cut");
      name+=comp->GetNumRangeCut();
      TCutG* cut = new TCutG(name, 5);
      cut->SetTitle("range cut of X and Y axis");
      cut->SetPoint(0, minx, miny);
      cut->SetPoint(1, maxx, miny);
      cut->SetPoint(2, maxx, maxy);
      cut->SetPoint(3, minx, maxy);
      cut->SetPoint(4, minx, miny);
      comp->AddRangeCut(cut);
   }

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_DeleteAction(QFitItem* item)
{
   if (item==0) return;
   TGo4FitterAction* action = dynamic_cast<TGo4FitterAction*> (item->Object());
   TGo4Fitter* fitter = GetFitter();

   if(action && fitter) {
     if(fbNeedConfirmation)
       if ( QMessageBox::information(0,
            QString("Delete action"),
            QString("Are you sure to delete ") + item->text(0),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

      fitter->DeleteAction(action);
      UpdateItem(item->Parent(), TRUE);
   }
}

void TGo4FitPanel::Cmd_MoveAction(QFitItem* item, int dir)
{
   if ((item==0) || (item->Parent()==0)) return;

   TGo4FitterAction* action = dynamic_cast<TGo4FitterAction*> (item->Object());
   TGo4Fitter* fitter = GetFitter();

   if(action && fitter) {
      fitter->ReplaceAction(action, dir);
      QFitItem* parent = item->Parent();
      UpdateItem(parent, TRUE);
      item = FindItem(action, -1, 0);
      if (item!=0)
        FitList->setSelected(item, TRUE);
   }
}

void TGo4FitPanel::Cmd_ExecuteAction(QFitItem* item)
{
   TGo4Fitter* fitter = GetFitter();
   if ((item==0) || (fitter==0)) return;

   TGo4FitterAction* action = dynamic_cast<TGo4FitterAction*> (item->Object());
   if (action==0) return;

   fitter->DoAction(action);

   if (action->CanChangeFitter()) UpdateActivePage(); else {
     UpdateItem(item, TRUE);
     UpdateItemsOfType(FitGui::ot_par, 0);
   }
}

void TGo4FitPanel::Cmd_ExecuteActions(QFitItem* item, bool expert)
{
   TGo4Fitter* fitter = GetFitter();

   if ((item==0) || (fitter==0)) return;

   fitter->MemorizePars();

   fitter->DoActions(expert);

   if (expert) UpdateActivePage(); else {
     UpdateItem(item, TRUE);
     UpdateItemsOfType(FitGui::ot_par, 0);
   }
}

void TGo4FitPanel::Cmd_DeleteOutputActions(QFitItem* item)
{
   TGo4Fitter* fitter = GetFitter();
   if ((item==0) || (item->ObjectType()!=FitGui::ot_actlist) || (fitter==0)) return;

   if(fbNeedConfirmation)
     if ( QMessageBox::information(0,
         QString("Delete output actions"),
         QString("Are you sure to delete all output actions") + item->text(0),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   fitter->DeleteOutputActions();
   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_DeleteActions(QFitItem* item)
{
   TGo4Fitter* fitter = GetFitter();
   if ((item==0) || (item->ObjectType()!=FitGui::ot_actlist) || (fitter==0)) return;

   if(fbNeedConfirmation)
     if ( QMessageBox::information(0,
         QString("Delete all actions"),
         QString("Are you sure to delete all actions"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   fitter->DeleteActions();
   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_AddNewAction(QFitItem* item, int id)
{
   TGo4Fitter* fitter = GetFitter();
   if ((item==0) || (item->ObjectType()!=FitGui::ot_actlist) || (fitter==0)) return;

   switch(id) {
     case 0: fitter->AddAction(new TGo4FitterConfig("Config","Fitter configuration")); break;
     case 1: fitter->AddAmplEstimation(); break;
     case 2: fitter->AddSimpleMinuit(); break;
     case 3: fitter->AddOutputAction("Print","*"); break;
     case 4: fitter->AddAction(new TGo4FitPeakFinder("Finder")); break;
   }
   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_DeleteDependency(QFitItem* item)
{
   if ((item==0) || (item->ObjectType()!=FitGui::ot_depend)) return;

   TGo4FitDependency* depen = dynamic_cast<TGo4FitDependency*> (item->Object());
   TObjArray* lst = dynamic_cast<TObjArray*> (item->Parent()->Object());

   if((depen==0) || (lst==0)) return;

   if(fbNeedConfirmation)
     if ( QMessageBox::information(0,
          QString("Delete dependency item"),
          QString("Are you sure to delete ") + item->text(0),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   lst->Remove(depen);
   lst->Compress();
   delete depen;

   UpdateItem(item->Parent(), TRUE);
}

void TGo4FitPanel::Cmd_DeleteDependencies(QFitItem* item)
{
   if (item==0) return;
   TObjArray* lst = dynamic_cast<TObjArray*> (item->Object());
   if(lst==0) return;

   if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete all"),
         QString("Are you sure to delete all items from ") + item->text(0),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   lst->Clear();

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_AddDependency(QFitItem* item)
{
   if (item==0) return;

   TObjArray* lst = dynamic_cast<TObjArray*> (item->Object());
   if(lst==0) return;

   if(item->ObjectType()==FitGui::ot_reslist)
      lst->Add(new TGo4FitDependency(0,0.0));
   else
      lst->Add(new TGo4FitDependency("Par1",1.0));

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_MemorizePars(QFitItem* item)
{
  if (item==0) return;

  TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
  if (pars==0) return;

  pars->MemorizePars();
}

void TGo4FitPanel::Cmd_RememberPars(QFitItem* item)
{
  if (item==0) return;

  TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
  if (pars==0) return;

  pars->RememberPars();

  UpdateItemsOfType(FitGui::ot_par, item);

  ShowItem(item, false);
}

void TGo4FitPanel::Cmd_DeletePars(QFitItem* item)
{
   if (item==0) return;
   TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
   if (pars==0) return;

   if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete all"),
         QString("Are you sure to delete all items"),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   pars->ClearPars();

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_MemorizePar(QFitItem* item)
{
   if (item==0) return;
   TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (item->Object());
   if (par==0) return;

   par->MemorizeValue();
}

void TGo4FitPanel::Cmd_RememberPar(QFitItem* item)
{
   if (item==0) return;
   TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (item->Object());
   if (par==0) return;

   par->RememberValue();

   SetItemText(item, TRUE);

   ShowItem(item, false);
}

void TGo4FitPanel::Cmd_AddNewPar(QFitItem* item)
{
   if (item==0) return;

   TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
   if (pars==0) return;

   TString name;
   int num = 0;
   do {
      name = "Par";
      name += num;
      num++;
   } while (pars->FindPar(name));
   pars->CreatePar(name.Data(),"additional parameter",0.);

   UpdateItem(item, TRUE);
}

void TGo4FitPanel::Cmd_DeletePar(QFitItem* item)
{
   if (item==0) return;

   TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (item->Object());
   TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Parent()->Object());
   if ((pars==0) || (par==0)) return;

   if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete item"),
         QString("Are you sure to delete ") + par->GetName(),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   pars->RemovePar(par);

   UpdateItem(item->Parent(), TRUE);
}

void TGo4FitPanel::Cmd_DeleteMinuitResult(QFitItem* item)
{
   if (item==0) return;

   TGo4FitMinuitResult* res = dynamic_cast<TGo4FitMinuitResult*> (item->Object());
   TGo4FitMinuit* minuit = dynamic_cast<TGo4FitMinuit*> (item->Parent()->Object());
   if ((res==0) || (minuit==0)) return;

   if(fbNeedConfirmation)
    if ( QMessageBox::information(0,
         QString("Delete result"),
         QString("Are you sure to delete ") + res->GetName(),
         QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   minuit->RemoveResult(res);
   delete res;

   UpdateItem(item->Parent(), TRUE);
}

void TGo4FitPanel::Cmd_UpdateAllSlots(QFitItem* item)
{
  if (item==0) return;

  UpdateObjectReferenceInSlots();

  QListViewItem* child = item->firstChild();
  while (child) {
     UpdateItem(dynamic_cast<QFitItem*>(child), TRUE);
     child = child->nextSibling();
  }
}


// *************************** fitslots  ends ******************************


void TGo4FitPanel::FitList_contextMenuRequested( QListViewItem * item, const QPoint & pnt, int )
{
   QFitItem* fititem = dynamic_cast<QFitItem*> (item);
   if (fititem==0) return;

   QPopupMenu menu(0,"ListPopup");

   if (FillPopupForItem(fititem,&menu)) {
      int id =  menu.exec(pnt);
      ExecPopupForItem(fititem, id /*, menu.text(id) */);
   }
}


void TGo4FitPanel::FitList_currentChanged( QListViewItem * item)
{
   QFitItem* fititem = dynamic_cast<QFitItem*> (item);
   if (fititem==0) return;

   ShowItem(fititem, false);
   fxCurrentItem = fititem;

   UpdateItemMenu();
}

void TGo4FitPanel::AboutToShowViewMenu()
{
  ViewMenu->clear();

  ViewMenu->insertItem("&Simple", this, SLOT(ChangeViewType(int)), 0, FitGui::pm_Simple);
  ViewMenu->insertItem("&Wizard", this, SLOT(ChangeViewType(int)), 0, FitGui::pm_Wizard);
  ViewMenu->insertItem("&Expert", this, SLOT(ChangeViewType(int)), 0, FitGui::pm_Expert);

  ViewMenu->setItemChecked(fiPanelMode, TRUE);
}


void TGo4FitPanel::ChangeViewType(int id)
{
  if (id!=FitGui::pm_None)
    fiPanelMode = id;

  fbParsWidgetShown = false;
  UpdateActivePage();
}

void TGo4FitPanel::ShowPanelPage( int id )
{
   QWidget* w = PageEmpty;
   switch(id) {
      case 1: w = PageSimple; break;
      case 2: w = PageWizard; break;
      case 3: w = PageExtended; break;
      case 100: w = PageParameters; break;
   }
   if (w!=WidgetStack->visibleWidget())
     WidgetStack->raiseWidget(w);
}

void TGo4FitPanel::UpdateStatusBar(const char* info)
{
   QString status;
   TGo4Fitter* fitter = GetFitter();

   if (info!=0) status=info; else {
       if (WorkingWithPanel()) {
          status = QString("Active: ") + ActivePad()->GetName();
          if (!WorkingWithOnlyPad())
            status += ". Muiltifit";

       } else status = "Workspace";
       if (fitter==0) status+=".  No fitter";
                 else status = status + ". Fitter: "+fitter->GetName();
   }
   MainStatusLbl->setText(status);
   MainStatusLbl->update();
   MainStatusLbl->repaint();

   MainAttachBtn->setEnabled(TRUE);

   QToolTip::remove(MainParsBtn);
   if (fbParsWidgetShown) {
     MainParsBtn->setText("Back");
     QToolTip::add(MainParsBtn, "Back to last selected page");
   } else {
     MainParsBtn->setText("Pars");
     QToolTip::add(MainParsBtn, "Show list of fitter parameters");
   }

   if (fitter==0) {
     MainFitBtn->setEnabled(FALSE);
     MainDrawBtn->setEnabled(FALSE);
     MainFindBtn->setEnabled(FALSE);
     MainParsBtn->setEnabled(FALSE);
   } else {
     MainFitBtn->setEnabled((fitter->GetNumData()>0) &&
           ((fitter->GetNumModel()>0) || (fiPanelMode==FitGui::pm_Simple)));
     MainDrawBtn->setEnabled(fitter->GetNumData()>0);
     MainFindBtn->setEnabled((fitter->GetNumData()>0) && (fiPanelMode==FitGui::pm_Wizard)
                             && GetPeakFinder(false));
     MainParsBtn->setEnabled(TRUE);
   }
}

void TGo4FitPanel::UpdateItemMenu()
{
  bool showitem = fiPanelMode==FitGui::pm_Expert;

  QString itemtext;

  if (showitem) {
    QFitItem* item = dynamic_cast<QFitItem*> (FitList->currentItem());
    if ((item==0) || (item->ObjectType()==FitGui::ot_fitter)) showitem = FALSE; else
       itemtext = item->text(0);
  }

  if (showitem) {
    if (ItemMenu==0) {
      ItemMenu = new QPopupMenu(this);
      MenuBar->insertItem( itemtext, ItemMenu, 111);
      connect(ItemMenu, SIGNAL(aboutToShow()), this, SLOT(AboutToShowItemMenu()) );
    } else MenuBar->changeItem(111, itemtext);
  } else
    if (ItemMenu!=0) {
      MenuBar->removeItem(111);
      delete ItemMenu;
      ItemMenu = 0;
    }
}

void TGo4FitPanel::AboutToShowSettMenu()
{
  SettMenu->setItemChecked(1, fbNeedConfirmation);
  SettMenu->setItemChecked(2, fbShowPrimitives);
  SettMenu->setItemChecked(3, fbFreezeMode);
  SettMenu->setItemChecked(4, fbSaveWithReferences);
  SettMenu->setItemVisible(4, (fiPanelMode==FitGui::pm_Expert) || (fiPanelMode==FitGui::pm_Wizard));

  SettMenu->setItemChecked(10,fbUseCurrentRange);
  SettMenu->setItemChecked(11,fbDrawModels);
  SettMenu->setItemChecked(12,fbDrawComponents);
  SettMenu->setItemChecked(13,fbUseSamePanelForDraw);
  SettMenu->setItemChecked(14,fbDrawBackground);
  SettMenu->setItemChecked(15,fbDrawInfoOnPad);
  SettMenu->setItemChecked(16,fbRecalculateGaussWidth);

  SettMenu->setItemChecked(17,fiIntegralMode==0);
  SettMenu->setItemChecked(18,fiIntegralMode==1);
  SettMenu->setItemChecked(19,fiIntegralMode==2);
  SettMenu->setItemChecked(20,fiIntegralMode==3);

  SettMenu->setItemChecked(21,fiBuffersUsage==0);
  SettMenu->setItemChecked(22,fiBuffersUsage==1);
  SettMenu->setItemChecked(23,fiBuffersUsage==2);
  SettMenu->setItemChecked(24,fiBuffersUsage==3);
}

void TGo4FitPanel::ChangeSettings(int id)
{
   switch(id) {
     case 1:  fbNeedConfirmation = !fbNeedConfirmation; break;
     case 2:  fbShowPrimitives = !fbShowPrimitives; break;
     case 3:  fbFreezeMode = !fbFreezeMode; break;
     case 4:  fbSaveWithReferences = !fbSaveWithReferences; break;
     case 10: fbUseCurrentRange = !fbUseCurrentRange; break;
     case 11: fbDrawModels = !fbDrawModels; break;
     case 12: fbDrawComponents = !fbDrawComponents; break;
     case 13: fbUseSamePanelForDraw = !fbUseSamePanelForDraw; break;
     case 14: fbDrawBackground = !fbDrawBackground; break;
     case 15: fbDrawInfoOnPad = !fbDrawInfoOnPad; break;
     case 16: fbRecalculateGaussWidth = !fbRecalculateGaussWidth; UpdateActivePage(); break;
     case 17: fiIntegralMode = 0; UpdateActivePage(); break;
     case 18: fiIntegralMode = 1; UpdateActivePage(); break;
     case 19: fiIntegralMode = 2; UpdateActivePage(); break;
     case 20: fiIntegralMode = 3; UpdateActivePage(); break;
     case 21: fiBuffersUsage = 0; break;
     case 22: fiBuffersUsage = 1; break;
     case 23: fiBuffersUsage = 2; break;
     case 24: fiBuffersUsage = 3; break;
   }

   go4sett->setBool("/FitPanel/NeedConfirmation",fbNeedConfirmation);
   go4sett->setBool("/FitPanel/ShowPrimitives",fbShowPrimitives);
   go4sett->setBool("/FitPanel/SaveWithReferences",fbSaveWithReferences);
   go4sett->setBool("/FitPanel/UseCurrentRange",fbUseCurrentRange);
   go4sett->setBool("/FitPanel/DrawModels",fbDrawModels);
   go4sett->setBool("/FitPanel/DrawComponents",fbDrawComponents);
   go4sett->setBool("/FitPanel/UseSamePanelForDraw",fbUseSamePanelForDraw);
   go4sett->setBool("/FitPanel/DrawBackground",fbDrawBackground);
   go4sett->setBool("/FitPanel/DrawInfoOnPad",fbDrawInfoOnPad);
   go4sett->setBool("/FitPanel/RecalculateGaussWidth",fbRecalculateGaussWidth);
   go4sett->setInt("/FitPanel/IntegralMode",fiIntegralMode);
   go4sett->setInt("/FitPanel/BuffersUsage",fiBuffersUsage);
}

void TGo4FitPanel::AboutToShowFitterMenu()
{
   FitterMenu->clear();

   TGo4Fitter* fitter = GetFitter();
   TGo4WorkSpace* space = TGo4WorkSpace::Instance();
   TGo4ViewPanel* panel = LastActivePanel();

   bool samepad = (panel==0) ? FALSE : WorkingWithPanel() &&
     (panel==ActivePanel()) && (panel->GetActivePad()==ActivePad());

   QString padname;
   if (panel!=0) padname = QString("panel \"") + panel->caption() + "\"";

   if (fiPanelMode==FitGui::pm_Expert)
     FitterMenu->insertItem("&Create for workspace", this, SLOT(FitterMenuItemSelected(int)), 0, 1);
   FitterMenu->insertItem("Create &for "+padname, this, SLOT(FitterMenuItemSelected(int)), 0, 2);
   FitterMenu->insertItem("&Delete", this, SLOT(FitterMenuItemSelected(int)), 0, 3);
   FitterMenu->insertSeparator();

   FitterMenu->insertItem("Save to &browser", this, SLOT(FitterMenuItemSelected(int)), 0, 21);
   if (fiPanelMode==FitGui::pm_Expert)
      FitterMenu->insertItem("&Workspace", this, SLOT(FitterMenuItemSelected(int)), 0, 23);
   FitterMenu->insertItem("&Update references", this, SLOT(FitterMenuItemSelected(int)), 0, 24);
   FitterMenu->insertItem("&Print parameters", this, SLOT(FitterMenuItemSelected(int)), 0, 25);
   FitterMenu->insertItem("&Rollback parameters", this, SLOT(FitterMenuItemSelected(int)), 0, 26);

   FitterMenu->insertSeparator();

   FitterMenu->insertItem("&Close", this, SLOT(FitterMenuItemSelected(int)), 0, 99);

   FitterMenu->setItemEnabled(1, TRUE);
   FitterMenu->setItemEnabled(2, (panel!=0));
   FitterMenu->setItemEnabled(3, (fitter!=0));

   FitterMenu->setItemEnabled(11, (fitter!=0) && (panel!=0) && !samepad);
   FitterMenu->setItemEnabled(12, (fitter!=0) && (panel!=0) && !samepad);
   FitterMenu->setItemEnabled(13, (fitter!=0) && WorkingWithPanel());
   FitterMenu->setItemEnabled(14, (fitter!=0) && WorkingWithPanel());

   FitterMenu->setItemEnabled(21, (fitter!=0));
   FitterMenu->setItemEnabled(22, (fitter!=0));
   FitterMenu->setItemEnabled(23, WorkingWithPanel());
   FitterMenu->setItemEnabled(24, (fitter!=0));
   FitterMenu->setItemEnabled(25, (fitter!=0) && fbParsWidgetShown);
   FitterMenu->setItemEnabled(26, (fitter!=0) && fitter->CanRollbackPars());

   FitterMenu->setItemEnabled(99, TRUE);
}

void TGo4FitPanel::FitterMenuItemSelected(int id)
{
   switch (id) {
      case 1: Fitter_New(); break;
      case 2: Fitter_NewForActivePad(true); break;
      case 3: Fitter_Delete(); break;
      case 21: Fitter_SaveToBrowser(); break;
      case 23: Fitter_UseWorkspace(); break;
      case 24: Fitter_UpdateReferences(); break;
      case 25: Fitter_PrintParameters(); break;
      case 26: Fitter_RollbackParameters(); break;
      case 99: close(); break;
   }
}

void TGo4FitPanel::AboutToShowItemMenu()
{
  if (ItemMenu==0) return;
  QFitItem* item = dynamic_cast<QFitItem*> (FitList->currentItem());
  if (item==0) return;
  ItemMenu->clear();
  if (!FillPopupForItem(item,ItemMenu)) return;
  for(uint n=0;n<ItemMenu->count();n++) {
     int id = ItemMenu->idAt(n);
     ItemMenu->connectItem(id, this, SLOT(ItemMenuItemSelected(int)));
  }
}

void TGo4FitPanel::ItemMenuItemSelected(int id)
{
   QFitItem* item = dynamic_cast<QFitItem*> (FitList->currentItem());
   if (item==0) return;
   ExecPopupForItem(item, id /*, ItemMenu->text(id) */);
}

void TGo4FitPanel::focusInEvent( QFocusEvent* event)
{
  UpdateStatusBar(0);
  QWidget::focusInEvent(event);
}

void TGo4FitPanel::windowActivationChange( bool OldActive)
{
   UpdateStatusBar(0);
   QWidget::windowActivationChange(OldActive);
}

void TGo4FitPanel::UpdatePFAmplLbl()
{
   TGo4FitPeakFinder* finder = GetPeakFinder(true);
   if (finder!=0)
     PF_AmplLbl->setText(QString("Threshold: ")+QString::number(finder->Get0MaxAmplFactor()*100)+"%");
   else
     PF_AmplLbl->setText("Threshold:");
   PF_AmplLbl->adjustSize();
}

void TGo4FitPanel::UpdatePFRelNoiseLbl()
{
   TGo4FitPeakFinder* finder = GetPeakFinder(true);

   if (finder!=0)
     PF_RelNoiseLbl->setText(QString("Noise factor: ")+QString::number(finder->Get2NoiseFactor()));
   else
     PF_RelNoiseLbl->setText("Noise factor: ");
   PF_RelNoiseLbl->adjustSize();
}

void TGo4FitPanel::PF_MinWidthEdt_textChanged( const QString & str )
{
  if (fbFillingWidget) return;
  bool ok;
  double zn = str.toDouble(&ok);
  if (ok) {
    TGo4FitPeakFinder* finder = GetPeakFinder(true);
    if (finder!=0) finder->Set0MinWidth(zn);
  }
}


void TGo4FitPanel::PF_MaxWidthEdt_textChanged( const QString & str )
{
  if (fbFillingWidget) return;
  bool ok;
  double zn = str.toDouble(&ok);
  if (ok) {
    TGo4FitPeakFinder* finder = GetPeakFinder(true);
    if (finder) finder->Set0MaxWidth(zn);
  }
}


void TGo4FitPanel::PF_WidthEdit_textChanged( const QString & str)
{
  if (fbFillingWidget) return;
  bool ok;
  double zn = str.toDouble(&ok);
  if (ok) {
    TGo4FitPeakFinder* finder = GetPeakFinder(true);
    if (finder) finder->Set1LineWidth(zn);
  }
}

void TGo4FitPanel::PF_AmplSlider_valueChanged( int zn )
{
  if (!fbFillingWidget) {
     TGo4FitPeakFinder* finder = GetPeakFinder(true);
     if (finder) finder->Set0MaxAmplFactor(1.-zn/100.);
     UpdatePFAmplLbl();
  }
}

void TGo4FitPanel::PF_AmplSlider_sliderReleased()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::PF_RelNoiseSlider_valueChanged( int zn )
{
  if (!fbFillingWidget) {
     TGo4FitPeakFinder* finder = GetPeakFinder(true);
     if (finder) finder->Set2NoiseFactor((101-zn)/10.);
     UpdatePFRelNoiseLbl();
  }
}

void TGo4FitPanel::PF_RelNoiseSlider_sliderReleased()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::PF_MinNoiseEdit_textChanged( const QString & str)
{
  if (fbFillingWidget) return;
  bool ok;
  double zn = str.toDouble(&ok);
  if (ok) {
    TGo4FitPeakFinder* finder = GetPeakFinder(true);
    if (finder!=0) finder->Set2NoiseMinimum(zn);
  }
}


void TGo4FitPanel::PF_SumUpSpin_valueChanged( int num)
{
  if (!fbFillingWidget) {
    TGo4FitPeakFinder* finder = GetPeakFinder(true);
    if (finder!=0) {
       finder->Set2ChannelSum(num);
       Button_PeakFinder();
    }
  }
}


void TGo4FitPanel::UpdateWizDataList()
{
   TGo4Fitter* fitter = GetFitter();

   fbFillingWidget = true;

   bool updatestack = FALSE, updatepaint = FALSE;

   Wiz_DataList->clear();
   if (fitter==0) fxWizDataName = "";
   else {
     if ((Wiz_SelectedData()==0) && (fitter->GetNumData()>0)) {
       fxWizDataName = fitter->GetDataName(0);
       if (fiWizPageIndex<1) updatestack = TRUE;
       if (fiPaintMode==0) updatepaint = TRUE;
     }

     int selindx = -1;
     for(Int_t n=0;n<fitter->GetNumData();n++) {
       TGo4FitData* data = fitter->GetData(n);
       Wiz_DataList->insertItem(data->GetName());
       if (strcmp(data->GetName(), fxWizDataName.latin1())==0) selindx = n;
     }
     Wiz_DataList->setSelected(selindx, TRUE);
     if (selindx<0) fxWizDataName = "";
               else Wiz_DataList->setSelected(selindx, TRUE);
   }
   UpdateWizDataBtns();
   fbFillingWidget = false;

   if (updatestack) {
      fiWizPageIndex = 2;
      UpdateWizStackWidget();
   }
   if (updatepaint)
     UpdateWizPaint(1);

}

void TGo4FitPanel::UpdateWizDataBtns()
{
   TGo4Fitter* fitter = GetFitter();
   TGo4FitData* data = Wiz_SelectedData();

   Wiz_PFSetupBtn->setEnabled(data!=0);

   if (!WorkingWithPanel()) {
     Wiz_RebuildDataBtn->setShown(FALSE);
     Wiz_AddDataBtn->setShown(TRUE);
     Wiz_DelDataBtn->setShown(TRUE);
     Wiz_AddDataBtn->setEnabled(fitter!=0);
     Wiz_DelDataBtn->setEnabled(data!=0);
   } else {
     Wiz_RebuildDataBtn->setShown(TRUE);
     Wiz_RebuildDataBtn->setEnabled(fitter!=0);
     Wiz_AddDataBtn->setShown(FALSE);
     Wiz_DelDataBtn->setShown(FALSE);
   }
}

void TGo4FitPanel::UpdateWizModelsList(bool changestack)
{
    fbFillingWidget = true;

    Wiz_ModelList->clear();

    TGo4Fitter* fitter = GetFitter();

    Wiz_ShowAllMod->setChecked(fbWizShowAllModels);
    int selindx = -1;
    if(fitter!=0) {
       TGo4FitData* data = Wiz_SelectedData();
       int indx = 0;
       for(Int_t n=0;n<fitter->GetNumModel();n++) {
          TGo4FitModel* model = fitter->GetModel(n);
          if (data && !fbWizShowAllModels)
            if (!model->IsAssignTo(data->GetName())) continue;

          bool assign = FALSE;
          if (data && model->IsAssignTo(data->GetName())) assign = TRUE;
          if (assign) Wiz_ModelList->insertItem(QPixmap(SelectedXPM), model->GetName());
                 else Wiz_ModelList->insertItem(QPixmap(UnselectedXPM), model->GetName());

          if (strcmp(model->GetName(), fxWizModelName.latin1())==0) selindx = indx;
          indx++;
       }

       Wiz_ModelList->setSelected(selindx, TRUE);
       if (selindx>=0)
          Wiz_ModelList->setCurrentItem(selindx);

       if (selindx<0) fxWizModelName = "";
                 else Wiz_ModelList->ensureCurrentVisible();

    } else fxWizModelName = "";

    if (changestack)
      if (selindx>=0) fiWizPageIndex = 1; else
       if (fiWizPageIndex==1) fiWizPageIndex = 0;

    UpdateWizModelsBtns();

    fbFillingWidget = false;

    if(changestack)
      UpdateWizStackWidget();
}

void TGo4FitPanel::UpdateWizModelsBtns()
{
   TGo4Fitter* fitter = GetFitter();
   TGo4FitModel* model = Wiz_SelectedModel();

   Wiz_ShowAllMod->setShown(!WorkingWithOnlyPad());

   if (fitter) {
      Wiz_AddModelBtn->setEnabled(TRUE);
      Wiz_DelModelBtn->setEnabled(model!=0);
      Wiz_CloneModelBtn->setEnabled(model!=0);
      Wiz_ShowAllMod->setEnabled(TRUE);
   } else {
      Wiz_AddModelBtn->setEnabled(FALSE);
      Wiz_DelModelBtn->setEnabled(FALSE);
      Wiz_CloneModelBtn->setEnabled(FALSE);
      Wiz_ShowAllMod->setEnabled(FALSE);
   }
}

void TGo4FitPanel::UpdateWizStackWidget()
{
    fbFillingWidget = true;
    TGo4Fitter* fitter = GetFitter();

    int indx = -1;
    if (fitter==0) fiWizPageIndex = -1;
              else indx = fiWizPageIndex;

    QWidget* target = 0;

    switch (indx) {
       case -1:
          target = Wiz_EmptyPage;
          break;
       case 0: {
          target = Wiz_FitterPage;

          int typ = fitter->GetFitFunctionType();
          if (typ==TGo4Fitter::ff_user) typ = TGo4Fitter::ff_least_squares;
          Wiz_FitFuncCmb->setCurrentItem(typ);

          Wiz_UseAmplEstimChk->setChecked(fbUseAmplEstim);
          Wiz_MigradIterSpin->setValue(fiNumMigradIter);

          break;
       }

       case 1: {
          TGo4FitModel* model = Wiz_SelectedModel();
          if (model==0) break;

          target = Wiz_ModelPage;

          QString modelinfo;
          Wiz_GetModelInfo(model, &modelinfo);
          Wiz_ModelInfoLbl->setText(modelinfo);

          Wiz_BackgroundChk->setChecked(model->GetGroupIndex()==0);

          Wiz_ModelBufChk->setChecked(model->GetUseBuffers());
          Wiz_ModelBufChk->setEnabled(fiBuffersUsage==3);

          FillParsTable(Wiz_ParTable, fitter, model, FALSE, fxWizPars);

          break;
       }

       case 2: {
         TGo4FitData* data = Wiz_SelectedData();
         if (data==0) break;

         target = Wiz_DataPage;

         QString datainfo("Data: ");
         datainfo += data->GetName();
         datainfo += "  of class: ";
         datainfo += data->ClassName();
         if (fiIntegralMode==1) {
            datainfo += "\nCounts=";
            double v = fitter->CalculatesIntegral(data->GetName(),0,kTRUE);
            datainfo += QString::number(v);
         }
         if (fiIntegralMode==2) {
            datainfo += "\nIntegral=";
            double v = fitter->CalculatesIntegral(data->GetName(),0,kFALSE);
            datainfo += QString::number(v);
         }
         Wiz_DataInfoLbl->setText(datainfo);

         QString models("Models:");
         for(int n=0;n<fitter->GetNumModel();n++) {
            TGo4FitModel* model = fitter->GetModel(n);
            if (model->IsAssignTo(data->GetName())) {
               if (models.length()>45) {
                  models += " ...";
                  break;
               }
               models += " ";
               models += model->GetName();
            }
         }
         Wiz_DataModelsLbl->setText(models);

         Wiz_DataBufChk->setChecked(
           (fiBuffersUsage==1) || (fiBuffersUsage==2) ||
           ((fiBuffersUsage==3) && data->GetUseBuffers()));
         Wiz_DataBufChk->setEnabled(fiBuffersUsage==3);

         fxWizSlots->Clear();
         data->FillSlotList(fxWizSlots);

         Wiz_DataSlotsTable->setNumRows(fxWizSlots->GetLast()+1);

         for(int n=0;n<=fxWizSlots->GetLast();n++) {
           TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (fxWizSlots->At(n));
           if (slot==0) continue;

           Wiz_DataSlotsTable->verticalHeader()->setLabel(n, slot->GetName());
           TObject* obj = slot->GetObject();

           if (obj==0) {
              Wiz_DataSlotsTable->setText(n, 0, " --- ");
              Wiz_DataSlotsTable->setText(n, 1, slot->GetClass()->GetName());
              Wiz_DataSlotsTable->setText(n, 2, "false");
           } else {
              Wiz_DataSlotsTable->setText(n, 0, obj->GetName());
              Wiz_DataSlotsTable->setText(n, 1, obj->ClassName());
              if (slot->GetOwned()) Wiz_DataSlotsTable->setText(n, 2, "true");
                               else Wiz_DataSlotsTable->setText(n, 2, "false");
           }

           Wiz_DataSlotsTable->setText(n, 3, Wiz_GetSlotSourceInfo(slot));
         }

         for(int n=0;n<=3;n++)
           Wiz_DataSlotsTable->adjustColumn(n);

         Wiz_DataUseRangeBtn->setEnabled(FindPadWhereData(data)!=0);
         if (data!=0)
           Wiz_DataClearRangesBtn->setEnabled(data->IsAnyRangeLimits());
         Wiz_DrawDataBtn->setEnabled(data!=0);

         break;
      }
      case 3: {
         target = Wiz_PFPage;

         TGo4FitPeakFinder* finder = GetPeakFinder(true);
         if (finder==0) break;

         UpdateStatusBar(0);

         FindersTab->setCurrentPage(finder->GetPeakFinderType());

         Wiz_PFUsePolynChk->setChecked(finder->GetUsePolynom());
         Wiz_PFPolynSpin->setEnabled(finder->GetUsePolynom());
         Wiz_PFPolynSpin->setValue(finder->GetPolynomOrder());

         PF_MinWidthEdt->setText(QString::number(finder->Get0MinWidth()));
         PF_MaxWidthEdt->setText(QString::number(finder->Get0MaxWidth()));
         PF_AmplSlider->setValue(int(100*(1.-finder->Get0MaxAmplFactor())));
         UpdatePFAmplLbl();

         PF_WidthEdit->setText(QString::number(finder->Get1LineWidth()));

         int zn = int(10*finder->Get2NoiseFactor());  // from 1..100
         PF_RelNoiseSlider->setValue(101 - zn);
         UpdatePFRelNoiseLbl();
         PF_MinNoiseEdit->setText(QString::number(finder->Get2NoiseMinimum()));
         PF_SumUpSpin->setValue(finder->Get2ChannelSum());

         break;
      }
    }

    if (target) Wiz_Stack->raiseWidget(target);
           else Wiz_Stack->raiseWidget(Wiz_EmptyPage);

    fbFillingWidget = false;
}

void TGo4FitPanel::UpdateWizPaint(int mode)
{
    RemovePrimitives();

    if (!fbShowPrimitives) return;

    if (mode>=0) fiPaintMode = mode;

    TGo4Fitter* fitter = GetFitter();
    TGo4FitModel* model = Wiz_SelectedModel();
    TGo4FitData* data = Wiz_SelectedData();

    if ((fitter!=0) && (data!=0) && (fiPaintMode==1)) {
        TPad* pad = FindPadWhereData(data);

        if (PaintModelsFor(fitter, data, 0, false))
          for(Int_t n=0;n<data->GetNumRangeCondition();n++)
             PaintRange(data, n, pad, 0);
   }

   if ((fitter!=0) && (model!=0) && (fiPaintMode==2)) {
      for(Int_t n=0;n<model->NumAssigments();n++) {
         data = fitter->FindData(model->AssignmentName(n));
         if ((data!=0) &&  !data->IsAnyDataTransform()) {
            TPad* pad = FindPadWhereData(data);
            PaintModel(model, pad, 0);
         }
     }
   }
}

void TGo4FitPanel::Wiz_DataListSelect(int indx)
{
   if (fbFillingWidget) return;
   QString name = Wiz_DataList->text(indx);

   if ( (name==fxWizDataName) &&
        (fiWizPageIndex == 2) ) return;
   fxWizDataName = name;
   fiWizPageIndex = 2;
   UpdateWizDataBtns();
   UpdateWizModelsList(FALSE);
   UpdateWizStackWidget();
   UpdateWizPaint(1);
}

void TGo4FitPanel::Wiz_DataList_highlighted( int indx)
{
  Wiz_DataListSelect(indx);
}

void TGo4FitPanel::Wiz_ModelListSelect(int indx, bool ontext)
{
  if (fbFillingWidget) return;

  QString name = Wiz_ModelList->text(indx);

  bool needupdate = ( (name != fxWizModelName.latin1()) ||
                      (fiWizPageIndex != 1) );

  fxWizModelName = name;
  fiWizPageIndex = 1;

  if (!ontext) {

    TGo4Fitter* fitter = GetFitter();
    TGo4FitModel* model = Wiz_SelectedModel();
    TGo4FitData* data = Wiz_SelectedData();
    if ((fitter!=0) && (data!=0) && (model!=0)) {
       bool wasassigned = model->IsAssignTo(data->GetName());
       if (wasassigned) fitter->ClearModelAssignmentTo(model->GetName(), data->GetName());
                   else fitter->AssignModelTo(model->GetName(), data->GetName());

      fbFillingWidget = true;
      if (!wasassigned) Wiz_ModelList->changeItem(QPixmap(SelectedXPM), model->GetName(), indx);
                   else Wiz_ModelList->changeItem(QPixmap(UnselectedXPM), model->GetName(), indx);
      Wiz_ModelList->setSelected(indx, TRUE);
      fbFillingWidget = false;
      needupdate = TRUE;
    }
  }

  if (needupdate) {
     UpdateWizModelsBtns();
     UpdateWizStackWidget();
     UpdateWizPaint(2);
  }
}

void TGo4FitPanel::Wiz_ModelList_highlighted(int indx)
{
   Wiz_ModelListSelect(indx, TRUE);
}


void TGo4FitPanel::Wiz_AddDataBtn_clicked()
{
   if (fbFillingWidget) return;
   QPopupMenu menu(0,"WizPopup");

   Wiz_PrepareDataTypeList(&menu);
   int id =  menu.exec(Wiz_AddDataBtn->mapToGlobal(QPoint(5,5)));
   TGo4FitData* data = Wiz_CreateNewData(id);
   if (data) {
     fxWizDataName = data->GetName();
     fiWizPageIndex = 2;
     UpdateWizDataList();
     UpdateWizModelsList(FALSE);
     UpdateWizStackWidget();
     UpdateWizPaint(1);
     UpdateStatusBar(0);
   }
}


void TGo4FitPanel::Wiz_DelDataBtn_clicked()
{
  if (fbFillingWidget) return;

  if (Wiz_RemoveData()) {
    UpdateWizDataList();
    UpdateWizModelsList(FALSE);
    fiWizPageIndex = 0;
    UpdateWizStackWidget();
    UpdateWizPaint(0);
    UpdateStatusBar(0);
  }
}

void TGo4FitPanel::Wiz_AddModelBtn_clicked()
{
   if (fbFillingWidget) return;
   QPopupMenu menu(0,"WizPopup");

   Wiz_PrepareModelTypeList(&menu);
   int id =  menu.exec(Wiz_AddModelBtn->mapToGlobal(QPoint(5,5)));
   TGo4FitModel* model = Wiz_CreateNewModel(id);
   if (model) fxWizModelName = model->GetName();
   UpdateWizModelsList(TRUE);
   if(Wiz_SelectedModel()!=0) UpdateWizPaint(2);
   UpdateStatusBar(0);
}

void TGo4FitPanel::Wiz_DelModelBtn_clicked()
{
  if (fbFillingWidget) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  if(fbNeedConfirmation)
    if ( QMessageBox::information(0,"Delete models",
       QString("Are you sure to delete selected models"),
       QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

  for(uint n=0; n<Wiz_ModelList->count();n++) {
     if (!Wiz_ModelList->isSelected(n)) continue;
     QString name = Wiz_ModelList->text(n);
     fitter->RemoveModel(name, kTRUE);
  }

  fxWizModelName = "";
  UpdateWizModelsList(TRUE);
  UpdateWizPaint(0);
  UpdateStatusBar(0);
}

void TGo4FitPanel::Wiz_CloneModelBtn_clicked()
{
  if (fbFillingWidget) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  for(uint n=0; n<Wiz_ModelList->count();n++) {
     if (!Wiz_ModelList->isSelected(n)) continue;
     QString name = Wiz_ModelList->text(n);
     fitter->CloneModel(name);
  }

  fxWizModelName = "";
  UpdateWizModelsList(TRUE);
  UpdateWizPaint(0);
  UpdateStatusBar(0);
}

void TGo4FitPanel::Wiz_ShowAllMod_toggled(bool zn)
{
  if (fbFillingWidget) return;
  fbWizShowAllModels = zn;
  UpdateWizModelsList(TRUE);
  UpdateWizPaint(2);
}


void TGo4FitPanel::Wiz_FitFuncCmb_activated( int typ)
{
  if (!fbFillingWidget && GetFitter())
    GetFitter()->SetFitFunctionType(typ);
}


void TGo4FitPanel::Wiz_FitNameEdt_textChanged( const QString & name)
{
  TGo4Fitter* fitter = GetFitter();
  if(fbFillingWidget || (fitter==0) || (name.length()==0)) return;
  fitter->SetName(name);

  fiWizPageIndex = 0;
  UpdateWizStackWidget();
  UpdateWizPaint(0);

  UpdateStatusBar(0);
}


void TGo4FitPanel::Wiz_ParTable_valueChanged( int nrow, int ncol)
{
  ParsTableChanged(Wiz_ParTable, nrow, ncol, FALSE, fxWizPars, TRUE);
}


void TGo4FitPanel::Wiz_DataList_doubleClicked( QListBoxItem * )
{
  TGo4Fitter* fitter = GetFitter();
  TGo4FitData* data = Wiz_SelectedData();
  if ((data==0) || (fitter==0)) return;
  bool ok;
  QString newname = QInputDialog::getText("Change data name", "Input new name", QLineEdit::Normal, data->GetName(), &ok, this);
  if (ok && (newname.length()>0) && (newname!=data->GetName())) {
     if (fitter->FindData(newname)) {
       QMessageBox::information(this, "Fit panel", "Unable to rename data.\n Name " +
                                newname + " already exists","return");
       return;
     }
     fitter->ChangeDataNameInAssignments(data->GetName(), newname);
     data->SetName(newname);
     fxWizDataName = newname;
     UpdateWizDataList();
     UpdateWizPaint(1);
  }
}


void TGo4FitPanel::Wiz_ModelList_doubleClicked( QListBoxItem * )
{
  TGo4Fitter* fitter = GetFitter();
  TGo4FitModel* model = Wiz_SelectedModel();
  if ((model==0) || (fitter==0)) return;
  bool ok;
  QString newname = QInputDialog::getText("Change model name", "Input new name", QLineEdit::Normal, model->GetName(), &ok, this);
  if (ok && (newname.length()>0) && (newname!=model->GetName())) {
     if (fitter->FindModel(newname)) {
       QMessageBox::information(this, "Fit panel", QString("Unable to rename model.\n Name ")+
                                newname + " already exists\n", "Return");
       return;
     }
     model->SetName(newname);
     fxWizModelName = newname;
     UpdateWizModelsList(TRUE);
     UpdateWizPaint(2);
   }
}

void TGo4FitPanel::Wiz_RebuildDataBtn_clicked()
{
   Wiz_RebuildDataList();
}

void TGo4FitPanel::Wiz_DataBufChk_toggled( bool zn)
{
   if (fbFillingWidget) return;
   TGo4FitData* data = Wiz_SelectedData();
   if (data) data->SetUseBuffers(zn);
}


void TGo4FitPanel::Wiz_UseAmplEstimChk_toggled( bool chk)
{
   if (!fbFillingWidget)
     fbUseAmplEstim = chk;
}


void TGo4FitPanel::Wiz_MigradIterSpin_valueChanged( int num)
{
   if (!fbFillingWidget)
     fiNumMigradIter = num;
}


void TGo4FitPanel::Wiz_DataSlotsTable_contextMenuRequested( int nrow, int ncol, const QPoint& pnt )
{
  if (fbFillingWidget || (fxWizSlots==0)) return;

  if ((nrow<0) || (ncol<0) || (nrow>fxWizSlots->GetLast())) return;

  TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (fxWizSlots->At(nrow));
  if (slot==0) return;

  QPopupMenu menu(0,"SlotPopup");

  if (FillPopupForSlot(slot,&menu)) {
     int id =  menu.exec(pnt);
     ExecutePopupForSlot(0, slot, id /*, menu.text(id) */);
  }
}

void TGo4FitPanel::Wiz_DataUseRangeBtn_clicked()
{
  if (!fbFillingWidget)
    Wiz_UseSelectedRange();
}


void TGo4FitPanel::Wiz_DataClearRangesBtn_clicked()
{
  if (fbFillingWidget) return;
  TGo4FitData* data = Wiz_SelectedData();
  if (data!=0) {
     data->ClearRanges();
     UpdateActivePage();
  }
}

void TGo4FitPanel::Wiz_DrawDataBtn_clicked()
{
  if (fbFillingWidget) return;
  if (Wiz_SelectedData()!=0)
    Button_FitterDraw(Wiz_SelectedData());
}

void TGo4FitPanel::MainAttachBtn_clicked()
{
  UpdateStatusBar(0);

  Button_WorkWithPanel();
}

void TGo4FitPanel::MainFitBtn_clicked()
{
   QApplication::setOverrideCursor(Qt::WaitCursor);

   Button_PerformFit();

   QApplication::restoreOverrideCursor();
}

void TGo4FitPanel::MainDrawBtn_clicked()
{
  Button_FitterDraw(0);
}

void TGo4FitPanel::MainFindBtn_clicked()
{
   if (GetFitter()!=0)
      Button_PeakFinder();
}

void TGo4FitPanel::Wiz_DataList_clicked( QListBoxItem * )
{
  Wiz_DataListSelect(Wiz_DataList->currentItem());
}

void TGo4FitPanel::Wiz_DataList_pressed( QListBoxItem * )
{
  Wiz_DataListSelect(Wiz_DataList->currentItem());
}

void TGo4FitPanel::Wiz_DataList_selected( int indx)
{
   Wiz_DataListSelect(indx);
}

void TGo4FitPanel::Wiz_ModelList_selected( int indx)
{
   Wiz_ModelListSelect(indx, TRUE);
}

void TGo4FitPanel::MainParsBtn_clicked()
{
   fbParsWidgetShown = !fbParsWidgetShown;

   UpdateActivePage();
}

void TGo4FitPanel::FillParsWidget()
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;
  fbFillingWidget = true;

  Double_t FF = fitter->GetResultFF();
  Int_t NDF = fitter->GetResultNDF();
  if ((FF==0) && (NDF==0)) Par_FitterResLbl->hide();
  else {
    Par_FitterResLbl->setText("Result: Fit func = " + QString::number(FF) +
                                    "  NDF = " + QString::number(NDF));
    Par_FitterResLbl->show();
  }

  FillParsTable(ParsTable, fitter, 0, LineParsChk->isChecked(), fxParsTableList);
  fbFillingWidget = false;
}

void TGo4FitPanel::LineParsChk_toggled(bool)
{
  if (fbFillingWidget) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;
  fbFillingWidget = true;
  FillParsTable(ParsTable, fitter, 0, LineParsChk->isChecked(), fxParsTableList);
  WidgetStack->raiseWidget(PageParameters);
  fbFillingWidget = false;
}


void TGo4FitPanel::ParsTable_valueChanged( int nrow, int ncol)
{
  if (!fbFillingWidget)
     ParsTableChanged(ParsTable, nrow, ncol, LineParsChk->isChecked(), fxParsTableList, FALSE);
}


void TGo4FitPanel::FillParsTable(QTable* table, TGo4Fitter* fitter, TGo4FitModel* model, bool LinesView, TObjArray* TableList) {
   if ((table==0) || (fitter==0) || (TableList==0)) return;

   if (model && LinesView) return;
   TableList->Clear();

   for (int n=0;n<table->numRows();n++) table->clearCell(n, 0);

   for (int nr=0;nr<table->numRows();nr++)
     for (int nc=0;nc<table->numCols();nc++)
       if (table->item(nr,nc))
          table->item(nr,nc)->setEnabled(TRUE);

   if (LinesView) {
     int MaxAxis = 0;
     for (Int_t n=0; n<fitter->GetNumModel();n++) {
        TGo4FitModel* m = fitter->GetModel(n);
        if (m==0) continue;
        Double_t zn;
        for (int naxis=0;naxis<10;naxis++)
          if (m->GetPosition(naxis,zn) || m->GetWidth(naxis,zn)) MaxAxis = naxis;
     }

     int nfcol = (fiIntegralMode>0) ? 2 : 1;
     table->setNumCols(nfcol+(MaxAxis+1)*2);
     table->horizontalHeader()->setLabel(0,"Amplitude");
     if (fiIntegralMode==1)
        table->horizontalHeader()->setLabel(1,"Counts");
     else if (fiIntegralMode==2)
        table->horizontalHeader()->setLabel(1,"Integral");
     else if (fiIntegralMode==3)
        table->horizontalHeader()->setLabel(1,"Gauss Int");

     for (int naxis=0;naxis<=MaxAxis;naxis++) {
        QString capt = QString("Position ") + QString::number(naxis);
        table->horizontalHeader()->setLabel(nfcol + naxis*2, capt);
        if (fbRecalculateGaussWidth)
          capt = QString("FWHM ");
        else
          capt = QString("Sigma ");
        capt += QString::number(naxis);
        table->horizontalHeader()->setLabel(nfcol+1 + naxis*2, capt);
     }

     table->setNumRows(fitter->GetNumModel());

     for (Int_t n=0; n<fitter->GetNumModel();n++) {
        TGo4FitModel* m = fitter->GetModel(n);
        TableList->Add(m);

        bool recalculatew = (m->InheritsFrom(TGo4FitModelGauss1::Class()) ||
                             m->InheritsFrom(TGo4FitModelGauss2::Class()) ||
                             m->InheritsFrom(TGo4FitModelGaussN::Class())) &&
                             fbRecalculateGaussWidth;
        double widthk = recalculatew ? 2.3548 : 1.0;

        table->verticalHeader()->setLabel(n, m->GetName());
        table->setText(n, 0, QString::number(m->GetAmplValue()));
        table->item(n,0)->setEnabled(m->GetAmplPar());

        if (fiIntegralMode>0) {
           Double_t v = 0.;
           TGo4FitData* data = Wiz_SelectedData();
           switch (fiIntegralMode) {
              case 1:
                 if (data!=0)
                   v = fitter->CalculatesIntegral(data->GetName(), m->GetName(), kTRUE);
                 else
                   v = fitter->CalculatesModelIntegral(m->GetName(), kTRUE);
                 break;
              case 2:
                 if (data!=0)
                   v = fitter->CalculatesIntegral(data->GetName(), m->GetName(), kFALSE);
                 else
                   v = fitter->CalculatesModelIntegral(m->GetName(), kFALSE);
                 break;
              case 3: v = m->Integral(); break;
              default: v = 0;
           }
           if ((v<=0.) && (fiIntegralMode==3))
             table->setText(n, 1, "---");
           else
             table->setText(n, 1, QString::number(v));
           table->item(n,1)->setEnabled(FALSE);
        }

        for (int naxis=0;naxis<=MaxAxis;naxis++) {

            Double_t pos, width;

            if (m->GetPosition(naxis,pos)) {
              table->setText(n, nfcol+naxis*2, QString::number(pos));
              table->item(n, nfcol+naxis*2)->setEnabled(TRUE);
            } else {
              table->setText(n, nfcol+naxis*2, "---");
              table->item(n, nfcol+naxis*2)->setEnabled(FALSE);
            }

            if (m->GetWidth(naxis,width)) {
               table->setText(n, nfcol+1+naxis*2, QString::number(width*widthk));
               table->item(n, nfcol+1+naxis*2)->setEnabled(TRUE);
            } else {
              table->setText(n, nfcol+1+naxis*2, "---");
              table->item(n, nfcol+1+naxis*2)->setEnabled(FALSE);
            }
        }
     }

   } else {

     TGo4FitParsList* pars = (model==0) ? dynamic_cast<TGo4FitParsList*> (fitter) :
                                          dynamic_cast<TGo4FitParsList*> (model);
     if (pars==0) return;

     for(Int_t n=0; n<pars->NumPars();n++)
       TableList->Add(pars->GetPar(n));

     table->setNumCols(6);
     table->horizontalHeader()->setLabel(0,"Fixed");
     table->horizontalHeader()->setLabel(1,"Value");
     table->horizontalHeader()->setLabel(2,"Error");
     table->horizontalHeader()->setLabel(3,"Epsilon");
     table->horizontalHeader()->setLabel(4,"Min");
     table->horizontalHeader()->setLabel(5,"Max");

     table->setNumRows(TableList->GetLast()+1);

     for(Int_t n=0;n<=TableList->GetLast();n++) {
       TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (TableList->At(n));
       if (par==0) continue;

       if (model) table->verticalHeader()->setLabel(n, par->GetName());
             else table->verticalHeader()->setLabel(n, par->GetFullName());

       QCheckTableItem* checkitem = new QCheckTableItem(table, "fix");
       checkitem->setChecked(par->GetFixed());
       table->setItem(n, 0, checkitem);

       table->setText(n, 1, QString::number(par->GetValue()));
       table->item(n,1)->setEnabled(!par->GetFixed());

       table->setText(n, 2, QString::number(par->GetError()));

       Double_t eps, min, max;
       if (par->GetEpsilon(eps)) table->setText(n, 3, QString::number(eps));
                            else table->setText(n, 3, "");


       if (par->GetRange(min,max)) {
         table->setText(n, 4, QString::number(min));
         table->setText(n, 5, QString::number(max));
       } else {
         table->setText(n, 4, "");
         table->setText(n, 5, "");
       }
     }
   }

   for(int n=0;n<table->numCols();n++)
     table->adjustColumn(n);

   table->verticalHeader()->adjustHeaderSize();
   table->horizontalHeader()->adjustHeaderSize();
}

void TGo4FitPanel::ParsTableChanged(QTable* table, int nrow, int ncol, bool LinesView, TObjArray* TableList, bool updatepaint) {
   if (fbFillingWidget || (table==0) || (TableList==0)) return;

   if ((nrow<0) || (nrow>TableList->GetLast())) return;

   if (LinesView) {
      TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (TableList->At(nrow));
      if (model==0) return;

      bool recalculatew = (model->InheritsFrom(TGo4FitModelGauss1::Class()) ||
                           model->InheritsFrom(TGo4FitModelGauss2::Class()) ||
                           model->InheritsFrom(TGo4FitModelGaussN::Class())) &&
                           fbRecalculateGaussWidth;
      double widthk = recalculatew ? 2.3548 : 1.0;

      bool ok = FALSE;
      double zn = table->text(nrow, ncol).toDouble(&ok);
      if (!ok) return;

      if (ncol==0) model->SetAmplValue(zn); else {
         int nfcol = (fiIntegralMode>0) ? 2 : 1;
         int naxis = (ncol-nfcol) /2;
         if ((ncol-nfcol) % 2 == 0) model->SetPosition(naxis, zn);
                               else model->SetWidth(naxis, zn/widthk);
      }

   } else {

       TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (TableList->At(nrow));
       if (par==0) return;

       switch (ncol) {
         case 0: {
           QCheckTableItem* checkitem = dynamic_cast<QCheckTableItem*> ( table->item(nrow,0));
           if (checkitem) par->SetFixed(checkitem->isChecked());
           table->item(nrow,1)->setEnabled(!par->GetFixed());
           break; }
         case 1: {
           if (par->GetFixed()) break;
           bool ok = FALSE;
           double zn = table->text(nrow,1).toDouble(&ok);
           if (ok) {
             par->SetValue(zn);
             if (updatepaint) UpdateWizPaint(2);
           }
           break; }
         case 2: {
           if (par->GetFixed()) break;
           bool ok = FALSE;
           double zn = table->text(nrow,2).toDouble(&ok);
           if (ok) par->SetError(zn);
           break; }
         case 3: {
           bool ok = FALSE;
           double zn = table->text(nrow,3).toDouble(&ok);
           if (ok) par->SetEpsilon(zn);
           break; }
         case 4:
         case 5: {
           cout << "Changing col " << ncol << endl;

           bool ok = FALSE;
           bool range_changed = false;
           double zn = table->text(nrow, ncol).toDouble(&ok);
           if(ok) {

             bool changemin = false;
             bool changemax = false;

             Double_t min, max;
             if (!par->GetRange(min,max)) {
                double vvv = par->GetValue();
                if (ncol==4) {
                   min = zn;
                   if (min<vvv) max = 2*vvv - min;
                           else max = min;
                   changemax = true;
                } else {
                   max = zn;
                   if (max>vvv) min = 2*vvv - max;
                           else min = max;
                   changemin = true;
                }
             } else
             if (ncol==4) {
               min = zn;
               if (max<min) { max = min; changemax = true; }
             } else
             if (ncol==5) {
               max = zn;
               if (min>max) { min = max; changemin = true; }
             }
             par->SetRange(min,max);
             range_changed = true;
             fbFillingWidget = true;
             if (changemin)
               table->setText(nrow, 4, QString::number(min));
             if (changemax)
               table->setText(nrow, 5, QString::number(max));
             fbFillingWidget = false;
           } else {
             if ((table->text(nrow, ncol).length()==0) &&
                 (table->text(nrow, 9-ncol).length()==0)) {
                    par->ClearRange();
                    range_changed = true;
                 }
           }

           // reset first columns of pars list when we touch range values
           if (range_changed) {
              fbFillingWidget = true;
              QCheckTableItem* checkitem = dynamic_cast<QCheckTableItem*> ( table->item(nrow,0));
              if (checkitem) checkitem->setChecked(par->GetFixed());
              table->item(nrow,1)->setEnabled(!par->GetFixed());
              table->setText(nrow, 1, QString::number(par->GetValue()));
              fbFillingWidget = false;
           }

           break; }
       }
   }
}


void TGo4FitPanel::Wiz_ModelBufChk_toggled( bool zn)
{
  if (fbFillingWidget) return;
  TGo4FitModel* model = Wiz_SelectedModel();
  if (model) model->SetUseBuffers(zn);
}


void TGo4FitPanel::Wiz_PFSetupBtn_clicked()
{
  if (fbFillingWidget) return;

  if (Wiz_SelectedData()==0) return;
  fiWizPageIndex = 3;
  UpdateWizStackWidget();
  UpdateWizPaint(1);
}


void TGo4FitPanel::Wiz_PFUsePolynChk_toggled( bool zn )
{
  if (fbFillingWidget) return;

  TGo4FitPeakFinder* finder = GetPeakFinder(true);
  if (finder!=0) finder->SetUsePolynom(zn);

  Wiz_PFPolynSpin->setEnabled(zn);
}

void TGo4FitPanel::Wiz_PFPolynSpin_valueChanged( int zn )
{
  if (fbFillingWidget) return;

  TGo4FitPeakFinder* finder = GetPeakFinder(true);
  if (finder!=0) finder->SetPolynomOrder(zn);
}


void TGo4FitPanel::Smp_PolynomBtn_clicked()
{
  Button_SimpleFit(0);
}

void TGo4FitPanel::Smp_GaussianBtn_clicked()
{
  Button_SimpleFit(1);
}

void TGo4FitPanel::Smp_LorenzBtn_clicked()
{
  Button_SimpleFit(2);
}

void TGo4FitPanel::Smp_ExponentBtn_clicked()
{
  Button_SimpleFit(3);
}

void TGo4FitPanel::Smp_ClearBtn_clicked()
{
  Button_SimpleClear();
}

void TGo4FitPanel::Wiz_MinSetupBtn_clicked()
{
  if (fbFillingWidget) return;

  fiWizPageIndex = 0;
  UpdateWizStackWidget();
  UpdateWizPaint(0);
}

void TGo4FitPanel::FindersTab_currentChanged( QWidget * )
{
  if (fbFillingWidget) return;

  TGo4FitPeakFinder* finder = GetPeakFinder(true);
  if (finder)
    finder->SetPeakFinderType(FindersTab->currentPageIndex());
}

void TGo4FitPanel::Wiz_ModelList_clicked( QListBoxItem * item, const QPoint & pnt)
{
  if (item==0) return;

  QRect rect = Wiz_ModelList->itemRect(item);
  QPoint pnt2 = Wiz_ModelList->viewport()->mapFromGlobal(pnt);

  Wiz_ModelListSelect(Wiz_ModelList->currentItem(), (pnt2.x() - rect.left()) >=15 );
}

void TGo4FitPanel::Wiz_BackgroundChk_toggled( bool chk)
{
  if (fbFillingWidget) return;

  TGo4FitModel* model = Wiz_SelectedModel();
  if (model!=0)
    if (chk) model->SetBackgroundGroupIndex();
        else model->SetGroupIndex(-1);
}

void TGo4FitPanel::PF_MinWidthEdt_returnPressed()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::PF_MaxWidthEdt_returnPressed()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::PF_WidthEdit_returnPressed()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::PF_MinNoiseEdit_returnPressed()
{
  if (!fbFillingWidget)
    Button_PeakFinder();
}

void TGo4FitPanel::ParsTable_contextMenuRequested(int nrow, int ncol, const QPoint &)
{
}

TGo4Fitter* TGo4FitPanel::CreateFitterFor(TGo4ViewPanel* panel, TPad* pad, const char* name)
{
   if ((panel==0) || (pad==0)) return 0;

   TGo4Fitter* fitter = new TGo4Fitter(name,"Fitter object");

   CreateDataFor(panel, pad, fitter);

   fitter->AddStandardActions();

   fitter->SetMemoryUsage(3);

   fitter->SetFitFunctionType(TGo4Fitter::ff_chi_square);

   return fitter;
}

void TGo4FitPanel::CreateDataFor(TGo4ViewPanel* panel, TPad* pad, TGo4Fitter* fitter)
{
   if ((fitter==0) || (panel==0) || (pad==0)) return;

   int npads = panel->GetNumberOfPads(pad);

   for (Int_t n=0;n<npads;n++) {
      TPad* subpad = panel->GetSubPad(pad, n, false);
      if (panel->IsPadHasSubPads(subpad)) continue;
      TObject* obj = panel->GetPadMainObject(subpad);
      if (obj==0) continue;

      TGo4FitData* data = 0;

      if (obj->InheritsFrom(TH1::Class()))
         data = new TGo4FitDataHistogram(fitter->FindNextName("Data",0,kFALSE));
      else
      if (obj->InheritsFrom(TGraph::Class()))
         data = new TGo4FitDataGraph(fitter->FindNextName("Data",0,kFALSE));

      if (data!=0)
         fitter->AddData(data);
   }
}

void TGo4FitPanel::UpdateActivePage()
{
   int select = fiPanelMode;
   if (GetFitter()==0) select = FitGui::pm_None;

   if (fbParsWidgetShown && (select!=FitGui::pm_None)) select = 100;

   switch (select) {
      case FitGui::pm_None: break;
      case FitGui::pm_Simple: UpdateSimplePage(); break;
      case FitGui::pm_Wizard: UpdateWizardPage(); break;
      case FitGui::pm_Expert: UpdateExtendedPage(); break;
      case 100: {
         FillParsWidget();
         if (fiPanelMode==FitGui::pm_Wizard) {
            RemovePrimitives();
            UpdateWizPaint(-1);
         }
         break;
      }
   }

   ShowPanelPage(select);
   UpdateStatusBar(0);
   UpdateItemMenu();
}

void TGo4FitPanel::UpdateSimplePage()
{
  RemovePrimitives();

  PaintFitter(GetFitter(), 0, TRUE);

  UpdateStatusBar(0);
}

void TGo4FitPanel::UpdateWizardPage()
{
   RemovePrimitives();

   TGo4Fitter* fitter = GetFitter();
   fbFillingWidget = true;
   if (fitter) Wiz_FitNameEdt->setText(QString(fitter->GetName()));
          else Wiz_FitNameEdt->setText(QString(""));
   fbFillingWidget = false;

   UpdateWizDataList();

   UpdateWizModelsList(FALSE);

   UpdateWizStackWidget();

   UpdateWizPaint(-1);

   UpdateStatusBar(0);
}

void TGo4FitPanel::UpdateExtendedPage()
{
   RemoveItemWidget();

   RemovePrimitives();

   FitList->clear();
   FitList->setSorting(-1);
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) {
     new QFitItem(this, FitList, 0, FitGui::ot_empty, FitGui::wt_none, FitGui::mt_empty);
   } else {
      QFitItem* fitteritem = new QFitItem(this, FitList, fitter, FitGui::ot_fitter, FitGui::wt_fitter, FitGui::mt_fitter, FitGui::gt_fitter);
      fitteritem->setOpen(TRUE);
      FitList->setSelected(fitteritem, kTRUE);
      ShowItem(fitteritem, false);
   }

   UpdateItemMenu();

   UpdateStatusBar(0);
}

void TGo4FitPanel::RemovePrimitives()
{
   if (!WorkingWithPanel()) return;

   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return;

   for(Int_t n = 0; n<fitter->NumSlots();n++) {
      TPad* pad = FindPadForSlot(fitter->GetSlot(n));
      if (pad==0) continue;
      if (ActivePanel()->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitArrows))
        ActivePanel()->ShootRepaintTimer();
   }
}


bool TGo4FitPanel::FillPopupForItem(QFitItem* item, QPopupMenu* menu)
{
  if((item==0) || (menu==0)) return false;

  if(item->PopupMenuType() == FitGui::mt_empty) {
     menu->insertItem(QString("Create empty fitter"),8);
     if (WorkingWithPanel()) menu->insertItem(QString("Create appropriate fitter"),9);
  }

  if(item->PopupMenuType() == FitGui::mt_fitter) {
    menu->insertItem(QString("Delete fitter"),1);
    menu->insertItem(QString("Clear fitter"),2);
    menu->insertItem(QString("Save fitter"),3);
    menu->insertItem(QString("Save fitter as ..."),4);
    menu->insertItem(QString("Print ..."),5);
    menu->insertItem(QString("Draw"),6);
    menu->insertItem(QString("Memorize parameters"), 706);
    menu->insertItem(QString("Remember parameters"), 707);

  }

  if(item->PopupMenuType() == FitGui::mt_data) {
     if (!WorkingWithPanel())
       menu->insertItem(QString("Remove ")+item->Object()->GetName(),101);
     menu->insertItem(QString("Remove assosiated models"),102);
     menu->insertItem(QString("Draw"),105);
     menu->insertItem(QString("Print"),5);
  }

  if(item->PopupMenuType() == FitGui::mt_datalist)
    if (WorkingWithPanel()) {
       menu->insertItem(QString("Rebuild data list"),108);
    } else {
       menu->insertItem(QString("Delete all data"),107);
       FillDataTypesList(menu, 110);
    }

  if(item->PopupMenuType() == FitGui::mt_model) {
     menu->insertItem(QString("Remove ")+item->Object()->GetName(), 201);
     menu->insertItem(QString("Clone"), 203);
     menu->insertItem(QString("Print"), 5);
  }

  if(item->PopupMenuType() == FitGui::mt_modellist) {
    menu->insertItem(QString("Delete all models"),202);
    FillModelTypesList(menu, 210, TRUE);
  }

  if(item->PopupMenuType() == FitGui::mt_asslist) {
      TGo4Fitter* fitter = GetFitter();
      TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
      if (fitter && model) {
         menu->insertItem(QString("Clear all assignments"), 301);
         menu->insertItem(QString("Assign to all data"), 302);
         menu->insertSeparator();
        for(Int_t n=0; n<fitter->GetNumData();n++)
          if(!model->IsAssignTo(fitter->GetDataName(n)))
            menu->insertItem(QString("Assign to: ") + fitter->GetDataName(n), 310+n);
      }
  }

  if(item->PopupMenuType() == FitGui::mt_ass)
     menu->insertItem(QString("Clear assignment to ") + item->text(0), 303);

  if(item->PopupMenuType() == FitGui::mt_range)
      menu->insertItem(QString("Remove range condition"), 401);

  if(item->PopupMenuType() == FitGui::mt_rangecut)
      menu->insertItem(QString("Remove range condition"), 401);

  if(item->PopupMenuType() == FitGui::mt_rangelist) {
     TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (item->Object());
     if(comp) {
        if (comp->IsAnyRangeLimits()>0) {
           menu->insertItem(QString("Clear all ranges"), 402);
           menu->insertSeparator();
        }
        menu->insertItem(QString("Include new range"), 403);
        menu->insertItem(QString("Exclude new range"), 404);
        menu->insertItem(QString("Add left bound"), 405);
        menu->insertItem(QString("Add right bound"), 406);
        menu->insertSeparator();
        menu->insertItem(QString("Add range cut"), 407);
     }
  }

  if(item->PopupMenuType() == FitGui::mt_action) {
      menu->insertItem(QString("Delete action"), 501);
      menu->insertItem(QString("Move action up"), 502);
      menu->insertItem(QString("Move action down"), 503);
      menu->insertItem(QString("Print"),5);
      menu->insertItem(QString("Execute"), 508);
  }

  if(item->PopupMenuType() == FitGui::mt_actlist) {
     TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
     if(fitter) {
        if (fitter->GetNumActions()>0) {
           menu->insertItem(QString("Execute norm actions"), 504);
           menu->insertItem(QString("Execute all actions"), 507);
           menu->insertItem(QString("Delete all actions"), 505);
           menu->insertItem(QString("Delete output actions"), 506);
           menu->insertSeparator();
        }
        menu->insertItem(QString("Add config"), 510);
        menu->insertItem(QString("Add peak finder"), 514);
        menu->insertItem(QString("Add amplitude estimation"), 511);
        menu->insertItem(QString("Add minuit"), 512);
        menu->insertItem(QString("Add output"), 513);
     }
  }

  if(item->PopupMenuType() == FitGui::mt_depend)
      menu->insertItem(QString("Delete item"), 601);

  if(item->PopupMenuType() == FitGui::mt_deplist) {
      menu->insertItem(QString("Delete all items"), 602);
      menu->insertItem(QString("Insert new item"), 603);
  }

  if(item->PopupMenuType() == FitGui::mt_newpars) {
     menu->insertItem(QString("Delete all items"), 701);
     menu->insertItem(QString("Insert new item"), 702);
  }

  if(item->PopupMenuType() == FitGui::mt_parcfg)
     menu->insertItem(QString("Delete item"), 703);

  if(item->PopupMenuType() == FitGui::mt_par) {
     menu->insertItem(QString("Memorize value"), 704);
     menu->insertItem(QString("Remember value"), 705);
  }

  if((item->PopupMenuType() == FitGui::mt_parslist)) {
     menu->insertItem(QString("Memorize parameters"), 706);
     menu->insertItem(QString("Remember parameters"), 707);
  }

  if(item->PopupMenuType() == FitGui::mt_minuitres) {
     menu->insertItem(QString("Delete result entry"), 801);
     menu->insertItem(QString("Print"),5);
  }

  if(item->PopupMenuType() == FitGui::mt_slot) {
     TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (item->Object());
     if (slot!=0) FillPopupForSlot(slot,menu);
  }

  if(item->PopupMenuType() == FitGui::mt_allslots) {
    TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
    if(fitter) {
       menu->insertItem(QString("Update slots from sources"), 904);
    }
  }

  return (menu->count()>0);
}

void TGo4FitPanel::ExecPopupForItem(QFitItem * item, int id)
{
  switch(id) {
    case   1: Cmd_DeleteFitter(); break;
    case   2: Cmd_ClearFitter(); break;
    case   3: Cmd_SaveFitter(FALSE); break;
    case   4: Cmd_SaveFitter(TRUE); break;
    case   5: Cmd_ItemPrint(item); break;
    case   6: Button_FitterDraw(0); break;
    case   8: Cmd_CreateFitter(); break;
    case   9: Cmd_CreateAppropriateFitter(); break;
    case 101: Cmd_DeleteData(item); break;
    case 102: Cmd_DeleteAssosiatedModels(item); break;
    case 105: Cmd_DrawData(item); break;
    case 107: Cmd_DeleteAllData(item); break;
    case 108: Wiz_RebuildDataList(); break;
    case 201: Cmd_DeleteModel(item); break;
    case 202: Cmd_DeleteModels(item); break;
    case 203: Cmd_CloneModel(item); break;
    case 301: Cmd_ClearAssigments(item); break;
    case 302: Cmd_AssignModelToAllData(item); break;
    case 303: Cmd_ClearAssigment(item); break;
    case 401: Cmd_RemoveRangeCondition(item); break;
    case 402: Cmd_RemoveRangeConditions(item); break;
    case 403:
    case 404:
    case 405:
    case 406:
    case 407: Cmd_AddRangeCondition(item, id-403); break;
    case 501: Cmd_DeleteAction(item); break;
    case 502: Cmd_MoveAction(item, -1); break;
    case 503: Cmd_MoveAction(item, +1); break;
    case 504: Cmd_ExecuteActions(item, FALSE); break;
    case 505: Cmd_DeleteActions(item); break;
    case 506: Cmd_DeleteOutputActions(item); break;
    case 507: Cmd_ExecuteActions(item, TRUE); break;
    case 508: Cmd_ExecuteAction(item); break;
    case 601: Cmd_DeleteDependency(item); break;
    case 602: Cmd_DeleteDependencies(item); break;
    case 603: Cmd_AddDependency(item); break;
    case 701: Cmd_DeletePars(item); break;
    case 702: Cmd_AddNewPar(item); break;
    case 703: Cmd_DeletePar(item); break;
    case 704: Cmd_MemorizePar(item); break;
    case 705: Cmd_RememberPar(item); break;
    case 706: Cmd_MemorizePars(item); break;
    case 707: Cmd_RememberPars(item); break;
    case 801: Cmd_DeleteMinuitResult(item); break;
    case 904: Cmd_UpdateAllSlots(item); break;

    default:
      if ((id>=110) && (id<200)) Cmd_AddNewData(item,id-110); else
      if ((id>=210) && (id<300)) Cmd_AddNewModel(item,id-210); else
      if ((id>=310) && (id<400)) Cmd_AssignModelToData(item,id-310); else
      if ((id>=510) && (id<600)) Cmd_AddNewAction(item,id-510); else
      if ((id>=1000) && (id<2000)) ExecutePopupForSlot(item, 0, id);
  }
}

bool TGo4FitPanel::FillPopupForSlot(TGo4FitSlot* slot, QPopupMenu* menu)
{
   if ((slot==0) || (menu==0)) return false;

   if(!slot->GetOwned() && slot->IsObject())
     menu->insertItem(QString("Clone object in slot"), 1000);

   if(slot->IsObject())
     menu->insertItem(QString("Clear object in slot"), 1001);

   if (FindPadForSlot(slot)!=0)
      menu->insertItem(QString("Update from ") + Wiz_GetSlotSourceInfo(slot), 1002);

   uint cnt=menu->count();
   if (cnt>0) menu->insertSeparator();

   if (slot->GetConnectedSlot())
     menu->insertItem(QString("Brake connection to slot"), 1004);
   TGo4Fitter* fitter = GetFitter();
   if (fitter)
     for(Int_t n=0;n<fitter->NumSlots();n++) {
        TGo4FitSlot* sl = fitter->GetSlot(n);
        if (slot->CanConnectToSlot(sl) && (slot->GetConnectedSlot()!=sl))
          menu->insertItem(QString("Connect to ") + sl->GetFullName(), 1400 + n);
     }

   if (cnt<menu->count()) menu->insertSeparator();

   if (slot->GetClass() == TGo4FitData::Class()) FillDataTypesList(menu, 1100); else
   if (slot->GetClass() == TGo4FitModel::Class()) FillModelTypesList(menu, 1200, FALSE); else
   if (slot->GetClass() == TGo4FitAxisTrans::Class()) {
      menu->insertItem(QString(TGo4FitLinearTrans::Class()->GetName()), 1300);
      menu->insertItem(QString(TGo4FitMatrixTrans::Class()->GetName()), 1301);
   }

   return (menu->count()>0);
}

void TGo4FitPanel::ExecutePopupForSlot(QFitItem* item, TGo4FitSlot* slot, int id)
{
   if (item!=0) slot = dynamic_cast<TGo4FitSlot*> (item->Object());

   TGo4Fitter* fitter = GetFitter();
   if ((slot==0) || (fitter==0)) return;

   switch (id) {
     case 1000: {
        TObject* newobj = slot->CloneObject();
        slot->SetObject(newobj, kTRUE);
        break;
     }

     case 1001: {

       if(fbNeedConfirmation)
         if ( QMessageBox::information(0,
            QString("Clear slot"),
            QString("Are you sure to clear object from slot ") + slot->GetName(),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

       fitter->ClearSlot(slot, kFALSE);

       if (item)
         for(Int_t i=0;i<fitter->NumSlots();i++) {
            QFitItem* sitem = FindItem(fitter->GetSlot(i), FitGui::ot_slot, 0);
            if (sitem) UpdateItem(sitem, TRUE);
         }

       break; }

     case 1002: {
       UpdateObjectReferenceInSlot(slot, false);
       break;
     }

     case 1004: {
       slot->ClearConnectionToSlot();
       break;
     }

     default: {
       TObject* obj = 0;
       if ((id>=1100) && (id<1200)) obj = CreateData(id-1100,"Data"); else
       if ((id>=1200) && (id<1300)) obj = CreateModel(id-1200, "Model", 0, 0); else
       if (id==1300) obj = new TGo4FitLinearTrans("Trans","Linear axis transformation"); else
       if (id==1301) obj = new TGo4FitMatrixTrans("Trans","Matrix axis transformation"); else
       if (id>=1400) {
         TGo4FitSlot* sl = fitter->GetSlot(id-1400);
         slot->ConnectToSlot(sl);
         break;
       }
       if (obj) slot->SetObject(obj, kTRUE);
     }
   }

   if (item) {
     ShowItem(item, false);
     UpdateItem(item, TRUE);
     item->setOpen(TRUE);
   } else
     UpdateActivePage();
}

void TGo4FitPanel::UpdateItem(QFitItem* item, bool trace)
{
  if (item==0) return;

  if (fxCurrentItem!=0)
      if (fxCurrentItem->FindInParents(item)) RemoveItemWidget();

   while (item->firstChild())
      delete item->firstChild();

  SetItemText(item, FALSE);

  switch (item->ObjectType()) {
    case FitGui::ot_empty: break;
    case FitGui::ot_datalist: {
      TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
      if(fitter)
        for(Int_t n=fitter->GetNumData()-1;n>=0;n--)
          new QFitItem(this, item, fitter->GetData(n), FitGui::ot_data, FitGui::wt_data, FitGui::mt_data, FitGui::gt_data, n);
      break; }
    case FitGui::ot_data: {
      TGo4FitData* data = dynamic_cast<TGo4FitData*> (item->Object());
      if (data==0) break;

      FillSlotsList(item, data->GetSlotList(), data);

      new QFitItem(this, item, data, FitGui::ot_parslist, FitGui::wt_none, FitGui::mt_parslist);

      new QFitItem(this, item, data, FitGui::ot_rangelist, FitGui::wt_none, FitGui::mt_rangelist, FitGui::gt_ranges);

      break; }
    case FitGui::ot_modellist: {
      TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
      if(fitter)
        for(Int_t n=fitter->GetNumModel()-1;n>=0;n--)
           new QFitItem(this, item, fitter->GetModel(n), FitGui::ot_model, DefineModelWidgetType(fitter->GetModel(n)), FitGui::mt_model, FitGui::gt_model, n);
      break; }
    case FitGui::ot_model: {
      TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
      if (model==0) break;

      FillSlotsList(item, model->GetSlotList(), model);

      new QFitItem(this, item, model, FitGui::ot_parslist, FitGui::wt_none, FitGui::mt_parslist);

      new QFitItem(this, item, model, FitGui::ot_rangelist, FitGui::wt_none, FitGui::mt_rangelist, FitGui::gt_ranges);

      new QFitItem(this, item, model, FitGui::ot_asslist, FitGui::wt_none, FitGui::mt_asslist, FitGui::gt_none);

      break; }
    case FitGui::ot_parslist: FillParsList(item); break;
    case FitGui::ot_par: break;
    case FitGui::ot_parcfg: break;
    case FitGui::ot_asslist: {
      TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
      if (model)
        for (Int_t n = model->NumAssigments()-1; n>=0; n--)
          new QFitItem(this, item, model, FitGui::ot_ass, FitGui::wt_none, FitGui::mt_ass, FitGui::gt_ass, n);
      break; }
    case FitGui::ot_ass: break;
    case FitGui::ot_rangelist: {
       TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (item->Object());
       if (comp) {
         for (Int_t n = comp->GetNumRangeCut()-1; n>=0; n--)
           new QFitItem(this, item, comp->GetRangeCut(n), FitGui::ot_rangecut, FitGui::wt_rangecut, FitGui::mt_rangecut, FitGui::gt_none, n);
         for (Int_t n = comp->GetNumRangeCondition()-1; n>=0; n--)
           new QFitItem(this, item, comp, FitGui::ot_range, FitGui::wt_range, FitGui::mt_range, FitGui::gt_range, n);
       }
       break; }
    case FitGui::ot_range: break;
    case FitGui::ot_rangecut: break;
    case FitGui::ot_actlist: {
       TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
       if (fitter==0) break;
       for (Int_t n = fitter->GetNumActions()-1; n>=0; n--) {
          TGo4FitterAction* action = fitter->GetAction(n);
          int ot = FitGui::ot_action, wt = FitGui::wt_named;
          if (action->InheritsFrom(TGo4FitterConfig::Class())) ot = FitGui::ot_config; else
          if (action->InheritsFrom(TGo4FitAmplEstimation::Class())) { ot = FitGui::ot_amplest; wt = FitGui::wt_amplest; } else
          if (action->InheritsFrom(TGo4FitPeakFinder::Class())) { ot = FitGui::ot_peakfinder; wt = FitGui::wt_peakfinder; } else
          if (action->InheritsFrom(TGo4FitterOutput::Class())) { ot = FitGui::ot_output; wt = FitGui::wt_output; } else
          if (action->InheritsFrom(TGo4FitMinuit::Class())) { ot = FitGui::ot_minuit; wt = FitGui::wt_minuit; }

          new QFitItem(this, item, action, ot, wt, FitGui::mt_action);
       }
       break;
    }
    case FitGui::ot_action: break;
    case FitGui::ot_amplest: break;
    case FitGui::ot_peakfinder: break;
    case FitGui::ot_output: break;
    case FitGui::ot_config: {
      TGo4FitterConfig* cfg = dynamic_cast<TGo4FitterConfig*> (item->Object());
      if (cfg==0) break;
      new QFitItem(this, item, &(cfg->GetResults()), FitGui::ot_reslist, FitGui::wt_none, FitGui::mt_deplist);
      new QFitItem(this, item, &(cfg->GetParsDepend()), FitGui::ot_pardeplist, FitGui::wt_none, FitGui::mt_deplist);
      new QFitItem(this, item, &(cfg->GetParsInit()), FitGui::ot_parinitlist, FitGui::wt_none, FitGui::mt_deplist);
      new QFitItem(this, item, &(cfg->GetParsCfg()), FitGui::ot_parscfg, FitGui::wt_none, FitGui::mt_newpars);
      new QFitItem(this, item, &(cfg->GetParsNew()), FitGui::ot_newpars, FitGui::wt_none, FitGui::mt_newpars);
      break; }
    case FitGui::ot_minuit: {
      TGo4FitMinuit* minuit = dynamic_cast<TGo4FitMinuit*> (item->Object());
      if (minuit==0) break;
      for (Int_t n=minuit->GetNumResults()-1;n>=0;n--)
         new QFitItem(this, item, minuit->GetResult(n), FitGui::ot_minuitres, FitGui::wt_minuitres, FitGui::mt_minuitres);
      break; }
    case FitGui::ot_parinitlist: FillDependencyList(item); break;
    case FitGui::ot_pardeplist: FillDependencyList(item); break;
    case FitGui::ot_reslist: FillDependencyList(item); break;
    case FitGui::ot_depend: break;
    case FitGui::ot_newpars: FillParsList(item); break;
    case FitGui::ot_parscfg: {
       TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
       if (pars==0) return;
       for (Int_t n = pars->NumPars()-1; n>=0;n--)
          new QFitItem(this, item, pars->GetPar(n), FitGui::ot_parcfg, FitGui::wt_parcfg, FitGui::mt_parcfg);
       break; }
    case FitGui::ot_minuitres: break;
    case FitGui::ot_slot: {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (item->Object());
      if(slot==0) break;

      TObject* obj = slot->GetObject();
      if (obj==0) break;

      if (obj->InheritsFrom(TGo4FitData::Class()))
         new QFitItem(this, item, obj, FitGui::ot_data, FitGui::wt_data, FitGui::mt_none);
         else

      if (obj->InheritsFrom(TGo4FitModel::Class()))
         new QFitItem(this, item, obj, FitGui::ot_model, DefineModelWidgetType(obj), FitGui::mt_none);
         else

      if (obj->InheritsFrom(TGo4FitLinearTrans::Class()))
         new QFitItem(this, item, obj, FitGui::ot_lintrans, FitGui::wt_lintrans, FitGui::mt_none);
         else

      if (obj->InheritsFrom(TGo4FitMatrixTrans::Class()))
         new QFitItem(this, item, obj, FitGui::ot_matrtrans, FitGui::wt_matrtrans, FitGui::mt_none);
         else

      if (obj->InheritsFrom(TGo4FitAxisTrans::Class()))
         new QFitItem(this, item, obj, FitGui::ot_trans, FitGui::wt_named, FitGui::mt_none);
         else

      new QFitItem(this, item, obj, FitGui::ot_none, FitGui::wt_none, FitGui::mt_none);

      break; }
    case FitGui::ot_fitter: {
      TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
      if (fitter==0) break;

      new QFitItem(this, item, fitter, FitGui::ot_results, FitGui::wt_table);
      new QFitItem(this, item, fitter, FitGui::ot_actlist, FitGui::wt_none, FitGui::mt_actlist);
      new QFitItem(this, item, fitter, FitGui::ot_allslots, FitGui::wt_none, FitGui::mt_allslots);
      new QFitItem(this, item, fitter, FitGui::ot_modellist, FitGui::wt_none, FitGui::mt_modellist);
      new QFitItem(this, item, fitter, FitGui::ot_datalist, FitGui::wt_none, FitGui::mt_datalist);
      break; }
    case FitGui::ot_trans: FillParsList(item); break;
    case FitGui::ot_lintrans: FillParsList(item); break;
    case FitGui::ot_matrtrans: FillParsList(item); break;
    case FitGui::ot_results: break;
    case FitGui::ot_allslots: {
      TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (item->Object());
      if (fitter)
        FillSlotsList(item, fitter->GetSlotList(), 0);
      break; }
  }

  if (trace && (item->Object()!=0)) {
    QFitItem* topitem = GetFitterItem();
    if (topitem==0) return;

    QListViewItemIterator iter(topitem);
    while (iter.current()!=0) {
      QFitItem* it = dynamic_cast<QFitItem*> (iter.current());
      if ((it!=0) && (it!=item) &&
          (item->ObjectType()==it->ObjectType()) &&
          (item->Object()==it->Object())) UpdateItem(it, FALSE);
      ++iter;
   }
  }
}

void TGo4FitPanel::SetItemText(QFitItem* item, bool trace)
{
  if (item==0) return;
  QString text;
  if (item->Object()) text = item->Object()->GetName();
  switch (item->ObjectType()) {
    case FitGui::ot_empty: text = "empty"; break;
    case FitGui::ot_datalist: text = "Data"; break;
    case FitGui::ot_data: break;
    case FitGui::ot_modellist: text = "Models"; break;
    case FitGui::ot_model: break;
    case FitGui::ot_parslist: text = "Parameters"; break;
    case FitGui::ot_par:
    case FitGui::ot_parcfg: {
      TGo4FitParameter* par = dynamic_cast<TGo4FitParameter*> (item->Object());
      if (par) text += " = " + QString::number(par->GetValue());
      break; }
    case FitGui::ot_asslist: text = "Assigments"; break;
    case FitGui::ot_ass: {
       TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (item->Object());
       if (model) text = model->AssignmentName(item->Tag());
       break; }
    case FitGui::ot_rangelist: text = "Ranges"; break;
    case FitGui::ot_range: {
       TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (item->Object());
       if ((comp==0) || (item->Tag()<0) || (item->Tag()>=comp->GetNumRangeCondition())) break;

       Int_t typ, naxis;
       Double_t left, right;
       comp->GetRangeCondition(item->Tag(), typ, naxis, left, right);
       text = QString::number(naxis) + ": ";

       switch(typ) {
         case 0: text+="[" + QString::number(left) + "..." + QString::number(right) + "]"; break;
         case 1: text+="... " + QString::number(left) + "]   [" + QString::number(right) + " ...";  break;
         case 2: text+="[" + QString::number(left) + " ..."; break;
         case 3: text+="... " + QString::number(right) + "]"; break;
       }
       break; }
    case FitGui::ot_rangecut: break;
    case FitGui::ot_actlist: text = "Actions"; break;
    case FitGui::ot_action: break;
    case FitGui::ot_amplest: break;
    case FitGui::ot_peakfinder: break;
    case FitGui::ot_output: break;
    case FitGui::ot_config: break;
    case FitGui::ot_minuit: break;
    case FitGui::ot_parinitlist: text = "Initialization"; break;
    case FitGui::ot_pardeplist: text = "Dependency"; break;
    case FitGui::ot_reslist: text = "Results"; break;
    case FitGui::ot_depend: {
       TGo4FitDependency* depen = dynamic_cast<TGo4FitDependency*> (item->Object());
       if (depen==0) break;
       if (depen->IsResultDepend()) text = "Res" + QString::number(item->Tag());
                               else text = depen->GetParameter().Data();
       text += " = ";
       if (depen->IsInitValue()) text += QString::number(depen->GetInitValue());
                            else text += depen->GetExpression().Data();
       break; }
    case FitGui::ot_newpars: text = "New pars"; break;
    case FitGui::ot_parscfg: text = "Pars setup"; break;
    case FitGui::ot_minuitres: break;
    case FitGui::ot_slot: break;
    case FitGui::ot_fitter: break;
    case FitGui::ot_trans: break;
    case FitGui::ot_lintrans: break;
    case FitGui::ot_matrtrans: break;
    case FitGui::ot_results: text = "Results"; break;
    case FitGui::ot_allslots: text = "Objects"; break;
  }

  item->setText(0, text);

  if (trace && (item->Object()!=0)) {
    QFitItem* topitem = GetFitterItem();
    if (topitem==0) return;

    QListViewItemIterator iter(topitem);
    while (iter.current()!=0) {
      QFitItem* it = dynamic_cast<QFitItem*> (iter.current());
      if ((it!=0) && (it!=item) &&
          (item->ObjectType()==it->ObjectType()) &&
          (item->Object()==it->Object()) && (item->Tag()==it->Tag()))
            it->setText(0, text);
      ++iter;
   }
  }
}

void TGo4FitPanel::UpdateItemsOfType(int typ, QFitItem* parent)
{
   if (parent==0) parent = GetFitterItem();

   QListViewItemIterator iter(parent);
   while (iter.current()!=0) {
      QFitItem* item = dynamic_cast<QFitItem*> (iter.current());
      if ((item!=0) && (item->ObjectType()==typ)) UpdateItem(item, FALSE);
      ++iter;
   }
}

QFitItem* TGo4FitPanel::GetFitterItem()
{
  QFitItem* item = dynamic_cast<QFitItem*> (FitList->firstChild());
  if (item && (item->ObjectType()==FitGui::ot_fitter)) return item;
  return 0;
}

QFitItem* TGo4FitPanel::FindItem(TObject* obj, int ObjectType, QFitItem* parent)
{
   if (parent==0) parent = GetFitterItem();
   QListViewItemIterator iter(parent);
   while (iter.current()!=0) {
      QFitItem* item = dynamic_cast<QFitItem*> (iter.current());
      if (item!=0)
        if ((obj==0) || (item->Object()==obj))
          if ((ObjectType==FitGui::ot_none) || (ObjectType==item->ObjectType())) return item;
      ++iter;
   }
   return 0;
}

bool TGo4FitPanel::ShowItem(QFitItem* item, bool force)
{
   if (force) RemoveItemWidget();

   bool gr = FALSE;

   RemovePrimitives();
   if (fbShowPrimitives)
     gr = ShowItemAsGraph(item, force);

   bool txt = ShowItemAsText(item, force);

   return (gr || txt);
}

bool TGo4FitPanel::ShowItemAsText(QFitItem* item, bool force)
{

  QFitItem* widgetitem = item->DefineWidgetItem();

  QFitWidget* oldwidget = dynamic_cast<QFitWidget*> (ListStack->widget(FitGui::ListStackId));
  if ((oldwidget!=0) && (oldwidget->GetItem()==widgetitem)) {
    oldwidget->FillWidget();
    return TRUE;
  }

   RemoveItemWidget();

   QFitWidget* widget = 0;

   if (widgetitem==0) return TRUE;

   QWidget* owner = 0;

   switch(widgetitem->WidgetType()) {
     case FitGui::wt_par: widget = new QFitParWidget(owner,"Parameter"); break;
     case FitGui::wt_range : widget = new QFitRangeWidget(owner,"Range"); break;
     case FitGui::wt_rangecut : widget = new QFitRangeCutWidget(owner,"Range cut"); break;
     case FitGui::wt_data : widget = new QFitDataWidget(owner,"Data object"); break;
     case FitGui::wt_model : widget = new QFitModelWidget(owner,"Model"); break;
     case FitGui::wt_polynom : widget = new QFitModelPolynomWidget(owner,"Polynom"); break;
     case FitGui::wt_gauss1: widget = new QFitModelGauss1Widget(owner,"Gauss1"); break;
     case FitGui::wt_gauss2: widget = new QFitModelGauss2Widget(owner,"Gauss2"); break;
     case FitGui::wt_gaussn: widget = new QFitModelGaussNWidget(owner,"GaussN"); break;
     case FitGui::wt_formula:  widget = new QFitModelFormulaWidget(owner,"Formula"); break;
     case FitGui::wt_function:  widget = new QFitModelFunctionWidget(owner,"Function"); break;
     case FitGui::wt_amplest: widget = new QFitAmplEstimWidget(owner,"Amplitude estimation"); break;
     case FitGui::wt_peakfinder: widget = new QFitPeakFinderWidget(owner,"Peak finder"); break;
     case FitGui::wt_output: widget = new QFitOutputActionWidget(owner,"Fitter output"); break;
     case FitGui::wt_minuit: widget = new QFitMinuitWidget(owner,"Minuit"); break;
     case FitGui::wt_minuitres: widget = new QFitMinuitResWidget(owner,"Minuit result"); break;
     case FitGui::wt_depend: widget = new QFitDependencyWidget(owner,"Dependency"); break;
     case FitGui::wt_parcfg: widget = new QFitParCfgWidget(owner,"Parameter configuration"); break;
     case FitGui::wt_slot: widget = new QFitSlotWidget(owner,"Slot for external object"); break;
     case FitGui::wt_fitter: widget = new QFitterWidget(owner,"Fitter"); break;
     case FitGui::wt_named: widget = new QFitNamedWidget(owner,"Named object"); break;
     case FitGui::wt_table: widget = new QFitTableWidget(owner,"Table"); break;
     case FitGui::wt_lintrans: widget = new QFitLinearTransWidget(owner,"Simple transform"); break;
     case FitGui::wt_matrtrans: widget = new QFitMatrixTransWidget(owner,"Matrix transform"); break;
   }

   if (widget!=0) {
      widget->SetItem(this, widgetitem);
      ListStack->addWidget(widget, FitGui::ListStackId);
      ListStack->raiseWidget(widget);
   }

   return TRUE;
}

bool TGo4FitPanel::ShowItemAsGraph(QFitItem* item, bool force)
{
   if (item==0) return FALSE;

   QFitItem* gritem = item->DefineGraphItem();

   if (gritem==0) return false;

   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return false;

   if (gritem->GraphType()==FitGui::gt_fitter) {
     PaintFitter(fitter, gritem, false);
     return true;
   }

  if (gritem->GraphType()==FitGui::gt_data) {
     TGo4FitData* data = dynamic_cast<TGo4FitData*> (gritem->Object());

     if (data!=0)
       PaintModelsFor(fitter, data, gritem->Parent()->Parent(), false);

     return (data!=0);
   }

   if (gritem->GraphType()==FitGui::gt_model) {
     TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (gritem->Object());
     if (model==0) return FALSE;
     bool draw = FALSE;
     for(Int_t n=0;n<model->NumAssigments();n++) {
        TGo4FitData* data = fitter->FindData(model->AssignmentName(n));
        if (PaintModel(model, FindPadWhereData(data), gritem)) draw = true;
     }
     return draw;
   }

   if (gritem->GraphType()==FitGui::gt_ass) {
     TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (gritem->Parent()->Parent()->Object());
     if (model) {
        TGo4FitData* data =  fitter->FindData(gritem->text(0).latin1());
        if (PaintModel(model, FindPadWhereData(data), gritem->Parent()->Parent()))
           return true;
     }
   }

   if (gritem->GraphType()==FitGui::gt_range) {
      TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (gritem->Parent()->Object());
      if (comp==0) return false;
      int nrange = gritem->Tag();
      if (PaintRange(comp, nrange, FindPadWhereComp(comp), gritem)) return true;
   }

   if (gritem->GraphType()==FitGui::gt_ranges) {
      TGo4FitComponent* comp = dynamic_cast<TGo4FitComponent*> (gritem->Object());
      if (comp==0) return false;

      TPad* pad = FindPadWhereComp(comp);
      if (pad!=0) {
        QListViewItem* child = gritem->firstChild();
        for(int nrange=0;nrange<comp->GetNumRangeCondition();nrange++) {
           if (child==0) return false;
           PaintRange(comp, nrange, pad, (QFitItem*)child);
           child = child->nextSibling();
        }
        return true;
      }
   }

   return false;
}

void TGo4FitPanel::RemoveItemWidget()
{
   fxCurrentItem = 0;
   QWidget* w = ListStack->widget(FitGui::ListStackId);
   if (w) {
     ListStack->removeWidget(w);
     delete w;
   }
}

void TGo4FitPanel::FitItemDeleted(QFitItem* item)
{
  if ((item!=0) && (fxCurrentItem==item))
    RemoveItemWidget();
}

void TGo4FitPanel::FillParsList(QFitItem* item)
{
   if (item==0) return;
   TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Object());
   if (pars==0) return;
   for (Int_t n = pars->NumPars()-1; n>=0;n--)
      new QFitItem(this, item, pars->GetPar(n), FitGui::ot_par, FitGui::wt_par, FitGui::mt_par);
}

void TGo4FitPanel::FillSlotsList(QFitItem* parent, const TObjArray* lst, TObject* owner)
{
   if ((parent==0) || (lst==0)) return;
   for(Int_t n = lst->GetLast();n>=0;n--) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(n));
      if (slot && ((slot->GetOwner()==owner) || (owner==0)))
         new QFitItem(this, parent, slot, FitGui::ot_slot, FitGui::wt_slot, FitGui::mt_slot);
   }
}

void TGo4FitPanel::FillDependencyList(QFitItem* parent)
{
   if (parent==0) return;

   TObjArray* lst = dynamic_cast<TObjArray*> (parent->Object());
   if (lst)
     for(Int_t n=lst->GetLast();n>=0;n--)
       new QFitItem(this, parent, lst->At(n), FitGui::ot_depend, FitGui::wt_depend, FitGui::mt_depend, FitGui::gt_none, n);
}

void TGo4FitPanel::FillModelTypesList(QPopupMenu* menu, int id, bool extend)
{
  if (menu->count()>0) menu->insertSeparator();
  if (extend) {
     menu->insertItem(QString("Add gaussian"), id+20);
     menu->insertItem(QString("Add lorenzian"), id+24);
     menu->insertItem(QString("Add exponent"), id+25);
     menu->insertItem(QString("Add 1-order polynom"), id+21);
     menu->insertItem(QString("Add 3-order polynom"), id+22);
     menu->insertItem(QString("Add 7-order polynom"), id+23);
     menu->insertSeparator();
  }

  menu->insertItem(QString("Make  ") + TGo4FitModelPolynom::Class()->GetName(), id+0);
  menu->insertItem(QString("Make  ") + TGo4FitModelGauss1::Class()->GetName(),  id+1);
  menu->insertItem(QString("Make  ") + TGo4FitModelGauss2::Class()->GetName(),  id+2);
  menu->insertItem(QString("Make  ") + TGo4FitModelGaussN::Class()->GetName(),  id+3);
  menu->insertItem(QString("Make  ") + TGo4FitModelFormula::Class()->GetName(), id+4);
  menu->insertItem(QString("Make  ") + TGo4FitModelFunction::Class()->GetName(),id+5);
  menu->insertItem(QString("Make  ") + TGo4FitModelFromData::Class()->GetName(),id+6);
}

void TGo4FitPanel::FillDataTypesList(QPopupMenu* menu, int id)
{
  if (menu->count()>0) menu->insertSeparator();
  menu->insertItem(QString("Make  ") + TGo4FitDataHistogram::Class()->GetName(), id+0);
  menu->insertItem(QString("Make  ") + TGo4FitDataGraph::Class()->GetName(),     id+1);
  menu->insertItem(QString("Make  ") + TGo4FitDataProfile::Class()->GetName(),   id+2);
  menu->insertItem(QString("Make  ") + TGo4FitDataRidge::Class()->GetName(),     id+3);
}

void TGo4FitPanel::PaintFitter(TGo4Fitter* fitter, QFitItem* item, bool update)
{
   if (fitter)
     for(Int_t nd=0;nd<fitter->GetNumData();nd++)
       PaintModelsFor(fitter, fitter->GetData(nd), item, update);
}

bool TGo4FitPanel::PaintModel(TGo4FitModel* model, TPad* pad, QFitItem* item)
{
   if ((model==0) || (pad==0) || !fbShowPrimitives) return false;

   TH1* histo = ActivePanel()->GetPadHistogram(pad);
   if ((histo==0) || (histo->GetDimension()!=1)) return false;

   Double_t pos, width, ampl;

   if (model->GetPosition(0,pos)) {

     Int_t nbin = histo->GetXaxis()->FindBin(pos);
     ampl = histo->GetBinContent(nbin);

     Double_t limit = pad->GetUymin()*.7 + pad->GetUymax()*.3;
     if (ampl<limit) ampl = limit;

     TGo4FitGuiArrow* arr = new TGo4FitGuiArrow(0.01,">");
     arr->SetLineColor(2);
     arr->SetLineWidth(1);
     arr->SetItem(item, this);

     TGo4FitGuiArrow* warr = 0;
     if (model->GetWidth(0,width)) {
         warr = new TGo4FitGuiArrow(0.01,"<|>");
         warr->SetLineColor(2);
         warr->SetLineWidth(1);
         warr->SetForWidth(model, arr);
         warr->SetItem(item, this);
     }

     arr->SetForPosition(model, warr, ampl);
     arr->Locate();

     ActivePanel()->AddDrawObject(pad, TGo4ViewPanel::kind_FitArrows, "::Model_VArrow", arr, true, 0);

     if (warr) {
       warr->Locate();
       ActivePanel()->AddDrawObject(pad, TGo4ViewPanel::kind_FitArrows, "::Model_HArrow", warr, true, 0);
     }

     ActivePanel()->ShootRepaintTimer();
  }

  return true;
}

bool TGo4FitPanel::PaintModelsFor(TGo4Fitter* fitter, TGo4FitData* data, QFitItem* item, bool update)
{
   if ((fitter==0) || (data==0)) return false;

   if (data->IsAnyDataTransform()) return false;

   for (Int_t nm=0;nm<fitter->GetNumModel();nm++) {
      TGo4FitModel* model = fitter->GetModel(nm);
      if (model->IsAssignTo(data->GetName()))
        PaintModel(model, FindPadWhereData(data), item);
   }

   return true;
}

bool TGo4FitPanel::PaintRange(TGo4FitComponent* comp, int nrange, TPad* pad, QFitItem* item)
{
   if ((comp==0) || (pad==0) || !fbShowPrimitives) return false;

   TH1* histo = ActivePanel()->GetPadHistogram(pad);

   if ((histo==0) || (histo->GetDimension()!=1)) return false;

   double range = 0.1 * (histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin());

   double ampl = (0.07 + nrange*0.02) * (histo->GetMaximum() - histo->GetMinimum()) + histo->GetMinimum();

   double size = 0.015;

   Int_t typ, naxis;
   Double_t left, right;

   comp->GetRangeCondition(nrange, typ, naxis, left, right);

   if (naxis!=0) return false;

   TGo4FitGuiArrow* arr = 0;

   switch (typ) {
     case 0: arr = new TGo4FitGuiArrow(size,"<|>"); break;
     case 1: arr = new TGo4FitGuiArrow(size,"-><-"); break;
     case 2: arr = new TGo4FitGuiArrow(size,"<"); break;
     case 3: arr = new TGo4FitGuiArrow(size,">"); break;
   }

   if (arr!=0) {
       arr->SetLineColor(3);
       arr->SetLineWidth(3);
       arr->SetForRange(comp,nrange,ampl,range);
       arr->SetItem(item, this);
       arr->Locate();

       ActivePanel()->AddDrawObject(pad, TGo4ViewPanel::kind_FitArrows, "::Range_Arrow", arr, true, 0);

       ActivePanel()->ShootRepaintTimer();
   }
   return arr!=0;
}

TGo4FitPeakFinder* TGo4FitPanel::GetPeakFinder(bool autocreate)
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return 0;
   for (Int_t n=0;n<fitter->GetNumActions();n++) {
      TGo4FitPeakFinder* finder = dynamic_cast<TGo4FitPeakFinder*> (fitter->GetAction(n));
      if (finder!=0) return finder;
   }

   if (autocreate) {
      TGo4FitPeakFinder* finder = new TGo4FitPeakFinder("Finder", 0, kTRUE, 1);
      finder->SetPeakFinderType(2);
      fitter->AddActionAt(finder, 0);
      return finder;
   }

   return 0;
}

TGo4FitData* TGo4FitPanel::Wiz_SelectedData()
{
   TGo4Fitter* fitter = GetFitter();
   return (fitter==0) ? 0 : fitter->FindData(fxWizDataName.latin1());
}

TGo4FitModel* TGo4FitPanel::Wiz_SelectedModel()
{
   TGo4Fitter* fitter = GetFitter();
   return (fitter==0) ? 0 : fitter->FindModel(fxWizModelName.latin1());
}

void TGo4FitPanel::Wiz_PrepareModelTypeList(QPopupMenu* menu)
{
  if (menu) FillModelTypesList(menu,0, TRUE);
}

TGo4FitModel* TGo4FitPanel::Wiz_CreateNewModel(int id)
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return 0;

  TGo4FitData* data = Wiz_SelectedData();
  TGo4FitModel* model = CreateModel(id, 0, fitter, data);

  LocateModel(model, data, true);

  return model;
}

void TGo4FitPanel::Wiz_PrepareDataTypeList(QPopupMenu* menu)
{
  if (menu) FillDataTypesList(menu,0);
}

TGo4FitData* TGo4FitPanel::Wiz_CreateNewData(int id)
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return 0;

   TGo4FitData* data = CreateData(id, 0);
   if (data==0) return 0;

   fitter->AddData(data);

   UpdateObjectReferenceInSlots();

   return data;
}

void TGo4FitPanel::Wiz_RebuildDataList()
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return;
   if ((fitter->GetNumData()>0) && fbNeedConfirmation)
     if ( QMessageBox::information(0,
       QString("Remake data list"),
       QString("Remove all current data?"),
       QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return;

   fitter->DeleteAllData();
   CreateDataFor(ActivePanel(), ActivePad(), fitter);

   UpdateObjectReferenceInSlots();
   UpdateActivePage();
}

bool TGo4FitPanel::Wiz_RemoveData()
{
  TGo4Fitter* fitter = GetFitter();
  TGo4FitData* data = Wiz_SelectedData();
  if ((data==0) || (fitter==0)) return FALSE;

  if(fbNeedConfirmation)
   if ( QMessageBox::information(0,
      QString("Delete data"),
      QString("Are you sure to delete ") + data->GetName(),
      QMessageBox::Yes, QMessageBox::No) == QMessageBox::No ) return FALSE;

  fitter->RemoveData(data->GetName(), kTRUE);

  fxWizDataName = "";

  UpdateObjectReferenceInSlots();

  return TRUE;
}

void TGo4FitPanel::Wiz_UseSelectedRange()
{
  if (!WorkingWithPanel()) return;

  TGo4FitData* data = Wiz_SelectedData();
  TPad* pad = FindPadWhereData(data);
  if ((pad==0) || (data==0)) return;

  double xmin, xmax;
  if (ActivePanel()->GetVisibleRange(pad, 0, xmin, xmax))
     data->SetRange(0, xmin, xmax);

  UpdateActivePage();
}

void TGo4FitPanel::Wiz_TakeCurrentRange()
{
  TGo4Fitter* fitter = GetFitter();
  if ((fitter==0) || (ActivePanel()==0)) return;

  for (int n=0;n<fitter->GetNumData();n++) {
     TGo4FitData* data = fitter->GetData(n);
     TPad* pad = FindPadWhereData(data);

     if ((pad!=0) && (data!=0)) {
       data->ClearRanges();
       double xmin, xmax;
       if (ActivePanel()->GetVisibleRange(pad, 0, xmin, xmax))
         data->SetRange(0, xmin, xmax);
     }
  }
}

void TGo4FitPanel::Wiz_GetModelInfo(TGo4FitModel* model, QString* info)
{
  if ((model==0) || (info==0)) return;
  *info = "Model: ";
  *info += model->GetName();
  *info += "  of class: ";
  *info += model->ClassName();

  if (model->InheritsFrom(TGo4FitModelFormula::Class())) {
     TGo4FitModelFormula* formula = dynamic_cast<TGo4FitModelFormula*> (model);
     if (formula) {
        *info += "\nFormula: ";
        *info += formula->GetExpression();
     }
  }

  if (model->InheritsFrom(TGo4FitModelFunction::Class())) {
     TGo4FitModelFunction* func = dynamic_cast<TGo4FitModelFunction*> (model);
     if (func) {
       *info += "\nFunction: ";
       *info += func->GetFunctionName();
       *info += "  from: ";
       *info += func->GetLibraryName();
     }
  }
  if (fiIntegralMode>0) {
     int mode = fiIntegralMode;

     if ((mode==1) || (mode==2)) {
        TGo4Fitter* fitter = GetFitter();
        TGo4FitData* data = Wiz_SelectedData();
        if (data)
          if (!model->IsAssignTo(data->GetName())) data = 0;
        if ((data!=0) && (fitter!=0)) {
           if (mode==1) *info += "\nCounts="; else *info += "\nIntegral=";
           double v = fitter->CalculatesIntegral(data->GetName(),model->GetName(),(mode==1));
           *info += QString::number(v);
        }
     }

     if (mode==3) {
        double v = model->Integral();
        if (v>0) {
           *info += "\nGauss integral=";
           *info += QString::number(v);
        }
     }
  }
}


QString TGo4FitPanel::Wiz_GetSlotSourceInfo(TGo4FitSlot* slot)
{
  QString res = "";
  if (slot==0) return res;

  if (slot->GetObject()==0) {
     res = "none";
  } else
  if (slot->GetOwned()) {
     res = "internal object";
  } else
  if (WorkingWithPanel()) {
     TPad* pad = FindPadForSlot(slot);
     if (pad==0)
       res = "reference without pad";
     else {
       res = "Panel: ";
       res+= ActivePanel()->name();
       if (ActivePanel()->GetCanvas() != pad) {
          res += "  pad: ";
          res += pad->GetName();
       }
     }
  } else {
      int slotindex = GetPadIndexForSlot(slot);
      QString linkname;
      linkname.sprintf("FitSlotLink_%d", slotindex);
      const char* itemname = GetLinkedName(linkname);

      if (itemname!=0) res = itemname;
                  else res = "reference to external object";
  }

  return res;
}

void TGo4FitPanel::ArrowChanged(TGo4FitGuiArrow* arr)
{
   TGo4Fitter* fitter = GetFitter();

   if ((arr==0) || (fitter==0)) return;

   bool ismodified = false;
   for(Int_t n=0;n<fitter->NumSlots();n++) {
      TPad* pad = FindPadForSlot(fitter->GetSlot(n));
      if (pad==0) continue;

      TObjArray lst;
      ActivePanel()->CollectSpecialObjects(pad, &lst, TGo4ViewPanel::kind_FitArrows);

      bool res = false;
      for (Int_t k=0;k<=lst.GetLast();k++) {
        TGo4FitGuiArrow* a = dynamic_cast<TGo4FitGuiArrow*> (lst[k]);
        if (a==0) continue;
        if (a->Locate()) res = true;
      }
      if (res) {
        ActivePanel()->MarkPadModified(pad);
        ismodified = true;
      }
    }
   if (ismodified)
     ActivePanel()->ShootRepaintTimer(ActivePad());

   if (fiPanelMode==FitGui::pm_Expert) {
        if ((arr->GetType()==TGo4FitGuiArrow::at_pos) ||
           (arr->GetType()==TGo4FitGuiArrow::at_width)) UpdateItemsOfType(FitGui::ot_par, arr->GetItem());

        if (arr->GetType()==TGo4FitGuiArrow::at_range)
          SetItemText(arr->GetItem(), TRUE);

        QFitWidget* current = dynamic_cast<QFitWidget*> (ListStack->widget(FitGui::ListStackId));
        if(current) current->FillWidget();
   }

   if (fiPanelMode==FitGui::pm_Wizard)
     if (fbParsWidgetShown)
       FillParsWidget();
     else
       UpdateWizStackWidget();
}

void TGo4FitPanel::DeleteModelWithPrimit(TGo4FitGuiArrow* arr)
{
   if (arr==0) return;
   TGo4FitModel* model = arr->GetModel();
   if (model==0) return;

   if (fiPanelMode==FitGui::pm_Expert) {
     QFitItem* item = FindItem(model, FitGui::ot_model, 0);
     if (item) Cmd_DeleteModel(item);
   } else
     Cmd_RemoveModel(model);
}

int TGo4FitPanel::GetPadIndexForSlot(TGo4FitSlot* slot)
{
   TGo4Fitter* fitter = GetFitter();
   if ((slot==0) || (fitter==0)) return -1;

   int indx = -1;
   for (int n=0;n<fitter->NumSlots();n++) {
      TGo4FitSlot* subslot = fitter->GetSlot(n);
      bool needpad = !subslot->GetOwned() &&
         (subslot->GetClass()->InheritsFrom(TH1::Class()) ||
           subslot->GetClass()->InheritsFrom(TGraph::Class()));
      if (needpad) indx++;
      if (slot==subslot)
        return needpad ? indx : -1;
   }
   return -1;
}

TPad* TGo4FitPanel::FindPadForSlot(TGo4FitSlot* slot)
{
   if (!WorkingWithPanel()) return 0;
   int indx = GetPadIndexForSlot(slot);
   return (indx<0) ? 0 : ActivePanel()->GetSubPad(ActivePad(), indx, true);
}

TPad* TGo4FitPanel::FindPadWhereData(TGo4FitData* data)
{
   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return 0;
   for(Int_t n = 0; n<fitter->NumSlots();n++) {
      TGo4FitSlot* slot = fitter->GetSlot(n);
      if (slot->GetOwner()!=data) continue;
      TPad* pad = FindPadForSlot(slot);
      if (pad!=0) return pad;
   }
   return 0;
}

TPad* TGo4FitPanel::FindPadWhereModel(TGo4FitModel* model)
{
   TGo4Fitter* fitter = GetFitter();
   if ((fitter==0) || (model==0)) return 0;

   for(Int_t n=0;n<model->NumAssigments();n++) {
      TGo4FitData* data = fitter->FindData(model->AssignmentName(n));
      TPad* pad = FindPadWhereData(data);
      if (pad!=0) return pad;
   }
   return 0;
}

TPad* TGo4FitPanel::FindPadWhereComp(TGo4FitComponent* comp)
{
   if (comp==0) return 0;
   if (comp->InheritsFrom(TGo4FitData::Class()))
      return FindPadWhereData(dynamic_cast<TGo4FitData*> (comp));
   else
      return FindPadWhereModel(dynamic_cast<TGo4FitModel*> (comp));
}

TGo4FitData* TGo4FitPanel::CreateData(int id, const char* name)
{
    TString dataname;

    if (name==0) {
      TGo4Fitter* fitter = GetFitter();
      if (fitter==0) return 0;
      dataname = fitter->FindNextName("Data", 0, kFALSE);
    } else dataname = name;

   switch(id) {
      case 0: return new TGo4FitDataHistogram(dataname);
      case 1: return new TGo4FitDataGraph(dataname);
      case 2: return new TGo4FitDataProfile(dataname);
      case 3: return new TGo4FitDataRidge(dataname);
   }
   return 0;
}

TGo4FitModel* TGo4FitPanel::CreateModel(int id, const char* namebase, TGo4Fitter* fitter, TGo4FitData* data)
{
    TString NameBase;

    if (fitter)
      if ((data==0) && (fitter->GetNumData()>0)) data = fitter->GetData(0);
    Int_t NumDim = 0;
    if (data) NumDim = data->DefineDimensions();
    if (NumDim<=0) NumDim = 1;

    Int_t order = 0;
    switch(id) {
      case 20:  // gaussian
        switch (NumDim) {
           case 1: id = 1; break;
           case 2: id = 2; break;
           default: id = 3; break;
        }
        break;
      case 21: order = 1; id = 7; break;  // polynom 1
      case 22: order = 3; id = 7; break;  // polynom 3
      case 23: order = 7; id = 7; break;  // polynom 7
    }

    if (namebase) NameBase = namebase; else
      switch(id) {
        case 0: NameBase = "Pol"; break;
        case 1:
        case 2:
        case 3: NameBase = "Gauss"; break;
        case 4: NameBase = "Form"; break;
        case 5: NameBase = "Func"; break;
        case 6: NameBase = "FromData"; break;
        case 7: NameBase = "Pol"; break;
        case 24: NameBase = "Lor"; break;
        case 25: NameBase = "Exp"; break;
        default: NameBase = "Model";
      }

    TString ModelName;

    if (fitter && (id!=7)) ModelName = fitter->FindNextName(NameBase.Data(), 0);
                      else ModelName = NameBase;

   TGo4FitModel* model = 0;

   switch(id) {
      case 0: model = new TGo4FitModelPolynom(ModelName); break;
      case 1: model = new TGo4FitModelGauss1(ModelName); break;
      case 2: model = new TGo4FitModelGauss2(ModelName); break;
      case 3: model = new TGo4FitModelGaussN(ModelName, NumDim<3 ? 3 : NumDim); break;
      case 4: model = new TGo4FitModelFormula(ModelName); break;
      case 5: model = new TGo4FitModelFunction(ModelName); break;
      case 6: model = new TGo4FitModelFromData(ModelName); break;
      case 7: fitter->AddPolynoms(data ? data->GetName() : 0, NameBase.Data(), order, NumDim); break;
      case 24: {
         TGo4FitModelFormula* formula = new TGo4FitModelFormula(ModelName,"1./(1.+sq((x-Pos)/Width))",2,kTRUE);
         formula->SetParsNames("Ampl","Pos","Width");
         formula->SetPosParIndex(0, 0);
         formula->SetWidthParIndex(0, 1);
         model = formula;
         break;
      }
     case 25: {
         model = new TGo4FitModelFormula(ModelName,"exp(Koef*x)",1,kTRUE);
         model->SetParsNames("Ampl","Koef");
         break;
      }
   }

   if (fitter && model) {
      if (data) model->AssignToData(data->GetName());
      fitter->AddModel(model);
   }

  return model;
}

void TGo4FitPanel::LocateModel(TGo4FitModel* model, TGo4FitData* data, bool usepad)
{
   TGo4Fitter* fitter = GetFitter();

   if ((fitter==0) || (model==0)) return;

   if (data==0)
     for(int n=0;n<=model->NumAssigments();n++) {
        data = fitter->FindData(model->AssignmentName(n));
        if (!usepad || FindPadWhereData(data)) break;
     }
   if (data==0) return;

   if (!usepad && data->DefineDimensions()!=1) usepad = TRUE;

   if (usepad) {
       TPad* pad = FindPadWhereData(data);
       if (pad) {
          model->SetPosition(0, (pad->GetUxmin() + pad->GetUxmax())/2.);
          model->SetWidth(0, TMath::Abs(pad->GetUxmax() - pad->GetUxmin())*.05);
          model->SetPosition(1, (pad->GetUymin() + pad->GetUymax())/2.);
          model->SetWidth(1, TMath::Abs(pad->GetUymax() - pad->GetUymin())*.05);
       }
   } else {
     Double_t mid, width;
     if (fitter->CalculatesMomentums(data->GetName(), kTRUE, kTRUE, mid, width)) {
        model->SetPosition(0, mid);
        model->SetWidth(0, width);
     }
   }
}

int TGo4FitPanel::DefineModelWidgetType(TObject* obj)
{
   if (obj==0) return FitGui::wt_none;
   if (obj->InheritsFrom(TGo4FitModelPolynom::Class())) return FitGui::wt_polynom;
   if (obj->InheritsFrom(TGo4FitModelGauss1::Class())) return FitGui::wt_gauss1;
   if (obj->InheritsFrom(TGo4FitModelGauss2::Class())) return FitGui::wt_gauss2;
   if (obj->InheritsFrom(TGo4FitModelGaussN::Class())) return FitGui::wt_gaussn;
   if (obj->InheritsFrom(TGo4FitModelFormula::Class())) return FitGui::wt_formula;
   if (obj->InheritsFrom(TGo4FitModelFunction::Class())) return FitGui::wt_function;
   if (obj->InheritsFrom(TGo4FitModel::Class())) return FitGui::wt_model;
   return FitGui::wt_none;
}

void TGo4FitPanel::CreateFitSlotLink(TGo4FitSlot* slot, const char * itemname)
{
   cout << "****** CreateFitSlotLink " << slot->GetName() << "  " << itemname << endl;

   if ((slot==0) || (itemname==0)) return;

   int slotindex = GetPadIndexForSlot(slot);

   cout << "Slot index = " << slotindex << endl;

   if (slotindex<0) return;

   QString linkname;
   linkname.sprintf("FitSlotLink_%d", slotindex);

   RemoveLink(linkname);

   AddLink(itemname, linkname);

   GetLinked(linkname, 1);

   UpdateObjectReferenceInSlot(slot, false);

//   GetLinked(linkname, 0);
}


bool TGo4FitPanel::UpdateObjectReferenceInSlot(TGo4FitSlot* slot, bool createlink)
{
   if (slot==0) return true;

   if (slot->GetOwned()) return true;

   bool res = true;

   TObject* obj = 0;

   if (WorkingWithPanel()) {

      TPad* pad = FindPadForSlot(slot);

      TGo4Slot* objslot = ActivePanel()->GetPadMainObjectSlot(pad);
      obj = objslot==0 ? 0 : objslot->GetAssignedObject();

      if ((obj!=0) && createlink) {
         TString linkname = "data_";
         linkname+=slot->GetName();
         AddLink(objslot, linkname);
      }
   } else {
      int slotindex = GetPadIndexForSlot(slot);
      QString linkname;
      linkname.sprintf("FitSlotLink_%d", slotindex);
      obj = GetLinked(linkname, 0);
   }

   if (obj==0) res = false;
   if ((obj==0) || !slot->IsSuitable(obj))
      slot->SetObject(0, kFALSE);
   else
      slot->SetObject(obj, kFALSE);

   return res;
}


bool TGo4FitPanel::UpdateObjectReferenceInSlots()
{
  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return false;

  bool res = true;

  // remove all links, responsible to data connection,
  // which are started with "data_"
//  cout << "UpdateObjectReferenceInSlots(). Remove all links " << endl;
  if (WorkingWithPanel())
      RemoveLinksMasked("data_");

  // here new links will be create, which are connect widget with datasources
  for(Int_t n = 0; n<fitter->NumSlots();n++) {
     TGo4FitSlot* slot = fitter->GetSlot(n);
     if (slot->GetClass()->InheritsFrom(TH1::Class()) ||
         slot->GetClass()->InheritsFrom(TGraph::Class()))
        res = res && UpdateObjectReferenceInSlot(slot, true);
  }

//  cout << "UpdateObjectReferenceInSlots() done res = " << res << endl;

  return res;
}

void TGo4FitPanel::ClearObjectReferenceInSlots()
{
  if (!WorkingWithPanel()) return;

  TGo4Fitter* fitter = GetFitter();
  if (fitter==0) return;

  for(Int_t n = 0; n<fitter->NumSlots();n++) {
     TGo4FitSlot* slot = fitter->GetSlot(n);

     if (slot->GetClass()->InheritsFrom(TH1::Class()) ||
         slot->GetClass()->InheritsFrom(TGraph::Class()))
         if (!slot->GetOwned())
             slot->SetObject(0, kFALSE);
  }
}

void TGo4FitPanel::RemoveDrawObjects()
{
   if (!WorkingWithPanel()) return;

   TGo4Fitter* fitter = GetFitter();
   if (fitter==0) return;

   bool res = false;

   for(Int_t n = 0; n<fitter->NumSlots();n++) {
      TPad* pad = FindPadForSlot(fitter->GetSlot(n));
      if (pad==0) continue;
      if (ActivePanel()->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitModels)) res = true;
      if (ActivePanel()->DeleteDrawObjects(pad, TGo4ViewPanel::kind_FitInfo)) res = true;
   }
   if (res) ActivePanel()->ShootRepaintTimer();
}

void TGo4FitPanel::CloseDrawPanel()
{
   if (fxDrawNewPanel!=0) {
      fxDrawNewPanel->close();
      fxDrawNewPanel = 0;
   }
}

void TGo4FitPanel::FillNamedWidget(QFitNamedWidget* w)
{
   if (w==0) return;
   TObject* obj = w->GetObject();
   if (obj==0) return;

   w->ClassNameLbl->setText(QString("Class name: ") + obj->ClassName());
   w->ClassNameLbl->adjustSize();
   w->NameEdt->setText(obj->GetName());
   w->TitleEdt->setText(obj->GetTitle());
   TNamed* n = dynamic_cast<TNamed*> (obj);
   w->NameEdt->setReadOnly(n==0);
   w->TitleEdt->setReadOnly(n==0);

   TGo4FitNamed* go4n = dynamic_cast<TGo4FitNamed*> (obj);
   if((go4n!=0) && go4n->GetOwner())
     w->FullNameLbl->setText(QString("Full name: ")+go4n->GetFullName());
   else
     w->FullNameLbl->setText("");
   w->FullNameLbl->adjustSize();

   QFitModelWidget* mw = dynamic_cast<QFitModelWidget*> (w);
   TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (obj);
   if ((mw!=0) && (model!=0)) {
       mw->AmplChk->setChecked(model->GetAmplPar()!=0);
       mw->AmplChk->setEnabled(model->CanAmplTouch());
       mw->BuffersChk->setChecked(model->GetUseBuffers());
       mw->GroupSpin->setValue(model->GetGroupIndex());
    }
}

void TGo4FitPanel::ChangeObjectName(QFitNamedWidget* w, const char* newname)
{
  if (w==0) return;
  QFitItem* item = w->GetItem();
  if ((item==0) || (strlen(newname)==0)) return;
  TNamed* obj = dynamic_cast<TNamed*> (item->Object());
  if (obj==0) return;

  if (item->ObjectType()==FitGui::ot_slot) return;

  if (item->ObjectType()==FitGui::ot_par)
    if (item->Parent()!=0)
      if (item->Parent()->ObjectType()==FitGui::ot_parslist) {
          TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (item->Parent()->Object());
          if (pars->FindPar(newname)) return;
      }

  if (item->ObjectType()==FitGui::ot_data)
    if (item->Parent()!=0)
      if (item->Parent()->ObjectType()==FitGui::ot_datalist) {
          TGo4Fitter* fitter = GetFitter();
          if ((fitter==0) || (fitter->FindData(newname))) return;
          fitter->ChangeDataNameInAssignments(obj->GetName(), newname);
          UpdateItemsOfType(FitGui::ot_ass, 0);
      }

  if (item->ObjectType()==FitGui::ot_model)
    if (item->Parent()!=0)
      if (item->Parent()->ObjectType()==FitGui::ot_modellist) {
          TGo4Fitter* fitter = GetFitter();
          if ((fitter==0) || (fitter->FindModel(newname))) return;
      }

  obj->SetName(newname);

  w->SetWidgetItemText(true);

  TGo4FitNamed* go4n = dynamic_cast<TGo4FitNamed*> (obj);
  if((go4n!=0) && go4n->GetOwner())
    w->FullNameLbl->setText(QString("Full name: ")+go4n->GetFullName());
  else
    w->FullNameLbl->setText("");
  w->FullNameLbl->adjustSize();
}

void TGo4FitPanel::ChangeObjectTitle(QFitNamedWidget* w, const char* newtitle)
{
   if (w==0) return;
   TNamed* n = dynamic_cast<TNamed*> (w->GetObject());
   if (n!=0) n->SetTitle(newtitle);
}

void TGo4FitPanel::ChangeModelPar(QFitModelWidget* w, int npar, int value)
{
   if (w==0) return;
   TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (w->GetObject());
   if (model==0) return;
   switch (npar) {
      case 0: {
         if (model->CanAmplTouch()) {
            bool res = FALSE;
            if (value!=0) res = model->MakeAmpl();
                    else  res = model->RemoveAmpl();
            if (res)
              w->UpdateItemsOfType(FitGui::ot_parslist, false);
         }
         break;
      }

      case 1: {
         model->SetUseBuffers(value!=0);
         break;
      }

      case 2:{
         model->SetGroupIndex(value);
         break;
      }
   }
}
