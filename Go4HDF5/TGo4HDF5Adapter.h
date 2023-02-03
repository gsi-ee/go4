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

#ifndef TGO4HDF5ADAPTER_H
#define TGO4HDF5ADAPTER_H


#include "Go4HDF5.h"
#include "RtypesCore.h"
#include "TString.h"

#ifndef __CINT__
#include "H5Cpp.h"
#endif

#include "TGo4HDF5DataHandle.h"


/**
 * Adapter class for io with hdf5 formatted file
 *  * @author J. Adamczewski-Musch
 * @since 5/2019
 */
class TGo4HDF5Adapter {
  public:

   TGo4HDF5Adapter() :
         fxFile(nullptr), fxHandle(nullptr), fbDataSetExists(kFALSE), fxEvent(nullptr),
         fiFlags(H5F_ACC_TRUNC), fiFillCount(0) {
   }

    virtual ~TGo4HDF5Adapter() {}


    /** Standard suffix for file name */
    static const char *fgcFILESUF; //!

    /** Add subhandle to handle that represents a containend collection.
       * classname or valueclass, resp, refers to type of element in container,*/
      static void AddSubHandle(TGo4HDF5DataHandle *handle, const char *name, const char *type, size_t size,
          size_t memberoffset, const char *membername, const char *classname, TClass *valueclass);

      /** evaluate h5 type information from root class streamer*/
      static void FillTypeInfo(TGo4HDF5DataHandle *handle, TClass *rootclass, const char *basename = nullptr);

         /** evaluate h5 type information for basic types of memtypename*/
      static void FillTypeInfo(TGo4HDF5DataHandle* handle,
               const char *membername, const char *memtypename, size_t memberoffset = 0,
               Int_t arraydim = 0, TDataMember *member = nullptr);


protected:

    /** opens the hdf5 file depending on the setup */
    virtual void OpenFile(const char *fname = nullptr) = 0;

    /** opens the hdf5 file depending on the setup */
    virtual void CloseFile();

    /** initialize dataset from event structure*/
    virtual void BuildDataSet(TGo4EventElement *event) = 0;

    /** delete dataset resource*/
    virtual void DeleteDataSet();

    /** Prepare data type from event structure for hdf5.
     * Return value is name of ROOT class that was used to compose the hdf5 type.
     * In case of composite subevents, the current parent data handle and the index in array may be passed.*/
     void BuildDataType(TGo4EventElement *event, TGo4HDF5DataHandle* parent = nullptr, Int_t index = 0);

    /** evaluate total memory size of event object regarding composite subevents*/
    size_t ScanEventSize(TGo4EventElement *event);

    /** Convert common go4 filemode flags to hdf5 flags: **/
    UInt_t ConvertFileMode(Go4_H5_File_Flags flags);

#ifndef __CINT__

    /** the currently open hdf5 file*/
    H5::H5File* fxFile{nullptr}; //!

    /** handle object to recursively treat the io of nested event components*/
    TGo4HDF5DataHandle* fxHandle{nullptr}; //!

#endif
    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbDataSetExists{kFALSE};

    /** Points to event structure to be filled into dataset. */
    TGo4EventElement *fxEvent{nullptr}; //!

    /** remember file property flags?. */
    UInt_t fiFlags{0};

#ifndef __CINT__
    /** counter of filled events. */
    hsize_t fiFillCount{0}; //!
#endif

};

#endif //TGO4HDF5ADAPTER_H
