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

// JAM2016: activate this for global ROOT "escape mode" before redraw (NOT RECOMMENDED)
// otherwise, escape mode will reset global arrays of TGraph painter class only
//#define GLOBALESCAPE 1

#include "TGo4ViewPanel.h"

#include <math.h>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TVirtualX.h"
#include "RVersion.h"
#include "Riostream.h"
#include "THStack.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFrame.h"
#include "TArrayD.h"
#include "TCutG.h"
#include "TArrow.h"
#include "TList.h"
#include "TLatex.h"
#include "TPaveStats.h"
#include "TPaveLabel.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TMath.h"
#include "TClass.h"
#include "TSystem.h"

#include "TGCanvas.h"
#include "TGTab.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QTimer>
#include <QInputDialog>
#include <QToolTip>
#include <QTime>
#include <QMenu>
#include <QtCore/QSignalMapper>

#include <QTimer>

#include "QRootCanvas.h"
#include "QRootApplication.h"

#include "TGo4Log.h"
#include "TGo4Picture.h"
#include "TGo4Fitter.h"
#include "TGo4Marker.h"
#include "TGo4Condition.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4WinCondView.h"
#include "TGo4PolyCondView.h"
#include "TGo4LockGuard.h"
#include "TGo4MdiArea.h"
#include "TGo4ASImage.h"
#include "TGo4PrintWidget.h"
#include "TGo4Proxy.h"
#include "TGo4Slot.h"
#include "TGo4ObjectProxy.h"
#include "TGo4LinkProxy.h"
#include "TGo4DrawObjProxy.h"
#include "TGo4DrawCloneProxy.h"
#include "TGo4Iter.h"
#include "TGo4ObjectManager.h"
#include "TGo4BrowserProxy.h"
#include "TGo4QSettings.h"

const char* NoStackDrawOption = "nostack, ";

#ifdef GO4_WEBGUI
#include "QWebCanvas.h"
#endif

class TPadLock {
   TVirtualPad *fSave;
public:

   TPadLock(TVirtualPad *repl = nullptr)
   {
      fSave = gPad;
      gPad = repl;
   }

   ~TPadLock()
   {
      gPad = fSave;
   }
};


TGo4ViewPanel::TGo4ViewPanel(QWidget *parent, const char* name) :
   QGo4Widget(parent, name)
{
   setupUi(this);

   // TODO: viewpanel does not closed when press close button
   // setAttribute(Qt::WA_DeleteOnClose, true);

   fPanelName = objectName();

   fxActivePad = 0;

   fiSkipRedrawCounter = 0;
   fxRepaintTimerPad = 0;
   fxResizeTimerPad = 0;
   fxDoubleClickTimerPad = 0;
   fbFreezeTitle = false;
   fFreezedTitle = "";
   fbApplyToAllFlag = false;
   fbAutoZoomFlag = false;
   fbCanvasCrosshair = false;

   fbCloneFlag = true;
   fbModifiedSignalFlag = false;
   fbLeaveFocusAfterCondEnd = false;

   // setup of marker editor
   fbMarkEditorVisible = false;
   fbTypingMode = true;
   MarkerPanel->setVisible(false);
   fiMouseMode = kMouseROOT;
   fiPickCounter = 0;
   fbPickAgain = false;

   setWindowTitle(GetPanelName());

   QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
   sizePolicy3.setHorizontalStretch(0);
   sizePolicy3.setVerticalStretch(20);

   fxQCanvas = 0;
   fxWCanvas = 0;

   CanvasStatus = 0;

#ifdef GO4_WEBGUI
   if (go4sett->getWebBasedCanvas())  {
      fxWCanvas = new QWebCanvas(this);

      fxWCanvas->setMinimumSize(QSize(50, 50));
      sizePolicy3.setHeightForWidth(fxWCanvas->sizePolicy().hasHeightForWidth());
      fxWCanvas->setSizePolicy(sizePolicy3);

      fxGridLayout->addWidget(fxWCanvas, 1, 1, 1, 1);

      fxWCanvas->setObjectName(GetPanelName());
      fxWCanvas->getCanvas()->SetName(GetPanelName());
   }
#endif

   if (!fxWCanvas) {
      fxQCanvas = new QRootCanvas(this);
      // fxQCanvas->setObjectName(QStringLiteral("fxQCanvas"));
      fxQCanvas->setMinimumSize(QSize(50, 50));
      sizePolicy3.setHeightForWidth(fxQCanvas->sizePolicy().hasHeightForWidth());
      fxQCanvas->setSizePolicy(sizePolicy3);

      fxGridLayout->addWidget(fxQCanvas, 1, 1, 1, 1);

      fxQCanvas->setObjectName(GetPanelName());
      fxQCanvas->getCanvas()->SetName(GetPanelName());
      fxQCanvas->setEditorFrame(EditorFrame);
   }

//   printf("Resize x %d y %d\n", go4sett->lastPanelSize().width(), go4sett->lastPanelSize().height());
//   resize(go4sett->lastPanelSize());

   fSelectMenu = 0;
   fSelectMap = 0;

   fMenuBar = new QMenuBar(MenuFrame);
   fMenuBar->setMinimumWidth(50);

#if (QT_VERSION >= 0x040600)
   fMenuBar->setNativeMenuBar(kFALSE); // disable putting this to screen menu. for MAC style WMs
#endif

   QMenu* fileMenu = fMenuBar->addMenu("F&ile");
   fileMenu->addAction("&Save as...", this, SLOT(SaveCanvas()));
   fileMenu->addAction("Print...", this, SLOT(PrintCanvas()));
   fileMenu->addAction("Produce &Picture", this, SLOT(ProducePicture()));
   fileMenu->addAction("Produce &Graph from markers", this,
                       SLOT(ProduceGraphFromMarkers()));

//   fileMenu->addAction("Copy to T&Canvas in Memory", this, SLOT(SendToBrowser()));
//   fileMenu->addAction("&Load marker setup...", this, SLOT(LoadMarkers()));
//   fileMenu->addAction("Save &marker setup...", this, SLOT(SaveMarkers()));
   fileMenu->addAction("Cl&ose", this, SLOT(close()));

   //Edit Menu
   QMenu* editMenu = fMenuBar->addMenu("&Edit");

   AddChkAction(editMenu, "Show marker &editor", fbMarkEditorVisible, this,
         SLOT(SetMarkerPanel()));

   bool ed_visible = false, ed_allowed = true;

   if (!fxWCanvas) {
      ed_visible = fxQCanvas->isEditorVisible();
      ed_allowed = fxQCanvas->isEditorAllowed();
   }

   fxCanvasEditorChk = AddChkAction(editMenu, "Show &ROOT attributes editor",
                         ed_visible, this, SLOT(StartRootEditor()));
   fxCanvasEditorChk->setEnabled(ed_allowed);

   bool status_flag = go4sett->getPadEventStatus();
   fxCanvasEventstatusChk = AddChkAction(editMenu, "Show &event status", status_flag, this,
            SLOT(ShowEventStatus()));

   editMenu->addAction("Start &condition editor", this,
         SLOT(StartConditionEditor()));

   editMenu->addSeparator();
   editMenu->addAction("Clear &markers", this, SLOT(ClearAllMarkers()));
   editMenu->addAction("Clear &pad", this, SLOT(ClearActivePad()));
   editMenu->addAction("Clear c&anvas", this, SLOT(ClearCanvas()));

   fSelectMap = new QSignalMapper(this);
   connect(fSelectMap, SIGNAL(mapped(int)), this,
         SLOT(SelectMenuItemActivated(int)));
   fSelectMenu = fMenuBar->addMenu("&Select");

   fOptionsMap = new QSignalMapper(this);
   fOptionsMenu = fMenuBar->addMenu("&Options");
   connect(fOptionsMenu, SIGNAL(aboutToShow()), this,
         SLOT(AboutToShowOptionsMenu()));
   connect(fOptionsMap, SIGNAL(mapped(int)), this,
         SLOT(OptionsMenuItemActivated(int)));

   AddIdAction(fOptionsMenu, fOptionsMap, "&Crosshair", CrosshairId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Super&impose", SuperimposeId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Auto &zoom", AutoZoomId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Histogram &Statistics", StatisticsId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Multiplot &Legend", SetLegendId);

   fOptionsMenu->addSeparator();
   AddIdAction(fOptionsMenu, fOptionsMap, "Histogram &Title", SetTitleId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Draw Time", DrawTimeId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Draw Date", DrawDateId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Draw item name", DrawItemnameId);
   fOptionsMenu->addSeparator();
   AddIdAction(fOptionsMenu, fOptionsMap, "&1:1 Coordinate ratio", SetXYRatioOneId);
   fOptionsMenu->addSeparator();
   AddIdAction(fOptionsMenu, fOptionsMap, "&X-Axis displays time", AxisTimeDisplayId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Set X-Axis time format...", SetTimeFormatId);
   fOptionsMenu->addSeparator();
   AddIdAction(fOptionsMenu, fOptionsMap, "&Keep Viewpanel Title", FreezeTitleId);
   AddIdAction(fOptionsMenu, fOptionsMap, "Set &Viewpanel Title...", SetTitleTextId);

   QCheckBox* box1 = new QCheckBox("Apply to all", MenuFrame);
   box1->setObjectName("ApplyToAllCheck");
   connect(box1, SIGNAL(toggled(bool)), this, SLOT(ApplyToAllToggled(bool)));

   fAutoScaleCheck = new QCheckBox("AutoScale", MenuFrame);
   fAutoScaleCheck->setObjectName("AutoScaleCheck");
   connect(fAutoScaleCheck, SIGNAL(toggled(bool)), this, SLOT(AutoScaleToggled(bool)));

   QHBoxLayout* menugrid = new QHBoxLayout(0/*MenuFrame*/);
   menugrid->setMargin(0);
   menugrid->setSpacing(0);
   menugrid->addWidget(fMenuBar, 10, Qt::AlignLeft);
   menugrid->addWidget(box1, 1, Qt::AlignRight);
   menugrid->addWidget(fAutoScaleCheck, 1, Qt::AlignRight);

   fxGridLayout->addLayout(menugrid, 0, 0, 1, 2);

   // status widget

   if (fxWCanvas) {
      // TODO: one can get status from webcanvas here
#ifdef GO4_WEBGUI
      fxWCanvas->setStatusBarVisible(status_flag);

      connect(fxWCanvas, SIGNAL(CanvasDropEvent(QDropEvent*,TPad*)), this,
               SLOT(CanvasDropEventSlot(QDropEvent*,TPad*)));

      connect(fxWCanvas, SIGNAL(SelectedPadChanged(TPad*)), this,
              SLOT(SetActivePad(TPad*)));

      connect(fxWCanvas, SIGNAL(PadClicked(TPad*,int,int)), this,
               SLOT(PadClickedSlot(TPad*,int,int)));

      connect(fxWCanvas, SIGNAL(PadDblClicked(TPad*,int,int)), this,
               SLOT(PadDoubleClickedSlot(TPad*,int,int)));

      connect(fxWCanvas, SIGNAL(CanvasUpdated()), this,
              SLOT(CanvasUpdatedSlot()));

#endif
   } else {
      CanvasStatus = new QStatusBar(this);
      fxGridLayout->addWidget(CanvasStatus, 3, 0, 1, 2);
      CanvasStatus->setVisible(false);
      fxQCanvas->setStatusBar(CanvasStatus);
      fxQCanvas->setStatusBarVisible(status_flag);

      connect(fxQCanvas, SIGNAL(CanvasDropEvent(QDropEvent*,TPad*)), this,
              SLOT(CanvasDropEventSlot(QDropEvent*,TPad*)));
      connect(fxQCanvas, SIGNAL(SelectedPadChanged(TPad*)), this,
              SLOT(SetActivePad(TPad*)));
      connect(fxQCanvas, SIGNAL(PadClicked(TPad*)), this,
              SLOT(PadClickedSlot(TPad*)));
      connect(fxQCanvas, SIGNAL(PadDoubleClicked(TPad*)), this,
              SLOT(PadDoubleClickedSlot(TPad*)));
      connect(fxQCanvas, SIGNAL(MenuCommandExecuted(TObject*, const char*)),
              this, SLOT(MenuCommandExecutedSlot(TObject*, const char*)));
      connect(fxQCanvas, SIGNAL(CanvasLeaveEvent()), this,
               SLOT(RefreshButtons()));
      connect(fxQCanvas, SIGNAL(CanvasUpdated()), this,
              SLOT(CanvasUpdatedSlot()));
   }
}

TGo4ViewPanel::~TGo4ViewPanel()
{
   TGo4LockGuard lock;

   // we should delete all markers first, while they
   // have internal reference on the pad, which will be
   // deleted by the net canvas->Clear() call
   ProcessMarkersClear(GetCanvas(), true);

   CleanupGedEditor();

   GetCanvas()->Clear();

   fxRepaintTimerPad = 0;

   CallPanelFunc(panel_Deleted);

   if (gPad != 0)
      if (IsPanelPad((TPad*) gPad))
         gPad = 0;

   if (gROOT->GetSelectedPad() != 0)
      if (IsPanelPad((TPad*) gROOT->GetSelectedPad()))
         gROOT->SetSelectedPad(0);
}

const char* TGo4ViewPanel::GetPanelName()
{
   fbaPanelName = fPanelName.toLatin1();
   return fbaPanelName.constData();
}

void TGo4ViewPanel::SetPanelName(const char* newname)
{
   fPanelName = newname;
}

TGo4Slot* TGo4ViewPanel::GetPanelSlot()
{
   return GetTopSlot(true);
}

TGo4Slot* TGo4ViewPanel::AddNewSlot(const char* name, TGo4Slot* parent)
{
   if (parent == 0)
      parent = GetPanelSlot();
   return new TGo4Slot(parent, name, "title");
}

void TGo4ViewPanel::ResetWidget()
{
   // do nothing
}

void TGo4ViewPanel::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (linkname == 0)
      return;

   if (strcmp(linkname, "PadRangeAxisChanged") == 0) {
      PadRangeAxisChanged(GetActivePad());
   } else if (strcmp(linkname, "PadModified") == 0) {
      if (IsRedrawBlocked())
         return;
      if (!fbModifiedSignalFlag)
         QTimer::singleShot(1, this, SLOT(ProcessPadModifiedSignal()));
      fbModifiedSignalFlag = true;
   }
}

void TGo4ViewPanel::linkedUpdated(TGo4Slot* slot, TObject* obj)
{
   if (slot == 0)
      return;

   if (IsRedrawBlocked())
      return;

   Int_t kind = GetDrawKind(slot);

   TGo4Slot* padslot = slot;
   if (kind != kind_PadSlot)
      padslot = slot->GetParent();

   if (((kind > 0) && (kind < 100)) || (kind == kind_Condition)
         || (kind == kind_Latex) || (kind == kind_Func)) {
      TGo4Picture* padopt = GetPadOptions(padslot);

      if (padopt != 0) {
         padopt->SetContentModified(true);
         padopt->SetPadModified();

         // in this small period other objects can come,
         // therefore only one repaint will be done
         ShootRepaintTimer();
      }
   }
}

void TGo4ViewPanel::linkedRemoved(TGo4Slot* slot, TObject* obj)
{
   if (slot == 0)
      return;

   if (IsRedrawBlocked())
      return;

   int kind = GetDrawKind(slot);

   if (kind == kind_ThisPad) {
      TPad* pad = (TPad*) obj;
      if (pad != 0)
         PadDeleted(pad);
      return;
   }

   TGo4Slot* padslot = slot;
   if (kind != kind_PadSlot)
      padslot = slot->GetParent();

   CheckObjectsAssigments(GetSlotPad(padslot), padslot);

   if (((kind > 0) && (kind < 100)) || (kind == kind_Condition)
         || (kind == kind_Latex) || (kind == kind_Func)) {
      CleanupGedEditor();
      TGo4Picture* padopt = GetPadOptions(padslot);
      if (padopt != 0) {
         padopt->SetPadModified();
         ShootRepaintTimer();
      }
   }
}

bool TGo4ViewPanel::IsAcceptDrag(const char * itemname, TClass * cl, int kind)
{
   if (cl == 0)
      return false;
   int cando = Browser()->ItemCanDo(itemname);
   return TGo4BrowserProxy::CanDrawItem(cando)
         || cl->InheritsFrom(TGo4Condition::Class());
}

void TGo4ViewPanel::DropOnPad(TPad* pad, const char * itemname, TClass * cl, int kind)
{
   if (cl == 0) return;
   if (pad == 0) pad = GetCanvas();

   if (cl->InheritsFrom(TGo4Fitter::Class())) {
      SetActivePad(pad);
      EditItem(itemname);
      return;
   }
   int cando = Browser()->ItemCanDo(itemname);
   if (!TGo4BrowserProxy::CanDrawItem(cando)
         && !cl->InheritsFrom(TGo4Condition::Class()))
      return;

   if (AddDrawObject(pad, kind_Link, itemname, 0, false, 0) == 0) return;

   SetActivePad(pad);

   ShootRepaintTimer();

   Browser()->GetBrowserObject(itemname,
         go4sett->getFetchDataWhenDraw() ? 2 : 1);
}

// ****************************************************************

void TGo4ViewPanel::CompleteInitialization()
{
   TGo4LockGuard lock;

   // create appropriate entry in OM
   UpdatePadStatus(GetCanvas(), true);

   fAutoScaleCheck->setChecked(GetPadOptions(GetCanvas())->IsAutoScale());

   //    fMenuBar
   connect(TGo4MdiArea::Instance(),
         SIGNAL(panelSignal(TGo4ViewPanel*, TPad*, int)), this,
         SLOT(panelSlot(TGo4ViewPanel*, TPad*, int)));

   // printf("Resize again x %d y %d\n", go4sett->lastPanelSize().width(), go4sett->lastPanelSize().height());
   // resize(go4sett->lastPanelSize());

   // adjust canvas size before any drawing will be done
   if (fxQCanvas) fxQCanvas->Resize();

   SetActivePad(GetCanvas());

   SetPadDefaults(GetCanvas());

   //fxQCanvas->Modified();

   fbCloneFlag = go4sett->getCloneFlag();
}

void TGo4ViewPanel::SetMouseMode(int mode)
{
   fiMouseMode = mode;
}

int TGo4ViewPanel::GetMouseMode()
{
   return fiMouseMode;
}

QString TGo4ViewPanel::GetSelectedMarkerName(TPad* pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0) return QString::null;
   return QString(padslot->GetPar("::SelMarker"));
}

int TGo4ViewPanel::GetSelectedMarkerIndex(TPad* pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0) return -1;
   Int_t selindex = -1;
   if (!padslot->GetIntPar("::SelIndex", selindex)) return -1;
   return selindex;
}

TGo4Slot* TGo4ViewPanel::GetSelectedSlot(TPad* pad, int* selkind, TObject** selobj)
{
   if (selkind != 0) *selkind = kind_None;
   if (selobj != 0) *selobj = 0;

   TGo4Slot* padslot = GetPadSlot(pad);
   QString selname = GetSelectedMarkerName(pad);
   int selindex = GetSelectedMarkerIndex(pad);

   if ((padslot == 0) || (selname.length() == 0)) return 0;

   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      TObject* obj = subslot->GetAssignedObject();

      if ((drawkind == kind_Link) && (obj != 0)) {
         if (obj->InheritsFrom(TGo4Condition::Class()))
            drawkind = kind_Condition;
      }

      if ((drawkind == kind_Marker) || (drawkind == kind_Window)
            || (drawkind == kind_Poly) || (drawkind == kind_Latex)
            || (drawkind == kind_Arrow) || (drawkind == kind_Condition)) {

         if ((obj == 0) || (selname != obj->GetName()))
            continue;

         if (drawkind == kind_Condition) {
            TGo4Condition* selcond = dynamic_cast<TGo4Condition*>(obj);
            if (obj->InheritsFrom(TGo4CondArray::Class())) {
               TGo4CondArray* arr = (TGo4CondArray*) obj;
               selcond = 0;
               if ((selindex >= 0) && (selindex < arr->GetNumber()))
                  selcond = arr->At(selindex);
            }
            drawkind = kind_None;
            obj = selcond;
            if (selcond != 0)
               if (selcond->InheritsFrom(TGo4WinCond::Class()))
                  drawkind = kind_Window;
               else if (selcond->InheritsFrom(TGo4PolyCond::Class()))
                  drawkind = kind_Poly;
         }
         if (selkind != 0)
            *selkind = drawkind;
         if (selobj != 0)
            *selobj = obj;
         return subslot;
      }
   }
   return 0;
}

bool TGo4ViewPanel::IsConditionSelected(TPad* pad)
{
   TGo4Slot* slot = GetSelectedSlot(pad, 0, 0);
   if (slot == 0)
      return false;

   return (GetDrawKind(slot) == kind_Condition);
}

TPad* TGo4ViewPanel::FindPadWithItem(const char* itemname)
{
   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      int drawkind = GetDrawKind(subslot);
      if ((drawkind == kind_Link) || (drawkind == kind_Condition) || (drawkind == kind_Latex) || (drawkind == kind_Func)) {
         const char* linkname = GetLinkedName(subslot);
         if (linkname != 0)
            if (strcmp(linkname, itemname) == 0)
               return GetSlotPad(subslot->GetParent());
      }
   }
   return 0;
}

const char* TGo4ViewPanel::GetDrawItemName(int itemcnt)
{
   int cnt = 0;

   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      int drawkind = GetDrawKind(subslot);
      if ((drawkind == kind_Link) || (drawkind == kind_Condition)) {
         const char* linkname = GetLinkedName(subslot);
         if (linkname != 0) {
            if (cnt++ == itemcnt) return linkname;
         }
      }
   }

   return 0;
}


void TGo4ViewPanel::UndrawItemOnPanel(const char* itemname)
{
   TGo4LockGuard lock;

   TObjArray delslots;

   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      int drawkind = GetDrawKind(subslot);
      if ((drawkind == kind_Link) || (drawkind == kind_Condition) || (drawkind == kind_Latex) || (drawkind == kind_Func)) {
         const char* linkname = GetLinkedName(subslot);
         if ((linkname != 0) && (strcmp(linkname, itemname) == 0)) {
            delslots.Add(subslot);
            TGo4Slot* padslot = subslot->GetParent();
            TGo4Picture* padopt = GetPadOptions(padslot);
            if (padopt != 0)
               padopt->SetPadModified();
         }
      }
   }
   if (delslots.GetLast() >= 0) {
      delslots.Delete();
      ShootRepaintTimer();
   }
}

void TGo4ViewPanel::SetSelectedMarker(TPad* pad, const QString& selname,
      int selindex)
{
   TGo4LockGuard lock;

   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0)
      return;

   if (selname.length() == 0)
      selindex = -1;

   int oldselindex = GetSelectedMarkerIndex(pad);
   QString oldselname = GetSelectedMarkerName(pad);

   TGo4Slot* oldsel = GetSelectedSlot(pad, 0, 0);
   if (oldsel != 0)
      SetSpecialDrawOption(oldsel, 0);

   if (selname.length() > 0)
      padslot->SetPar("::SelMarker", selname.toLatin1().constData());
   else
      padslot->RemovePar("::SelMarker");

   if (selindex > -1)
      padslot->SetIntPar("::SelIndex", selindex);
   else
      padslot->RemovePar("::SelIndex");

   int newselkind = 0;
   TObject* newselobj = 0;
   TGo4Slot* newselslot = GetSelectedSlot(pad, &newselkind, &newselobj);

   if ((selindex >= 0) && (newselslot != 0)) {
      QString drawopt("sel=");
      drawopt += QString::number(selindex);
      SetSpecialDrawOption(newselslot, drawopt.toLatin1().constData());
   }

   if (((oldselindex != selindex) || (oldselname != selname))
         && ((oldselindex >= 0) || (selindex >= 0))) {
      MarkPadModified(pad);
      ShootRepaintTimer(pad);
   } else if (newselobj != 0) {
      // this will bring object to the top
      newselobj->Pop();
      // very special case, normally one should not call pad->Update()
      pad->Update();
   }

}

