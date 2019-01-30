// $Id: TGo4HDF5Store.cxx 2130 2018-08-14 10:13:11Z linev $
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


#include "TGo4HDF5Store.h"

#include "TFolder.h"
#include "TFile.h"
#include "TTree.h"
#include "TDataMember.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"
#include "TGo4EventStoreException.h"

#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4Fitter.h"
#include "TGo4HDF5StoreParameter.h"

const char* TGo4HDF5Store::fgcTREESUF = "xTree";
const char* TGo4HDF5Store::fgcFILESUF = ".h5";
const char* TGo4HDF5Store::fgcEVBRANCHNAME = "Go4DataSet.";


TGo4HDF5Store::TGo4HDF5Store() :
   TGo4EventStore("Go4 Default HDF5 Store"),
   fxFile(0),
   fxDataSet(),fxType(0), fxDataSpace(0),
   fbDataSetExists(kFALSE),
   fxEvent(0),fiFlags(H5F_ACC_TRUNC),
   fiEventSize(0),fiFillCount(0)
{
   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store()", __LINE__, __FILE__));

   printf("TGo4HDF5Store::TGo4HDF5Store() default ctor\n");
   // public default ctor for streamer

}

TGo4HDF5Store::TGo4HDF5Store(const char* name,
                             UInt_t flags) :
   TGo4EventStore(name),
   fxFile(0),
   fxDataSet(),fxType(0), fxDataSpace(0),
   fbDataSetExists(kFALSE),
   fxEvent(0),fiFlags(flags),
   fiFillCount(0)
{


   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store(char*,...)", __LINE__, __FILE__));
   printf("TGo4HDF5Store::TGo4HDF5Store(char*,...)\n");
   OpenFile();
}


TGo4HDF5Store::TGo4HDF5Store(TGo4HDF5StoreParameter* par) :
         TGo4EventStore("dummy"),
         fxFile(0),
         fxDataSet(),fxType(0), fxDataSpace(0),
         fbDataSetExists(kFALSE),
         fxEvent(0),fiFlags(0),
         fiFillCount(0)
{
   GO4TRACE((15,"TGo4HDF5Store::TGo4HDF5Store(TGo4HDF5StoreParameter* par)", __LINE__, __FILE__));
   printf("TGo4HDF5Store::TGo4HDF5Store(TGo4HDF5StoreParameter* par)\n");
   if (par==0) {
      TGo4Log::Error("TGo4HDF5Store::TGo4HDF5Store(.., TGo4HDF5StoreParameter* is not specified");
      return;
   }
   SetName(par->GetName());
   fiFlags=ConvertFileMode(par->GetHDF5Flags());
   OpenFile();
}


TGo4HDF5Store::~TGo4HDF5Store()
{
   GO4TRACE((15,"TGo4HDF5Store::~TGo4HDF5Store()", __LINE__, __FILE__));
   printf("TGo4HDF5Store::~TGo4HDF5Store()\n");
   DeleteDataSet();
   CloseFile();
}

void TGo4HDF5Store::OpenFile()
{
  TString buffer(GetName());
  if (strstr(buffer.Data(), fgcFILESUF) == 0)
    buffer.Append(fgcFILESUF);

try{
  CloseFile();
  fxFile = new H5::H5File(buffer.Data(), fiFlags);
  if (fiFlags == H5F_ACC_TRUNC)
  {
    TGo4Log::Info("TGo4HDF5Store: Open file %s RECREATE", buffer.Data());
  }
  else if (fiFlags == H5F_ACC_RDWR)
  {
    TGo4Log::Info("TGo4HDF5Store: Open file %s for UPDATE", buffer.Data());
  }
  else if (fiFlags == H5F_ACC_EXCL)
  {
    TGo4Log::Info("TGo4HDF5Store: Open file %s exclusively", buffer.Data());
  }
  else
  {
    TGo4Log::Info("TGo4HDF5Store: Open file %s with unsupported mode %d", buffer.Data(),fiFlags);

  }

}
catch(H5::Exception& ex)
{
   TString msg=   TString::Format( "OpenFile with HDF5 exception in %s : %s\n", ex. getCFuncName (), ex.getCDetailMsg ());
   TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
  throw TGo4EventStoreException(this, msg.Data());
}


}

void TGo4HDF5Store::CloseFile()
{
  if(fxFile==0) return;
  delete fxFile;
  fxFile=0;
}

size_t TGo4HDF5Store::ScanEventSize(TGo4EventElement* event)
{
  if (event==0) return 0;
  TClass* evclass=event->Class();
  TClass* actualclass=evclass->GetActualClass(event);
  size_t rev=actualclass->Size();

//////// TODO later JAM2019
//  if(actualclass->InheritsFrom("TGo4CompositeEvent"));
//  {
//    TGo4CompositeEvent* theComp= dynamic_cast<TGo4CompositeEvent*>(event);
//    for(Short_t i=0; (theComp && i<theComp->getNElements()) ;++i)
//       {
//         TGo4EventElement* subev=dynamic_cast<TGo4EventElement*>(theComp->getEventElement(i));
//         rev+=ScanEventSize(subev);
//       }
//  }
  printf("TGo4HDF5Store: ScanEventSize for class %s with object size %ld\n",
       actualclass->GetName(), rev);
  return rev;
}


