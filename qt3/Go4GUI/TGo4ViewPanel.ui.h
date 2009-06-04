const char* NoStackDrawOption = "nostack, ";

#ifdef __GO4GED__
#include "TGedEditor.h"
#endif

enum Go4ViewPanelMenuesId {
    ShowMarkEditorId = 201,
    ShowRootEditorId = 202,
    EventStatusId    = 301,
    CrosshairId      = 302,
    FreezeTitleId    = 309,
    SetTitleTextId   = 310,
    SelectObjectId   = 501,
    SuperimposeId    = 1011,
    StatisticsId     = 1006,
    SetTitleId       = 1007,
    SetLegendId      = 1008,
    DrawTimeId       = 1012,
    DrawDateId       = 1013,
    DrawItemnameId   = 1014,
    BringToFrontId   = 4998,
    MasterSelectId   = 4999,
    FirstSelectId    = 5000 };

enum Go4MouseMode_t {
   kMouseROOT ,   // normal root mouse
   kMousePickCursor,  // click sets cursor / marker
   kMousePickLimits,  // click sets region limit / window condition
   kMousePickPolygon, // click sets polygon condition
   kMousePickLatex,   // click sets latex label
   kMouseDraw        // draw on mouse press
};

const char*  TGo4ViewPanel::GetPanelName()
{
   return fPanelName.latin1();
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
   if (parent==0) parent = GetPanelSlot();
   return new TGo4Slot(parent, name, "title");
}

void TGo4ViewPanel::ResetWidget()
{
   // do nothing
}

void TGo4ViewPanel::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (linkname==0) return;

   if (strcmp(linkname,"PadRangeAxisChanged")==0) {
      PadRangeAxisChanged(GetActivePad());
   } else
   if (strcmp(linkname,"PadModified")==0) {
      if (IsRedrawBlocked()) return;
      if (!fbModifiedSignalFlag)
        QTimer::singleShot(1, this, SLOT(ProcessPadModifiedSignal()));
      fbModifiedSignalFlag = true;
   }
}

void TGo4ViewPanel::linkedUpdated(TGo4Slot* slot, TObject* obj)
{
   if (slot==0) return;

   if (IsRedrawBlocked()) return;

   Int_t kind = GetDrawKind(slot);

   TGo4Slot* padslot = slot;
   if (kind!=kind_PadSlot) padslot = slot->GetParent();

   if (((kind>0) && (kind<100)) || (kind==kind_Condition)) {
      TGo4Picture* padopt = GetPadOptions(padslot);

      if (padopt!=0) {
         padopt->SetContentModified(true);
         padopt->SetPadModified();

         // in this small period other objects can come,
         // therefore only one repain will be done
         ShootRepaintTimer();
      }
   }
}

void TGo4ViewPanel::linkedRemoved(TGo4Slot* slot, TObject* obj)
{
   if (slot==0) return;

   if (IsRedrawBlocked()) return;

   int kind = GetDrawKind(slot);

   if (kind==kind_ThisPad) {
      TPad* pad = (TPad*) obj;
      if (pad!=0) PadDeleted(pad);
      return;
   }

   TGo4Slot* padslot = slot;
   if (kind!=kind_PadSlot) padslot = slot->GetParent();

   CheckObjectsAssigments(GetSlotPad(padslot), padslot);

   if (((kind>0) && (kind<100)) || (kind==kind_Condition)) {
      CleanupGedEditor();
      TGo4Picture* padopt = GetPadOptions(padslot);
      if (padopt!=0) {
         padopt->SetPadModified();
         ShootRepaintTimer();
      }
   }
}

bool TGo4ViewPanel::IsAcceptDrag( const char * itemname, TClass * cl, int kind )
{
    if (cl==0) return false;

    int cando = Browser()->ItemCanDo(itemname);
    return TGo4BrowserProxy::CanDrawItem(cando) || cl->InheritsFrom(TGo4Condition::Class());
}

void TGo4ViewPanel::DropOnPad(TPad* pad,  const char * itemname, TClass * cl, int kind )
{
   if (cl==0) return;
   if (pad==0) pad = GetCanvas();

   if (cl->InheritsFrom(TGo4Fitter::Class())) {
      SetActivePad(pad);
      EditItem(itemname);
      return;
   }
   int cando = Browser()->ItemCanDo(itemname);
   if (!TGo4BrowserProxy::CanDrawItem(cando) && !cl->InheritsFrom(TGo4Condition::Class())) return;

   if (AddDrawObject(pad, kind_Link, itemname, 0, false, 0)==0) return;

   SetActivePad(pad);

   ShootRepaintTimer();

   Browser()->GetBrowserObject(itemname, 2);
}

// ****************************************************************

void TGo4ViewPanel::init()
{
   fPanelName = name();

   ActivePad = 0;

   fbEditorFrameVisible = false;
   fiSkipRedrawCounter = 0;
   fxRepaintTimerPad = 0;
   fxDoubleClickTimerPad = 0;
   fbFreezeTitle = false;
   fbApplyToAllFlag = false;
   fbCanvasCrosshair = false;
   fbCanvasEventstatus = false;

   fbCloneFlag = true;
   fbModifiedSignalFlag = false;
   fbLeaveFocusAfterCondEnd = false;

   // setup of marker editor
   fbMarkEditorVisible = false;
   fbTypingMode = true;
   MarkerPanel->setShown(false);
   MarkerPanel->setRadioButtonExclusive(false);
   MarkerPanel->setExclusive(false);
   fiMouseMode = kMouseROOT;
   fiPickCounter = 0;
   fbPickAgain = false;

   setCaption(GetPanelName());
   fxGo4QRootCanvas->setName(GetPanelName());
   fxGo4QRootCanvas->getCanvas()->SetName(GetPanelName());

   fSelectMenu = 0;
   fxPeditor = 0;
   fDummyHisto = 0;

   connect(fxGo4QRootCanvas, SIGNAL(SelectedPadChanged(TPad*)),
           this, SLOT(SetActivePad(TPad*)));
   connect(fxGo4QRootCanvas, SIGNAL(PadClicked(TPad*)),
           this, SLOT(PadClickedSlot(TPad*)));
   connect(fxGo4QRootCanvas, SIGNAL(PadDoubleClicked(TPad*)),
           this, SLOT(PadDoubleClickedSlot(TPad*)));
   connect(fxGo4QRootCanvas->getContextMenu(), SIGNAL(MenuCommandExecuted(TObject*, const char*)),
           this, SLOT(MenuCommandExecutedSlot(TObject*, const char*)));
   connect(fxGo4QRootCanvas, SIGNAL(CanvasLeaveEvent()),
           this, SLOT(RefreshButtons()));
}

void TGo4ViewPanel::CompleteInitialization()
{
   TGo4LockGuard lock;

   // create apropriate entry in OM
   UpdatePadStatus(GetCanvas(), true);

   //    fMenuBar
   fMenuBar = new QMenuBar( MenuFrame, "MenuBar" );
   fMenuBar->setMinimumWidth(50);
   fMenuBar->setFrameShape(QFrame::NoFrame);
    //File Menu
   QPopupMenu* FileMenu = new QPopupMenu( fMenuBar );
   fMenuBar->insertItem( "F&ile",FileMenu); // avoid conflict with mainwindow shortcut!
   FileMenu->insertItem("&Save as...", this, SLOT(SaveCanvas()));
   FileMenu->insertItem("Print...", this, SLOT(PrintCanvas()));
   FileMenu->insertItem("Produce &Picture", this, SLOT(ProducePicture()));
   FileMenu->insertItem("Produce &Graph from markers", this, SLOT(ProduceGraphFromMarkers()));

//   FileMenu->insertItem("Copy to T&Canvas in Memory", this, SLOT(SendToBrowser()));
//   FileMenu->insertItem("&Load marker setup...", this, SLOT(LoadMarkers()));
//   FileMenu->insertItem("Save &marker setup...", this, SLOT(SaveMarkers()));
   FileMenu->insertItem("Cl&ose", this, SLOT(close()));

    //Edit Menu
   QPopupMenu* EditMenu = new QPopupMenu( fMenuBar );
   fMenuBar->insertItem("&Edit", EditMenu);

   EditMenu->insertItem("Show Marker &editor", this, SLOT(SetMarkerPanel()), 0, ShowMarkEditorId);
   EditMenu->setItemChecked(ShowMarkEditorId, fbMarkEditorVisible);
   EditMenu->insertItem("Show &ROOT Attributes Editor", this, SLOT(StartRootEditor()), 0, ShowRootEditorId);
   EditMenu->setItemChecked(ShowRootEditorId, false);
   EditMenu->insertItem("Show &Event Status", this, SLOT(ShowEventStatus()), 0, EventStatusId);
   EditMenu->setItemChecked(EventStatusId, false);
   EditMenu->insertItem("Start &condition editor", this, SLOT(StartConditionEditor()));
   EditMenu->insertSeparator();
   EditMenu->insertItem("&1:1 coordinates ratio", this, SLOT(RectangularRatio()));
   EditMenu->insertItem("&Default pad margins", this, SLOT(DefaultPadMargin()));
   EditMenu->insertSeparator();
   EditMenu->insertItem("Clear &Markers", this, SLOT(ClearAllMarkers()));
   EditMenu->insertItem("Clear &Pad", this, SLOT(ClearActivePad()));
   EditMenu->insertItem("Clear C&anvas", this, SLOT(ClearCanvas()));

   fSelectMenu = new QPopupMenu( fMenuBar );
   fMenuBar->insertItem("&Select", fSelectMenu, SelectObjectId);
   connect(fSelectMenu, SIGNAL(activated(int)), this, SLOT(SelectMenuItemActivated(int)));

   QPopupMenu* OptionsMenu = new QPopupMenu( fMenuBar );
   fMenuBar->insertItem( "&Options",OptionsMenu);
   connect(OptionsMenu, SIGNAL(aboutToShow()), this, SLOT(AboutToShowOptionsMenu()));

   OptionsMenu->insertItem("&Crosshair", CrosshairId);
   OptionsMenu->insertItem("Super&impose", SuperimposeId);
   OptionsMenu->insertItem("Histogram &Statistics", StatisticsId);
   OptionsMenu->insertItem("Multiplot &Legend", SetLegendId);

   OptionsMenu->insertSeparator();
   OptionsMenu->insertItem("Histogram &Title", SetTitleId);
   OptionsMenu->insertItem("Draw Time", DrawTimeId);
   OptionsMenu->insertItem("Draw Date", DrawDateId);
   OptionsMenu->insertItem("Draw item name", DrawItemnameId);
   OptionsMenu->insertSeparator();
   OptionsMenu->insertItem("&Keep Viewpanel Title", FreezeTitleId);
   OptionsMenu->insertItem("Set &Viewpanel Title...", SetTitleTextId);
   connect(OptionsMenu, SIGNAL(activated(int)), this, SLOT(OptionsMenuItemActivated(int)));

   QCheckBox* box1 = new QCheckBox(MenuFrame, "ApplyToAllCheck");
   box1->setText("Apply to all");
   connect(box1, SIGNAL(toggled(bool)), this, SLOT(ApplyToAllToggled(bool)));

   fAutoScaleCheck = new QCheckBox(MenuFrame, "AutoScaleCheck");
   fAutoScaleCheck->setText("AutoScale");
   fAutoScaleCheck->setChecked(GetPadOptions(GetCanvas())->IsAutoScale());
   connect(fAutoScaleCheck, SIGNAL(toggled(bool)), this, SLOT(AutoScaleToggled(bool)));

   QHBoxLayout* menugrid = new QHBoxLayout(MenuFrame, 0, -1, "MenuLayout");
   menugrid->addWidget(fMenuBar, 10, Qt::AlignLeft);
   menugrid->addWidget(box1, 1, Qt::AlignRight);
   menugrid->addWidget(fAutoScaleCheck, 1, Qt::AlignRight);
   TGo4ViewPanelLayout->addMultiCellLayout(menugrid, 0, 0, 0, 1);

   connect(TGo4WorkSpace::Instance(), SIGNAL(panelSignal(TGo4ViewPanel*, TPad*, int)),
           this, SLOT(panelSlot(TGo4ViewPanel*, TPad*, int)));

   // status widget
   CanvasStatus = new QStatusBar(this, "Canvas Status");
   TGo4ViewPanelLayout->addMultiCellWidget( CanvasStatus, 3, 3, 0, 1 );
   CanvasStatus->setShown(false);

   // setup of root editor
   fxRooteditor = new TQRootWindow(EditorFrame,"rootwrapperwindow");
   QVBoxLayout* gedlayout = new QVBoxLayout(EditorFrame);
   if (fxRooteditor) gedlayout->addWidget(fxRooteditor);
   EditorFrame->polish();
   EditorFrame->update();
   EditorFrame->show();
   EditorFrame->setShown(fbEditorFrameVisible);

   //RefreshButtons();

   SetActivePad(GetCanvas());

   SetPadDefaults(GetCanvas());

   go4sett->restorePanelSize(this);
   fbCloneFlag = go4sett->getCloneFlag();
}

void TGo4ViewPanel::destroy()
{
   TGo4LockGuard lock;

  // prevent problems with root's subeditor cache
   if(fxPeditor!=0) {
      fxPeditor->DeleteEditors();
      delete fxPeditor;
      fxPeditor = 0;
   }

   if (fDummyHisto!=0) {
      delete fDummyHisto;
      fDummyHisto = 0;
   }

   // we should delete all markers first, while they
   // have internal reference on the pad, which will be
   // deleted by the net canvas->Clear() call
   ProcessMarkersClear(GetCanvas(), true);

   GetCanvas()->Clear();

   fxRepaintTimerPad = 0;

   CallPanelFunc(panel_Deleted);

   if (gPad!=0)
      if (IsPanelPad((TPad*)gPad))
          gPad = 0;

   if (gROOT->GetSelectedPad()!=0)
      if (IsPanelPad((TPad*)gROOT->GetSelectedPad()))
         gROOT->SetSelectedPad(0);
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
   if (padslot==0) return QString::null;
   return QString(padslot->GetPar("::SelMarker"));
}

int TGo4ViewPanel::GetSelectedMarkerIndex(TPad* pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot==0) return -1;
   Int_t selindex = -1;
   if (!padslot->GetIntPar("::SelIndex",selindex)) return -1;
   return selindex;
}

TGo4Slot* TGo4ViewPanel::GetSelectedSlot(TPad* pad, int* selkind, TObject** selobj)
{
   if (selkind!=0) *selkind = kind_None;
   if (selobj!=0) *selobj = 0;

   TGo4Slot* padslot = GetPadSlot(pad);
   QString selname = GetSelectedMarkerName(pad);
   int selindex = GetSelectedMarkerIndex(pad);

   if ((padslot==0) || (selname.length()==0)) return 0;

   for (int n=0; n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      TObject* obj = subslot->GetAssignedObject();

      if (drawkind==kind_Link) {
         if ((obj!=0) && (dynamic_cast<TGo4Condition*> (obj)!=0))
            drawkind = kind_Condition;
      }

      if ((drawkind==kind_Marker) || (drawkind==kind_Window) ||
          (drawkind==kind_Poly) || (drawkind==kind_Latex) ||
          (drawkind==kind_Arrow) || (drawkind==kind_Condition)) {

        if ((obj==0) || (selname!=obj->GetName())) continue;

        if (drawkind==kind_Condition) {
           TGo4Condition* selcond = dynamic_cast<TGo4Condition*> (obj);
           if (obj->InheritsFrom(TGo4CondArray::Class())) {
              TGo4CondArray* arr = (TGo4CondArray*) obj;
              selcond = 0;
              if ((selindex>=0) &&
                  (selindex<arr->GetNumber()))
                     selcond = arr->At(selindex);
           }
           drawkind = kind_None;
           obj = selcond;
           if (selcond!=0)
             if (selcond->InheritsFrom(TGo4WinCond::Class())) drawkind = kind_Window; else
             if (selcond->InheritsFrom(TGo4PolyCond::Class())) drawkind = kind_Poly;
        }
        if (selkind!=0) *selkind = drawkind;
        if (selobj!=0) *selobj = obj;
        return subslot;
      }
   }
   return 0;
}

bool TGo4ViewPanel::IsConditionSelected(TPad* pad)
{
   TGo4Slot* slot = GetSelectedSlot(pad, 0, 0);
   if (slot==0) return false;

   return (GetDrawKind(slot)==kind_Condition);
}

TPad* TGo4ViewPanel::FindPadWithItem(const char* itemname)
{
   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      int drawkind = GetDrawKind(subslot);
      if ((drawkind==kind_Link) || (drawkind==kind_Condition)) {
         const char* linkname = GetLinkedName(subslot);
         if (linkname!=0)
            if (strcmp(linkname, itemname)==0) return GetSlotPad(subslot->GetParent());
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
      if ((drawkind==kind_Link) || (drawkind==kind_Condition)) {
         const char* linkname = GetLinkedName(subslot);
         if ((linkname!=0) && (strcmp(linkname, itemname)==0)) {
            delslots.Add(subslot);
            TGo4Slot* padslot = subslot->GetParent();
            TGo4Picture* padopt = GetPadOptions(padslot);
            if (padopt!=0) padopt->SetPadModified();
        }
      }
   }
   if (delslots.GetLast()>=0) {
     delslots.Delete();
     ShootRepaintTimer();
   }
}

void TGo4ViewPanel::SetSelectedMarker(TPad* pad, const QString& selname, int selindex)
{
   TGo4LockGuard lock;

   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot==0) return;

   if (selname.length()==0) selindex = -1;

   int oldselindex = GetSelectedMarkerIndex(pad);
   QString oldselname = GetSelectedMarkerName(pad);

   TGo4Slot* oldsel = GetSelectedSlot(pad, 0, 0);
   if (oldsel!=0)
      SetSpecialDrawOption(oldsel, 0);

   if (selname.length()>0)
      padslot->SetPar("::SelMarker", selname.latin1());
   else
      padslot->RemovePar("::SelMarker");

   if (selindex>-1)
      padslot->SetIntPar("::SelIndex", selindex);
   else
      padslot->RemovePar("::SelIndex");

   int newselkind = 0;
   TObject* newselobj = 0;
   TGo4Slot* newselslot = GetSelectedSlot(pad, &newselkind, &newselobj);

   if ((selindex>=0) && (newselslot!=0)) {
      QString drawopt("sel=");
      drawopt += QString::number(selindex);
      SetSpecialDrawOption(newselslot, drawopt.latin1());
   }

   if (((oldselindex!=selindex) || (oldselname!=selname)) &&
       ((oldselindex>=0) || (selindex>=0))) {
         MarkPadModified(pad);
         ShootRepaintTimer(pad);
      } else
      if (newselobj!=0) {
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
   if (padslot==0) return;

   bool find = false;

   for(int n=0;n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if ((drawkind==kind_Marker) ||
          (drawkind==kind_Window) ||
          (drawkind==kind_Poly)) {
             TObject* obj = subslot->GetAssignedObject();
             if ((obj!=0) && (strcmp(obj->GetName(),name)==0)) {
                SetSelectedMarker(pad, name, -1);
                SetActiveObj(pad, drawkind, subslot);
                find = true;
                break;
             }
          }
      if (drawkind!=kind_Condition) continue;
      TGo4Condition* cond = dynamic_cast<TGo4Condition*>
         (subslot->GetAssignedObject());
      if (cond==0) continue;

      if (strcmp(cond->GetName(),name)==0) {
         SetSelectedMarker(pad, name, -1);

         if (cond->InheritsFrom(TGo4PolyCond::Class()))
           drawkind=kind_Poly;
         else
           drawkind=kind_Window;

         SetActiveObj(pad, drawkind, subslot);
         find = true;
         break;
      }

      TGo4CondArray* arr = dynamic_cast<TGo4CondArray*> (cond);
      if (arr==0) continue;

      for(int ncon=0;ncon<arr->GetNumber();ncon++)
        if (strcmp(arr->At(ncon)->GetName(),name)==0) {
           SetSelectedMarker(pad, arr->GetName(), ncon);
           if (arr->At(ncon)->InheritsFrom(TGo4PolyCond::Class()))
             drawkind=kind_Poly;
           else
             drawkind=kind_Window;
           SetActiveObj(pad, drawkind, subslot);
           find = true;
           break;
        }
   }
   if (find) RefreshButtons();
}