void TGo4ViewPanel::SetSelectedMarkerByMouseClick(TPad* pad, const char* name)
{
   TGo4LockGuard lock;

   if (!fbMarkEditorVisible) return;
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0) return;

   bool find = false;

   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if ((drawkind == kind_Marker) || (drawkind == kind_Window)
            || (drawkind == kind_Poly)) {
         TObject* obj = subslot->GetAssignedObject();
         if ((obj != 0) && (strcmp(obj->GetName(), name) == 0)) {
            SetSelectedMarker(pad, name, -1);
            SetActiveObj(pad, drawkind, subslot);
            find = true;
            break;
         }
      }
      if (drawkind != kind_Condition) continue;
      TGo4Condition* cond =
            dynamic_cast<TGo4Condition*>(subslot->GetAssignedObject());
      if (cond==0) continue;

      if (strcmp(cond->GetName(), name) == 0) {
         SetSelectedMarker(pad, name, -1);

         if (cond->InheritsFrom(TGo4PolyCond::Class()))
            drawkind = kind_Poly;
         else
            drawkind = kind_Window;

         SetActiveObj(pad, drawkind, subslot);
         find = true;
         break;
      }

      TGo4CondArray* arr = dynamic_cast<TGo4CondArray*>(cond);
      if (arr == 0)
         continue;

      for (int ncon = 0; ncon < arr->GetNumber(); ncon++)
         if (strcmp(arr->At(ncon)->GetName(), name) == 0) {
            SetSelectedMarker(pad, arr->GetName(), ncon);
            if (arr->At(ncon)->InheritsFrom(TGo4PolyCond::Class()))
               drawkind = kind_Poly;
            else
               drawkind = kind_Window;
            SetActiveObj(pad, drawkind, subslot);
            find = true;
            break;
         }
   }
   if (find)
      RefreshButtons();
}

void TGo4ViewPanel::StartConditionEditing(TPad* pad)
{
   TGo4LockGuard lock;

   int selectedkind;
   TGo4Slot* selslot = GetSelectedSlot(GetActivePad(), &selectedkind, 0);

   if (selslot == 0)
      return;

   if (selectedkind == kind_Window) {
      FreezeMode->setChecked(false);
      RegionB->setChecked(true);
      fbLeaveFocusAfterCondEnd = true;
   } else if (selectedkind == kind_Poly) {
      FreezeMode->setChecked(false);
      PolyB->setChecked(true);
      fbLeaveFocusAfterCondEnd = true;
   } else
      return;

   MarkPadModified(pad);
   ShootRepaintTimer(pad);
}

void TGo4ViewPanel::SwitchMarkerButton(int kind, bool on)
{
   if (!fbTypingMode)
      return;
   CompleteMarkerEdit(GetActivePad());
   if (!on) {
      SetMouseMode(kMouseROOT);
   } else {
      fiPickCounter = 0;
      switch (kind) {
         case kind_Marker:
            SetMouseMode(kMousePickCursor);
            break;
         case kind_Window:
            SetMouseMode(kMousePickLimits);
            break;
         case kind_Poly:
            SetMouseMode(kMousePickPolygon);
            break;
         case kind_Latex:
            SetMouseMode(kMousePickLatex);
            break;
         case kind_Arrow:
            SetMouseMode(kMouseDraw);
            break;
         default:
            SetMouseMode(kMousePickCursor);
            break;
      }
      int selectedkind;
      TGo4Slot* selslot = GetSelectedSlot(GetActivePad(), &selectedkind, 0);
      if (selectedkind != kind)
         SetSelectedMarker(GetActivePad(), "", -1);
   }

   RefreshButtons();
}

void TGo4ViewPanel::SetCursorMode(bool on)
{
   SwitchMarkerButton(kind_Marker, on);
}

void TGo4ViewPanel::SetRegionMode(bool on)
{
   SwitchMarkerButton(kind_Window, on);
}

void TGo4ViewPanel::SetPolygonMode(bool on)
{
   SwitchMarkerButton(kind_Poly, on);
}

void TGo4ViewPanel::SetLateXMode(bool on)
{
   SwitchMarkerButton(kind_Latex, on);
}

void TGo4ViewPanel::SetDrawingMode(bool on)
{
   SwitchMarkerButton(kind_Arrow, on);
}

void TGo4ViewPanel::SetFreezeMouseMode(bool on)
{
   if (!fbTypingMode)
      return;
   fbPickAgain = on;
   RefreshButtons();
}

void TGo4ViewPanel::RefreshButtons()
{
   TGo4LockGuard lock;

   MarkerPanel->setVisible(fbMarkEditorVisible);
   if (fxQCanvas)
      fxQCanvas->setMaskDoubleClick(fbMarkEditorVisible);

//   if(!fbMarkEditorVisible) return;

   bool iscondition = IsConditionSelected(GetActivePad());

   fbTypingMode = false;
   GetConditionBtn->setVisible(iscondition);
   InfoConditionBtn->setVisible(iscondition);
   EditConditionBtn->setVisible(iscondition);
   SetConditionBtn->setVisible(iscondition);
   if (iscondition) {
      TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
      TGo4Condition* cond =
            slot == 0 ? 0 : dynamic_cast<TGo4Condition*>(slot->GetAssignedObject());
      ModifyConditionBtn->setVisible((cond != 0) && (cond->IsChanged() != 0));

      QString iconname = ":/icons/right.png";
      QString tooltip = "Refresh condition from analysis";
      if (!BrowserItemRemote(GetLinkedName(slot))) {
         iconname = ":/icons/refresh.png";
         tooltip = "Refresh condition from source";
      }
      GetConditionBtn->setIcon(QIcon(iconname));
      GetConditionBtn->setToolTip(tooltip);

   } else
      ModifyConditionBtn->setVisible(false);

   switch (GetMouseMode()) {
      case kMouseROOT:
         CursorB->setChecked(false);
         RegionB->setChecked(false);
         LatexB->setChecked(false);
         DrawB->setChecked(false);
         PolyB->setChecked(false);
         break;
      case kMousePickCursor:
         CursorB->setChecked(true);
         RegionB->setChecked(false);
         LatexB->setChecked(false);
         DrawB->setChecked(false);
         PolyB->setChecked(false);
         break;
      case kMousePickLimits:
         CursorB->setChecked(false);
         RegionB->setChecked(true);
         LatexB->setChecked(false);
         DrawB->setChecked(false);
         PolyB->setChecked(false);
         break;
      case kMousePickPolygon:
         CursorB->setChecked(false);
         RegionB->setChecked(false);
         LatexB->setChecked(false);
         DrawB->setChecked(false);
         PolyB->setChecked(true);
         break;
      case kMousePickLatex:
         CursorB->setChecked(false);
         RegionB->setChecked(false);
         LatexB->setChecked(true);
         DrawB->setChecked(false);
         PolyB->setChecked(false);
         break;
      case kMouseDraw: // currently, we only draw arrows:
         CursorB->setChecked(false);
         RegionB->setChecked(false);
         LatexB->setChecked(false);
         DrawB->setChecked(true);
         PolyB->setChecked(false);
         break;
      default:
         CursorB->setChecked(false);
         RegionB->setChecked(false);
         LatexB->setChecked(false);
         DrawB->setChecked(false);
         PolyB->setChecked(false);
         break;
   }; // switch()
   FreezeMode->setChecked(fbPickAgain);

   SelectedMarkerCmb->clear();
   SelectedMarkerCmb->addItem("new");

   TGo4Slot* slot = GetPadSlot(GetActivePad());
   int findindx = -1;

   QString selname = GetSelectedMarkerName(GetActivePad());
   int selindex = GetSelectedMarkerIndex(GetActivePad());

   if (slot != 0)
      for (int n = 0; n < slot->NumChilds(); n++) {
         TGo4Slot* subslot = slot->GetChild(n);
         int drawkind = GetDrawKind(subslot);
         if ((drawkind == kind_Marker) || (drawkind == kind_Window)
               || (drawkind == kind_Poly) || (drawkind == kind_Condition)) {
            TObject* obj = subslot->GetAssignedObject();
            if (obj == 0)
               continue;

            if (obj->InheritsFrom(TGo4CondArray::Class())) {
               TGo4CondArray* arr = (TGo4CondArray*) obj;
               for (int ncon = 0; ncon < arr->GetNumber(); ncon++) {
                  TGo4Condition* sub = arr->At(ncon);
                  QString fullname(arr->GetName());
                  fullname += "/Sub";
                  fullname += QString::number(ncon);
                  SelectedMarkerCmb->addItem(fullname);

                  if ((selname == obj->GetName()) && (selindex == ncon))
                     findindx = SelectedMarkerCmb->count() - 1;
               }
            } else {
               SelectedMarkerCmb->addItem(obj->GetName());
               if (selname == obj->GetName())
                  findindx = SelectedMarkerCmb->count() - 1;
            }
         }
      }

   if (findindx < 0) {
      findindx = 0;
      SetSelectedMarker(GetActivePad(), "", -1);
   }

   SelectedMarkerCmb->setCurrentIndex(findindx);
   DelSelectedMarker->setEnabled((findindx > 0) /*&& !iscondition*/);

   if (fbMarkEditorVisible) {
      MarkerPanel->ensurePolished();
      MarkerPanel->update();
      MarkerPanel->show();
   }
   fbTypingMode = true;
}

void TGo4ViewPanel::SelectedMarkerCmb_activated(int indx)
{
   if (!fbTypingMode)
      return;
   if (indx == 0)
      SetSelectedMarker(GetActivePad(), "", -1);
   else {
      QString selname = SelectedMarkerCmb->itemText(indx);
      int selindex = -1;
      int p = selname.indexOf("/Sub");
      if (p > 0) {
         selindex = selname.mid(p + 4).toInt();
         selname.truncate(p);
      } else
         selindex = -1;
      SetSelectedMarker(GetActivePad(), selname, selindex);
   }

   int drawkind = 0;
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), &drawkind, 0);

   if ((slot != 0) && (drawkind > 0)) {
      SetActiveObj(GetActivePad(), drawkind, slot);
      SwitchMarkerButton(drawkind, true);
   } else
      SwitchMarkerButton(kind_Marker, false);
}

void TGo4ViewPanel::DelSelectedMarker_clicked()
{
   if (!fbTypingMode) return;
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (slot == 0) return;
   delete slot;
   SetSelectedMarker(GetActivePad(), "", -1);
   RedrawPanel(GetActivePad(), true);
}

void TGo4ViewPanel::SetMarkerPanel()
{
   fbMarkEditorVisible = !fbMarkEditorVisible;
   if (!fbMarkEditorVisible) {
      // switch back to normal root mouse when editor is hidden
      CompleteMarkerEdit(GetActivePad());
      SetMouseMode(kMouseROOT);
      gROOT->SetEditorMode("");
      fiPickCounter = 0;
   }
   RefreshButtons();
   ShootRepaintTimer();
}

void TGo4ViewPanel::LogMarkerValues()
{
   if (!fbTypingMode)
      return;

   TGo4Slot* slot = GetPadSlot(GetActivePad());
   if (slot == 0)
      return;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      TObject* obj = subslot->GetAssignedObject();
      if ((drawkind < kind_Condition) || (obj == 0))
         continue;
      switch (drawkind) {
         case kind_Marker:
            obj->Print("*");
            break;
         case kind_Latex:
            obj->Print("*");
            break;
         case kind_Arrow:
            obj->Print("go4log");
            break;
         case kind_Window:
         case kind_Poly:
         case kind_Condition:
            obj->Print("go4log-limits-stats");
            break;
      }
   }
}

void TGo4ViewPanel::ClearAllMarkers()
{
   TGo4LockGuard lock;

   TPad* pad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (pad == 0)
      pad = GetCanvas();

   ProcessMarkersClear(pad, IsApplyToAllFlag());

   RedrawPanel(pad, true);
}

void TGo4ViewPanel::ProcessMarkersClear(TPad *pad, bool withsubpads)
{
   if (pad == 0) return;

   DeleteDrawObjects(pad, kind_Marker);
   DeleteDrawObjects(pad, kind_Window);
   DeleteDrawObjects(pad, kind_Poly);
   DeleteDrawObjects(pad, kind_Latex);
   DeleteDrawObjects(pad, kind_Arrow);
   if (!withsubpads) return;

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0) return;

   TGo4Iter iter(slot, true);
   while (iter.next()) {
      TPad* subpad = GetSlotPad(iter.getslot());
      if (subpad != 0)
         ProcessMarkersClear(subpad, false);
   }
}

void TGo4ViewPanel::GetConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot) != kind_Condition)
      return;
   const char* itemname = GetLinkedName(slot);
   if (itemname == 0)
      return;

   Browser()->GetBrowserObject(itemname, 2);

   RefreshButtons();
}

void TGo4ViewPanel::InfoConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot) != kind_Condition)
      return;
   const char* itemname = GetLinkedName(slot);
   if (itemname != 0)
      ShowItemInfo(itemname);
}

void TGo4ViewPanel::EditConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot) != kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname != 0)
      EditItem(itemname);
}

void TGo4ViewPanel::SetConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot) != kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname != 0) {
      UpdateItemInAnalysis(itemname);
      TGo4Condition* cond = dynamic_cast<TGo4Condition*>(slot->GetAssignedObject());
      if(cond!=0) cond->SetChanged(kFALSE);
      RefreshButtons();
   }
}

void TGo4ViewPanel::SaveMarkers()
{
   QFileDialog fd(this, "Save Markers of active pad into", QString(),
         "ROOT file (*.root)");
   fd.setFileMode(QFileDialog::AnyFile);
   fd.setAcceptMode(QFileDialog::AcceptSave);
   if (fd.exec() == QDialog::Accepted) {
      QStringList flst = fd.selectedFiles();
      if (flst.isEmpty())
         return;

      QString filename = flst[0];
      if (!filename.endsWith(".root"))
         filename.append(".root");
//      fxTGo4PreviewPanelSlots->SaveMarkerSetup(filename,"Markersetup");
   }
}

void TGo4ViewPanel::LoadMarkers()
{
   QFileDialog fd(this, "Load Marker setup from:", QString(),
         "ROOT file (*.root)");
   fd.setFileMode(QFileDialog::ExistingFile);
   if (fd.exec() == QDialog::Accepted) {
      QStringList flst = fd.selectedFiles();
      if (flst.isEmpty())
         return;
      QString filename = flst[0];
   }
}

void TGo4ViewPanel::SetActivePad(TPad* pad)
{
   TGo4LockGuard lock;

   if (fxActivePad != pad)
      CompleteMarkerEdit(fxActivePad);

   raise();

   if (pad == 0) {
      if (!fxWCanvas) {
         GetCanvas()->SetSelected(0);
         GetCanvas()->SetSelectedPad(0);
         CanvasUpdate();
      }
      return;
   }

   fxActivePad = pad;
   if (!fxWCanvas) {
      fxActivePad->cd();
      GetCanvas()->SetSelectedPad(fxActivePad);
   }

   TGo4MdiArea::Instance()->SetSelectedPad(fxActivePad);

   // no need for update of web canvas here
   if (!fxWCanvas) {
      BlockPanelRedraw(true);
      CanvasUpdate();
      BlockPanelRedraw(false);
   }

   DisplayPadStatus(fxActivePad);

   UpdatePanelCaption();
   CallPanelFunc(panel_Activated, fxActivePad);
}

