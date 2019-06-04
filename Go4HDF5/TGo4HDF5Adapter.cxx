// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------


#include "TGo4HDF5Adapter.h"

#include "TFolder.h"
#include "TFile.h"
#include "TTree.h"
#include "TDataMember.h"
#include "TVirtualCollectionProxy.h"
#include "TBaseClass.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4EventSourceException.h"
#include "TGo4HDF5Source.h"


 TGo4HDF5DataHandle::TGo4HDF5DataHandle(const char* name, size_t datasize, const char* collectiontype) : fxTypeName(name),
 fxType(0),fxCollection(0), fxDataSet(), fxDataSpace(0), fxFileSpace(0), fiEntries(0),
 fxData(0), fiParentOffset(0), fiElementSize(0), fxReadBuffer(0), fiDataSize(datasize), fiReadOffset(0),
 fiCollectionType(GO4HDF5_COLL_NONE), fxEvent(0)

 {
   fxType = new H5::CompType(datasize);
   if(collectiontype)
       {
         if(strstr(collectiontype,"vector")!=0){
             fiCollectionType=GO4HDF5_COLL_STDVEC;

             // add collectionclass to our name:sssssssssss
             TString theCollection(collectiontype);
             fxTypeName+="[";
             fxTypeName=fxTypeName.Append(theCollection);
             fxTypeName+="]";
             // need to create comp type with hvl_t handle here!
             //fxCollection = new H5::VarLenType(fxType); // this does not work, file is corrupt afterwards JAM 27-may-2019
             fxCollection = new H5::CompType(sizeof(TGo4HDF5VarContainer));

             fxCollection -> insertMember( fxTypeName.Data(), HOFFSET(TGo4HDF5VarContainer, fxArray), H5::VarLenType(fxType));
             go4hdfdbg("TGo4HDF5DataHandle ctor for collection type %s, use std::vector\n", collectiontype);
         }
         // TODO: parse other collections here.
       }

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
   delete fxCollection;
   delete fxType;
 }


 void TGo4HDF5DataHandle::InsertTypeMember(const H5std_string& name, size_t offset, const H5::DataType& new_member)
 {
   fxType->insertMember( name, offset, new_member);
   if (fiReadOffset==0) fiReadOffset=offset; // keep location of first real data member
 }

 TGo4HDF5DataHandle* TGo4HDF5DataHandle::AddSubMember(const char* name, size_t datasize, const char* collectiontype)
 {
   TGo4HDF5DataHandle* sub= new TGo4HDF5DataHandle(name, datasize, collectiontype);
   fxSubcomponents.push_back(sub);
   return sub;
 }


 void TGo4HDF5DataHandle::SetObjectPointer(void* memptr)
{
  fxData = (char*) (memptr) + fiParentOffset;

  // in case of collection/vector, evaluate the real location of it and update variable lenght stubs:
  if (fiCollectionType == GO4HDF5_COLL_STDVEC)
  {

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

    size_t len = (*p_end_ptr - *p_begin_ptr) / fiElementSize;
    //size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;

    fxVarHandle.fxArray.p = *p_begin_ptr;
    fxVarHandle.fxArray.len = len;

#ifdef GO4HDF5_DEBUG
    static unsigned debugcount = 0;
    char** p_cap_ptr = p_collection + 2;
    size_t cap = (*p_cap_ptr - *p_begin_ptr) / fiElementSize;
    if (debugcount < 10)
    {
      go4hdfdbg(
          "TGo4HDF5DataHandle SetObjectPointer for event %d has collection size: %ld capacity: %ld pointer to buffer 0x%lx\n",
          debugcount, len, cap, (unsigned long) *p_begin_ptr);
      debugcount++;
    }

#endif

  }    //GO4HDF5_COLL_STDVEC

  // now recursively do it for our components:
  for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
  {
    TGo4HDF5DataHandle* entry = fxSubcomponents[i];
    entry->SetObjectPointer(fxData);
  }

}
;
 void TGo4HDF5DataHandle::SetTopEvent(TGo4EventElement* eve)
 {
   fxEvent=eve;

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
    go4hdfdbg("TGo4HDF5DataHandle::SetTopEventClass TTTTTTTT to %s \n",fxEventClass.Data());
    // now recursively do it for our components:
     for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
     {
       TGo4HDF5DataHandle* entry = fxSubcomponents[i];
       entry->SetTopEventClass(txt);
     }
  }


 void TGo4HDF5DataHandle::BuildReadDataset(H5::H5File*file, TGo4HDF5Source* parent)
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

   fxDataSet = file->openDataSet(fxTypeName.Data());

   H5T_class_t type_class = fxDataSet.getTypeClass();
   H5::CompType intype;
   if( type_class == H5T_COMPOUND)
   {
      intype= fxDataSet.getCompType();
//      size_t size = intype.getSize();
      go4hdfdbg("TGo4HDF5DataHandle::BuildDataSet finds in file compound type of size %ld \n",  intype.getSize());
   }
   else if( type_class == H5T_VLEN)
     {
         H5::VarLenType vtype = fxDataSet.getVarLenType();
         H5::DataType entrytype = vtype.getSuper();
         //H5::DataType* entrytype = vtype.decode();
         //size_t size = entrytype.getSize();
         //size_t vsize = vtype.getSize();
         H5T_class_t entry_class =  entrytype.getClass();
         go4hdfdbg("TGo4HDF5DataHandle::BuildDataSet finds in file vlen type of size %ld, with entry class %d , size %ld \n",
             vtype.getSize(), entry_class, entrytype.getSize());
         if( entry_class == H5T_COMPOUND)
           {

             H5::DataType* ptype=&entrytype;
             H5::CompType* entrycomp = static_cast<H5::CompType*>(ptype); // dynamic cast fails here?
             if (entrycomp){
                 intype = *entrycomp;
                 go4hdfdbg("TGo4HDF5DataHandle::BuildDataSet uses compound entry of vlen dtype %d \n", entry_class );
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
   int ndims = fxFileSpace.getSimpleExtentDims( dims_out, NULL);
   go4hdfdbg("TGo4HDF5DataHandle::BuildReadDataSet file dataspace has rank %d, dimensions:%d, #1. dimension=%ld \n", frank , ndims,
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

   // now recursively do it for our components:
   for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
     {
       TGo4HDF5DataHandle* entry= fxSubcomponents[i];
       entry->BuildReadDataset(file, parent);
     }



 }


 void TGo4HDF5DataHandle::AllocReadBuffer(size_t size)
 {
   if(fxReadBuffer && (size == fiDataSize)) return;
   delete fxReadBuffer;
   fxReadBuffer= new Char_t[size];
   fiDataSize=size;
   go4hdfdbg("TGo4HDF5DataHandle: AllocReadBuffer has created read buffer 0x%x for  size %ld \n",
          (unsigned long) fxReadBuffer, fiDataSize);
 }


 void TGo4HDF5DataHandle::BuildWriteDataset(H5::H5File* file)
 {



   if(fiCollectionType == GO4HDF5_COLL_NONE)
   {
     go4hdfdbg("TGo4HDF5DataHandle: BuildWriteDataset  for plain set %s\n", fxTypeName.Data());
      Int_t rank=1; // entries of the ntuple/tree
      hsize_t      dims[1]  = { 1};  // dataset dimensions at creation
      hsize_t      maxdims[1] = {H5S_UNLIMITED};
      fxDataSpace= new H5::DataSpace(rank, dims, maxdims);

      H5::DSetCreatPropList cparms;
      hsize_t      chunk_dims[1] ={1};
      cparms.setChunk( rank, chunk_dims );
      cparms.setFillValue(*fxType, fxData);

     fxDataSet =  file->createDataSet( fxTypeName.Data(), *fxType, *fxDataSpace, cparms);

   }
   else if (fiCollectionType == GO4HDF5_COLL_STDVEC)
   {
     go4hdfdbg("TGo4HDF5DataHandle: BuildWriteDataset  for collection set %s using rank 2 \n",fxTypeName.Data());

     Int_t rank=1; // entries of the ntuple/tree. vector dimension is handled by variable size type?
     hsize_t      dims[1]  = { 1};  // dataset dimensions at creation
     hsize_t      maxdims[1] = {H5S_UNLIMITED};
     fxDataSpace= new H5::DataSpace(rank, dims, maxdims);
     H5::DSetCreatPropList cparms;
     hsize_t      chunk_dims[1] ={1};
     cparms.setChunk( rank, chunk_dims );

     cparms.setFillValue(H5::VarLenType(fxType), &fxVarHandle.fxArray);
     fxDataSet =  file->createDataSet( fxTypeName.Data(), H5::VarLenType(fxType), *fxDataSpace, cparms);


// test only if we can handle multiple datasets in file - use first element in vector only!
//     cparms.setFillValue(*fxType, fxVarHandle.fxArray.p);
//     fxDataSet =  file->createDataSet( fxTypeName.Data(), *fxType, *fxDataSpace, cparms);
//////// the above works with rank=1 JAM 27-may-2019

   }
   else
   {
     go4hdfdbg("TGo4HDF5DataHandle: BuildWriteDataset : collection type %d not supported yet\n",
             fiCollectionType);

   }




      // now recursively do it for our components:
       for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
         {
           TGo4HDF5DataHandle* entry= fxSubcomponents[i];
           entry->BuildWriteDataset(file);
         }

 }






 void TGo4HDF5DataHandle::Read(hsize_t sequencenum)
 {
   if(sequencenum>=fiEntries)
   {
     go4hdfdatahandlethrow( "TGo4HDF5DataHandle::Read - reached end of dataspace at %ld entries!\n", fiEntries);
   }

   hsize_t offset[1]= {sequencenum};
   hsize_t onerow[1] ={1};
   fxFileSpace.selectHyperslab( H5S_SELECT_SET, onerow, offset ); // one row of event object at last entry (offset is nextsize-1)
         // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
         // the data space in file for each event is of course increasing, so we need to extend the dataset and select
         // a hyperslab to the write destination in the file.
   go4hdfdbg("TGo4HDF5DataHandle::Read of %s has selected hyperslab for onerow:[%ld] offset:[%ld]\n",
               fxTypeName.Data(),(unsigned long) onerow[0], (unsigned long) offset[0]);


   if (fiCollectionType == GO4HDF5_COLL_STDVEC)
      {
          go4hdfdbg("TGo4HDF5DataHandle::Read %s collection \n", fxTypeName.Data());

          // TODO: set varhandle to buffer - setobjectpointer has done this to actual vector in event object
          // we need to resize this before we can directly fill it!
//          size_t vsize =  fxDataSet.getVarLenType().getSize(); // this is just size in bytes, not #entries
//          go4hdfdbg("TGo4HDF5DataHandle::Read FFFF tells us size of vlen type size is now %ld \n",
//              vsize);

          // TODO: how to get from hdf5 length of vector _before_ reading it!!!?
          // not necessary, since hdf5 will do allocation itself at hvl_t.p
//          int numentries = 5; //? get from filespace ?
//          size_t currentsize= fiElementSize * numentries;
//          AllocReadBuffer(currentsize);
///////////////////////////


          fxVarHandle.fxArray.p=0; //note: read will allocate the necessary buffer memory automatically!
          fxDataSet.read(&fxVarHandle.fxArray, H5::VarLenType(fxType), *fxDataSpace, fxFileSpace);
          go4hdfdbg("TGo4HDF5DataHandle::Read LLLL length after reading is %ld, buffer is 0x%x \n",
              fxVarHandle.fxArray.len,
              fxVarHandle.fxArray.p);


/////////////// First attempt: resize vector via interpreter and do memcpy from read buffer
          // build interpreter command to resize our vector using handle on top event:
//          TString resizecom = TString::Format("%s * myevent= (%s *) %ld; myevent-> %s.resize(%ld)",
//              fxEventClass.Data(), fxEventClass.Data(), (unsigned long) fxEvent, fxMemberHandle.Data(), fxVarHandle.fxArray.len);
//
//
//
//
////          TString resizecom = TString::Format("%s.resize(%ld)", fxMemberHandle.Data(),fxVarHandle.fxArray.len);
//          go4hdfdbg("TGo4HDF5DataHandle::Read  will use command: %s \n", resizecom.Data());
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
//                   "TGo4HDF5DataHandle::Read has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
//                   len, cap, (unsigned long) *p_begin_ptr, (unsigned long) *p_end_ptr, (unsigned long) *p_cap_ptr);
//          // here copy data from our hdf5 bounce buffer to actual event object:
//          Char_t* target = *p_begin_ptr;
//          Char_t* source = (Char_t*) fxVarHandle.fxArray.p; //(Char_t*) (fxReadBuffer);
//          size_t copylen = fxVarHandle.fxArray.len * fiElementSize;
//          go4hdfdbg(
//                  "TGo4HDF5DataHandle::Read before memcopy from 0x%x to 0x%x with length %ld (element size is :%ld)\n",
//                           (unsigned long) source, (unsigned long) target, copylen, fiElementSize);
//
//
//          if(fxVarHandle.fxArray.len >0){
//            memcpy (target, source, copylen);
//
//            go4hdfdbg(
//                 "TGo4HDF5DataHandle::Read AFTER COPY has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
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

          go4hdfdbg("TGo4HDF5DataHandle::Read  will use command: %s \n", copycom.Data());
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
              "TGo4HDF5DataHandle::Read after INSERT has collection size: %ld capacity: %ld pointer to buffer 0x%lx, end: 0x%lx cap: 0x%lx\n",
              len, cap, (unsigned long) *p_begin_ptr, (unsigned long) *p_end_ptr, (unsigned long) *p_cap_ptr);
#endif

          }

////////////// end alternative approach with insert.

      }
   else  if(fiCollectionType == GO4HDF5_COLL_NONE)
      {




//#ifdef GO4HDF5_DEBUG
//
//        printf("Go4 event memory dump before reading  \n");
//                      Char_t* cev= (Char_t*) fxData;
//                      for(unsigned int t=0; t< fiDataSize; ++t)
//                        {
//                        printf(" %.2hhx ", *(cev+t));
//                        //if( t!=0 && (((t+1) % 64) == 0)) printf("\n"); else
//                        if (t!=0 && (((t+1) % 8) == 0)) printf(",\t");
//                        }
//                      printf("\n");
//
//#endif
//

        // we must not use actual eventclass directly,
        // because hdf5 will destroy memory of not registered data members like pointer to eventsource and class info!
        void* data= (void*)(fxReadBuffer);

        // printf("TGo4HDF5Source: BuildEvent before read on data pointer 0x%lx \n",data);

        fxDataSet.read( data, *fxType, *fxDataSpace, fxFileSpace );

//#ifdef GO4HDF5_DEBUG
//         printf("raw buffer dump without offset \n");
//         Char_t* cr= fxReadBuffer;
//         for(unsigned int t=0; t< fiDataSize; ++t)
//           {
//              printf(" %.2hhx ", *(cr+t));
//              //if( t!=0 && (((t+1) % 64) == 0)) printf("\n"); else
//              if (t!=0 && (((t+1) % 8) == 0)) printf(",\t");
//                  }
//         printf("\n");
//
//         int delta=fiReadOffset; // first data member offset, 92 for example2step
//         printf("int buffer dump with offset %d \n",delta);
//
//         Int_t* cursor= (Int_t*)(fxReadBuffer + delta);
//         for(unsigned int t=0; t< (fiDataSize-delta)/sizeof(Int_t); ++t)
//         {
//           printf(" %d, ", *(cursor+t));
//           if(t !=0 && (((t+1) % 8) == 0)) printf("\n");
//         }
//         printf("\n");
//#endif

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



         // now recursively do it for our subcomponents :
         for(unsigned int i=0; i<fxSubcomponents.size();++i) // raterecord index
           {
             TGo4HDF5DataHandle* entry= fxSubcomponents[i];
             entry->Read(sequencenum);
           }

 }

void TGo4HDF5DataHandle::Write(hsize_t sequencenum)
{
  hsize_t offset[1] = { sequencenum };
  hsize_t nextsize[1] = { sequencenum + 1 };


//  int ref=fxDataSet.getCounter()>0;
//  //printf("TGo4HDF5DataHandle::Write %s has dataset refcounter %d\n", fxTypeName.Data(), ref);
//  if(ref)
//  {

  fxDataSet.extend(nextsize);
  H5::DataSpace fspace = fxDataSet.getSpace();    // dataspace in file
  hsize_t onerow[1] = { 1 };
  fspace.selectHyperslab(H5S_SELECT_SET, onerow, offset);    // one row of event object at last entry (offset is nextsize-1)
  // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
  // the data space in file for each event is of course increasing, so we need to extend the dataset and select
  // a hyperslab to the write destination in the file.

//}

  if (fiCollectionType == GO4HDF5_COLL_STDVEC)
  {
    if (fxVarHandle.fxArray.p != 0)
    {
      go4hdfdbg("TGo4HDF5DataHandle::Write %s collection \n", fxTypeName.Data());
      fxDataSet.write(&fxVarHandle.fxArray, H5::VarLenType(fxType), *fxDataSpace, fspace);
 //       fxDataSet.write(fxVarHandle.fxArray.p, *fxType, *fxDataSpace, fspace); // test if we can write at lease first element in array? yes!
    }
    else
    {
      go4hdfdbg("TGo4HDF5DataHandle::Write DO NOT WRITE empty %s collection\n", fxTypeName.Data());
    }
  }
  else
  {
    go4hdfdbg("TGo4HDF5DataHandle::Write %s plain\n", fxTypeName.Data());
    fxDataSet.write(fxData, *fxType, *fxDataSpace, fspace);
  }
  // now recursively do it for our subcomponents :
  for (unsigned int i = 0; i < fxSubcomponents.size(); ++i)    // raterecord index
  {
    TGo4HDF5DataHandle* entry = fxSubcomponents[i];
    entry->Write(sequencenum);
  }

}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


const char* TGo4HDF5Adapter::fgcFILESUF = ".h5";




void TGo4HDF5Adapter::CloseFile()
{
  if(fxFile==0) return;
  delete fxFile;
  fxFile=0;
}


void TGo4HDF5Adapter::DeleteDataSet()
{
  if(fbDataSetExists)
    delete fxHandle;
  fbDataSetExists=kFALSE;
}

size_t TGo4HDF5Adapter::ScanEventSize(TGo4EventElement* event)
{
  if (event==0) return 0;
  TClass* evclass=event->Class();
  TClass* actualclass=evclass->GetActualClass(event);
  size_t rev=actualclass->Size();
  go4hdfdbg("TGo4HDF5Adapter: ScanEventSize for class %s with object size %ld\n",
       actualclass->GetName(), rev);
  return rev;
}



void TGo4HDF5Adapter::FillTypeInfo(TGo4HDF5DataHandle* handle, TClass* rootclass, const char* basename)
{
  if(handle==0 || rootclass==0) return;
  go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  CCCC - for class %s \n", rootclass->GetName());
  // first check here baseclass members:
  // otherwise will not store complete object
  // and we have the problem of the bounce buffer with offset when reading back (first implementation!)
  TIter baseiter(rootclass->GetListOfBases());
  TObject* obj = 0;
    while ((obj=baseiter()) != 0) {
      //printf("TGo4HDF5Adapter::FillTypeInfo - baseiter object 0x%x of name %s , class:%s\n", obj, (obj ? obj->GetName()  : "No base class"), (obj ? obj->IsA()->GetName()  : "No type"));
       TBaseClass* base = dynamic_cast<TBaseClass*>(obj);
       go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo - base class 0x%lx %s \n", (unsigned long) base, (base ? base->GetName() : "No base class"));
       if (base==0) continue;
       TClass* bclass=base->GetClassPointer();
       if(bclass==0) continue;
       FillTypeInfo(handle, bclass, basename);
    }

    // skip base class members not necessary for our dataset:G
    if(strcmp(rootclass->GetName(),"TObject")==0) return;
    if(strcmp(rootclass->GetName(),"TNamed")==0) return;
    if(strcmp(rootclass->GetName(),"TGo4EventElement")==0) return;

  // follows the members of our class
  TIter iter(rootclass->GetListOfDataMembers());

    //TObject* obj = 0;
    while ((obj=iter()) != 0) {
       TDataMember* member = dynamic_cast<TDataMember*>(obj);
       if (member==0) continue;
       const char* memtypename = member->GetFullTypeName();
       TString fullname= (basename ?    TString::Format("%s_%s", basename, member->GetName()) : TString(member->GetName()));
       const char* membername = fullname.Data();
       size_t memberoffset = member->GetOffset();
       go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  *** sees member %s of type %s, offset=%ld \n", membername, memtypename, memberoffset);
       //continue; // DEBUG IT
       Int_t arraydim = member->GetArrayDim();
       if (arraydim>2) continue;
       hsize_t maxindex1(1), maxindex2(1);
       H5::DataType theType;
       // do not edit IsA info
       if(strstr(memtypename,"TClass")) // handles TClass* and atomic_TClass_ptr of ROOT6
         continue;
       if(strstr(membername,"fgIsA")!=0) // paranoidly redundant, never come here
         continue;

       // first check if we have a collection member.
       TClass* innerclass=TClass::GetClass(memtypename);
       TVirtualCollectionProxy* cprox = (innerclass ? innerclass->GetCollectionProxy() : 0);
       if(cprox)
       {
            TClass* collectionclass=cprox->GetCollectionClass();
            TClass* valueclass=cprox->GetValueClass();
           if(valueclass==0) continue;
           size_t innersize=valueclass->Size();
           go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  has collection proxy for type %d, collectionclass %s, class %s, size:%ld\n",
               collectionclass->GetCollectionType(), collectionclass->GetName(), valueclass->GetName(), innersize);//, cprox->Size() needs real collection object set to proxy);

           TGo4HDF5DataHandle* subhandle=handle->AddSubMember(membername, innersize, collectionclass->GetName());
           subhandle->SetParentOffset(memberoffset);
           subhandle->SetElementSize(valueclass->Size());
           subhandle->SetObjectPointer(handle->Data()); // offset to our parent will be accounted internally here!
           subhandle->SetMemberName(member->GetName()); // put plain name of the data member here first. will be exended later to format access expression
           subhandle->SetMemberClass(valueclass->GetName());
           FillTypeInfo(subhandle, valueclass, membername);
           continue; // everything is done in subhandle, continue with next member of our level
        }

       // now test simple members:
       else if ((strcmp(memtypename,"Char_t")==0) || (strcmp(memtypename,"char")==0))
              theType=H5::PredType::NATIVE_CHAR;
       else if ((strcmp(memtypename,"UChar_t")==0) || (strcmp(memtypename,"unsigned char")==0))
         theType=H5::PredType::NATIVE_UCHAR;
       else if ((strcmp(memtypename,"Short_t")==0) || (strcmp(memtypename,"short")==0))
         theType=H5::PredType::NATIVE_SHORT;
       else if ((strcmp(memtypename,"UShort_t")==0) || (strcmp(memtypename,"unsigned short")==0))
              theType=H5::PredType::NATIVE_USHORT;
       else if ((strcmp(memtypename,"Int_t")==0) || (strcmp(memtypename,"int")==0))
         theType=H5::PredType::NATIVE_INT;
       else if ((strcmp(memtypename,"UInt_t")==0) || (strcmp(memtypename,"unsigned int")==0))
              theType=H5::PredType::NATIVE_UINT;
       else if ((strcmp(memtypename,"Double_t")==0)|| (strcmp(memtypename,"double")==0))
              theType=H5::PredType::NATIVE_DOUBLE;
       else if ((strcmp(memtypename,"Float_t")==0) ||  (strcmp(memtypename,"float")==0))
                  theType=H5::PredType::NATIVE_FLOAT;
       else if ((strcmp(memtypename,"Bool_t")==0)|| (strcmp(memtypename,"bool")==0))
                   theType=H5::PredType::NATIVE_HBOOL;


       else if((strcmp(memtypename,"TString")==0) || (strstr(memtypename,"string")!=0))
       {
           continue; // skip for the moment names and text information TODO!
       }

       // TODO: pointer component on heap. should be also possible


       // TODO: go4 composite event. probably more easy than vector?


       else   {
         // evaluate structure components here
         TClass* innerclass=TClass::GetClass(memtypename);
         if(innerclass==0) continue;
         go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  finds root class info for type %s\n",memtypename);

         size_t innersize=innerclass->Size();
         // temporary datahandle just to extract the type components
         TGo4HDF5DataHandle innercomp(memtypename,innersize);
         FillTypeInfo(&innercomp, innerclass, membername);
         theType= *(innercomp.GetType());
       }

       const H5std_string theMEMBER(membername);

       switch(arraydim) {
         case 1:
         {
           maxindex1 = member->GetMaxIndex(0);
           hsize_t dims[1]={maxindex1};
           H5::ArrayType theArray(theType, arraydim, dims);
           go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
               membername,memtypename, arraydim,maxindex1, memberoffset);
 //          handle->GetType()->insertMember( theMEMBER, memberoffset, theArray);
           handle->InsertTypeMember( theMEMBER, memberoffset, theArray);
           break;
         }
         case 2:
         {
           maxindex1 = member->GetMaxIndex(0);
           maxindex2 = member->GetMaxIndex(1);
           hsize_t dims[2]={maxindex1, maxindex2};
           H5::ArrayType theArray(theType, arraydim, dims);
           go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
                    membername,memtypename, arraydim,maxindex1, memberoffset);

          //handle->GetType()->insertMember( theMEMBER, memberoffset, theArray);
           handle->InsertTypeMember( theMEMBER, memberoffset, theArray);
           break;

         }


         default:
           go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo inserts simple member %s, type %s dimension %d,  maxindex:%lld , offset:%ld\n",
                    membername, memtypename, arraydim,maxindex1, memberoffset);

           //handle->GetType()->insertMember( theMEMBER, memberoffset, theType);
           handle->InsertTypeMember(theMEMBER, memberoffset, theType);
           break;

         } // switch()
       go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo inserted member %s \n", membername);

    }

}



