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

#ifndef TGO4BROWSERPROXY_H
#define TGO4BROWSERPROXY_H

#include "TGo4Proxy.h"

#include "TString.h"

class TList;
class TTimer;
class TAxis;
class TGraph;
class TH1;

class TGo4ObjectManager;
class TGo4AnalysisProxy;
class TGo4ServerProxy;
class TGo4Picture;
class TVirtualTreePlayer;

class TGo4BrowserProxy : public TGo4Proxy {
   public:
      TGo4BrowserProxy() {}
      TGo4BrowserProxy(const char *datapath,
                       const char *viewpath,
                       Bool_t withRootBrowser = kFALSE);
      virtual ~TGo4BrowserProxy();

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;

      Bool_t Use() const override { return kFALSE; }

      Bool_t ProcessEvent(TGo4Slot *slot, TGo4Slot *source, Int_t id, void *param) override;
      Int_t GetObjectKind() const override { return TGo4Access::kndFolder; }
      const char *GetContainedClassName() const override { return ClassName(); }
      TObject *GetAssignedObject() override { return this; }

      // browser functionality

      void DataSlotName(const char *item, TString &res) const;
      void BrowserSlotName(const char *item, TString &res) const;
      TGo4Slot *BrowserSlot(const char *item = nullptr);
      TGo4Slot *DataSlot(const char *item);
      TGo4Slot *BrowserMemorySlot();
      Bool_t BrowserItemName(TGo4Slot *itemslot, TString &res);

      void UpdateBrowserContent();
      void InformBrowserUpdate();
      void SetItemsFilter(Int_t filter); // 0 - all, 1 - fetched, 2 - monitored
      Int_t GetItemFilter() const { return fiFilterIndex; }

      Int_t RequestBrowserObject(const char *name, Int_t wait_time = 0);
      Int_t RequestBrowserObject(TGo4Slot *slot, Int_t wait_time = 0);

      void AddServerProxy(TGo4ServerProxy *serv, const char *slotname, const char *info);
      void OpenFile(const char *fname);
      Bool_t ConnectHServer(const char *servername,
                            Int_t portnumber,
                            const char *basename,
                            const char *userpass,
                            const char *filter);
      Bool_t ConnectDabc(const char *nodename);

      void MakeFilesList(TObjArray *arr);
      void MakeDabcList(TObjArray *arr);
      void MakeHttpList(TObjArray *arr);
      void MakeHServerList(TObjArray *arr);

      Bool_t ProduceExplicitCopy(const char *itemname, const char *tgtpath = nullptr, Bool_t forcerequest = kFALSE);
      Bool_t ProduceExplicitCopy(TGo4Slot *itemslot, const char *tgtpath = nullptr, Bool_t forcerequest = kFALSE);

      void ClearClipboard();
      void AddToClipboard(const char *itemname);
      Bool_t IsClipboard() const;
      void CopyClipboard(const char *tgtpath, Bool_t forcerequest = kFALSE);

      void CreateMemorySubfolder(const char *itemname, const char *newfoldername);
      void RenameMemoryItem(const char *itemname, const char *newname);
      void ClearMemoryItem(const char *itemname);

      void RequestObjectStatus(const char *name, TGo4Slot *tgtslot);

      void PerformTreeDraw(const char *treename,
                           const char *Xexp,
                           const char *Yexp,
                           const char *Zexp,
                           const char *cutcond,
                           const char *hname,
                           TString &createdhistoname);

      TString SaveToMemory(const char *pathname, TObject *obj, Bool_t ownership, Bool_t overwrite = kFALSE);
      void CreateMemoryFolder(const char *foldername = nullptr);

      void Scan_gROOT();

      Bool_t SaveBrowserToFile(const char *filename,
                               Bool_t prefetch = kFALSE,
                               const char *selectedpath = nullptr,
                               const char *description = nullptr);
      void ExportItemsTo(TObjArray *items,  // array of TObjString
                         Bool_t fetchitems,
                         const char *filename,
                         const char *filedir,
                         const char *format,
                         const char *description);

      void ImportObjectFromFile(const char *filename,const char *filedir,
          const char *format);

      TObject *GetBrowserObject(const char *name, Int_t update = 0);

      Bool_t DeleteDataSource(TGo4Slot *itemslot);

      void DoItemMonitor(TGo4Slot *slot);

      TGo4Slot *BrowserTopSlot();
      TGo4Slot *ItemSlot(const char *itemname);

      Bool_t DefineTreeName(const char *itemname, TString &treename);
      Bool_t DefineLeafName(const char *itemname, const char *treename, TString &leafname);

      TGo4Slot *FindServerSlot(Bool_t databranch, Int_t kind = 0);
      TGo4AnalysisProxy *FindAnalysis(const char *itemname = nullptr);
      TGo4ServerProxy *FindServer(const char *itemname = nullptr, Bool_t asanalysis = kTRUE);
      TString FindItemInAnalysis(const char *objname);
      TString FindItem(const char *objname);
      void FetchItem(const char *itemname, Int_t wait_time = 0);
      void RedrawItem(const char *itemname);

      Bool_t DefineFileObject(const char *itemname, TString &filedataslot, const char **filepath);
      Bool_t UpdateObjectInFile(const char *itemname, const char *fileslotname, const char *filepath);
      Bool_t SaveItemToFile(const char *itemname, const char *filename, const char *subfolder = nullptr);

      Bool_t UpdateAnalysisItem(const char *itemname, TObject *obj = nullptr);

      Bool_t DefineRelatedObject(const char *itemname, const char *objectname, TString &objectitem, Int_t mask = 3);