void TGo4ViewPanel::StartConditionEditing(TPad* pad)
{
   TGo4LockGuard lock;

   int selectedkind;
   TGo4Slot* selslot = GetSelectedSlot(GetActivePad(), &selectedkind, 0);

   if (selslot==0) return;

   if (selectedkind==kind_Window) {
      FreezeMode->setChecked(false);
      RegionB->setOn(true);
      fbLeaveFocusAfterCondEnd = true;
   } else
   if (selectedkind==kind_Poly) {
      FreezeMode->setChecked(false);
      PolyB->setOn(true);
      fbLeaveFocusAfterCondEnd = true;
   } else
     return;

   MarkPadModified(pad);
   ShootRepaintTimer(pad);
}

void TGo4ViewPanel::SwitchMarkerButton(int kind, bool on)
{
   if(!fbTypingMode) return;
   CompleteMarkerEdit(GetActivePad());
   if (!on) {
      SetMouseMode(kMouseROOT);
   } else {
     fiPickCounter = 0;
     switch(kind) {
        case kind_Marker: SetMouseMode(kMousePickCursor); break;
        case kind_Window: SetMouseMode(kMousePickLimits); break;
        case kind_Poly: SetMouseMode(kMousePickPolygon); break;
        case kind_Latex: SetMouseMode(kMousePickLatex); break;
        case kind_Arrow: SetMouseMode(kMouseDraw); break;
        default: SetMouseMode(kMousePickCursor); break;
     }
     int selectedkind;
     TGo4Slot* selslot = GetSelectedSlot(GetActivePad(), &selectedkind, 0);
     if (selectedkind!=kind)
        SetSelectedMarker(GetActivePad(), "", -1);
   }

   RefreshButtons();
}

void TGo4ViewPanel::SetCursorMode( bool on )
{
   SwitchMarkerButton(kind_Marker, on);
}

void TGo4ViewPanel::SetRegionMode( bool on )
{
   SwitchMarkerButton(kind_Window, on);
}

void TGo4ViewPanel::SetPolygonMode( bool on )
{
   SwitchMarkerButton(kind_Poly, on);
}

void TGo4ViewPanel::SetLateXMode( bool on )
{
   SwitchMarkerButton(kind_Latex, on);
}

void TGo4ViewPanel::SetDrawingMode( bool on )
{
   SwitchMarkerButton(kind_Arrow, on);
}

void TGo4ViewPanel::SetFreezeMouseMode( bool on )
{
   if(!fbTypingMode) return;
   fbPickAgain = on;
   RefreshButtons();
}

void TGo4ViewPanel::RefreshButtons()
{
   TGo4LockGuard lock;

   MarkerPanel->setShown(fbMarkEditorVisible);
   fxGo4QRootCanvas->setMaskDoubleClick(fbMarkEditorVisible);

//   if(!fbMarkEditorVisible) return;

   bool iscondition = IsConditionSelected(GetActivePad());

   fbTypingMode=false;
   GetConditionBtn->setShown(iscondition);
   InfoConditionBtn->setShown(iscondition);
   EditConditionBtn->setShown(iscondition);
   SetConditionBtn->setShown(iscondition);
   if (iscondition) {
      TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
      TGo4Condition* cond = slot==0 ? 0 :
        dynamic_cast<TGo4Condition*> (slot->GetAssignedObject());
      ModifyConditionBtn->setShown((cond!=0) && (cond->IsChanged()!=0));

      QString iconname = "right.png";
      QString tooltip = "Refresh condition from analysis";
      if (!BrowserItemRemote(GetLinkedName(slot))) {
         iconname = "refresh.png";
         tooltip = "Refresh condition from source";
      }
      GetConditionBtn->setIconSet( QIconSet( QPixmap::fromMimeSource(iconname) ) );
      QToolTip::remove(GetConditionBtn);
      QToolTip::add(GetConditionBtn, tooltip);

   } else
      ModifyConditionBtn->setShown(false);

   switch(GetMouseMode()) {
      case kMouseROOT:
         CursorB->setOn(false);
         RegionB->setOn(false);
         LatexB->setOn(false);
         DrawB->setOn(false);
         PolyB->setOn(false);
         break;
      case kMousePickCursor:
         CursorB->setOn(true);
         RegionB->setOn(false);
         LatexB->setOn(false);
         DrawB->setOn(false);
         PolyB->setOn(false);
         break;
      case kMousePickLimits:
         CursorB->setOn(false);
         RegionB->setOn(true);
         LatexB->setOn(false);
         DrawB->setOn(false);
         PolyB->setOn(false);
         break;
      case kMousePickPolygon:
         CursorB->setOn(false);
         RegionB->setOn(false);
         LatexB->setOn(false);
         DrawB->setOn(false);
         PolyB->setOn(true);
         break;
      case kMousePickLatex:
         CursorB->setOn(false);
         RegionB->setOn(false);
         LatexB->setOn(true);
         DrawB->setOn(false);
         PolyB->setOn(false);
         break;
      case kMouseDraw:   // currently, we only draw arrows:
         CursorB->setOn(false);
         RegionB->setOn(false);
         LatexB->setOn(false);
         DrawB->setOn(true);
         PolyB->setOn(false);
         break;
      default:
         CursorB->setOn(false);
         RegionB->setOn(false);
         LatexB->setOn(false);
         DrawB->setOn(false);
         PolyB->setOn(false);
         break;
   }; // switch()
   FreezeMode->setChecked(fbPickAgain);

   SelectedMarkerCmb->clear();
   SelectedMarkerCmb->insertItem("new");

   TGo4Slot* slot = GetPadSlot(GetActivePad());
   int findindx = -1;

   QString selname = GetSelectedMarkerName(GetActivePad());
   int selindex = GetSelectedMarkerIndex(GetActivePad());

   if (slot!=0)
     for (int n=0; n<slot->NumChilds();n++) {
       TGo4Slot* subslot = slot->GetChild(n);
       int drawkind = GetDrawKind(subslot);
       if ((drawkind==kind_Marker) || (drawkind==kind_Window) ||
           (drawkind==kind_Poly) || (drawkind==kind_Condition)) {
          TObject* obj = subslot->GetAssignedObject();
          if (obj==0) continue;

          if (obj->InheritsFrom(TGo4CondArray::Class())) {
             TGo4CondArray* arr = (TGo4CondArray*) obj;
             for(int ncon=0;ncon<arr->GetNumber();ncon++) {
                TGo4Condition* sub = arr->At(ncon);
                QString fullname(arr->GetName());
                fullname+="/Sub";
                fullname+=QString::number(ncon);
                SelectedMarkerCmb->insertItem(fullname);

                if ((selname==obj->GetName()) &&
                    (selindex==ncon))
                       findindx = SelectedMarkerCmb->count()-1;
             }
          } else {
             SelectedMarkerCmb->insertItem(obj->GetName());
             if (selname==obj->GetName())
               findindx = SelectedMarkerCmb->count()-1;
          }
        }
     }

   if (findindx<0) {
      findindx = 0;
      SetSelectedMarker(GetActivePad(), "", -1);
   }

   SelectedMarkerCmb->setCurrentItem(findindx);
   DelSelectedMarker->setEnabled((findindx>0) /*&& !iscondition*/);

   if (fbMarkEditorVisible) {
      MarkerPanel->polish();
      MarkerPanel->update();
      MarkerPanel->show();
   }
   fbTypingMode=true;
}

void TGo4ViewPanel::SelectedMarkerCmb_activated(int indx)
{
   if (!fbTypingMode) return;
   if (indx==0)
      SetSelectedMarker(GetActivePad(), "", -1);
   else {
     QString selname = SelectedMarkerCmb->text(indx);
     int selindex = -1;
     int p = selname.find("/Sub");
     if (p>0) {
        selindex = atoi(selname.latin1()+p+4);
        selname.truncate(p);
     } else
        selindex = -1;
     SetSelectedMarker(GetActivePad(), selname, selindex);
   }

   int drawkind = 0;
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), &drawkind, 0);

   if ((slot!=0) && (drawkind>0)) {
      SetActiveObj(GetActivePad(), drawkind, slot);
      SwitchMarkerButton(drawkind, true);
   } else
      SwitchMarkerButton(kind_Marker, false);
}

void TGo4ViewPanel::DelSelectedMarker_clicked()
{
   if (!fbTypingMode) return;
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0,0);
   if (slot==0) return;
   delete slot;
   SetSelectedMarker(GetActivePad(), "", -1);
   RedrawPanel(GetActivePad(), true);
}

void TGo4ViewPanel::SetMarkerPanel()
{
    fbMarkEditorVisible= !fMenuBar->isItemChecked(ShowMarkEditorId);
    fMenuBar->setItemChecked(ShowMarkEditorId, fbMarkEditorVisible);
    if(!fbMarkEditorVisible) {
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
    if (!fbTypingMode) return;

    TGo4Slot* slot = GetPadSlot(GetActivePad());
    if (slot==0) return;

    for (int n=0; n<slot->NumChilds();n++) {
       TGo4Slot* subslot = slot->GetChild(n);
       int drawkind = GetDrawKind(subslot);
       TObject* obj = subslot->GetAssignedObject();
       if ((drawkind<kind_Condition) || (obj==0)) continue;
       switch (drawkind) {
          case kind_Marker: obj->Print("*"); break;
          case kind_Latex: obj->Print("*"); break;
          case kind_Arrow: obj->Print("go4log"); break;
          case kind_Window:
          case kind_Poly:
          case kind_Condition:
             obj->Print("go4log-limits-stats"); break;
       }
    }
}

void TGo4ViewPanel::ClearAllMarkers()
{
   TGo4LockGuard lock;

   TPad* pad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (pad==0) pad = GetCanvas();

   ProcessMarkersClear(pad, IsApplyToAllFlag());

   RedrawPanel(pad, true);
}

void TGo4ViewPanel::ProcessMarkersClear(TPad *pad, bool withsubpads)
{
   if (pad==0) return;

   DeleteDrawObjects(pad, kind_Marker);
   DeleteDrawObjects(pad, kind_Window);
   DeleteDrawObjects(pad, kind_Poly);
   DeleteDrawObjects(pad, kind_Latex);
   DeleteDrawObjects(pad, kind_Arrow);
   if (!withsubpads) return;

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return;
   TGo4Iter iter(slot, true);
   while (iter.next()) {
      TPad* subpad = GetSlotPad(iter.getslot());
      if (subpad!=0) ProcessMarkersClear(subpad, false);
   }
}


void TGo4ViewPanel::GetConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot)!=kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname==0) return;

   Browser()->GetBrowserObject(itemname, 2);

   RefreshButtons();
}

void TGo4ViewPanel::InfoConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot)!=kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname!=0) ShowItemInfo(itemname);
}

void TGo4ViewPanel::EditConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot)!=kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname!=0) EditItem(itemname);
}

void TGo4ViewPanel::SetConditionBtn_clicked()
{
   TGo4Slot* slot = GetSelectedSlot(GetActivePad(), 0, 0);
   if (GetDrawKind(slot)!=kind_Condition) return;
   const char* itemname = GetLinkedName(slot);
   if (itemname!=0) {
      UpdateItemInAnalysis(itemname);
      TGo4Condition* cond = dynamic_cast<TGo4Condition*> (slot->GetAssignedObject());
      if (cond!=0) cond->SetChanged(kFALSE);
      RefreshButtons();
   }
}

void TGo4ViewPanel::SaveMarkers()
{
    QFileDialog* fd = new QFileDialog( this, "Save Markers", TRUE );
    fd->setCaption("Save Markers of active pad into ");
    fd->setMode( QFileDialog::AnyFile);
    fd->setName( "Save to file: ");
    fd->setFilter( "ROOT file (*.root)");
    if ( fd->exec() == QDialog::Accepted ) {
       QString filename = fd->selectedFile();
       if (!filename.endsWith(".root")) filename.append(".root");
//       fxTGo4PreviewPanelSlots->SaveMarkerSetup(filename,"Markersetup");
    }
    delete fd;
}

void TGo4ViewPanel::LoadMarkers()
{
    QFileDialog* fd = new QFileDialog( this, "Load Markers", TRUE );
    fd->setCaption("Load Marker setup from:");
    fd->setMode( QFileDialog::ExistingFile );
    fd->setName( "Load from file: ");
    fd->setFilter( "ROOT file (*.root)");
    if ( fd->exec() == QDialog::Accepted ) {
        QString filename = fd->selectedFile();
//        fxTGo4PreviewPanelSlots->LoadMarkerSetup(filename,"Markersetup");
    }
    delete fd;
}


void TGo4ViewPanel::SetActivePad(TPad* pad)
{
   TGo4LockGuard lock;

   if (ActivePad!=pad)
     CompleteMarkerEdit(ActivePad);

   if (pad==0) {
      GetCanvas()->SetSelected(0);
      GetCanvas()->SetSelectedPad(0);
      GetCanvas()->Update();
      return;
   }

   ActivePad = pad;
   ActivePad->cd();
   GetCanvas()->SetSelectedPad(ActivePad);

   TGo4WorkSpace::Instance()->SetSelectedPad(ActivePad);

   BlockPanelRedraw(true);
   ActivePad->Update();
   BlockPanelRedraw(false);

   DisplayPadStatus(ActivePad);

   UpdatePanelCaption();
   CallPanelFunc(panel_Activated, ActivePad);
}

