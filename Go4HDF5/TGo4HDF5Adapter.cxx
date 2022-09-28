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


#include "TGo4HDF5Adapter.h"

#include "TList.h"
#include "TDataMember.h"
#include "TVirtualCollectionProxy.h"
#include "TBaseClass.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"

const char* TGo4HDF5Adapter::fgcFILESUF = ".h5";


void TGo4HDF5Adapter::CloseFile()
{
  if(fxFile) {
     delete fxFile;
     fxFile = nullptr;
  }
}


void TGo4HDF5Adapter::DeleteDataSet()
{
  if(fbDataSetExists)
    delete fxHandle;
  fbDataSetExists = kFALSE;
  fxHandle = nullptr;
}

size_t TGo4HDF5Adapter::ScanEventSize(TGo4EventElement* event)
{
   if (!event)
      return 0;
   TClass *evclass = event->Class();
   TClass *actualclass = evclass->GetActualClass(event);
   size_t rev = actualclass->Size();
   go4hdfdbg("TGo4HDF5Adapter: ScanEventSize for class %s with object size %ld\n", actualclass->GetName(), rev);
   return rev;
}

void TGo4HDF5Adapter::AddSubHandle(TGo4HDF5DataHandle* handle, const char *name, const char* type, size_t size,
    size_t memberoffset, const char* membername, const char* classname, TClass* valueclass)
{
   TGo4HDF5DataHandle *subhandle = handle->AddSubMember(name, size, type);
   subhandle->SetParentOffset(memberoffset);
   subhandle->SetMemberName(membername);
   subhandle->SetMemberClass(classname);
   TGo4HDF5SubVectorDataHandle *subvector = dynamic_cast<TGo4HDF5SubVectorDataHandle *>(subhandle);
   if (subvector) {
      TString containerclass = "TGo4HDF5VectorProxy";
      FillTypeInfo(subhandle, name, containerclass.Data());

   } else {
      if (valueclass)
         FillTypeInfo(subhandle, valueclass, name);
      else
         FillTypeInfo(subhandle, name, classname);
   }
}

