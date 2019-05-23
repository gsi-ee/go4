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

#ifndef TGO4HDF5ADAPTER_H
#define TGO4HDF5ADAPTER_H


#include "Go4HDF5.h"
#include "Rtypes.h"

#ifndef __CINT__
#include "H5Cpp.h"
#endif


class TClass;
class TGo4EventElement;

/**
 * Adapter class for io with hdf5 formatted file
 *  * @author J. Adamczewski-Musch
 * @since 5/2019
 */
class TGo4HDF5Adapter {
  public:

    TGo4HDF5Adapter() : fxFile(0),
    fxDataSet(),fxType(0), fxDataSpace(0),
    fbDataSetExists(kFALSE),
    fxEvent(0),fiFlags(H5F_ACC_TRUNC),
    fiEventSize(0),fiFillCount(0) {};

    virtual ~TGo4HDF5Adapter(){};


    /** Standard suffix for file name */
    static const char* fgcFILESUF; //!




  protected:

    /** opens the hdf5 file depending on the setup */
    virtual void OpenFile(const char* fname=0) =0;

    /** opens the hdf5 file depending on the setup */
    virtual void CloseFile();




    /** initialize dataset from event structure*/
    virtual void BuildDataSet(TGo4EventElement* event) =0;

    /** delete dataset resource*/
    virtual void DeleteDataSet();

    /** Prepare data type from event structure for hdf5.
     * Return value is name of ROOT class that was used to compose the hdf5 type*/
     const char* BuildDataType(TGo4EventElement* event);

    /** evaluate total memory size of event object regarding composite subevents*/
    size_t ScanEventSize(TGo4EventElement* event);

    /** evaluate h5 type information from root class streamer*/
    void FillTypeInfo(H5::CompType* h5type, TClass* rootclass, const char* basename=0);


    /** Convert common go4 filemode flags to hdf5 flags: **/
    UInt_t ConvertFileMode(Go4_H5_File_Flags flags);



#ifndef __CINT__
    H5::H5File* fxFile; //!

    H5::DataSet fxDataSet; //!

    H5::CompType* fxType; //!

    H5::DataSpace* fxDataSpace; //!
#endif
    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbDataSetExists;

    /** Points to event structure to be filled into dataset. */
    TGo4EventElement * fxEvent; //!

    /** remember file property flags?. */
    UInt_t fiFlags;

    /** size of event structure in bytes, for redefining output dataset*/
    size_t fiEventSize;

#ifndef __CINT__
    /** counter of filled events. */
    hsize_t fiFillCount; //!
#endif

};



#endif //TGO4HDF5ADAPTER_H