void TGo4ViewPanel::PadClickedSlot(TPad* pad)
{
   TGo4LockGuard lock;

   SetActivePad(pad);

   if (pad==0) return;

   Int_t px = pad->GetEventX();
   Int_t py = pad->GetEventY();
   Double_t x = pad->PadtoX(pad->AbsPixeltoX(px));
   Double_t y = pad->PadtoY(pad->AbsPixeltoY(py));
   bool docreate = GetSelectedMarkerName(pad).length()==0;
   bool docheck = false;

//   cout << "PadClickedSlot( px = " << px << " py = " << py << endl;

   switch(fiMouseMode) {
     case kMouseROOT: {
        TObject *obj = GetCanvas()->GetSelected();
//        if (obj!=0)
//           cout << "  obj = " << obj->GetName()
//                << "  class = " << obj->ClassName() << endl;

        if (obj!=0)
        if (obj->InheritsFrom(TGo4Marker::Class()) ||
            obj->InheritsFrom(TGo4WinCondView::Class()) ||
            obj->InheritsFrom(TGo4PolyCondView::Class()))
              SetSelectedMarkerByMouseClick(pad, obj->GetName());

        break;
     }


     case kMousePickCursor: {
       // we have a click on our working pad, get coordinates:
       gROOT->SetEditorMode("");

       if(docreate) {
          TGo4Marker* mark = new TGo4Marker(x,y,28);
          AddMarkerObj(pad, kind_Marker, mark);
          Int_t ix = TGo4Marker::GetInstances()-1;
          mark->SetMarkerColor((ix)%6 +2);
          mark->SetHistogram(GetPadHistogram(pad));
          if (!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
        } else {
          TGo4Marker* mark = dynamic_cast<TGo4Marker*> (GetActiveObj(pad, kind_Marker));
          if(mark!=0) {
             mark->SetX(x);
             mark->SetY(y);
          }
          if (!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
        }

        RedrawPanel(pad, true);
        break;
     }

     case kMousePickLimits: {
        gROOT->SetEditorMode("");
        TGo4WinCond* conny = 0;
        Double_t xmin(x), xmax(x), ymin(y), ymax(y);
        if(fiPickCounter==0) {
           // pick the first time after enabling limits record:
           if(docreate) {
              TH1* hist = GetPadHistogram(pad);
              bool fbTwoDimRegion=(hist!=0) && (hist->GetDimension()>1);
              int ix = GetNumMarkers(pad, kind_Window);
              QString name = "Region " + QString::number(ix+1);
              conny = new TGo4WinCond(name.latin1());
              if(fbTwoDimRegion)
                 conny->SetValues(0,0,0,0);
              else
                 conny->SetValues(0,0);
              conny->SetLineColor(ix%6 +2);
              conny->SetFillColor(ix%6 +2);
              conny->SetFillStyle(3002);
              conny->SetWorkHistogram(hist);
              // adjust two dim region to one dim defaults
              conny->SetYRMSDraw(fbTwoDimRegion  && conny->IsXRMSDraw());
              conny->SetYMeanDraw(fbTwoDimRegion && conny->IsXMeanDraw());
              conny->SetYMaxDraw(fbTwoDimRegion && conny->IsXMaxDraw());
              AddMarkerObj(pad, kind_Window, conny);
           } else
              conny = dynamic_cast<TGo4WinCond*> (GetActiveObj(pad, kind_Window));
//           cout << "Start wincond = " << (conny ? conny->GetName() : "null") << endl;
           if(conny==0) return;
           fiPickCounter++;
        } else
        if (fiPickCounter==1) {
           conny = dynamic_cast<TGo4WinCond*> (GetActiveObj(pad, kind_Window));
//           cout << "Stop wincond = " << (conny ? conny->GetName() : "null") << endl;
           if(conny==0) return;
           xmin = conny->GetXLow();
           ymin = conny->GetYLow();
           fiPickCounter=0;
           if(!fbPickAgain)  fiMouseMode=kMouseROOT;
           docheck = true;
        } else {
           cout <<"TGo4ViewPanel:MouseClick() NEVER COME HERE" << endl;
           return;
        }
        // do not change original condition dimension
        if(conny->GetDimension()>1)
           conny->SetValues(xmin,xmax,ymin,ymax);
        else
           conny->SetValues(xmin, xmax);

//        cout << "XUP = " << conny->GetXUp() << endl;
//        cout << "YUP = " << conny->GetYUp() << endl;

        TGo4Slot* condslot = GetSelectedSlot(pad, 0, 0);
        if (GetDrawKind(condslot)==kind_Condition) {
           TGo4Condition* maincond = dynamic_cast<TGo4Condition*> (condslot->GetAssignedObject());
           if (maincond!=0) maincond->SetChanged(kTRUE);
        }

        conny->SetChanged(kTRUE);

        RedrawPanel(pad, true);
        break;
     }

     case kMousePickPolygon: {
        gROOT->SetEditorMode("");
        TGo4PolyCond* cond = 0;

        if(fiPickCounter==0) {
           // pick the first time after enabling limits record:
           if(docreate) {
              TH1* hist = GetPadHistogram(pad);
              int ix = GetNumMarkers(pad, kind_Poly);
              QString name = "Polygon " + QString::number(ix+1);
              cond = new TGo4PolyCond(name.latin1());
              AddMarkerObj(pad, kind_Poly, cond);
              cond->SetWorkHistogram(hist);
           } else {
              cond = dynamic_cast<TGo4PolyCond*> (GetActiveObj(pad, kind_Poly));
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

        if (cond!=0) {
           TCutG* cut = cond->GetCut(kFALSE);

           // this insert point in last mouse position
           if (cut==0) {
              cut = new TCutG(TGo4PolyCond::NextAvailableName(), 1);
              cut->SetPoint(0, x, y);
              cond->SetValuesDirect(cut);
           } else {
              cut->InsertPoint();
           }

           cond->SetChanged(kTRUE);

           int ix = GetNumMarkers(pad, kind_Poly);

           cond->SetLineColor(ix%6 +2);
           cond->SetFillColor(ix%6 +2);
           cond->SetFillStyle(3002);
        }

        // mark condition
        TGo4Slot* condslot = GetSelectedSlot(pad, 0, 0);
        if (GetDrawKind(condslot)==kind_Condition) {
           TGo4Condition* maincond = dynamic_cast<TGo4Condition*> (condslot->GetAssignedObject());
           if (maincond!=0) maincond->SetChanged(kTRUE);
        }

        RedrawPanel(pad, true);
        break;
     }

     case kMousePickLatex: {
        gROOT->SetEditorMode("");
        if(docreate) {
           int ix = GetNumMarkers(pad, kind_Latex);
           QString name = QString("Label ") + QString::number(ix+1);
           bool ok;
           QString txt = QInputDialog::getText("Enter new LaTeX label text:",
                         name, QLineEdit::Normal, QString::null, &ok);
           if (ok && (txt.length()>0)) {
              TLatex* latex = new TLatex(x,y, name.latin1());
              latex->SetName(name.latin1());
              latex->SetTitle(txt.latin1());
              AddMarkerObj(pad, kind_Latex, latex);
           } else {
              fiMouseMode=kMouseROOT;
           }
        } else {
           TLatex* latex = dynamic_cast<TLatex*> (GetActiveObj(pad, kind_Latex));
           if(latex!=0) {
              latex->SetX(x);
              latex->SetY(y);
           }
        }
        if(!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
        RedrawPanel(pad, true);
        break;
     }

     case kMouseDraw: {
        gROOT->SetEditorMode("");
        if(fiPickCounter==0) {
           // pick the first time after enabling limits record:
           TArrow* arrow = new TArrow(x,y,x,y);
           AddMarkerObj(pad, kind_Arrow, arrow);
           fiPickCounter++;
        } else
        if (fiPickCounter==1) {
           TArrow* arrow = dynamic_cast<TArrow*> (GetActiveObj(pad, kind_Arrow));
           if(arrow!=0) {
              arrow->SetX2(x);
              arrow->SetY2(y);
           }
           if(!fbPickAgain) fiMouseMode=kMouseROOT; // reset pick
           fiPickCounter=0;
        } else {
           cout <<"TGo4ViewPanel:MouseClick() NEVER COME HERE" << endl;
           return;
        }
        // do not change original condition dimension
        RedrawPanel(pad, true);
        break;
     }
   }

   if (docheck) CheckActionAtTheEnd(pad);
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
   bool needredraw = false;
   bool needrefresh = false;
   bool docheck = false;
   bool candelete = !IsConditionSelected(pad);

   switch(fiMouseMode) {
     case kMousePickLimits: {
        if (fiPickCounter>0) {
           TGo4WinCond* cond = dynamic_cast<TGo4WinCond*> (GetActiveObj(pad, kind_Window));
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
         TGo4PolyCond* cond = dynamic_cast<TGo4PolyCond*> (GetActiveObj(pad, kind_Poly));
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
               cond->SetLineColor(ix%6 +2);
               cond->SetFillColor(ix%6 +2);
               cond->SetFillStyle(3002);
            }

            if (delcond && candelete)
              DeleteDrawObject(pad, cond);

            needredraw = true;
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
   if (needredraw) RedrawPanel(pad, true); else
   if (needrefresh) RefreshButtons();

   if (docheck) CheckActionAtTheEnd(pad);

   return res;
}

void TGo4ViewPanel::PadDoubleClickedSlot(TPad* pad)
{
   if (CompleteMarkerEdit(pad)) return;

   if (GetNumberOfPads(GetCanvas())<=1) return;

   if (fxDoubleClickTimerPad!=0) return;
   fxDoubleClickTimerPad = pad;
   QTimer::singleShot(100, this, SLOT(ProcessPadDoubleClick()));
}

void TGo4ViewPanel::CanvasDropEventSlot(QDropEvent* event, TPad* pad)
{
   emit widgetService(this, service_DropEvent, (const char*) pad, event);
}

void TGo4ViewPanel::CanvasStatusEventSlot(const char* message)
{
   if (CanvasStatus!=0)
     CanvasStatus->message(message);
}

void TGo4ViewPanel::ProcessPadDoubleClick()
{
   if (fxDoubleClickTimerPad==0) return;

   TGo4Picture pic;
   MakePictureForPad(&pic, fxDoubleClickTimerPad, true);
   fxDoubleClickTimerPad = 0;

   if (pic.GetNumObjNames()==0) return;

   TGo4ViewPanel* newpanel = CreateViewPanel();
   newpanel->ProcessPictureRedraw("", newpanel->GetCanvas(), &pic);
   newpanel->ShootRepaintTimer();
}

void TGo4ViewPanel::MenuCommandExecutedSlot(TObject* obj, const char* cmdname)
{
   TPad* pad = dynamic_cast<TPad*> (obj);
   if (pad!=0)
      UpdatePadStatus(pad, true);

   Browser()->Scan_gROOT();

   if ((strcmp(cmdname, "UnZoom")==0) && obj->InheritsFrom(TAxis::Class())) {

      // this code is done specially to treat unzoom in the THStack

      TGo4Iter iter(GetPanelSlot(), kTRUE);
      TGo4Slot* subslot = 0;

      do  {
         if (subslot==0) subslot = GetPanelSlot();
                   else  subslot = iter.getslot();

         TPad* subpad = GetSlotPad(subslot);
         if (subpad==0) continue;

         TGo4Slot* sislot = GetSuperimposeSlot(subslot);
         if (sislot==0) continue;

         THStack* hs = dynamic_cast<THStack*> (sislot->GetAssignedObject());
         if (hs==0) continue;

         TH1* framehisto = hs->GetHistogram();
         if (framehisto==0) continue;

         if (framehisto->GetXaxis()!=obj) continue;

         TIter next(hs->GetHists());
         TH1* hs_h1 = 0;
         while ( (hs_h1 = (TH1*) next()) !=0 )
            hs_h1->GetXaxis()->UnZoom();

         return;
      } while (iter.next());
   }
}

void TGo4ViewPanel::DoCanvasResizeSlot()
{
   ResizeGedEditor();
}

void TGo4ViewPanel::SaveCanvas()
{
   ServiceCall("SavePanelCanvas");
}

void TGo4ViewPanel::ProduceGraphFromMarkers()
{
   // get list of markers here
   TObjArray markers;
   CollectSpecialObjects(GetActivePad(), &markers, kind_Marker);
   Int_t npts=markers.GetEntries();
   //cout <<"Found "<<npts<<" markers in pad" <<endl;
   if(npts==0) return;
   // create arrays of length
   Double_t x[npts];
   Double_t y[npts];
   // copy marker values to array:
   for(Int_t j=0;j<npts;++j)
    {
        TGo4Marker* mark=dynamic_cast<TGo4Marker*>(markers[j]);
        if(mark==0)
            {
                cout <<"NEVER COME HERE: no marker at index "<<j << endl;
                return;
            }
        x[j]=mark->GetX();
        y[j]=mark->GetY();
        //cout <<"Set point "<<j <<" to x="<<x[j]<<", y="<<y[j]<<endl;
    }

   // create graph from points array:
   TString grname=GetPanelName()+TString("-Markergraph");
   TGraph* graf = new TGraph(npts,x,y);
   graf->SetName(grname.Data());
   graf->SetMarkerStyle(28);
   SaveObjectInMemory("", graf);
}


void TGo4ViewPanel::ProducePicture()
{
   TGo4Picture* pic = new TGo4Picture(GetPanelName(), "Copy of picture");

   MakePictureForPad(pic, GetCanvas(), false);

   SaveObjectInMemory("", pic);
}

void TGo4ViewPanel::MakePictureForPad(TGo4Picture* pic, TPad* pad, bool useitemname)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   TGo4Slot* slot = GetPadSlot(pad);
   if ((padopt==0) || (slot==0)) return;

   pic->CopyOptionsFrom(padopt);

   int objnamenum = 0;

   for(int n=0;n<slot->NumChilds();n++) {

      TGo4Slot* subslot = slot->GetChild(n);
      int kind = GetDrawKind(subslot);

      if ((kind==kind_Arrow) || (kind==kind_Latex) ||
          (kind==kind_Marker) || (kind==kind_Window) ||
          (kind==kind_Poly) || (kind==kind_Specials)) {
         TObject* obj = subslot->GetAssignedObject();
         const char* drawopt = GetSpecialDrawOption(subslot);
         if (obj!=0) {
            obj = obj->Clone();
            TGo4Marker* mark = dynamic_cast<TGo4Marker*> (obj);
            if (mark!=0) mark->DeletePainter();
            TGo4Condition* cond = dynamic_cast<TGo4Condition*> (obj);
            if (cond!=0) cond->DeletePainter();
            pic->AddSpecialObject(obj, drawopt);
         }
         continue;
      }

      if ((kind!=kind_Link) && (kind!=kind_Condition)) continue;

      const char* drawopt = padopt->GetDrawOption(objnamenum++);

      if (useitemname) {
         const char* itemname = GetLinkedName(subslot);
         if (itemname!=0) pic->AddObjName(itemname, drawopt);
      } else {
         TNamed* nm = dynamic_cast<TNamed*> (subslot->GetAssignedObject());
         if (nm!=0) pic->AddObjName(nm->GetName(), drawopt);
      }

      Int_t rebinx, rebiny;
      if (subslot->GetIntPar("::HasRebinX", rebinx))
         pic->SetRebinX(rebinx);
      if (subslot->GetIntPar("::HasRebinY", rebiny))
         pic->SetRebinY(rebiny);
   }

   TObjArray pads;

   for (int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      TPad* subpad = GetSlotPad(subslot);
      if (subpad!=0)
        pads.Add(subpad);
   }

   if (pads.GetLast()<0) return;

   double lastx = -1;
   int xcnt = 0, sizex = 1, sizey = 1;
   for (int n=0;n<=pads.GetLast();n++) {
      TPad* subpad = (TPad*) pads.At(n);
      double mitx = subpad->GetXlowNDC()+subpad->GetWNDC()/2.;
      if (mitx>lastx) { xcnt++; lastx = mitx; }
                 else { xcnt = 0; lastx = -1; }
      if (xcnt>sizex) sizex = xcnt;
   }

   while (sizex*sizey<=pads.GetLast()) sizey++;

   pic->SetDivision(sizey, sizex);

   for(int ny=0;ny<sizey;ny++)
     for(int nx=0;nx<sizex;nx++) {
        int indx = ny*sizex + nx;
        if (indx>pads.GetLast()) break;
        TPad* subpad = (TPad*) pads.At(indx);
        MakePictureForPad(pic->Pic(ny,nx), subpad, useitemname);
     }
}

void TGo4ViewPanel::PrintCanvas()
{
   TGo4PrintWidget dlg;
   if (dlg.exec() != QDialog::Accepted) return;

   go4sett->setPrinterSett(dlg.GetPrinter(), dlg.GetPrintProg());

   QString outfile = "~/go4printout.ps";
   QString PrnCmd = dlg.GetPrintProg() +
                    " -P " + dlg.GetPrinter() +
                    " " + outfile;
   QString DelCmd = QString("rm -f ") + outfile;

   GetCanvas()->Print(outfile.latin1());
   gSystem->Exec(PrnCmd.latin1());
   gSystem->Exec(DelCmd.latin1());
}

void TGo4ViewPanel::StartRootEditor()
{
   TGo4LockGuard lock;

   fbEditorFrameVisible = !fMenuBar->isItemChecked(ShowRootEditorId);
   fMenuBar->setItemChecked(ShowRootEditorId, fbEditorFrameVisible);
   EditorFrame->setShown(fbEditorFrameVisible);

   if(fbEditorFrameVisible && (fxPeditor==0)) {
      SetActivePad(GetCanvas());
      fxRooteditor->SetEditable();      // mainframe will adopt pad editor window
      fxPeditor = TVirtualPadEditor::LoadEditor();
      fxRooteditor->SetEditable(kFALSE); // back to window manager as root window
   }

   ActivateInGedEditor(GetSelectedObject(GetActivePad(), 0));

   show();

   if (fbEditorFrameVisible)
     QTimer::singleShot(100, this, SLOT(ResizeGedEditorSlot()));
}

void TGo4ViewPanel::StartConditionEditor()
{
   TGo4Slot* padslot = GetPadSlot(GetActivePad());
   if (padslot==0) return;

   for (int n=0; n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind!=kind_Condition) continue;
      const char* itemname = GetLinkedName(subslot);
      if (itemname!=0) {
         EditItem(itemname);
         return;
      }
   }
}

void TGo4ViewPanel::RectangularRatio()
{
   TPad *pad = GetActivePad();
   if (pad==0) return;

   double dx = fabs(pad->AbsPixeltoX(1) - pad->AbsPixeltoX(0));
   double dy = fabs(pad->AbsPixeltoY(1) - pad->AbsPixeltoY(0));

   if ((dx<=0) || (dy<=0)) return;

   double ratio = dx / dy;

   if (ratio<1.) {
     double left = pad->GetLeftMargin();
     double right = pad->GetRightMargin();
     double change = (1.-left-right)*(1-ratio);
     pad->SetLeftMargin(left+change/2.);
     pad->SetRightMargin(right+change/2.);
   } else {
     double bottom = pad->GetBottomMargin();
     double top = pad->GetTopMargin();
     double change = (1.-bottom-top)*(1.-1/ratio);
     pad->SetTopMargin(top + change/2.);
     pad->SetBottomMargin(bottom + change/2.);
   }

   RedrawPanel(pad, true);
}

void TGo4ViewPanel::DefaultPadMargin()
{
   TPad *pad = GetActivePad();
   if (pad==0) return;

   pad->SetLeftMargin(gStyle->GetPadLeftMargin());
   pad->SetRightMargin(gStyle->GetPadRightMargin());
   pad->SetTopMargin(gStyle->GetPadTopMargin());
   pad->SetBottomMargin(gStyle->GetPadBottomMargin());

   RedrawPanel(pad, true);
}

void TGo4ViewPanel::ClearActivePad()
{
   TPad* pad = GetActivePad();
   if (pad==0) pad = GetCanvas();

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
   if (pad==0) pad = GetCanvas();

   TGo4Picture *padopt = GetPadOptions(pad);

   fMenuBar->setItemChecked(StatisticsId, padopt->IsHisStats());
   fMenuBar->setItemChecked(SuperimposeId, padopt->IsSuperimpose());
   fMenuBar->setItemChecked(SetTitleId, padopt->IsHisTitle());
   fMenuBar->setItemChecked(DrawTimeId, padopt->IsTitleTime());
   fMenuBar->setItemEnabled(DrawTimeId, padopt->IsHisTitle() && fbCloneFlag);
   fMenuBar->setItemChecked(DrawDateId, padopt->IsTitleDate());
   fMenuBar->setItemEnabled(DrawDateId, padopt->IsHisTitle() && fbCloneFlag);
   fMenuBar->setItemChecked(DrawItemnameId, padopt->IsTitleItem());
   fMenuBar->setItemEnabled(DrawItemnameId, padopt->IsHisTitle() && fbCloneFlag);

   fMenuBar->setItemChecked(FreezeTitleId, fbFreezeTitle);

   fMenuBar->setItemChecked(CrosshairId, fbCanvasCrosshair);
   fMenuBar->setItemChecked(SetLegendId, padopt->IsLegendDraw());
}

void TGo4ViewPanel::SelectMenuItemActivated(int id)
{
   TGo4LockGuard lock;

   if (id==BringToFrontId) {
      if (ShiftSelectedObjectToEnd(GetActivePad())) {
         MarkPadModified(GetActivePad());
         ShootRepaintTimer(GetActivePad());
      }
      return;
   }

   int selected = TGo4Picture::PictureIndex;

   if (id!=MasterSelectId) selected = id - FirstSelectId;

   TGo4Slot* slot = GetPadSlot(GetActivePad());
   if (slot==0) return;

   int wasselected = GetSelectedObjectIndex(slot);

   SetSelectedObjectIndex(slot, selected);

   if (selected!=wasselected) {
      CallPanelFunc(panel_Updated, GetActivePad());
      UpdatePanelCaption();
   }

   if (selected!=TGo4Picture::PictureIndex)
      ActivateInGedEditor(GetSelectedObject(GetActivePad(), 0));
}

void TGo4ViewPanel::ShowEventStatus()
{
   // TGo4LockGuard glob;
   bool s = !fMenuBar->isItemChecked(EventStatusId);
   fMenuBar->setItemChecked(EventStatusId, s);
   fbCanvasEventstatus = s;
   fxGo4QRootCanvas->setShowEventStatus(s);
   CanvasStatus->setShown(s);
   if(!s) DisplayPadStatus(ActivePad);
}

void TGo4ViewPanel::UpdatePadStatus(TPad* pad, bool removeitems)
{
   if (GetPadSlot(pad)==0) return;

   BlockPanelRedraw(true);
   ProcessPadStatusUpdate(pad, 0, removeitems);
   BlockPanelRedraw(false);
}

void TGo4ViewPanel::ProcessPadStatusUpdate(TPad* pad, TGo4Slot* parent, bool removeitems)
{
   if (pad==0) return;

   TGo4Slot* slot = 0;

   bool setdefaults = false;
   if (parent==0) {
      slot = GetPadSlot(pad);
      if (slot==0) return;
   } else {
      slot = parent->FindChild(pad->GetName());
      // create slot for subpad if not existing
      if (slot==0) {
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
        TPad* subpad = dynamic_cast<TPad*> (obj);
        if (subpad==0) continue;
        issubpads = true;
        if (subpads.FindObject(subpad->GetName())!=0)
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
   for(int n =slot->NumChilds()-1; n>=0; n--) {
      TGo4Slot* subslot = slot->GetChild(n);
      TPad* subpad = GetSlotPad(subslot);
      if (subpad!=0)
         if (pad->GetListOfPrimitives()->FindObject(subpad) == 0)
            delete subslot;
         else
           issubpads = true;
   }

   if (setdefaults) SetPadDefaults(pad);

   if (!issubpads) return;

   TIter iter(pad->GetListOfPrimitives());
   TObjArray removedItems;
   TObject* obj = 0;
   while ((obj = iter()) != 0) {
      TPad* subpad = dynamic_cast<TPad*> (obj);
      if (subpad!=0)
         ProcessPadStatusUpdate(subpad, slot, removeitems);
      else
         removedItems.Add(obj);
   }

   pad->GetListOfPrimitives()->RemoveAll(&removedItems);

   if (removeitems)
     ClearPadItems(slot, 0);
}

TGo4Slot* TGo4ViewPanel::AddDrawObject(TPad* pad, int kind, const char* itemname, TObject* obj, bool owner, const char* drawopt)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   if (padslot==0) {
      if (owner) delete obj;
      return 0;
   }

   // clear only if link is added
   if (kind<100)
     ClearPad(pad, false, true);

   TGo4Slot* tgtslot = 0;

   if (kind==kind_Link) {
      TClass* cl = Browser()->ItemClass(itemname);
      if ((cl!=0) && cl->InheritsFrom(TGo4Condition::Class()))
        UndrawItem(itemname);
//        if (WhereItemDrawn(itemname)!=0) return 0;

//      tgtslot = AddLink(itemname, padslot);
     TGo4Slot* brslot = Browser()->BrowserSlot(itemname);

     if (brslot!=0) {
        tgtslot = AddNewSlot(brslot->GetName(), padslot);
        SetLinkedName(tgtslot, itemname);
        if (fbCloneFlag)
          tgtslot->SetProxy(new TGo4DrawCloneProxy(brslot, this));
        else
          tgtslot->SetProxy(new TGo4LinkProxy(brslot));
     }
   } else {
      QString newslotname = itemname;
      if ((newslotname.length()==0) || (padslot->FindChild(newslotname.latin1())!=0)) {
         int cnt = 0;
         do {
           if ((itemname==0) || (*itemname==0))
              newslotname = "::SpecialObject_";
           else
              newslotname = itemname;
           newslotname += QString::number(cnt++);
         } while (padslot->FindChild(newslotname.latin1())!=0);
      }
      tgtslot = AddNewSlot(newslotname.latin1(), padslot);
      tgtslot->SetProxy(new TGo4ObjectProxy(obj, owner));
   }
   if (tgtslot==0) return 0;

   if (kind<100)
     CallPanelFunc(panel_Modified, pad);

   tgtslot->SetPar("::FirstDraw", "true");
   SetDrawKind(tgtslot, kind);
   SetSpecialDrawOption(tgtslot, drawopt);

   TGo4Picture* padopt = GetPadOptions(padslot);
   if (padopt!=0) {
      padopt->SetContentModified(true);
      padopt->SetPadModified();
      if ((kind<100) && (drawopt!=0))
         padopt->SetDrawOption(drawopt, TGo4Picture::PictureIndex);
   }

   return tgtslot;
}

TGo4Slot* TGo4ViewPanel::GetDrawObjectSlot(TPad* pad, const char* name)
{
   TGo4Slot* slot = GetPadSlot(pad);

   return slot==0 ? 0 : slot->FindChild(name);
}

TObject* TGo4ViewPanel::GetDrawObject(TPad* pad, const char* name)
{
   TGo4Slot* subslot = GetDrawObjectSlot(pad, name);

   return subslot==0 ? 0 : subslot->GetAssignedObject();
}

void TGo4ViewPanel::DeleteDrawObject(TPad* pad, const char* name)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   TGo4Slot* subslot = padslot==0 ? 0 : padslot->FindChild(name);

   if (subslot!=0) {
      delete subslot;
      TGo4Picture* padopt = GetPadOptions(padslot);
      if (padopt!=0) padopt->SetPadModified();
   }
}

void TGo4ViewPanel::DeleteDrawObject(TPad* pad, TObject* obj)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot==0) return;
   for(int n=0;n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      if (GetDrawKind(subslot)<0) continue;
      if (subslot->GetAssignedObject()==obj) {
         delete subslot;
         break;
      }
   }
}

void TGo4ViewPanel::CollectSpecialObjects(TPad* pad, TObjArray* objs, int selectkind)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((slot==0) || (objs==0)) return;

   for(int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind<0) continue;
      if ((kind!=kind_Link) && ((selectkind<0) || (kind==selectkind))) {
         TObject* obj = subslot->GetAssignedObject();
         if (obj!=0) objs->Add(obj);
      }
   }
}

bool TGo4ViewPanel::DeleteDrawObjects(TPad* pad, int kindtodelete)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);

   if ((slot==0) || (padopt==0)) return false;

   bool res = false;

   for(int n=slot->NumChilds()-1;n>=0;n--) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind<0) continue;
      if ((kind!=kind_Link) && ((kindtodelete<0) || (kind==kindtodelete))) {
         res = true;
         delete subslot;
      }
   }
   if (res) padopt->SetPadModified();
   return true;
}