void TGo4HDF5Adapter::FillTypeInfo(TGo4HDF5DataHandle* handle, TClass* rootclass, const char* basename)
{
   if (!handle || !rootclass)
      return;
   go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  CCCC - for class %s \n", rootclass->GetName());
   // first check here baseclass members:
   // otherwise will not store complete object
   // and we have the problem of the bounce buffer with offset when reading back (first implementation!)
   TIter baseiter(rootclass->GetListOfBases());
   while (auto obj = baseiter()) {
      // printf("TGo4HDF5Adapter::FillTypeInfo - baseiter object 0x%x of name %s , class:%s\n", obj, (obj ?
      // obj->GetName()  : "No base class"), (obj ? obj->IsA()->GetName()  : "No type"));
      TBaseClass *base = dynamic_cast<TBaseClass *>(obj);
      go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo - base class 0x%lx %s \n", (unsigned long)base,
                (base ? base->GetName() : "No base class"));
      if (!base)
         continue;
      TClass *bclass = base->GetClassPointer();
      if (!bclass)
         continue;
      FillTypeInfo(handle, bclass, basename);
   }

   // skip base class members not necessary for our dataset:G
   if (strcmp(rootclass->GetName(), "TObject") == 0)
      return;
   if (strcmp(rootclass->GetName(), "TNamed") == 0)
      return;
   if (strcmp(rootclass->GetName(), "TGo4EventElement") == 0)
      return;

   if (strcmp(rootclass->GetName(), "TGo4CompositeEvent") == 0) {
      go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  COMP detects go4 composite event. Assign fMaxIndex member...\n");
      TDataMember *member = rootclass->GetDataMember("fMaxIndex");
      if (!member)
         return;
      size_t memberoffset = member->GetOffset();
      const char *memtypename = member->GetFullTypeName();
      TString fullname =
         (basename ? TString::Format("%s_%s", basename, member->GetName()) : TString(member->GetName()));
      const char *membername = fullname.Data();
      Int_t arraydim = member->GetArrayDim();
      FillTypeInfo(handle, membername, memtypename, memberoffset, arraydim, member);
      return;
      // avoid that reading back the event will overwrite our objectarray pointers, since it is inside the fiReadOffset
      // range! for composite events without own members, however, need the fMaxIndex as dummy to produce a hdf5 dataset
   }

   // follows the members of our class
   TIter iter(rootclass->GetListOfDataMembers());

   while (auto obj = iter()) {
      TDataMember *member = dynamic_cast<TDataMember *>(obj);
      if (!member)
         continue;
      const char *memtypename = member->GetFullTypeName();
      TString fullname =
         (basename ? TString::Format("%s_%s", basename, member->GetName()) : TString(member->GetName()));
      const char *membername = fullname.Data();
      size_t memberoffset = member->GetOffset();
      Int_t arraydim = member->GetArrayDim();
      go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  *** sees member %s of type %s, offset=%ld , arraydim=%d\n", membername,
                memtypename, memberoffset, arraydim);
      // continue; // DEBUG IT

      if (arraydim > 2)
         continue;
      //       hsize_t maxindex1(1), maxindex2(1);
      //       H5::DataType theType;
      // do not edit IsA info
      if (strstr(memtypename, "TClass")) // handles TClass* and atomic_TClass_ptr of ROOT6
         continue;
      if (strstr(membername, "fgIsA")) // paranoidly redundant, never come here
         continue;

      if (member->Property() & kIsStatic) {
         go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo ignored static member.\n");
         continue;
      }

      // first check if we have a collection member.
      TClass *innerclass = TClass::GetClass(memtypename);
      TVirtualCollectionProxy *cprox = (innerclass ? innerclass->GetCollectionProxy() : 0);
      if (cprox) {
         TClass *collectionclass = cprox->GetCollectionClass();
         TClass *valueclass = cprox->GetValueClass();
         EDataType valuetype = cprox->GetType();
         // Int_t colltype=cprox->GetCollectionType();
         go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  *** has collection proxy, type:%d, valueclass=0x%lx, valuetype=%d\n",
                   cprox->GetCollectionType(), (unsigned long)valueclass, valuetype);
         size_t innersize = 0;
         size_t collsize = 0;
         TString typenm;
         TString colltypnm;

         // continue; // JAM DEBUG - leave out collections to test composite event io first! OK, this works

         if (valueclass) {
            collsize = collectionclass->Size();
            innersize = valueclass->Size();
            typenm = valueclass->GetName();
            colltypnm = collectionclass->GetName();
            go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  has collection proxy for type %d, collectionclass %s, class %s, "
                      "size:%ld, collsize:%ld\n",
                      collectionclass->GetCollectionType(), colltypnm.Data(), typenm.Data(), innersize,
                      collsize); //, cprox->Size() needs real collection object set to proxy);

         } else if (valuetype) {
            TDataType *datatype = TDataType::GetDataType(valuetype);
            typenm = datatype->GetTypeName();
            innersize = datatype->Size();

            // will not work, since basic type do not have class object in ROOT!
            // valueclass=TClass::GetClass(typenm.Data());
            // go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo sees valueclass %s\n",(valueclass? valueclass->GetName() :"NO
            // CLASS"));

            colltypnm = memtypename; // get  the vector<double> here!
            collectionclass = TClass::GetClass(memtypename);
            if (collectionclass)
               collsize = collectionclass->Size();
            else
               go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo could not get class for collection %s \n", memtypename);
            go4hdfdbg(
               "TGo4HDF5Adapter::FillTypeInfo  has collection proxy for value %d, type %s, size:%ld, collsize:%ld\n",
               collectionclass->GetCollectionType(), typenm.Data(), innersize, collsize);
         } else {
            continue;
         }

         ///// arraydim=0; // JAM DEBUG: check if we can handle the first vector in array only

         // now evaluate the subhandles depending on the array dimension:
         // here treat special case of array of vectors (poland example)
         Int_t maxindex1 = 0, maxindex2 = 0;
         switch (arraydim) {

         default:
            // plain collection that is not part of another array:
            AddSubHandle(handle, membername, colltypnm.Data(), innersize, memberoffset, member->GetName(),
                         typenm.Data(), valueclass);
            break;

         case 1:
            maxindex1 = member->GetMaxIndex(0);
            for (int x = 0; x < maxindex1; ++x) {
               TString arraymember =
                  TString::Format("%s[%d]", membername, x); // contains supermembers as prefix for full name
               TString memberhandle =
                  TString::Format("%s[%d]", member->GetName(), x); // only local member for pointer handle
               AddSubHandle(handle, arraymember.Data(), colltypnm.Data(), innersize, memberoffset, memberhandle.Data(),
                            typenm.Data(), valueclass);
               memberoffset += collsize;
            }
            break;

         case 2:
            maxindex1 = member->GetMaxIndex(0);
            maxindex2 = member->GetMaxIndex(1);
            for (int x = 0; x < maxindex1; ++x)
               for (int y = 0; y < maxindex2; ++y) {
                  TString arraymember =
                     TString::Format("%s[%d][%d]", membername, x, y); // contains supermembers as prefix for full name
                  TString memberhandle =
                     TString::Format("%s[%d][%d]", member->GetName(), x, y); // only local member for pointer handle
                  AddSubHandle(handle, arraymember.Data(), colltypnm.Data(), innersize, memberoffset,
                               memberhandle.Data(), typenm.Data(), valueclass);
                  memberoffset += collsize;
               }
            break;

         }; // switch(arraydim)
         continue;
      }

      FillTypeInfo(handle, membername, memtypename, memberoffset, arraydim, member);

   }
}


 void TGo4HDF5Adapter::FillTypeInfo(TGo4HDF5DataHandle* handle,
       const char* membername, const char* memtypename, size_t memberoffset,
       Int_t arraydim, TDataMember* member)
 {
   H5::DataType theType;
   hsize_t maxindex1(1), maxindex2(1);

   go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  begins for type %s ...\n",memtypename);

   if ((strcmp(memtypename, "Char_t") == 0) || (strcmp(memtypename, "char") == 0))
      theType = H5::PredType::NATIVE_CHAR;
   else if ((strcmp(memtypename, "UChar_t") == 0) || (strcmp(memtypename, "unsigned char") == 0))
      theType = H5::PredType::NATIVE_UCHAR;
   else if ((strcmp(memtypename, "Short_t") == 0) || (strcmp(memtypename, "short") == 0))
      theType = H5::PredType::NATIVE_SHORT;
   else if ((strcmp(memtypename, "UShort_t") == 0) || (strcmp(memtypename, "unsigned short") == 0))
      theType = H5::PredType::NATIVE_USHORT;
   else if ((strcmp(memtypename, "Int_t") == 0) || (strcmp(memtypename, "int") == 0))
      theType = H5::PredType::NATIVE_INT;
   else if ((strcmp(memtypename, "UInt_t") == 0) || (strcmp(memtypename, "unsigned int") == 0))
      theType = H5::PredType::NATIVE_UINT;
   else if ((strcmp(memtypename, "ULong_t") == 0) || (strcmp(memtypename, "unsigned long") == 0))
      theType = H5::PredType::NATIVE_ULONG;
   else if ((strcmp(memtypename, "Double_t") == 0) || (strcmp(memtypename, "double") == 0))
      theType = H5::PredType::NATIVE_DOUBLE;
   else if ((strcmp(memtypename, "Float_t") == 0) || (strcmp(memtypename, "float") == 0))
      theType = H5::PredType::NATIVE_FLOAT;
   else if ((strcmp(memtypename, "Bool_t") == 0) || (strcmp(memtypename, "bool") == 0))
      theType = H5::PredType::NATIVE_HBOOL;
   else if ((strcmp(memtypename, "TString") == 0) || strstr(memtypename, "string")) {
      return; // skip for the moment names and text information TODO!
   }

   else if (strcmp(memtypename, "TGo4HDF5VectorProxy") == 0) {
      // this is dummy class to represent vector of vector entry, handle it manually without root
      go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  handles explicitly vector entry  %s\n", memtypename);

      TString innermemname;
      TGo4HDF5DataHandle innercomp(memtypename, sizeof(TGo4HDF5VectorProxy));
      innermemname = TString::Format("%s_fx_Begin_ptr", membername);
      innercomp.InsertTypeMember(innermemname.Data(), HOFFSET(TGo4HDF5VectorProxy, fx_Begin_ptr),
                                 H5::PredType::NATIVE_ULONG);
      innermemname = TString::Format("%s_fx_End_ptr", membername);
      innercomp.InsertTypeMember(innermemname.Data(), HOFFSET(TGo4HDF5VectorProxy, fx_End_ptr),
                                 H5::PredType::NATIVE_ULONG);
      innermemname = TString::Format("%s_fx_Cap_ptr", membername);
      innercomp.InsertTypeMember(innermemname.Data(), HOFFSET(TGo4HDF5VectorProxy, fx_Cap_ptr),
                                 H5::PredType::NATIVE_ULONG);
      theType = *(innercomp.GetType());
   }

   else {
      // evaluate structure components here
      TClass *innerclass = TClass::GetClass(memtypename);

      if (!innerclass)
         return;
      go4hdfdbg("TGo4HDF5Adapter::FillTypeInfo  finds root class info for type %s\n", memtypename);

      size_t innersize = innerclass->Size();
      // temporary datahandle just to extract the type components
      TGo4HDF5DataHandle innercomp(memtypename, innersize);
      FillTypeInfo(&innercomp, innerclass, membername);
      theType = *(innercomp.GetType());
   }

   const H5std_string theMEMBER(membername);

   switch (arraydim) {
   case 1: {
      maxindex1 = member->GetMaxIndex(0);
      hsize_t dims[1] = {maxindex1};
      H5::ArrayType theArray(theType, arraydim, dims);
      go4hdfdbg(
         "TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
         membername, memtypename, arraydim, maxindex1, memberoffset);
      handle->InsertTypeMember(theMEMBER, memberoffset, theArray);
      break;
   }
   case 2: {
      maxindex1 = member->GetMaxIndex(0);
      maxindex2 = member->GetMaxIndex(1);
      hsize_t dims[2] = {maxindex1, maxindex2};
      H5::ArrayType theArray(theType, arraydim, dims);
      go4hdfdbg(
         "TGo4HDF5Adapter::FillTypeInfo inserts array member %s, type %s dimension %d, maxindex:%lld , offset:%ld\n",
         membername, memtypename, arraydim, maxindex1, memberoffset);

      handle->InsertTypeMember(theMEMBER, memberoffset, theArray);
      break;
   }

   default:
      go4hdfdbg(
         "TGo4HDF5Adapter::FillTypeInfo inserts simple member %s, type %s dimension %d,  maxindex:%lld , offset:%ld\n",
         membername, memtypename, arraydim, maxindex1, memberoffset);

      handle->InsertTypeMember(theMEMBER, memberoffset, theType);
      break;

   } // switch()
 }

