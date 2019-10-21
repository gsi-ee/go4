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


#include "TGo4HDF5DataHandle.h"

#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4EventSourceException.h"
#include "TGo4HDF5Source.h"

TGo4HDF5DataHandleFactory TGo4HDF5DataHandle::fxFactory;

 TGo4HDF5DataHandle::TGo4HDF5DataHandle(const char* name, size_t datasize) :  fxTypeName(name),
 fxType(0),fxDataSet(), fxDataSpace(0), fxFileSpace(0), fiEntries(0),
 fxData(0), fiParentOffset(0),fiDataSize(datasize), fiReadOffset(0),
 fxEvent(0), fxParentSource(0), fbDataSetExists(false), fbDataSetActive(false)

 {
   fxType = new H5::CompType(datasize);
   go4hdfdbg("TGo4HDF5DataHandle  DDD created %s for size %ld \n", fxTypeName.Data(), datasize);

 };

 TGo4HDF5DataHandle::~TGo4HDF5DataHandle()
 {
   go4hdfdbg("TGo4HDF5DataHandle  is deleted %s \n", fxTypeName.Data());

   for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
   {
     TGo4HDF5DataHandle* entry= fxSubcomponents[i];
     delete entry;
   }
   fxSubcomponents.clear();
   fxDataSet.close();
   delete fxDataSpace;
   delete fxType;
 }


 void TGo4HDF5DataHandle::InsertTypeMember(const H5std_string& name, size_t offset, const H5::DataType& new_member)
 {
   fxType->insertMember( name, offset, new_member);
   if (fiReadOffset==0) fiReadOffset=offset; // keep location of first real data member
   fbDataSetActive=kTRUE; // only work with handle if it has at least one member assigned to type
 }

 TGo4HDF5DataHandle* TGo4HDF5DataHandle::AddSubMember(const char* name, size_t datasize, const char* collectiontype)
 {
   TGo4HDF5DataHandle* sub= fxFactory.CreateDataHandle(name,datasize,collectiontype);
   fxSubcomponents.push_back(sub);
   return sub;
 }


 TGo4HDF5DataHandle* TGo4HDF5DataHandle::FindSubMember(const char* name)
 {
   if(name==0) return 0;
   //go4hdfdbg("TGo4HDF5DataHandle FindSubMember for name %s\n", name);
   for(unsigned int i=0; i< fxSubcomponents.size(); ++i)
   {
     TGo4HDF5DataHandle* cursor = fxSubcomponents[i];
//     go4hdfdbg("TGo4HDF5DataHandle FindSubMember %d compares with name %s\n",
//              i, cursor ? cursor->GetTypeName() : "EMPTY CURSOR!");
     if(strcmp(cursor->GetTypeName(),name)==0) return cursor;

     // scan recursively name in possible subcomponents. do we need this?
     TGo4HDF5DataHandle* subcursor = cursor->FindSubMember(name);
     if (subcursor) return subcursor;
   }
   return 0;
 }


 void TGo4HDF5DataHandle::SetAllSubMembersActive(Bool_t on)
 {
   for(unsigned int i=0; i< fxSubcomponents.size(); ++i)
      {
        TGo4HDF5DataHandle* cursor = fxSubcomponents[i];
        cursor->SetActive(on);
        cursor->SetAllSubMembersActive(on);
      }
 }


 void TGo4HDF5DataHandle::SetObjectPointer(void* memptr)
{
  fxData = (char*) (memptr) + fiParentOffset;
  go4hdfdbg("TGo4HDF5DataHandle SetObjectPointer for %s with memptr 0x%lx and parentoffset 0x%lx has fxData:0x%lx\n",
      fxTypeName.Data(), (unsigned long) memptr, (unsigned long) fiParentOffset, (unsigned long) fxData);

  // now recursively do it for our components:
  for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
  {
    TGo4HDF5DataHandle* entry = fxSubcomponents[i];
    entry->SetObjectPointer(fxData);
  }

}


 void TGo4HDF5DataHandle::SetTopEvent(TGo4EventElement* eve)
 {
   fxEvent=eve;
   go4hdfdbg(
            "TTTTTTTT TGo4HDF5DataHandle %s  - SetTopEvent sets pointer to %ld \n",
            fxTypeName.Data(), (unsigned long) eve);
   // now recursively do it for our components:
    for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
    {
      TGo4HDF5DataHandle* entry = fxSubcomponents[i];
      entry->SetTopEvent(fxEvent);
    }
 }

 void TGo4HDF5DataHandle::SetTopEventClass(const char* txt)
  {

    fxEventClass=txt;
    //go4hdfdbg("TGo4HDF5DataHandle::SetTopEventClass TTTTTTTT to %s \n",fxEventClass.Data());
    // now recursively do it for our components:
     for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
     {
       TGo4HDF5DataHandle* entry = fxSubcomponents[i];
       entry->SetTopEventClass(txt);
     }
  }


 void TGo4HDF5DataHandle::BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent)
 {
   // just recursively do it for our components:
   for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
     {
       TGo4HDF5DataHandle* entry= fxSubcomponents[i];
       entry->BuildReadDataset(file, parent);
     }
 }



 void TGo4HDF5DataHandle::BuildWriteDataset(H5::H5File* file)
 {
      // just recursively do it for our components:
       for(unsigned int i=0; i<fxSubcomponents.size();++i)
         {
           TGo4HDF5DataHandle* entry= fxSubcomponents[i];
           entry->BuildWriteDataset(file);
         }
       // actual dataset is build in subclass
 }






 void TGo4HDF5DataHandle::Read(hsize_t sequencenum, H5::H5File* file)
 {
   if(sequencenum>=fiEntries)
   {
     go4hdfdatahandlethrow( "TGo4HDF5DataHandle::Read - reached end of dataspace at %ld entries!\n", fiEntries);
   }

   //if(fbDataSetActive && fbDataSetExists)
   if(fbDataSetExists)
   {
   hsize_t offset[1]= {sequencenum};
   hsize_t onerow[1] ={1};
   fxFileSpace.selectHyperslab( H5S_SELECT_SET, onerow, offset ); // one row of event object at last entry (offset is nextsize-1)
         // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
         // the data space in file for each event is of course increasing, so we need to extend the dataset and select
         // a hyperslab to the write destination in the file.
   go4hdfdbg("TGo4HDF5DataHandle::Read of %s has selected hyperslab for onerow:[%ld] offset:[%ld]\n",
               fxTypeName.Data(),(unsigned long) onerow[0], (unsigned long) offset[0]);

   // the actual reading from filespace is implemented in subclass!

   }

   // now recursively do it for our subcomponents :
         for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
           {
             TGo4HDF5DataHandle* entry= fxSubcomponents[i];
             entry->Read(sequencenum, file);
           }







 }

