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

#ifndef TGO4FITPANEL_H
#define TGO4FITPANEL_H

#include "QGo4Widget.h"
#include "ui_TGo4FitPanel.h"
#include "TArrow.h"


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QWidgetStack;
class QWidget;
class QSplitter;
class QPushButton;
class QSpinBox;
class QLabel;
class QTableWidget;
class QCheckBox;
class QComboBox;
class QTabWidget;
class QGo4LineEdit;
class QSlider;
class QFrame;
class QLineEdit;
class TGo4ViewPanel;
class TPad;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QMenuBar;
class QFitItem;
class QFitNamedWidget;
class QFitModelWidget;
class TObject;
class TObjArray;
class TGo4Slot;
class TGo4FitSlot;
class TGo4FitComponent;
class TGo4FitModel;
class TGo4FitData;
class TGo4Fitter;

class TGo4Fitter;
class TGo4FitGuiArrow;
class TGo4FitPeakFinder;

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




class TGo4FitPanel : public QGo4Widget, public Ui::TGo4FitPanel
 {
    Q_OBJECT

public:
    TGo4FitPanel( QWidget* parent = 0, const char* name = 0 );
    virtual ~TGo4FitPanel();

    virtual bool IsAcceptDrag(const char* itemname, TClass * cl, int kind );
    virtual void DropOnPanel( QDropEvent* event, const char * itemname, TClass * cl, int kind );
    virtual void linkedObjectUpdated( const char * linkname, TObject * obj );
    virtual void linkedObjectRemoved( const char * linkname );
    virtual void linkedRemoved(TGo4Slot* slot, TObject* obj);
    virtual void CreateFitSlotLink(TGo4FitSlot* slot, const char * itemname);
    virtual void WorkWithFitter(const char* itemname, TGo4ViewPanel* panel, TPad* pad);
    virtual void WorkWithFitterInSlot(TGo4Slot* slot);
    virtual bool WorkingWithPanel();
    virtual bool WorkingWithOnlyPad();
    virtual TGo4ViewPanel* ActivePanel();
    virtual TPad* ActivePad();
    virtual TGo4Fitter* GetFitter();
    virtual TGo4Fitter* CloneFitter();
    virtual void RemoveFitterLink();
    virtual void SetFitter(TGo4Fitter*);
    virtual void Fitter_New();
    virtual void Fitter_NewForActivePad(bool overwrite);
    virtual void Fitter_Delete();
    virtual void Fitter_UseWorkspace();
    virtual void Fitter_UpdateReferences();
    virtual void Fitter_SaveToBrowser();
    virtual void Fitter_PrintParameters();
    virtual void Fitter_RollbackParameters();
    virtual void Button_WorkWithPanel();
    virtual void Button_SimpleFit(int nmodel);
    virtual void Button_SimpleClear();
    virtual void Button_PeakFinder();
    virtual void Button_PerformFit();
    virtual void Button_FitterDraw(TGo4FitData* selecteddata);
    virtual void Cmd_CreateFitter();
    virtual void Cmd_CreateAppropriateFitter();
    virtual void Cmd_DeleteFitter();
    virtual void Cmd_ClearFitter();
    virtual void Cmd_SaveFitter(bool ask);
    virtual void Cmd_ItemPrint(QFitItem* item);
    virtual void Cmd_AddNewData(QFitItem * item, int id);
    virtual void Cmd_DrawData(QFitItem * item);
    virtual void Cmd_DeleteData(QFitItem * item);
    virtual void Cmd_DeleteAssosiatedModels(QFitItem * item);
    virtual void Cmd_DeleteAllData(QFitItem * item);
    virtual void Cmd_AddNewModel(QFitItem * item, int id);
    virtual void Cmd_DeleteModel(QFitItem * item);
    virtual void Cmd_RemoveModel(TGo4FitModel* model);
    virtual void Cmd_DeleteModels(QFitItem * item);
    virtual void Cmd_CloneModel(QFitItem * item);
    virtual void Cmd_ClearAssigment(QFitItem* item);
    virtual void Cmd_ClearAssigments(QFitItem* item);
    virtual void Cmd_AssignModelToAllData(QFitItem* item);
    virtual void Cmd_AssignModelToData(QFitItem* item, int id);
    virtual void Cmd_RemoveRangeCondition(QFitItem* item);
    virtual void Cmd_RemoveRangeConditions(QFitItem* item);
    virtual void Cmd_AddRangeCondition(QFitItem* item, int id);
    virtual void Cmd_DeleteAction(QFitItem* item);
    virtual void Cmd_DeleteActions(QFitItem* item);
    virtual void Cmd_MoveAction(QFitItem* item, int dir);
    virtual void Cmd_ExecuteAction(QFitItem* item);
    virtual void Cmd_ExecuteActions(QFitItem* item, bool expert);
    virtual void Cmd_DeleteOutputActions(QFitItem* item);
    virtual void Cmd_AddNewAction(QFitItem* item, int id);
    virtual void Cmd_DeleteDependency(QFitItem* item);
    virtual void Cmd_DeleteDependencies(QFitItem* item);
    virtual void Cmd_AddDependency(QFitItem* item);
    virtual void Cmd_MemorizePars(QFitItem* item);
    virtual void Cmd_RememberPars(QFitItem* item);
    virtual void Cmd_DeletePars(QFitItem* item);
    virtual void Cmd_AddNewPar(QFitItem* item);
    virtual void Cmd_MemorizePar(QFitItem* item);
    virtual void Cmd_RememberPar(QFitItem* item);
    virtual void Cmd_DeletePar(QFitItem* item);
    virtual void Cmd_DeleteMinuitResult(QFitItem* item);
    virtual void Cmd_UpdateAllSlots(QFitItem* item);
    virtual void UpdateActivePage();
    virtual void UpdateSimplePage();
    virtual void UpdateWizardPage();
    virtual void UpdateExtendedPage();
    virtual void RemovePrimitives();
    virtual TGo4Fitter* CreateFitterFor(TGo4ViewPanel* panel, TPad* pad, const char* name);
    virtual void CreateDataFor(TGo4ViewPanel* panel, TPad* pad, TGo4Fitter* fitter);
    virtual bool FillPopupForItem(QFitItem* item, QMenu* menu, QSignalMapper* map);
    virtual bool FillPopupForSlot(TGo4FitSlot* slot, QMenu* menu, QSignalMapper* map);
    virtual void ExecutePopupForSlot(QFitItem* item, TGo4FitSlot* slot, int id);
    virtual QFitItem* GetFitterItem();
    virtual QFitItem* FindItem(TObject* obj, int ObjectType, QFitItem* parent);
    virtual bool ShowItem(QFitItem* item, bool force);
    virtual bool ShowItemAsText(QFitItem* item, bool force);
    virtual bool ShowItemAsGraph(QFitItem* item, bool force);
    virtual void RemoveItemWidget();
    virtual void UpdateItem(QFitItem* item, bool trace);
    virtual void SetItemText(QFitItem* item, bool trace);
    virtual void UpdateItemsOfType(int typ, QFitItem* parent);
    virtual void FitItemDeleted(QFitItem* item);
    virtual void FillModelTypesList(QMenu* menu, QSignalMapper* map, int id, bool extend);
    virtual void FillDataTypesList(QMenu* menu, QSignalMapper* map, int id);
    virtual void FillParsList(QFitItem* item);
    virtual void FillSlotsList(QFitItem* parent, const TObjArray* lst, TObject* owner);
    virtual void FillDependencyList(QFitItem* parent);
    virtual void PaintFitter(TGo4Fitter* fitter, QFitItem* item, bool update);
    virtual bool PaintModel(TGo4FitModel* model, TPad* pad, QFitItem* item);
    virtual bool PaintModelsFor(TGo4Fitter* fitter, TGo4FitData* data, QFitItem* item, bool update);
    virtual bool PaintRange(TGo4FitComponent* comp, int nrange, TPad* pad, QFitItem* item);
    virtual TGo4FitPeakFinder* GetPeakFinder(bool autocreate);
    virtual TGo4FitData* Wiz_SelectedData();
    virtual TGo4FitModel* Wiz_SelectedModel();
    virtual TGo4FitModel* Wiz_CreateNewModel(int id);
    virtual TGo4FitData* Wiz_CreateNewData(int id);
    virtual void Wiz_RebuildDataList();
    virtual bool Wiz_RemoveData();
    virtual void Wiz_UseSelectedRange();
    virtual void Wiz_TakeCurrentRange();
    virtual void Wiz_GetModelInfo(TGo4FitModel* model, QString* info);
    virtual QString Wiz_GetSlotSourceInfo(TGo4FitSlot* slot);
    virtual void ArrowChanged(TGo4FitGuiArrow* arr);
    virtual void DeleteModelWithPrimit(TGo4FitGuiArrow* arr);
    virtual int GetPadIndexForSlot(TGo4FitSlot* slot);
    virtual TPad* FindPadForSlot(TGo4FitSlot* slot);
    virtual TPad* FindPadWhereData(TGo4FitData* data);
    virtual TPad* FindPadWhereModel(TGo4FitModel* model);
    virtual TPad* FindPadWhereComp(TGo4FitComponent* comp);
    virtual TGo4FitData* CreateData(int id, const char* name);
    virtual TGo4FitModel* CreateModel(int id, const char* namebase, TGo4Fitter* fitter, TGo4FitData* data);
    virtual void LocateModel(TGo4FitModel* model, TGo4FitData* data, bool usepad);
    virtual int DefineModelWidgetType(TObject* obj);
    virtual bool UpdateObjectReferenceInSlot(TGo4FitSlot* slot, bool createlink);
    virtual bool UpdateObjectReferenceInSlots();
    virtual void ClearObjectReferenceInSlots();
    virtual void RemoveDrawObjects();
    virtual void CloseDrawPanel();
    virtual void UpdateStatusBar(const char* info);
    virtual void UpdateItemMenu();
    virtual void UpdatePFAmplLbl();
    virtual void UpdatePFRelNoiseLbl();
    virtual void UpdateWizDataList();
    virtual void UpdateWizDataBtns();
    virtual void UpdateWizModelsList(bool changestack);
    virtual void UpdateWizModelsBtns();
    virtual void UpdateWizStackWidget();
    virtual void UpdateWizPaint(int mode);
    virtual void FillParsTable( QTableWidget* table, TGo4Fitter * fitter, TGo4FitModel * model, bool LinesView, TObjArray
 * TableList );
    virtual void FillNamedWidget(QFitNamedWidget* w);
    virtual void ChangeObjectName(QFitNamedWidget* w, const char* newname);
    virtual void ChangeObjectTitle(QFitNamedWidget* w, const char* newtitle);
    virtual void ChangeModelPar(QFitModelWidget* w, int npar, int value);


public slots:
    virtual void panelSlot(TGo4ViewPanel*, TPad*, int);
    virtual void FitList_customContextMenuRequested(const QPoint &);
    virtual void FitList_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);
    virtual void AboutToShowViewMenu();
    virtual void ChangeViewType(int id);
    virtual void ShowPanelPage(int id);
    virtual void AboutToShowSettMenu();
    virtual void ChangeSettings( int id );
    virtual void AboutToShowFitterMenu();
    virtual void FitterMenuItemSelected( int id );
    virtual void AboutToShowItemMenu();
    virtual void ItemMenuItemSelected( int id );
    virtual void PF_MinWidthEdt_textChanged( const QString & str );
    virtual void PF_MaxWidthEdt_textChanged( const QString & str );
    virtual void PF_WidthEdit_textChanged( const QString & str );
    virtual void PF_AmplSlider_valueChanged( int zn );
    virtual void PF_AmplSlider_sliderReleased();
    virtual void PF_RelNoiseSlider_valueChanged( int zn );
    virtual void PF_RelNoiseSlider_sliderReleased();
    virtual void PF_MinNoiseEdit_textChanged( const QString & str );
    virtual void PF_SumUpSpin_valueChanged( int num );
    virtual void Wiz_DataListSelect(QListWidgetItem*);
    virtual void Wiz_ModelListSelect(QListWidgetItem*);
    virtual void Wiz_AddDataBtn_clicked();
    virtual void Wiz_DelDataBtn_clicked();
    virtual void Wiz_AddModelBtn_clicked();
    virtual void Wiz_DelModelBtn_clicked();
    virtual void Wiz_CloneModelBtn_clicked();
    virtual void Wiz_ShowAllMod_toggled( bool zn );
    virtual void Wiz_FitFuncCmb_activated( int typ );
    virtual void Wiz_FitNameEdt_textChanged( const QString & name );
    virtual void Wiz_ParTable_valueChanged( int nrow, int ncol );
    virtual void Wiz_DataList_doubleClicked(QListWidgetItem*);
    virtual void Wiz_ModelList_doubleClicked(QListWidgetItem*);
    virtual void Wiz_RebuildDataBtn_clicked();
    virtual void Wiz_DataBufChk_toggled( bool zn );
    virtual void Wiz_UseAmplEstimChk_toggled( bool chk );
    virtual void Wiz_MigradIterSpin_valueChanged( int num );
    virtual void Wiz_DataSlotsTable_contextMenuRequested(const QPoint & pnt );
    virtual void Wiz_DataUseRangeBtn_clicked();
    virtual void Wiz_DataClearRangesBtn_clicked();
    virtual void Wiz_DrawDataBtn_clicked();
    virtual void Wiz_ModelBufChk_toggled( bool zn );
    virtual void Wiz_PFSetupBtn_clicked();
    virtual void Wiz_PFUsePolynChk_toggled( bool zn );
    virtual void Wiz_PFPolynSpin_valueChanged( int zn );
    virtual void Wiz_MinSetupBtn_clicked();
    virtual void MainAttachBtn_clicked();
    virtual void MainFitBtn_clicked();
    virtual void MainDrawBtn_clicked();
    virtual void MainFindBtn_clicked();
    virtual void MainParsBtn_clicked();
    virtual void FillParsWidget();
    virtual void LineParsChk_toggled( bool );
    virtual void ParsTable_valueChanged( int nrow, int ncol );
    virtual void ParsTableChanged( QTableWidget * table, int nrow, int ncol,
                                    bool LinesView, TObjArray * TableList,
                                    bool updatepaint );
    virtual void Smp_PolynomBtn_clicked();
    virtual void Smp_GaussianBtn_clicked();
    virtual void Smp_LorenzBtn_clicked();
    virtual void Smp_ExponentBtn_clicked();
    virtual void Smp_ClearBtn_clicked();
    virtual void FindersTab_currentChanged( int);
    virtual void Wiz_BackgroundChk_toggled( bool chk );
    virtual void PF_MinWidthEdt_returnPressed();
    virtual void PF_MaxWidthEdt_returnPressed();
    virtual void PF_WidthEdit_returnPressed();
    virtual void PF_MinNoiseEdit_returnPressed();

