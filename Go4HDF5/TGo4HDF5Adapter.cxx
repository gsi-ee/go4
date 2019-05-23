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

#include "TGo4Log.h"
#include "TGo4EventElement.h"

const char* TGo4HDF5Adapter::fgcFILESUF = ".h5";




void TGo4HDF5Adapter::CloseFile()
{
  if(fxFile==0) return;
  delete fxFile;
  fxFile=0;
}


void TGo4HDF5Adapter::DeleteDataSet()
{
  if(fbDataSetExists) fxDataSet.close();
  delete fxDataSpace;
  delete fxType;
  fbDataSetExists=kFALSE;
}

size_t TGo4HDF5Adapter::ScanEventSize(TGo4EventElement* event)
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
  printf("TGo4HDF5Adapter: ScanEventSize for class %s with object size %ld\n",
       actualclass->GetName(), rev);
  return rev;
}



void TGo4HDF5Adapter::FillTypeInfo(H5::CompType* h5type, TClass* rootclass, const char* basename)
{
  if(h5type==0 || rootclass==0) return;
  printf("TGo4HDF5Adapter::FillTypeInfo  CCCC - for class %s \n", rootclass->GetName());

  // first check here baseclass members:
  // otherwise will not store complete object
  // and we have the problem of the bounce buffer with offset when reading back (first implementation!)
  TIter baseiter(rootclass->GetListOfBases());
  TObject* obj = 0;
    while ((obj=baseiter()) != 0) {
      //printf("TGo4HDF5Adapter::FillTypeInfo - baseiter object 0x%x of name %s , class:%s\n", obj, (obj ? obj->GetName()  : "No base class"), (obj ? obj->IsA()->GetName()  : "No type"));
       TBaseClass* base = dynamic_cast<TBaseClass*>(obj);
       printf("TGo4HDF5Adapter::FillTypeInfo - base class 0x%x %s \n", base, (base ? base->GetName() : "No base class"));
       if (base==0) continue;
       TClass* bclass=base->GetClassPointer();
       if(bclass==0) continue;
       FillTypeInfo(h5type, bclass, basename);
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
       printf("TGo4HDF5Adapter::FillTypeInfo  *** sees member %s of type %s, offset=%ld \n", membername, memtypename, memberoffset);
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

        // TODO here std::vector<Double_t>  etc
//       else if (strcmp(memtypename,"vector<Double_t>")==0)
//             {
//                  theType=H5::PredType::NATIVE_DOUBLE;
//                  if(arraydim>0) continue; // do not support arrays of vectors yet
//                  arraydim=1;
//             }
//       // TODO: other native vectors here

       // JAM test if we can handle this explicitely anyway. Later more generic
//       else if (strcmp(memtypename,"vector<TRemiDetectorHit>")==0)
//                  {
//                       TClass* remiclass=TClass::GetClass("TRemiDetectorHit");
//                       if(remiclass==0) continue;
//                       size_t innersize=remiclass->Size();
//                       // TODO here scan the innerclass of vector (size and types)
//                       H5::CompType innercomp=H5::CompType(innersize);
//                       FillTypeInfo(&innercomp, remiclass, membername);
//                       theType=innercomp;
//                       //
//                       size_t maxindex = member->GetMaxIndex(0);
//                       printf("TGo4HDF5Adapter::FillTypeInfo  for vector<TRemiDetectorHit>, arraydim=%d, maxindex=%d\n",
//                           arraydim, maxindex);
//
//                       //if(arraydim>0) continue; // do not support arrays of vectors yet
//                       arraydim=1;
//                  }

       else if (strstr(memtypename,"vector")!=0)
                 {
                     TClass* innerclass=TClass::GetClass(memtypename);
                     if(innerclass==0) continue;
                     TVirtualCollectionProxy* cprox=innerclass->GetCollectionProxy();
                     if(cprox)
                     {
                          TClass* collectionclass=cprox->GetCollectionClass();
                          TClass* valueclass=cprox->GetValueClass();
                         if(valueclass==0) continue;
                         size_t innersize=valueclass->Size();
                         printf("TGo4HDF5Adapter::FillTypeInfo  has collection proxy for type %d, class %s, size:%d\n",
                             collectionclass->GetCollectionType(), valueclass->GetName(), innersize);//, cprox->Size() needs real collection object set to proxy);
                         H5::CompType innercomp=H5::CompType(innersize);
                         FillTypeInfo(&innercomp, valueclass, membername);
                         theType=innercomp;
                              //
                              size_t maxindex = member->GetMaxIndex(0);
                              printf("TGo4HDF5Adapter::FillTypeInfo  for vector<%s>, arraydim=%d, maxindex=%d\n",
                                  valueclass->GetName(), arraydim, maxindex);
                              //if(arraydim>0) continue; // do not support arrays of vectors yet
                              arraydim=99;
                              //maxindex1=cprox->Size();
                              maxindex1=1; // use fix size here for testing!
                              // TODO NOTE: this will not work such, since std::vector has data memory outside the object
                              // need to tell hdf5 where to find the data....
                     }
                     else
                     {
                       // no collection, continue with plain structure for general imp here?
                       printf("TGo4HDF5Adapter::FillTypeInfo  does not find collection proxy for %s\n",memtypename);
                       continue;
                     }

                 }
       else if((strcmp(memtypename,"TString")==0) || (strstr(memtypename,"string")!=0))
       {
           continue; // skip for the moment names and text information
       }


       else   {
         // evaluate structure components here
         TClass* innerclass=TClass::GetClass(memtypename);
         if(innerclass==0) continue;
         printf("TGo4HDF5Adapter::FillTypeInfo  finds root class info for type %s\n",memtypename);

         size_t innersize=innerclass->Size();
         // here scan the innerclass of vector (size and types)
         H5::CompType innercomp=H5::CompType(innersize);
         FillTypeInfo(&innercomp, innerclass, membername);
         theType=innercomp;


//         if(arraydim>0) continue; // do not support arrays of vectors yet
//         arraydim=1;
                        }



//       else continue;
       // skip for a moment all types which are not basic types


       // have to add parent name to membername for composite events:

       const H5std_string theMEMBER(membername);



       switch(arraydim) {
         case 1:
         {
           maxindex1 = member->GetMaxIndex(0);
           hsize_t dims[1]={maxindex1};
           H5::ArrayType theArray(theType, arraydim, dims);
           printf("TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
               membername,memtypename, arraydim,maxindex1, memberoffset);
           h5type->insertMember( theMEMBER, memberoffset, theArray);
           break;
         }
         case 2:
         {
           maxindex1 = member->GetMaxIndex(0);
           maxindex2 = member->GetMaxIndex(1);
           hsize_t dims[2]={maxindex1, maxindex2};
           H5::ArrayType theArray(theType, arraydim, dims);
           printf("TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
                    membername,memtypename, arraydim,maxindex1, memberoffset);

           h5type->insertMember( theMEMBER, memberoffset, theArray);
           break;

         }

         case 99:
         {
           // treat std: vectors and other collections here
           hsize_t dims[1]={maxindex1};
           H5::ArrayType theArray(theType, 1, dims);
           printf("TGo4HDF5Adapter::FillTypeInfo inserts from vector array member %s, type %s , maxindex:%lld , offset:%ld\n",
                         membername,memtypename, maxindex1, memberoffset);
           h5type->insertMember( theMEMBER, memberoffset, theArray);
           break;
         }

         default:
           printf("TGo4HDF5Adapter::FillTypeInfo inserts simple member %s, type %s dimension %d,  maxindex:%lld , offset:%ld\n",
                    membername, memtypename, arraydim,maxindex1, memberoffset);

           h5type->insertMember( theMEMBER, memberoffset, theType);
           break;

         } // switch()
      // printf("TGo4HDF5Adapter::BuildDataSet has array dimension %d with max indexes %d %d\n", arraydim, maxindex1, maxindex2);



       printf("TGo4HDF5Adapter::FillTypeInfo inserted member %s \n", membername);

    }

}



const char* TGo4HDF5Adapter::BuildDataType(TGo4EventElement* event)
{
  TClass* evclass=event->Class();
  if(!evclass)
    {
      TGo4Log::Error("TGo4HDF5Adapter::BuildDataType can not find an event class\n");
      return "";
    }
  TClass* actualclass=evclass->GetActualClass(event);
  //fiEventSize=actualclass->Size();
  printf("TGo4HDF5Adapter::BuildDataType for class %s\n",
      actualclass->GetName());

//  if(fxType==0)
//
//  {
    fiEventSize=ScanEventSize(event);
    printf("TGo4HDF5Adapter::BuildDataType creates new type of size %ld\n",fiEventSize);
    fxType = new H5::CompType(fiEventSize);

//  }


 // JAM 2019 iterate over class member description and tranlate to hdf5 type:

  // TODO: in case of go4 composite events, get child class and  recursively scan this types:
  if(actualclass->InheritsFrom("TGo4CompositeEvent"))
  {
    TGo4Log::Warn("TGo4HDF5Adapter::BuildDataType - TGo4CompositeEvent is currently not yet supported properly!\n");

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


   FillTypeInfo(fxType, actualclass, event->GetName());
   return actualclass->GetName();

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