void TGo4HDF5Store::BuildDataSet(TGo4EventElement* event, size_t parentoffset)
{
 if(fbDataSetExists) return;
 if (event==0 || fxFile==0) return;
 TClass* evclass=event->Class();
 if(!evclass)
   {
     TGo4Log::Error("TGo4HDF5Store: BuildDataSet can not find an event class\n");
     return;
   }
 TClass* actualclass=evclass->GetActualClass(event);
 //fiEventSize=actualclass->Size();
 printf("TGo4HDF5Store: BuildDataSet for class %s with parentoffset is %ld\n",
     actualclass->GetName(), parentoffset);

try {

 if(fxType==0)

 {
   fiEventSize=ScanEventSize(event);
   printf("TGo4HDF5Store: BuildDataSet creates new type of size %ld\n",fiEventSize);
   fxType = new H5::CompType(fiEventSize);
 }


// JAM 2019 iterate over class member description and tranlate to hdf5 type:

 // TODO: in case of go4 composite events, get child class and  recursively scan this types:
 if(actualclass->InheritsFrom("TGo4CompositeEvent"))
 {
   TGo4Log::Warn("TGo4HDF5Store: BuildDataSet - TGo4CompositeEvent is currently not yet supported properly!\n");

   //JAM2019 The problem with composite event is that the components are all located on the heap and
   // do not build a coherent data structure that can be stored in a single dataset type
   // we would need to treat several dataspaces in the same file for each subcomponent
   // TODO later!

//   TGo4CompositeEvent* theComp= dynamic_cast<TGo4CompositeEvent*>(event);
//   Short_t max=theComp->getNElements();
//   for(Short_t i=0; i<max;++i)
//   {
//     TGo4EventElement* subev=dynamic_cast<TGo4EventElement*>(theComp->getEventElement(i));
//     if(subev==0) continue;
//     size_t compoffset=(char*) subev - (char*) event; // try to figure out distance to master object on heap?
//     printf("TGo4HDF5Store::BuildDataSet sees composite event %s, offset to parent is =%ld \n", subev->GetName(),
//         compoffset);
//     BuildDataSet(subev, parentoffset+compoffset);
//   }
 }

 TIter iter(actualclass->GetListOfDataMembers());

   TObject* obj = 0;
   while ((obj=iter()) != 0) {
      TDataMember* member = dynamic_cast<TDataMember*>(obj);
      if (member==0) continue;
      const char* memtypename = member->GetFullTypeName();
      TString fullname=    TString::Format("%s_%s",event->GetName(),member->GetName());
      const char* membername = fullname.Data();
      size_t memberoffset = member->GetOffset() + parentoffset;
      printf("TGo4HDF5Store::BuildDataSet sees member %s of type %s, offset=%ld \n", membername, memtypename, memberoffset);
      //continue; // DEBUG IT
      Int_t arraydim = member->GetArrayDim();
      if (arraydim>2) continue;


      H5::DataType theType;
      // do not edit IsA info
      if(strstr(memtypename,"TClass")) // handles TClass* and atomic_TClass_ptr of ROOT6
        continue;
      if(strstr(membername,"fgIsA")!=0) // paranoidly redundant, never come here
        continue;




      else if (strcmp(memtypename,"Char_t")==0)
             theType=H5::PredType::NATIVE_CHAR;
      else if (strcmp(memtypename,"UChar_t")==0)
        theType=H5::PredType::NATIVE_UCHAR;
      else if (strcmp(memtypename,"Short_t")==0)
        theType=H5::PredType::NATIVE_SHORT;
      else if (strcmp(memtypename,"UShort_t")==0)
             theType=H5::PredType::NATIVE_USHORT;
      else if (strcmp(memtypename,"Int_t")==0)
        theType=H5::PredType::NATIVE_INT;
      else if (strcmp(memtypename,"UInt_t")==0)
             theType=H5::PredType::NATIVE_UINT;
      else if (strcmp(memtypename,"Double_t")==0)
             theType=H5::PredType::NATIVE_DOUBLE;
      else if (strcmp(memtypename,"Float_t")==0)
                 theType=H5::PredType::NATIVE_FLOAT;
      else if (strcmp(memtypename,"Bool_t")==0)
                  theType=H5::PredType::NATIVE_HBOOL;

       // TODO here std::vector<Double_t>  etc
      else if (strcmp(memtypename," std::vector<Double_t>")==0)
            {
                 theType=H5::PredType::NATIVE_DOUBLE;
                 if(arraydim>0) continue; // do not support arrays of vectors yet
                 arraydim=1;
            }


      else continue;
      // skip for a moment all types which are not basic types


      // have to add parent name to membername for composite events:

      const H5std_string theMEMBER(membername);
      hsize_t maxindex1(1), maxindex2(1);


      switch(arraydim) {
        case 1:
        {
          maxindex1 = member->GetMaxIndex(0);
          hsize_t dims[1]={maxindex1};
          H5::ArrayType theArray(theType, arraydim, dims);
          printf("TGo4HDF5Store::BuildDataSet inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
              membername,memtypename, arraydim,maxindex1, memberoffset);
          fxType->insertMember( theMEMBER, memberoffset, theArray);
          break;
        }
        case 2:
        {
          maxindex1 = member->GetMaxIndex(0);
          maxindex2 = member->GetMaxIndex(1);
          hsize_t dims[2]={maxindex1, maxindex2};
          H5::ArrayType theArray(theType, arraydim, dims);
          printf("TGo4HDF5Store::BuildDataSet inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
                   membername,memtypename, arraydim,maxindex1, memberoffset);

          fxType->insertMember( theMEMBER, memberoffset, theArray);
          break;

        }
        default:
          printf("TGo4HDF5Store::BuildDataSet inserts simple member %s, type %s dimension %d,  maxindex:%lld , offset:%ld\n",
                   membername, memtypename, arraydim,maxindex1, memberoffset);

          fxType->insertMember( theMEMBER, memberoffset, theType);
          break;

        } // switch()
     // printf("TGo4HDF5Store::BuildDataSet has array dimension %d with max indexes %d %d\n", arraydim, maxindex1, maxindex2);



      printf("TGo4HDF5Store::BuildDataSet inserted member %s \n", membername);

   }

if(parentoffset!=0) return; // only top event level will create the data set in the end


Int_t rank=1; // entries of the ntuple/tree
hsize_t      dims[1]  = { 1};  // dataset dimensions at creation
hsize_t      maxdims[1] = {H5S_UNLIMITED};
fxDataSpace= new H5::DataSpace(rank, dims, maxdims);

H5::DSetCreatPropList cparms;
hsize_t      chunk_dims[1] ={1};
cparms.setChunk( rank, chunk_dims );
cparms.setFillValue(*fxType, event);

fxDataSet =  fxFile->createDataSet( actualclass->GetName(), *fxType, *fxDataSpace, cparms);

}

catch(H5::Exception& ex)
{
   TString msg=   TString::Format( "BuildDataSet with HDF5 exception in %s : %s\n", ex.getCFuncName (), ex.getCDetailMsg ());
   TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
  throw TGo4EventStoreException(this, msg.Data());

}




fbDataSetExists=kTRUE;
}

