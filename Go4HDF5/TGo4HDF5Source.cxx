// $Id: TGo4HDF5Source.cxx 2130 2018-08-14 10:13:11Z linev $
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




#include "TGo4HDF5Source.h"

#include "TKey.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TRegexp.h"
#include "TDataMember.h"

#include "TGo4Log.h"
#include "TGo4HDF5SourceParameter.h"
#include "TGo4EventElement.h"
#include "TGo4EventSourceException.h"
// get static name constants from here
#include "TGo4HDF5Store.h"

// following define will switch on detailed dumps of read event data:
//#define GO4HDF5_DEBUG 1


TGo4HDF5Source::TGo4HDF5Source(const char* name) :
   TGo4EventSource(name), TGo4HDF5Adapter(), fxFileSpace(0), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
{
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, Form("!!! ERROR: Cannot open source %s!!!", GetName()));
}

TGo4HDF5Source::TGo4HDF5Source(TGo4HDF5SourceParameter* par) :
   TGo4EventSource(par->GetName()), TGo4HDF5Adapter(), fxFileSpace(0), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
{
   // TODO: all file regexp magic into base class for all eventsources
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, Form("!!! ERROR: Cannot open source %s!!!", GetName()));
}


TGo4HDF5Source::TGo4HDF5Source() :
   TGo4EventSource("Go4HDF5Source"),TGo4HDF5Adapter(), fxFileSpace(0), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
{
   // for streamer, do not open here!
}

TGo4HDF5Source::~TGo4HDF5Source()
{
   CloseCurrentFile();

   if (fxFilesNames) {
      delete fxFilesNames;
      fxFilesNames = 0;
   }
}

TList* TGo4HDF5Source::ProducesFilesList(const char* mask)
{
   if ((mask==0) || (strlen(mask)==0)) return 0;

   TString dirname, basename(mask);

   if (!basename.MaybeWildcard()) {

      // add default suffix
      if(strstr(basename.Data(),TGo4HDF5Adapter::fgcFILESUF)==0)
         basename += TGo4HDF5Adapter::fgcFILESUF;

      TList* lst = new TList();
      lst->SetOwner(kTRUE);
      lst->Add(new TObjString(basename));
      return lst;
   }

   Bool_t withdir = kFALSE;
   Int_t slash = basename.Last('/');

#ifdef WIN32
   if (slash<0) slash = basename.Last('\\');
#endif

   if (slash>=0) {
      dirname = basename(0, slash);
      basename.Remove(0, slash+1);
      withdir = kTRUE;
   } else {
      dirname = gSystem->WorkingDirectory();
   }

   void *dir = gSystem->OpenDirectory(gSystem->ExpandPathName(dirname.Data()));

   if (dir==0) return 0;

   TList* lst = 0;

   TRegexp re(basename, kTRUE);
   const char* file = 0;
   while ((file = gSystem->GetDirEntry(dir)) != 0) {
      if (!strcmp(file,".") || !strcmp(file,"..")) continue;
      TString s = file;
      if ( (basename!=s) && s.Index(re) == kNPOS) continue;
      if (lst==0) {
         lst = new TList;
         lst->SetOwner(kTRUE);
      }
      if (withdir)
         lst->Add(new TObjString(dirname + "/" + file));
      else
         lst->Add(new TObjString(file));
   }
   gSystem->FreeDirectory(dir);

   if (lst) lst->Sort();

   return lst;
}

Bool_t TGo4HDF5Source::OpenNextFile()
{
   CloseCurrentFile();

   if ((fxFilesNames==0) || (fxFilesNames->GetSize()==0)) return kFALSE;

   TObject* obj = fxFilesNames->First();
   fxCurrentFileName = obj->GetName();
   fxFilesNames->Remove(fxFilesNames->FirstLink());
   delete obj;

   OpenFile(fxCurrentFileName.Data());
   return kTRUE;
}


