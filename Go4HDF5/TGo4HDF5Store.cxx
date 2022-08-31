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


#include "TGo4HDF5Store.h"

#include "TFolder.h"
#include "TFile.h"
#include "TTree.h"
#include "TDataMember.h"
#include "TVirtualCollectionProxy.h"
#include "TBaseClass.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"
#include "TGo4EventStoreException.h"

#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4Fitter.h"
#include "TGo4HDF5StoreParameter.h"


TGo4HDF5Store::TGo4HDF5Store() :
   TGo4EventStore("Go4 Default HDF5 Store"), TGo4HDF5Adapter()

{
   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store()", __LINE__, __FILE__));
}

TGo4HDF5Store::TGo4HDF5Store(const char *name,
                             UInt_t flags) :
   TGo4EventStore(name),TGo4HDF5Adapter()

{
   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store(char*,...)", __LINE__, __FILE__));
   OpenFile();
}


TGo4HDF5Store::TGo4HDF5Store(TGo4HDF5StoreParameter* par) :
         TGo4EventStore("dummy"),TGo4HDF5Adapter()
{
   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store(TGo4HDF5StoreParameter* par)", __LINE__, __FILE__));
   if (!par) {
      TGo4Log::Error("TGo4HDF5Store::TGo4HDF5Store(.., TGo4HDF5StoreParameter* is not specified");
      return;
   }
   SetName(par->GetName());
   fiFlags=ConvertFileMode(par->GetHDF5Flags());
   OpenFile();
}


TGo4HDF5Store::~TGo4HDF5Store()
{
   GO4TRACE((15,"TGo4HDF5Store::~TGo4HDF5Store()", __LINE__, __FILE__));
   //printf("TGo4HDF5Store::~TGo4HDF5Store()\n");
   DeleteDataSet();
   CloseFile();
}

void TGo4HDF5Store::OpenFile(const char*)
{
   TString buffer(GetName());
   if (!strstr(buffer.Data(), fgcFILESUF))
      buffer.Append(fgcFILESUF);

   try {
      CloseFile();
      fxFile = new H5::H5File(buffer.Data(), fiFlags);
      if (fiFlags == H5F_ACC_TRUNC) {
         TGo4Log::Info("TGo4HDF5Store: Open file %s RECREATE", buffer.Data());
      } else if (fiFlags == H5F_ACC_RDWR) {
         TGo4Log::Info("TGo4HDF5Store: Open file %s for UPDATE", buffer.Data());
      } else if (fiFlags == H5F_ACC_EXCL) {
         TGo4Log::Info("TGo4HDF5Store: Open file %s exclusively", buffer.Data());
      } else {
         TGo4Log::Info("TGo4HDF5Store: Open file %s with unsupported mode %d", buffer.Data(), fiFlags);
      }

   } catch (H5::Exception &ex) {
      TString msg = TString::Format("OpenFile with HDF5 exception in %s : %s\n", ex.getCFuncName(), ex.getCDetailMsg());
      TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
      throw TGo4EventStoreException(this, msg.Data());
   }
}


void TGo4HDF5Store::BuildDataSet(TGo4EventElement* event)
{
   if (fbDataSetExists)
      return;
   if (!event || !fxFile)
      return;
   try {

      BuildDataType(event);
      fxHandle->BuildWriteDataset(fxFile);
      fbDataSetExists = kTRUE;
   }

   catch (H5::Exception &ex) {
      TString msg =
         TString::Format("BuildDataSet with HDF5 exception in %s : %s\n", ex.getCFuncName(), ex.getCDetailMsg());
      TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
      throw TGo4EventStoreException(this, msg.Data());
   }
}

Int_t TGo4HDF5Store::Store(TGo4EventElement* event)
{
   GO4TRACE((12,"TGo4HDF5Store::Store(TGo4EventElement*)", __LINE__, __FILE__));

   fxEvent = event; // address of next event into event pointer
   BuildDataSet(event);

   try {

      fxHandle->SetObjectPointer(
         event); // this will recursively update all subpointers and properties of vector (hopefully...)
      fxHandle->Write(fiFillCount++, fxFile);

   } catch (H5::Exception &ex) {
      TString msg = TString::Format("Store() with HDF5 exception in %s : %s\n", ex.getCFuncName(), ex.getCDetailMsg());
      TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
      throw TGo4EventStoreException(this, msg.Data());
   }
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Parameter* cali)
{
   WriteToStore(cali);
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Condition* conny)
{
   WriteToStore(conny);
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Fitter* fitter)
{
   WriteToStore(fitter);
   return 0;
}

Int_t TGo4HDF5Store::Store(TFolder* fold)
{
   WriteToStore(fold);
   return 0;
}

void TGo4HDF5Store::WriteToStore(TNamed* ob)
{
   if (!ob) return;
   TGo4Log::Info("TGo4HDF5Store: WriteToStore not yet implemented for auxiliary objects of class %s (name:%s)\n",ob->Class()->GetName(), ob->GetName());
}