void TGo4HDF5DataHandle::Write(hsize_t sequencenum, H5::H5File* file)
{
  //if(fbDataSetActive && fbDataSetExists)
  if(fbDataSetExists)
  {

    go4hdfdbg("TGo4HDF5DataHandle::Write %s base function will extend dataset for %lld \n", fxTypeName.Data(), sequencenum);
  hsize_t offset[1] = { sequencenum };
  hsize_t nextsize[1] = { sequencenum + 1 };

  fxDataSet.extend(nextsize);

  fxFileSpace = fxDataSet.getSpace();    // dataspace in file
  hsize_t onerow[1] = { 1 };
  fxFileSpace.selectHyperslab(H5S_SELECT_SET, onerow, offset);    // one row of event object at last entry (offset is nextsize-1)
  // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
  // the data space in file for each event is of course increasing, so we need to extend the dataset and select
  // a hyperslab to the write destination in the file.

  // the actual writing to filespace is implemented in subclass!

  }
  // now recursively do it for our subcomponents :
   for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
   {
     TGo4HDF5DataHandle* entry = fxSubcomponents[i];
     entry->Write(sequencenum, file);
   }



}


//////////////////////////////////////////////////////////////////////////////////77


TGo4HDF5BasicDataHandle::TGo4HDF5BasicDataHandle(const char* name, size_t datasize) :
    TGo4HDF5DataHandle(name, datasize), fxReadBuffer(0)

 {
  go4hdfdbg("TGo4HDF5BasicDataHandle ctor\n");
 };


TGo4HDF5BasicDataHandle::~TGo4HDF5BasicDataHandle()
 {
   go4hdfdbg("TGo4HDF5BasicDataHandle  is deleted %s \n", fxTypeName.Data());
 }