void TGo4HDF5Store::DeleteDataSet()
{
  if(fbDataSetExists) fxDataSet.close();
  delete fxDataSpace;
  delete fxType;
  fbDataSetExists=kFALSE;
}




Int_t TGo4HDF5Store::Store(TGo4EventElement* event)
{
   GO4TRACE((12,"TGo4HDF5Store::Store(TGo4EventElement*)", __LINE__, __FILE__));

   fxEvent = event; // address of next event into event pointer
   BuildDataSet(event);

try{
   // extend dataset for next event:
   hsize_t offset[1]= {fiFillCount};
   hsize_t nextsize[1]  = {++fiFillCount};
   fxDataSet.extend(nextsize);


   // write current dataset to file:

   H5::DataSpace fspace = fxDataSet.getSpace (); // dataspace in file
   hsize_t onerow[1] ={1};
   fspace.selectHyperslab( H5S_SELECT_SET, onerow, offset ); // one row of event object at last entry (offset is nextsize-1)
   // idea: the go4 event is always at same location in memory, so we use the original data space as defined in BuildDataSet
   // the data space in file for each event is of course increasing, so we need to extend the dataset and select
   // a hyperslab to the write destination in the file.


   const void* data= (void*)(event);
   // we rely on ROOT here that all offsets given by streamer info are relative to the begin of the object

   fxDataSet.write(data, *fxType, *fxDataSpace, fspace);
}
catch(H5::Exception& ex)
{
   TString msg=   TString::Format( "Store() with HDF5 exception in %s : %s\n", ex.getCFuncName (), ex.getCDetailMsg ());
   TGo4Log::Error("TGo4HDF5Store: %s", msg.Data());
   throw TGo4EventStoreException(this, msg.Data());
}
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Parameter* cali)
{
   WriteToStore(cali);
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Condition* conny)
{
   WriteToStore(conny);
   return 0;
}

Int_t TGo4HDF5Store::Store(TGo4Fitter* fitter)
{
   WriteToStore(fitter);
   return 0;
}

Int_t TGo4HDF5Store::Store(TFolder* fold)
{
   WriteToStore(fold);
   return 0;
}



void TGo4HDF5Store::WriteToStore(TNamed* ob)
{
   if (ob==0) return;
   TGo4Log::Info("TGo4HDF5Store: WriteToStore not yet implemented for auxiliary objects of class %s (name:%s)\n",ob->Class()->GetName(), ob->GetName());

}

UInt_t TGo4HDF5Store::ConvertFileMode(Go4_H5_File_Flags flags)
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