void TGo4ViewPanel::PadClickedSlot(TPad* pad, int px, int py)
{
   TGo4LockGuard lock;
   SetActivePad(pad);

   if (pad == 0)
      return;

   if ((px<0) || (py<0)) {
      px = pad->GetEventX();
      py = pad->GetEventY();
   }

   Double_t x = pad->PadtoX(pad->AbsPixeltoX(px));
   Double_t y = pad->PadtoY(pad->AbsPixeltoY(py));

   bool docreate = GetSelectedMarkerName(pad).length() == 0;
   bool docheck = false;
   bool iscreated = false;

   switch (fiMouseMode) {
      case kMouseROOT: {
         TObject *obj = GetCanvas()->GetSelected();
         if (obj != 0)
            if (obj->InheritsFrom(TGo4Marker::Class())
                  || obj->InheritsFrom(TGo4WinCondView::Class())
                  || obj->InheritsFrom(TGo4PolyCondView::Class()))
               SetSelectedMarkerByMouseClick(pad, obj->GetName());

         break;
      }

      case kMousePickCursor: {
         // we have a click on our working pad, get coordinates:
         gROOT->SetEditorMode("");

         if (docreate) {
            TGo4Marker* mark = new TGo4Marker(x, y, 28);
            AddMarkerObj(pad, kind_Marker, mark);
            Int_t ix = TGo4Marker::GetInstances() - 1;
            mark->SetMarkerColor((ix) % 6 + 2);
            mark->SetHistogram(GetPadHistogram(pad));
            if (!fbPickAgain)
               fiMouseMode = kMouseROOT; // reset pick
            mark->Draw("");
         } else {
            TGo4Marker* mark = dynamic_cast<TGo4Marker*>(GetActiveObj(pad, kind_Marker));
            if(mark!=0) {
               mark->SetX(x);
               mark->SetY(y);
            }
            if (!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
         }
         pad->Modified();
         pad->Update();

//        RedrawPanel(pad, true);
         break;
      }

      case kMousePickLimits: {
         gROOT->SetEditorMode("");
         TGo4WinCond* conny = 0;
         Double_t xmin(x), xmax(x), ymin(y), ymax(y);
         if (fiPickCounter == 0) {
            // pick the first time after enabling limits record:
            if (docreate) {
               TH1* hist = GetPadHistogram(pad);
               bool fbTwoDimRegion = (hist != 0) && (hist->GetDimension() > 1);
               int ix = GetNumMarkers(pad, kind_Window);
               QString name = "Region " + QString::number(ix + 1);
               conny = new TGo4WinCond(name.toLatin1().constData());
               iscreated = true;
               if (fbTwoDimRegion)
                  conny->SetValues(0, 0, 0, 0);
               else
                  conny->SetValues(0, 0);
               conny->SetLineColor(GetAutoColor(ix%9 + 1));
               conny->SetFillColor(GetAutoColor(ix%9 + 1));
               conny->SetFillStyle(3002);
               conny->SetWorkHistogram(hist);
               // adjust two dim region to one dim defaults
               conny->SetYRMSDraw(fbTwoDimRegion && conny->IsXRMSDraw());
               conny->SetYMeanDraw(fbTwoDimRegion && conny->IsXMeanDraw());
               conny->SetYMaxDraw(fbTwoDimRegion && conny->IsXMaxDraw());
               AddMarkerObj(pad, kind_Window, conny);
            } else
               conny = dynamic_cast<TGo4WinCond*>(GetActiveObj(pad, kind_Window));
            if(conny==0) return;
            fiPickCounter++;
         } else
            if (fiPickCounter==1) {
               conny = dynamic_cast<TGo4WinCond*> (GetActiveObj(pad, kind_Window));
               if(conny==0) return;
               xmin = conny->GetXLow();
               ymin = conny->GetYLow();
               fiPickCounter=0;
               if(!fbPickAgain) fiMouseMode=kMouseROOT;
               docheck = true;
            } else {
               std::cout <<"TGo4ViewPanel:PadClickedSlot() NEVER COME HERE" << std::endl;
               return;
            }
         // do not change original condition dimension
         if (conny->GetDimension() > 1)
            conny->SetValues(xmin, xmax, ymin, ymax);
         else
            conny->SetValues(xmin, xmax);

         TGo4Slot* condslot = GetSelectedSlot(pad, 0, 0);
         if (GetDrawKind(condslot) == kind_Condition) {
            TGo4Condition* maincond =
                  dynamic_cast<TGo4Condition*>(condslot->GetAssignedObject());
            if (maincond!=0) maincond->SetChanged(kTRUE);
         }

         conny->SetChanged(kTRUE);
         if (iscreated)
            conny->Draw("");

         pad->Modified();
         pad->Update();
         // RedrawPanel(pad, true);
         break;
      }

      case kMousePickPolygon: {
        //std::cout <<"TGo4ViewPanel:PadClickedSlot() in kMousePickPolygon" << std::endl;
         gROOT->SetEditorMode("");
         TGo4PolyCond* cond = 0;

         if (fiPickCounter == 0) {
            // pick the first time after enabling limits record:
            if (docreate) {
               TH1* hist = GetPadHistogram(pad);
               int ix = GetNumMarkers(pad, kind_Poly);
               QString name = "Polygon " + QString::number(ix + 1);
               cond = new TGo4PolyCond(name.toLatin1().constData());
               iscreated = true;
               AddMarkerObj(pad, kind_Poly, cond);
               cond->SetWorkHistogram(hist);
            } else {
               cond = dynamic_cast<TGo4PolyCond*>(GetActiveObj(pad, kind_Poly));
               // start region from the beginning
               if (cond!=0) {
                  TCutG* cut = cond->GetCut(kTRUE);
                  delete cut;
               }
            }
            if(cond==0) return;
            fiPickCounter++;
         } else {
            cond = dynamic_cast<TGo4PolyCond*> (GetActiveObj(pad, kind_Poly));
            if(cond==0) return;
            fiPickCounter++;
         }

         if (cond != 0) {
            TCutG* cut = cond->GetCut(kFALSE);

            // this insert point in last mouse position
            if (cut == 0) {
               cut = new TCutG(TGo4PolyCond::NextAvailableName(), 1);
               cut->SetPoint(0, x, y);
               cond->SetValuesDirect(cut);
            } else {
               cut->InsertPoint();
            }

            cond->SetChanged(kTRUE);

            int ix = GetNumMarkers(pad, kind_Poly);

            cond->SetLineColor(GetAutoColor(ix%9 + 1));
            cond->SetFillColor(GetAutoColor(ix%9 + 1));
            cond->SetFillStyle(3002);
         }

         // mark condition
         TGo4Slot* condslot = GetSelectedSlot(pad, 0, 0);
         if (GetDrawKind(condslot) == kind_Condition) {
            TGo4Condition* maincond =
                  dynamic_cast<TGo4Condition*>(condslot->GetAssignedObject());
            if (maincond!=0) maincond->SetChanged(kTRUE);
         }

         if (iscreated)
            cond->Draw("");

         pad->Modified();
         pad->Update();
         // RedrawPanel(pad, true);
         break;
      }

      case kMousePickLatex: {
         gROOT->SetEditorMode("");
         if (docreate) {
            int ix = GetNumMarkers(pad, kind_Latex);
            QString name = QString("Label ") + QString::number(ix + 1);
            bool ok;
            QString txt = QInputDialog::getText(this,
                  "Enter new LaTeX label text:", name, QLineEdit::Normal,
                  QString::null, &ok);
            if (ok && (txt.length() > 0)) {
               TLatex* latex = new TLatex(x, y, name.toLatin1().constData());
               latex->SetName(name.toLatin1().constData());
               latex->SetTitle(txt.toLatin1().constData());
               AddMarkerObj(pad, kind_Latex, latex);
               latex->Draw();
            } else {
               fiMouseMode = kMouseROOT;
            }
         } else {
            TLatex* latex = dynamic_cast<TLatex*>(GetActiveObj(pad, kind_Latex));
            if(latex!=0) {
               latex->SetX(x);
               latex->SetY(y);
            }
         }
         if (!fbPickAgain)
            fiMouseMode = kMouseROOT; // reset pick

         pad->Modified();
         pad->Update();
//        RedrawPanel(pad, true);
         break;
      }

      case kMouseDraw: {
         gROOT->SetEditorMode("");
         if (fiPickCounter == 0) {
            // pick the first time after enabling limits record:
            TArrow* arrow = new TArrow(x, y, x, y, 0.02);
            AddMarkerObj(pad, kind_Arrow, arrow);
            fiPickCounter++;
            arrow->Draw("");
         } else if (fiPickCounter == 1) {
            TArrow* arrow = dynamic_cast<TArrow*>(GetActiveObj(pad, kind_Arrow));
            if(arrow!=0) {
               arrow->SetX2(x);
               arrow->SetY2(y);
            }
            if(!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
            fiPickCounter=0;
         } else {
            std::cout <<"TGo4ViewPanel:MouseClick() NEVER COME HERE" << std::endl;
            return;
         }
         // do not change original condition dimension
         pad->Modified();
         pad->Update();

         // RedrawPanel(pad, true);
         break;
      }
   }

   if (docheck)
      CheckActionAtTheEnd(pad);
}

void TGo4ViewPanel::CheckActionAtTheEnd(TPad* pad)
{
   bool goback = true;

// uncomment this code to have loop mode for array of conditions

//   if(fbPickAgain) {
//      QString selname = GetSelectedMarkerName(pad);
//      int selindex = GetSelectedMarkerIndex(pad);
//      if (selindex>=0) {
//         TGo4Slot* slot = GetSelectedSlot(pad, 0, 0);
//         TGo4CondArray* arr = 0;
//         if (slot!=0)
//           arr = dynamic_cast<TGo4CondArray*> (slot->GetAssignedObject());
//         if (arr!=0) {
//           if (selindex<arr->GetNumber()-1) {
//              SetSelectedMarker(pad, selname, selindex+1);
//              goback = false;
//           }
//           else
//             SetSelectedMarker(pad, "", -1);
//         }
//      }
//   }
   if (goback) {
      if (fbLeaveFocusAfterCondEnd)
         ServiceCall("ActivateConditionEditor");
      fbLeaveFocusAfterCondEnd = false;
//      MarkPadModified(pad);
//      ShootRepaintTimer(pad);
   }
}

bool TGo4ViewPanel::CompleteMarkerEdit(TPad* pad)
{
   bool res = false;
   bool needredraw = false; // complete repaint
   bool needupdate = false; // only pad update
   bool needrefresh = false; // refresh buttons
   bool docheck = false;
   bool candelete = !IsConditionSelected(pad);

   switch (fiMouseMode) {
      case kMousePickLimits: {
         if (fiPickCounter > 0) {
            TGo4WinCond* cond = dynamic_cast<TGo4WinCond*>(GetActiveObj(pad, kind_Window));
            if (cond!=0) {
               if (candelete) DeleteDrawObject(pad, cond);
               needredraw = true;
            }
            fiPickCounter = 0;
            if(!fbPickAgain) fiMouseMode=kMouseROOT;
            docheck = true;
         }

         res = true;
         break;
      }

      case kMousePickPolygon: {
         if (fiPickCounter>0) {
            TGo4PolyCond* cond = dynamic_cast<TGo4PolyCond*> (GetActiveObj(pad,kind_Poly));
         if (cond!=0) {
            bool delcond = true;
            TCutG* cut = cond->GetCut(kFALSE);
            if (cut!=0) {
               int n = cut->GetN();
               Double_t x, y;
               cut->GetPoint(0, x, y);
               delcond = (n<3);

               if (n>=3)
                  cut->SetPoint(n, x, y);

               int ix = GetNumMarkers(pad, kind_Poly);
               cond->SetLineColor(GetAutoColor(ix%9 + 1));
               cond->SetFillColor(GetAutoColor(ix%9 + 1));
               cond->SetFillStyle(3002);
            }

            if (delcond && candelete) {
               DeleteDrawObject(pad, cond);
               needredraw = true;
            } else {
               needupdate = true;
            }
         }
         if(!fbPickAgain) fiMouseMode=kMouseROOT;
         fiPickCounter = 0;
         docheck = true;
       }

       needrefresh = true;
       res = true;
       break;
     }

     case kMouseDraw: {
        if (fiPickCounter>0) {
           TArrow* arrow = dynamic_cast<TArrow*> (GetActiveObj(pad, kind_Arrow));
           if (arrow!=0) {
              DeleteDrawObject(pad, arrow);
              needredraw = true;
           }
           fiPickCounter = 0;
           if(!fbPickAgain) fiMouseMode=kMouseROOT;
        }
        res = true;
        break;
     }
   }
   if (needredraw)
      RedrawPanel(pad, true);
   else {
      if (needupdate) {
         pad->Modified();
         pad->Update();
      }
      if (needrefresh)
         RefreshButtons();
   }

   if (docheck)
      CheckActionAtTheEnd(pad);

   return res;
}

void TGo4ViewPanel::PadDoubleClickedSlot(TPad* pad, int, int)
{
   if (CompleteMarkerEdit(pad)) return;
   if (fxDoubleClickTimerPad != 0) return;

   fxDoubleClickTimerPad = pad;
   QTimer::singleShot(100, this, SLOT(ProcessPadDoubleClick()));
}

void TGo4ViewPanel::CanvasDropEventSlot(QDropEvent* event, TPad* pad)
{
   emit widgetService(this, service_DropEvent, (const char*) pad, event);
}

void TGo4ViewPanel::ProcessPadDoubleClick()
{
   if (fxDoubleClickTimerPad == 0) return;

   if (GetNumberOfPads(GetCanvas()) <= 1)  {
      MoveScale(1,0,0,0);
      fxDoubleClickTimerPad = 0;
      return;
   }

   TGo4Picture pic;
   MakePictureForPad(&pic, fxDoubleClickTimerPad, true);
   fxDoubleClickTimerPad = 0;

   if (pic.GetNumObjNames() == 0) return;

   TGo4ViewPanel* newpanel = CreateViewPanel();
   newpanel->ProcessPictureRedraw("", newpanel->GetCanvas(), &pic);
   newpanel->ShootRepaintTimer();
}

void TGo4ViewPanel::MenuCommandExecutedSlot(TObject* obj, const char* cmdname)
{
   TPad* pad = dynamic_cast<TPad*>(obj);
   if (pad != 0)
      UpdatePadStatus(pad, true);

   Browser()->Scan_gROOT();

   if ((strcmp(cmdname, "UnZoom") == 0) && obj->InheritsFrom(TAxis::Class())) {

      // this code is done specially to treat unzoom in the THStack

      TGo4Iter iter(GetPanelSlot(), kTRUE);
      TGo4Slot* subslot = 0;

      do {
         if (subslot == 0)
            subslot = GetPanelSlot();
         else
            subslot = iter.getslot();

         TPad* subpad = GetSlotPad(subslot);
         if (subpad == 0)
            continue;

         TGo4Slot* sislot = GetSuperimposeSlot(subslot);
         if (sislot == 0)
            continue;

         THStack* hs = dynamic_cast<THStack*>(sislot->GetAssignedObject());
         if(hs==0) continue;

         TH1* framehisto = hs->GetHistogram();
         if (framehisto == 0) continue;

         if (framehisto->GetXaxis() != obj) continue;

         TIter next(hs->GetHists());
         TH1* hs_h1 = 0;
         while ((hs_h1 = (TH1*) next()) != 0)
            hs_h1->GetXaxis()->UnZoom();

         return;
      } while (iter.next());
   }
}

void TGo4ViewPanel::CanvasUpdatedSlot()
{
   if (fxWCanvas) {
      TCanvasImp *imp = GetCanvas()->GetCanvasImp();
      fxCanvasEventstatusChk->setChecked(imp->HasStatusBar());
      fxCanvasEditorChk->setChecked(imp->HasEditor());
   } else {
      ResizeGedEditor();
   }
}

void TGo4ViewPanel::SaveCanvas()
{
   ServiceCall("SavePanelCanvas");
}

void TGo4ViewPanel::ProducePicture()
{
   TGo4Picture* pic = new TGo4Picture(GetPanelName(), "Copy of picture");

   MakePictureForPad(pic, GetCanvas(), false);

   SaveObjectInMemory("", pic);
}

void TGo4ViewPanel::ProduceGraphFromMarkers()
{
   // get list of markers here
   TObjArray markers;
   CollectSpecialObjects(GetActivePad(), &markers, kind_Marker);
   Int_t npts = markers.GetEntries();
   //std::cout <<"Found "<<npts<<" markers in pad" << std::endl;
   if (npts == 0)
      return;
   // create arrays of length
   TArrayD x(npts), y(npts);
   // copy marker values to array:
   for (Int_t j = 0; j < npts; ++j) {
      TGo4Marker* mark = dynamic_cast<TGo4Marker*>(markers[j]);
      if (mark == 0) {
         std::cout << "NEVER COME HERE: no marker at index " << j << std::endl;
         return;
      }
      x[j] = mark->GetX();
      y[j] = mark->GetY();
      //std::cout <<"Set point "<<j <<" to x="<<x[j]<<", y="<<y[j]<< std::endl;
   }

   // create graph from points array:
   TString grname = GetPanelName() + TString("-Markergraph");
   TGraph* graf = new TGraph(npts, x.GetArray(), y.GetArray());
   graf->SetName(grname.Data());
   graf->SetMarkerStyle(28);
   SaveObjectInMemory("", graf);
}

void TGo4ViewPanel::MakePictureForPad(TGo4Picture* pic, TPad* pad, bool useitemname)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   TGo4Slot* slot = GetPadSlot(pad);
   if ((padopt == 0) || (slot == 0)) return;

   pic->CopyOptionsFrom(padopt);

   pic->SetApplyToAll(fbApplyToAllFlag);
   pic->SetAutoZoom(fbAutoZoomFlag);

   if (pad == GetCanvas() && fbFreezeTitle)
      pic->SetTitle(fFreezedTitle.toLatin1().constData());

   if (!padopt->IsXYRatioOne())
      pic->SetFrameAttr(pad);

   int objnamenum = 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int kind = GetDrawKind(subslot);

      if ((kind == kind_Arrow) || (kind == kind_Latex) || (kind == kind_Marker)
            || (kind == kind_Window) || (kind == kind_Poly) || (kind == kind_Func)
            || (kind == kind_Specials)) {
         TObject* obj = subslot->GetAssignedObject();
         const char* drawopt = GetSpecialDrawOption(subslot);

         if (obj != 0) {
            if (dynamic_cast<TLatex*>(obj) || dynamic_cast<TF1*>(obj)) {
               // test here if we have local object or monitored remote one
               if (dynamic_cast<TGo4ObjectProxy*>(subslot->GetProxy())) {
                  // make clone when really needed
                  pic->AddSpecialObject(obj->Clone(), drawopt);
               } else if (dynamic_cast<TGo4LinkProxy*>(subslot->GetProxy())) {
                  const char* itemname = GetLinkedName(subslot);
                  if (itemname != 0)
                     pic->AddObjName(itemname, drawopt);
               } else {
                  TGo4Log::Error("MakePictureForPad NEVER COME HERE, unknown proxy type");
               }
            } else {
               obj = obj->Clone();

               TGo4Marker* mark = dynamic_cast<TGo4Marker*>(obj);
               if (mark != 0) mark->DeletePainter();
               TGo4Condition* cond = dynamic_cast<TGo4Condition*>(obj);
               if (cond != 0) cond->DeletePainter();

               pic->AddSpecialObject(obj, drawopt);
            }
         }
         continue;
      }

      if ((kind != kind_Link) && (kind != kind_Condition))
         continue;

      const char* drawopt = padopt->GetDrawOption(objnamenum++);

      if (useitemname) {
         const char* itemname = GetLinkedName(subslot);
         if (itemname != 0)
            pic->AddObjName(itemname, drawopt);
      } else {
         TNamed* nm = dynamic_cast<TNamed*>(subslot->GetAssignedObject());
         if (nm!=0) pic->AddObjName(nm->GetName(), drawopt);
      }

      Int_t rebinx, rebiny;
      if (subslot->GetIntPar("::HasRebinX", rebinx))
         pic->SetRebinX(rebinx);
      if (subslot->GetIntPar("::HasRebinY", rebiny))
         pic->SetRebinY(rebiny);
   }

   TObjArray pads;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TPad* subpad = GetSlotPad(subslot);
      if (subpad != 0)
         pads.Add(subpad);
   }
   if (pads.GetLast() < 0)
      return;

   double lastx = -1;
   int xcnt = 0, sizex = 1, sizey = 1;
   for (int n = 0; n <= pads.GetLast(); n++) {
      TPad* subpad = (TPad*) pads.At(n);
      double mitx = subpad->GetXlowNDC() + subpad->GetWNDC() / 2.;
      if (mitx > lastx) {
         xcnt++;
         lastx = mitx;
      } else {
         xcnt = 0;
         lastx = -1;
      }
      if (xcnt > sizex)
         sizex = xcnt;
   }

   while (sizex * sizey <= pads.GetLast())
      sizey++;

   pic->SetDivision(sizey, sizex);

   for (int ny = 0; ny < sizey; ny++)
      for (int nx = 0; nx < sizex; nx++) {
         int indx = ny * sizex + nx;
         if (indx > pads.GetLast())
            break;
         TPad* subpad = (TPad*) pads.At(indx);
         MakePictureForPad(pic->Pic(ny, nx), subpad, useitemname);
      }
}

void TGo4ViewPanel::PrintCanvas()
{
   TGo4PrintWidget dlg;
   if (dlg.exec() != QDialog::Accepted)
      return;

   go4sett->setPrinterSett(dlg.GetPrinter(), dlg.GetPrintProg());

   QString outfile = "~/go4printout.ps";
   QString PrnCmd = dlg.GetPrintProg() + " -P " + dlg.GetPrinter() + " "
         + outfile;
   QString DelCmd = QString("rm -f ") + outfile;

   GetCanvas()->Print(outfile.toLatin1().constData());
   gSystem->Exec(PrnCmd.toLatin1().constData());
   gSystem->Exec(DelCmd.toLatin1().constData());
}

void TGo4ViewPanel::StartRootEditor()
{
   bool visible = false;

   if (fxQCanvas) {

      if (!fxQCanvas->isEditorAllowed()) return;

      visible = !fxQCanvas->isEditorVisible();

      if (visible)
         SetActivePad(GetCanvas());

      fxQCanvas->toggleEditor();

   } else if (fxWCanvas) {

#ifdef GO4_WEBGUI

      // SetActivePad(GetCanvas());

      visible = !fxWCanvas->isEditorVisible();

      fxWCanvas->setEditorVisible(visible);

#endif
   }

   if (visible)
      ActivateInGedEditor(GetSelectedObject(GetActivePad(), 0));

   show();

   ResizeGedEditor();
}

void TGo4ViewPanel::StartConditionEditor()
{
   TGo4Slot* padslot = GetPadSlot(GetActivePad());
   if (padslot == 0)
      return;

   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind != kind_Condition)
         continue;
      const char* itemname = GetLinkedName(subslot);
      if (itemname != 0) {
         EditItem(itemname);
         return;
      }
   }
}

void TGo4ViewPanel::RectangularRatio(TPad *pad)
{
   if (pad == 0) return;

   double defleft =gStyle->GetPadLeftMargin(); // always refer to ideal margins
   double defright =gStyle->GetPadRightMargin();
   double deftop =gStyle->GetPadTopMargin();
   double defbottom =gStyle->GetPadBottomMargin();

   double dx = fabs(pad->AbsPixeltoX(1) - pad->AbsPixeltoX(0));
   double dy = fabs(pad->AbsPixeltoY(1) - pad->AbsPixeltoY(0));
   //std::cout <<"-- dx:"<<dx<<", dy:"<<dy<< std::endl;
   if ((dx <= 0) || (dy <= 0)) return;
   double ratio = dx / dy;
   //std::cout <<"-- Ratio is:"<<ratio<< std::endl;
   if(fabs(1.0-ratio) < 1.0E-3)
   {
     //std::cout <<"DO NOT CHANGE!"<< std::endl;
     return; // do not change margins again!
   }


   if (ratio < 1.) {

     double left = pad->GetLeftMargin();
     double right = pad->GetRightMargin();
     double change = (1. - left - right) * (1 - ratio);
     //std::cout <<"-- left:"<<left<<", right:"<<right<<", change:"<<change<< std::endl;
     double newleft = left + change / 2.;
     double newright = right + change / 2.;
     double newtop = pad->GetTopMargin();
     double newbottom = pad->GetBottomMargin();
     //std::cout << " -- newleft"<<newleft<<", newright"<<newright;
     //std::cout <<"newtop:"<<newtop<<", newbottom:"<<newbottom<< std::endl;
     double shrink=newtop- deftop; // zoom everything consistent to the default margins
     if(shrink>newleft-defleft) shrink = newleft-defleft; // avoid exceeding default boundaries
     if(shrink>newright-defright) shrink = newright-defright; // avoid exceeding default boundaries

     // now scale all margins up to the point that any margin reaches default margin:
     newtop = newtop - shrink;
     newbottom = newbottom - shrink;
     newleft = newleft - shrink;
     newright = newright - shrink;
     //std::cout << "after shrink="<<shrink<<" :";
     //std::cout << " -- newleft"<<newleft<<", newright"<<newright;
     //std::cout <<"newtop:"<<newtop<<", newbottom:"<<newbottom<< std::endl;

      pad->SetLeftMargin(newleft);
      pad->SetRightMargin(newright);
      pad->SetTopMargin(newtop);
      pad->SetBottomMargin(newbottom);
   } else {

      double bottom = pad->GetBottomMargin();
      double top = pad->GetTopMargin();
      double change = (1. - bottom - top) * (1. - 1 / ratio);
      //std::cout <<"-- left:"<<left<<", right:"<<right<<", change:"<<change<< std::endl;
      double newleft=pad->GetLeftMargin();
      double newright=pad->GetRightMargin();
      double newtop= top + change / 2.;
      double newbottom= bottom + change / 2.;
      //std::cout << " -- newleft"<<newleft<<", newright"<<newright;
      //std::cout <<"newtop:"<<newtop<<", newbottom:"<<newbottom<< std::endl;
      double shrink=newleft - defleft;
      if(shrink>newtop-deftop) shrink=newtop-deftop; // avoid exceeding default boundaries
      if(shrink>newbottom-defbottom) shrink=newbottom-defbottom; // avoid exceeding default boundaries
      // now scale all margins up to the point that any margin reaches default margin:
      newtop = newtop - shrink;
      newbottom = newbottom -shrink;
      newleft = newleft- shrink;
      newright = newright - shrink;
      //std::cout << "after shrink="<<shrink<<" :";
      //std::cout << " -- newleft"<<newleft<<", newright"<<newright;
      //std::cout <<"newtop:"<<newtop<<", newbottom:"<<newbottom<< std::endl;

      pad->SetTopMargin(newtop);
      pad->SetBottomMargin(newbottom);
      pad->SetLeftMargin(newleft);
      pad->SetRightMargin(newright);
   }
}

void TGo4ViewPanel::DefaultPadMargin(TPad *pad)
{
   if (pad == 0) return;
   pad->SetLeftMargin(gStyle->GetPadLeftMargin());
   pad->SetRightMargin(gStyle->GetPadRightMargin());
   pad->SetTopMargin(gStyle->GetPadTopMargin());
   pad->SetBottomMargin(gStyle->GetPadBottomMargin());
}

void TGo4ViewPanel::ClearActivePad()
{
   TPad* pad = GetActivePad();
   if (pad == 0)
      pad = GetCanvas();

   ClearPad(pad, true, false);

   RedrawPanel(pad, true);
}

void TGo4ViewPanel::ClearCanvas()
{
   // TGo4LockGuard glob;

   ClearPad(GetCanvas(), true, true);

   SetPadDefaults(GetCanvas());

   RedrawPanel(GetCanvas(), true);
}

void TGo4ViewPanel::AboutToShowOptionsMenu()
{
   TPad* pad = GetActivePad();
   if (pad == 0)
      pad = GetCanvas();

   TGo4Picture *padopt = GetPadOptions(pad);

   SetIdAction(fOptionsMap, StatisticsId, true, padopt->IsHisStats());
   SetIdAction(fOptionsMap, SuperimposeId, true, padopt->IsSuperimpose());
   SetIdAction(fOptionsMap, AutoZoomId, true, IsAutoZoomFlag());
   SetIdAction(fOptionsMap, SetTitleId, true, padopt->IsHisTitle());
   SetIdAction(fOptionsMap, DrawTimeId, padopt->IsHisTitle() && fbCloneFlag,
         padopt->IsTitleTime());
   SetIdAction(fOptionsMap, DrawDateId, padopt->IsHisTitle() && fbCloneFlag,
         padopt->IsTitleDate());
   SetIdAction(fOptionsMap, DrawItemnameId, padopt->IsHisTitle() && fbCloneFlag,
         padopt->IsTitleItem());
   SetIdAction(fOptionsMap, FreezeTitleId, true, fbFreezeTitle);
   SetIdAction(fOptionsMap, CrosshairId, true, fbCanvasCrosshair);
   SetIdAction(fOptionsMap, SetLegendId, true, padopt->IsLegendDraw());
   SetIdAction(fOptionsMap, AxisTimeDisplayId, true,
         padopt->IsXAxisTimeDisplay());
   SetIdAction(fOptionsMap, SetXYRatioOneId, true, padopt->IsXYRatioOne());

}

void TGo4ViewPanel::SelectMenuItemActivated(int id)
{
   TGo4LockGuard lock;
   if (id == BringToFrontId) {
      if (ShiftSelectedObjectToEnd(GetActivePad())) {
         MarkPadModified(GetActivePad());
         ShootRepaintTimer(GetActivePad());
      }
      return;
   }

   int selected = TGo4Picture::PictureIndex;

   if (id != MasterSelectId)
      selected = id - FirstSelectId;

   TGo4Slot* slot = GetPadSlot(GetActivePad());
   if (slot == 0)
      return;

   int wasselected = GetSelectedObjectIndex(slot);

   SetSelectedObjectIndex(slot, selected);

   if (selected != wasselected) {
      CallPanelFunc(panel_Updated, GetActivePad());
      UpdatePanelCaption();
   }

   if (selected != TGo4Picture::PictureIndex)
      ActivateInGedEditor(GetSelectedObject(GetActivePad(), 0));
}

void TGo4ViewPanel::ShowEventStatus()
{
   bool flag = true;

   if (fxQCanvas) {
      flag = !fxQCanvas->isStatusBarVisible();
      fxQCanvas->setStatusBarVisible(flag);
   } else if (fxWCanvas) {
#ifdef GO4_WEBGUI
      flag = !fxWCanvas->isStatusBarVisible();
      fxWCanvas->setStatusBarVisible(flag);
#endif
   }

   fxCanvasEventstatusChk->setChecked(flag);
   if (!flag)
      DisplayPadStatus(GetActivePad());
}