void TGo4HDF5BasicDataHandle::AllocReadBuffer(size_t size)
 {
   if(fxReadBuffer && (size == fiDataSize)) return;
   delete fxReadBuffer;
   fxReadBuffer= new Char_t[size];
   fiDataSize=size;
   go4hdfdbg("TGo4HDF5DataHandle: AllocReadBuffer has created read buffer 0x%lx for  size %ld \n",
          (unsigned long) fxReadBuffer, fiDataSize);
 }



 void TGo4HDF5BasicDataHandle::BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent)
 {
   if (!fbDataSetExists)// && fbDataSetActive)
    {

   fxParentSource=parent;

     Int_t rank=1; // entries of the ntuple/tree
     hsize_t      dims[1]  = { 1};  // dataset dimensions at creation
     hsize_t      maxdims[1] = {H5S_UNLIMITED}; // 1 ? only one entry maps to current event object -
     fxDataSpace= new H5::DataSpace(rank, dims, maxdims);

     // dataspace in memory covers only the current input event structure
     hsize_t onerow[1] ={1};
     hsize_t offset[1]= {0};
     fxDataSpace->selectHyperslab( H5S_SELECT_SET, onerow, offset );




     // get existing dataset from file and compare data with type of the target event
     // we only allow datasets that match the memory structure concerning member names etc,

     // NOTE that for reading back data, we first have to figure out our subcomponents from the file dataspace by name!
     // TODO: when reading, scan all datasets in file
     // for composite events, configgure the input event according the information in hdf5 file ????
     // this means BuildDataType has to be done after we have scanned the input file and reconstructed the substructure setup
     // not so easy, so constraint is currently that all substructures in code must match the status when hdf5 file was recorded

     go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet will open set %s\n", fxTypeName.Data());
     fxDataSet = file->openDataSet(fxTypeName.Data());

     H5T_class_t type_class = fxDataSet.getTypeClass();
     H5::CompType intype;
     if( type_class == H5T_COMPOUND)
     {
        intype= fxDataSet.getCompType();
  //      size_t size = intype.getSize();
        go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet finds in file compound type of size %ld \n",  intype.getSize());
     }
     else if( type_class == H5T_VLEN)
       {
           H5::VarLenType vtype = fxDataSet.getVarLenType();
           H5::DataType entrytype = vtype.getSuper();
           //H5::DataType* entrytype = vtype.decode();
           //size_t size = entrytype.getSize();
           //size_t vsize = vtype.getSize();
           H5T_class_t entry_class =  entrytype.getClass();
           go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet finds in file vlen type of size %ld, with entry class %d , size %ld \n",
               vtype.getSize(), entry_class, entrytype.getSize());
           if( entry_class == H5T_COMPOUND)
             {

               H5::DataType* ptype=&entrytype;
               H5::CompType* entrycomp = static_cast<H5::CompType*>(ptype); // dynamic cast fails here?
               if (entrycomp){
                   intype = *entrycomp;
                   go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet uses compound entry of vlen dtype %d \n", entry_class );
               }
               else
               {
                 go4hdfdatahandlethrow("TGo4HDF5DataHandle: BuildReadDataSet - could not assign component entry of varlen data type !\n");
              }
             }
           // TODO: treat vectors with simple data members here?
           else
           {
             go4hdfdatahandlethrow( "TGo4HDF5DataHandle: BuildReadDataSet - varlen data type without compound, but contains class %d!\n", entry_class);
           }
       }
     else
     {
       go4hdfdatahandlethrow("TGo4HDF5DataHandle: BuildReadDataSet - file contains no compound or varlen data type, but type %d!\n",type_class)
     }


     fxFileSpace = fxDataSet.getSpace();
     int frank = fxFileSpace.getSimpleExtentNdims();
     hsize_t dims_out[frank];
     fxFileSpace.getSimpleExtentDims( dims_out, NULL);
     go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet file dataspace has rank %d, dimensions:%d, #1. dimension=%ld \n",
         frank ,
         fxFileSpace.getSimpleExtentDims( dims_out, NULL),
         (unsigned long)(dims_out[0]));
     fiEntries=dims_out[0]; // events filled in this file, for leaving the read loop later.

  #ifdef GO4HDF5_DEBUG
     // check consistency of file dataset with input event structure:
     int nfilemembers= intype.getNmembers ();
     int neventmembers= fxType->getNmembers ();


     go4hdfdbg("Type comparison: number of compound members: file  %d, event %d\n",nfilemembers, neventmembers);

     go4hdfdbg("Dump Type from file:\n");
         for (int i = 0; i < nfilemembers; ++i)
         {
           H5::DataType mem_type=intype.getMemberDataType(i);
           go4hdfdbg("\t %d: name:%s class:%s", i, intype.getMemberName(i).c_str(), mem_type.fromClass().c_str());
           H5T_class_t mem_class = intype.getMemberClass(i);
           if (mem_class == H5T_ARRAY)
           {
             H5::ArrayType artyp = intype.getMemberArrayType(i);
             int ardim = artyp.getArrayNDims();
             hsize_t dims[ardim];
             artyp.getArrayDims(dims);
             go4hdfdbg(" array dimensions:%d [", ardim);
             for (int j = 0; j < ardim; ++j)
             {
               go4hdfdbg("%d", (int) dims[j]);
             }
             go4hdfdbg("]");
           }
           go4hdfdbg("\n");
         }
         go4hdfdbg("Dump Type from event structure:\n");
             for (int i = 0; i < neventmembers; ++i)
             {
               H5::DataType mem_type=fxType->getMemberDataType(i);
               go4hdfdbg("\t %d: name:%s class:%s", i, intype.getMemberName(i).c_str(), mem_type.fromClass().c_str());
               H5T_class_t mem_class = fxType->getMemberClass(i);
               if (mem_class == H5T_ARRAY)
               {
                 H5::ArrayType artyp = fxType->getMemberArrayType(i);
                 int ardim = artyp.getArrayNDims();
                 hsize_t dims[ardim];
                 artyp.getArrayDims(dims);
                 go4hdfdbg("array dimensions:%d [", ardim);
                 for (int j = 0; j < ardim; ++j)
                 {
                   go4hdfdbg("%lu", (unsigned long) dims[j]);
                 }
                 go4hdfdbg("]");
               }
               go4hdfdbg("\n");
             }

     #endif

     if(!(*fxType==intype))
     {
       TString msg=   TString::Format( "BuildReadDataSet type mismatch between event structure(%s) and input from file (%s)\n",
           fxType->fromClass().c_str(), intype.fromClass().c_str());
         TGo4Log::Error("TGo4HDF5DataHandle: %s", msg.Data());
        parent->SetErrMess(msg.Data());
         throw TGo4EventSourceException(parent);

     }

     AllocReadBuffer(fiDataSize);

    fbDataSetExists=kTRUE;

    }

   TGo4HDF5DataHandle::BuildReadDataset(file, parent);

 }


 void TGo4HDF5BasicDataHandle::BuildWriteDataset(H5::H5File* file)
{

  if (!fbDataSetExists  && fbDataSetActive)
  {
    go4hdfdbg("TGo4HDF5BasicDataHandle: BuildWriteDataset  for event set %s\n", fxTypeName.Data());
    Int_t rank = 1;    // entries of the ntuple/tree
    hsize_t dims[1] = { 1 };    // dataset dimensions at creation
    hsize_t maxdims[1] = { H5S_UNLIMITED };
    fxDataSpace = new H5::DataSpace(rank, dims, maxdims);

    H5::DSetCreatPropList cparms;
    hsize_t chunk_dims[1] = { 1 };
    cparms.setChunk(rank, chunk_dims);
    cparms.setFillValue(*fxType, fxData);

    fxDataSet = file->createDataSet(fxTypeName.Data(), *fxType, *fxDataSpace, cparms);
    fbDataSetExists = kTRUE;
  }
  TGo4HDF5DataHandle::BuildWriteDataset(file);

}






 void TGo4HDF5BasicDataHandle::Read(hsize_t sequencenum, H5::H5File* file)
 {
   TGo4HDF5DataHandle::Read(sequencenum, file); // prepare dataspace and treat subcomponents
   if(!fbDataSetActive) return;

     // we must not use actual eventclass directly,
     // because hdf5 will destroy memory of not registered data members like pointer to eventsource and class info!
     void* data= (void*)(fxReadBuffer);

             // printf("TGo4HDF5Source: BuildEvent before read on data pointer 0x%lx \n",data);

     fxDataSet.read( data, *fxType, *fxDataSpace, fxFileSpace );


              // here copy data from our hdf5 bounce buffer to actual event object:
              Char_t* target = (Char_t*) (fxData) + fiReadOffset;
              Char_t* source = (Char_t*) (fxReadBuffer) + fiReadOffset;
              size_t copylen = fiDataSize - fiReadOffset;
              memcpy (target, source, copylen);

     #ifdef GO4HDF5_DEBUG
              printf("Copy %ld bytes from %lx to %lx, fiReadOffset=%ld, fiDataSize=%ld \n",
              copylen, (long) source, (long)target, fiReadOffset, fiDataSize);
     #endif

 }