void TGo4ViewPanel::SetPadSuperImpose(TPad* pad, bool on)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt!=0) {
      padopt->SetSuperimpose(on);
      padopt->SetPadModified();
   }
}

void TGo4ViewPanel::ProcessPadModifiedSignal()
{
   fbModifiedSignalFlag = false;
   if (GetActivePad()==0) return;

   if (ScanDrawOptions(GetActivePad(), GetPadSlot(GetActivePad()), GetPadOptions(GetActivePad()), true))
      CallPanelFunc(panel_Updated, GetActivePad());
}

bool TGo4ViewPanel::ScanDrawOptions(TPad* pad, TGo4Slot* padslot, TGo4Picture* pic, bool onlyscan)
{
   TGo4LockGuard lock;

   if ((pad==0) || (pic==0) || (padslot==0)) return false;

   bool optchanged = false;
   TObjLink* link = pad->GetListOfPrimitives()->FirstLink();

   while (link!=0) {
      const char* clname = link->GetObject()->ClassName();
      if ((strcmp(clname, "TFrame")==0) ||
          (strcmp(clname, "TPaveText")==0) ||
          (strcmp(clname, "TLegend")==0)) {

         link = link->Next();
      }
      else
        break;
   }

   // take draw options from first drawn object
   if (link!=0) {

      TString newopt(link->GetOption());
      Int_t drawoptindx = GetSuperimposeSlot(padslot)==0 ? 0 : TGo4Picture::PictureIndex;
      TString oldopt(pic->GetDrawOption(drawoptindx));

      while (newopt.BeginsWith(NoStackDrawOption, TString::kIgnoreCase))
         newopt.Remove(0, strlen(NoStackDrawOption));

      if (oldopt!="asimage")
      if ((newopt.Length()!=oldopt.Length()) ||
          (newopt.CompareTo(oldopt, TString::kIgnoreCase)!=0)) {
           optchanged = true;
           pic->SetDrawOption(newopt.Data(), drawoptindx);
         }

      TH1* h1 = dynamic_cast<TH1*> (link->GetObject());
      if (h1!=0) {
         TPaveStats* stats = dynamic_cast<TPaveStats*>
           (h1->GetListOfFunctions()->FindObject("stats"));
         if (h1->TestBit(TH1::kNoStats))
            pic->SetHisStats(kFALSE);
         else {
            pic->SetHisStats(kTRUE);
            pic->SetStatsAttr(stats);
         }
      }

   }

   if (pad->GetLogx()!=pic->GetLogScale(0)) {
      pic->SetLogScale(0, pad->GetLogx());
      optchanged = true;
   }

   if (pad->GetLogy()!=pic->GetLogScale(1)) {
      pic->SetLogScale(1, pad->GetLogy());
      optchanged = true;
   }

   if (pad->GetLogz()!=pic->GetLogScale(2)) {
      pic->SetLogScale(2, pad->GetLogz());
      optchanged = true;
   }

   if (pad==GetCanvas())
     fbCanvasCrosshair = pad->HasCrosshair();

   TPaveText* titl = dynamic_cast<TPaveText*> (pad->GetListOfPrimitives()->FindObject("title"));
   if (titl) {
      pic->SetTitleAttr(titl);
      optchanged = true;
   }

   TObjArray objs, objslots;

   CollectMainDrawObjects(padslot, &objs, &objslots, 0);
   ScanObjectsDrawOptions(onlyscan, padslot, &objs, &objslots);

   return optchanged;
}

void TGo4ViewPanel::ScanObjectsDrawOptions(bool onlyscan, TGo4Slot* padslot, TObjArray* objs, TObjArray* objslots)
{
   TGo4Picture* pic = GetPadOptions(padslot);

   TPad* pad = GetSlotPad(padslot);
   if ((pad!=0) && (pic!=0))
      if (padslot->GetPar("::DrawOptAssigned")!=0) {
          pic->SetDrawAttributes(pad, TGo4Picture::PictureIndex);
      } else
      if (!onlyscan) {
         pic->GetDrawAttributes(pad, TGo4Picture::PictureIndex);
         padslot->SetPar("::DrawOptAssigned","1");
      }

   if ((padslot==0) || (pic==0) || (objs==0) || (objslots==0)) return;

   for(int n=0;n<=objs->GetLast();n++) {
      TObject* obj = objs->At(n);
      TGo4Slot* subslot = (TGo4Slot*) objslots->At(n);

      if ((obj==0) || (subslot==0)) continue;

      if (subslot->GetPar("::DrawOptAssigned")!=0) {
         pic->SetDrawAttributes(obj, n);
      } else
      if (!onlyscan) {
         pic->GetDrawAttributes(obj, n);
         subslot->SetPar("::DrawOptAssigned","1");
      }
   }
}

void TGo4ViewPanel::CollectMainDrawObjects(TGo4Slot* slot, TObjArray* objs, TObjArray* objslots, int modifier)
{
   // modifier == 0 - no objects can be deleted
   //          == 1 - objects of same types should be in the list
   //          == 2 - only last object survive

   if ((slot==0) || (objs==0)) return;

   TObjArray mainslots;

   int lastobjtype = 0;

   for(int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);

      if ((kind<=0) || (kind>=kind_Additional)) continue;

      TObject* obj = subslot->GetAssignedObject();
      if (obj==0) continue;

      int objtype = 0;

      if (obj->InheritsFrom(TH1::Class())) objtype = 1; else
      if (obj->InheritsFrom(TGraph::Class())) objtype = 2; else
      if (obj->InheritsFrom(TMultiGraph::Class())) objtype = 3; else
      if (obj->InheritsFrom(THStack::Class())) objtype = 4;


      // can happen condition here, which is add as link and not identified as condition yet
      // should bot be recognised as "main" draw object
      if (objtype>0) {
         lastobjtype = objtype;
         mainslots.Add(subslot);
      }
   }

   Bool_t deletesomething = kFALSE;

   for(int n=0;n<=mainslots.GetLast();n++) {
      TGo4Slot* subslot = (TGo4Slot*) mainslots.At(n);
      TObject* obj = subslot->GetAssignedObject();
      Int_t objtype = 0;
      if (obj->InheritsFrom(TH1::Class())) objtype = 1; else
      if (obj->InheritsFrom(TGraph::Class())) objtype = 2; else
      if (obj->InheritsFrom(TMultiGraph::Class())) objtype = 3; else
      if (obj->InheritsFrom(THStack::Class())) objtype = 4;


      // check if all main object correspond to type of last object
      // if no, delete

      if ((n<mainslots.GetLast()) &&
          ((modifier==2) || ((modifier==1) && (lastobjtype!=objtype)))) {
         delete subslot;
         deletesomething = kTRUE;
      } else {
         objs->Add(obj);
         if (objslots!=0) objslots->Add(subslot);
         TGo4DrawCloneProxy* pr = dynamic_cast<TGo4DrawCloneProxy*> (subslot->GetProxy());
         if (pr!=0) {
            pr->UpdateTitle();
            pr->PerformRebin();
         }
      }
   }

   if (deletesomething) CleanupGedEditor();

   for(int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);

      Int_t kind = GetDrawKind(subslot);
      if ((kind>=kind_Additional) && (kind<kind_Specials)) {
         TObject* obj = subslot->GetAssignedObject();
         if (obj!=0) {
            objs->Add(obj);
            if (objslots!=0) objslots->Add(subslot);
         }
      }
   }
}

TObject* TGo4ViewPanel::ProduceSuperimposeObject(TGo4Picture* padopt, TGo4Slot* sislot, TGo4Slot* legslot, TObjArray* objs, TObjArray * objslots, bool showitems)
{
   if ((sislot==0) || (objs==0) || (padopt==0)) return 0;

   TObject* oldobj = sislot->GetAssignedObject();

   Bool_t ishstack = kFALSE;
   Bool_t isgstack = kFALSE;
   Bool_t iserror = kFALSE;
   Bool_t resetcolors = kFALSE;

   for(int n=0;n<=objs->GetLast();n++) {
      TObject* obj = objs->At(n);
      if (obj->InheritsFrom(TH1::Class())) ishstack = kTRUE; else
      if (obj->InheritsFrom(TGraph::Class())) isgstack = kTRUE; else
        iserror = kTRUE;
   }

   // if error, no superimpose is allowed
   if (iserror || (ishstack && isgstack)) {
       cerr << "Error detected: superimpose of multiple objects with different types" << endl;
       return 0;
   }

   if (ishstack) {
      THStack* hs = dynamic_cast<THStack*> (oldobj);

      if (hs==0) {
         hs = new THStack(objs->First()->GetName(), objs->First()->GetTitle());
         sislot->SetProxy(new TGo4DrawObjProxy(hs, kTRUE, kTRUE));
         resetcolors = kTRUE;
      } else {
         if (hs->GetHists())
            hs->GetHists()->Clear();
      }

      for(int n=0;n<=objs->GetLast();n++) {
         TH1* histo = (TH1*) objs->At(n);
         TGo4Slot* slot = (TGo4Slot*) objslots->At(n);

         Int_t kind = GetDrawKind(slot);
         if ((resetcolors) || (kind==kind_FitModels) || (slot->GetPar("::FirstDraw")!=0))
            histo->SetLineColor(n+1);
         histo->GetXaxis()->UnZoom();

         const char* drawopt = padopt->GetDrawOption(n);
         if (drawopt==0)
            drawopt = GetSpecialDrawOption(slot);

         slot->RemovePar("::FirstDraw");

         hs->Add(histo, drawopt);
      }
      oldobj = hs;
   } else
   if (isgstack) {
      TMultiGraph* mgr = dynamic_cast<TMultiGraph*> (oldobj);
      if (mgr==0) {
         mgr = new TMultiGraph(objs->First()->GetName(), objs->First()->GetTitle());
         sislot->SetProxy(new TGo4DrawObjProxy(mgr, kTRUE, kTRUE));
         resetcolors = kTRUE;
      } else
      if (mgr->GetListOfGraphs())
        mgr->GetListOfGraphs()->Clear();
      for(int n=0;n<=objs->GetLast();n++) {
         TGraph* gr = (TGraph*) objs->At(n);
         TGo4Slot* slot = (TGo4Slot*) objslots->At(n);

         Int_t kind = GetDrawKind(slot);

         TString drawopt = padopt->GetDrawOption(n);
         if (drawopt.Length()==0)
            drawopt = GetSpecialDrawOption(slot);
         if (drawopt.Length()==0) drawopt = "AP";

         if (n>0) {
            // suppress multiple drawing of axis for subgraphs
            drawopt.ReplaceAll("a","");
            drawopt.ReplaceAll("A","");
         }

         if ((resetcolors) || (kind==kind_FitModels) || (slot->GetPar("::FirstDraw")!=0)) {
            gr->SetLineColor(n+1);
            gr->SetMarkerColor(n+1);
         }

         slot->RemovePar("::FirstDraw");

         mgr->Add(gr, drawopt.Data());
      }
      oldobj = mgr;
   }

   if ((ishstack || isgstack) && (legslot!=0)) {
      TLegend* legend = dynamic_cast<TLegend*> (legslot->GetAssignedObject());

      if (legend == 0) {
         legend = new TLegend(0.6,0.91,0.95,0.995);
         legend->SetBorderSize(2);
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

         legend->AddEntry(stob,objname,ishstack ? "l" : "p");
      }
   }

   return oldobj;
}


void TGo4ViewPanel::Divide( int numX, int numY )
{
   // remove subpads wil not cleanup labels of this pad:
   // fxTGo4PreviewPanelSlots->ClearAllLabels(ActivePad);

   TPad* pad = GetActivePad();

   TGo4Slot* padslot = GetPadSlot(pad);
   if ((pad==0) || (padslot==0)) return;

   ClearPad(pad, true, true);
   RedrawPanel(pad, true);

   if ((numX>1) || (numY>1)) {
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
   if (tgtslot==0)
     tgtslot = new TGo4Slot(padslot, "::ThisPad", "Special object");
   tgtslot->SetProxy(new TGo4ObjectProxy(pad, kFALSE));
   SetDrawKind(tgtslot, kind_ThisPad);
   ConnectPad(pad);

   AllocatePadOptions(pad);
}

TGo4Slot* TGo4ViewPanel::GetPadSlot(TPad* pad)
{
   if (pad==GetCanvas()) return GetPanelSlot();

   TGo4Iter iter(GetPanelSlot(), kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      if (GetDrawKind(subslot)==kind_PadSlot)
        if (GetSlotPad(subslot)==pad) return subslot;
   }
   return 0;
}

TPad* TGo4ViewPanel::GetSlotPad(TGo4Slot* slot)
{
   if (slot==0) return 0;

   TGo4Slot* tgtslot = slot->FindChild("::ThisPad");

   return tgtslot==0 ? 0 : (TPad*) tgtslot->GetAssignedObject();
}

bool TGo4ViewPanel::IsPadHasSubPads(TGo4Slot* padslot)
{
   if (padslot==0) return false;
   for(int n=0;n<padslot->NumChilds();n++)
     if (GetSlotPad(padslot->GetChild(n))!=0) return true;
   return false;
}

bool TGo4ViewPanel::IsPadHasSubPads(TPad* pad)
{
   return IsPadHasSubPads(GetPadSlot(pad));
}

void TGo4ViewPanel::MarkPadModified(TPad* pad)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt!=0)
     padopt->SetPadModified();
}

bool TGo4ViewPanel::IsPanelPad(TPad* pad)
{
   return GetPadSlot(pad)!=0;
}

int TGo4ViewPanel::GetNumberOfPads(TPad* toppad)
{
   if (toppad==0) toppad = GetCanvas();
   int number = 1;
   TGo4Iter iter(GetPadSlot(toppad), kTRUE);
   while (iter.next()) {
     TPad* pad = GetSlotPad(iter.getslot());
     if (pad!=0) number++;
   }
   return number;
}