void TGo4ViewPanel::UpdatePadStatus(TPad* pad, bool removeitems)
{
   if (GetPadSlot(pad) == 0)
      return;

   BlockPanelRedraw(true);
   ProcessPadStatusUpdate(pad, 0, removeitems);
   BlockPanelRedraw(false);
}

void TGo4ViewPanel::ProcessPadStatusUpdate(TPad* pad, TGo4Slot* parent,
      bool removeitems)
{
   if (pad == 0)
      return;

   TGo4Slot* slot = 0;

   bool setdefaults = false;
   if (parent == 0) {
      slot = GetPadSlot(pad);
      if (slot == 0)
         return;
   } else {
      slot = parent->FindChild(pad->GetName());
      // create slot for subpad if not existing
      if (slot == 0) {
         slot = AddNewSlot(pad->GetName(), parent);
         SetDrawKind(slot, kind_PadSlot);
         setdefaults = true;
      }
   }

   SetSlotPad(slot, pad);

   TGo4Picture* padopt = GetPadOptions(slot);

   padopt->SetPadModified();

   bool issubpads = false;

   bool isdupluicate = false;

   // check if pads with duplicate names appears in list.
   // Remove first copies. Can appiar by RMB menu Divide call
   do {
      TObjArray subpads;
      isdupluicate = false;
      TIter iter(pad->GetListOfPrimitives());
      TObject* obj = 0;
      while ((obj = iter()) != 0) {
         TPad* subpad = dynamic_cast<TPad*>(obj);
         if (subpad == 0)
            continue;
         issubpads = true;
         if (subpads.FindObject(subpad->GetName()) != 0)
            isdupluicate = true;
         if (!isdupluicate)
            subpads.Add(subpad);
      }
      if (isdupluicate) {
         pad->GetListOfPrimitives()->RemoveAll(&subpads);
         subpads.Delete();
         subpads.Compress();
         padopt->SetPadModified();
      }

   } while (isdupluicate);

   // remove all subslots, which are correspond to non-existing subpads
   for (int n = slot->NumChilds() - 1; n >= 0; n--) {
      TGo4Slot* subslot = slot->GetChild(n);
      TPad* subpad = GetSlotPad(subslot);
      if (subpad != 0)
         if (pad->GetListOfPrimitives()->FindObject(subpad) == 0)
            delete subslot;
         else
            issubpads = true;
   }

   if (setdefaults)
      SetPadDefaults(pad);

   if (!issubpads)
      return;

   TIter iter(pad->GetListOfPrimitives());
   TObjArray removedItems;
   TObject* obj = 0;
   while ((obj = iter()) != 0) {
      TPad* subpad = dynamic_cast<TPad*>(obj);
      if (subpad != 0)
         ProcessPadStatusUpdate(subpad, slot, removeitems);
      else
         removedItems.Add(obj);
   }

   pad->GetListOfPrimitives()->RemoveAll(&removedItems);

   if (removeitems)
      ClearPadItems(slot, 0);
}

TGo4Slot* TGo4ViewPanel::AddDrawObject(TPad* pad, int kind, const char* itemname,
                                       TObject* obj, bool owner, const char* drawopt)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   if (padslot == 0) {
      if (owner) delete obj;
      return 0;
   }

   //std::cout << "Add object = " << (obj ? obj->ClassName() : "---") << " pad = " << pad->GetName() << " kind = " << kind << std::endl;

   // clear only if link is added
   if (kind < 100)
      ClearPad(pad, false, true);

   TGo4Slot* tgtslot = 0;

   if (kind == kind_Link) {
      TClass* cl = Browser()->ItemClass(itemname);
      if ((cl != 0) && cl->InheritsFrom(TGo4Condition::Class()))
         UndrawItem(itemname);

//      std::cout << "Item " << itemname << " class = " << (cl ? cl->GetName() : "null") << std::endl;

      TGo4Slot* brslot = Browser()->BrowserSlot(itemname);

      if (brslot != 0) {
         tgtslot = AddNewSlot(brslot->GetName(), padslot);
         SetLinkedName(tgtslot, itemname);
         if (fbCloneFlag)
            tgtslot->SetProxy(new TGo4DrawCloneProxy(brslot, this));
         else
            tgtslot->SetProxy(new TGo4LinkProxy(brslot));
      }
   } else {

      QString newslotname = itemname;
      if ((newslotname.length() == 0)
            || (padslot->FindChild(newslotname.toLatin1().constData()) != 0)) {
         int cnt = 0;
         do {
            if ((itemname == 0) || (*itemname == 0))
               newslotname = "::SpecialObject_";
            else
               newslotname = itemname;
            newslotname += QString::number(cnt++);
         } while (padslot->FindChild(newslotname.toLatin1().constData()) != 0);
      }
      tgtslot = AddNewSlot(newslotname.toLatin1().constData(), padslot);
      tgtslot->SetProxy(new TGo4ObjectProxy(obj, owner));
   }

   if (tgtslot == 0) return 0;

   if (kind < 100)
      CallPanelFunc(panel_Modified, pad);

   tgtslot->SetPar("::FirstDraw", "true");
   SetDrawKind(tgtslot, kind);
   SetSpecialDrawOption(tgtslot, drawopt);

   TGo4Picture* padopt = GetPadOptions(padslot);
   if (padopt != 0) {
      padopt->SetContentModified(true);
      padopt->SetPadModified();
      if ((kind < 100) && (drawopt != 0))
         padopt->SetDrawOption(drawopt, TGo4Picture::PictureIndex);
   }

   return tgtslot;
}

TGo4Slot* TGo4ViewPanel::GetDrawObjectSlot(TPad* pad, const char* name)
{
   TGo4Slot* slot = GetPadSlot(pad);

   return slot == 0 ? 0 : slot->FindChild(name);
}

TObject* TGo4ViewPanel::GetDrawObject(TPad* pad, const char* name)
{
   TGo4Slot* subslot = GetDrawObjectSlot(pad, name);

   return subslot == 0 ? 0 : subslot->GetAssignedObject();
}

void TGo4ViewPanel::DeleteDrawObject(TPad* pad, const char* name)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   TGo4Slot* subslot = padslot == 0 ? 0 : padslot->FindChild(name);

   if (subslot != 0) {
      delete subslot;
      TGo4Picture* padopt = GetPadOptions(padslot);
      if (padopt != 0)
         padopt->SetPadModified();
   }
}

void TGo4ViewPanel::DeleteDrawObject(TPad* pad, TObject* obj)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0)
      return;
   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      if (GetDrawKind(subslot) < 0)
         continue;
      if (subslot->GetAssignedObject() == obj) {
         delete subslot;
         break;
      }
   }
}

void TGo4ViewPanel::CollectSpecialObjects(TPad* pad, TObjArray* objs,
      int selectkind)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((slot == 0) || (objs == 0))
      return;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind < 0)
         continue;
      if ((kind != kind_Link) && ((selectkind < 0) || (kind == selectkind))) {
         TObject* obj = subslot->GetAssignedObject();
         if (obj != 0)
            objs->Add(obj);
      }
   }
}

bool TGo4ViewPanel::DeleteDrawObjects(TPad* pad, int kindtodelete)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);

   if ((slot == 0) || (padopt == 0))
      return false;

   bool res = false;

   for (int n = slot->NumChilds() - 1; n >= 0; n--) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind < 0)
         continue;
      if ((kind != kind_Link)
            && ((kindtodelete < 0) || (kind == kindtodelete))) {
         res = true;
         delete subslot;
      }
   }
   if (res)
      padopt->SetPadModified();
   return true;
}

void TGo4ViewPanel::SetPadSuperImpose(TPad* pad, bool on)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt != 0) {
      padopt->SetSuperimpose(on);
      padopt->SetPadModified();
   }
}

void TGo4ViewPanel::ProcessPadModifiedSignal()
{
   fbModifiedSignalFlag = false;
   if (GetActivePad() == 0)
      return;

   if (ScanDrawOptions(GetActivePad(), GetPadSlot(GetActivePad()),
         GetPadOptions(GetActivePad()), true))
      CallPanelFunc(panel_Updated, GetActivePad());
}

bool TGo4ViewPanel::ScanDrawOptions(TPad* pad, TGo4Slot* padslot,
      TGo4Picture* pic, bool onlyscan)
{
   TGo4LockGuard lock;

   if ((pad == 0) || (pic == 0) || (padslot == 0))
      return false;

   bool optchanged = false;
   TObjLink* link = pad->GetListOfPrimitives()->FirstLink();

   while (link != 0) {
      const char* clname = link->GetObject()->ClassName();
      if ((strcmp(clname, "TFrame") == 0) || (strcmp(clname, "TLegend") == 0)) {

         // comment out, can never be working
         //TPaveText* titl = dynamic_cast<TPaveText*>(link->GetObject());
         //if (titl &&(strcmp(titl->GetName(),"title")==0))
         //   pic->SetTitleAttr(titl);

         link = link->Next();
      } else
         break;
   }

   // take draw options from first drawn object
   if (link != 0) {

      TString newopt(link->GetOption());
      Int_t drawoptindx =
            GetSuperimposeSlot(padslot) == 0 ? 0 : TGo4Picture::PictureIndex;
      TString oldopt(pic->GetDrawOption(drawoptindx));

      while (newopt.BeginsWith(NoStackDrawOption, TString::kIgnoreCase))
         newopt.Remove(0, strlen(NoStackDrawOption));

      if (!oldopt.Contains("asimage"))
         if ((newopt.Length() != oldopt.Length())
               || (newopt.CompareTo(oldopt, TString::kIgnoreCase) != 0)) {
            optchanged = true;
            pic->SetDrawOption(newopt.Data(), drawoptindx);
         }

      TH1* h1 = dynamic_cast<TH1*>(link->GetObject());
      // access axis properties of graphs
      if(h1==0) {
         TGraph* gr=dynamic_cast<TGraph*> (link->GetObject());
         if(gr) h1=gr->GetHistogram();
      }

      if (h1 != 0) {
         TPaveStats* stats =
               dynamic_cast<TPaveStats*>(h1->GetListOfFunctions()->FindObject("stats"));
         if (h1->TestBit(TH1::kNoStats)) {
            pic->SetHisStats(kFALSE);
         } else {
            pic->SetHisStats(kTRUE);
            pic->SetStatsAttr(stats);
         }
         // test: set here time display
         TAxis* xax = h1->GetXaxis();
         pic->SetXAxisAttTime(xax->GetTimeDisplay(), xax->GetTimeFormat() ,TGo4Picture::PictureIndex);
         //std::cout <<"Set time attributes to pad options: display:"<<xax->GetTimeDisplay()<<", format:"<< xax->GetTimeFormat()<< std::endl;
      }

   }

   if (pad->GetLogx() != pic->GetLogScale(0)) {
      pic->SetLogScale(0, pad->GetLogx());
      optchanged = true;
   }

   if (pad->GetLogy() != pic->GetLogScale(1)) {
      pic->SetLogScale(1, pad->GetLogy());
      optchanged = true;
   }

   if (pad->GetLogz() != pic->GetLogScale(2)) {
      pic->SetLogScale(2, pad->GetLogz());
      optchanged = true;
   }

   if (pad == GetCanvas())
      fbCanvasCrosshair = pad->HasCrosshair();

   TPaveText* titl =
         dynamic_cast<TPaveText*>(pad->GetListOfPrimitives()->FindObject("title"));
   if(titl) {
      pic->SetTitleAttr(titl);
      optchanged = true;
   }

   TObjArray objs, objslots;

   CollectMainDrawObjects(padslot, &objs, &objslots, 0);
   ScanObjectsDrawOptions(onlyscan, padslot, &objs, &objslots);

   return optchanged;
}

void TGo4ViewPanel::ScanObjectsDrawOptions(bool onlyscan, TGo4Slot* padslot,
      TObjArray* objs, TObjArray* objslots)
{
   TGo4Picture* pic = GetPadOptions(padslot);

   TPad* pad = GetSlotPad(padslot);
   if ((pad != 0) && (pic != 0))
      if (padslot->GetPar("::DrawOptAssigned") != 0) {
         pic->SetDrawAttributes(pad, TGo4Picture::PictureIndex);
      } else if (!onlyscan) {
         pic->GetDrawAttributes(pad, TGo4Picture::PictureIndex);
         padslot->SetPar("::DrawOptAssigned", "1");
      }

   if ((padslot == 0) || (pic == 0) || (objs == 0) || (objslots == 0))
      return;

   for (int n = 0; n <= objs->GetLast(); n++) {
      TObject* obj = objs->At(n);
      TGo4Slot* subslot = (TGo4Slot*) objslots->At(n);

      if ((obj == 0) || (subslot == 0))
         continue;

      if (subslot->GetPar("::DrawOptAssigned") != 0) {
         pic->SetDrawAttributes(obj, n);
      } else if (!onlyscan) {
         pic->GetDrawAttributes(obj, n);
         subslot->SetPar("::DrawOptAssigned", "1");
      }
   }
}

void TGo4ViewPanel::CollectMainDrawObjects(TGo4Slot* slot, TObjArray* objs,
      TObjArray* objslots, int modifier)
{
   // modifier == 0 - no objects can be deleted
   //          == 1 - objects of same types should be in the list
   //          == 2 - only last object survive
   if ((slot == 0) || (objs == 0))
      return;

   TObjArray mainslots;

   int lastobjtype = 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);

      if ((kind <= 0) || (kind >= kind_Additional))
         continue;

      TObject* obj = subslot->GetAssignedObject();

      if (obj == 0)
         continue;

      int objtype = 0;

      if (obj->InheritsFrom(TH1::Class()))
         objtype = 1;
      else if (obj->InheritsFrom(TGraph::Class()))
         objtype = 2;
      else if (obj->InheritsFrom(TMultiGraph::Class()))
         objtype = 3;
      else if (obj->InheritsFrom(THStack::Class()))
         objtype = 4;

      // can happen condition here, which is add as link and not identified as condition yet
      // should both be recognized as "main" draw object
      if (objtype > 0) {
         lastobjtype = objtype;
         mainslots.Add(subslot);
      }
   }

   Bool_t deletesomething = kFALSE;

   for (int n = 0; n <= mainslots.GetLast(); n++) {
      TGo4Slot* subslot = (TGo4Slot*) mainslots.At(n);
      TObject* obj = subslot->GetAssignedObject();
      Int_t objtype = 0;
      if (obj->InheritsFrom(TH1::Class()))
         objtype = 1;
      else if (obj->InheritsFrom(TGraph::Class()))
         objtype = 2;
      else if (obj->InheritsFrom(TMultiGraph::Class()))
         objtype = 3;
      else if (obj->InheritsFrom(THStack::Class()))
         objtype = 4;

      // check if all main object correspond to type of last object
      // if no, delete

      if ((n < mainslots.GetLast())
            && ((modifier == 2) || ((modifier == 1) && (lastobjtype != objtype)))) {
         delete subslot;
         deletesomething = kTRUE;
      } else {
         objs->Add(obj);
         if (objslots != 0)
            objslots->Add(subslot);
         TGo4DrawCloneProxy* pr =
               dynamic_cast<TGo4DrawCloneProxy*>(subslot->GetProxy());
         if(pr!=0) {
            pr->UpdateTitle();
            pr->PerformRebin();
         }
      }
   }

   if (deletesomething)
      CleanupGedEditor();

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);

      Int_t kind = GetDrawKind(subslot);
      if ((kind >= kind_Additional) && (kind < kind_Specials)) {
         TObject* obj = subslot->GetAssignedObject();
         if (obj != 0) {
            objs->Add(obj);
            if (objslots != 0)
               objslots->Add(subslot);
         }
      }
   }
}

int TGo4ViewPanel::GetAutoColor(int indx) {
   if (indx<0) indx = 0;

   switch (indx % 10) {
      case 0: return kBlack;
      case 1: return kRed;
      case 2: return kGreen;
      case 3: return kBlue;
      case 4: return kCyan;
      case 5: return kOrange;
      case 6: return kSpring;
      case 7: return kViolet;
      case 8: return kPink;
      case 9: return kAzure;
   }

   return kBlack;
}


TObject* TGo4ViewPanel::ProduceSuperimposeObject(TGo4Slot* padslot, TGo4Picture* padopt,
                             TGo4Slot* sislot, TGo4Slot* legslot, TObjArray* objs,
                             TObjArray * objslots, bool showitems)
{
   if ((sislot == 0) || (objs == 0) || (padopt == 0)) return 0;
   TObject* oldobj = sislot->GetAssignedObject();

   Bool_t ishstack = kFALSE;
   Bool_t isgstack = kFALSE;
   Bool_t iserror = kFALSE;
   Bool_t resetcolors = kFALSE;

   for (int n = 0; n <= objs->GetLast(); n++) {
      TObject* obj = objs->At(n);
      if (obj->InheritsFrom(TH1::Class()))
         ishstack = kTRUE;
      else if (obj->InheritsFrom(TGraph::Class()))
         isgstack = kTRUE;
      else
         iserror = kTRUE;
   }

   // if error, no superimpose is allowed
   if (iserror || (ishstack && isgstack)) {
      TGo4Log::Error("Superimpose of multiple objects with different types");
      std::cout<< "Superimpose of multiple objects with different types"<< std::endl;
      return 0;
   }

   if (ishstack) {
      THStack* hs = dynamic_cast<THStack*>(oldobj);

      if (hs == 0) {
         hs = new THStack(objs->First()->GetName(), objs->First()->GetTitle());
         sislot->SetProxy(new TGo4DrawObjProxy(hs, kTRUE, kTRUE));
         resetcolors = kTRUE;
      } else {
         if (hs->GetHists())
            hs->GetHists()->Clear();
      }

      for (int n = 0; n <= objs->GetLast(); n++) {
         TH1* histo = (TH1*) objs->At(n);
         TGo4Slot* objslot = (TGo4Slot*) objslots->At(n);

         Int_t kind = GetDrawKind(objslot);
         Int_t objindx = padslot->GetIndexOf(objslot); // slot index for object starts from 2

         if (resetcolors || (kind == kind_FitModels) || (objslot->GetPar("::FirstDraw") != 0)) {
            histo->SetLineColor(GetAutoColor(n));
            if ((go4sett->getDrawLineWidth() > 1) && (histo->GetLineWidth()==1))
                 histo->SetLineWidth(go4sett->getDrawLineWidth());
         }




         histo->GetXaxis()->UnZoom();

         const char* drawopt = padopt->GetDrawOption(n);
         if (drawopt == 0)
            drawopt = GetSpecialDrawOption(objslot);

         objslot->RemovePar("::FirstDraw");

         hs->Add(histo, drawopt);
      }
      oldobj = hs;
   } else if (isgstack) {
      TMultiGraph* mgr = dynamic_cast<TMultiGraph*>(oldobj);
      if (mgr == 0) {
         mgr = new TMultiGraph(objs->First()->GetName(),
               objs->First()->GetTitle());
         sislot->SetProxy(new TGo4DrawObjProxy(mgr, kTRUE, kTRUE));
         resetcolors = kTRUE;
      } else if (mgr->GetListOfGraphs()) {
         mgr->GetListOfGraphs()->Clear();
      }

      for (int n = 0; n <= objs->GetLast(); n++) {
         TGraph* gr = (TGraph*) objs->At(n);
         TGo4Slot* objslot = (TGo4Slot*) objslots->At(n);

         Int_t kind = GetDrawKind(objslot);

         TString drawopt = padopt->GetDrawOption(n);
         if (drawopt.Length() == 0)
            drawopt = GetSpecialDrawOption(objslot);
         if (drawopt.Length() == 0)
            drawopt = go4sett->getTGraphDrawOpt().toLatin1().constData();

         drawopt.ToLower();

         // suppress multiple drawing of axis for subgraphs
         if (n > 0) drawopt.ReplaceAll("a", "");

         Bool_t first_draw = objslot->GetPar("::FirstDraw") != 0;

         Int_t objindx = padslot->GetIndexOf(objslot); // slot index for object starts from 2

         if (resetcolors || (kind == kind_FitModels) || first_draw) {
            // use only basic 9 colors
            Int_t nextcol = GetAutoColor(((objindx + 7) % 9));
            gr->SetLineColor(nextcol);
            gr->SetMarkerColor(nextcol);
            if ((go4sett->getDrawLineWidth() > 1) && (gr->GetLineWidth()==1))
                             gr->SetLineWidth(go4sett->getDrawLineWidth());
         }

         if (first_draw && (n==0)) {
            TAxis *ax = gr->GetXaxis();
            if ((ax!=0) && ax->GetTimeDisplay()) {
               padopt->SetHisStats(kFALSE);
               padopt->SetXAxisTimeDisplay(kTRUE);
               padopt->SetXAxisTimeFormat(ax->GetTimeFormat());
            }
         }

         objslot->RemovePar("::FirstDraw");

         mgr->Add(gr, drawopt.Data());
      }
      oldobj = mgr;
   }

   if ((ishstack || isgstack) && (legslot != 0)) {
      TLegend* legend = dynamic_cast<TLegend*>(legslot->GetAssignedObject());

      if (legend == 0) {
         double miny = 0.94 - 0.03 * objs->GetLast();
         if (miny < 0.6) miny = 0.6; else if (miny>0.92) miny = 0.92;
         legend = new TLegend(0.6, miny, 0.95, 0.99);
         legend->SetBorderSize(2);
         legend->SetName("fitlegend");
         legslot->SetProxy(new TGo4ObjectProxy(legend, kTRUE));
      } else
      legend->Clear();

      for(int n=0;n<=objs->GetLast();n++) {
         TObject* stob = objs->At(n);

         const char* objname = stob->GetName();

         if (showitems && (objslots!=0)) {
            const char* itemname = GetLinkedName((TGo4Slot*) objslots->At(n));
            if (itemname!=0) objname = itemname;
         }

         TString ldrawopt = "l";

         if (!ishstack) {
            ldrawopt = "";
            TString drawopt = padopt->GetDrawOption(n);
            drawopt.ToLower();
            if (drawopt.Contains("l")) ldrawopt+="l";
            if (drawopt.Contains("p")) ldrawopt+="p";
         }

         if (ldrawopt.Length()==0) ldrawopt = "l";

         legend->AddEntry(stob, objname, ldrawopt.Data());
      }
   }

   return oldobj;
}

void TGo4ViewPanel::Divide(int numX, int numY)
{
   TPad* pad = GetActivePad();

   TGo4Slot* padslot = GetPadSlot(pad);
   if ((pad == 0) || (padslot == 0)) return;

   ClearPad(pad, true, true);
   RedrawPanel(pad, true);

   if ((numX > 1) || (numY > 1)) {
      pad->Divide(numX, numY);

      UpdatePadStatus(pad, true);

      RedrawPanel(pad, true);
   }

   // note: number of pads will be changed in list of previewpanelstatus by InitPad
//   RefreshButtons();
}

void TGo4ViewPanel::SetSlotPad(TGo4Slot* padslot, TPad* pad)
{
   TGo4Slot* tgtslot = padslot->FindChild("::ThisPad");
   if (tgtslot == 0)
      tgtslot = new TGo4Slot(padslot, "::ThisPad", "Special object");
   tgtslot->SetProxy(new TGo4ObjectProxy(pad, kFALSE));
   SetDrawKind(tgtslot, kind_ThisPad);
   ConnectPad(pad);

   AllocatePadOptions(pad);
}

TGo4Slot* TGo4ViewPanel::GetPadSlot(TPad* pad)
{
   if (pad == GetCanvas())
      return GetPanelSlot();

   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      if (GetDrawKind(subslot) == kind_PadSlot)
         if (GetSlotPad(subslot) == pad)
            return subslot;
   }
   return 0;
}

