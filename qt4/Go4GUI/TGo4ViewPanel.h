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

#ifndef TGO4VIEWPANEL_H
#define TGO4VIEWPANEL_H

#include "QGo4Widget.h"
#include "ui_TGo4ViewPanel.h"


class THStack;
class TObject;
class TCanvas;
class TPad;
class TH1;
class TH2;
class TAxis;
class TGraph;
class TCutG;
class TMultiGraph;
class TGo4ASImage;
class TGo4Picture;
class QRootWindow;
class QRootCanvas;
class TGo4Slot;
class TGo4Picture;
class TObjArray;
class TVirtualPadEditor;
class QMenuBar;
class QStatusBar;
class QAction;


class TGo4ViewPanel : public QGo4Widget, public Ui::TGo4ViewPanel
{
   Q_OBJECT

   public:

      enum Go4ViewPanelMenuesId {
         CrosshairId      = 302,
         FreezeTitleId    = 309,
         SetTitleTextId   = 310,
         SuperimposeId    = 1011,
         StatisticsId     = 1006,
         SetTitleId       = 1007,
         SetLegendId      = 1008,
         DrawTimeId       = 1012,
         DrawDateId       = 1013,
         DrawItemnameId   = 1014,
         AxisTimeDisplayId= 1015,
         SetTimeFormatId  = 1016,
         SetXYRatioOneId  = 1017,
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

      enum {kind_None = -1,
         kind_Link = 1,
         knd_Reference = 2,
         kind_Additional = 100,
         kind_FitModels = 101,
         kind_Specials = 200,
         kind_Condition = 201,
         kind_Marker = 202,
         kind_Window = 203,
         kind_Poly = 204,
         kind_Latex = 205,
         kind_Arrow = 206,
         kind_Func = 207,
         kind_FitInfo = 221,
         kind_FitArrows = 222,
         kind_Other = 300,
         kind_Fitter = 301,
         kind_Permanet = 1000,
         kind_PadOptions = 1001,
         kind_ThisPad = 1002,
         kind_PadSlot = 1003 };

      TGo4ViewPanel(QWidget *parent = 0, const char* name=0);

      virtual ~TGo4ViewPanel();