      Bool_t IsItemRemote(const char *name);
      Bool_t IsItemRemote(TGo4Slot *slot) const;
      Bool_t IsAnalysisItem(const char *name);

      static void SetItemTimeDate(TGo4Slot *slot, const char *stime = nullptr, const char *sdate = nullptr);
      static const char *ItemTime(TGo4Slot *slot);
      static const char *ItemDate(TGo4Slot *slot);

      static void SetLinkedName(TGo4Slot *slot, const char *itemname);
      static const char *GetLinkedName(TGo4Slot *slot);

      void SetCanDelete(TGo4Slot *slot, Bool_t on = kTRUE);
      Bool_t IsCanDelete(TGo4Slot *slot) const;

      Int_t ItemKind(const char *name);
      static Int_t ItemKind(TGo4Slot *slot);
      void SetItemKind(TGo4Slot *slot, Int_t kind, const char *classname, const char *info, Int_t sizeinfo);
      static const char *ItemInfo(TGo4Slot *slot);
      Int_t ItemSizeInfo(TGo4Slot *slot);

      void SetCalcSize(TGo4Slot *slot, Int_t size);
      Int_t GetCalcSize(TGo4Slot *slot);

      TClass *ItemClass(const char *name);
      TClass *ItemClass(TGo4Slot *slot);
      const char *ItemClassName(const char *name);
      static const char *ItemClassName(TGo4Slot *slot);

      Int_t ItemCanDo(const char *name);
      Int_t ItemCanDo(TGo4Slot *slot);
      static void SetItemCanDo(TGo4Slot *slot, Int_t cando);

      Bool_t IsItemMonitored(TGo4Slot *slot) const;
      void SetItemMonitored(TGo4Slot *slot, Bool_t on = kTRUE);

      void SetProtectionBits(TGo4Slot *slot, Int_t delprot, Int_t clearprot);
      void GetProtectionBits(TGo4Slot *slot, Int_t &delprot, Int_t &clearprot);

      void ToggleMonitoring(Int_t period);
      void SetMonitorBlockingFlag(Bool_t blocked = kFALSE) { fbBlockMonitoring = blocked; }
      Int_t MonitoringPeriod() const { return fiMonitoringPeriod; }
      Int_t UpdateVisibleAnalysisObjects(bool checkmonitor);
      Int_t UpdateAllMonitoredObjects();

      TGo4ServerProxy *DefineServerProxy(const char *itemname);
      TGo4ServerProxy *DefineServerObject(const char *itemname, TString *objname = nullptr, Bool_t onlyanalysis = kTRUE);
      TGo4ServerProxy *DefineAnalysisObject(const char *itemname, TString &analysisname);

      static bool CanExecuteItem(int cando);
      static bool CanExpandItem(int cando);
      static bool CanExportItem(int cando);
      static bool CanInfoItem(int cando);
      static bool CanCloseItem(int cando);
      static bool CanClearItem(int cando);
      static bool CanDrawItem(int cando);
      static bool CanDragItem(int cando);
      static bool CanEditItem(int cando);

      static Int_t DefineItemProperties(Int_t kind, TClass *cl, TString &pixmap);
      static Bool_t UpdateObjectContent(TObject *obj, TObject *newobj, Int_t *hasrebinx = nullptr, Int_t *hasrebiny = nullptr);

      void SyncBrowserSlots();

      Bool_t HandleTimer(TTimer *timer) override;

   protected:

      void UpdateAllCanvases();

      void AddWaitingList(TGo4Slot *itemslot, const char *destination = nullptr);
      void CheckWaitingList(TGo4Slot *source);

      static Int_t CompareAxis(TAxis *ax1, TAxis *ax2);
      static Bool_t CompareAxisValues(Double_t v1, Double_t v2, Double_t scale);

      Int_t CalculateFolderSizes(TGo4Slot *topslot);

      void CheckPictureMonitor(TGo4Slot *slot);
      void CheckPictureMonitor(TGo4Picture *pic, const char *picitemname);

      static void SaveAxisTimeProperties(TGraph *gr, Bool_t& timedisplay, TString &format);
      static void RestoreAxisTimeProperties(TGraph *gr, Bool_t& timedisplay, TString &format);

      /** preserve fit result functions contained in any graph-like objects*/
      static void UpdateListOfFunctions(TGraph *oldgr, TGraph *newgr);

      /** JAM24: also preserve fit result functions contained in any TH1 */
      static void UpdateHistoListOfFunctions(TH1 *oldh, TH1 *newh);

      TString             fxDataPath;                 //!
      TString             fxBrowserPath;              //!
      TString             fxViewPath;                 //!
      Bool_t              fbWithRootBrowser{kFALSE};  //!
      TString             fxMemoryPath;               //!
      TGo4ObjectManager  *fxOM{nullptr};              //!
      TGo4Slot           *fxBrowserSlot{nullptr};     //!
      Int_t               fiMonitoringPeriod{0};      //!
      TTimer             *fxMonitorTimer{nullptr};    //!
      Bool_t              fbBlockMonitoring{kFALSE};  //!
      Int_t               fiFilterIndex{0};           //!
      TList              *fxWaitingList{nullptr};     //!
      TObjArray          *fxClipboard{nullptr};       //!
      TTimer             *fxSyncTimer{nullptr};       //!
      Bool_t              fbBlockSync{kFALSE};        //!
      TVirtualTreePlayer *fDummyTreePlayer{nullptr};  //!  to overcome problem in ROOT

   ClassDefOverride(TGo4BrowserProxy, 0);
};

#endif