void TGo4HDF5BasicDataHandle::Write(hsize_t sequencenum, H5::H5File* file)
{
   TGo4HDF5DataHandle::Write(sequencenum, file); // treat subcomponents here, extend dataset

   if(!fbDataSetActive) return;
   // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
   // the data space in file for each event is of course increasing, so we need to extend the dataset and select
   // a hyperslab to the write destination in the file.
   go4hdfdbg("TGo4HDF5BasicDataHandle::Write %s plain\n", fxTypeName.Data());
   fxDataSet.write(fxData, *fxType, *fxDataSpace, fxFileSpace);

}






////////////////////////////////////////////////


TGo4HDF5CompositeDataHandle::TGo4HDF5CompositeDataHandle(const char* name, size_t datasize) :
    TGo4HDF5BasicDataHandle(name, datasize)

 {
  go4hdfdbg("TGo4HDF5CompositeDataHandle ctor use go4 composite event component\n");
 };


TGo4HDF5CompositeDataHandle::~TGo4HDF5CompositeDataHandle()
 {
   go4hdfdbg("TGo4HDF5CompositeDataHandle  is deleted %s \n", fxTypeName.Data());
 }




 void TGo4HDF5CompositeDataHandle::BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent)
 {
   TGo4HDF5BasicDataHandle::BuildReadDataset(file, parent);
   // NOTE: currently we can not reconstruct the setup of the composite event from the information in hdf5 file
   // we just rely that the user code has set up the input event accordingly and check if the types are consistent
   // with information from hdf5 input
   // later TODO?: evaluate substructure of composite event and dynamically create components of known class
   // <- interpreter business. For the moment, do not follow this approach.

 }


 void TGo4HDF5CompositeDataHandle::BuildWriteDataset(H5::H5File* file)
 {
      // same as plain set here. do we need different treatment?

     TGo4HDF5BasicDataHandle::BuildWriteDataset(file);

 }






 void TGo4HDF5CompositeDataHandle::Read(hsize_t sequencenum, H5::H5File* file)
 {
   TGo4HDF5BasicDataHandle::Read(sequencenum, file);



 }

void TGo4HDF5CompositeDataHandle::Write(hsize_t sequencenum, H5::H5File* file)
{
  TGo4HDF5BasicDataHandle::Write(sequencenum, file);



}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////



