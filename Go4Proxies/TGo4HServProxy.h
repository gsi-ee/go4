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

#ifndef TGO4HSERVPROXY_H
#define TGO4HSERVPROXY_H

#include "TGo4ServerProxy.h"

#include "TString.h"

class TH1;

class TGo4HServProxy : public TGo4ServerProxy {
   public:
      TGo4HServProxy();
      virtual ~TGo4HServProxy();

      void SetHServConfig(const char *servername,
                          Int_t portnumber,
                          const char *basename,
                          const char *userpass,
                          const char *filter);

      Int_t GetPortNumber() const { return fPortNumber; }
      const char *GetBaseName() const { return fBaseName.Data(); }
      const char *GetUserPass() const { return fUserPass.Data(); }
      const char *GetFilter() const { return fFilter.Data(); }

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;

      Bool_t HasSublevels() const override;

      TGo4LevelIter* MakeIter() override;

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override;

      void WriteData(TGo4Slot *slot, TDirectory* dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory* dir) override;

      Int_t GetObjectKind() override;
      const char *GetContainedClassName() override;

      void Update(TGo4Slot *slot, Bool_t strong) override;

      const char *GetServerName() const override { return fServerName.Data(); }
      Bool_t RefreshNamesList() override;
      TH1* GetHistogram(const char *remotehistoname);

   protected:

      TString    fServerName;        //!
      Int_t      fPortNumber{0};     //!
      TString    fBaseName;          //!
      TString    fUserPass;          //!
      TString    fFilter;            //!

      TGo4Slot  *fxStructure{nullptr};   //!

   ClassDefOverride(TGo4HServProxy, 1);
};

#endif