TPad* TGo4ViewPanel::GetSlotPad(TGo4Slot* slot)
{
   if (slot == 0)
      return 0;

   TGo4Slot* tgtslot = slot->FindChild("::ThisPad");

   return tgtslot == 0 ? 0 : (TPad*) tgtslot->GetAssignedObject();
}

bool TGo4ViewPanel::IsPadHasSubPads(TGo4Slot* padslot)
{
   if (padslot == 0)
      return false;
   for (int n = 0; n < padslot->NumChilds(); n++)
      if (GetSlotPad(padslot->GetChild(n)) != 0)
         return true;
   return false;
}

bool TGo4ViewPanel::IsPadHasSubPads(TPad* pad)
{
   return IsPadHasSubPads(GetPadSlot(pad));
}

void TGo4ViewPanel::MarkPadModified(TPad* pad)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt != 0)
      padopt->SetPadModified();
}

bool TGo4ViewPanel::IsPanelPad(TPad* pad)
{
   return GetPadSlot(pad) != 0;
}

int TGo4ViewPanel::GetNumberOfPads(TPad* toppad)
{
   if (toppad == 0)
      toppad = GetCanvas();
   int number = 1;
   TGo4Iter iter(GetPadSlot(toppad), kTRUE);
   while (iter.next()) {
      TPad* pad = GetSlotPad(iter.getslot());
      if (pad != 0)
         number++;
   }
   return number;
}

TPad* TGo4ViewPanel::GetSubPad(TPad* toppad, int num, bool onlytoplevel)
{
   if (toppad == 0)
      toppad = GetCanvas();
   TGo4Slot* slot = GetPadSlot(toppad);
   if (slot == 0)
      return 0;

   int cnt = -1;
   if (!onlytoplevel || !IsPadHasSubPads(slot))
      cnt++;

   if (num == cnt)
      return toppad;

   TGo4Iter iter(slot, kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      TPad* pad = GetSlotPad(subslot);
      if (pad == 0)
         continue;
      if (!onlytoplevel || !IsPadHasSubPads(subslot))
         cnt++;
      if (num == cnt)
         return pad;
   }
   return 0;
}

void TGo4ViewPanel::ProducePadsList(TObjArray* arr, TPad* toppad)
{
   if (arr == 0)
      return;
   if (toppad == 0)
      toppad = GetCanvas();

   arr->Add(toppad);

   TGo4Iter iter(GetPadSlot(toppad), kTRUE);
   while (iter.next()) {
      TPad* pad = GetSlotPad(iter.getslot());
      if (pad != 0)
         arr->Add(pad);
   }
}

const char* TGo4ViewPanel::GetDrawObjectLinkName(TPad * pad, TObject* obj)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((pad == 0) || (slot == 0) || (obj == 0))
      return 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      if (subslot->GetAssignedObject() == obj)
         return GetLinkedName(subslot);
   }
   return 0;
}

TGo4Slot* TGo4ViewPanel::GetSuperimposeSlot(TGo4Slot* padslot)
{
   return padslot == 0 ? 0 : padslot->FindChild("::Superimpose");
}

TGo4Slot* TGo4ViewPanel::GetPadMainObjectSlot(TPad* pad)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((pad == 0) || (slot == 0))
      return 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if ((kind <= 0) || (kind >= kind_Additional))
         continue;
      TObject* obj = subslot->GetAssignedObject();
      if (obj != 0)
         return subslot;
   }
   return 0;
}

TObject* TGo4ViewPanel::GetPadMainObject(TPad* pad)
{
   TGo4Slot* slot = GetPadMainObjectSlot(pad);
   return slot == 0 ? 0 : slot->GetAssignedObject();
}

TCanvas* TGo4ViewPanel::GetCanvas()
{
   if (fxQCanvas)
      return fxQCanvas->getCanvas();
#ifdef GO4_WEBGUI
   if (fxWCanvas)
      return fxWCanvas->getCanvas();
#endif
   return 0;
}

void TGo4ViewPanel::CanvasUpdate(bool modify)
{
   if (fxQCanvas) {
      if (modify) fxQCanvas->Modified();
      fxQCanvas->Update();
   } else if (fxWCanvas) {
#ifdef GO4_WEBGUI
      if (modify) fxWCanvas->Modified();
      fxWCanvas->Update();
#endif
   }
}

TPad * TGo4ViewPanel::GetActivePad()
{
   return fxActivePad;
}

void TGo4ViewPanel::AllocatePadOptions(TPad* pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0)
      return;

   TGo4Slot* tgtslot = padslot->FindChild("::PadOptions");
   if (tgtslot == 0) {
      tgtslot = new TGo4Slot(padslot, "::PadOptions", "Pad options slot");
      TGo4Picture* opt = new TGo4Picture;
      opt->SetSuperimpose(go4sett->getPadSuperimpose());
      tgtslot->SetProxy(new TGo4ObjectProxy(opt, kTRUE));
      SetDrawKind(tgtslot, kind_PadOptions);
   }
}

TGo4Picture* TGo4ViewPanel::GetPadOptions(TPad* pad)
{
   return GetPadOptions(GetPadSlot(pad));
}

TGo4Picture* TGo4ViewPanel::GetPadOptions(TGo4Slot* padslot)
{
   if (padslot == 0)
      return 0;
   TGo4Slot* tgtslot = padslot->FindChild("::PadOptions");
   return (tgtslot == 0) ? 0 : (TGo4Picture*) tgtslot->GetAssignedObject();
}

TH1* TGo4ViewPanel::GetPadHistogram(TPad *pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   if ((pad == 0) || (padslot == 0))
      return 0;

   TObject* obj = 0;

   TGo4Slot* sislot = GetSuperimposeSlot(padslot);
   if (sislot != 0)
      obj = sislot->GetAssignedObject();

   if (obj == 0)
      obj = GetPadMainObject(pad);
   if (obj == 0)
      return 0;

   if (obj->InheritsFrom(TH1::Class()))
      return (TH1*) obj;

   TPadLock lock(fxWCanvas ? nullptr : gPad); // replace gPad to avoid redrawing of canvas

   if (obj->InheritsFrom(TGraph::Class())) {
      TGraph* gr = dynamic_cast<TGraph*>(obj);
      if (gr != 0)
         return gr->GetHistogram();
   }

   if (obj->InheritsFrom(THStack::Class())) {
      THStack* hs = dynamic_cast<THStack*>(obj);
      if (hs != 0)
         return hs->GetHistogram();
   }

   if (obj->InheritsFrom(TMultiGraph::Class())) {
      TMultiGraph* mg = dynamic_cast<TMultiGraph*>(obj);
      if (mg) {
         if (mg->GetHistogram())
            return mg->GetHistogram();

         TGraph* gr = 0;
         TIter iter(mg->GetListOfGraphs());
         while ((gr = (TGraph*) iter()) != 0)
            if (gr->GetHistogram())
               return gr->GetHistogram();
      }
   }

   return 0;
}

void TGo4ViewPanel::BlockPanelRedraw(bool on)
{
   if (on)
      fiSkipRedrawCounter++;
   else
      fiSkipRedrawCounter--;
}

bool TGo4ViewPanel::IsRedrawBlocked()
{
   return fiSkipRedrawCounter > 0;
}

void TGo4ViewPanel::UpdatePanelCaption()
{
   TGo4Slot* slot = GetPadSlot(GetActivePad());
   if (slot == 0)
      return;

   fSelectMenu->clear();

   TGo4Slot* sislot = GetSuperimposeSlot(slot);

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   Int_t nselectitem = 0;

   int selected = GetSelectedObjectIndex(slot);

   if ((selected > objs.GetLast())
         || ((selected == TGo4Picture::PictureIndex) && (sislot == 0))) {
      SetSelectedObjectIndex(slot, 0);
      selected = 0;
   }

   if (sislot != 0) {
      AddIdAction(fSelectMenu, fSelectMap, "Master object", MasterSelectId);
      fSelectMenu->addSeparator();
      nselectitem++;
   }

   QString capt = GetPanelName();

   QString fulllist = "";
   QString selslotname = "";

   for (int n = 0; n <= objslots.GetLast(); n++) {
      TGo4Slot* subslot = (TGo4Slot*) objslots.At(n);
      AddIdAction(fSelectMenu, fSelectMap, objs.At(n)->GetName(),
            FirstSelectId + n);
      nselectitem++;

      QString subslotname = subslot->GetName();
      if (n == selected) {
         subslotname = QString("[") + subslotname + QString("]");
         selslotname = subslotname;
      }
      if (n > 0) fulllist += ", ";
      fulllist += subslotname;
   }

   if ((selected != TGo4Picture::PictureIndex) && (objslots.GetLast() > 0)
         && (selected < objslots.GetLast())) {
      fSelectMenu->addSeparator();
      AddIdAction(fSelectMenu, fSelectMap,
            QString("Show ") + selslotname + QString(" on top"),
            BringToFrontId);
   }

   if ((selected == TGo4Picture::PictureIndex) && (fulllist.length() > 0))
      fulllist = QString("[") + fulllist + QString("]");

   fSelectMenu->setEnabled(nselectitem > 0);

   if (nselectitem > 0) {
      if (selected == TGo4Picture::PictureIndex)
         SetIdAction(fSelectMap, MasterSelectId, true, true);
      else
         SetIdAction(fSelectMap, FirstSelectId + selected, true, true);
   }

   if (fulllist.length() > 0) {
      capt += ": ";
      capt += fulllist;
   }

   if (capt.length() > 60) {
      capt.truncate(58);
      capt += "...";
   }

   if (!fbFreezeTitle)
      setWindowTitle(capt);
}

void TGo4ViewPanel::SetDrawKind(TGo4Slot* slot, int kind)
{
   if (slot == 0)
      return;
   if (kind <= 0)
      slot->RemovePar("::DrawKind");
   else
      slot->SetIntPar("::DrawKind", kind);
}

void TGo4ViewPanel::SetSpecialDrawOption(TGo4Slot* slot, const char* drawopt)
{
   if (slot == 0)
      return;
   if (drawopt != 0)
      slot->SetPar("::DrawOpt", drawopt);
   else
      slot->RemovePar("::DrawOpt");
}

int TGo4ViewPanel::GetDrawKind(TGo4Slot* slot)
{
   if (slot == 0)
      return -1;
   Int_t kind;
   if (!slot->GetIntPar("::DrawKind", kind))
      return -1;
   return kind;
}

const char* TGo4ViewPanel::GetSpecialDrawOption(TGo4Slot* slot)
{
   return (slot == 0) ? 0 : slot->GetPar("::DrawOpt");
}

void TGo4ViewPanel::SetSelectedObjectIndex(TGo4Slot* slot, int indx)
{
   if (slot != 0)
      slot->SetIntPar("::SelectedObject", indx);
}

int TGo4ViewPanel::GetSelectedObjectIndex(TGo4Slot* slot)
{
   if (slot == 0)
      return 0;
   Int_t indx = 0;
   if (!slot->GetIntPar("::SelectedObject", indx))
      indx = 0;
   return indx;
}

TObject* TGo4ViewPanel::GetSelectedObject(TPad * pad, const char** drawopt)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot == 0) || (padopt == 0))
      return 0;

   int indx = GetSelectedObjectIndex(slot);

   TGo4Slot* sislot = GetSuperimposeSlot(slot);
   if ((sislot != 0) && (indx == TGo4Picture::PictureIndex)) {
      if (drawopt != 0)
         *drawopt = padopt->GetDrawOption(TGo4Picture::PictureIndex);
      return sislot->GetAssignedObject();
   }

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   if (objs.GetLast() < 0)
      return 0;

   if ((indx > objs.GetLast()) || (indx < 0))
      indx = 0;

   if (drawopt != 0) {
      const char* resopt = padopt->GetDrawOption(indx);
      if (resopt == 0)
         resopt = GetSpecialDrawOption((TGo4Slot*) objslots.At(indx));

      *drawopt = resopt;
   }

   return objs.At(indx);
}

bool TGo4ViewPanel::ShiftSelectedObjectToEnd(TPad * pad)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot == 0) || (padopt == 0))
      return false;

   int indx = GetSelectedObjectIndex(slot);

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   // no sense to shift object which is already at the end
   if ((objs.GetLast() <= 0) || (indx == objs.GetLast()))
      return false;

   TGo4Slot* selslot = (TGo4Slot*) objslots.At(indx);
   TGo4Slot* lastslot = (TGo4Slot*) objslots.Last();

   if (!slot->ShiftSlotAfter(selslot, lastslot))
      return false;

   SetSelectedObjectIndex(slot, objs.GetLast());

   return true;
}

void TGo4ViewPanel::CheckObjectsAssigments(TPad * pad, TGo4Slot * padslot)
{
   if ((pad == 0) || (padslot == 0)) return;

   TObjArray objs, objslots;
   CollectMainDrawObjects(padslot, &objs, &objslots, 0);

   int indx = GetSelectedObjectIndex(padslot);
   if (indx < 0) indx = 0;

   TH1* selhisto = dynamic_cast<TH1*>((indx <= objs.GetLast()) ? objs.At(indx) : 0);
   if (selhisto == 0)
      selhisto = GetPadHistogram(pad);

   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      TObject* obj = subslot->GetAssignedObject();
      if (obj == 0) continue;

      TGo4Marker* mark = 0;
      TGo4Condition* cond = 0;

      // reset condition and marker histogram
      if ((kind >= kind_Specials) && (kind < kind_Other)) {
         if (obj->InheritsFrom(TGo4Condition::Class()))
            cond = (TGo4Condition*) obj;
         else if (obj->InheritsFrom(TGo4Marker::Class()))
            mark = (TGo4Marker*) obj;
      } else if (kind == kind_Condition)
         cond = dynamic_cast<TGo4Condition*>(obj);

      TH1* oldhisto = 0;

      if (cond != 0)
         oldhisto = cond->GetWorkHistogram();
      else if (mark != 0)
         oldhisto = mark->GetHistogram();

      if (oldhisto != 0)
         if (objs.FindObject(oldhisto) == 0)
            oldhisto = 0;

      if (oldhisto == 0)
         if (cond != 0)
         {
           //std::cout<< "CheckObjectsAssigments sets work histogram:"<<(long) selhisto<<"("<<selhisto->GetName()<<") to condition:"<<(long) cond<<" ("<<cond->GetName()<<")" << std::endl;
           cond->SetWorkHistogram(selhisto);

         }
         else if (mark != 0)
            mark->SetHistogram(selhisto);
   }
}

void TGo4ViewPanel::SetFreezedTitle(const QString& title)
{
   QString mycaption = GetPanelName();
   mycaption += ": ";
   mycaption += title;
   setWindowTitle(mycaption);
   fbFreezeTitle = true;
   fFreezedTitle = title;
}

void TGo4ViewPanel::CheckForSpecialObjects(TPad *pad, TGo4Slot* padslot)
{
   if ((pad == 0) || (padslot == 0))
      return;

   TGo4Picture* pic = 0;
   TGo4Slot* picslot = 0;
   TCanvas* canv = 0;
   TGo4Slot* canvslot = 0;

   int numcond = 0;

   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind == kind_Condition)
         numcond++;

      TObject* obj = subslot->GetAssignedObject();
      if (obj == 0)
         continue;

      if ((kind < 0) || (kind >= 100))
         continue;

      // change drawkind of condition which is drawn as normal object
      if (obj->InheritsFrom(TGo4Condition::Class())) {
         numcond++;
         TGo4Condition* cond = static_cast<TGo4Condition*>(obj);
         cond->SetLineColor(GetAutoColor(numcond));
         cond->SetFillColor(GetAutoColor(numcond));
         cond->SetFillStyle(3444);
         SetDrawKind(subslot, kind_Condition);
         continue;
      }

      if (obj->InheritsFrom(TLatex::Class())) {
         SetDrawKind(subslot, kind_Latex);
         continue;
      }
      // JAM test
      if (obj->InheritsFrom(TF1::Class())) {
        SetDrawKind(subslot, kind_Func);
        continue;
           }
      if (pic == 0) {
         pic = dynamic_cast<TGo4Picture*>(obj);
         if (pic != 0)
            picslot = subslot;
      }

      if (canv == 0) {
         canv = dynamic_cast<TCanvas*>(obj);
         if (canv != 0)
            canvslot = subslot;
      }
   }

   if (pic != 0) {

      ClearPadItems(padslot, picslot);

      // remove all subpads if any but do not remove items while picture is there
      ClearPad(pad, false, true);

      ProcessPictureRedraw(GetLinkedName(picslot), pad, pic);

      if (pad == GetCanvas())
         SetFreezedTitle(pic->GetTitle());

      // remove picture from the pad
      delete picslot;

      return;
   }

   if (canv != 0) {
      // remove all other
      ClearPadItems(padslot, canvslot);

      // remove all subpads if any but do not remove items while picture is there
      ClearPad(pad, false, true);

      TVirtualPad* padsav = gPad;

      ProcessCanvasAdopt(pad, canv, GetLinkedName(canvslot));

      if (padsav != 0)
         padsav->cd();

      delete canvslot;
   }
}

void TGo4ViewPanel::ProcessPictureRedraw(const char* picitemname, TPad* pad, TGo4Picture* pic)
{
   if ((pad == 0) || (pic == 0)) return;

   if (pic->IsDivided()) {
      pad->Divide(pic->GetDivX(), pic->GetDivY());

      // this create appropriate entries in the OM
      UpdatePadStatus(pad, false);

      for (Int_t posy = 0; posy < pic->GetDivY(); posy++)
         for (Int_t posx = 0; posx < pic->GetDivX(); posx++) {
            TGo4Picture* sub = pic->FindPic(posy, posx);
            if (sub != 0)
               ProcessPictureRedraw(picitemname,
                     (TPad*) pad->GetPad(posy * pic->GetDivX() + posx + 1),
                     sub);
         }

      return;
   }

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt == 0) {
      TGo4Log::Error("Internal problem in view panel redraw");
      return;
   }

   pic->GetFrameAttr(pad); // do it only once, pad preserves automatically

   SetApplyToAllFlag(pic->IsApplyToAll());

   SetAutoZoomFlag(pic->IsAutoZoom());

   padopt->CopyOptionsFrom(pic);

   padopt->GetDrawAttributes(pad, TGo4Picture::PictureIndex);

   TGo4BrowserProxy* brcont = Browser();

   Option_t* drawopt = pic->GetDrawOption(TGo4Picture::PictureIndex);
   if (drawopt != 0)
      pic->SetDrawOption(drawopt, TGo4Picture::PictureIndex);

   Int_t ndraw = 0;

   for (Int_t n = 0; n < pic->GetNumObjNames(); n++) {
      Option_t* drawopt = pic->GetDrawOption(n);
      if (drawopt != 0)
         pic->SetDrawOption(drawopt, n);

      const char* objname = pic->GetObjName(n);

      TString drawname;

      if (brcont->DefineRelatedObject(picitemname, objname, drawname)) {

         TGo4Slot* slot = AddDrawObject(pad, kind_Link, drawname.Data(), 0, false, 0);

         brcont->GetBrowserObject(drawname.Data(), go4sett->getFetchDataWhenDraw() ? 2 : 1);
         ndraw++;

         if (pic->GetRebinX(n) > 1) {
            slot->SetIntPar("::DoRebinX", pic->GetRebinX(n));
            slot->SetIntPar("::HasRebinX", pic->GetRebinX(n));
         }

         if (pic->GetRebinY(n) > 1) {
            slot->SetIntPar("::DoRebinY", pic->GetRebinY(n));
            slot->SetIntPar("::HasRebinY", pic->GetRebinY(n));
         }
      }
   }

   if (ndraw > 1)
      padopt->SetSuperimpose(kTRUE);

   TListIter iter(pic->GetSpecialObjects());
   TObject* obj = 0;
   while ((obj = iter()) != 0) {
      Option_t* drawopt = iter.GetOption();
      if (dynamic_cast<TArrow*>(obj) != 0)
         AddMarkerObj(pad, kind_Arrow, obj->Clone());
      else if (dynamic_cast<TLatex*>(obj) != 0)
         AddMarkerObj(pad, kind_Latex, obj->Clone());
      else if (dynamic_cast<TGo4Marker*>(obj) != 0)
         AddMarkerObj(pad, kind_Marker, obj->Clone());
      else if (dynamic_cast<TGo4WinCond*>(obj) != 0)
         AddMarkerObj(pad, kind_Window, obj->Clone());
      else if (dynamic_cast<TGo4PolyCond*>(obj) != 0)
         AddMarkerObj(pad, kind_Poly, obj->Clone());
      else if (dynamic_cast<TPaveLabel*>(obj) != 0)
         AddDrawObject(pad, kind_Specials, obj->GetName(), obj->Clone(), kTRUE,
               drawopt ? drawopt : "br");
      else
         AddDrawObject(pad, kind_Specials, obj->GetName(), obj->Clone(), kTRUE,
               drawopt);
   }

   padopt->SetPadModified();
}

