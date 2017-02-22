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

#include "TGo4MbsFile.h"

#include <string.h>

#include "Riostream.h"
#include "RVersion.h"
#if ROOT_VERSION_CODE <= ROOT_VERSION(6,8,0)
#include "Riosfwd.h"
#else
#include <iosfwd>
#endif
#include "TSystem.h"
#include "TROOT.h"
#include "TList.h"
#include "TObjString.h"

#include "TGo4Log.h"

#include "TGo4MbsFileParameter.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4EventEndException.h"
#include "TGo4FileSource.h"
#include "TGo4AnalysisImp.h"

const char* TGo4MbsFile::fgcNOTAGFILE="GO4-NOLMDTAG";
const char* TGo4MbsFile::fgcLMDSUF=".lmd";
const char* TGo4MbsFile::fgcFILELISTSUF=".lml";

TGo4MbsFile::TGo4MbsFile() :
   TGo4MbsSource(),
   fxTagFile(),
   fxCurrentFile(),
   fxCurrentTag(),
   fxMultiFile(0),
   fbFileOpen(kFALSE),
   fbShowInfo(kTRUE)
{
   GO4TRACE((15,"TGo4MbsFile::TGo4MbsFile()",__LINE__, __FILE__));
}

TGo4MbsFile::TGo4MbsFile(const char* name) :
   TGo4MbsSource(name, GETEVT__FILE),
   fxTagFile(),
   fxCurrentFile(),
   fxCurrentTag(),
   fxMultiFile(0),
   fbFileOpen(kFALSE),
   fbShowInfo(kTRUE)
{
   GO4TRACE((15,"TGo4MbsFile::TGo4MbsFile(const char*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsFile %s:  ", GetName());
   fxTagFile = fgcNOTAGFILE;

   AddFileName(name, 0, kTRUE);

   Open();
}

TGo4MbsFile::TGo4MbsFile(TGo4MbsFileParameter* par) :
   TGo4MbsSource(par, GETEVT__FILE),
   fxTagFile(),
   fxCurrentFile(),
   fxCurrentTag(),
   fxMultiFile(0),
   fbFileOpen(kFALSE),
   fbShowInfo(kTRUE)
{
   GO4TRACE((15,"TGo4MbsFile::TGo4MbsFile(TGo4MbsFileParameter*)",__LINE__, __FILE__));

   if(par!=0) {
      fxTagFile = par->GetTagName();

      AddFileName(GetName(), fxTagFile.Data(), par->NumMoreFiles()==0);
      for (Int_t n=0;n<par->NumMoreFiles();n++)
         AddFileName(par->GetMoreName(n), 0, kFALSE);
   }

   TGo4Log::Debug("New Event Source MbsFile %s", GetName());

   Open();
}

TGo4MbsFile::~TGo4MbsFile()
{
   GO4TRACE((15,"TGo4MbsFile::~TGo4MbsFile()",__LINE__, __FILE__));
   Close();

   if(fxMultiFile) { delete fxMultiFile; fxMultiFile=0; }
}

void TGo4MbsFile::AddFileName(const char* name, const char* tagname, bool isonly)
{
   if ((name==0) || (*name==0)) return;

   TString fname(name);

   bool read_multi = false;

   // evaluate wildcard input:
   if(fname[0]=='@') { // old style: filelist name starts with @
      // name indicates steering file
      fname.Remove(0, 1);
      read_multi = true;
   } else
   if(fname.EndsWith(fgcFILELISTSUF)) { // new style: list mode list
      read_multi = true;
   } else
   if(fname.Contains("*") || fname.Contains("?")) {
      // name indicates wildcard expression
      TList* lst = TGo4FileSource::ProducesFilesList(fname.Data());

      if (lst==0) {
         SetCreateStatus(GETEVT__NOFILE);
         SetErrMess(Form("No lmd file with mask %s", GetName()));
         throw TGo4EventErrorException(this);
      }

      if (fxMultiFile==0) {
         fxMultiFile=lst;
      } else {
         lst->SetOwner(kFALSE);
         fxMultiFile->AddAll(lst);
         delete lst;
      }

   } else
   if (!isonly) {
      // only if more file names are expected we will start extra list with files names

      if (fxMultiFile==0) { fxMultiFile = new TList; fxMultiFile->SetOwner(kTRUE); }

      if ((tagname!=0) && (strcmp(tagname,fgcNOTAGFILE)!=0)) {
         fname += " ";
         fname += tagname;
      }

      fxMultiFile->Add(new TObjString(fname));
   }

    /////////////////////////////
   // now treat different input modes:
   if(read_multi) {
      std::ifstream ff(fname.Data());
      if(!ff) {
         SetCreateStatus(GETEVT__NOFILE);
         SetErrMess(Form("Error opening multiple file:%s", fname.Data()));
         throw TGo4EventErrorException(this);
      }

      if (fxMultiFile==0) {
         fxMultiFile = new TList;
         fxMultiFile->SetOwner(kTRUE);
      }

      char nextline[TGo4EventSource::fguTXTLEN];

      while (!ff.eof()) {
         ff.getline(nextline, TGo4EventSource::fguTXTLEN, '\n');

         fxMultiFile->Add(new TObjString(nextline));
      }
   }

   if (isonly) SetName(fname);
          else SetName("LmdFilesSelection");
}

Int_t TGo4MbsFile::NextEvent()
{
   if (fbShowInfo) {
      fbShowInfo = kFALSE;
      TGo4Log::Info("Start file: %s tagfile:%s first:%lu last:%lu delta:%lu",GetCurrentFileName(),GetCurrentTagName(), fuStartEvent,fuStopEvent, fuEventInterval);
   }

   GO4TRACE((12,"TGo4MbsFile::NextEvent()",__LINE__, __FILE__));
   try{
      Int_t skip = 0;
      // test if we had reached the last event:
      if(fuStopEvent!=0 && fuEventCounter>=fuStopEvent) {
         SetEventStatus(GETEVT__NOMORE);
      } else {
         if(fbFirstEvent) {
            if(fuStartEvent>0) {
               // we want to work with "real" event number for first event
               skip = (Int_t) fuStartEvent-1;
               if(skip) fuEventCounter++; // need to correct for init value of event counter below!
            } else {
               skip = 0;
            }
            fbFirstEvent = kFALSE;
         } else {
            skip = (Int_t) fuEventInterval;
         }
         void* evptr = &fxEvent; // some new compilers may warn if we directly dereference member variable in function argument
         Int_t status = f_evt_get_tagnext(fxInputChannel, skip, (Int_t **) evptr);
         if(skip)
            fuEventCounter+=skip;
         else
            fuEventCounter++;
         SetEventStatus(status);
      }
      if(GetEventStatus()!=0) {
         char buffer[TGo4EventSource::fguTXTLEN];
         f_evt_error(GetEventStatus(),buffer,1); // provide text message for later output
         SetErrMess(Form("%s file:%s", buffer, GetCurrentFileName()));
      }

      if(GetEventStatus() == GETEVT__NOMORE) throw TGo4EventEndException(this);
      else  if(GetEventStatus()!=0) throw TGo4EventErrorException(this);

      return GetEventStatus();

   } // try

   catch(TGo4EventEndException& ex) {
      if(fxMultiFile) {
         ex.Handle(); // display message
         // catch here the end of file case only
         // try to open next file in list:
         TGo4Log::Info("End file: %s",GetCurrentFileName());

         while(NextFile()<0);
         //skip filenames with open error until a file
         // in the list opens properly (retval==0)
         SetErrMess("");
         NewFileAction();
         throw TGo4EventErrorException(this,0);
         // priority 0 means do not stop analysis
         // we (mis-)use an error exception with no stop to
         // skip the processing of the previous event  in the
         // subsequent analysis for a second time
         // note that NextFile() throws an exception itself
         // if list of files is at end
      } else {
         throw;  // normal end of input for one file
      }
   }
}


Int_t TGo4MbsFile::Close()
{
   GO4TRACE((12,"TGo4MbsFile::Close()",__LINE__, __FILE__));
   if(!fbIsOpen) return -1;

   Int_t rev = GetCreateStatus();
   // close connection/file
   if(CloseFile() == GETEVT__SUCCESS) fbIsOpen = kFALSE;

   if(fxMultiFile) { delete fxMultiFile; fxMultiFile=0; }

   return rev;
}

Int_t TGo4MbsFile::Open()
{
   GO4TRACE((12,"TGo4MbsFile::Open()",__LINE__, __FILE__));

   if(fbIsOpen) return -1;

    /////////////////////////////
   // now treat different input modes:
   if(fxMultiFile!=0) {

      while(NextFile()<0); // skip invalid filenames
      // note that TGo4EventEndException will break this loop if no valid file in list
      fbIsOpen = kTRUE;
      NewFileAction(kFALSE);
   } else {
      //std::cout <<"Open in single mode" << std::endl;
      if(NextFile()<0) {
         // only for single mode the
        // error result of first NextFile()
        // will indicate that open failed
        fbIsOpen = kFALSE;
      } else {
         fbIsOpen = kTRUE;
         TGo4Log::Info("TGo4MbsFile: Open file %s", GetCurrentFileName());
      }
   }

   return 0;
}


Int_t TGo4MbsFile::NextFile()
{
   CloseFile();
   fuEventCounter=0;
   // read next name from namesfile
   if(fxMultiFile!=0) {
      TString nextline;
      char nextfile[TGo4EventSource::fguTXTLEN];
      char nexttag[TGo4EventSource::fguTXTLEN];
      const char* command=0;
      const char* rem1=0;
      const char* rem2=0;
      Int_t convs=0;
      //static int cnt=0;
      do {
         //std::cout <<"read line "<<cnt++<<" : "<<nextline << std::endl;
         //if(fxMultiFile->rdstate()==ios::eofbit)
         if((fxMultiFile==0) || (fxMultiFile->GetSize()==0)) {
            // reached last filename, or read error?
            SetCreateStatus(GETEVT__NOFILE);
            SetErrMess("End of files list");
            //throw TGo4EventErrorException(this,3);
            throw TGo4EventEndException(this);
         }

         TObject* obj = fxMultiFile->First();
         nextline = obj->GetName();
         fxMultiFile->Remove(fxMultiFile->FirstLink());
         delete obj;

         // this indicates that we will show file info when first event will be extracted
         fbShowInfo = kTRUE;

         rem1 = strstr(nextline.Data(), "!");
         rem2 = strstr(nextline.Data(), "#");
         command = strstr(nextline.Data(), "@");
         if(command!=0 && !(rem1!=0 && rem1<command) && !(rem2!=0 && rem2<command)) {
            // keycharacter indicates we want to execute a root macro
            // treat the case that @command is commented out before!
            command++; // skip @ letter
            TGo4Analysis::Instance()->Message(1,"TGo4MbsFile list:%s-- executing command: %s ", GetName(), command);
            //TGo4Log::Info("TGo4MbsFile list:%s-- executing command: %s ", GetName(), command);
            gROOT->ProcessLineSync(command);
         }
      } while((nextline.Length()==0) || rem1!=0 || rem2!=0 || command!=0); // skip any comments and empty lines, and continue after macro execution
      convs = sscanf(nextline.Data(),"%s %s %lu %lu %lu",nextfile,nexttag,
                     &fuStartEvent, &fuStopEvent, &fuEventInterval);
      if(convs<2) {
         // line contained not all parameters, reset remaining
         fuStartEvent=0;
         fuStopEvent=0;
         fuEventInterval=0;
         strncpy(nexttag, TGo4MbsFile::fgcNOTAGFILE, TGo4EventSource::fguTXTLEN);
      }
      //     std::cout <<"Read next filename "<<nextfile<<" and tag "<<nexttag << std::endl;
      //     std::cout <<"Got Start:"<<fuStartEvent<<". stop:"<<fuStopEvent,
      //     std::cout <<", interval:" <<fuEventInterval<< std::endl;
      fxCurrentFile = nextfile;
      if(!strcmp(nexttag,"0") || !strcmp(nexttag,""))
         fxCurrentTag = TGo4MbsFile::fgcNOTAGFILE; // no tagfile if no name
      else
         fxCurrentTag = nexttag;
   } else {
      //no multiple file: use default names
      fxCurrentTag = GetTagName();
      fxCurrentFile = GetName();
   }

   try {
      OpenFile();
      return 0;
   }// try

   catch(TGo4EventErrorException& ex)
   {
      if(fxMultiFile) {
         // something went wrong opening next file, skip it
         ex.Handle();
         CloseFile();
         return -1;
      } else {
         throw;
      }

   }
}

Int_t TGo4MbsFile::OpenFile()
{
   if(fbFileOpen) return -1;

   const char* tagfile = GetCurrentTagName();
   if(!strcmp(tagfile,TGo4MbsFile::fgcNOTAGFILE)) {
      tagfile=0;
      fxCurrentTag="none"; // looks better in display message
   }
   void* headptr=&fxInfoHeader; // some new compilers may warn if we directly dereference member
   Int_t status = f_evt_get_tagopen(fxInputChannel,
                                    const_cast<char*>(tagfile),
                                    const_cast<char*>(GetCurrentFileName()),
                                    (Char_t**) headptr, 0);
   SetCreateStatus(status);
   if(GetCreateStatus() !=GETEVT__SUCCESS) {
      char buffer[TGo4EventSource::fguTXTLEN];
      f_evt_error(GetCreateStatus(),buffer,1); // provide text message for later output
      SetErrMess(Form("%s file:%s", buffer, GetCurrentFileName()));
      fbFileOpen = kFALSE;
      throw TGo4EventErrorException(this);
   } else {
      fbFileOpen = kTRUE;
      fbFirstEvent = kTRUE;
      TGo4Log::Debug(" Mbs File -- opened %s ", GetName());
   }
   return status;
}



Int_t TGo4MbsFile::CloseFile()
{
   if(!fbFileOpen) return -1;
   Int_t rev = f_evt_get_tagclose(fxInputChannel);
   if(rev == GETEVT__SUCCESS) fbFileOpen = kFALSE;
   return rev;
}

Int_t TGo4MbsFile::NewFileAction(Bool_t dosave)
{
   TGo4Analysis* ana = TGo4Analysis::Instance();
   ana->SetNewInputFile(kTRUE);
   ana->SetInputFileName(GetCurrentFileName());
   if(ana->IsAutoSaveFileChange()) {
      TString fname = GetCurrentFileName();
      fname.ReplaceAll(".lmd",4,"_ASF",4);
      if(dosave) ana->AutoSave();
      ana->ClearObjects("Histograms");
      TString asfname = fname+".root";
      std::cout << "Setting autosavefile to name " << asfname << std::endl;
      ana->SetAutoSaveFile(asfname.Data());
      if(dosave) ana->AutoSave();
   }
   return 0;
}

const char* TGo4MbsFile::GetActiveName()
{
   return GetCurrentFileName();
}