void TGo4HDF5Adapter::BuildDataType(TGo4EventElement* event)
{
  TClass* evclass=event->Class();
  if(!evclass)
    {
      TGo4Log::Error("TGo4HDF5Adapter::BuildDataType can not find an event class\n");
      return;
    }
  TClass* actualclass=evclass->GetActualClass(event);
  //fiEventSize=actualclass->Size();
  go4hdfdbg("TGo4HDF5Adapter::BuildDataType for class %s\n",
      actualclass->GetName());

    fxHandle=new TGo4HDF5DataHandle(actualclass->GetName(), ScanEventSize(event));
    fxHandle->SetObjectPointer(event);
    FillTypeInfo(fxHandle, actualclass, actualclass->GetName());


    fxHandle->SetTopEvent(event);
    fxHandle->SetTopEventClass(actualclass->GetName());

}



UInt_t TGo4HDF5Adapter::ConvertFileMode(Go4_H5_File_Flags flags)
{
 UInt_t h5flags=0;

 switch(flags)
 {
   case GO4_H5F_ACC_NONE:
   case GO4_H5F_ACC_TRUNC:
     h5flags=H5F_ACC_TRUNC;
     break;
   case GO4_H5F_ACC_EXCL:
     h5flags=H5F_ACC_EXCL;
     break;
   case GO4_H5F_ACC_RDONLY:
     h5flags=H5F_ACC_RDONLY;
     break;
   case GO4_H5F_ACC_RDWR:
     h5flags=H5F_ACC_RDWR;
   break;
   default:
     h5flags=H5F_ACC_TRUNC;
     break;
 }
 return h5flags;
}