void TGo4HDF5Adapter::BuildDataType(TGo4EventElement* event, TGo4HDF5DataHandle* parent, Int_t index)
{
  TClass* evclass=event->Class();
  if(!evclass)
    {
      TGo4Log::Error("TGo4HDF5Adapter::BuildDataType can not find an event class\n");
      return;
    }
  TClass* actualclass=evclass->GetActualClass(event);
  TString actualclassname= actualclass->GetName();
  size_t eventsize=ScanEventSize(event);
  TGo4HDF5DataHandle* theHandle = nullptr;
  go4hdfdbg("TGo4HDF5Adapter::BuildDataType for class %s\n",
      actualclassname.Data());
  if(!parent)
  {
    // top level event, components are scanned recursively:
    fxHandle=new TGo4HDF5BasicDataHandle(actualclassname.Data(), eventsize);
    fxHandle->SetObjectPointer(event);
    FillTypeInfo(fxHandle, actualclass, actualclassname.Data());
    theHandle=fxHandle;
    fxHandle->SetTopEvent(event);
    fxHandle->SetTopEventClass(actualclassname.Data());

  }
  else
  {
    // component of a composite event: belongs to the parent handle
    TString compname=TString::Format("%s_%s(%d)",parent->GetTypeName(), actualclassname.Data(), index);
    TString comptype="Go4CompEv"; // do we need to tag the subhandles here?
    theHandle=parent->AddSubMember(compname.Data(), eventsize, comptype.Data());
    void* super=parent->Data(); // everything is relative to immediate mother element
    size_t delta= (char*) event - (char*) super;
    go4hdfdbg("TGo4HDF5Adapter::BuildDataType sets parent offset %ld (event:0x%lx, super:0x%lx)\n",
         delta, (unsigned long) event, (unsigned long) super);
    theHandle->SetParentOffset(delta);
    theHandle->SetObjectPointer(super);
    // prepend here the parent name to our subhandle:
    FillTypeInfo(theHandle, actualclass, theHandle->GetTypeName());

    // for reading back component data, each composite subevent has to override the top level parent event:
    theHandle->SetTopEvent(event);
    theHandle->SetTopEventClass(actualclassname.Data());

  }

// need to scan components of composite event here:
    TGo4CompositeEvent* comp= dynamic_cast<TGo4CompositeEvent*>(event);
    if(comp)
    {
      go4hdfdbg("TGo4HDF5Adapter::BuildDataType evaluates members of Go4 composite event %s\n",
           comp->GetName());
      Short_t numSubEvents=comp->getNElements();
      for (int i = 0; i < numSubEvents; ++i) {
         TGo4EventElement *sub = comp->getEventElement(i);
         if (!sub)
            continue;
         BuildDataType(sub, theHandle, i);
      }
    }

    // finally, set reference pointers for all evaluated subcomponents:
    if(!parent)
     {
      theHandle->SetObjectPointer(event); // set it here, because parentoffsets of subcomponents may have changed at recursive evaluation
     }
}



UInt_t TGo4HDF5Adapter::ConvertFileMode(Go4_H5_File_Flags flags)
{
   UInt_t h5flags = 0;

   switch (flags) {
      case GO4_H5F_ACC_NONE:
      case GO4_H5F_ACC_TRUNC: h5flags = H5F_ACC_TRUNC; break;
      case GO4_H5F_ACC_EXCL: h5flags = H5F_ACC_EXCL; break;
      case GO4_H5F_ACC_RDONLY: h5flags = H5F_ACC_RDONLY; break;
      case GO4_H5F_ACC_RDWR: h5flags = H5F_ACC_RDWR; break;
      default: h5flags = H5F_ACC_TRUNC; break;
   }
   return h5flags;
}