TPad* TGo4ViewPanel::GetSubPad(TPad* toppad, int num, bool onlytoplevel)
{
   if (toppad==0) toppad = GetCanvas();
   TGo4Slot* slot = GetPadSlot(toppad);
   if (slot==0) return 0;

   int cnt=-1;
   if (!onlytoplevel || !IsPadHasSubPads(slot)) cnt++;

   if (num==cnt) return toppad;

   TGo4Iter iter(slot, kTRUE);
   while (iter.next()) {
      TGo4Slot* subslot = iter.getslot();
      TPad* pad = GetSlotPad(subslot);
      if (pad==0) continue;
      if (!onlytoplevel || !IsPadHasSubPads(subslot)) cnt++;
      if (num==cnt) return pad;
   }
   return 0;
}

void TGo4ViewPanel::ProducePadsList(TObjArray* arr, TPad* toppad)
{
   if (arr==0) return;
   if (toppad==0) toppad = GetCanvas();

   arr->Add(toppad);

   TGo4Iter iter(GetPadSlot(toppad), kTRUE);
   while (iter.next()) {
     TPad* pad = GetSlotPad(iter.getslot());
     if (pad!=0) arr->Add(pad);
   }
}

const char* TGo4ViewPanel::GetDrawObjectLinkName( TPad * pad, TObject* obj )
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((pad==0) || (slot==0) || (obj==0)) return 0;

   for(int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      if (subslot->GetAssignedObject()==obj) return GetLinkedName(subslot);
   }
   return 0;
}

TGo4Slot* TGo4ViewPanel::GetSuperimposeSlot(TGo4Slot* padslot)
{
   return padslot==0 ? 0 : padslot->FindChild("::Superimpose");
}

TGo4Slot* TGo4ViewPanel::GetPadMainObjectSlot(TPad* pad)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if ((pad==0) || (slot==0)) return 0;

   for(int n=0;n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if ((kind<=0) || (kind>=kind_Additional)) continue;
      TObject* obj = subslot->GetAssignedObject();
      if (obj!=0) return subslot;
   }
   return 0;
}

TObject* TGo4ViewPanel::GetPadMainObject(TPad* pad)
{
   TGo4Slot* slot = GetPadMainObjectSlot(pad);
   return slot==0 ? 0 : slot->GetAssignedObject();
}

TCanvas* TGo4ViewPanel::GetCanvas()
{
    return fxGo4QRootCanvas->getCanvas();
}

QGo4RootCanvas* TGo4ViewPanel::GetQCanvas()
{
   return fxGo4QRootCanvas;
}

TPad * TGo4ViewPanel::GetActivePad()
{
   return ActivePad;
}

void TGo4ViewPanel::AllocatePadOptions(TPad* pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot==0) return;

   TGo4Slot* tgtslot = padslot->FindChild("::PadOptions");
   if (tgtslot==0) {
     tgtslot = new TGo4Slot(padslot, "::PadOptions", "Pad options slot");
     tgtslot->SetProxy(new TGo4ObjectProxy(new TGo4Picture, kTRUE));
     SetDrawKind(tgtslot, kind_PadOptions);
   }
}

TGo4Picture* TGo4ViewPanel::GetPadOptions(TPad* pad)
{
   return GetPadOptions(GetPadSlot(pad));
}

TGo4Picture* TGo4ViewPanel::GetPadOptions(TGo4Slot* padslot)
{
   if (padslot==0) return 0;
   TGo4Slot* tgtslot = padslot->FindChild("::PadOptions");
   return (tgtslot==0) ? 0 : (TGo4Picture*) tgtslot->GetAssignedObject();
}

TH1* TGo4ViewPanel::GetPadHistogram(TPad *pad)
{
   TGo4Slot* padslot = GetPadSlot(pad);

   if ((pad==0) || (padslot==0)) return 0;

   TObject* obj = 0;

   TGo4Slot* sislot = GetSuperimposeSlot(padslot);
   if (sislot!=0) obj = sislot->GetAssignedObject();

   if (obj==0) obj = GetPadMainObject(pad);
   if (obj==0) return 0;

   if (obj->InheritsFrom(TH1::Class()))
      return (TH1*) obj;

   if (obj->InheritsFrom(TGraph::Class())) {
      TGraph* gr = dynamic_cast<TGraph*> (obj);
      if (gr!=0) return gr->GetHistogram();
   }

   if (obj->InheritsFrom(THStack::Class())) {
      THStack* hs = dynamic_cast<THStack*> (obj);
      if (hs!=0) return hs->GetHistogram();
   }

   if (obj->InheritsFrom(TMultiGraph::Class())) {
      TMultiGraph* mg = dynamic_cast<TMultiGraph*> (obj);
      if (mg) {
         if (mg->GetHistogram()) return mg->GetHistogram();

         TGraph* gr = 0;
         TIter iter(mg->GetListOfGraphs());
         while ((gr = (TGraph*)iter()) != 0)
           if (gr->GetHistogram()) return gr->GetHistogram();
      }
   }

   return 0;
}

void TGo4ViewPanel::BlockPanelRedraw(bool on)
{
   if (on) fiSkipRedrawCounter++;
      else fiSkipRedrawCounter--;
}

bool TGo4ViewPanel::IsRedrawBlocked()
{
   return fiSkipRedrawCounter>0;
}

void TGo4ViewPanel::UpdatePanelCaption()
{
   TGo4Slot* slot = GetPadSlot(GetActivePad());
   if (slot==0) return;

   fSelectMenu->clear();

   TGo4Slot* sislot = GetSuperimposeSlot(slot);

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   Int_t nselectitem = 0;

   int selected = GetSelectedObjectIndex(slot);

   if ((selected>objs.GetLast()) ||
       ((selected==TGo4Picture::PictureIndex) && (sislot==0))) {
      SetSelectedObjectIndex(slot, 0);
      selected = 0;
   }

   if (sislot!=0) {
      fSelectMenu->insertItem("Master object", MasterSelectId);
      fSelectMenu->insertSeparator();
      nselectitem++;
   }

   QString capt = GetPanelName();

   QString fulllist = "";
   QString selslotname = "";

   for(int n=0;n<=objslots.GetLast();n++) {
      TGo4Slot* subslot = (TGo4Slot*) objslots.At(n);
      fSelectMenu->insertItem(objs.At(n)->GetName(), FirstSelectId+n);
      nselectitem++;

      QString subslotname = subslot->GetName();
      if (n==selected) {
         subslotname = QString("[") + subslotname + QString("]");
         selslotname = subslotname;
      }
      if (n>0) fulllist += ", ";
      fulllist += subslotname;
   }

   if ((selected!=TGo4Picture::PictureIndex) &&
       (objslots.GetLast()>0) && (selected<objslots.GetLast())) {
           fSelectMenu->insertSeparator();
           fSelectMenu->insertItem(QString("Show ") + selslotname + QString(" on top"), BringToFrontId);
       }

   if ((selected==TGo4Picture::PictureIndex) && (fulllist.length()>0))
      fulllist = QString("[") + fulllist + QString("]");

   if (nselectitem==0) {
      fMenuBar->setItemEnabled(SelectObjectId, false);
   } else {
      fMenuBar->setItemEnabled(SelectObjectId, true);
      if (selected==TGo4Picture::PictureIndex)
         fMenuBar->setItemChecked(MasterSelectId, true);
      else
         fMenuBar->setItemChecked(FirstSelectId+selected, true);
   }

   if (fulllist.length()>0) {
      capt += ": ";
      capt += fulllist;
   }

   if (capt.length()>60) {
      capt.truncate(58);
      capt+="...";
   }

   if (!fbFreezeTitle)
      setCaption(capt);
}

void TGo4ViewPanel::SetDrawKind(TGo4Slot* slot, int kind)
{
   if (slot==0) return;
   if (kind<=0) slot->RemovePar("::DrawKind");
           else slot->SetIntPar("::DrawKind", kind);
}

void TGo4ViewPanel::SetSpecialDrawOption(TGo4Slot* slot, const char* drawopt)
{
   if (slot==0) return;
   if (drawopt!=0)
      slot->SetPar("::DrawOpt", drawopt);
   else
      slot->RemovePar("::DrawOpt");
}

int TGo4ViewPanel::GetDrawKind(TGo4Slot* slot)
{
   if (slot==0) return -1;
   Int_t kind;
   if (!slot->GetIntPar("::DrawKind",kind)) return -1;
   return kind;
}

const char* TGo4ViewPanel::GetSpecialDrawOption(TGo4Slot* slot)
{
   return (slot==0) ? 0 : slot->GetPar("::DrawOpt");
}

void TGo4ViewPanel::SetSelectedObjectIndex(TGo4Slot* slot, int indx)
{
   if (slot!=0) slot->SetIntPar("::SelectedObject", indx);
}

int TGo4ViewPanel::GetSelectedObjectIndex(TGo4Slot* slot)
{
   if (slot==0) return 0;
   Int_t indx = 0;
   if (!slot->GetIntPar("::SelectedObject", indx)) indx = 0;
   return indx;
}

TObject* TGo4ViewPanel::GetSelectedObject(TPad * pad, const char** drawopt)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot==0) || (padopt==0)) return 0;

   int indx = GetSelectedObjectIndex(slot);

   TGo4Slot* sislot = GetSuperimposeSlot(slot);
   if ((sislot!=0) && (indx==TGo4Picture::PictureIndex)) {
      if (drawopt!=0) *drawopt = padopt->GetDrawOption(TGo4Picture::PictureIndex);
      return sislot->GetAssignedObject();
   }

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   if (objs.GetLast()<0) return 0;

   if ((indx>objs.GetLast()) || (indx<0)) indx = 0;

   if (drawopt!=0) {
       const char* resopt = padopt->GetDrawOption(indx);
       if (resopt==0)
          resopt = GetSpecialDrawOption((TGo4Slot*) objslots.At(indx));

       *drawopt = resopt;
   }

   return objs.At(indx);
}

bool TGo4ViewPanel::ShiftSelectedObjectToEnd(TPad * pad)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot==0) || (padopt==0)) return false;

   int indx = GetSelectedObjectIndex(slot);

   TObjArray objs, objslots;
   CollectMainDrawObjects(slot, &objs, &objslots, 0);

   // no sense to shift object which is already at the end
   if ((objs.GetLast()<=0) || (indx==objs.GetLast())) return false;

   TGo4Slot* selslot = (TGo4Slot*) objslots.At(indx);
   TGo4Slot* lastslot = (TGo4Slot*) objslots.Last();

   if (!slot->ShiftSlotAfter(selslot, lastslot)) return false;

   SetSelectedObjectIndex(slot, objs.GetLast());

   return true;
}

void TGo4ViewPanel::CheckObjectsAssigments( TPad * pad, TGo4Slot * padslot )
{
   if ((pad==0) || (padslot==0)) return;

   TObjArray objs, objslots;
   CollectMainDrawObjects(padslot, &objs, &objslots, 0);

   int indx = GetSelectedObjectIndex(padslot);
   if (indx<0) indx = 0;

   TH1* selhisto = dynamic_cast<TH1*> ((indx<=objs.GetLast()) ? objs.At(indx) : 0);
   if (selhisto==0) selhisto = GetPadHistogram(pad);

   for(int n=0;n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      TObject* obj = subslot->GetAssignedObject();
      if (obj==0) continue;

      TGo4Marker* mark = 0;
      TGo4Condition* cond = 0;

      // reset condition and marker histogram
      if ((kind>=kind_Specials) && (kind<kind_Other)) {
         if (obj->InheritsFrom(TGo4Condition::Class()))
            cond = (TGo4Condition*) obj;
         else
         if (obj->InheritsFrom(TGo4Marker::Class()))
            mark = (TGo4Marker*) obj;
      } else
      if (kind==kind_Condition)
          cond = dynamic_cast<TGo4Condition*>(obj);

      TH1* oldhisto = 0;

      if (cond!=0) oldhisto = cond->GetWorkHistogram(); else
      if (mark!=0) oldhisto = mark->GetHistogram();

      if (oldhisto!=0)
        if (objs.FindObject(oldhisto)==0) oldhisto = 0;

      if (oldhisto==0)
        if (cond!=0) cond->SetWorkHistogram(selhisto); else
        if (mark!=0) mark->SetHistogram(selhisto);
   }
}

void TGo4ViewPanel::CheckForSpecialObjects(TPad *pad, TGo4Slot* padslot)
{
   if ((pad==0) || (padslot==0)) return;

   TGo4Picture* pic = 0;
   TGo4Slot* picslot = 0;
   TCanvas* canv = 0;
   TGo4Slot* canvslot = 0;

   int numcond = 0;

   for(int n=0;n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);
      Int_t kind = GetDrawKind(subslot);
      if (kind==kind_Condition) numcond++;

      TObject* obj = subslot->GetAssignedObject();
      if (obj==0) continue;

      if ((kind<0) || (kind>=100)) continue;

      TGo4Condition* cond = dynamic_cast<TGo4Condition*>(obj);
      // change drawkind of condition which is drawn as normal object
      if (cond!=0) {
         numcond++;
         cond->SetLineColor(numcond+1);
         cond->SetFillColor(numcond+1);
         cond->SetFillStyle(3444);
         SetDrawKind(subslot, kind_Condition);
         continue;
      }

      if (pic==0) {
         pic = dynamic_cast<TGo4Picture*> (obj);
         if (pic!=0) picslot = subslot;
      }

      if (canv==0) {
         canv = dynamic_cast<TCanvas*> (obj);
         if (canv!=0) canvslot = subslot;
      }
   }

   if (pic!=0) {

      ClearPadItems(padslot, picslot);

      // remove all subpads if any but do not remove items while picture is there
      ClearPad(pad, false, true);

      ProcessPictureRedraw(GetLinkedName(picslot), pad, pic);

      if (pad==GetCanvas()) {
         QString mycaption = GetPanelName();
         mycaption += ": ";
         mycaption += pic->GetTitle();
         setCaption(mycaption);
         fbFreezeTitle = true;
      }

      // remove picture from the pad
      delete picslot;

      // cout << "!!!!!!!!!!!! Picture redraw done" << endl;

      return;
   }

   if (canv!=0) {
      // remove all other
      ClearPadItems(padslot, canvslot);

      // remove all subpads if any but do not remove items while picture is there
      ClearPad(pad, false, true);

      TVirtualPad* padsav = gPad;

      ProcessCanvasAdopt(pad, canv, GetLinkedName(canvslot));

      if (padsav!=0) padsav->cd();

      delete canvslot;
   }
}

void TGo4ViewPanel::ProcessPictureRedraw(const char* picitemname, TPad* pad, TGo4Picture* pic)
{
   if ((pad==0) || (pic==0)) return;

   if (pic->IsDivided()) {
      pad->Divide(pic->GetDivX(), pic->GetDivY());

      // this create appropriate entries in the OM
      UpdatePadStatus(pad, false);

      for(Int_t posy=0; posy<pic->GetDivY(); posy++)
         for(Int_t posx=0; posx<pic->GetDivX(); posx++) {
           TGo4Picture* sub = pic->FindPic(posy,posx);
           if (sub!=0)
             ProcessPictureRedraw(picitemname, (TPad*) pad->GetPad(posy*pic->GetDivX() + posx + 1), sub);
       }

      return;
   }

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt==0) {
      cerr << "!!!!!!!! Should not be" << endl;
      return;
   }

   padopt->CopyOptionsFrom(pic);

   padopt->GetDrawAttributes(pad, TGo4Picture::PictureIndex);

   TGo4BrowserProxy* brcont = Browser();

   Option_t* drawopt = pic->GetDrawOption(TGo4Picture::PictureIndex);
   if (drawopt!=0) pic->SetDrawOption(drawopt, TGo4Picture::PictureIndex);

   Int_t ndraw = 0;

   for (Int_t n=0; n<pic->GetNumObjNames(); n++) {
      Option_t* drawopt = pic->GetDrawOption(n);
      if (drawopt!=0) pic->SetDrawOption(drawopt, n);

      const char* objname = pic->GetObjName(n);

      TString drawname;

      if (brcont->DefineRelatedObject(picitemname, objname, drawname)) {
         TGo4Slot* slot = AddDrawObject(pad, kind_Link, drawname.Data(), 0, false, 0);
         brcont->GetBrowserObject(drawname.Data(), 2);
         ndraw++;

         if (pic->GetRebinX(n)>1) {
             slot->SetIntPar("::DoRebinX", pic->GetRebinX(n));
             slot->SetIntPar("::HasRebinX", pic->GetRebinX(n));
         }

         if (pic->GetRebinY(n)>1) {
             slot->SetIntPar("::DoRebinY", pic->GetRebinY(n));
             slot->SetIntPar("::HasRebinY", pic->GetRebinY(n));
         }
      }
   }

   if (ndraw>1)
     padopt->SetSuperimpose(true);

   TListIter iter(pic->GetSpecialObjects());
   TObject* obj = 0;
   while ((obj=iter())!=0) {
      Option_t* drawopt = iter.GetOption();
      if (dynamic_cast<TArrow*>(obj)!=0)
         AddMarkerObj(pad, kind_Arrow, obj->Clone());
      else
      if (dynamic_cast<TLatex*>(obj)!=0)
         AddMarkerObj(pad, kind_Latex, obj->Clone());
      else
      if (dynamic_cast<TGo4Marker*>(obj)!=0)
         AddMarkerObj(pad, kind_Marker, obj->Clone());
      else
      if (dynamic_cast<TGo4WinCond*>(obj)!=0)
         AddMarkerObj(pad, kind_Window, obj->Clone());
      else
      if (dynamic_cast<TGo4PolyCond*>(obj)!=0)
         AddMarkerObj(pad, kind_Poly, obj->Clone());
      else
      if (dynamic_cast<TPaveLabel*>(obj)!=0)
         AddDrawObject(pad, kind_Specials, obj->GetName(), obj->Clone(), kTRUE, drawopt ? drawopt : "br");
   }

   padopt->SetPadModified();
}