void TGo4ViewPanel::ProcessCanvasAdopt(TPad* tgtpad, TPad* srcpad,
      const char* srcpaditemname)
{
   if ((tgtpad == 0) || (srcpad == 0)) return;

   TGo4Slot* padslot = GetPadSlot(tgtpad);

   TGo4Picture* padopt = GetPadOptions(tgtpad);

   if ((padopt == 0) || (padslot == 0)) return;

   tgtpad->SetTickx(srcpad->GetTickx());
   tgtpad->SetTicky(srcpad->GetTicky());
   tgtpad->SetGridx(srcpad->GetGridx());
   tgtpad->SetGridy(srcpad->GetGridy());
   tgtpad->SetBorderSize(srcpad->GetBorderSize());
   tgtpad->SetBorderMode(srcpad->GetBorderMode());
   srcpad->TAttLine::Copy(*tgtpad);
   srcpad->TAttFill::Copy(*tgtpad);
   srcpad->TAttPad::Copy(*tgtpad);

   tgtpad->SetPhi(srcpad->GetPhi());
   tgtpad->SetTheta(srcpad->GetTheta());

   int nsubpads = 0, nmain = 0, mainkind = 0;
   TObjLink* link = srcpad->GetListOfPrimitives()->FirstLink();
   while (link != 0) {
      TObject* obj = link->GetObject();
      const char* drawopt = link->GetOption();

      TH1* h1 = 0;

      int kind = 0;

      TPad* srcsubpad = dynamic_cast<TPad*>(obj);

      TString itemname = TString::Format("%s/%s", srcpaditemname,
            obj->GetName());

      if (srcsubpad != 0) {
         nsubpads++;
         QString subpadname = tgtpad->GetName();
         subpadname += "_";
         subpadname += QString::number(nsubpads);

         Double_t xlow, ylow, xup, yup;

         srcsubpad->GetPadPar(xlow, ylow, xup, yup);

         tgtpad->cd();
         TPad* tgtsubpad = new TPad(subpadname.toLatin1().constData(),
               srcsubpad->GetName(), xlow, ylow, xup, yup);
         tgtsubpad->SetNumber(nsubpads);
         tgtsubpad->Draw();

         TGo4Slot* subpadslot = AddNewSlot(tgtsubpad->GetName(), padslot);
         SetDrawKind(subpadslot, kind_PadSlot);
         SetSlotPad(subpadslot, tgtsubpad);

         ProcessCanvasAdopt(tgtsubpad, srcsubpad, itemname.Data());
      } else if (dynamic_cast<TGo4Condition*>(obj) != 0) {
         TGo4Condition* cond = (TGo4Condition*) obj->Clone();
         cond->SetWorkHistogram(0);
         AddDrawObject(tgtpad, kind_Condition, cond->GetName(), cond, kTRUE, 0);
      } else if (dynamic_cast<TGo4Marker*>(obj) != 0) {
         TGo4Marker* mark = (TGo4Marker*) obj->Clone();
         mark->SetHistogram(0);
         AddDrawObject(tgtpad, kind_Marker, mark->GetName(), mark, kTRUE, 0);
      } else if (dynamic_cast<TLatex*>(obj) != 0) {
         AddDrawObject(tgtpad, kind_Latex, obj->GetName(), obj->Clone(), kTRUE,
               0);
      } else if (dynamic_cast<TPaveLabel*>(obj) != 0) {
         AddDrawObject(tgtpad, kind_Specials, obj->GetName(), obj->Clone(),
               kTRUE, drawopt ? drawopt : "br");
      } else if (dynamic_cast<TArrow*>(obj) != 0) {
         AddDrawObject(tgtpad, kind_Arrow, obj->GetName(), obj->Clone(), kTRUE,
               0);
      } else

      if (dynamic_cast<TH1*>(obj) != 0) {
         kind = 1;
         h1 = (TH1*) obj;
      } else

      if (dynamic_cast<TGraph*>(obj) != 0) {
         kind = 2;
         h1 = ((TGraph*) obj)->GetHistogram();
      } else

      if (dynamic_cast<THStack*>(obj) != 0) {
         kind = 3;
         h1 = ((THStack*) obj)->GetHistogram();
      } else

      if (dynamic_cast<TMultiGraph*>(obj) != 0) {
         kind = 4;
         h1 = ((TMultiGraph*) obj)->GetHistogram();
      } else {
//         std::cout << tgtpad->GetName() << ":  Add other object ???" << obj->GetName() << std::endl;
      }

      // only first object is added,
      // make superimpose only for histos and graphs
      if ((kind > 0) && ((mainkind == 0) || (kind == mainkind) && (kind < 3))) {

         if (drawopt != 0)
            padopt->SetDrawOption(drawopt, nmain);

         AddDrawObject(tgtpad, knd_Reference, obj->GetName(), obj, false, 0);

         mainkind = kind;

         if ((h1 != 0) && (nmain == 0)) {
            TakeFullRangeFromHisto(h1, padopt, nmain == 0);
            Int_t ndim = h1->GetDimension();
            TakeSelectedAxisRange(0, padopt, h1->GetXaxis());
            if (ndim > 1)
               TakeSelectedAxisRange(1, padopt, h1->GetYaxis());
            if (ndim > 2)
               TakeSelectedAxisRange(2, padopt, h1->GetZaxis());
            if (ndim < 3) {
               Double_t selmin = h1->GetMinimum();
               Double_t selmax = h1->GetMaximum();

               if (selmin < selmax)
                  padopt->SetRange(ndim, selmin, selmax);

               padopt->SetAutoScale(
                     !h1->TestBit(TH1::kIsZoomed) || (selmin >= selmax));
            }
         }

         nmain++;
      }

      link = link->Next();
   }

   if (nmain > 1)
      padopt->SetSuperimpose(kTRUE);

   if (nsubpads == 0)
      ScanDrawOptions(srcpad, padslot, padopt, false);
}

void TGo4ViewPanel::RedrawPanel(TPad* pad, bool force)
{
   if (IsRedrawBlocked()) return;

   TGo4LockGuard lock;

   BlockPanelRedraw(true);

   // JAM2016: does this help for some array conflicts in TGraph painter? YES!
   // must be done once before pad->Clear in ProcessPadRedraw, so we do it here instead for each subpad
#ifdef GLOBALESCAPE
   if (fxQCanvas) {
      gROOT->SetEscape();
      fxQCanvas->HandleInput(kButton1Up, 0, 0);
      fxQCanvas->HandleInput(kMouseMotion, 0, 0); // stolen from TRootEmbeddedCanvas::HandleContainerKey
      // SL2016 - one need to reset escape status back, some other functionality (like zooming) may not work
      gROOT->SetEscape(kFALSE);
   }

#else
   // JAM 2016: only reset crucial static arrays in TGraph subclasses, do not escape complete root...
   // turned out to have strange side effects with TBox which also has static variables reacting on escape flag :(
   if(fxQCanvas) { // just for the scope to delete dummy graph on stack
      gPad = pad;
      Bool_t oldmodified = gPad->IsModified();
      TGraph dummy(1);
      gROOT->SetEscape();
      dummy.ExecuteEvent(kButton1Up, 0, 0); // will reset escape flag internally
      gPad->Modified(oldmodified); // probably not necessary, since we only use escape mode in event handler. But who knows what future root brings?
   }
#endif

   bool isanychildmodified = false;
   bool ispadupdatecalled = false;

   QTime starttm = QTime::currentTime();
   bool intime = true;


   // this loop done to allow consequent update of multi-pad canvas
   // each subpad separately redrawn in ProcessPadRedraw() method and
   // than pad->Update() is called. If it takes too long,
   // loop is finishing and via paint timer will be activated later

   do {
      TPad* selpad = TGo4MdiArea::Instance()->GetSelectedPad();

      isanychildmodified = ProcessPadRedraw(pad, force);

      TGo4MdiArea::Instance()->SetSelectedPad(selpad);

      // here pad->Update should redraw only modified subpad
      if (isanychildmodified) {
         CanvasUpdate();
         ispadupdatecalled = true;
      }

      int delay = starttm.msecsTo(QTime::currentTime());
      intime = (delay >= 0) && (delay < 100);

   } while (!force && isanychildmodified && intime);

   if (GetActivePad() != 0)
      UpdatePanelCaption();

   RefreshButtons();

   // to correctly select active pad, one should call canvas->Update()
   if (!ispadupdatecalled)
      CanvasUpdate();

   SetApplyToAllFlag(fbApplyToAllFlag);

   BlockPanelRedraw(false);

   if (!force && isanychildmodified)
      ShootRepaintTimer(pad);
}

bool TGo4ViewPanel::ProcessPadRedraw(TPad* pad, bool force)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0) return false;

   TGo4Picture* padopt = GetPadOptions(slot);
   if (padopt == 0) return false;

   bool ischilds = false;
   bool ischildmodified = false;

   CheckObjectsAssigments(pad, slot);

   CheckForSpecialObjects(pad, slot);

   // this parameter ensure that all pads will be scanned one after another
   Int_t lastdrawnpad = 0;
   if (!force)
      if (!slot->GetIntPar("::LastDrawnPad", lastdrawnpad))
         lastdrawnpad = 0;

   Int_t subpadindx = 0;

   // first redraw all subpads
   for (int n = 0; n < slot->NumChilds(); n++) {
      subpadindx = (n + lastdrawnpad) % slot->NumChilds();
      TPad* subpad = GetSlotPad(slot->GetChild(subpadindx));
      if (subpad == 0) continue;
      ischilds = true;
      if (ProcessPadRedraw(subpad, force)) {
         ischildmodified = true;
         if (!force) break; // break if any of child is modified
      }
   }

   if (!force && ischildmodified)
      slot->SetIntPar("::LastDrawnPad", subpadindx);
   else
      slot->RemovePar("::LastDrawnPad");

   if (!force && !padopt->IsPadModified())
      return ischildmodified;

   bool updatecontent = padopt->IsContentModified();

   padopt->SetPadModified(false);
   padopt->SetContentModified(false);

   // do not draw anything else if subpads are there
   if (ischilds) return ischildmodified;

   pad->Clear();

   pad->SetCrosshair(fbCanvasCrosshair);
   pad->SetLogx(padopt->GetLogScale(0));
   pad->SetLogy(padopt->GetLogScale(1));
   pad->SetLogz(padopt->GetLogScale(2));

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots,
         padopt->IsSuperimpose() ? 1 : 2);

   ScanObjectsDrawOptions(false, slot, &objs, &objslots);

   TGo4Slot* sislot = GetSuperimposeSlot(slot);
   TGo4Slot* legslot = slot->FindChild("::Legend");
   TGo4Slot* asislot = slot->FindChild("::ASImage");

   // if nothing to draw, delete all additional slots and exit
   if (objs.GetLast() < 0) {
      delete sislot;
      delete legslot;
      delete asislot;

      RedrawSpecialObjects(pad, slot);

      CallPanelFunc(panel_Updated, pad);

      // indicate that nothing is happen
      return true;
   }

   TH2* asihisto = 0;

   TObject* drawobj = 0;

   // Bool_t dosuperimpose = padopt->IsSuperimpose() && (objs.GetLast()>0);
   Bool_t dosuperimpose = objs.GetLast() > 0;

   const char* drawopt = padopt->GetDrawOption(0);

   Bool_t doasiimage = !dosuperimpose && !fxWCanvas &&
                        objs.Last()->InheritsFrom(TH2::Class());

   if (drawopt && TString(drawopt).Contains("asimage")) {
      if (!doasiimage) { drawopt = "col"; padopt->SetDrawOption("col", 0); }
   } else {
      doasiimage = kFALSE;
   }

   if (dosuperimpose) {
      if (sislot == 0)
         sislot = new TGo4Slot(slot, "::Superimpose", "place for superimpose object");
      if (padopt->IsLegendDraw()) {
         if (legslot == 0)
            legslot = new TGo4Slot(slot, "::Legend", "place for legends object");
      } else
      if (legslot != 0) {
         delete legslot;
         legslot = 0;
      }

      drawobj = ProduceSuperimposeObject(slot, padopt, sislot, legslot, &objs,
                                         &objslots, padopt->IsTitleItem());
      if (drawobj == 0)
         dosuperimpose = kFALSE;
   }

   if (!dosuperimpose) {
      if (sislot != 0) {
         delete sislot;
         sislot = 0;
      }
      if (legslot != 0) {
         delete legslot;
         legslot = 0;
      }
      drawobj = objs.Last();
   }

   if (doasiimage) {
      asihisto = dynamic_cast<TH2*>(drawobj);
      if (asihisto == 0)
         doasiimage = false;
      else {
         if (asislot == 0)
            asislot = new TGo4Slot(slot, "::ASImage", "place for Go4 ASI image");
         TGo4ASImage* image =
               dynamic_cast<TGo4ASImage*>(asislot->GetAssignedObject());
         if(image==0) {
            image = new TGo4ASImage;
            asislot->SetProxy(new TGo4ObjectProxy(image, kTRUE));
            updatecontent = true;
         }
         if (updatecontent)
            image->SetHistogramContent(asihisto);

         drawobj = image;
      }
   }

   if (!doasiimage) {
      if (asislot != 0) {
         delete asislot;
         asislot = 0;
      }
   }

   gPad = pad; // instead of pad->cd(), while it is redraw frame
   if (drawobj != 0) {
      bool first_draw = (slot->GetPar("::PadFirstDraw") == 0);
      if (first_draw) slot->SetPar("::PadFirstDraw", "true");

      if (drawobj->InheritsFrom(TH1::Class())) {
         TH1* h1 = (TH1*) drawobj;
         h1->SetBit(kCanDelete, kFALSE);
         RedrawHistogram(pad, padopt, h1, updatecontent, first_draw);
      } else if (drawobj->InheritsFrom(THStack::Class())) {
         THStack* hs = (THStack*) drawobj;
         RedrawStack(pad, padopt, hs, dosuperimpose, updatecontent);
      } else if (drawobj->InheritsFrom(TGraph::Class())) {
         TGraph* gr = (TGraph*) drawobj;
         RedrawGraph(pad, padopt, gr, updatecontent, first_draw);
      } else if (drawobj->InheritsFrom(TMultiGraph::Class())) {
         TMultiGraph* mg = (TMultiGraph*) drawobj;
         RedrawMultiGraph(pad, padopt, mg, dosuperimpose, updatecontent);
      } else if (dynamic_cast<TGo4ASImage*>(drawobj) != 0) {
         TGo4ASImage* ai = (TGo4ASImage*) drawobj;
         RedrawImage(pad, padopt, ai, asihisto, updatecontent);
      }
   }

   if (legslot != 0)
      RedrawLegend(pad, padopt, legslot);

   if (!doasiimage)
      RedrawSpecialObjects(pad, slot);

   CallPanelFunc(panel_Updated, pad);

   return true;
}

void TGo4ViewPanel::RedrawHistogram(TPad *pad, TGo4Picture* padopt, TH1 *his, bool scancontent, bool first_draw)
{
   if ((pad == 0) || (padopt == 0) || (his == 0)) return;

   if (scancontent)
      TakeFullRangeFromHisto(his, padopt, true);

   TString drawopt(padopt->GetDrawOption(0));
   if (drawopt.Length() == 0)
      if (his->GetDimension() == 1)
         drawopt = go4sett->getTH1DrawOpt().toLatin1().constData();
      else if (his->GetDimension() == 2)
         drawopt = go4sett->getTH2DrawOpt().toLatin1().constData();
      else if (his->GetDimension() == 3)
         drawopt = go4sett->getTH3DrawOpt().toLatin1().constData();

   drawopt.ToUpper();

   if (first_draw && (go4sett->getDrawLineWidth() > 1) && (his->GetLineWidth()==1))
      his->SetLineWidth(go4sett->getDrawLineWidth());

   // only activate panel defaults if no fill color set by user:
   if (first_draw && (go4sett->getDrawFillColor()>0) && (his->GetFillColor()==0))
     his->SetFillColor(go4sett->getDrawFillColor());
   if (first_draw && (go4sett->getDrawFillStyle()!=1001) && (his->GetFillStyle()==1001))
     his->SetFillStyle(go4sett->getDrawFillStyle());

   Int_t nlvl = padopt->GetHisContour();
   if (nlvl > 0) his->SetContour(nlvl);

   his->SetStats(padopt->IsHisStats());
   his->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   his->Draw(drawopt.Data());

   SetSelectedRangeToHisto(pad, his, 0, padopt, true);
}

void TGo4ViewPanel::RedrawStack(TPad *pad, TGo4Picture* padopt, THStack * hs,
                                bool dosuperimpose, bool scancontent)
{
   if ((pad == 0) || (padopt == 0) || (hs == 0)) return;

   if (scancontent) {
      TIter iter(hs->GetHists());
      TH1 *h1 = 0;
      bool first = true;
      while ((h1 = (TH1*) iter()) != 0) {
         TakeFullRangeFromHisto(h1, padopt, first);
         first = false;
      }
   }

   // never draw statistics with stack

   Int_t drawoptindx = dosuperimpose ? TGo4Picture::PictureIndex : 0;
   TString drawopt(padopt->GetDrawOption(drawoptindx));
   if (drawopt.Length() == 0)
      drawopt = "hist";
   if (!drawopt.Contains(NoStackDrawOption, TString::kIgnoreCase))
      drawopt.Prepend(NoStackDrawOption);

   hs->Draw(drawopt.Data());
   TH1* framehisto = hs->GetHistogram();
   if (framehisto == 0) return;

   framehisto->SetStats(false);
   framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   TH1* h1 = dynamic_cast<TH1*>(hs->GetHists() ? hs->GetHists()->First() : 0);
   if (h1!=0) {
      hs->SetTitle(h1->GetTitle());
      framehisto->SetTitle(h1->GetTitle());
      framehisto->GetXaxis()->SetTitle(h1->GetXaxis()->GetTitle());
      framehisto->GetYaxis()->SetTitle(h1->GetYaxis()->GetTitle());
      framehisto->GetZaxis()->SetTitle(h1->GetZaxis()->GetTitle());
   }

   SetSelectedRangeToHisto(pad, framehisto, hs, padopt, false);
}

void TGo4ViewPanel::RedrawGraph(TPad *pad, TGo4Picture* padopt, TGraph * gr, bool scancontent, bool first_draw)
{
   if ((pad == 0) || (padopt == 0) || (gr == 0)) return;

   if (scancontent) {
      TakeFullRangeFromGraph(gr, padopt, true);
      gr->SetEditable(kFALSE);
   }

   TString drawopt = padopt->GetDrawOption(0);

   // when graph drawn for the first time, check if time units used in axis
   if (first_draw) {
      TAxis *ax = gr->GetXaxis();
      if ((ax!=0) && ax->GetTimeDisplay()) {
         padopt->SetHisStats(kFALSE);
         padopt->SetXAxisTimeDisplay(kTRUE);
         padopt->SetXAxisTimeFormat(ax->GetTimeFormat());
         if (drawopt.Length() == 0) {
            drawopt = go4sett->getTGraphDrawOpt().toLatin1().constData();
            padopt->SetDrawOption(drawopt);
         }
      }
      if ((go4sett->getDrawLineWidth() > 1) && (gr->GetLineWidth()==1))
         gr->SetLineWidth(go4sett->getDrawLineWidth());


      // only activate panel defaults if no fill color set by user:
         if ((go4sett->getDrawFillColor()>0) && (gr->GetFillColor()==0))
           gr->SetFillColor(go4sett->getDrawFillColor());
         if ((go4sett->getDrawFillStyle()!=1001) && (gr->GetFillStyle()==1001))
           gr->SetFillStyle(go4sett->getDrawFillStyle());



   }

   if (drawopt.Length() == 0)
      drawopt = go4sett->getTGraphDrawOpt().toLatin1().constData();

   TH1* framehisto = gr->GetHistogram();
   if (framehisto == 0) {
      gr->Draw(drawopt.Data());
      framehisto = gr->GetHistogram();
   }
   if (framehisto != 0) {
      framehisto->SetStats(padopt->IsHisStats());
      framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   }
   gr->Draw(drawopt.Data());

   SetSelectedRangeToHisto(pad, framehisto, 0, padopt, false);
}

void TGo4ViewPanel::RedrawMultiGraph(TPad *pad, TGo4Picture* padopt,
      TMultiGraph * mg, bool dosuperimpose, bool scancontent)
{
   if ((pad == 0) || (padopt == 0) || (mg == 0)) return;

   TIter iter(mg->GetListOfGraphs());
   TGraph *gr(0), *firstgr(0);
   bool first = true;
   while ((gr = (TGraph*) iter()) != 0) {
      if (scancontent) {
         gr->SetEditable(kFALSE);
         TakeFullRangeFromGraph(gr, padopt, first);
      }
      if (first)
         firstgr = gr;
      first = false;
   }

   Int_t drawoptindx = dosuperimpose ? TGo4Picture::PictureIndex : 0;
   TString drawopt(padopt->GetDrawOption(drawoptindx));
   if (drawopt.Length() == 0)
      drawopt = go4sett->getTGraphDrawOpt().toLatin1().constData();
   if (dosuperimpose)
      drawopt = "";

   TH1* framehisto = (dosuperimpose && (firstgr != 0)) ? firstgr->GetHistogram() : mg->GetHistogram();

   if (framehisto == 0) {
      // this is workaround to prevent recreation of framehistogram in TMultiGraph::Paint
      mg->Draw(drawopt.Data());
      framehisto = (dosuperimpose && (firstgr != 0)) ? firstgr->GetHistogram() : mg->GetHistogram();
   }

   if (framehisto == 0) {
      TGo4Log::Error("Internal problem with MultiGraph drawing - cannot access frame histo");
      return;
   }

   SetSelectedRangeToHisto(pad, framehisto, 0, padopt, false);

   // try to avoid flicker of range when in fullscale: set range before and after draw
   Double_t miny, maxy, selmin, selmax;
   if (padopt->GetFullRange(1, miny, maxy) && !padopt->GetRangeY(selmin, selmax)) {
      if (padopt->GetLogScale(1)) {
         if (maxy <= 0) maxy = 1.;
         if ((miny <= 0) || (miny >= maxy)) {
            miny = maxy * 1e-4;
            if (miny > 1.) miny = 1.;
         }
      } else {
         maxy *= ((maxy>0) ? 1.1 : 0.9);
         miny *= ((miny>0) ? 0.9 : 1.1);
      }
      framehisto->SetMaximum(maxy);
      framehisto->SetMinimum(miny);
   }

   framehisto->SetStats(kFALSE); // never draw statistics with multigraph
   framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());

   // set title of first TGraph to TMultiGraph and frame histo
   if (firstgr != 0) {
      mg->SetTitle(firstgr->GetTitle());
      framehisto->SetTitle(firstgr->GetTitle());
      framehisto->GetXaxis()->SetTitle(firstgr->GetXaxis()->GetTitle());
      framehisto->GetYaxis()->SetTitle(firstgr->GetYaxis()->GetTitle());
   }

   mg->Draw(drawopt.Data());
}

void TGo4ViewPanel::RedrawImage(TPad *pad, TGo4Picture* padopt, TGo4ASImage* im,
                                TH2* asihisto, bool scancontent)
{
   if ((pad == 0) || (padopt == 0) || (im == 0)) return;

   im->SetDrawData(asihisto, this, pad);

   if (scancontent)
      TakeFullRangeFromHisto(asihisto, padopt, true);

   TString drawopt(padopt->GetDrawOption(0));

   double uminx, umaxx, uminy, umaxy;
   padopt->GetRange(0, uminx, umaxx);
   padopt->GetRange(1, uminy, umaxy);
   im->SetSelectedRange(uminx, umaxx, uminy, umaxy);

   im->SetPaletteEnabled(drawopt.Contains("Z"));

   im->Draw();
}

void TGo4ViewPanel::RedrawLegend(TPad *pad, TGo4Picture* padopt,
      TGo4Slot* legslot)
{
   if (legslot == 0) return;
   TLegend* legend = dynamic_cast<TLegend*>(legslot->GetAssignedObject());
   //legend->SetBit(kCanDelete, kFALSE); // jam2016
   if(legend!=0) legend->Draw();
}

void TGo4ViewPanel::RedrawSpecialObjects(TPad *pad, TGo4Slot* padslot)
{
   if ((pad == 0) || (padslot == 0))
      return;

   CheckObjectsAssigments(pad, padslot);

   QString selname = GetSelectedMarkerName(pad);
   TObject* selectedobj = 0;
   const char* selectdrawopt = 0;
   Int_t numtf1=0;
   for (int n = 0; n < padslot->NumChilds(); n++) {
      TGo4Slot* subslot = padslot->GetChild(n);

      Int_t kind = GetDrawKind(subslot);

      if ((kind < kind_Specials) || (kind >= kind_Other))
         continue;

      TObject* obj = subslot->GetAssignedObject();
      if (obj == 0)
         continue;

      TString drawopt = GetSpecialDrawOption(subslot);
      if(obj->InheritsFrom(TF1::Class())){
         if(!pad->GetListOfPrimitives()->IsEmpty())
            drawopt.Append("LSAME"); // for correct overlay of TF1 objects JAM
         TF1* func = dynamic_cast<TF1*>(obj);
         Int_t objindx = padslot->GetIndexOf(subslot);
         func->SetLineColor(GetAutoColor(((objindx+7) % 9) + 1)); // 9 basic colors for superimpose of tf1, like for other superimpose
      }

      if ((selname == obj->GetName()) && (selectedobj == 0)) {
         selectedobj = obj;
         selectdrawopt = drawopt.Data();
      } else {
         //obj->SetBit(kCanDelete, kFALSE); // jam2016
         obj->Draw(drawopt.Data());
     }
   }

   // if one has selected object on the pad, one should
   // draw it as last to bring it to the front of other
   if (selectedobj != 0){
       //selectedobj->SetBit(kCanDelete, kFALSE); // jam2016
       selectedobj->Draw(selectdrawopt ? selectdrawopt : "");
   }

}

