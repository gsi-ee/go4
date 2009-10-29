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

#ifndef TGO4HSERVPROXY_H
#define TGO4HSERVPROXY_H

#include "TGo4Proxy.h"

#include "TString.h"

class TH1;

class TGo4HServProxy : public TGo4Proxy {
   public:
      TGo4HServProxy();
      virtual ~TGo4HServProxy();

      void SetHServConfig(const char* servername,
                          Int_t portnumber,
                          const char* basename,
                          const char* userpass,
                          const char* filter);

      const char* GetServerName() const { return fServerName.Data(); }
      Int_t GetPortNumber() const { return fPortNumber; }
      const char* GetBaseName() const { return fBaseName.Data(); }
      const char* GetUserPass() const { return fUserPass.Data(); }
      const char* GetFilter() const { return fFilter.Data(); }

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);

      virtual Bool_t HasSublevels() const;

      virtual TGo4LevelIter* MakeIter();

      virtual TGo4Access* MakeProxy(const char* name);

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      virtual void Update(TGo4Slot* slot, Bool_t strong);

      Bool_t RequestHistosList();
      TH1* GetHistogram(const char* remotehistoname);

   protected:

      TString    fServerName;     //!
      Int_t      fPortNumber;     //!
      TString    fBaseName;       //!
      TString    fUserPass;       //!
      TString    fFilter;         //!

      TGo4Slot*   fxStructure;   //!

   ClassDef(TGo4HServProxy, 1);
};

#endif