TGo4HDF5VectorDataHandle::TGo4HDF5VectorDataHandle(const char* name, size_t datasize) :
    TGo4HDF5BasicDataHandle(name, datasize), fxCollection(0), fiElementSize(0)

 {
  go4hdfdbg("TGo4HDF5VectorDataHandle ctor \n");



               // need to create comp type with hvl_t handle here!
               //fxCollection = new H5::VarLenType(fxType); // this does not work, file is corrupt afterwards JAM 27-may-2019
  fxCollection = new H5::CompType(sizeof(TGo4HDF5VarContainer));

  fxCollection -> insertMember( fxTypeName.Data(), HOFFSET(TGo4HDF5VarContainer, fxArray), H5::VarLenType(fxType));

  go4hdfdbg("TGo4HDF5VectorDataHandle ctor, typename is %s\n",
                   fxTypeName.Data());
 }






TGo4HDF5VectorDataHandle::~TGo4HDF5VectorDataHandle()
 {
   go4hdfdbg("TGo4HDF5VectorDataHandle  is deleted %s \n", fxTypeName.Data());
   delete fxCollection;
 }



void TGo4HDF5VectorDataHandle::SetObjectPointer(void* memptr)
{

  TGo4HDF5DataHandle::SetObjectPointer(memptr);

    // depends on vector implementation! following is probably not true:
    // struct std::vector<T>
    //  {
    //      size_t size;
    //      size_t cap;
    //      T* buffer;
    //  }
    //
    // maybe std::vector member in classlooks like this?
    //                         struct vector {    // Simple C struct as example (T is the type supplied by the template)
    //                           T *begin;        // vector::begin() probably returns this value
    //                           T *end;          // vector::end() probably returns this value
    //                           T *end_capacity; // First non-valid address
    //                           // Allocator state might be stored here (most allocators are stateless)
    //                         };

    char** p_collection = (char**) fxData;
    char** p_begin_ptr = p_collection;
    char** p_end_ptr = p_collection + 1;
    //char** p_cap_ptr = p_collection + 2;

    go4hdfdbg("TGo4HDF5VectorDataHandle SetObjectPointer has fxData=0x%lx elementsize=%ld\n",
        (unsigned long) fxData, fiElementSize);

    size_t len = (*p_end_ptr - *p_begin_ptr) / fiElementSize;
    //size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;

    fxVarHandle.fxArray.p = *p_begin_ptr;
    fxVarHandle.fxArray.len = len;

//    SetActive(((fxVarHandle.fxArray.p!=0) &&  (fxVarHandle.fxArray.len>0)) ? true : false); // disable writing out empty vector

#ifdef GO4HDF5_DEBUG
    static unsigned debugcount = 0;
    char** p_cap_ptr = p_collection + 2;
    size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;
    if (debugcount < 10000)
    {
      go4hdfdbg(
          "TGo4HDF5VectorDataHandle SetObjectPointer for count %d has collection size: %ld capacity: %ld pointer to buffer 0x%lx, ACTIVE=%d\n",
          debugcount, len, cap, (unsigned long) *p_begin_ptr, fbDataSetActive);
      debugcount++;
    }

#endif

//    printf(
//             "TGo4HDF5VectorDataHandle %s - SetObjectPointer  has collection size: %ld capacity: %ld pointer to buffer 0x%lx, fxData=0x%x\n",
//             fxTypeName.Data(), len, cap, (unsigned long) *p_begin_ptr, fxData);

}


 void TGo4HDF5VectorDataHandle::BuildReadDataset(H5::H5File* file, TGo4HDF5Source* parent)
 {
   TGo4HDF5BasicDataHandle::BuildReadDataset(file, parent);

 }


 void TGo4HDF5VectorDataHandle::BuildWriteDataset(H5::H5File* file)
 {
   if(!fbDataSetExists) // && fbDataSetActive)
   {
      go4hdfdbg("TGo4HDF5VectorDataHandle: BuildWriteDataset  for collection set %s \n",fxTypeName.Data());

       Int_t rank=1; // entries of the ntuple/tree. vector dimension is handled by variable size type?
       hsize_t      dims[1]  = { 1};  // dataset dimensions at creation
       hsize_t      maxdims[1] = {H5S_UNLIMITED};
       fxDataSpace= new H5::DataSpace(rank, dims, maxdims);
       H5::DSetCreatPropList cparms;
       hsize_t      chunk_dims[1] ={1};
       cparms.setChunk( rank, chunk_dims );

       cparms.setFillValue(H5::VarLenType(fxType), &fxVarHandle.fxArray);
       fxDataSet =  file->createDataSet( fxTypeName.Data(), H5::VarLenType(fxType), *fxDataSpace, cparms);

       fbDataSetExists=kTRUE;
   }
   TGo4HDF5DataHandle::BuildWriteDataset(file);



 }






 void TGo4HDF5VectorDataHandle::Read(hsize_t sequencenum, H5::H5File* file)
 {
   TGo4HDF5DataHandle::Read(sequencenum, file); // prepare dataspaces and handle subcompoments
   if(!fbDataSetActive) return;
   go4hdfdbg("TGo4HDF5VectorDataHandle::Read %s collection \n", fxTypeName.Data());

          // TODO: set varhandle to buffer - setobjectpointer has done this to actual vector in event object
          // we need to resize this before we can directly fill it!
//          size_t vsize =  fxDataSet.getVarLenType().getSize(); // this is just size in bytes, not #entries
//          go4hdfdbg("TGo4HDF5VectorDataHandle::Read FFFF tells us size of vlen type size is now %ld \n",
//              vsize);

          // TODO: how to get from hdf5 length of vector _before_ reading it!!!?
          // not necessary, since hdf5 will do allocation itself at hvl_t.p
//          int numentries = 5; //? get from filespace ?
//          size_t currentsize= fiElementSize * numentries;
//          AllocReadBuffer(currentsize);
///////////////////////////


          fxVarHandle.fxArray.p=0; //note: read will allocate the necessary buffer memory automatically!
          fxDataSet.read(&fxVarHandle.fxArray, H5::VarLenType(fxType), *fxDataSpace, fxFileSpace);
          go4hdfdbg("TGo4HDF5VectorDataHandle::Read LLLL length after reading is %ld, buffer is 0x%lx \n",
              fxVarHandle.fxArray.len,
              (unsigned long) fxVarHandle.fxArray.p);


/////////////// First attempt: resize vector via interpreter and do memcpy from read buffer
          // build interpreter command to resize our vector using handle on top event:
//          TString resizecom = TString::Format("%s * myevent= (%s *) %ld; myevent-> %s.resize(%ld)",
//              fxEventClass.Data(), fxEventClass.Data(), (unsigned long) fxEvent, fxMemberHandle.Data(), fxVarHandle.fxArray.len);
//
//
//
//
////          TString resizecom = TString::Format("%s.resize(%ld)", fxMemberHandle.Data(),fxVarHandle.fxArray.len);
//          go4hdfdbg("TGo4HDF5VectorDataHandle::Read  will use command: %s \n", resizecom.Data());
//          gROOT->ProcessLineFast(resizecom.Data());
//
//          char** p_collection = (char**) fxData;
//          char** p_begin_ptr = p_collection;
//          char** p_end_ptr = p_collection + 1;
//          char** p_cap_ptr = p_collection + 2;
//
//          size_t len = (*p_end_ptr - *p_begin_ptr) / fiElementSize;
//          size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;
//
//          go4hdfdbg(
//                   "TGo4HDF5VectorDataHandle::Read has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
//                   len, cap, (unsigned long) *p_begin_ptr, (unsigned long) *p_end_ptr, (unsigned long) *p_cap_ptr);
//          // here copy data from our hdf5 bounce buffer to actual event object:
//          Char_t* target = *p_begin_ptr;
//          Char_t* source = (Char_t*) fxVarHandle.fxArray.p; //(Char_t*) (fxReadBuffer);
//          size_t copylen = fxVarHandle.fxArray.len * fiElementSize;
//          go4hdfdbg(
//                  "TGo4HDF5VectorDataHandle::Read before memcopy from 0x%x to 0x%x with length %ld (element size is :%ld)\n",
//                           (unsigned long) source, (unsigned long) target, copylen, fiElementSize);
//
//
//          if(fxVarHandle.fxArray.len >0){
//            memcpy (target, source, copylen);
//
//            go4hdfdbg(
//                 "TGo4HDF5VectorDataHandle::Read AFTER COPY has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
//                 len, cap, (unsigned long) *p_begin_ptr, (unsigned long) *p_end_ptr, (unsigned long) *p_cap_ptr);
//
//          }
// NOTE: after doing this copy, the next clear on this vector will crash!
////////////////////////////////////////////////////////////////////////////

////////////// alternative approach: use std::vector methods via cint here:
          //dataVec.insert(dataVec.end(), &dataArray[0], &dataArray[dataArraySize]);

          if(fxVarHandle.fxArray.len >0)
          {
          Char_t* first =(Char_t*) fxVarHandle.fxArray.p;
          Char_t* last = (Char_t*) fxVarHandle.fxArray.p + (fxVarHandle.fxArray.len) * fiElementSize;

          TString vectorhandle = TString::Format("myevent->%s",fxMemberHandle.Data());
          TString copycom = TString::Format("%s * myevent= (%s *) %ld; %s.insert(%s.end(), (%s *) %ld, (%s *) %ld)",
              fxEventClass.Data(), fxEventClass.Data(), (unsigned long) fxEvent, vectorhandle.Data(), vectorhandle.Data(),
              fxMemberClass.Data(), (unsigned long) first, fxMemberClass.Data(),(unsigned long) last);

          go4hdfdbg("TGo4HDF5VectorDataHandle::Read  will use command: %s \n", copycom.Data());
                  gROOT->ProcessLineFast(copycom.Data());

         // now check if vector has changed:
#ifdef GO4HDF5_DEBUG

          char** p_collection = (char**) fxData;
          char** p_begin_ptr = p_collection;
          char** p_end_ptr = p_collection + 1;
          char** p_cap_ptr = p_collection + 2;

          size_t len = (*p_end_ptr - *p_begin_ptr) / fiElementSize;
          size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;

          go4hdfdbg(
              "TGo4HDF5VectorDataHandle::Read after INSERT has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
              len, cap, (unsigned long) *p_begin_ptr, (unsigned long) *p_end_ptr, (unsigned long) *p_cap_ptr);
#endif

          }

////////////// end alternative approach with insert.

 }