bool TGo4ViewPanel::IsAutoZoomFlag()
{
   return fbAutoZoomFlag;
}

void TGo4ViewPanel::SetAutoZoomFlag(bool on)
{
   fbAutoZoomFlag = on;
}

bool TGo4ViewPanel::IsApplyToAllFlag()
{
   return fbApplyToAllFlag;
}

bool TGo4ViewPanel::IsFreezeTitle()
{
   return fbFreezeTitle;
}

void TGo4ViewPanel::ChangeDrawOptionForPad(TGo4Slot* padslot, int kind,
      int value, const char* drawopt)
{
   TGo4LockGuard lock;
   TGo4Picture* subopt = GetPadOptions(padslot);
   if (subopt == 0) return;
   switch (kind) {
      case 0:
      case 1:
      case 2: {
         int selindx = GetSelectedObjectIndex(padslot);
         subopt->SetDrawOption(drawopt, selindx);
         subopt->SetPadModified();
         break;
      }
      case 100: {
         subopt->SetPadModified();
         break;
      }
      case 101: {
         TPad* subpad = GetSlotPad(padslot);
         if (subpad != 0) {
            subpad->SetFillColor(value);
            if (subpad->GetFrame())
               subpad->GetFrame()->SetFillColor(value);
         }
         subopt->SetPadModified();
         break;
      }
      case 16: {
         // this is for setting time axis format:
         subopt->SetXAxisTimeFormat(drawopt);
         subopt->SetPadModified();
         break;
      }

      default:
         subopt->ChangeDrawOption(kind, value);
         break;
   }
}

void TGo4ViewPanel::ChangeDrawOption(int kind, int value, const char* drawopt)
{
   TGo4LockGuard lock;

   bool scanall = IsApplyToAllFlag();

   if (kind == 100) {
      gStyle->SetPalette(value);
      scanall = true;
   }

   TPad* pad = scanall ? GetCanvas() : GetActivePad();

   if (pad == 0)
      pad = GetCanvas();

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0) return;

   ChangeDrawOptionForPad(slot, kind, value, drawopt);

   TGo4Iter iter(slot, true);
   while (iter.next())
      ChangeDrawOptionForPad(iter.getslot(), kind, value, drawopt);

   RedrawPanel(pad, false);
}

void TGo4ViewPanel::ResetPadFillColors(TPad* pad, int col, TPad* backup)
{
   TGo4LockGuard lock;
   if (pad == 0)
      return;
   if(backup) col=backup->GetFillColor();
   pad->SetFillColor((Color_t) col);
   //pad->Modified(); // for image formats, display window bitmap has to change
   //std::cout<<"ResetPadFillColors sets color "<<col<<" to pad "<< (unsigned long) pad<<", name:"<<pad->GetName()<<", backup was "<<backup << std::endl;
   TIter iter(pad->GetListOfPrimitives());
   TObject* obj = 0;
   while ((obj = iter()) != 0) {
      TPad* subpad = dynamic_cast<TPad*>(obj);
      TFrame* fram = dynamic_cast<TFrame*>(obj);
      if (subpad != 0){
        //std::cout<<"ResetPadFillColors finds subpad " << (unsigned long) subpad << std::endl;
        TPad* backpad=0;
        if(backup)
          {
            backpad=dynamic_cast<TPad*>(backup->GetListOfPrimitives()->FindObject(subpad->GetName()));
            // debug only:
//            if(backpad)
//            {
//                col=backpad->GetFillColor();
//                std::cout<<"ResetPadFillColors finds backup pad "<<backpad->GetName()<<" with color "<< col << std::endl;
//            }
            // end debug
          }
         ResetPadFillColors(subpad, col, backpad);
       }
      else if (fram != 0)
      {
        if(backup)
        {
         TFrame* backframe=dynamic_cast<TFrame*>(backup->GetListOfPrimitives()->FindObject(fram->GetName()));
         if(backframe)
          {
              col=backframe->GetFillColor();
              //std::cout<<"ResetPadFillColors finds backup frame "<<backframe->GetName()<<" with color "<< col << std::endl;
          }
        }
        //std::cout<<"ResetPadFillColors sets color "<<col<<" to subframe "<< (unsigned long) fram->GetName()<< std::endl;
        fram->SetFillColor((Color_t) col);
      }
   }

}

void TGo4ViewPanel::ClearPad(TPad* pad, bool removeitems, bool removesubpads)
{
   TGo4LockGuard lock;

   BlockPanelRedraw(true);
   CleanupGedEditor();
   ProcessPadClear(pad, removeitems, removesubpads);
   if (GetActivePad() == 0)
      SetActivePad(GetCanvas());
   GetCanvas()->SetSelected(0);
   BlockPanelRedraw(false);
}

void TGo4ViewPanel::ClearPadItems(TGo4Slot* padslot, TGo4Slot* remain)
{
   if (padslot == 0) return;

   for (int n = padslot->NumChilds() - 1; n >= 0; n--) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int kind = GetDrawKind(subslot);
      if ((kind < kind_Permanet) && (subslot != remain)) {
         delete subslot;
      }
   }
}

void TGo4ViewPanel::ProcessPadClear(TPad * pad, bool removeitems, bool removesubpads)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot == 0) || (padopt == 0))
      return;

//   padopt->Clear(""); // remove all settings completely
   padopt->SetPadModified();

   if (removeitems)
      ClearPadItems(slot, 0);

   CheckObjectsAssigments(pad, slot);

   for (int n = slot->NumChilds() - 1; n >= 0; n--) {
      TGo4Slot* subslot = slot->GetChild(n);

      TPad* subpad = GetSlotPad(subslot);
      if (subpad == 0)
         continue;
      ProcessPadClear(subpad, removeitems || removesubpads, removesubpads);
      if (!removesubpads)
         continue;
      if (fxActivePad == subpad)
         fxActivePad = 0;

      delete subslot;

      pad->GetListOfPrimitives()->Remove(subpad);

      CallPanelFunc(panel_PadDeleted, subpad);

      if (GetCanvas()->GetSelectedPad() == subpad)
         GetCanvas()->SetSelectedPad(0);

      delete subpad;
   }

   CallPanelFunc(panel_Modified, pad);
}

void TGo4ViewPanel::SetPadDefaults(TPad* pad)
{
   TGo4LockGuard lock;

   if (pad == 0) return;

   {
      TPadLock lock; // workaround to avoid canvas update
      gStyle->SetOptStat(go4sett->getOptStat());
   }

   if (go4sett->getOptStatW()>0) gStyle->SetStatW(go4sett->getOptStatW()*0.01);
   if (go4sett->getOptStatH()>0) gStyle->SetStatH(go4sett->getOptStatH()*0.01);

   fbCanvasCrosshair = go4sett->getPadCrosshair();
   bool show_status = go4sett->getPadEventStatus();

   int fiPadcolorR, fiPadcolorG, fiPadcolorB;
   go4sett->getCanvasColor(fiPadcolorR, fiPadcolorG, fiPadcolorB);
   int padfillcolor = TColor::GetColor(fiPadcolorR, fiPadcolorG, fiPadcolorB);

   if (padfillcolor != 0) {
      // now define associated colors for WBOX shading
      // note: root restricts this mechanism to number<50
      // we extend it since TGX11ttf expects the shadow color
      // the following are copies from TColor code:
      Int_t nplanes = 16;
      if (gVirtualX)
         gVirtualX->GetPlanes(nplanes);
      if (nplanes == 0)
         nplanes = 16;
      TColor *normal = gROOT->GetColor(padfillcolor);
      if (normal != 0) {
         Float_t h, l, s;
         normal->GetHLS(h, l, s);
         const char* cname = normal->GetName();
         // assign the color numbers and names for shading:
         Float_t dr, dg, db, lr, lg, lb;
         TColor *dark = gROOT->GetColor(100 + padfillcolor);
         if (dark == 0) {
            new TColor(100 + padfillcolor, -1, -1, -1,
                  Form("%s%s", cname, "_dark"));
            dark = gROOT->GetColor(100 + padfillcolor);
         }
         TColor *light = gROOT->GetColor(150 + padfillcolor);
         if (light == 0) {
            new TColor(150 + padfillcolor, -1, -1, -1,
                  Form("%s%s", cname, "_bright"));
            light = gROOT->GetColor(150 + padfillcolor);
         }

         // set dark color
         TColor::HLStoRGB(h, 0.7 * l, s, dr, dg, db);
         if (dark != 0) {
            if (nplanes > 8)
               dark->SetRGB(dr, dg, db);
            else
               dark->SetRGB(0.3, 0.3, 0.3);
         }

         // set light color
         TColor::HLStoRGB(h, 1.2 * l, s, lr, lg, lb);
         if (light != 0) {
            if (nplanes > 8)
               light->SetRGB(lr, lg, lb);
            else
               light->SetRGB(0.8, 0.8, 0.8);
         }
      } else { // if(normal)
         TGo4Log::Error("Internal problem in view panel: Could not assign root shadow colors for number %d",  padfillcolor);
      }
   }

   pad->SetCrosshair(fbCanvasCrosshair);
   pad->SetFillColor(padfillcolor);


   if (fxQCanvas)
      if (show_status != fxQCanvas->isStatusBarVisible())
         ShowEventStatus();

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt != 0) {
      padopt->SetDrawOption(0, TGo4Picture::PictureIndex);
      padopt->SetDrawOption(0, 0);
      padopt->SetTitleTime(go4sett->getDrawTimeFlag());
      padopt->SetTitleDate(go4sett->getDrawDateFlag());
      padopt->SetTitleItem(go4sett->getDrawItemFlag());
      padopt->SetHisStats(go4sett->getStatBoxVisible());
   }
}

void TGo4ViewPanel::DisplayPadStatus(TPad * pad)
{
   if (pad == 0)
      return;
   TString output = pad->GetName();
   output.Append(": ");
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt != 0)
      if (padopt->IsSuperimpose())
         output.Append(" SuperImpose:");

   if (IsApplyToAllFlag())
      output.Append(" All Pads:");
   output.Append(" Ready");

   if (fxQCanvas) {
      fxQCanvas->showStatusMessage(output.Data());
   } else if (fxWCanvas) {
      printf("Implement display pad status\n");
   }
}

void TGo4ViewPanel::MoveScale(int expandfactor, int xaction, int yaction, int zaction)
{
   TPad* selpad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (selpad == 0)
      return;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt != 0) {
//     padopt->MoveScale(expandfactor, xaction, yaction);

      TObject* padhist = GetPadMainObject(selpad);

      MoveSingleScale(expandfactor, xaction, 0, padopt, padhist);
      MoveSingleScale(expandfactor, yaction, 1, padopt, padhist);
      MoveSingleScale(expandfactor, zaction, 2, padopt, padhist);
      if ((xaction == 0) && (yaction == 0) && (zaction == 0)) {
         padopt->ClearRange();
         padopt->SetAutoScale(true);
      }
      padopt->SetPadModified();
   }

   if (IsApplyToAllFlag()) {
      TGo4Iter iter(GetPadSlot(selpad), kTRUE);
      while (iter.next()) {
         TPad* subpad = GetSlotPad(iter.getslot());
         padopt = GetPadOptions(subpad);
         if (padopt == 0) continue;

         TObject* padhist = GetPadMainObject(subpad);

         MoveSingleScale(expandfactor, xaction, 0, padopt, padhist);
         MoveSingleScale(expandfactor, yaction, 1, padopt, padhist);
         MoveSingleScale(expandfactor, zaction, 2, padopt, padhist);
         if ((xaction == 0) && (yaction == 0) && (zaction == 0)) {
            padopt->ClearRange();
            padopt->SetAutoScale(true);
         }
         padopt->SetPadModified();
      }
   }

   RedrawPanel(selpad, false);
}

void TGo4ViewPanel::MoveSingleScale(int expandfactor, int action, int naxis,
                                   TGo4Picture* padopt, TObject* padobj)
{
   if (action <= 0) return;

   double new_umin, new_umax, fmin, fmax, tmin, tmax;
   double fact = expandfactor / 100.;

   bool sel = padopt->GetRange(naxis, new_umin, new_umax) && (action!=6);

   padopt->GetFullRange(naxis, fmin, fmax);

   // we use number of dimensions to determine if we have contents
   int ndim = padopt->GetFullRangeDim();

   if (!sel || (new_umin >= new_umax)) { new_umin = fmin; new_umax = fmax; }

   double shift = (new_umax - new_umin) * fact;
   // protect if changes is out of full axis ranges
   if (shift <= 0) return;

   switch (action) {
      case 1:
         if ((new_umax + shift) > fmax) // SlotShiftL
            shift = fmax - new_umax;
         new_umin += shift;
         new_umax += shift;
         break;
      case 2:
         if (new_umin - shift < fmin) // SlotShiftR
            shift = new_umin - fmin;
         new_umin -= shift;
         new_umax -= shift;
         break;
      case 3:
         if (ndim == 1) {
            new_umax -= shift; // Expand 1d upper
            if (naxis < 1) // lower only for X
               new_umin += shift; // Expand 1d lower X
         }
         if (ndim == 2) {
            new_umax -= shift; // Expand 2d upper
            if (naxis < 2) // lower only for X,Y
               new_umin += shift; // Expand 1d lower X,Y
         }
         break;
      case 4:
         if (ndim == 1) { // Shrink 1d upper
            if (naxis < 1) { // X axis
               tmax = (-fact * new_umin + (1. - fact) * new_umax)
                     / (1. - 2. * fact);
               tmin = (-fact * new_umax + (1. - fact) * new_umin)
                     / (1. - 2. * fact); // Shrink 1d X
            } else { // content, lower has not been expanded
               tmax = (-fact * new_umin + new_umax) / (1. - fact);
               tmin = (-fact * new_umax + (1. - fact) * new_umin)
                     / (1. - 2. * fact); // Shrink 1d X
            }
         }
         if (ndim == 2) {
            if (naxis < 2) { // X,Y axis
               tmax = (-fact * new_umin + (1. - fact) * new_umax)
                     / (1. - 2. * fact);
               tmin = (-fact * new_umax + (1. - fact) * new_umin)
                     / (1. - 2. * fact); // Shrink 1d X
            } else { // content, lower has not been expanded
               tmax = (-fact * new_umin + new_umax) / (1. - fact);
               tmin = (-fact * new_umax + (1. - fact) * new_umin)
                     / (1. - 2. * fact); // Shrink 1d X
            }
         }
         new_umin = tmin;
         new_umax = tmax;
         if (new_umin < fmin)
            new_umin = fmin;
         if (new_umax > fmax)
            new_umax = fmax;
         break;

      case 5:   // Auto-zoom
      case 6: { // Auto zoom, but ignore selected range
         TH1* padhist = dynamic_cast<TH1*>(padobj);

         if (padhist==0) break;

         if (naxis >= ndim) break;

         TAxis* axis = padhist->GetXaxis();
         if (naxis==1) axis = padhist->GetYaxis();
         if (naxis==2) axis = padhist->GetZaxis();

         // keep selected bins
         Int_t sel_l = sel ? axis->GetFirst() : 0;
         Int_t sel_r = sel ? axis->GetLast() : axis->GetNbins();

         Int_t firstbin(0), lastbin(0);

         // X axis in case of 1-dim histogram
         if ((ndim==1) && (naxis==0)) {
            for (Int_t n1 = 1; n1<=padhist->GetNbinsX(); n1++) {
               Double_t v = padhist->GetBinContent(n1);
               if (TMath::Abs(v)<1e-10) continue;
               if ((n1<sel_l) || (n1>sel_r)) continue;
               lastbin = n1;
               if (firstbin==0) firstbin = n1;
            }
         }

         // X,Y axis in case of 2-dim histogram
         if ((ndim==2) && (naxis<2))
            for (Int_t n1 = 1; n1<=padhist->GetNbinsX(); n1++)
               for (Int_t n2 = 1; n2<=padhist->GetNbinsY(); n2++) {
                 Double_t v = padhist->GetBinContent(n1,n2);
                 if (TMath::Abs(v)<1e-10) continue;
                 Int_t bin = naxis==0 ? n1 : n2;
                 if ((bin<sel_l) || (bin>sel_r)) continue;
                 if ((lastbin==0) || (bin>lastbin)) lastbin = bin;
                 if ((firstbin==0) || (bin<firstbin)) firstbin = bin;
              }

         // X,Y,Z axis in case of 3-dim histogram
         if ((ndim==3) && (naxis<3))
            for (Int_t n1 = 1; n1<=padhist->GetNbinsX(); n1++)
               for (Int_t n2 = 1; n2<=padhist->GetNbinsY(); n2++)
                  for (Int_t n3 = 1; n3<=padhist->GetNbinsZ(); n3++) {
                    Double_t v = padhist->GetBinContent(n1,n2,n3);
                    if (TMath::Abs(v)<1e-10) continue;
                    Int_t bin = naxis==0 ? n1 : ((naxis==1) ? n2 : n3);
                    if ((bin<sel_l) || (bin>sel_r)) continue;
                    if ((lastbin==0) || (bin>lastbin)) lastbin = bin;
                    if ((firstbin==0) || (bin<firstbin)) firstbin = bin;
                  }

         if (firstbin>0) {

            if (firstbin >= lastbin) { firstbin--; lastbin++; }

            if (firstbin<=3) firstbin = 1;
            if (lastbin >=axis->GetNbins()-3) lastbin = axis->GetNbins();

            Double_t left = axis->GetBinLowEdge(firstbin);
            Double_t right = axis->GetBinUpEdge(lastbin);

            Double_t margin = (right - left) * fact;
            left -= margin; right += margin;

            if ((left <= new_umin) && (right >= new_umax)) return;

            if (left > new_umin) new_umin = left;
            if (right < new_umax) new_umax = right;

            if (action==6) axis->SetRange(firstbin,lastbin);
         }

         break;
      }

      default:
         return;
   }

   TakeSelectedAxisRange(naxis, padopt, new_umin, new_umax, false);

   if (naxis == ndim)
      if (padopt->GetRange(naxis, new_umin, new_umax))
         padopt->SetAutoScale(kFALSE);
}

void TGo4ViewPanel::TakeFullRangeFromHisto(TH1* h1, TGo4Picture* padopt, bool isfirsthisto)
{
   if ((h1 == 0) || (padopt == 0)) return;

   TAxis* xax = h1->GetXaxis();
   TAxis* yax = h1->GetYaxis();
   TAxis* zax = h1->GetZaxis();
   int ndim = h1->GetDimension();

   if (isfirsthisto) {
      padopt->SetFullRangeDim(ndim);
      padopt->SetFullRange(0, xax->GetBinLowEdge(1),
            xax->GetBinUpEdge(xax->GetNbins()));
      if (ndim > 1)
         padopt->SetFullRange(1, yax->GetBinLowEdge(1),
               yax->GetBinUpEdge(yax->GetNbins()));
      else
         padopt->ClearFullRange(2);

      if (ndim > 2)
         padopt->SetFullRange(2, zax->GetBinLowEdge(1),
               zax->GetBinUpEdge(zax->GetNbins()));
   } else
      ndim = padopt->GetFullRangeDim();

   if (ndim >= 3)
      return;

   Int_t dimindx = (ndim == 1) ? 1 : 2;

   Double_t minimum = 0, maximum = 0;
   Bool_t first = kTRUE;
   if (!isfirsthisto) {
      padopt->GetFullRange(dimindx, minimum, maximum);
      first = kFALSE;
   }

   for (Int_t biny = 1; biny <= yax->GetNbins(); biny++)
      for (Int_t binx = 1; binx <= xax->GetNbins(); binx++) {
         Int_t bin = h1->GetBin(binx, biny);
         Double_t value = h1->GetBinContent(bin);
         if (first) {
            minimum = value;
            maximum = value;
            first = kFALSE;
         }
         if (value < minimum)
            minimum = value;
         else if (value > maximum)
            maximum = value;
      }

   padopt->SetFullRange(dimindx, minimum, maximum);
}

void TGo4ViewPanel::TakeFullRangeFromGraph(TGraph * gr, TGo4Picture * padopt, bool isfirst)
{
   if ((gr == 0) || (padopt == 0)) return;

   Double_t minx(0), maxx(0), miny(0), maxy(0), xx, yy;
   if (isfirst) {
      if (gr->GetN() > 0) {
         gr->GetPoint(0, minx, miny);
         maxx = minx;
         maxy = miny;
      }
   } else {
      padopt->GetFullRange(0, minx, maxx);
      padopt->GetFullRange(1, miny, maxy);
   }

   for (Int_t n = 0; n < gr->GetN(); n++) {
      gr->GetPoint(n, xx, yy);
      if (xx < minx)
         minx = xx;
      else if (xx > maxx)
         maxx = xx;
      if (yy < miny)
         miny = yy;
      else if (yy > maxy)
         maxy = yy;
   }

   padopt->SetFullRangeDim(1);
   padopt->SetFullRange(0, minx, maxx);
   padopt->SetFullRange(1, miny, maxy);
   padopt->ClearFullRange(2);
}