      virtual const char* GetPanelName();
      virtual void SetPanelName(const char* newname);
      virtual TGo4Slot* GetPanelSlot();
      virtual TGo4Slot* AddNewSlot(const char* name, TGo4Slot* parent);
      virtual void BlockPanelRedraw(bool on);
      virtual void ResetWidget();
      virtual void linkedObjectUpdated(const char* linkname, TObject* obj);
      virtual void linkedUpdated(TGo4Slot* slot, TObject* obj);
      virtual void linkedRemoved(TGo4Slot* slot, TObject* obj);
      virtual bool IsAcceptDrag( const char * itemname, TClass * cl, int kind );
      virtual void DropOnPad(TPad* pad, const char * itemname, TClass * cl, int kind );
      virtual void CompleteInitialization();
      virtual void SetMouseMode( int mode );
      virtual int GetMouseMode();
      virtual QString GetSelectedMarkerName( TPad * pad );
      virtual int GetSelectedMarkerIndex( TPad * pad );
      virtual TGo4Slot * GetSelectedSlot( TPad * pad, int * selkind, TObject * * selobj );
      virtual bool IsConditionSelected( TPad * pad );
      virtual TPad* FindPadWithItem(const char* itemname);
      virtual void UndrawItemOnPanel(const char* itemname);
      virtual const char* GetDrawItemName(int itemcnt = 0);
      virtual void SetSelectedMarker( TPad * pad, const QString & selname, int selindex );
      virtual void SetSelectedMarkerByMouseClick(TPad* pad, const char* name);
      virtual void StartConditionEditing(TPad* pad);
      virtual void SwitchMarkerButton( int kind, bool on );
      virtual void MakePictureForPad(TGo4Picture* pic, TPad* pad, bool useitemname);
      virtual void UpdatePadStatus( TPad * pad, bool removeitems );
      virtual void ProcessPadStatusUpdate( TPad * pad, TGo4Slot* parent, bool removeitems );
      virtual TGo4Slot* AddDrawObject( TPad * pad, int kind, const char * itemname, TObject * obj, bool owner, const char * drawopt );
      virtual TGo4Slot* GetDrawObjectSlot( TPad * pad, const char * name );
      virtual TObject* GetDrawObject( TPad * pad, const char * name );
      virtual void DeleteDrawObject( TPad * pad, const char * name );
      virtual void DeleteDrawObject( TPad * pad, TObject * obj );
      virtual void CollectSpecialObjects( TPad * pad, TObjArray * objs, int selectkind );
      virtual bool DeleteDrawObjects( TPad * pad, int kindtodelete );
      virtual void SetPadSuperImpose( TPad * pad, bool on );
      virtual bool ScanDrawOptions(TPad* pad, TGo4Slot* padslot, TGo4Picture* pic, bool onlyscan);
      virtual void ScanObjectsDrawOptions(bool onlyscan, TGo4Slot* padslot, TObjArray* objs, TObjArray* objslots);
      virtual void CollectMainDrawObjects( TGo4Slot * slot, TObjArray* objs, TObjArray* objslots, int modifier);
      virtual TObject * ProduceSuperimposeObject(TGo4Slot* padslot, TGo4Picture* padopt, TGo4Slot * sislot, TGo4Slot * legslot,
                                          TObjArray * objs, TObjArray * objslots, bool showitems );
      virtual void Divide( int numX, int numY );
      virtual void SetSlotPad(TGo4Slot* padslot, TPad * pad);
      virtual TGo4Slot* GetPadSlot( TPad * pad );
      virtual TPad * GetSlotPad( TGo4Slot * slot );
      virtual bool IsPadHasSubPads( TGo4Slot * padslot );
      virtual bool IsPadHasSubPads( TPad * pad );
      virtual void MarkPadModified( TPad * pad );
      virtual bool IsPanelPad( TPad * pad );
      virtual int GetNumberOfPads( TPad * toppad );
      virtual TPad * GetSubPad( TPad * toppad, int num, bool onlytoplevel );
      virtual void ProducePadsList( TObjArray * arr, TPad * toppad );
      virtual const char* GetDrawObjectLinkName( TPad * pad, TObject* obj );
      virtual TGo4Slot * GetPadMainObjectSlot( TPad * pad );
      virtual TGo4Slot * GetSuperimposeSlot(TGo4Slot* slot);
      virtual TObject * GetPadMainObject( TPad * pad );
      virtual TCanvas * GetCanvas();
      virtual QRootCanvas * GetQCanvas();
      virtual TPad * GetActivePad();
      virtual void AllocatePadOptions(TPad* pad);
      virtual TGo4Picture * GetPadOptions( TPad * pad );
      virtual TGo4Picture * GetPadOptions( TGo4Slot * padslot );
      virtual TH1* GetPadHistogram( TPad * pad );
      virtual bool IsRedrawBlocked();
      virtual void UpdatePanelCaption();
      virtual void SetDrawKind( TGo4Slot * slot, int kind );
      virtual void SetSpecialDrawOption( TGo4Slot * slot, const char * drawopt );
      virtual int GetDrawKind( TGo4Slot * slot );
      virtual void SetSelectedObjectIndex( TGo4Slot * slot, int indx );
      virtual int GetSelectedObjectIndex(TGo4Slot* slot);
      virtual TObject* GetSelectedObject( TPad * pad, const char** drawopt );
      virtual bool ShiftSelectedObjectToEnd(TPad * pad);
      virtual const char * GetSpecialDrawOption( TGo4Slot * slot );
      virtual void CheckForSpecialObjects( TPad * pad, TGo4Slot * padslot );
      virtual void CheckObjectsAssigments( TPad * pad, TGo4Slot * padslot );
      virtual void ProcessPictureRedraw( const char* picitemname, TPad * pad, TGo4Picture * pic );
      virtual void ProcessCanvasAdopt( TPad * tgtpad, TPad * srcpad, const char* srcitemname);
      virtual bool ProcessPadRedraw( TPad * pad, bool force);
      virtual void RedrawHistogram( TPad * pad, TGo4Picture * padopt, TH1 * his, bool scancontent, bool first_draw );
      virtual void RedrawStack( TPad * pad, TGo4Picture * padopt, THStack * hs, bool dosuperimpose, bool scancontent );
      virtual void RedrawGraph( TPad * pad, TGo4Picture * padopt, TGraph * gr, bool scancontent, bool first_draw );
      virtual void RedrawMultiGraph( TPad * pad, TGo4Picture * padopt, TMultiGraph * mg, bool dosuperimpose, bool scancontent );
      virtual void RedrawImage( TPad * pad, TGo4Picture * padopt, TGo4ASImage * im, TH2 * asihisto, bool scancontent );
      virtual void RedrawLegend( TPad * pad, TGo4Picture * padopt, TGo4Slot * legslot );
      virtual void RedrawSpecialObjects( TPad * pad, TGo4Slot* padslot);
      virtual bool IsApplyToAllFlag();
      virtual bool IsFreezeTitle();
      virtual void ChangeDrawOptionForPad(TGo4Slot* padslot, int kind, int value, const char* drawopt );
      virtual void ChangeDrawOption( int kind, int value, const char* drawopt );
      virtual void ResetPadFillColors( TPad * pad, int col , TPad* backup=0);
      virtual void ClearPad( TPad * pad, bool removeitems, bool removesubpads );
      virtual void ClearPadItems( TGo4Slot * padslot, TGo4Slot* remain );
      virtual void ProcessPadClear( TPad * pad, bool removeitems, bool removesubpads );
      virtual void ProcessMarkersClear(TPad *pad, bool withsubpads);
      virtual void SetPadDefaults( TPad * pad );
      virtual void DisplayPadStatus( TPad * pad );
      virtual void MoveScale( int expandfactor, int xaction, int yaction, int zaction );
      virtual void MoveSingleScale( int expandfactor, int action, int naxis, TGo4Picture* opt, TObject* padhist);
      virtual void TakeFullRangeFromHisto( TH1 * h1, TGo4Picture * padopt, bool isfirsthisto);
      virtual void TakeFullRangeFromGraph( TGraph * gr, TGo4Picture * padopt, bool isfirst);
      virtual void SetSelectedRangeToHisto( TPad* pad, TH1 * h1, THStack* hs, TGo4Picture * padopt, bool ishisto );
      virtual bool GetVisibleRange(TPad* pad, int naxis, double & min, double & max);
      virtual void PadRangeAxisChanged( TPad * pad );
      virtual void PadRangeAxisChanged( TPad * pad, double rxmin, double rxmax, double rymin, double rymax );
      virtual bool TakeSelectedAxisRange(int naxis, TGo4Picture* padopt, double selmin, double selmax, bool force);
      virtual bool TakeSelectedAxisRange(int naxis, TGo4Picture* padopt, TAxis* ax);
      virtual void GetSelectedRange(int & ndim, bool & autoscale,
                                    double & xmin, double & xmax,
                                    double & ymin, double & ymax,
                                    double & zmin, double & zmax);
      virtual void SetSelectedRange(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax );
      virtual void SetAutoScale(bool on, TPad* selpad);
      virtual void ResizeGedEditor();
      virtual void ActivateInGedEditor(TObject* obj);
      virtual void CleanupGedEditor();
      virtual void ShootRepaintTimer();
      virtual bool IsRepaintTimerActive();
      virtual void ShootRepaintTimer(TPad * pad);
      virtual void PadDeleted( TPad * pad );
      virtual void SetFreezedTitle(const QString& title);
      virtual void RectangularRatio(TPad* pad);
      virtual void DefaultPadMargin(TPad* pad);