void TGo4HDF5VectorDataHandle::Write(hsize_t sequencenum, H5::H5File* file)
{
  SetActive(((fxVarHandle.fxArray.p!=0) &&  (fxVarHandle.fxArray.len>0)) ? true : false); // disable writing out empty vector

  BuildWriteDataset(file); // dataset is no sooner created than we have really data to write in vector
  TGo4HDF5DataHandle::Write(sequencenum, file);

  if(fbDataSetActive)
  {
       //printf("TGo4HDF5VectorDataHandle::Write %s collection \n", fxTypeName.Data());
       go4hdfdbg("TGo4HDF5VectorDataHandle::Write %s collection \n", fxTypeName.Data());
       //TGo4HDF5DataHandle::Write(sequencenum, file);
       fxDataSet.write(&fxVarHandle.fxArray, H5::VarLenType(fxType), *fxDataSpace, fxFileSpace);
  //       fxDataSet.write(fxVarHandle.fxArray.p, *fxType, *fxDataSpace, fspace); // test if we can write at lease first element in array? yes!
     }
  else
     {
       go4hdfdbg("TGo4HDF5VectorDataHandle::Write DO NOT WRITE inactive %s collection\n", fxTypeName.Data());
     }

}






//////////////////////////////////////////////////////////////////////////////////7
///////////////////////////////////////////////////////////////////////////////////