Bool_t TGo4HDF5Source::CloseCurrentFile()
{
  try{
   CloseFile();
   TGo4Log::Info("TGo4HDF5Source: Closed file %s", fxCurrentFileName.Data());
   fxCurrentFileName = "";
   return kTRUE;
  }
  catch(H5::Exception& ex)
  {
     TString msg=   TString::Format( "Close File %s with HDF5 exception in %s : %s\n", fxCurrentFileName.Data(), ex. getCFuncName (), ex.getCDetailMsg ());
     TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
     //SetErrMess(msg.Data());
     //throw TGo4EventSourceException(this);
     return kFALSE;
  }
}


void TGo4HDF5Source::OpenFile(const char* fname)
{
  TString buffer(fname);
  if (strstr(buffer.Data(), TGo4HDF5Adapter::fgcFILESUF) == 0)
    buffer.Append(TGo4HDF5Adapter::fgcFILESUF);

try{
  CloseFile();
  fxFile = new H5::H5File(buffer.Data(), H5F_ACC_RDONLY);
  TGo4Log::Info("TGo4HDF5Source %s: Open file %s for reading", GetName(), buffer.Data());
}
catch(H5::Exception& ex)
{
   TString msg=   TString::Format( "OpenFile with HDF5 exception in %s : %s\n", ex. getCFuncName (), ex.getCDetailMsg ());
   TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
   SetErrMess(msg.Data());
   throw TGo4EventSourceException(this);
}


}



