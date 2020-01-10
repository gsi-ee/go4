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

      TGo4HDF5StoreParameter(const char* name,
                  Go4_H5_File_Flags flags=GO4_H5F_ACC_TRUNC);

      virtual ~TGo4HDF5StoreParameter();

      /** basic method to printout status information
        * on stdout; to be overridden by specific subclass */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

      Go4_H5_File_Flags GetHDF5Flags() const { return fiFlags; }
      void SetHDF5Flags(Go4_H5_File_Flags flags) { fiFlags=flags; }


   private:

      /** Flags for HDF5 file mode*/
      Go4_H5_File_Flags fiFlags;


   ClassDef(TGo4HDF5StoreParameter,1)
};

#endif //TGO4FILESTOREPARAMETER_H
