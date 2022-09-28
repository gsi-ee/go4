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

#ifndef TGO4BROWSERITEM_H
#define TGO4BROWSERITEM_H

#include "TFolder.h"

class TIterator;
class TGo4BrowserProxy;
class TGo4RootBrowserProxy;

class TGo4BrowserItem : public TFolder {
   public:
      TGo4BrowserItem();
      TGo4BrowserItem(const char *name, const char* title);
      TGo4BrowserItem(TGo4BrowserItem* parent, TGo4BrowserItem* previtem, const char *name, const char* title);
      virtual ~TGo4BrowserItem();

      void Browse(TBrowser* b) override;
      virtual void DrawItem();         // *MENU*
      virtual void CopyToWorkspace();  // *MENU*
      virtual void DeleteItem();       // *MENU*
      virtual void SetMonitorOn();     // *MENU*
      virtual void SetMonitorOff();    // *MENU*
      virtual void ToggleMonitoring(Int_t sec); // *MENU*
      virtual void StartAnalysis(); // *MENU*
      virtual void StopAnalysis(); // *MENU*

      TGo4BrowserItem* GetParent() const { return fParent; }

      void SetBrowser(TGo4BrowserProxy* br, TGo4RootBrowserProxy* br2);

      TGo4BrowserItem* firstChild();
      TGo4BrowserItem* nextChild();
      void deleteChild(TGo4BrowserItem* item);
      void deleteChilds();

      void SetItemClass(const char *name) { fItemClass = name; }
      const char* GetItemClass() const { return fItemClass.Data(); }

      void SetIsFolder(Bool_t b) { fIsFolder = b; }
      Bool_t IsFolder() const override { return fIsFolder; }

      const char* GetIconName() const override { return fIconName.Data(); }
      void SetIconName(const char *name) { fIconName = name; }

      void ProduceFullName(TString& fullname);
      TString GetFullName();

      const char* ClassName() const override { return GetItemClass(); }
      void Delete(Option_t* option = "") override;
      void SetName(const char *name) override;
      void SetTitle(const char* title = "") override;
      void ls(Option_t* option = "*") const override;

      void SaveAs(const char *filename = "", Option_t *option = "") const override; // *MENU*

      void DrawClass() const override;
      TObject* DrawClone(Option_t* option = "") const override;
      void Dump() const override;
      void Inspect() const override;
      void SetDrawOption(Option_t* option = "") override;
   protected:

      void SetMonitorFlag(Bool_t on);

      TGo4BrowserItem*  fParent{nullptr}; //!
      TIterator* fIter{nullptr};  //!
      TString fItemClass; //!
      TString fIconName; //!
      Bool_t fIsFolder{kFALSE};  //!

      TGo4BrowserProxy     *fBrowser{nullptr}; //!
      TGo4RootBrowserProxy *fRootBrowser{nullptr}; //!

      ClassDefOverride(TGo4BrowserItem,1);
};

#endif