// this handle is a collection of vector elements with subelements
TGo4HDF5SubVectorDataHandle::TGo4HDF5SubVectorDataHandle(const char* name, size_t datasize) :
    TGo4HDF5VectorDataHandle(name, datasize)

 {
  go4hdfdbg("TGo4HDF5SubVectorDataHandle ctor \n");


 }






TGo4HDF5SubVectorDataHandle::~TGo4HDF5SubVectorDataHandle()
 {
   go4hdfdbg("TGo4HDF5SubVectorDataHandle  is deleted %s \n", fxTypeName.Data());
 }



void TGo4HDF5SubVectorDataHandle::SetObjectPointer(void* memptr)
{

  TGo4HDF5VectorDataHandle::SetObjectPointer(memptr);


    // still need to update recursively all vectors in this container

  TDataType datatype(fxInnerClassName.Data());

  size_t innersize=datatype.Size();

  //  need to find out which of the existing subhandles should not be written, because
  // the corresponding entries in subervector are not exisiting for the current event.
  SetAllSubMembersActive(false); // first disable all subcomponents if existing


  for(unsigned int ix=0; ix< fxVarHandle.fxArray.len; ++ix)
  {
    char* cursor = (char*)(fxVarHandle.fxArray.p) + ix * fiElementSize;
    //TGo4HDF5VectorProxy* prox = (TGo4HDF5VectorProxy*) cursor;

    TString fullname= TString::Format("%s[%d]", GetTypeName(), ix ); // full name of dataset
    TString membername = TString::Format("%s[%d]",fxMemberHandle.Data(),ix); // local name to access member functions
    // before adding it, look if we already have one TODO!
    go4hdfdbg(
              "TGo4HDF5SubVectorDataHandle SetObjectPointer updating element %s , inner class is %s\n",
              fullname.Data(), fxInnerClassName.Data());

//    printf(
//                 "TGo4HDF5SubVectorDataHandle SetObjectPointer updating element %s , inner class is %s\n",
//                 fullname.Data(), fxInnerClassName.Data());

    TString memberclass = TString::Format("vector<%s>",fxInnerClassName.Data());
    TString searchname = TString::Format("%s[%s]",fullname.Data(), memberclass.Data());
    TGo4HDF5DataHandle* subhandle =FindSubMember(searchname.Data());
    if(subhandle)
    {
      go4hdfdbg(
                   "TGo4HDF5SubVectorDataHandle SetObjectPointer finds element %s , already in subcomponents\n",
                   searchname.Data());
      subhandle->SetActive(true);
    }
    else
    {
      go4hdfdbg(
                        "TGo4HDF5SubVectorDataHandle SetObjectPointer creates new subcomponent %s  \n",
                        fullname.Data());
      //TString memberclass = TString::Format("vector<%s>",fxInnerClassName.Data());
      subhandle = AddSubMember(fullname.Data(), innersize, memberclass.Data());
      subhandle->SetMemberName(membername);
      subhandle->SetMemberClass(fxInnerClassName.Data());

      TGo4HDF5Adapter::FillTypeInfo(subhandle, fullname.Data(), fxInnerClassName.Data());
      // newly created subcomponents will be set active at lazy creation of dataset later (when write or read is called).
      subhandle->SetActive(true);
    }

    void* super=Data(); // everything is relative to immediate mother element
    // pointer to the subvector is content of cursor:

//    char** psubvector = (char**) cursor;
//    char* subvector = *psubvector;
//       size_t delta= (char*) subvector - (char*) super;


#ifdef GO4HDF5_DEBUG
       char* subvector = cursor;
#endif       
       size_t delta= (char*) cursor - (char*) super;
       go4hdfdbg("TGo4HDF5SubVectorDataHandle SetObjectPointer sets parent offset %ld (subvector:0x%lx, super:0x%lx) cursor was 0x%lx\n",
               delta, (unsigned long) subvector, (unsigned long) super, (unsigned long) cursor);

     subhandle->SetParentOffset(delta);
     subhandle->SetObjectPointer(super); // will internally set active only if subvector has real data

     //subhandle->SetActive(false); // JAMDEBUG: disable all writing of subsubvectors

  } // for(unsigned int ix=0; ix< fxVarHandle.fxArray.len; ++ix)


}


 void TGo4HDF5SubVectorDataHandle::BuildReadDataset(H5::H5File* file, TGo4HDF5Source* parent)
 {
   TGo4HDF5DataHandle::BuildReadDataset(file, parent);
   SetActive(false);
 }


 void TGo4HDF5SubVectorDataHandle::BuildWriteDataset(H5::H5File* file)
 {
   // do nothing here until dataset is initalized lazy after set object pointer is done!
   // TGo4HDF5DataHandle::BuildWriteDataset(file); // recursively do it for all subcomponents
      // we do not build an own dataset to store the vector of vector in the file here.
      // the actual data is stored in our subcomponents (i.e. regular TGo4HDF5VectorDataHandle objects)

      go4hdfdbg("TGo4HDF5SubVectorDataHandle: BuildWriteDataset  NOP for collection set %s \n",fxTypeName.Data());
      SetActive(false); // mark to skip writing the vector of vector container itself.


 }






 void TGo4HDF5SubVectorDataHandle::Read(hsize_t sequencenum, H5::H5File* file)
 {
   // TODO here: for a consistent read back we would need to evaluate which of the subvectors are
   // filled in the file for each entry.


   TGo4HDF5DataHandle::BuildReadDataset(file, fxParentSource);
   TGo4HDF5DataHandle::Read(sequencenum, file);  // recursively do it for all subcomponents
   // we do not read the vector of vector from file here.
   // the actual data is read into our subcomponents (i.e. regular TGo4HDF5VectorDataHandle objects)

   go4hdfdbg("TGo4HDF5SubVectorDataHandle::Read %s collection \n", fxTypeName.Data());




 }




