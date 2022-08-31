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

#ifndef TGO4HDF5DATAHANDLE_H
#define TGO4HDF5DATAHANDLE_H


#include "Go4HDF5.h"
#include "TString.h"
#include "TObject.h"

#ifndef __CINT__
#include "H5Cpp.h"
#endif

class TClass;
class TDataMember;
class TGo4EventElement;
class TGo4HDF5Source;

class TGo4HDF5DataHandle;

class TGo4HDF5DataHandleFactory
{
public:

    TGo4HDF5DataHandleFactory(){}

    /** create implementation of data handle by type*/
    TGo4HDF5DataHandle* CreateDataHandle(const char *name, size_t datasize, const char* type);
};



/**
 * The base class to access a datatype with substructures  in hdf5.
 * datahandle may contain other datahandles as direct subcomponents
 * Each datahandle represents a single hdf5 dataset in the file
 * *  * @author J. Adamczewski-Musch
 * @since 6/2019
 */
class TGo4HDF5DataHandle {




public:

   /** create new data handle*/
   TGo4HDF5DataHandle(const char *name, size_t datasize);
   virtual ~TGo4HDF5DataHandle();

   /** return handle for type descriptor. Currently this is filled from external routine of adapter class*/
   H5::CompType* GetType(){return fxType;}

   const char* GetTypeName(){return fxTypeName.Data();}

   void* Data() {return fxData;}

   /** wrapper function to define contained structure member. Will also keep the offset to first assigned member*/
   void InsertTypeMember(const H5std_string& name, size_t offset, const H5::DataType& new_member);

   /** remember location of this object relative to upper level object.*/
   void SetParentOffset(size_t off) { fiParentOffset = off; }


   /** define location of corresponding object in memory. This is base pointer for all member specific offsets.*/
   virtual void SetObjectPointer(void* memptr);

   /** pass the top level event element down the member hierarchy.
    * Note that this is the "local" top event, e.g. the current Go4 composite subevent that owns the member*/
   void SetTopEvent(TGo4EventElement* eve);


   /** pass the top level event element class down the member hierarchy.
    *  Note that this is the "local" top event, e.g. the current Go4 composite subevent that owns the member*/
   void SetTopEventClass(const char* classname);

   /** identifier for the member to access from outside using the top event handle*/
   void SetMemberName(const char *name) {fxMemberHandle=name;}

   /** type of the member to access from outside*/
   void SetMemberClass(const char* clname) {fxMemberClass=clname;}


   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   virtual void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent);

   /** create datasets for writing memory structure of type into file.*/
   virtual void BuildWriteDataset(H5::H5File* file);


   /** Read event of sequence number from file*/
   virtual void Read(hsize_t sequencenum,  H5::H5File* file);

   /** Write event of sequence number from file*/
    virtual void Write(hsize_t sequencenum,  H5::H5File* file);

   /** create new subhandle for complex member component on heap.
    * Each complex component has separate dataset in file. returns the currently added handle to submember.
    * If submember is part of a collection like std::vector, the name of collection is also passed.*/
   TGo4HDF5DataHandle* AddSubMember(const char *name, size_t datasize, const char* collectiontype = nullptr);



   TGo4HDF5DataHandle* GetSubMember(UInt_t ix){ return (ix< fxSubcomponents.size() ? fxSubcomponents[ix] : nullptr);}

   /** lookup if subcomponent of name already is in list. returns 0 pointer if no match of name, otherwise th matching subhandle*/
   TGo4HDF5DataHandle* FindSubMember(const char *name);

   void SetActive(Bool_t on){fbDataSetActive=on;}

   void SetAllSubMembersActive(Bool_t on);


   static TGo4HDF5DataHandleFactory fxFactory;


protected:


   /** identifier of the dataset*/
   TString fxTypeName;

#ifndef __CINT__

   /** component type representing the data structure*/
   H5::CompType* fxType; //!


   /** dataset containing the structure*/
   H5::DataSet fxDataSet; //!

  /** dataspace in memory*/
   H5::DataSpace* fxDataSpace; //!

   /** dataspace in file*/
   H5::DataSpace fxFileSpace; //!

   /** number of event entries found in file dataspace*/
   ULong_t fiEntries{0};//!

#endif

       /** pointer to begin of data in memory */
       void* fxData{nullptr}; //!

       /** offset of this data structure relative to top level event object*/
       size_t fiParentOffset{0}; //!


       /** size of data structure in bytes, for redefining output dataset*/
       size_t fiDataSize{0};


       /** begin of real eventdata payload after event object pointer**/
       size_t fiReadOffset{0};


    /** collect subcomponents of the referenced data object*/
    std::vector<TGo4HDF5DataHandle*> fxSubcomponents;

    /** predefined interpreter access command e.g. to operate e.g. on the variable length collection*/
    TString fxMemberHandle;

    /** classname of the entry for interpreter to operate e.g. on variable length collection*/
    TString fxMemberClass;

    /** the top level event element to access the current data member from outside*/
    TGo4EventElement* fxEvent{nullptr}; //!

    /**  the top level event element classname to access the current data member from outside**/
    TString fxEventClass; //!

    /** backpointer to parent source for exceptions*/
    TGo4HDF5Source* fxParentSource{nullptr}; //!

    /** check on the fly if this handle has already created a dataset*/
    Bool_t fbDataSetExists{kFALSE}; //!

    /** we may disable this dataset temporarily from writing or reading
     * useful for dynamic vector of vector components*/
    Bool_t fbDataSetActive{kFALSE}; //!

};



