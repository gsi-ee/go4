// $Id: Go4HDF5.h $
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

// common definitions for go4 hdf5 plugins

#ifndef GO4HDF5_H
#define GO4HDF5_H


// following define will switch on detailed dumps of hdf5 io data:
//#define GO4HDF5_DEBUG 1

#ifdef GO4HDF5_DEBUG
#define go4hdfdbg( args... )  printf( args );
#else
#define  go4hdfdbg( args...) ;

#endif


#define go4hdfdatahandlethrow( args ... ) \
 TString msg=   TString::Format( args ); \
                 TGo4Log::Error("TGo4HDF5DataHandle: %s", msg.Data()); \
                 fxParentSource->SetErrMess(msg.Data()); \
                 throw TGo4EventSourceException(fxParentSource);  \




/** to decouple parameter from hdf5 library we use own definitions for file modes*/
typedef enum Go4_H5_File_Flags{
  GO4_H5F_ACC_NONE,
  GO4_H5F_ACC_TRUNC,
  GO4_H5F_ACC_EXCL,
  GO4_H5F_ACC_RDONLY,
  GO4_H5F_ACC_RDWR
}  Go4_H5_File_Flags;


#endif //GO4EVENTSERVER_H
