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

#ifndef TGO4BROWSERITEM_H
#define TGO4BROWSERITEM_H

#include "RVersion.h"

#include "TFolder.h"

class TIterator;
class TGo4BrowserProxy;
class TGo4RootBrowserProxy;

class TGo4BrowserItem : public TFolder {
   public:
      TGo4BrowserItem();
      TGo4BrowserItem(const char* name, const char* title);
      TGo4BrowserItem(TGo4BrowserItem* parent, TGo4BrowserItem* previtem, const char* name, const char* title);
      virtual ~TGo4BrowserItem();

      virtual void Browse(TBrowser* b);
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

      void SetItemClass(const char* name) { fItemClass = name; }
      const char* GetItemClass() const { return fItemClass.Data(); }

      void SetIsFolder(Bool_t b) { fIsFolder = b; }
      virtual Bool_t IsFolder() const { return fIsFolder; }

      virtual const char* GetIconName() const { return fIconName.Data(); }
      void SetIconName(const char* name) { fIconName = name; }

      void ProduceFullName(TString& fullname);
      TString GetFullName();

      virtual const char* ClassName() const { return GetItemClass(); }
      virtual void Delete(Option_t* option = "");
      virtual void SetName(const char* name);
      virtual void SetTitle(const char* title = "");
      virtual void ls(Option_t* option = "*") const;

#ifdef WIN32

      virtual void SaveAs(const char* filename = "", Option_t *option = "");

#else

#if ROOT_VERSION_CODE < ROOT_VERSION(5,13,6)
      virtual void SaveAs(const char* filename = "");
#else
      virtual void SaveAs(const char* filename = "", Option_t *option = "");
#endif
#endif
      virtual void DrawClass() const;
      virtual TObject* DrawClone(Option_t* option = "") const;
      virtual void Dump() const;
      virtual void Inspect() const;
      virtual void SetDrawOption(Option_t* option = "");
   protected:

      void SetMonitorFlag(Bool_t on);

      TGo4BrowserItem*  fParent; //!
      TIterator* fIter;  //!
      TString fItemClass; //!
      TString fIconName; //!
      Bool_t fIsFolder;  //!

      TGo4BrowserProxy* fBrowser; //!
      TGo4RootBrowserProxy* fRootBrowser; //!

      ClassDef(TGo4BrowserItem,1);
};

#endif