void TGo4HDF5SubVectorDataHandle::Write(hsize_t sequencenum, H5::H5File* file)
{
  // need to dynamically setup new write datsets for variable subcomponents like vectors of vectors
  //TGo4HDF5DataHandle::BuildWriteDataset(file);
  TGo4HDF5DataHandle::Write(sequencenum, file);


}














////////////////////////////////////////////////////////


TGo4HDF5DataHandle* TGo4HDF5DataHandleFactory::CreateDataHandle(const char* name, size_t datasize, const char* type)
{
  TGo4HDF5DataHandle* rev = 0;

  if (strstr(type, "vector<vector") != 0)
  {
    // add collectionclass to our datahandle name:
    TString theName(name);
    TString theCollection(type);
    theName += "[";
    theName = theName.Append(theCollection);
    theName += "]";
    TGo4HDF5SubVectorDataHandle* vrev = new TGo4HDF5SubVectorDataHandle(theName.Data(), datasize);
    vrev->SetElementSize(datasize);
    TObjArray* tokens = theCollection.Tokenize("<");
    if (tokens)
    {
      TObjString* part = dynamic_cast<TObjString*>(tokens->At(2));
      if (part)
      {
        TString rtype = part->GetString();
        TObjArray* stokens = rtype.Tokenize(">");
        if (stokens)
        {
          TObjString* spart = dynamic_cast<TObjString*>(stokens->At(0));
          TString valuetype = spart->GetString();
          vrev->SetInnerClassName(valuetype.Data());
          go4hdfdbg("TGo4HDF5DataHandleFactory::CreateDataHandle for %s sets inner class name to %s\n", type,
              valuetype.Data());
          delete stokens;
        }
        else
        {
          go4hdfdbg("TGo4HDF5DataHandleFactory::CreateDataHandle for %s could not evaluate valuetype from %ss\n", type,
              theCollection.Data());
        }
      }
      else
      {
        go4hdfdbg("TGo4HDF5DataHandleFactory::CreateDataHandle for %s could not evaluate valuetype from %ss\n", type,
            theCollection.Data());
      }
      delete tokens;
    }
    rev = vrev;
  }
  else if (strstr(type, "vector") != 0)
  {
    // add collectionclass to our datahandle name:
    TString theName(name);
    TString theCollection(type);
    theName += "[";
    theName = theName.Append(theCollection);
    theName += "]";
    TGo4HDF5VectorDataHandle* vrev = new TGo4HDF5VectorDataHandle(theName.Data(), datasize);
    vrev->SetElementSize(datasize);
    rev = vrev;
  }
  else if (strstr(type, "Go4Comp") != 0)
  {
    rev = new TGo4HDF5CompositeDataHandle(name, datasize);
    rev->SetActive(true);
  }
  else
  {
    rev = new TGo4HDF5DataHandle(name, datasize);
    rev->SetActive(true);
  }

  return rev;
}




