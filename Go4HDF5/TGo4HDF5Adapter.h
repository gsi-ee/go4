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
#include "TString.h"

#ifndef __CINT__
#include "H5Cpp.h"
#endif

// following define will switch on detailed dumps of read event data:
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





class TClass;
class TDataMember;
class TGo4EventElement;
class TGo4HDF5Source;


/** probably hdf5 needs structure containing the hvl_t to deal with it correctly?*/
struct TGo4HDF5VarContainer
{
  hvl_t fxArray;
};


/**
 * Handle object to access separate datasets in hdf5 formatted file with go4 event data
 *  * @author J. Adamczewski-Musch
 * @since 5/2019
 */
class TGo4HDF5DataHandle {


  enum Go4HDF5_Collection_t {
      GO4HDF5_COLL_NONE,
      GO4HDF5_COLL_STDVEC,
      GO4HDF5_COLL_CARRAY,
      GO4HDF5_COLL_ROOT,
      GO4HDF5_COLL_COMPEVENT
  };


public:

   /** create new data handle*/
   TGo4HDF5DataHandle(const char* name, size_t datasize, const char* collectiontype=0);
   virtual ~TGo4HDF5DataHandle();

   /** return handle for type descriptor. Currently this is filled from external routine of adapter class*/
   H5::CompType* GetType(){return fxType;}

   const char* GetTypeName(){return fxTypeName.Data();}

   void* Data() {return fxData;}

   /** wrapper function to define contained structure member. Will also keep the offset to first assigned member*/
   void InsertTypeMember(const H5std_string& name, size_t offset, const H5::DataType& new_member);

   /** remember location of this object relative to upper level object.*/
   void SetParentOffset(size_t off) {
     fiParentOffset=off;
   }

   /** remember size of a single element (structure) in this collection, if it is one. Otherwise this is size of this class itself*/
   void SetElementSize(size_t len) {fiElementSize=len;}


   /** define location of corresponding object in memory. This is base pointer for all member specific offsets.*/
   void SetObjectPointer(void* memptr);

   /** pass the top level event element down the member hierarchy.
    * Note that this is the "local" top event, e.g. the current Go4 composite subevent that owns the member*/
   void SetTopEvent(TGo4EventElement* eve);

   //TGo4EventElement* GetTopEvent(){return fxEvent;}

   /** pass the top level event element class down the member hierarchy.
    *  Note that this is the "local" top event, e.g. the current Go4 composite subevent that owns the member*/
   void SetTopEventClass(const char* classname);

   /** identifier for the member to access from outside using the top event handle*/
   void SetMemberName(const char* name) {fxMemberHandle=name;}

   /** type of the member to access from outside*/
   void SetMemberClass(const char* clname) {fxMemberClass=clname;}


   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent);

   /** create datasets for writing memory structure of type into file.*/
   void BuildWriteDataset(H5::H5File* file);


   /** Read event of sequence number from file*/
   void Read(hsize_t sequencenum);

   /** Write event of sequence number from file*/
    void Write(hsize_t sequencenum);

   /** create new subhandle for complex member component on heap.
    * Each complex component has separate dataset in file. returns the currently added handle to submember.
    * If submember is part of a collection like std::vector, the name of collection is also passed.*/
   TGo4HDF5DataHandle* AddSubMember(const char* name, size_t datasize, const char* collectiontype=0);



   TGo4HDF5DataHandle* GetSubMember(UInt_t ix){ return (ix< fxSubcomponents.size() ? fxSubcomponents[ix] : 0);}

protected:

   /** (Re-)allocate read buffer depending on the object size read from file*/
   void AllocReadBuffer(size_t size);



private:

#ifndef __CINT__

   /** identifier of the dataset*/
   TString fxTypeName;


   /** component type representing the data structure*/
   H5::CompType* fxType; //!

   /** optional collection type with fxType as entries*/
   //H5::VarLenType* fxCollection; //!

   H5::CompType* fxCollection; //!


   /** handle for variable arrays*/
   TGo4HDF5VarContainer fxVarHandle; //!


   /** dataset containing the structure*/
   H5::DataSet fxDataSet; //!

  /** dataspace in memory*/
   H5::DataSpace* fxDataSpace; //!

   /** dataspace in file*/
   H5::DataSpace fxFileSpace; //!

   /** number of event entries found in file dataspace*/
   ULong_t fiEntries;//!

#endif

       /** pointer to begin of data in memory */
       void* fxData; //!

       /** offset of this data structure relative to top level event object*/
       size_t fiParentOffset; //!



       /** size of the (collection element) structure*/
       size_t fiElementSize;//


    /** secondary read bounce buffer for hdf5 */
    Char_t* fxReadBuffer; //!

    /** size of data structure in bytes, for redefining output dataset*/
      size_t fiDataSize;

    /** begin of real eventdata payload after event object pointer**/
     size_t fiReadOffset;



    /** collect subcomponents of the referenced data object*/
    std::vector<TGo4HDF5DataHandle*> fxSubcomponents;

    /** marks the collection type of this component, e.g. std::vector*/
    Go4HDF5_Collection_t fiCollectionType;

    /** predefined interpreter access command e.g. to operate e.g. on the variable length collection*/
    TString fxMemberHandle;

    /** classname of the entry for interpreter to operate e.g. on variable length collection*/
    TString fxMemberClass;

    /** the top level event element to access the current data member from outside*/
    TGo4EventElement* fxEvent; //!

    /**  the top level event element classname to access the current data member from outside**/
    TString fxEventClass; //!

    /** backpointer to parent source for exceptions*/
    TGo4HDF5Source* fxParentSource; //!

};



/**
 * Adapter class for io with hdf5 formatted file
 *  * @author J. Adamczewski-Musch
 * @since 5/2019
 */
class TGo4HDF5Adapter {
  public:

    TGo4HDF5Adapter() : fxFile(0),fxHandle(0),
    fbDataSetExists(kFALSE),
    fxEvent(0),fiFlags(H5F_ACC_TRUNC),
    fiFillCount(0) {};

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
     * Return value is name of ROOT class that was used to compose the hdf5 type.
     * In case of composite subevents, the current parent data handle and the index in array may be passed.*/
     void BuildDataType(TGo4EventElement* event, TGo4HDF5DataHandle* parent=0, Int_t index=0);

    /** evaluate total memory size of event object regarding composite subevents*/
    size_t ScanEventSize(TGo4EventElement* event);

    /** evaluate h5 type information from root class streamer*/
    void FillTypeInfo(TGo4HDF5DataHandle* handle, TClass* rootclass, const char* basename=0);

    /** evaluate h5 type information for basic types of memtypename*/
    void FillTypeInfo(TGo4HDF5DataHandle* handle,
          const char* membername, const char* memtypename, size_t memberoffset=0,
          Int_t arraydim=0, TDataMember* member=0);

    /** Convert common go4 filemode flags to hdf5 flags: **/
    UInt_t ConvertFileMode(Go4_H5_File_Flags flags);



#ifndef __CINT__

    /** the currently open hdf5 file*/
    H5::H5File* fxFile; //!

    /** handle object to recursively treat the io of nested event components*/
    TGo4HDF5DataHandle* fxHandle; //!

    #endif
    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbDataSetExists;

    /** Points to event structure to be filled into dataset. */
    TGo4EventElement * fxEvent; //!

    /** remember file property flags?. */
    UInt_t fiFlags;


#ifndef __CINT__
    /** counter of filled events. */
    hsize_t fiFillCount; //!
#endif

};



#endif //TGO4HDF5ADAPTER_H