void TGo4HDF5Source::BuildDataSet(TGo4EventElement* event)
{
  // TODO: all hdf5 file and dataset treatment in adapter class common to source and store
 if(fbDataSetExists) return;
 if (event==0 || fxFile==0) return;

 try{

   TString classname=BuildDataType(event);



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

fxDataSet = fxFile->openDataSet(classname.Data());

H5T_class_t type_class = fxDataSet.getTypeClass();
if( type_class != H5T_COMPOUND)
{
  TString msg=   TString::Format( "TGo4HDF5Source: BuildDataSet - file contains no compound data type, but type %d!\n",type_class);
  TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
  SetErrMess(msg.Data());
  throw TGo4EventSourceException(this);


}
H5::CompType intype= fxDataSet.getCompType();
size_t size = intype.getSize();
printf("TGo4HDF5Source::BuildDataSet finds in file compound type of size %ld \n",  size);



fxFileSpace = fxDataSet.getSpace();

int frank = fxFileSpace.getSimpleExtentNdims();

hsize_t dims_out[2];
int ndims = fxFileSpace.getSimpleExtentDims( dims_out, NULL);
printf("TGo4HDF5Source::BuildDataSet file dataspace has rank %d, dimensions:%d, dimensions(%ld x %ld) \n", frank , ndims,
         (unsigned long)(dims_out[0]),(unsigned long)(dims_out[1]));



// this is redundant, since we know that we have dimension 1 for the custum structure
//H5::DSetCreatPropList cparms = fxDataSet.getCreatePlist();
//hsize_t chunk_dims[2];
//int     rank_chunk;
//if( H5D_CHUNKED == cparms.getLayout() )
//{
//    /*
//     * Get chunking information: rank and dimensions
//     */
//    rank_chunk = cparms.getChunk( 2, chunk_dims);
//    printf("TGo4HDF5Source::BuildDataSet file dataspace has CHUNK rank %d, CHUNK dimensions(%ld x %ld) \n",
//        rank_chunk , chunk_dims[0], chunk_dims[1]);
//}
//else
//{
//  printf("TGo4HDF5Source::BuildDataSet file dataspace is NOT CHUNKED !\n");
//}






#ifdef GO4HDF5_DEBUG
// check consistency of file dataset with input event structure:
int nfilemembers= intype.getNmembers ();
int neventmembers= fxType->getNmembers ();


printf("Type comparison: number of compound members: file  %d, event %d\n",nfilemembers, neventmembers);

printf("Dump Type from file:\n");
    for (int i = 0; i < nfilemembers; ++i)
    {
      H5::DataType mem_type=intype.getMemberDataType(i);
      printf("\t %d: name:%s class:%s", i, intype.getMemberName(i).c_str(), mem_type.fromClass().c_str());
      H5T_class_t mem_class = intype.getMemberClass(i);
      if (mem_class == H5T_ARRAY)
      {
        H5::ArrayType artyp = intype.getMemberArrayType(i);
        int ardim = artyp.getArrayNDims();
        hsize_t dims[ardim];
        artyp.getArrayDims(dims);
        printf(" array dimensions:%d [", ardim);
        for (int j = 0; j < ardim; ++j)
        {
          printf("%d", (int) dims[j]);
        }
        printf("]");
      }
      printf("\n");
    }
    printf("Dump Type from event structure:\n");
        for (int i = 0; i < neventmembers; ++i)
        {
          H5::DataType mem_type=fxType->getMemberDataType(i);
          printf("\t %d: name:%s class:%s", i, intype.getMemberName(i).c_str(), mem_type.fromClass().c_str());
          H5T_class_t mem_class = fxType->getMemberClass(i);
          if (mem_class == H5T_ARRAY)
          {
            H5::ArrayType artyp = fxType->getMemberArrayType(i);
            int ardim = artyp.getArrayNDims();
            hsize_t dims[ardim];
            artyp.getArrayDims(dims);
            printf("array dimensions:%d [", ardim);
            for (int j = 0; j < ardim; ++j)
            {
              printf("%lu", dims[j]);
            }
            printf("]");
          }
          printf("\n");
        }

#endif

if(!(*fxType==intype))
{
  TString msg=   TString::Format( "BuildDataSet type mismatch between event structure(%s) and input from file (%s)\n",
      fxType->fromClass().c_str(), intype.fromClass().c_str());
    TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
   SetErrMess(msg.Data());
    throw TGo4EventSourceException(this);

}

fxReadBuffer= new Char_t[fiEventSize];
#ifdef GO4HDF5_DEBUG
printf("TGo4HDF5Source: BuildDataSet has created read buffer for event size %d \n",
     fiEventSize);
#endif

  std::cout<< std::endl; // flush output in gui terminal window ?
  fbDataSetExists=kTRUE;
}

catch(H5::Exception& ex)
{
   TString msg=   TString::Format( "BuildDataSet with HDF5 exception in %s : %s\n", ex.getCFuncName (), ex.getCDetailMsg ());
   TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
  SetErrMess(msg.Data());
   throw TGo4EventSourceException(this);

}

}


void TGo4HDF5Source::DeleteDataSet()
{
  delete fxReadBuffer;
  TGo4HDF5Adapter::DeleteDataSet();
}




