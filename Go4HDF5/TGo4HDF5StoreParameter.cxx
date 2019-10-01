// $Id: TGo4HDF5StoreParameter.cxx 1636 2015-06-24 12:05:31Z adamczew $
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

#include "TGo4HDF5StoreParameter.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4HDF5StoreParameter::TGo4HDF5StoreParameter(const char* name,
    Go4_H5_File_Flags  flags)
: TGo4EventStoreParameter(name, GO4EV_HDF5),
   fiFlags(flags)
{
   GO4TRACE((14,"TGo4HDF5StoreParameter::TGo4HDF5StoreParameter(const char*,...)", __LINE__, __FILE__));
   SetTitle(name);
}

TGo4HDF5StoreParameter::TGo4HDF5StoreParameter()
: TGo4EventStoreParameter("Default Go4 HDF5 Store", GO4EV_HDF5),
   fiFlags(GO4_H5F_ACC_NONE)
{
   GO4TRACE((14,"TGo4HDF5StoreParameter::TGo4HDF5StoreParameter()", __LINE__, __FILE__));
   SetTitle("Go4HDF5Store");
}

TGo4HDF5StoreParameter::~TGo4HDF5StoreParameter()
{
}

Int_t TGo4HDF5StoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
 GO4TRACE((12,"TGo4HDF5StoreParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   Int_t delta=TGo4EventStoreParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  File flags: \t%d \n",fiFlags);
   if(buffer==0)
      {
      std::cout << localbuf << std::endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
            size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}


Bool_t TGo4HDF5StoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   GO4TRACE((12,"TGo4HDF5StoreParameter::UpdateFrom()",__LINE__, __FILE__));

   TGo4HDF5StoreParameter* filepar=dynamic_cast<TGo4HDF5StoreParameter*>(rhs);
   if (filepar==0) return kFALSE;
   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetHDF5Flags(filepar->GetHDF5Flags());
   return kTRUE;
}