////////////////////////////////////////////////////////////7


/**
 * Handle object to access basic event type datasets in hdf5 formatted file with go4 event data
 *  * @author J. Adamczewski-Musch
 * @since 6/2019
 */
class TGo4HDF5BasicDataHandle : public TGo4HDF5DataHandle
{

public:

   /** create new data handle*/
   TGo4HDF5BasicDataHandle(const char *name, size_t datasize);
   virtual ~TGo4HDF5BasicDataHandle();

   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent)  override;

   /** create datasets for writing memory structure of type into file.*/
   void BuildWriteDataset(H5::H5File* file) override;


   /** Read event of sequence number from file*/
   void Read(hsize_t sequencenum, H5::H5File*file) override;

   /** Write event of sequence number from file*/
   void Write(hsize_t sequencenum, H5::H5File*file) override;


protected:

   /** (Re-)allocate read buffer depending on the object size read from file*/
      void AllocReadBuffer(size_t size);


   /** secondary read bounce buffer for hdf5 */
     Char_t* fxReadBuffer; //!

};



/////////////////////////////////////////////////////////////




/**
 * Handle object to access composite event type datasets in hdf5 formatted file with go4 event data
 *  * @author J. Adamczewski-Musch
 * @since 6/2019
 */
class TGo4HDF5CompositeDataHandle : public TGo4HDF5BasicDataHandle

{

public:

   /** create new data handle*/
   TGo4HDF5CompositeDataHandle(const char *name, size_t datasize);
   virtual ~TGo4HDF5CompositeDataHandle();



   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent) override;

   /** create datasets for writing memory structure of type into file.*/
   void BuildWriteDataset(H5::H5File* file) override;


   /** Read event of sequence number from file*/
   void Read(hsize_t sequencenum, H5::H5File* file) override;

   /** Write event of sequence number from file*/
   void Write(hsize_t sequencenum, H5::H5File* file) override;

};


////////////////////////////////////////////////

/** hdf5 needs structure containing the hvl_t to deal with it correctly*/
struct TGo4HDF5VarContainer
{
  hvl_t fxArray;
};



/**
 * Handle object to access  datasets in hdf5 formatted file with go4 event data
 * implementation for std::vector collections
 *  * @author J. Adamczewski-Musch
 * @since 5/2019
 */
class TGo4HDF5VectorDataHandle : public TGo4HDF5BasicDataHandle
{

public:

   /** create new data handle*/
   TGo4HDF5VectorDataHandle(const char *name, size_t datasize);
   virtual ~TGo4HDF5VectorDataHandle();


   /** remember size of a single element (structure) in this collection, if it is one. Otherwise this is size of this class itself*/
   void SetElementSize(size_t len) {fiElementSize=len;}

    /** define location of corresponding object in memory. This is base pointer for all member specific offsets.*/
   void SetObjectPointer(void* memptr) override;

   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent) override;

   /** create datasets for writing memory structure of type into file.*/
   void BuildWriteDataset(H5::H5File* file) override;

   /** Read event of sequence number from file*/
   void Read(hsize_t sequencenum, H5::H5File* file) override;

   /** Write event of sequence number from file*/
   void Write(hsize_t sequencenum, H5::H5File* ile) override;

protected:


#ifndef __CINT__

   /** collection type with fxType as entries*/

   H5::CompType* fxCollection{nullptr}; //!


     /** handle for variable arrays*/
   TGo4HDF5VarContainer fxVarHandle; //!

#endif


   /** size of the (collection element) structure*/
       size_t fiElementSize{0};//

};


class TGo4HDF5VectorProxy
{
public:

  unsigned long  fx_Begin_ptr;
  unsigned long  fx_End_ptr;
  unsigned long  fx_Cap_ptr;
};



/**
 * Handle object to access  datasets in hdf5 formatted file with go4 event data
 * trial implementation for std::vectors of std::vector collections
 * may not work...
 *  * @author J. Adamczewski-Musch
 * @since 6/2019
 */
class TGo4HDF5SubVectorDataHandle : public TGo4HDF5VectorDataHandle
{

public:

   /** create new data handle*/
   TGo4HDF5SubVectorDataHandle(const char *name, size_t datasize);
   virtual ~TGo4HDF5SubVectorDataHandle();

   /** define location of corresponding object in memory. This is base pointer for all member specific offsets.*/
   void SetObjectPointer(void* memptr) override;

   /** create datasets and buffers for reading this structure from hdf5 file.
    * parent pointer is given for error handling case*/
   void BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent) override;

   /** create datasets for writing memory structure of type into file.*/
   void BuildWriteDataset(H5::H5File* file) override;

   /** Read event of sequence number from file*/
   void Read(hsize_t sequencenum, H5::H5File* file) override;

   /** Write event of sequence number from file*/
    void Write(hsize_t sequencenum, H5::H5File* file) override;

    void SetInnerClassName(const char* nm) {fxInnerClassName=nm;}


protected:

    /** class that is contained in innermost vector*/
      TString fxInnerClassName;

};




#endif //TGO4HDF5DATAHANDLE_H