Bool_t TGo4HDF5Source::BuildEvent(TGo4EventElement* dest)
{
   if(dest==0) ThrowError(0,22,"!!! ERROR BuildEvent: no destination event!!!");


   fxEvent = dest; // address of next event into event pointer
   BuildDataSet(dest);

  try{
    //printf("TGo4HDF5Source: BuildEvent entering try block... \n");
     // extend dataset for next event:
     hsize_t offset[1]= {fiFillCount++};
     hsize_t onerow[1] ={1};
     fxFileSpace.selectHyperslab( H5S_SELECT_SET, onerow, offset ); // one row of event object at last entry (offset is nextsize-1)
         // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
         // the data space in file for each event is of course increasing, so we need to extend the dataset and select
         // a hyperslab to the write destination in the file.


#ifdef GO4HDF5_DEBUG
        printf("TGo4HDF5Source: BuildEvent has selected hyperslab for onerow:[%d] offset:[%d]\n",
            onerow[0], offset[0]);
        printf("TGo4HDF5Source: fxEvent=0x%x\n", fxEvent);
        printf("TGo4HDF5Source: Eventname:%s\n", fxEvent->GetName());
        printf("TGo4HDF5Source: is valid:%d\n", fxEvent->IsValid());
        printf("Go4 event has eventsource pointer 0x%lx  \n",(long) fxEvent->GetEventSource());
        printf("TGo4HDF5Source: Event printout:\n");
        fxEvent->PrintEvent();
        printf("Go4 event memory dump before reading  \n");
               Char_t* cev= (Char_t*) fxEvent;
               for(int t=0; t< fiEventSize; ++t)
                 {
                 printf(" %.2hhx ", *(cev+t));
                 //if( t!=0 && (((t+1) % 64) == 0)) printf("\n"); else
                 if (t!=0 && (((t+1) % 8) == 0)) printf(",\t");
                 }
               printf("\n");
#endif



//         void* data= (void*)(fxEvent);
         // we rely on ROOT here that all offsets given by streamer info are relative to the begin of the object

        // we must not use actual eventclass directly,
        // because hdf5 will destroy memory of not registered data members like pointer to eventsource and class info!
        void* data= (void*)(fxReadBuffer);

          // printf("TGo4HDF5Source: BuildEvent before read on data pointer 0x%lx \n",data);

         fxDataSet.read( data, *fxType, *fxDataSpace, fxFileSpace );

#ifdef GO4HDF5_DEBUG
         printf("raw buffer dump without offset \n");
         Char_t* cr= fxReadBuffer;
         for(int t=0; t< fiEventSize; ++t)
           {
              printf(" %.2hhx ", *(cr+t));
              //if( t!=0 && (((t+1) % 64) == 0)) printf("\n"); else
              if (t!=0 && (((t+1) % 8) == 0)) printf(",\t");
                  }
         printf("\n");

         int delta=fiReadOffset; // first data member offset, 92 for example2step
         printf("int buffer dump with offset %d \n",delta);

         Int_t* cursor= (Int_t*)(fxReadBuffer + delta);
         for(int t=0; t< (fiEventSize-delta)/sizeof(Int_t); ++t)
         {
           printf(" %d, ", *(cursor+t));
           if(t !=0 && (((t+1) % 8) == 0)) printf("\n");
         }
         printf("\n");
#endif

         // here copy data from hdf5 buffer to actual event object:
         Char_t* target = (Char_t*) (fxEvent) + fiReadOffset;
         Char_t* source = (Char_t*) (fxReadBuffer) + fiReadOffset;
         size_t copylen = fiEventSize - fiReadOffset;
         memcpy (target, source, copylen);

#ifdef GO4HDF5_DEBUG
         printf("Copy %d bytes from %lx to %lx, fiReadOffset=%d, fiEventSize=%d \n",
         copylen, (long) source, (long)target, fiReadOffset, fiEventSize);
         printf("Go4 event memory dump  \n");
         cev= (Char_t*) fxEvent;
         for(int t=0; t< fiEventSize; ++t)
           {
           printf(" %.2hhx ", *(cev+t));
           //if( t!=0 && (((t+1) % 64) == 0)) printf("\n"); else
           if (t!=0 && (((t+1) % 8) == 0)) printf(",\t");
           }
         printf("\n");

         printf("Go4 event has eventsource pointer 0x%lx  \n",(long) fxEvent->GetEventSource());
         printf("Go4 event has identifier 0x%hx  \n",(long) fxEvent->getId());

#endif

  return kTRUE;
  }
  catch(H5::Exception& ex)
  {
     TString msg=   TString::Format( "BuildEvent() with HDF5 exception in %s : %s\n", ex.getCFuncName (), ex.getCDetailMsg ());
     TGo4Log::Error("TGo4HDF5Source: %s", msg.Data());
     SetErrMess(msg.Data());
     throw TGo4EventSourceException(this);
  }

}