public:
   QFitItem* CurrFitItem;

protected:

    QMenuBar* MenuBar;
    QMenu* FitterMenu;
    QSignalMapper* FitterMap;
    QMenu* ViewMenu;
    QSignalMapper* ViewMap;
    QMenu* SettMenu;
    QSignalMapper* SettMap;
    QMenu* ItemMenu;
    QSignalMapper* ItemMap;


    bool fbFillingWidget;
    int fiPaintMode;
    TObjArray* fxWizPars;
    TObjArray* fxWizSlots;
    TObjArray* fxParsTableList;
    TGo4ViewPanel* fxActivePanel;
    TPad* fxActivePad;
    TGo4ViewPanel* fxDrawNewPanel;
    bool fbDrawPanelCreation;
    QFitItem* fxCurrentItem;
    QWidget* fxCurrentItemWidget;
    bool fbNeedConfirmation;
    bool fbShowPrimitives;
    bool fbFreezeMode;
    int fiPanelMode;
    bool fbParsWidgetShown;
    bool fbDrawModels;
    bool fbDrawComponents;
    bool fbDrawBackground;
    bool fbUseSamePanelForDraw;
    bool fbDrawInfoOnPad;
    int fiIntegralMode;
    bool fbRecalculateGaussWidth;
    int fiBuffersUsage;
    bool fbSaveWithReferences;
    bool fbUseCurrentRange;
    QString fxWizDataName;
    QString fxWizModelName;
    bool fbWizShowAllModels;
    int fiWizPageIndex;
    bool fbUseAmplEstim;
    int fiNumMigradIter;

    virtual void focusInEvent( QFocusEvent * event );
    virtual void windowActivationChange( bool OldActive );



};

#endif // TGO4FITPANEL_H





