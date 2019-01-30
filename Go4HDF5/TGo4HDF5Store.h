// $Id: TGo4HDF5Store.h 1636 2015-06-24 12:05:31Z adamczew $
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

#ifndef TGO4HDF5STORE_H
#define TGO4HDF5STORE_H



#include "TGo4EventStore.h"
#include "TGo4HDF5StoreParameter.h"

#include "H5Cpp.h"

class TFile;
class TTree;

class TGo4EventCalibration;
class TGo4EventElement;
class TGo4HDF5StoreParameter;

/**
 * Event store which fills entries to a dataset in a hdf5 formatted file
 *  * @author J. Adamczewski-Musch
 * @since 12/2018
 */
class TGo4HDF5Store : public TGo4EventStore {
  public:

    TGo4HDF5Store();

    TGo4HDF5Store(const char* name,
                  UInt_t flags=H5F_ACC_TRUNC);

    TGo4HDF5Store(TGo4HDF5StoreParameter* par);

    virtual ~TGo4HDF5Store();

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    virtual Int_t Store(TGo4EventElement* event);

    /** Saves a calibration object correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Parameter* cali);

    /** Saves a condition correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Condition* conny);

    /** Saves a fitter correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Fitter* fitter);

    /** Saves a complete objects folder correllated with the current event into the storage. */
    virtual Int_t Store(TFolder* fold);

    /** Set the file compression level. May be changed on the fly. */
    void SetFlags(Int_t flags);


    /** Standard go4 name of the branch used in all treestore
      * and treesource implementations. */
    static const char* fgcEVBRANCHNAME; //!

    /** Standard suffix for file name */
    static const char* fgcFILESUF; //!

    /** Standard go4 suffix for tree name */
    static const char* fgcTREESUF; //!



  protected:

    /** opens the hdf5 file depending on the setup */
    void OpenFile();

    /** opens the hdf5 file depending on the setup */
    void CloseFile();

    /** used by all Store methods to write with referencing event number in name */
    void WriteToStore(TNamed* ob);

    /** initialize dataset from event structure*/
    void BuildDataSet(TGo4EventElement* event, size_t parentoffset=0);

    /** delete dataset resource*/
    void DeleteDataSet();

    /** evaluate total memory size of event object regarding composite subevents*/
    size_t ScanEventSize(TGo4EventElement* event);


    /** Convert common go4 filemode flags to hdf5 flags: **/
    UInt_t ConvertFileMode(Go4_H5_File_Flags flags);


  private:

    H5::H5File* fxFile; //!

    H5::DataSet fxDataSet; //!

    H5::CompType* fxType; //!

    H5::DataSpace* fxDataSpace; //!

    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbDataSetExists;

    /** Points to event structure to be filled into dataset. */
    TGo4EventElement * fxEvent; //!

    /** remember file property flags?. */
    UInt_t fiFlags;

    /** size of event structure in bytes, for redefining output dataset*/
    size_t fiEventSize;


    /** counter of filled events. */
    hsize_t fiFillCount; //!

  ClassDef(TGo4HDF5Store,1)
};



#endif //TGO4FILESTORE_H