void TGo4ViewPanel::ProcessCanvasAdopt(TPad* tgtpad, TPad* srcpad, const char* srcpaditemname)
{
   if ((tgtpad==0) || (srcpad==0)) return;

//   cout << "ProcessCanvasAdopt " << srcpad->GetName() << endl;

   TGo4Slot* padslot = GetPadSlot(tgtpad);

   TGo4Picture* padopt = GetPadOptions(tgtpad);

   if ((padopt==0) || (padslot==0)) return;

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
   while (link!=0) {
      TObject* obj = link->GetObject();
      const char* drawopt = link->GetOption();

      TH1* h1 = 0;

      int kind = 0;

      TPad* srcsubpad = dynamic_cast<TPad*> (obj);

      TString itemname(srcpaditemname);
      itemname+="/";
      itemname+=obj->GetName();

      if (srcsubpad!=0) {
         nsubpads++;
         QString subpadname = tgtpad->GetName();
         subpadname += "_";
         subpadname += QString::number(nsubpads);

         Double_t xlow, ylow, xup, yup;

         srcsubpad->GetPadPar(xlow, ylow, xup, yup);

         tgtpad->cd();
         TPad* tgtsubpad = new TPad(subpadname.latin1(), srcsubpad->GetName(), xlow, ylow, xup, yup);
         tgtsubpad->SetNumber(nsubpads);
         tgtsubpad->Draw();

         TGo4Slot* subpadslot = AddNewSlot(tgtsubpad->GetName(), padslot);
         SetDrawKind(subpadslot, kind_PadSlot);
         SetSlotPad(subpadslot, tgtsubpad);

         ProcessCanvasAdopt(tgtsubpad, srcsubpad, itemname.Data());
      } else
      if (dynamic_cast<TGo4Condition*>(obj)!=0) {
         TGo4Condition* cond = (TGo4Condition*) obj->Clone();
         cond->SetWorkHistogram(0);
         AddDrawObject(tgtpad, kind_Condition, cond->GetName(), cond, kTRUE, 0);
      } else
      if (dynamic_cast<TGo4Marker*>(obj)!=0) {
          TGo4Marker* mark = (TGo4Marker*) obj->Clone();
          mark->SetHistogram(0);
          AddDrawObject(tgtpad, kind_Marker, mark->GetName(), mark, kTRUE, 0);
      } else
      if (dynamic_cast<TLatex*>(obj)!=0) {
         AddDrawObject(tgtpad, kind_Latex, obj->GetName(), obj->Clone(), kTRUE, 0);
      } else
      if (dynamic_cast<TPaveLabel*>(obj)!=0) {
         AddDrawObject(tgtpad, kind_Specials, obj->GetName(), obj->Clone(), kTRUE, drawopt ? drawopt : "br");
      } else
      if (dynamic_cast<TArrow*>(obj)!=0) {
         AddDrawObject(tgtpad, kind_Arrow, obj->GetName(), obj->Clone(), kTRUE, 0);
      } else

      if (dynamic_cast<TH1*>(obj)!=0) {
         kind = 1;
         h1 = (TH1*) obj;
      } else

      if (dynamic_cast<TGraph*>(obj)!=0) {
         kind = 2;
         h1 = ((TGraph*) obj)->GetHistogram();
      } else

      if (dynamic_cast<THStack*>(obj)!=0) {
         kind = 3;
         h1 = ((THStack*) obj)->GetHistogram();
      } else

      if (dynamic_cast<TMultiGraph*>(obj)!=0) {
         kind = 4;
         h1 = ((TMultiGraph*) obj)->GetHistogram();
      } else {
//         cout << tgtpad->GetName() << ":  Add other object ???" << obj->GetName() << endl;
      }

      // only first object is added,
      // make superimpose only for histos and graphs
      if ((kind>0) && ((mainkind==0) || (kind==mainkind) && (kind<3))) {

//         cout << tgtpad->GetName() << ":  Add main draw object " << obj->GetName()
//              << "  class =  " << obj->ClassName()
//              << "  srcitem = " << itemname << endl;

         if (drawopt!=0)
            padopt->SetDrawOption(drawopt, nmain);

         AddDrawObject(tgtpad, knd_Reference, obj->GetName(), obj, false, 0);

         mainkind = kind;

         if ((h1!=0) && (nmain==0)) {
            TakeFullRangeFromHisto(h1, padopt, nmain==0);
            Int_t ndim = h1->GetDimension();
            TakeSelectedAxisRange(0, padopt, h1->GetXaxis());
            if (ndim>1) TakeSelectedAxisRange(1, padopt, h1->GetYaxis());
            if (ndim>2) TakeSelectedAxisRange(2, padopt, h1->GetZaxis());
            if (ndim<3) {
               Double_t selmin = h1->GetMinimum();
               Double_t selmax = h1->GetMaximum();

               if (selmin<selmax) padopt->SetRange(ndim, selmin, selmax);

               padopt->SetAutoScale(!h1->TestBit(TH1::kIsZoomed) || (selmin>=selmax));
            }
         }

         nmain++;
      }

      link = link->Next();
   }

   if (nmain>1)
      padopt->SetSuperimpose(kTRUE);

   if (nsubpads==0)
      ScanDrawOptions(srcpad, padslot, padopt, false);
}

void TGo4ViewPanel::RedrawPanel(TPad* pad, bool force)
{
   if (IsRedrawBlocked()) return;

   TGo4LockGuard lock;

   BlockPanelRedraw(true);

   bool isanychildmodified = false;
   bool ispadupdatecalled = false;

   QTime starttm = QTime::currentTime();
   bool intime = true;

   // this loop done to allow consiquent update of multi-pad canvas
   // each subpad separetely redrawn in ProcessPadRedraw() method and
   // than pad->Update() is called. If it takes too long,
   // loop is finishing and via paint timer will be activated later

   do {
      TPad* selpad = TGo4WorkSpace::Instance()->GetSelectedPad();

      isanychildmodified = ProcessPadRedraw(pad, force);

      TGo4WorkSpace::Instance()->SetSelectedPad(selpad);

      // here pad->Update should redraw only modifed subpad
      if (isanychildmodified) {
         pad->Update();
         ispadupdatecalled = true;
      }

      int delay = starttm.msecsTo(QTime::currentTime());
      intime = (delay>=0) && (delay<100);

   } while (!force && isanychildmodified && intime);

   if (ActivePad!=0)
     UpdatePanelCaption();

   RefreshButtons();

   // to correctly select active pad, one should call canvas->Update()
   if ((pad!=GetCanvas()) || !ispadupdatecalled)
      GetCanvas()->Update();

   QCheckBox* box1 = dynamic_cast<QCheckBox*> (child("ApplyToAllCheck"));
   if (box1!=0) box1->setChecked(fbApplyToAllFlag);

   BlockPanelRedraw(false);

   if (!force && isanychildmodified)
     ShootRepaintTimer(pad);
}

bool TGo4ViewPanel::ProcessPadRedraw(TPad* pad, bool force)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return false;

   TGo4Picture* padopt = GetPadOptions(slot);
   if (padopt==0) return false;

   bool ischilds = false;
   bool ischildmodified = false;

   CheckObjectsAssigments(pad, slot);

   CheckForSpecialObjects(pad, slot);

   // this parameter ensure that all pads will be scaned one after another
   Int_t lastdrawnpad = 0;
   if (!force)
     if (!slot->GetIntPar("::LastDrawnPad",lastdrawnpad))
        lastdrawnpad = 0;

   Int_t subpadindx = 0;

   // first redraw all subpads
   for(int n=0;n<slot->NumChilds();n++) {
      subpadindx = (n + lastdrawnpad) % slot->NumChilds();
      TPad* subpad = GetSlotPad(slot->GetChild(subpadindx));
      if (subpad==0) continue;
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

   if (!force && !padopt->IsPadModified()) return ischildmodified;

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
   CollectMainDrawObjects(slot, &objs, &objslots, padopt->IsSuperimpose() ? 1 : 2);

   ScanObjectsDrawOptions(false, slot, &objs, &objslots);

   TGo4Slot* sislot = GetSuperimposeSlot(slot);
   TGo4Slot* legslot = slot->FindChild("::Legend");
   TGo4Slot* asislot = slot->FindChild("::ASImage");

   // if nothing to draw, delete all additional slots and exit
   if (objs.GetLast()<0) {
      delete sislot;
      delete legslot;
      delete asislot;

      CallPanelFunc(panel_Updated, pad);

      // indicate that nothing is happen
      return true;
   }

   TH2* asihisto = 0;

   TObject* drawobj = 0;

   // Bool_t dosuperimpose = padopt->IsSuperimpose() && (objs.GetLast()>0);
   Bool_t dosuperimpose = objs.GetLast()>0;

   const char* drawopt = padopt->GetDrawOption(0);

   Bool_t doasiimage = (drawopt!=0) && !dosuperimpose && objs.Last()->InheritsFrom(TH2::Class());
   if (doasiimage)
      doasiimage = strstr(drawopt,"asimage")!=0;

   if (dosuperimpose) {
     if (sislot==0)
        sislot = new TGo4Slot(slot,"::Superimpose","place for superimpose object");
     if (padopt->IsLegendDraw()) {
        if (legslot==0)
          legslot = new TGo4Slot(slot,"::Legend","place for superimpose object");
     } else
       if (legslot!=0) {
          delete legslot;
          legslot = 0;
       }

     drawobj = ProduceSuperimposeObject(padopt, sislot, legslot, &objs, &objslots, padopt->IsTitleItem());
     if (drawobj==0) dosuperimpose = kFALSE;
   }

   if (!dosuperimpose) {
      if (sislot!=0) { delete sislot; sislot = 0; }
      if (legslot!=0) { delete legslot; legslot = 0; }
      drawobj = objs.Last();
   }

   if (doasiimage) {
      asihisto = dynamic_cast<TH2*> (drawobj);
      if (asihisto==0)
        doasiimage = false;
      else {
        if (asislot==0)
          asislot = new TGo4Slot(slot,"::ASImage","place for Go4 ASI image");
        TGo4ASImage* image = dynamic_cast<TGo4ASImage*> (asislot->GetAssignedObject());
        if (image==0) {
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
      if (asislot!=0) { delete asislot; asislot = 0; }
   }

   gPad = pad;  // instead of pad->cd(), while it is redraw frame

   if (drawobj!=0)
      if (drawobj->InheritsFrom(TH1::Class())) {
          TH1* h1 = (TH1*) drawobj;
          h1->SetBit(kCanDelete, kFALSE);
          RedrawHistogram(pad, padopt, h1, updatecontent);
       } else
       if(drawobj->InheritsFrom(THStack::Class())) {
          THStack* hs = (THStack*) drawobj;
          RedrawStack(pad, padopt, hs, dosuperimpose, updatecontent);
       } else
       if(drawobj->InheritsFrom(TGraph::Class())) {
          TGraph* gr = (TGraph*) drawobj;
          RedrawGraph(pad, padopt, gr, updatecontent);
       } else
       if(drawobj->InheritsFrom(TMultiGraph::Class())) {
          TMultiGraph* mg = (TMultiGraph*) drawobj;
          RedrawMultiGraph(pad, padopt, mg, dosuperimpose, updatecontent);
       }
       else
       if(dynamic_cast<TGo4ASImage*>(drawobj)!=0) {
          TGo4ASImage* ai = (TGo4ASImage*) drawobj;
          RedrawImage(pad, padopt, ai, asihisto, updatecontent);
       }

   if (legslot!=0)
      RedrawLegend(pad, padopt, legslot);

   if (!doasiimage)
      RedrawSpecialObjects(pad, slot);

   if (padopt->HasTitleAttr()) {
      TPaveText* titl = dynamic_cast<TPaveText*>
              (pad->GetListOfPrimitives()->FindObject("title"));
      if (titl==0) {
         pad->Update();
         titl = dynamic_cast<TPaveText*>
                       (pad->GetListOfPrimitives()->FindObject("title"));
      }
      if (titl) {
         padopt->GetTitleAttr(titl);
         pad->Modified();
      }
   }

   CallPanelFunc(panel_Updated, pad);

   return true;
}

void TGo4ViewPanel::RedrawHistogram(TPad *pad, TGo4Picture* padopt, TH1 *his, bool scancontent)
{
   if((pad==0) || (padopt==0) || (his==0)) return;

   if (scancontent)
      TakeFullRangeFromHisto(his, padopt, true);

   TString drawopt(padopt->GetDrawOption(0));
   if (drawopt.Length()==0) drawopt="hist";
   drawopt.ToUpper();

   his->SetStats(padopt->IsHisStats());
   his->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   his->Draw(drawopt.Data());

   SetSelectedRangeToHisto(his, 0, padopt, true);
}

void TGo4ViewPanel::RedrawStack(TPad *pad, TGo4Picture* padopt, THStack * hs, bool dosuperimpose, bool scancontent)
{
   if((pad==0) || (padopt==0) || (hs==0)) return;

   if (scancontent) {
      TIter iter(hs->GetHists());
      TH1 *h1 = 0;
      bool first = true;
      while ((h1 = (TH1*)iter())!=0) {
         TakeFullRangeFromHisto(h1, padopt, first);
         first = false;
      }
   }

   // never draw statistics with stack

   Int_t drawoptindx = dosuperimpose ? TGo4Picture::PictureIndex : 0;
   TString drawopt(padopt->GetDrawOption(drawoptindx));
   if (drawopt.Length()==0) drawopt="hist";
   if (!drawopt.Contains(NoStackDrawOption, TString::kIgnoreCase))
     drawopt.Prepend(NoStackDrawOption);

   hs->Draw(drawopt.Data());
   TH1* framehisto = hs->GetHistogram();

   if (framehisto==0) return;

   framehisto->SetStats(false);
   framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   if (hs->GetHists()) {
      TH1* h1 = dynamic_cast<TH1*> (hs->GetHists()->First());
      if (h1!=0) {
         hs->SetTitle(h1->GetTitle());
         framehisto->SetTitle(h1->GetTitle());
         framehisto->GetXaxis()->SetTitle(h1->GetXaxis()->GetTitle());
         framehisto->GetYaxis()->SetTitle(h1->GetYaxis()->GetTitle());
         framehisto->GetZaxis()->SetTitle(h1->GetZaxis()->GetTitle());
      }
   }

   SetSelectedRangeToHisto(framehisto, hs, padopt, false);
}


void TGo4ViewPanel::RedrawGraph(TPad *pad, TGo4Picture* padopt, TGraph * gr, bool scancontent)
{
   if((pad==0) || (padopt==0) || (gr==0)) return;

   if (scancontent) {
      TakeFullRangeFromGraph(gr, padopt, true);
      gr->SetEditable(kFALSE);
   }

   TString drawopt(padopt->GetDrawOption(0));

   if (drawopt.Length()==0) drawopt="AP";

   TH1* framehisto = gr->GetHistogram();
   if (framehisto==0) {
      gr->Draw(drawopt.Data());
      framehisto = gr->GetHistogram();
   }
   if (framehisto!=0) {
     framehisto->SetStats(padopt->IsHisStats());
     framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());
   }
   gr->Draw(drawopt.Data());

   SetSelectedRangeToHisto(framehisto, 0, padopt, false);
}

void TGo4ViewPanel::RedrawMultiGraph(TPad *pad, TGo4Picture* padopt, TMultiGraph * mg, bool dosuperimpose, bool scancontent)
{
   if((pad==0) || (padopt==0) || (mg==0)) return;

   TIter iter(mg->GetListOfGraphs());
   TGraph *gr(0), *firstgr(0);
   bool first = true;
   while ((gr = (TGraph*)iter())!=0) {
      if (scancontent) {
         gr->SetEditable(kFALSE);
         TakeFullRangeFromGraph(gr, padopt, first);
      }
      if (first) firstgr = gr;
      first = false;
   }

   Int_t drawoptindx = dosuperimpose ? TGo4Picture::PictureIndex : 0;
   TString drawopt(padopt->GetDrawOption(drawoptindx));
   if (drawopt.Length()==0) drawopt="AP";
   if (dosuperimpose) drawopt = "";

   // never draw statistics with multigraph
   TH1* framehisto = (dosuperimpose && (firstgr!=0)) ? firstgr->GetHistogram() : mg->GetHistogram();

   if (framehisto==0) {
      mg->Draw(drawopt.Data());
      framehisto = (dosuperimpose && (firstgr!=0)) ? firstgr->GetHistogram() : mg->GetHistogram();
   }

   if (framehisto!=0) {

      SetSelectedRangeToHisto(framehisto, 0, padopt, false);

      // this is workaround to prevent recreation of framehistogram in TMultiGraf::Paint

      Double_t miny, maxy, selmin, selmax;
      if (padopt->GetFullRange(1, miny, maxy) && !padopt->GetRangeY(selmin, selmax)) {
         framehisto->SetMaximum(maxy);
         framehisto->SetMinimum(miny);
      }

      framehisto->SetStats(kFALSE);
      framehisto->SetBit(TH1::kNoTitle, !padopt->IsHisTitle());

      // set title of first TGraph to TMultiGraph and frame histo
      if (firstgr!=0) {
         mg->SetTitle(firstgr->GetTitle());
         framehisto->SetTitle(firstgr->GetTitle());
         framehisto->GetXaxis()->SetTitle(firstgr->GetXaxis()->GetTitle());
         framehisto->GetYaxis()->SetTitle(firstgr->GetYaxis()->GetTitle());
      }
   }
   mg->Draw(drawopt.Data());
}

void TGo4ViewPanel::RedrawImage(TPad *pad, TGo4Picture* padopt, TGo4ASImage* im, TH2* asihisto, bool scancontent)
{
   if((pad==0) || (padopt==0) || (im==0)) return;

   im->SetDrawData(asihisto, this, pad);

   if(scancontent)
      TakeFullRangeFromHisto(asihisto, padopt, true);

   double uminx, umaxx, uminy, umaxy;
   padopt->GetRange(0, uminx, umaxx);
   padopt->GetRange(1, uminy, umaxy);
   im->SetSelectedRange(uminx, umaxx, uminy, umaxy);

   im->Draw();
}

void TGo4ViewPanel::RedrawLegend(TPad *pad, TGo4Picture* padopt, TGo4Slot* legslot)
{
   if (legslot==0) return;
   TLegend* legend = dynamic_cast<TLegend*> (legslot->GetAssignedObject());
   if (legend!=0)
      legend->Draw();
}

void TGo4ViewPanel::RedrawSpecialObjects(TPad *pad, TGo4Slot* padslot)
{
   if ((pad==0) || (padslot==0)) return;

   CheckObjectsAssigments(pad, padslot);

   QString selname = GetSelectedMarkerName(pad);
   TObject* selectedobj = 0;
   const char* selectdrawopt = 0;

   for(int n=0;n<padslot->NumChilds();n++) {
      TGo4Slot* subslot = padslot->GetChild(n);

      Int_t kind = GetDrawKind(subslot);

      if ((kind<kind_Specials) || (kind>=kind_Other)) continue;

      const char* drawopt = GetSpecialDrawOption(subslot);

      TObject* obj = subslot->GetAssignedObject();

      if (obj==0) continue;

      if ((selname == obj->GetName()) && (selectedobj==0)) {
         selectedobj = obj;
         selectdrawopt = drawopt;
      } else
         obj->Draw(drawopt ? drawopt : "");
   }

   // if one has selected object on the pad, one should
   // draw it as last to bring it to the front of other
   if (selectedobj!=0)
      selectedobj->Draw(selectdrawopt ? selectdrawopt : "");
}

bool TGo4ViewPanel::IsApplyToAllFlag()
{
   return fbApplyToAllFlag;
}

bool TGo4ViewPanel::IsFreezeTitle()
{
   return fbFreezeTitle;
}

void TGo4ViewPanel::ChangeDrawOptionForPad(TGo4Slot* padslot, int kind, int value, const char* drawopt )
{
   TGo4LockGuard lock;

   TGo4Picture* subopt = GetPadOptions(padslot);
   if (subopt==0) return;
   switch(kind) {
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
         if (subpad!=0) {
            subpad->SetFillColor(value);
            if (subpad->GetFrame())
              subpad->GetFrame()->SetFillColor(value);
         }
         subopt->SetPadModified();
         break;
      }
      default:
        subopt->ChangeDrawOption(kind, value);
   }
}