   public slots:

      virtual void SetCursorMode( bool on );
      virtual void SetRegionMode( bool on );
      virtual void SetPolygonMode( bool on );
      virtual void SetLateXMode( bool on );
      virtual void SetDrawingMode( bool on );
      virtual void SetFreezeMouseMode( bool on );
      virtual void RefreshButtons();
      virtual void SelectedMarkerCmb_activated( int indx );
      virtual void DelSelectedMarker_clicked();
      virtual void SetMarkerPanel();
      virtual void LogMarkerValues();
      virtual void ClearAllMarkers();
      virtual void GetConditionBtn_clicked();
      virtual void InfoConditionBtn_clicked();
      virtual void EditConditionBtn_clicked();
      virtual void SetConditionBtn_clicked();
      virtual void SaveMarkers();
      virtual void LoadMarkers();
      virtual void SetActivePad( TPad * pad );
      virtual void PadClickedSlot( TPad * pad );
      virtual void PadDoubleClickedSlot( TPad * pad );
      virtual void CanvasDropEventSlot( QDropEvent * event, TPad * pad );
      virtual void CanvasStatusEventSlot( const char * message );
      virtual void ProcessPadDoubleClick();
      virtual void MenuCommandExecutedSlot( TObject * obj, const char * cmdname );
      virtual void CanvasUpdatedSlot();
      virtual void SaveCanvas();
      virtual void ProducePicture();
      virtual void ProduceGraphFromMarkers();
      virtual void PrintCanvas();
      virtual void StartRootEditor();
      virtual void StartConditionEditor();
      virtual void ClearActivePad();
      virtual void ClearCanvas();
      virtual void AboutToShowOptionsMenu();
      virtual void ShowEventStatus();
      virtual void ProcessPadModifiedSignal();
      virtual void RedrawPanel( TPad * pad, bool force );
      virtual void checkRepaintSlot();
      virtual void checkResizeSlot();
      virtual void OptionsMenuItemActivated(int);
      virtual void SelectMenuItemActivated(int);
      virtual void ApplyToAllToggled(bool);
      virtual void AutoScaleToggled(bool);
      virtual void SuperImposeToggled(bool);
      virtual void panelSlot(TGo4ViewPanel * panel, TPad * pad, int signalid);