void TGo4ViewPanel::SetSelectedRangeToHisto(TPad* pad, TH1* h1, THStack* hs,
      TGo4Picture* padopt, bool isthishisto)
{
   // set selected range, stats and title position for histogram

   if ((h1 == 0) || (padopt == 0) || (pad == 0)) return;

   int ndim = padopt->GetFullRangeDim();

   bool autoscale = padopt->IsAutoScale();

   double hmin(0.), hmax(0.), umin, umax;

   TAxis* ax = h1->GetXaxis();

   if (IsAutoZoomFlag() && isthishisto) {
      MoveSingleScale(1., 6, 0, padopt, h1);
      if (ndim>1) MoveSingleScale(1., 6, 1, padopt, h1);
      if (ndim>2) MoveSingleScale(1., 6, 2, padopt, h1);
   } else
   if (padopt->GetRange(0, umin, umax)) {
      // note: go4 range was full visible range of histogram
      // in new ROOT automatic shift of ranges can appear,
      // to prevent this, center of each bin should be used

      Int_t i1 = ax->FindFixBin(umin);
      Int_t i2 = ax->FindFixBin(umax);
      if (i1<i2) ax->SetRange(i1,i2);
            else { ax->UnZoom(); padopt->ClearRange(0); }
   } else {
      ax->UnZoom();
   }

   TAxis* ay = h1->GetYaxis();

   if (padopt->GetRange(1, umin, umax)) {
      if (!autoscale && (ndim == 1)) {
         hmin = umin;
         hmax = umax;
         ay = 0;
      }

      // note: go4 range was full visible range of histogram
      // in new ROOT automatic shift of ranges can appear,
      // to prevent this, center of each bin should be used

      if (ndim>1) {
         Int_t i1 = ay->FindFixBin(umin);
         Int_t i2 = ay->FindFixBin(umax);
         if (i1<i2) { ay->SetRange(i1,i2); ay = 0; }
      }
   }

   if (ay!=0) {
      ay->UnZoom();
      padopt->ClearRange(1);
      // workaround for the 5.34/11 version
      if ((ndim==1) && (h1->GetMinimum()==0) && (h1->GetMaximum()==1)) {
         h1->SetMinimum();
         h1->SetMaximum();
      }
   }

   TAxis* az = h1->GetZaxis();
   if (padopt->GetRange(2, umin, umax) && (ndim > 1)) {
      if (!autoscale && (ndim == 2)) {
         hmin = umin;
         hmax = umax;
         az = 0;
      }
      // note: go4 range was full visible range of histogram
      // in new ROOT automatic shift of ranges can appear,
      // to prevent this, center of each bin should be used

      if (ndim>2) {
         Int_t i1 = az->FindFixBin(umin);
         Int_t i2 = az->FindFixBin(umax);
         if (i1<i2) { az->SetRange(i1,i2); az = 0; }
      }
   }

   if (az!=0) {
      az->UnZoom();
      padopt->ClearRange(2);
   }

   if (hmin != hmax) {
      // if scale axis is log, prevent negative values, otherwise
      // histogram will not be displayed
      if (padopt->GetLogScale(ndim)) {
         if (hmax <= 0) hmax = 1.;
         if ((hmin <= 0) || (hmin >= hmax)) {
            hmin = hmax * 1e-4;
            if (hmin > 1.) hmin = 1;
         }
      }

      h1->SetMinimum(hmin);
      h1->SetMaximum(hmax);
      h1->SetBit(TH1::kIsZoomed);
      if (hs != 0) {
         hs->SetMinimum(hmin);
         hs->SetMaximum(hmax);
      }
   } else {
      // this is autoscale mode

      if (hs != 0) {

         if (ndim == 1) {
            TIter next(hs->GetHists());
            TH1* hs_h1 = 0;
            while ((hs_h1 = (TH1*) next()) != 0) {
               if (padopt->GetRange(0, umin, umax)) {
                  // note: go4 range was full visible range of histogram
                  // in new ROOT automatic shift of ranges can appear,
                  // to prevent this, center of each bin should be used
                  TAxis* ax = hs_h1->GetXaxis();
                  Int_t i1 = ax->FindFixBin(umin);
                  Int_t i2 = ax->FindFixBin(umax);
                  if (i1<i2) { ax->SetRange(i1,i2); }
                        else { ax->UnZoom(); padopt->ClearRange(0); }
               } else {
                  hs_h1->GetXaxis()->UnZoom();
               }

               hs_h1->GetYaxis()->UnZoom();
               hs_h1->SetMinimum();
               hs_h1->SetMaximum();
            }
         }

         hs->SetMinimum();
         hs->SetMaximum();
      }

      h1->SetMinimum();
      h1->SetMaximum();
      h1->ResetBit(TH1::kIsZoomed);

      // here one can estimate actual range  which will be displayed on canvas

      if (ndim < 3) {

         Double_t selmin = h1->GetMinimum();
         Double_t selmax = h1->GetMaximum();

         if (selmin >= selmax) {
            padopt->ClearRange(ndim);
         } else {
            bool islogscale = (ndim == 1) && (padopt->GetLogScale(1) > 0);

            if (islogscale) {
               if ((selmin > 0) && (selmax > 0)) {
                  selmin = TMath::Log10(selmin) + TMath::Log10(0.5);
                  selmin = TMath::Power(10, selmin);
                  selmax = TMath::Log10(selmax)
                        + TMath::Log10(2 * (0.9 / 0.95));
                  selmax = TMath::Power(10, selmax);
               }

            } else {
               Double_t yMARGIN = 0.05;
#if ROOT_VERSION_CODE > ROOT_VERSION(5,0,9)
               yMARGIN = gStyle->GetHistTopMargin();
#endif
               Double_t dymin = yMARGIN * (selmax - selmin);
               if ((selmin >= 0) && (selmin - dymin < 0))
                  selmin = 0;
               else
                  selmin -= dymin;
               selmax += yMARGIN * (selmax - selmin);
            }

            padopt->SetRange(ndim, selmin, selmax);
         }
      }
   }

   if (padopt->IsHisStats() && isthishisto) {
      TPaveStats* stats =
            dynamic_cast<TPaveStats*>(h1->GetListOfFunctions()->FindObject("stats"));
      if (stats==0) {
         stats = new TPaveStats(
               gStyle->GetStatX()-gStyle->GetStatW(),
               gStyle->GetStatY()-gStyle->GetStatH(),
               gStyle->GetStatX(),
               gStyle->GetStatY(),"brNDC");
         stats->SetParent(h1);
         stats->UseCurrentStyle();
         stats->SetName("stats");
         h1->GetListOfFunctions()->Add(stats);
         stats->ConvertNDCtoPad(); // need to bypass TPave init problem
      }
      padopt->GetStatsAttr(stats);
   }

   if (padopt->IsHisTitle() && padopt->HasTitleAttr()) {
      TPaveText* titl =
            dynamic_cast<TPaveText*>(pad->GetListOfPrimitives()->FindObject("title"));

      if (titl==0) {
         titl = new TPaveText(gStyle->GetTitleX()-gStyle->GetTitleW(),
               gStyle->GetTitleY()-gStyle->GetTitleH(),
               gStyle->GetTitleX(),
               gStyle->GetTitleY(),"blNDC");
         titl->UseCurrentStyle();
         titl->SetFillColor(gStyle->GetTitleFillColor());
         titl->SetFillStyle(gStyle->GetTitleStyle());
         titl->SetName("title");
         titl->SetBorderSize(gStyle->GetTitleBorderSize());
         titl->SetTextColor(gStyle->GetTitleTextColor());
         titl->SetTextFont(gStyle->GetTitleFont(""));
         if (gStyle->GetTitleFont("")%10 > 2)
            titl->SetTextSize(gStyle->GetTitleFontSize());
         titl->AddText(h1->GetTitle());
         titl->SetBit(kCanDelete);
         pad->GetListOfPrimitives()->Add(titl);
         titl->ConvertNDCtoPad(); // need to bypass TPave init problem
      }
      padopt->GetTitleAttr(titl);
   }

   // add here setting the time format properties:
   TAxis* xax = h1->GetXaxis();
   xax->SetTimeDisplay(padopt->IsXAxisTimeDisplay());
   xax->SetTimeFormat(padopt->GetXAxisTimeFormat());

   // JAM 2016 finally we evaluate the rectangular axis scale property:
   //std::cout<< "SetSelectedRangeToHisto - 1:1 is "<< padopt->IsXYRatioOne()<< std::endl;
   if (padopt->IsXYRatioOne())
   {
     RectangularRatio(pad);
   }
   else
   {
     DefaultPadMargin(pad);
    }
}

bool TGo4ViewPanel::GetVisibleRange(TPad* pad, int naxis, double& min, double& max)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt == 0) return false;

   int NumDim = padopt->GetFullRangeDim();

   if ((naxis < 0) || (naxis > NumDim)) return false;

   bool res = padopt->GetRange(naxis, min, max);
   if (!res || (min >= max))
      res = padopt->GetFullRange(naxis, min, max);
   return res;
}

bool TGo4ViewPanel::TakeSelectedAxisRange(int naxis, TGo4Picture* padopt,
      double selmin, double selmax, bool force)
{
   if ((selmin == -1.) && (selmax == -1.)) return false;

   double min, max, umin, umax;

   bool full = padopt->GetFullRange(naxis, min, max);
   bool sel = padopt->GetRange(naxis, umin, umax);

   if (!full || (min >= max) || (selmin >= selmax)) {
      padopt->ClearRange(naxis);
      return true;
   }

   double delta = (max - min) / 100000.;
   bool changed = false;

   if (umin < umax) {
      if (fabs(umin - selmin) > delta) {
         umin = selmin;
         changed = true;
      }
      if (fabs(umax - selmax) > delta) {
         umax = selmax;
         changed = true;
      }
   } else {
      umin = selmin;
      umax = selmax;
      changed = true;
   }

   if ((selmin < min + delta) && (selmax > max - delta) && !force) {
      padopt->ClearRange(naxis);
   } else {
      padopt->SetRange(naxis, umin, umax);
   }

   return changed;
}

bool TGo4ViewPanel::TakeSelectedAxisRange(int naxis, TGo4Picture* padopt, TAxis* ax)
{
   Double_t selmin, selmax;

   // check if something selected on axis
   if (((ax->GetFirst() <= 0) && (ax->GetLast() >= ax->GetNbins() - 1))
         || (ax->GetFirst() >= ax->GetLast())) {
      selmin = -1111;
      selmax = -1111;
   } else {
      selmin = ax->GetBinCenter(ax->GetFirst());
      selmax = ax->GetBinCenter(ax->GetLast());
   }

   return TakeSelectedAxisRange(naxis, padopt, selmin, selmax, false);
}

void TGo4ViewPanel::PadRangeAxisChanged(TPad* pad)
{
   TGo4LockGuard lock;

   TGo4Picture* padopt = GetPadOptions(pad);

   if (IsRedrawBlocked() || (pad == 0) || (padopt == 0)) return;

   // check if we have histogram and can take range from it
   TH1* h1 = GetPadHistogram(pad);

   if (h1 != 0) {
      Int_t ndim = padopt->GetFullRangeDim();

      TakeSelectedAxisRange(0, padopt, h1->GetXaxis());
      if (ndim > 1)
         TakeSelectedAxisRange(1, padopt, h1->GetYaxis());
      if (ndim > 2)
         TakeSelectedAxisRange(2, padopt, h1->GetZaxis());
      if (ndim < 3) {

         bool iszoomed = h1->TestBit(TH1::kIsZoomed);

         padopt->SetAutoScale(!iszoomed);
         //h1->SetMinimum();
         //h1->SetMaximum();

         Double_t selmin = h1->GetMinimum();
         Double_t selmax = h1->GetMaximum();

         if (iszoomed) {
            padopt->SetRange(ndim, selmin, selmax);
         } else if (selmin >= selmax) {
            padopt->ClearRange(ndim);
         } else {

            bool islogscale = (ndim == 1) && (pad->GetLogy() > 0);

            if (islogscale) {
               if ((selmin > 0) && (selmax > 0)) {
                  selmin = TMath::Log10(selmin) + TMath::Log10(0.5);
                  selmin = TMath::Power(10, selmin);
                  selmax = TMath::Log10(selmax)
                         + TMath::Log10(2 * (0.9 / 0.95));
                  selmax = TMath::Power(10, selmax);
               }

            } else {
               Double_t yMARGIN = 0.05;
#if ROOT_VERSION_CODE > ROOT_VERSION(5,0,9)
               yMARGIN = gStyle->GetHistTopMargin();
#endif
               Double_t dymin = yMARGIN * (selmax - selmin);
               if ((selmin >= 0) && (selmin - dymin < 0))
                  selmin = 0;
               else
                  selmin -= dymin;
               selmax += yMARGIN * (selmax - selmin);
            }

            padopt->SetRange(ndim, selmin, selmax);
         }
      }

      // inform that options were changed in panel
      CallPanelFunc(panel_Updated, pad);
      return;
   }

   Double_t rxmin, rxmax, rymin, rymax;
   pad->GetRangeAxis(rxmin, rymin, rxmax, rymax);

   if (pad->GetLogx() > 0) {
      rxmin = TMath::Power(10., rxmin);
      rxmax = TMath::Power(10., rxmax);
   }

   if (pad->GetLogy() > 0) {
      rymin = TMath::Power(10., rymin);
      rymax = TMath::Power(10., rymax);
   }

   PadRangeAxisChanged(pad, rxmin, rxmax, rymin, rymax);
}

void TGo4ViewPanel::PadRangeAxisChanged(TPad* pad, double rxmin, double rxmax,
      double rymin, double rymax)
{
   TGo4LockGuard lock;

   if (IsRedrawBlocked() || (pad == 0)) return;

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt == 0) return;

   TakeSelectedAxisRange(0, padopt, rxmin, rxmax, false);
   TakeSelectedAxisRange(1, padopt, rymin, rymax, false);

   CallPanelFunc(panel_Updated, pad);
}

void TGo4ViewPanel::GetSelectedRange(int& ndim, bool& autoscale, double& xmin,
      double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
   ndim = 0;
   TGo4Picture* padopt = GetPadOptions(GetActivePad());
   if (padopt == 0) return;

   ndim = padopt->GetFullRangeDim();
   autoscale = padopt->IsAutoScale();

   GetVisibleRange(GetActivePad(), 0, xmin, xmax);
   GetVisibleRange(GetActivePad(), 1, ymin, ymax);
   GetVisibleRange(GetActivePad(), 2, zmin, zmax);
}

void TGo4ViewPanel::SetApplyToAllFlag(bool on)
{
   fbApplyToAllFlag = on;

   QCheckBox* box1 = findChild<QCheckBox*>("ApplyToAllCheck");
   if (box1 != 0) box1->setChecked(on);
}

void TGo4ViewPanel::SetAutoScale(bool on, TPad* selpad)
{
   TGo4LockGuard lock;

   bool modified = false;
   bool applytoall = false;
   bool redraw_immediately = (selpad == 0);

   if (selpad == 0) {
      applytoall = IsApplyToAllFlag();
      selpad = applytoall ? GetCanvas() : GetActivePad();
   }
   if (selpad == 0)
      return;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt != 0) {
      if (on != padopt->IsAutoScale())
         padopt->SetPadModified();
      padopt->SetAutoScale(on);
      modified = modified || padopt->IsPadModified();
   }

   if (applytoall) {
      TGo4Iter iter(GetPadSlot(selpad), kTRUE);
      while (iter.next()) {
         TPad* subpad = GetSlotPad(iter.getslot());
         padopt = GetPadOptions(subpad);
         if (padopt == 0)
            continue;
         if (on != padopt->IsAutoScale())
            padopt->SetPadModified();
         padopt->SetAutoScale(on);
         modified = modified || padopt->IsPadModified();
      }
   }

   if (modified && redraw_immediately)
      RedrawPanel(selpad, false);
}

void TGo4ViewPanel::SetSelectedRange(double xmin, double xmax, double ymin,
      double ymax, double zmin, double zmax)
{
   TGo4LockGuard lock;

   TPad* selpad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (selpad == 0)
      return;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt != 0) {
      Int_t ndim = padopt->GetFullRangeDim();

      TakeSelectedAxisRange(0, padopt, xmin, xmax, true);
      if (ndim > 1)
         TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
      if (ndim > 2)
         TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
      if (!padopt->IsAutoScale()) {
         if (ndim == 1)
            TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
         if (ndim == 2)
            TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
      }
      padopt->SetPadModified();
   }

   if (IsApplyToAllFlag()) {
      TGo4Iter iter(GetPadSlot(selpad), kTRUE);
      while (iter.next()) {
         TPad* subpad = GetSlotPad(iter.getslot());
         padopt = GetPadOptions(subpad);
         if (padopt == 0)
            continue;
         Int_t ndim = padopt->GetFullRangeDim();
         TakeSelectedAxisRange(0, padopt, xmin, xmax, true);
         if (ndim > 1)
            TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
         if (ndim > 2)
            TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
         if (!padopt->IsAutoScale()) {
            if (ndim == 1)
               TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
            if (ndim == 2)
               TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
         }

         padopt->SetPadModified();
      }
   }

   RedrawPanel(selpad, false);
}

void TGo4ViewPanel::resizeEvent(QResizeEvent * e)
{

  // store size of top widget -
   // size of top widget will be restored when new panel is created
   go4sett->storePanelSize(parentWidget(), "ViewPanel");
   //std::cout<< "TGo4ViewPanel::resizeEvent"<<std::endl;
   TPad* selpad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
     if (selpad == 0)
        return;
   // only if we are in 1:1 aspect ratio, we might need a redraw here:
   TGo4Slot* slot = GetPadSlot(selpad);
   if (slot == 0) return;
   TGo4Picture* padopt = GetPadOptions(slot);
   if (padopt == 0) return;

   if (padopt->IsXYRatioOne())
   {
     //std::cout<< "TGo4ViewPanel::resizeEvent with ratio :1"<<std::endl;
     // note: we need to delay execution of redraw, since resize Event in QtROOT canvas will
     // also happen in timer 100ms after us -> new coordinates are not refreshed here!
     fxResizeTimerPad=selpad;
     QTimer::singleShot(1000, this, SLOT(checkResizeSlot()));
   }
}

void TGo4ViewPanel::ResizeGedEditor()
{
   if (fxQCanvas) fxQCanvas->resizeEditor();
}

void TGo4ViewPanel::ActivateInGedEditor(TObject* obj)
{
   printf("TGo4ViewPanel::ActivateInGedEditor %s %p\n", obj ? obj->GetName() : "---", obj);

   if (obj && !obj->InheritsFrom(THStack::Class()) && !obj->InheritsFrom(TMultiGraph::Class()))
      if (fxQCanvas) {
         fxQCanvas->actiavteEditor(GetActivePad(), obj);
      } else if (fxWCanvas) {
#ifdef GO4_WEBGUI
         fxWCanvas->actiavteEditor(GetActivePad(), obj);
#endif
      }
}

void TGo4ViewPanel::CleanupGedEditor()
{
   if (fxQCanvas)
      fxQCanvas->cleanupEditor();
}

void TGo4ViewPanel::ShootRepaintTimer()
{
   ShootRepaintTimer(GetCanvas());
}

void TGo4ViewPanel::ShootRepaintTimer(TPad* pad)
{
   if (IsRepaintTimerActive()) return;

   if (pad == 0) pad = GetCanvas();

   fxRepaintTimerPad = pad;
   CallServiceFunc(service_PanelTimer);
}

bool TGo4ViewPanel::IsRepaintTimerActive()
{
   return fxRepaintTimerPad != 0;
}

void TGo4ViewPanel::checkRepaintSlot()
{
   TPad* pad = fxRepaintTimerPad;
   fxRepaintTimerPad = 0;
   if (pad != 0)
      RedrawPanel(pad, false);
}

void TGo4ViewPanel::checkResizeSlot()
{
   TPad* pad = fxResizeTimerPad;
   fxResizeTimerPad = 0;
   if (pad != 0)
      RedrawPanel(pad, true);
}

void TGo4ViewPanel::PadDeleted(TPad* pad)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot != 0)
      delete slot;

   GetCanvas()->SetSelectedPad(GetCanvas());
   GetCanvas()->SetSelected(GetCanvas());
   TGo4MdiArea::Instance()->SetSelectedPad(GetCanvas());
}

// marker functions

int TGo4ViewPanel::GetNumMarkers(TPad* pad, int kind)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0)
      return 0;

   int res = 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      if (GetDrawKind(subslot) == kind)
         res++;
   }
   return res;
}

void TGo4ViewPanel::AddMarkerObj(TPad* pad, int kind, TObject* obj)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot == 0) {
      delete obj;
      return;
   }

   QString basename;
   switch (kind) {
      case kind_Marker:
         basename = "Marker";
         break;
      case kind_Window:
         basename = "Window";
         break;
      case kind_Poly:
         basename = "Polygon";
         break;
      case kind_Latex:
         basename = "Latex";
         break;
      case kind_Arrow:
         basename = "Arrow";
         break;
      default:
         basename = "Something";
         break;
   }

   QString slotname;
   int cnt = 0;
   do {
      slotname = basename + QString::number(cnt++);
   } while (padslot->FindChild(slotname.toLatin1().constData()) != 0);

   TGo4Slot* objslot = AddDrawObject(pad, kind, slotname.toLatin1().constData(), obj, true, 0);

   SetActiveObj(pad, kind, objslot);
}

void TGo4ViewPanel::SetActiveObj(TPad* pad, int kind, TGo4Slot* activeslot)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0)
      return;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind != kind)
         continue;

      if (subslot != activeslot)
         subslot->RemovePar("::ActiveMarker");
      else
         subslot->SetPar("::ActiveMarker", "1");
   }
}

TObject* TGo4ViewPanel::GetActiveObj(TPad* pad, int kind)
{
   int selkind;
   TObject* selobj;
   TGo4Slot* selslot = GetSelectedSlot(pad, &selkind, &selobj);
   if ((kind == selkind) && (selobj != 0))
      return selobj;

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot == 0)
      return 0;

   TObject* lastobj = 0;

   for (int n = 0; n < slot->NumChilds(); n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind != kind)
         continue;

      lastobj = subslot->GetAssignedObject();
      if (subslot->GetPar("::ActiveMarker") != 0)
         return lastobj;
   }

   return lastobj;
}

QString TGo4ViewPanel::GetActiveObjName(TPad* pad, int kind)
{
   TObject* obj = GetActiveObj(pad, kind);
   return (obj == 0) ? QString("null") : QString(obj->GetName());
}

void TGo4ViewPanel::OptionsMenuItemActivated(int id)
{
   TGo4LockGuard lock;
   switch (id) {
      case CrosshairId: {
         fbCanvasCrosshair = !fbCanvasCrosshair;
         GetCanvas()->SetCrosshair(fbCanvasCrosshair);

#if ROOT_VERSION_CODE < ROOT_VERSION(4,0,8)

         TGo4Iter(GetPadSlot(GetCanvas()), true);
         while (iter.next()) {
            TPad* subpad = GetSlotPad(iter.getslot());
            if (subpad!=0) {
               subpad->SetCrosshair(fbCanvasCrosshair);
               subpad->Modified();
            }
         }
#endif

         CanvasUpdate(true);
         CallPanelFunc(panel_Updated, GetCanvas());
         break;
      }

      case FreezeTitleId: {
         fbFreezeTitle = !fbFreezeTitle;
         break;
      }

      case AutoZoomId: {
         fbAutoZoomFlag = !fbAutoZoomFlag;
         if (fbAutoZoomFlag) RedrawPanel(GetCanvas(), true);
         break;
      }

      case SetTitleTextId: {
         bool ok = false;
         QString mycaption = windowTitle();
         QString oldtitle = mycaption.remove(fPanelName + ": ");
         QString text = QInputDialog::getText(this, GetPanelName(),
               "Enter Viewpanel Title:", QLineEdit::Normal, oldtitle, &ok);
         if (ok && !text.isEmpty())
            SetFreezedTitle(text);
         break;
      }

      case SetTimeFormatId: {
         bool ok = false;
         TPad* pad = GetActivePad();
         if (pad == 0) pad = GetCanvas();
         TGo4Picture *padopt = GetPadOptions(pad);
         QString oldfmt = padopt->GetXAxisTimeFormat();
         QString text = QInputDialog::getText(this, GetPanelName(),
               "Enter Axis time format:", QLineEdit::Normal, oldfmt, &ok);
         if (ok && !text.isEmpty()) {
            //std::cout <<"changed time format to"<<(const char*)text.toLatin1() << std::endl;
            //padopt->SetXAxisTimeFormat(text.toLatin1());
            //padopt->SetPadModified();
            ChangeDrawOption(id - 1000, 0, text.toLatin1());
         }
         break;
      }

      default:
         if (id > 1000) {
            QAction* act = SetIdAction(fOptionsMap, id);
            bool s = act ? act->isChecked() : false;
            ChangeDrawOption(id - 1000, s, 0);
         }
         break;
   }
}

void TGo4ViewPanel::ApplyToAllToggled(bool on)
{
//   if (IsRedrawBlocked()) return;

   fbApplyToAllFlag = on;
}

void TGo4ViewPanel::AutoScaleToggled(bool on)
{
   if (IsRedrawBlocked())
      return;

   SetAutoScale(on, 0);
}

void TGo4ViewPanel::panelSlot(TGo4ViewPanel * panel, TPad * pad, int signalid)
{
   if (panel != this)
      return;

   BlockPanelRedraw(true);

   TGo4Picture* padopt = GetPadOptions(GetActivePad());
   if (padopt != 0)
      fAutoScaleCheck->setChecked(padopt->IsAutoScale());

   BlockPanelRedraw(false);
}

void TGo4ViewPanel::SuperImposeToggled(bool on)
{
   if (IsRedrawBlocked())
      return;

   ChangeDrawOption(11, on, "");
}