void TGo4ViewPanel::ChangeDrawOption(int kind, int value, const char* drawopt)
{
   TGo4LockGuard lock;

   bool scanall = IsApplyToAllFlag();

   if (kind==100) {
      gStyle->SetPalette(value);
      scanall = true;
   }

   TPad* pad = scanall ? GetCanvas() : GetActivePad();
   if (pad==0) pad = GetCanvas();
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return;

   ChangeDrawOptionForPad(slot, kind, value, drawopt);

   TGo4Iter iter(slot, true);
   while (iter.next())
      ChangeDrawOptionForPad(iter.getslot(), kind, value, drawopt);

   RedrawPanel(pad, false);
}

void TGo4ViewPanel::ResetPadFillColors(TPad* pad, int col)
{
   TGo4LockGuard lock;

   if(pad==0) return;
   pad->SetFillColor((Color_t)col);
   TIter iter(pad->GetListOfPrimitives());
   TObject* obj = 0;
   while((obj=iter()) != 0) {
      TPad* subpad = dynamic_cast<TPad*>(obj);
      TFrame* fram = dynamic_cast<TFrame*>(obj);
      if(subpad!=0)
         ResetPadFillColors(subpad, col);
      else
      if(fram!=0)
         fram->SetFillColor((Color_t)col);
   }
}

void TGo4ViewPanel::ClearPad(TPad* pad, bool removeitems, bool removesubpads)
{
   TGo4LockGuard lock;

   BlockPanelRedraw(true);
   CleanupGedEditor();
   ProcessPadClear(pad, removeitems, removesubpads);
   if (ActivePad==0)
     SetActivePad(GetCanvas());
   GetCanvas()->SetSelected(0);
   BlockPanelRedraw(false);
}

void TGo4ViewPanel::ClearPadItems(TGo4Slot* padslot, TGo4Slot* remain)
{
   if (padslot==0) return;

   for (int n=padslot->NumChilds()-1; n>=0; n--) {
      TGo4Slot* subslot = padslot->GetChild(n);
      int kind = GetDrawKind(subslot);
      if ((kind<kind_Permanet) && (subslot!=remain)) {
         delete subslot;
      }
   }
}

void TGo4ViewPanel::ProcessPadClear(TPad * pad, bool removeitems, bool removesubpads)
{
   TGo4Slot* slot = GetPadSlot(pad);
   TGo4Picture* padopt = GetPadOptions(slot);
   if ((slot==0) || (padopt==0)) return;

//   padopt->Clear(""); // remove all settings completely
   padopt->SetPadModified();

   if (removeitems)
     ClearPadItems(slot, 0);

   CheckObjectsAssigments(pad, slot);

   for(int n=slot->NumChilds()-1; n>=0; n--) {
      TGo4Slot* subslot = slot->GetChild(n);

      TPad* subpad = GetSlotPad(subslot);
      if (subpad==0) continue;
      ProcessPadClear(subpad, removeitems || removesubpads, removesubpads);
      if (!removesubpads) continue;
      if (ActivePad==subpad) ActivePad=0;

      delete subslot;

      pad->GetListOfPrimitives()->Remove(subpad);

      CallPanelFunc(panel_PadDeleted, subpad);

      if (GetCanvas()->GetSelectedPad()==subpad)
        GetCanvas()->SetSelectedPad(0);

      delete subpad;
   }

   CallPanelFunc(panel_Modified, pad);
}

void TGo4ViewPanel::SetPadDefaults(TPad* pad)
{
   TGo4LockGuard lock;

   if (pad==0) return;
   gStyle->SetOptStat(go4sett->getOptStat());
   fbCanvasCrosshair = go4sett->getPadCrosshair();
   fbCanvasEventstatus = go4sett->getPadEventStatus();

   int fiPadcolorR, fiPadcolorG, fiPadcolorB;
   go4sett->getCanvasColor(fiPadcolorR, fiPadcolorG, fiPadcolorB);
   int padfillcolor =  TColor::GetColor(fiPadcolorR, fiPadcolorG, fiPadcolorB);

   if (padfillcolor!=0) {
     // now define associated colors for WBOX shading
     // note: root restricts this mechanism to number<50
     // we extend it since TGX11ttf expects the shadow color
     // the following are copies from TColor code:
     Int_t nplanes = 16;
     if (gVirtualX) gVirtualX->GetPlanes(nplanes);
     if (nplanes == 0) nplanes = 16;
     TColor *normal = gROOT->GetColor(padfillcolor);
     if(normal!=0) {
       Float_t h,l,s;
       normal->GetHLS(h,l,s);
       const char* cname = normal->GetName();
       Text_t aname[64];
       // assign the color numbers and names for shading:
       Float_t dr, dg, db, lr, lg, lb;
       TColor *dark = gROOT->GetColor(100+padfillcolor);
       if(dark==0) {
          snprintf(aname,64,"%s%s",cname,"_dark");
          new TColor(100+padfillcolor, -1, -1, -1, aname);
          dark = gROOT->GetColor(100+padfillcolor);
       }
       TColor *light = gROOT->GetColor(150+padfillcolor);
       if(light==0) {
          snprintf(aname,64,"%s%s",cname,"_bright");
          new TColor(150+padfillcolor, -1, -1, -1, aname);
          light = gROOT->GetColor(150+padfillcolor);
       }

       // set dark color
       TColor::HLStoRGB(h, 0.7*l, s, dr, dg, db);
       if (dark!=0) {
          if (nplanes > 8) dark->SetRGB(dr, dg, db);
          else             dark->SetRGB(0.3,0.3,0.3);
       }

      // set light color
      TColor::HLStoRGB(h, 1.2*l, s, lr, lg, lb);
      if (light!=0) {
         if (nplanes > 8) light->SetRGB(lr, lg, lb);
         else             light->SetRGB(0.8,0.8,0.8);
      }
     } else { // if(normal)
       cerr <<"TGo4ViewPanel:: Could not assign root shadow colors for number " << padfillcolor << endl;
       cerr <<"Never come here!!!" <<endl;
     }
   }

   pad->SetCrosshair(fbCanvasCrosshair);
   pad->SetFillColor(padfillcolor);

   fMenuBar->setItemChecked(EventStatusId, !fbCanvasEventstatus);
   ShowEventStatus();

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt!=0) {
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
   if (pad==0) return;
   QString output = pad->GetName();
   output.append(": ");
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt!=0)
       if (padopt->IsSuperimpose()) output.append(" SuperImpose:");

   if (IsApplyToAllFlag()) output.append(" All Pads:");
   output.append(" Ready");
   if (CanvasStatus!=0)
     CanvasStatus->message(output);
}

void TGo4ViewPanel::MoveScale(int expandfactor, int xaction, int yaction, int zaction)
{
   TPad* selpad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (selpad==0) return;

//   cout << "TGo4ViewPanel::MoveScale " << expandfactor << " " << xaction << endl;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt!=0) {
//     padopt->MoveScale(expandfactor, xaction, yaction);
      MoveSingleScale(expandfactor, xaction, 0, padopt);
      MoveSingleScale(expandfactor, yaction, 1, padopt);
      MoveSingleScale(expandfactor, zaction, 2, padopt);
      if ((xaction==0) && (yaction==0) && (zaction==0)) {
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
         if (padopt==0) continue;

//       padopt->MoveScale(expandfactor, xaction, yaction);
         MoveSingleScale(expandfactor, xaction, 0, padopt);
         MoveSingleScale(expandfactor, yaction, 1, padopt);
         MoveSingleScale(expandfactor, zaction, 2, padopt);
         if ((xaction==0) && (yaction==0) && (zaction==0)) {
            padopt->ClearRange();
            padopt->SetAutoScale(true);
         }
         padopt->SetPadModified();
      }
   }

   RedrawPanel(selpad, false);
}

void TGo4ViewPanel::MoveSingleScale( int expandfactor, int action, int naxis, TGo4Picture* padopt)
{
   if (action<=0) return;

   double new_umin, new_umax, fmin, fmax, tmin,tmax;
   double fact = expandfactor / 100.;

   bool sel = padopt->GetRange(naxis, new_umin, new_umax);

   padopt->GetFullRange(naxis, fmin, fmax);

   // we use number of dimensions to determine if we have contents
   int ndim = padopt->GetFullRangeDim();

   if (!sel || (new_umin>=new_umax))
      padopt->GetFullRange(naxis, new_umin, new_umax);

   double shift = (new_umax - new_umin) * fact;
   // protect if changes is out of full axis ranges
   if (shift<=0) return;

   switch(action) {
      case 1: if ((new_umax+shift) > fmax) // SlotShiftL
                 shift = fmax - new_umax;
              new_umin += shift;
              new_umax += shift;
              break;
      case 2: if (new_umin-shift < fmin) // SlotShiftR
                 shift = new_umin - fmin;
              new_umin -= shift;
              new_umax -= shift;
              break;
      case 3: if(ndim == 1){
                 new_umax -= shift;  // Expand 1d upper
              if(naxis < 1)          // lower only for X
                new_umin += shift;  // Expand 1d lower X
              }
              if(ndim == 2){
                 new_umax -= shift;  // Expand 2d upper
              if(naxis < 2)          // lower only for X,Y
                new_umin += shift;  // Expand 1d lower X,Y
              }
              break;
      case 4: if(ndim == 1){       // Shrink 1d upper
              if(naxis < 1){        // X axis
                 tmax = (-fact*new_umin+(1.-fact)*new_umax)/(1.-2.*fact);
                tmin = (-fact*new_umax+(1.-fact)*new_umin)/(1.-2.*fact);  // Shrink 1d X
                }
                else {              // content, lower has not been expanded
                 tmax = (-fact*new_umin+new_umax)/(1.-fact);
                tmin = (-fact*new_umax+(1.-fact)*new_umin)/(1.-2.*fact);  // Shrink 1d X
                }
              }
              if(ndim == 2){
              if(naxis < 2){        // X,Y axis
                 tmax = (-fact*new_umin+(1.-fact)*new_umax)/(1.-2.*fact);
                tmin = (-fact*new_umax+(1.-fact)*new_umin)/(1.-2.*fact);  // Shrink 1d X
                }
                else {              // content, lower has not been expanded
                 tmax = (-fact*new_umin+new_umax)/(1.-fact);
                tmin = (-fact*new_umax+(1.-fact)*new_umin)/(1.-2.*fact);  // Shrink 1d X
                }
              }
             new_umin=tmin;
               new_umax=tmax;
              if (new_umin < fmin) new_umin = fmin;
              if (new_umax > fmax) new_umax = fmax;
              break;
      default: return;
   }

   TakeSelectedAxisRange(naxis, padopt, new_umin, new_umax, false);

   if (naxis==ndim)
     if (padopt->GetRange(naxis, new_umin, new_umax))
        padopt->SetAutoScale(kFALSE);
}

void TGo4ViewPanel::TakeFullRangeFromHisto(TH1* h1, TGo4Picture* padopt, bool isfirsthisto)
{
   if ((h1==0) || (padopt==0)) return;

   TAxis* xax = h1->GetXaxis();
   TAxis* yax = h1->GetYaxis();
   TAxis* zax = h1->GetZaxis();
   int ndim = h1->GetDimension();

   if (isfirsthisto) {
      padopt->SetFullRangeDim(ndim);
      padopt->SetFullRange(0, xax->GetBinLowEdge(1),
                              xax->GetBinUpEdge(xax->GetNbins()));
      if (ndim>1)
         padopt->SetFullRange(1, yax->GetBinLowEdge(1),
                                 yax->GetBinUpEdge(yax->GetNbins()));
      else
         padopt->ClearFullRange(2);

      if (ndim>2)
         padopt->SetFullRange(2, zax->GetBinLowEdge(1),
                                 zax->GetBinUpEdge(zax->GetNbins()));
   } else
      ndim = padopt->GetFullRangeDim();

   if (ndim>=3) return;

   Int_t dimindx = (ndim==1) ? 1 : 2;

   Double_t minimum = 0, maximum = 0;
   Bool_t first = kTRUE;
   if (!isfirsthisto) {
      padopt->GetFullRange(dimindx, minimum, maximum);
      first = kFALSE;
   }

   for (Int_t biny=1;biny<=yax->GetNbins();biny++)
     for (Int_t binx=1;binx<=xax->GetNbins();binx++) {
        Int_t bin = h1->GetBin(binx,biny);
        Double_t value = h1->GetBinContent(bin);
        if (first) {
           minimum = value;
           maximum = value;
           first = kFALSE;
        }
        if (value < minimum) minimum = value; else
        if (value > maximum) maximum = value;
     }

   padopt->SetFullRange(dimindx, minimum, maximum);
}

void TGo4ViewPanel::TakeFullRangeFromGraph(TGraph * gr, TGo4Picture * padopt, bool isfirst)
{
   if ((gr==0) || (padopt==0)) return;

   Double_t minx(0), maxx(0), miny(0), maxy(0), xx, yy;

   if (isfirst) {
      if (gr->GetN()>0) {
         gr->GetPoint(0, minx, miny);
         maxx = minx; maxy = miny;
      }
   } else {
      padopt->GetFullRange(0, minx, maxx);
      padopt->GetFullRange(1, miny, maxy);
   }

   for (Int_t n=0;n<gr->GetN();n++) {
      gr->GetPoint(n, xx, yy);
      if (xx<minx) minx = xx; else if (xx>maxx) maxx = xx;
      if (yy<miny) miny = yy; else if (yy>maxy) maxy = yy;
   }

   padopt->SetFullRangeDim(1);
   padopt->SetFullRange(0, minx, maxx);
   padopt->SetFullRange(1, miny, maxy);
   padopt->ClearFullRange(2);
}

void TGo4ViewPanel::SetSelectedRangeToHisto(TH1* h1, THStack* hs, TGo4Picture* padopt, bool isthishisto)
{
   // set selected range and stats position for histogram

   if ((h1==0) || (padopt==0)) return;

   int ndim = padopt->GetFullRangeDim();

   bool autoscale = padopt->IsAutoScale();

   double hmin(0.), hmax(0.), umin, umax;

   if (padopt->GetRange(0, umin, umax))
      {
        // note: go4 range is full visible range of histogram
        // [low edge first bin, up edge last bin]
        // need to correct for upper bin width when transforming to ROOT user range:
        TAxis* ax=h1->GetXaxis();
        Double_t bwidthx=ax->GetBinWidth(ax->FindFixBin(umax));
        ax->SetRangeUser(umin, umax-bwidthx);
      }
   else
      h1->GetXaxis()->UnZoom();

   // this peace of code allows autoscale for THStack when autoscale is
   // on and some range is selected. In this case ROOT uses h1->GetMaximum(),
   // which is calculated inside selected range. Therefore, even if range
   // selection of any histogram does not affect range with of THStack, range
   // selection defines GetMaximum() and GetMinimum() behaviour, used in autoscale
   // Comment out, while with unzoom of X axis user do not see full histogram range,
   // only previous selection until next refresh

/*
   if (hs!=0) {
      TIter next(hs->GetHists());
      TH1* hs_h1 = 0;
      while ( (hs_h1 = (TH1*) next()) !=0 )
         if (padopt->GetRange(0, umin, umax))
            {
             // note: go4 range is full visible range of histogram
             // [low edge first bin, up edge last bin]
            // need to correct for upper bin width when transforming to ROOT user range:
            TAxis* as=hs_h1->GetXaxis();
            Double_t bwidths=as->GetBinWidth(as->FindFixBin(umax));
            as->SetRangeUser(umin, umax-bwidths);
            }
         else
            hs_h1->GetXaxis()->UnZoom();
   }
*/
   if (padopt->GetRange(1, umin, umax)) {
      if (!autoscale && (ndim==1)) {
         hmin = umin;
         hmax = umax;
      }
        // note: go4 range is full visible range of histogram
        // [low edge first bin, up edge last bin]
        // need to correct for upper bin width when transforming to ROOT user range:
        TAxis* ay=h1->GetYaxis();
        Double_t bwidthy=ay->GetBinWidth(ay->FindFixBin(umax));
        ay->SetRangeUser(umin, umax-bwidthy);
   } else {
      h1->GetYaxis()->UnZoom();
   }

   if (padopt->GetRange(2, umin, umax) && (ndim>1)) {
     if (!autoscale && (ndim==2)) {
        hmin = umin;
        hmax = umax;
     }
         // note: go4 range is full visible range of histogram
        // [low edge first bin, up edge last bin]
        // need to correct for upper bin width when transforming to ROOT user range:
        TAxis* az=h1->GetZaxis();
        Double_t bwidthz=az->GetBinWidth(az->FindFixBin(umax));
        az->SetRangeUser(umin, umax-bwidthz);
   } else
     h1->GetZaxis()->UnZoom();

   if (hmin!=hmax) {
      // if scale axis is log, prevent negative values, othervise
      // histogram will not be displayed
      if (padopt->GetLogScale(ndim)) {
         if (hmax<=0) hmax = 1.;
         if ((hmin<=0) || (hmin>=hmax)) {
            hmin = hmax*1e-4;
            if (hmin>1.) hmin = 1;
         }
      }

      h1->SetMinimum(hmin);
      h1->SetMaximum(hmax);
      h1->SetBit(TH1::kIsZoomed);
      if (hs!=0) {
         hs->SetMinimum(hmin);
         hs->SetMaximum(hmax);
      }
   } else {
      // this is autoscale mode,
      if (hs!=0) {
         if (ndim==1) {
            TIter next(hs->GetHists());
            TH1* hs_h1 = 0;
            while ( (hs_h1 = (TH1*) next()) !=0 ) {
               if (padopt->GetRange(0, umin, umax)) {
                  TAxis* ax = hs_h1->GetXaxis();
                  Double_t bwidths = ax->GetBinWidth(ax->FindFixBin(umax));
                  ax->SetRangeUser(umin, umax-bwidths);
               } else
                  hs_h1->GetXaxis()->UnZoom();

               hs_h1->GetYaxis()->UnZoom();
            }
         }
         hs->SetMinimum();
         hs->SetMaximum();
      }
      h1->SetMinimum();
      h1->SetMaximum();
      h1->ResetBit(TH1::kIsZoomed);
      // here one can estimate actual range  which will be displayed on canvas

      if (ndim<3) {
         Double_t selmin = h1->GetMinimum();
         Double_t selmax = h1->GetMaximum();

         if (selmin>=selmax) padopt->ClearRange(ndim);
         else {
            bool islogscale = (ndim==1) && (padopt->GetLogScale(1)>0);

            if (islogscale) {
               if ((selmin>0) && (selmax>0)) {
                  selmin = TMath::Log10(selmin) + TMath::Log10(0.5);
                  selmin = TMath::Power(10, selmin);
                  selmax = TMath::Log10(selmax) + TMath::Log10(2*(0.9/0.95));
                  selmax = TMath::Power(10, selmax);
               }

            } else {
               Double_t yMARGIN = 0.05;
               #if ROOT_VERSION_CODE > ROOT_VERSION(5,0,9)
               yMARGIN = gStyle->GetHistTopMargin();
               #endif
               Double_t dymin = yMARGIN*(selmax-selmin);
               if ((selmin>=0) && (selmin-dymin < 0)) selmin = 0;
                                           else selmin-=dymin;
               selmax += yMARGIN*(selmax-selmin);
            }

            padopt->SetRange(ndim, selmin, selmax);
         }
      }

   }

   if (padopt->IsHisStats() && isthishisto) {
      TPaveStats* stats = dynamic_cast<TPaveStats*>
              (h1->GetListOfFunctions()->FindObject("stats"));
      if (stats==0) {
         stats  = new TPaveStats(
                       gStyle->GetStatX()-gStyle->GetStatW(),
                       gStyle->GetStatY()-gStyle->GetStatH(),
                       gStyle->GetStatX(),
                       gStyle->GetStatY(),"brNDC");
          stats->SetParent(h1);
          stats->UseCurrentStyle();
          stats->SetName("stats");
          h1->GetListOfFunctions()->Add(stats);
          stats->ConvertNDCtoPad();
        }
      padopt->GetStatsAttr(stats);
   }

   if (padopt->IsHisTitle() && isthishisto) {
      TPaveText* titl = dynamic_cast<TPaveText*>
              (h1->GetListOfFunctions()->FindObject("title"));
      if (titl) padopt->GetTitleAttr(titl);
   }
}

