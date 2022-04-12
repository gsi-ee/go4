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

#ifndef TGO4HDF5STOREPARAMETER_H
#define TGO4HDF5STOREPARAMETER_H

#include "TGo4EventStoreParameter.h"
#include "Go4HDF5.h"

class TGo4HDF5StoreParameter : public TGo4EventStoreParameter {

   public:
      TGo4HDF5StoreParameter() ;

      TGo4HDF5StoreParameter(const char* name, Go4_H5_File_Flags flags = GO4_H5F_ACC_TRUNC);

      virtual ~TGo4HDF5StoreParameter();

      /** basic method to printout object */
      void Print(Option_t* = "") const override;

      /** update contents of parameter class with external object. */
      Bool_t UpdateFrom(TGo4Parameter* rhs) override;

      Go4_H5_File_Flags GetHDF5Flags() const { return fiFlags; }
      void SetHDF5Flags(Go4_H5_File_Flags flags) { fiFlags=flags; }

   private:

      /** Flags for HDF5 file mode*/
      Go4_H5_File_Flags fiFlags{GO4_H5F_ACC_NONE};

   ClassDefOverride(TGo4HDF5StoreParameter,1)
};

#endif //TGO4HDF5STOREPARAMETER_H
