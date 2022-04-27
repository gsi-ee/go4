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



TGo4HDF5Source::TGo4HDF5Source(const char* name) :
   TGo4EventSource(name), TGo4HDF5Adapter(), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
{
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, Form("!!! ERROR: Cannot open source %s!!!", GetName()));
}

TGo4HDF5Source::TGo4HDF5Source(TGo4HDF5SourceParameter* par) :
   TGo4EventSource(par->GetName()), TGo4HDF5Adapter(), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
{
   // TODO: all file regexp magic into base class for all eventsources
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, Form("!!! ERROR: Cannot open source %s!!!", GetName()));
}


TGo4HDF5Source::TGo4HDF5Source() :
   TGo4EventSource("Go4HDF5Source"),TGo4HDF5Adapter(), fxReadBuffer(0), fiReadOffset(0),fxFilesNames(0)
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

#ifdef _MSC_VER
   if (slash < 0) slash = basename.Last('\\');
#endif

   if (slash >= 0) {
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
     SetErrMess(msg.Data());
     throw TGo4EventSourceException(this);
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

   BuildDataType(event);
   fxHandle->BuildReadDataset(fxFile, this);
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

#ifdef GO4HDF5_DEBUG
        printf("TGo4HDF5Source: fxEvent=0x%lx\n", (unsigned long) fxEvent);
        printf("TGo4HDF5Source: Eventname:%s\n", fxEvent->GetName());
        printf("TGo4HDF5Source: is valid:%d\n", fxEvent->IsValid());
        printf("Go4 event has eventsource pointer 0x%lx  \n",(long) fxEvent->GetEventSource());
        printf("TGo4HDF5Source: Event printout:\n");
        fxEvent->PrintEvent();
       printf("\n");
#endif

    //fxHandle->SetObjectPointer(dest); // do it here to account dynamic changes in structure
    fxHandle->Read(fiFillCount++, fxFile);

#ifdef GO4HDF5_DEBUG
         printf("Go4 event has eventsource pointer 0x%lx  \n",(long) fxEvent->GetEventSource());
         printf("Go4 event has identifier 0x%lx  \n",(long) fxEvent->getId());
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