   protected:

      QString fPanelName;
      QByteArray fbaPanelName;
      bool fbEditorFrameVisible;
      TVirtualPadEditor* fxPeditor;
      QRootWindow* fxRooteditor;
      bool fbFreezeTitle;
      QString fFreezedTitle;
      bool fbMarkEditorVisible;
      bool fbTypingMode;
      QMenuBar* fMenuBar;
      TPad* fxActivePad;
      int fiSkipRedrawCounter;
      bool fbCanvasCrosshair;
      bool fbCanvasEventstatus;
      QStatusBar *CanvasStatus;
      bool fbApplyToAllFlag;
      TPad* fxRepaintTimerPad;
      TPad* fxResizeTimerPad;
      TPad* fxDoubleClickTimerPad;
      int fiMouseMode;
      int fiPickCounter;
      bool fbPickAgain;
      bool fbCloneFlag;
      bool fbModifiedSignalFlag;
      bool fbLeaveFocusAfterCondEnd;
      TH1* fDummyHisto;
      QMenu* fSelectMenu;
      QSignalMapper* fSelectMap;
      QCheckBox* fAutoScaleCheck;
      QSignalMapper* fOptionsMap;
      QMenu* fOptionsMenu;

      virtual void CheckActionAtTheEnd( TPad * pad );
      virtual bool CompleteMarkerEdit( TPad * pad );
      virtual void resizeEvent( QResizeEvent * e );
      virtual int GetNumMarkers( TPad * pad, int kind );
      virtual void AddMarkerObj( TPad * pad, int kind, TObject * obj );
      virtual void SetActiveObj( TPad * pad, int kind, TGo4Slot * activeslot );
      virtual TObject * GetActiveObj( TPad * pad, int kind );
      virtual QString GetActiveObjName( TPad * pad, int kind );
      virtual int GetAutoColor(int indx);

};

#endif
