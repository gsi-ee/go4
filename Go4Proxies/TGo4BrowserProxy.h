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

#ifndef TGO4PANELCONTAINER_H
#define TGO4PANELCONTAINER_H

#include "TGo4Proxy.h"

#include "TString.h"

class TList;
class TTimer;

class TGo4ObjectManager;
class TGo4AnalysisProxy;
class TGo4ServerProxy;
class TGo4Picture;
class TAxis;
class TVirtualTreePlayer;
class TGraph;

class TGo4BrowserProxy : public TGo4Proxy {
   public:
      TGo4BrowserProxy();
      TGo4BrowserProxy(const char* datapath,
                       const char* viewpath,
                       Bool_t withRootBrowser = kFALSE);
      virtual ~TGo4BrowserProxy();

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);

      virtual Bool_t Use() const { return kFALSE; }

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param);
      virtual Int_t GetObjectKind() {  return TGo4Access::kndFolder; }
      virtual const char* GetContainedClassName() { return ClassName(); }
      virtual TObject* GetAssignedObject() { return this; }

      // browser functionality

      void DataSlotName(const char* item, TString& res);
      void BrowserSlotName(const char* item, TString& res);
      TGo4Slot* BrowserSlot(const char* item);
      TGo4Slot* DataSlot(const char* item);
      TGo4Slot* BrowserMemorySlot();
      Bool_t BrowserItemName(TGo4Slot* itemslot, TString& res);

      void UpdateBrowserContent();
      void InformBrowserUpdate();
      void SetItemsFilter(Int_t filter); // 0 - all, 1 - fetched, 2 - monitored
      Int_t GetItemFilter() const { return fiFilterIndex; }

      Int_t RequestBrowserObject(const char* name, Int_t wait_time = 0);
      Int_t RequestBrowserObject(TGo4Slot* slot, Int_t wait_time = 0);

      void AddServerProxy(TGo4ServerProxy* serv, const char* slotname, const char* info);
      void OpenFile(const char* fname);
      Bool_t ConnectHServer(const char* servername,
                            Int_t portnumber,
                            const char* basename,
                            const char* userpass,
                            const char* filter);
      Bool_t ConnectDabc(const char* nodename);

      void MakeFilesList(TObjArray* arr);
      void MakeDabcList(TObjArray* arr);
      void MakeHttpList(TObjArray* arr);
      void MakeHServerList(TObjArray* arr);

      Bool_t ProduceExplicitCopy(const char* itemname, const char* tgtpath = 0, Bool_t forcerequest = kFALSE);
      Bool_t ProduceExplicitCopy(TGo4Slot* itemslot, const char* tgtpath = 0, Bool_t forcerequest = kFALSE);

      void ClearClipboard();
      void AddToClipboard(const char* itemname);
      Bool_t IsClipboard();
      void CopyClipboard(const char* tgtpath, Bool_t forcerequest = kFALSE);

      void CreateMemorySubfolder(const char* itemname, const char* newfoldername);
      void RenameMemoryItem(const char* itemname, const char* newname);
      void ClearMemoryItem(const char* itemname);

      void RequestObjectStatus(const char* name, TGo4Slot* tgtslot);

      void PerformTreeDraw(const char* treename,
                           const char* Xexp,
                           const char* Yexp,
                           const char* Zexp,
                           const char* cutcond,
                           const char* hname,
                           TString& createdhistoname);

      TString SaveToMemory(const char* pathname, TObject* obj, Bool_t ownership, Bool_t overwrite = kFALSE);
      void CreateMemoryFolder(const char* foldername = 0);

      void Scan_gROOT();

      Bool_t SaveBrowserToFile(const char* filename,
                               Bool_t prefetch = kFALSE,
                               const char* selectedpath = 0,
                               const char* description = 0);
      void ExportItemsTo(TObjArray* items,  // array of TObjString
                         Bool_t fetchitems,
                         const char* filename,
                         const char* filedir,
                         const char* format,
                         const char* description);

      TObject* GetBrowserObject(const char* name, Int_t update = 0);

      Bool_t DeleteDataSource(TGo4Slot* itemslot);

      void DoItemMonitor(TGo4Slot* slot);

      TGo4Slot* BrowserTopSlot();
      TGo4Slot* ItemSlot(const char* itemname);

      Bool_t DefineTreeName(const char* itemname, TString& treename);
      Bool_t DefineLeafName(const char* itemname, const char* treename, TString& leafname);

      TGo4Slot* FindServerSlot(Bool_t databranch, Int_t kind = 0);
      TGo4AnalysisProxy* FindAnalysis(const char* itemname = 0);
      TGo4ServerProxy* FindServer(const char* itemname = 0, Bool_t asanalysis = kTRUE);
      TString FindItemInAnalysis(const char* objname);
      TString FindItem(const char* objname);
      void FetchItem(const char* itemname, Int_t wait_time = 0);
      void RedrawItem(const char* itemname);

      Bool_t DefineFileObject(const char* itemname, TString& filedataslot, const char** filepath);
      Bool_t UpdateObjectInFile(const char* itemname, const char* fileslotname, const char* filepath);
      Bool_t SaveItemToFile(const char* itemname, const char* filename, const char* subfolder = 0);

      Bool_t UpdateAnalysisItem(const char* itemname, TObject* obj = 0);

      Bool_t DefineRelatedObject(const char* itemname, const char* objectname, TString& objectitem, Int_t mask = 3);

      Bool_t IsItemRemote(const char* name);
      Bool_t IsItemRemote(TGo4Slot* slot);
      Bool_t IsAnalysisItem(const char* name);

      static void SetItemTimeDate(TGo4Slot* slot, const char* stime = 0, const char* sdate = 0);
      static const char* ItemTime(TGo4Slot* slot);
      static const char* ItemDate(TGo4Slot* slot);

      static void SetLinkedName(TGo4Slot* slot, const char* itemname);
      static const char* GetLinkedName(TGo4Slot* slot);

      void SetCanDelete(TGo4Slot* slot, Bool_t on = kTRUE);
      Bool_t IsCanDelete(TGo4Slot* slot);

      Int_t ItemKind(const char* name);
      static Int_t ItemKind(TGo4Slot* slot);
      void SetItemKind(TGo4Slot* slot, Int_t kind, const char* classname, const char* info, Int_t sizeinfo);
      static const char* ItemInfo(TGo4Slot* slot);
      Int_t ItemSizeInfo(TGo4Slot* slot);

      void SetCalcSize(TGo4Slot* slot, Int_t size);
      Int_t GetCalcSize(TGo4Slot* slot);

      TClass* ItemClass(const char* name);
      TClass* ItemClass(TGo4Slot* slot);
      const char* ItemClassName(const char* name);
      static const char* ItemClassName(TGo4Slot* slot);

      Int_t ItemCanDo(const char* name);
      Int_t ItemCanDo(TGo4Slot* slot);
      static void SetItemCanDo(TGo4Slot* slot, Int_t cando);

      Bool_t IsItemMonitored(TGo4Slot* slot);
      void SetItemMonitored(TGo4Slot* slot, Bool_t on = kTRUE);

      void SetProtectionBits(TGo4Slot* slot, Int_t delprot, Int_t clearprot);
      void GetProtectionBits(TGo4Slot* slot, Int_t& delprot, Int_t& clearprot);

      void ToggleMonitoring(Int_t period);
      void SetMonitorBlockingFlag(Bool_t blocked = kFALSE) { fbBlockMonitoring = blocked; }
      Int_t MonitoringPeriod() const { return fiMonitoringPeriod; }
      Int_t UpdateVisibleAnalysisObjects(bool checkmonitor);
      Int_t UpdateAllMonitoredObjects();

      TGo4ServerProxy* DefineServerProxy(const char* itemname);
      TGo4ServerProxy* DefineServerObject(const char* itemname, TString* objname = 0, Bool_t onlyanalysis = kTRUE);
      TGo4ServerProxy* DefineAnalysisObject(const char* itemname, TString& analysisname);

      static bool CanExecuteItem(int cando);
      static bool CanExpandItem(int cando);
      static bool CanExportItem(int cando);
      static bool CanInfoItem(int cando);
      static bool CanCloseItem(int cando);
      static bool CanClearItem(int cando);
      static bool CanDrawItem(int cando);
      static bool CanDragItem(int cando);
      static bool CanEditItem(int cando);

      static Int_t DefineItemProperties(Int_t kind, TClass* cl, TString& pixmap);
      static Bool_t UpdateObjectContent(TObject* obj, TObject* newobj, Int_t* hasrebinx = 0, Int_t* hasrebiny = 0);

      void SyncBrowserSlots();

      virtual Bool_t HandleTimer(TTimer* timer);

   protected:

      void UpdateAllCanvases();

      void AddWaitingList(TGo4Slot* itemslot, const char* destination = 0);
      void CheckWaitingList(TGo4Slot* source);

      static Int_t CompareAxis(TAxis* ax1, TAxis* ax2);
      static Bool_t CompareAxisValues(Double_t v1, Double_t v2, Double_t scale);

      Int_t CalculateFolderSizes(TGo4Slot* topslot);

      void CheckPictureMonitor(TGo4Slot* slot);
      void CheckPictureMonitor(TGo4Picture* pic, const char* picitemname);

      static void SaveAxisTimeProperties(TGraph* gr, Bool_t& timedisplay, TString& format);
      static void RestoreAxisTimeProperties(TGraph* gr, Bool_t& timedisplay, TString& format);

      TString            fxDataPath;    //!
      TString            fxBrowserPath; //!
      TString            fxViewPath;    //!
      Bool_t             fbWithRootBrowser; //!
      TString            fxMemoryPath;  //!
      TGo4ObjectManager* fxOM;          //!
      TGo4Slot*          fxBrowserSlot; //!
      Int_t              fiMonitoringPeriod; //!
      TTimer*            fxMonitorTimer; //!
      Bool_t             fbBlockMonitoring;  //!
      Int_t              fiFilterIndex;  //!
      TList*             fxWaitingList; //!
      TObjArray*         fxClipboard; //!
      TTimer*            fxSyncTimer; //!
      Bool_t             fbBlockSync; //!
      TVirtualTreePlayer* fDummyTreePlayer; //!  to overcome problem in ROOT

   ClassDef(TGo4BrowserProxy,1);
};

#endif