bool TGo4ViewPanel::GetVisibleRange(TPad* pad, int naxis, double& min, double& max)
{
   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt==0) return false;

   int NumDim = padopt->GetFullRangeDim();

   if ((naxis<0) || (naxis>NumDim)) return false;

   bool res = padopt->GetRange(naxis, min, max);
   if (!res || (min>=max))
      res = padopt->GetFullRange(naxis, min, max);
   return res;
}


bool TGo4ViewPanel::TakeSelectedAxisRange(int naxis, TGo4Picture* padopt, double selmin, double selmax, bool force)
{
   if ((selmin==-1) && (selmax==-1)) return false;

//   cout << "TakeSelectedAxisRange " << naxis << "  min = " << selmin << "  max = " << selmax << endl;

   double min, max, umin, umax;

   bool full = padopt->GetFullRange(naxis, min, max);
   bool sel = padopt->GetRange(naxis, umin, umax);

   if (!full || (min>=max) || (selmin>=selmax)) {
      padopt->ClearRange(naxis);
      return true;
   }

   double delta = (max-min) / 100000.;
   bool changed = false;

   if (umin<umax) {
      if (fabs(umin-selmin)>delta) { umin = selmin; changed = true; }
      if (fabs(umax-selmax)>delta) { umax = selmax; changed = true; }
   } else {
      umin = selmin;
      umax = selmax;
      changed = true;
   }

   if ((selmin<min+delta) && (selmax>max-delta) && !force)
      padopt->ClearRange(naxis);
   else
      padopt->SetRange(naxis, umin, umax);

   return changed;
}

bool TGo4ViewPanel::TakeSelectedAxisRange(int naxis,  TGo4Picture * padopt, TAxis* ax)
{
   Double_t selmin, selmax;

   // check if something selected on axis
   if (((ax->GetFirst()<=0) && (ax->GetLast()>=ax->GetNbins()-1)) ||
       (ax->GetFirst()>=ax->GetLast())) {
          selmin = -1111;
          selmax = -1111;
       } else {
          selmin = ax->GetBinLowEdge(ax->GetFirst());
          selmax = ax->GetBinUpEdge(ax->GetLast());
       }

   return TakeSelectedAxisRange(naxis, padopt, selmin, selmax, false);
}

void TGo4ViewPanel::PadRangeAxisChanged(TPad* pad)
{
   TGo4LockGuard lock;

   TGo4Picture* padopt = GetPadOptions(pad);

   if (IsRedrawBlocked() || (pad==0) || (padopt==0)) return;

   // check if we have histogram and can take range from it
   TH1* h1 = GetPadHistogram(pad);

   if (h1!=0) {
      Int_t ndim = padopt->GetFullRangeDim();

      TakeSelectedAxisRange(0, padopt, h1->GetXaxis());
      if (ndim>1) TakeSelectedAxisRange(1, padopt, h1->GetYaxis());
      if (ndim>2) TakeSelectedAxisRange(2, padopt, h1->GetZaxis());
      if (ndim<3) {

         bool iszoomed = h1->TestBit(TH1::kIsZoomed);

         padopt->SetAutoScale(!iszoomed);

         Double_t selmin = h1->GetMinimum();
         Double_t selmax = h1->GetMaximum();

         if (iszoomed)
            padopt->SetRange(ndim, selmin, selmax);
         else
            if (selmin>=selmax) padopt->ClearRange(ndim);
            else {
               bool islogscale = (ndim==1) && (pad->GetLogy()>0);

               if (islogscale) {
                  if ((selmin>0) && (selmax>0)) {
                     selmin = TMath::Log10(selmin) + TMath::Log10(0.5);
                     selmin = TMath::Power(10, selmin);
                     selmax = TMath::Log10(selmax) + TMath::Log10(2*(0.9/0.95));
                     selmax = TMath::Power(10, selmax);
                  }

               } else {
                  Double_t yMARGIN = 0.05;
                  #if ROOT_VERSION_CODE > ROOT_VERSION(5,0,9)
                  yMARGIN = gStyle->GetHistTopMargin();
                  #endif
                  Double_t dymin = yMARGIN*(selmax-selmin);
                  if ((selmin>=0) && (selmin-dymin < 0)) selmin = 0;
                                              else selmin-=dymin;
                  selmax += yMARGIN*(selmax-selmin);
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

   if (pad->GetLogx()>0) {
      rxmin = TMath::Power(10., rxmin);
      rxmax = TMath::Power(10., rxmax);
   }

   if (pad->GetLogy()>0) {
      rymin = TMath::Power(10., rymin);
      rymax = TMath::Power(10., rymax);
   }

   PadRangeAxisChanged(pad, rxmin, rxmax, rymin, rymax);
}

void TGo4ViewPanel::PadRangeAxisChanged(TPad* pad, double rxmin, double rxmax, double rymin, double rymax)
{
   TGo4LockGuard lock;

   if (IsRedrawBlocked() || (pad==0)) return;

   cout << "PadRangeAxisChanged pad = " << pad->GetName() << endl;

   TGo4Picture* padopt = GetPadOptions(pad);
   if (padopt==0) return;

   TakeSelectedAxisRange(0, padopt, rxmin, rxmax, false);
   TakeSelectedAxisRange(1, padopt, rymin, rymax, false);

   CallPanelFunc(panel_Updated, pad);
}

void TGo4ViewPanel::GetSelectedRange(int& ndim, bool& autoscale, double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
   ndim = 0;
   TGo4Picture* padopt = GetPadOptions(GetActivePad());
   if (padopt==0) return;

   ndim = padopt->GetFullRangeDim();
   autoscale = padopt->IsAutoScale();

   GetVisibleRange(GetActivePad(), 0, xmin, xmax);
   GetVisibleRange(GetActivePad(), 1, ymin, ymax);
   GetVisibleRange(GetActivePad(), 2, zmin, zmax);
}

void TGo4ViewPanel::SetAutoScale(bool on, TPad* selpad)
{
   TGo4LockGuard lock;

   bool modified = false;
   bool applytoall = false;

   if (selpad==0) {
      applytoall = IsApplyToAllFlag();
      selpad = applytoall ? GetCanvas() : GetActivePad();
   }
   if (selpad==0) return;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt!=0) {
      if (on!=padopt->IsAutoScale()) padopt->SetPadModified();
      padopt->SetAutoScale(on);
      modified = modified || padopt->IsPadModified();
   }

   if (applytoall) {
      TGo4Iter iter(GetPadSlot(selpad), kTRUE);
      while (iter.next()) {
         TPad* subpad = GetSlotPad(iter.getslot());
         padopt = GetPadOptions(subpad);
         if (padopt==0) continue;
         if (on!=padopt->IsAutoScale()) padopt->SetPadModified();
         padopt->SetAutoScale(on);
         modified = modified || padopt->IsPadModified();
      }
   }

   if (modified) RedrawPanel(selpad, false);
}

void TGo4ViewPanel::SetSelectedRange(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
   TGo4LockGuard lock;

   TPad* selpad = IsApplyToAllFlag() ? GetCanvas() : GetActivePad();
   if (selpad==0) return;

   TGo4Picture* padopt = GetPadOptions(selpad);
   if (padopt!=0) {
      Int_t ndim = padopt->GetFullRangeDim();

      TakeSelectedAxisRange(0, padopt, xmin, xmax, true);
      if (ndim>1) TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
      if (ndim>2) TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
      if (!padopt->IsAutoScale()) {
         if (ndim==1) TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
         if (ndim==2) TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
      }
      padopt->SetPadModified();
   }

   if (IsApplyToAllFlag()) {
      TGo4Iter iter(GetPadSlot(selpad), kTRUE);
      while (iter.next()) {
         TPad* subpad = GetSlotPad(iter.getslot());
         padopt = GetPadOptions(subpad);
         if (padopt==0) continue;
         Int_t ndim = padopt->GetFullRangeDim();
         TakeSelectedAxisRange(0, padopt, xmin, xmax, true);
         if (ndim>1) TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
         if (ndim>2) TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
         if (!padopt->IsAutoScale()) {
            if (ndim==1) TakeSelectedAxisRange(1, padopt, ymin, ymax, true);
            if (ndim==2) TakeSelectedAxisRange(2, padopt, zmin, zmax, true);
         }

         padopt->SetPadModified();
      }
   }

   RedrawPanel(selpad, false);
}

void TGo4ViewPanel::enterEvent( QEvent * e )
{
    QWidget::enterEvent(e);
//    if (ScanDrawOptions(GetActivePad(), GetPadOptions(GetActivePad()), true)) {
//      cout << "Options are changed" << endl;
//      CallPanelFunc(panel_Updated, GetActivePad());
//    }
}

void TGo4ViewPanel::leaveEvent( QEvent * e )
{
    #if ROOT_VERSION_CODE < ROOT_VERSION(4,3,1)

//    #if __GO4ROOTVERSION__ < 40301
    if(fxPeditor) fxPeditor->DeleteEditors();
    #endif
    QWidget::leaveEvent(e);
//    if (ScanDrawOptions(GetActivePad(), GetPadOptions(GetActivePad()), true)) {
//      cout << "Options are changed" << endl;
//       CallPanelFunc(panel_Updated, GetActivePad());
//    }
}

void TGo4ViewPanel::closeEvent( QCloseEvent* ce )
{
    ce->accept();
    delete this;
}

void TGo4ViewPanel::paintEvent(QPaintEvent* e)
{
   QWidget::paintEvent(e);
}

void TGo4ViewPanel::resizeEvent(QResizeEvent * e)
{
   QWidget::resizeEvent(e);
}

void TGo4ViewPanel::mouseReleaseEvent(QMouseEvent * e)
{
   CheckResizeFlags();
}

void TGo4ViewPanel::ResizeGedEditorSlot()
{
   TGo4LockGuard lock;

   ResizeGedEditor();
   fxGo4QRootCanvas->checkResizeFlag();
}

void TGo4ViewPanel::ResizeGedEditor()
{
   #ifdef __GO4GED__
   TGo4LockGuard lock;
   TGedEditor* ed = dynamic_cast<TGedEditor*> (fxPeditor);
   if ((ed!=0) && fbEditorFrameVisible)
      ed->Resize(fxRooteditor->width(),fxRooteditor->height());
   #endif
}

void TGo4ViewPanel::ActivateInGedEditor(TObject* obj)
{
   if (!fbEditorFrameVisible) return;
   #ifdef __GO4GED__
   TGedEditor* ed = dynamic_cast<TGedEditor*> (fxPeditor);
   if ((ed!=0) && (obj!=0))
      if (!obj->InheritsFrom(THStack::Class()) && !obj->InheritsFrom(TMultiGraph::Class())) {
         gTQSender = GetCanvas();
         ed->SetModel(GetActivePad(), obj, kButton1Down);
      }
   #endif
}

void TGo4ViewPanel::CleanupGedEditor()
{
   #ifdef __GO4GED__
//   cout << "TGo4ViewPanel::CleanupGedEditor()" << endl;
   TGedEditor* ed = dynamic_cast<TGedEditor*> (fxPeditor);
   if (ed==0) return;
   if (fDummyHisto==0) {
       fDummyHisto=new TH1I("dummyhisto","dummyhisto",100, -10., 10.);
       fDummyHisto->FillRandom("gaus",1000);
       fDummyHisto->SetDirectory(0);
       fDummyHisto->SetBit(kCanDelete, kFALSE);
   }
   gTQSender = GetCanvas();
   ed->SetModel(0, fDummyHisto, kButton1Down);
   ed->SetModel(0, GetCanvas(), kButton1Down);
   #endif
}

void TGo4ViewPanel::CheckResizeFlags()
{
   TGo4LockGuard lock;
   fxGo4QRootCanvas->checkResizeFlag();
   go4sett->storePanelSize(this);
}

void TGo4ViewPanel::ShootRepaintTimer()
{
   ShootRepaintTimer(GetCanvas());
}

void TGo4ViewPanel::ShootRepaintTimer(TPad* pad)
{
   if (IsRepaintTimerActive()) return;

   if (pad==0) pad = GetCanvas();

   fxRepaintTimerPad = pad;
   CallServiceFunc(service_PanelTimer);
}

bool TGo4ViewPanel::IsRepaintTimerActive()
{
   return fxRepaintTimerPad!=0;
}

void TGo4ViewPanel::checkRepaintSlot()
{
   TPad* pad = fxRepaintTimerPad;
   fxRepaintTimerPad = 0;
   CheckResizeFlags();
   if (pad!=0)
      RedrawPanel(pad, false);
}

void TGo4ViewPanel::PadDeleted(TPad* pad)
{
//   if (pad==GetActivePad())
//     SetActivePad(GetCanvas());

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot!=0) delete slot;

   GetCanvas()->SetSelectedPad(GetCanvas());
   GetCanvas()->SetSelected(GetCanvas());
   TGo4WorkSpace::Instance()->SetSelectedPad(GetCanvas());
}

// marker functions

int TGo4ViewPanel::GetNumMarkers(TPad* pad, int kind)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return 0;

   int res = 0;

   for (int n=0; n<slot->NumChilds();n++) {
     TGo4Slot* subslot = slot->GetChild(n);
     if (GetDrawKind(subslot)==kind) res++;
   }
   return res;
}

void TGo4ViewPanel::AddMarkerObj(TPad* pad, int kind, TObject* obj)
{
   TGo4Slot* padslot = GetPadSlot(pad);
   if (padslot==0) { delete obj; return; }

   QString basename;
   switch(kind) {
     case kind_Marker: basename = "Marker"; break;
     case kind_Window: basename = "Window"; break;
     case kind_Poly: basename = "Polygon"; break;
     case kind_Latex: basename = "Latex"; break;
     case kind_Arrow: basename = "Arrow"; break;
     default: basename = "Something"; break;
   }

   QString slotname;
   int cnt = 0;
   do {
     slotname = basename + QString::number(cnt++);
   } while (padslot->FindChild(slotname.latin1())!=0);

   TGo4Slot* objslot = AddDrawObject(pad, kind, slotname.latin1(), obj, true, 0);

   SetActiveObj(pad, kind, objslot);
}

void TGo4ViewPanel::SetActiveObj(TPad* pad, int kind, TGo4Slot* activeslot)
{
   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return;

   for (int n=0; n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind!=kind) continue;

      if (subslot!=activeslot)
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
   if ((kind==selkind) && (selobj!=0)) return selobj;

   TGo4Slot* slot = GetPadSlot(pad);
   if (slot==0) return 0;

   TObject* lastobj = 0;

   for (int n=0; n<slot->NumChilds();n++) {
      TGo4Slot* subslot = slot->GetChild(n);
      int drawkind = GetDrawKind(subslot);
      if (drawkind!=kind) continue;

      lastobj = subslot->GetAssignedObject();
      if (subslot->GetPar("::ActiveMarker")!=0)
        return lastobj;
   }

   return lastobj;
}

QString TGo4ViewPanel::GetActiveObjName(TPad* pad, int kind)
{
   TObject* obj = GetActiveObj(pad, kind);
   return (obj==0) ? QString("null") : QString(obj->GetName());
}

void TGo4ViewPanel::OptionsMenuItemActivated(int id)
{
   TGo4LockGuard lock;

   switch (id) {
      case CrosshairId: {
         fbCanvasCrosshair = !fMenuBar->isItemChecked(CrosshairId);
         GetCanvas()->SetCrosshair(fbCanvasCrosshair);

         #if ROOT_VERSION_CODE < ROOT_VERSION(4,0,8)

//         #if __GO4ROOTVERSION__ < 40008
           TGo4Iter(GetPadSlot(GetCanvas()), true);
           while (iter.next()) {
             TPad* subpad = GetSlotPad(iter.getslot());
             if (subpad!=0) {
                subpad->SetCrosshair(fbCanvasCrosshair);
                subpad->Modified();
             }
          }
         #endif

         GetCanvas()->Modified();
         GetCanvas()->Update();
         CallPanelFunc(panel_Updated, GetCanvas());
         break;
      }

      case FreezeTitleId: {
         fbFreezeTitle = !fMenuBar->isItemChecked(FreezeTitleId);
         break;
      }

      case SetTitleTextId:  {
         bool ok = false;
         QString mycaption = caption();
         QString oldtitle = mycaption.remove(fPanelName + ": ");
         QString text = QInputDialog::getText(
                          GetPanelName(), "Enter Viewpanel Title:", QLineEdit::Normal,
                         oldtitle, &ok, this );
         if ( ok && !text.isEmpty() ) {
            QString mycaption = GetPanelName();
            mycaption += ": ";
            mycaption += text;
            setCaption(mycaption);
            fbFreezeTitle = true;
         }
         break;
      }

      default:
        if (id>1000) {
           bool s = !fMenuBar->isItemChecked(id);
           ChangeDrawOption(id-1000, s, 0);
        }
   }
}


void TGo4ViewPanel::ApplyToAllToggled(bool on)
{
//   if (IsRedrawBlocked()) return;

   fbApplyToAllFlag = on;
}

void TGo4ViewPanel::AutoScaleToggled(bool on)
{
   if (IsRedrawBlocked()) return;

   SetAutoScale(on, 0);
}

void TGo4ViewPanel::panelSlot(TGo4ViewPanel * panel, TPad * pad, int signalid)
{
   if (panel!=this) return;

   BlockPanelRedraw(true);

   TGo4Picture* padopt = GetPadOptions(GetActivePad());
   if (padopt!=0)
     fAutoScaleCheck->setChecked(padopt->IsAutoScale());

   BlockPanelRedraw(false);
}

void TGo4ViewPanel::SuperImposeToggled(bool on)
{
   if (IsRedrawBlocked()) return;

   ChangeDrawOption(11, on, "");
}
