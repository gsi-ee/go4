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

#include "TGo4MbsFile.h"

#include <string.h>

#include "Riostream.h"
#include "Riosfwd.h"
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
   fxMultiName(),
   fxMultiFile(0),
   fbFileOpen(kFALSE)
{
   TRACE((15,"TGo4MbsFile::TGo4MbsFile()",__LINE__, __FILE__));
}

TGo4MbsFile::TGo4MbsFile(const char* name) :
   TGo4MbsSource(name , GETEVT__FILE),
   fxMultiName(),
   fxMultiFile(0),
   fbFileOpen(kFALSE)
{
   TRACE((15,"TGo4MbsFile::TGo4MbsFile(const char*)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsFile %s:  ",name);
   fxTagFile = fgcNOTAGFILE;
   Open();
}


TGo4MbsFile::TGo4MbsFile(TGo4MbsFileParameter* par) :
   TGo4MbsSource(par , GETEVT__FILE),
   fxMultiName(),
   fxMultiFile(0),
   fbFileOpen(kFALSE)
{
   TRACE((15,"TGo4MbsFile::TGo4MbsFile(TGo4MbsFileParameter**)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Event Source MbsFile %s:  ",GetName());
   if(par!=0) fxTagFile=par->GetTagName();

   Open();
}

TGo4MbsFile::~TGo4MbsFile()
{
  TRACE((15,"TGo4MbsFile::~TGo4MbsFile()",__LINE__, __FILE__));
  Close();
}

Int_t TGo4MbsFile::NextEvent()
{
   TRACE((12,"TGo4MbsFile::NextEvent()",__LINE__, __FILE__));
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
            fbFirstEvent=kFALSE;
         } else {
            skip= (Int_t) fuEventInterval;
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
         while(NextFile()<0);
         //skip filenames with open error until a file
         // in the list opens properly (retval==0)
         SetErrMess(Form("\n       Eventsource   Open file: %s tagfile:%s first:%lu last:%lu delta:%lu",
               GetCurrentFileName(),GetCurrentTagName(), fuStartEvent,fuStopEvent, fuEventInterval));
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
   TRACE((12,"TGo4MbsFile::Close()",__LINE__, __FILE__));
   if(!fbIsOpen) return -1;

   Int_t rev = GetCreateStatus();
   // close connection/file
   if(CloseFile() == GETEVT__SUCCESS) fbIsOpen = kFALSE;

   if(fxMultiFile) { delete fxMultiFile; fxMultiFile=0; }

   return rev;
}

Int_t TGo4MbsFile::Open()
{
   TRACE((12,"TGo4MbsFile::Open()",__LINE__, __FILE__));

   if(fbIsOpen) return -1;

   bool read_multi = false;

   // evaluate wildcard input:
   if(*GetName()=='@') { // old style: filelist name starts with @
      // name indicates steering file
      fxMultiName = GetName() + 1;
      read_multi = true;
   } else
   if(fName.EndsWith(fgcFILELISTSUF)) { // new style: list mode list
      fxMultiName = GetName();
      read_multi = true;
   } else
   if(strstr(GetName(),"*") || strstr(GetName(),"?")) {
      // name indicates wildcard expression
      fxMultiName = GetName();
      fxMultiFile = TGo4FileSource::ProducesFilesList(GetName());

      if (fxMultiFile==0) {
         SetCreateStatus(GETEVT__NOFILE);
         SetErrMess(Form("No lmd file with mask %s",GetName()));
         throw TGo4EventErrorException(this);
      }

   } else {
      // name is name of single input lmd file
   }


    /////////////////////////////
   // now treat different input modes:
   if(read_multi) {
      std::ifstream ff(fxMultiName.Data());
      if(!ff) {
         SetCreateStatus(GETEVT__NOFILE);
         SetErrMess(Form("Error opening multiple file:%s",fxMultiName.Data()));
         throw TGo4EventErrorException(this);
      }

      fxMultiFile = new TList;
      fxMultiFile->SetOwner(kTRUE);

      char nextline[TGo4EventSource::fguTXTLEN];

      while (!ff.eof()) {
         ff.getline(nextline,TGo4EventSource::fguTXTLEN, '\n');
         fxMultiFile->Add(new TObjString(nextline));
      }

      while(NextFile()<0); // skip invalid filenames
      // note that TGo4EventEndException will break this loop if no valid file in list
      fbIsOpen = kTRUE;
      NewFileAction(kFALSE);
      TGo4Analysis::Instance()->Message(1,Form(
            "Eventsource TGo4MbsFile:%s \n       Eventsource   Open file: %s tag:%s first:%lu last:%lu delta:%lu",
            GetName(),GetCurrentFileName(),GetCurrentTagName(), fuStartEvent,fuStopEvent, fuEventInterval));
   } else {
      //cout <<"Open in single mode" << endl;
      if(NextFile()<0)
         fbIsOpen=kFALSE; // only for single mode the
      // error result of first NextFile()
      // will indicate that open failed
      else {
         fbIsOpen=kTRUE;
         TGo4Log::Info("TGo4MbsFile: Open file %s", GetCurrentFileName());
      }

   }
   // note that open flag indicates state of complete mbssource,
   // not the state of one single file in the multiple file list.



//////////////////// provide dummy event for testing
//int fiNumSub=2; // number of subevents, fix
//int fiNumDat=16; // maximum allocated data longs per subevent
//int fiDLen=(sizeof(s_ve10_1)-sizeof(s_evhe)+fiNumSub*(sizeof(s_ves10_1)+fiNumDat*sizeof(Int_t))) / 2 ;
//fxEvent= (s_ve10_1*) new Short_t[fiDLen+sizeof(s_evhe)];   ;
//fxEvent->l_dlen=fiDLen;
//fxEvent->i_subtype=1;
//fxEvent->i_type=10;
//fxEvent->l_count=0;
//s_ves10_1* subevt=(s_ves10_1*) (void*) (fxEvent+1);
//for(Int_t i=0;i<fiNumSub;++i)
//   {
//      //cout <<"\tSubevt "<<i <<" at "<< subevt<< endl;
//      int l_val_num = (int)(10); // random number for number of data longwords
//      if(l_val_num>fiNumDat) l_val_num=fiNumDat; // never exceed allocated field
//      // setup subevent header:
//      subevt->i_type=10;
//      subevt->i_subtype=1;
//      subevt->h_subcrate=i+1; // set subevent id numbers:
//      subevt->h_control=2*i;
//      subevt->i_procid=4*i;
//      //subevt->l_dlen=fiNumDat*2+2; // length in short units + 2
//      subevt->l_dlen=l_val_num*2+2; // subevent length in short units + 2
//      fxEvent->l_dlen+=(l_val_num*sizeof(Int_t)) / 2 ; // add datalength to total length in shorts
//
//      //cout <<"\t dlen="<<subevt->l_dlen << endl;
//      Int_t* subdata= (Int_t*) (subevt+1); // data starts after subevt
//      //cout <<"\t data="<<subdata << endl;
//      for(Int_t j=0;j<l_val_num;++j)
//         {
//             *(subdata+j)=j; // later use random generator here
//             //cout <<"\t\t"<<"filled "<<j<<" with "<<*(subdata+j) <<"at "<<(subdata+j) << endl;
//         } // for (... numdat ...)
//      subevt=(s_ves10_1*) (subdata+l_val_num); // next subheader after last data
//   } // for(...numsub)
//   fxEvent->l_dlen+=(sizeof(s_ve10_1)-sizeof(s_evhe)+fiNumSub*sizeof(s_ves10_1))/2;
//// finally, add length of headers  to totalevent length
/////// end dummy event

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
         //cout <<"read line "<<cnt++<<" : "<<nextline << endl;
         //if(fxMultiFile->rdstate()==ios::eofbit)
         if((fxMultiFile==0) || (fxMultiFile->GetSize()==0)) {
            // reached last filename, or read error?
            SetCreateStatus(GETEVT__NOFILE);
            SetErrMess(Form("End of files list %s", fxMultiName.Data()));
            //throw TGo4EventErrorException(this,3);
            throw TGo4EventEndException(this);
         }

         TObject* obj = fxMultiFile->First();
         nextline = obj->GetName();
         fxMultiFile->Remove(fxMultiFile->FirstLink());
         delete obj;

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
      //     cout <<"Read next filename "<<nextfile<<" and tag "<<nexttag << endl;
      //     cout <<"Got Start:"<<fuStartEvent<<". stop:"<<fuStopEvent,
      //     cout <<", interval:" <<fuEventInterval<< endl;
      fxCurrentFile = nextfile;
      if(!strcmp(nexttag,"0") || !strcmp(nexttag,""))
         fxCurrentTag=TGo4MbsFile::fgcNOTAGFILE; // no tagfile if no name
      else
         fxCurrentTag=nexttag;
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
   TGo4Analysis* ana=TGo4Analysis::Instance();
   ana->SetNewInputFile(kTRUE);
   if(ana->IsAutoSaveFileChange()) {
      TString fname=GetCurrentFileName();
      fname.ReplaceAll(".lmd",4,"_ASF",4);
      if(dosave) ana->AutoSave();
      ana->ClearObjects("Histograms");
      TString asfname=fname+".root";
      cout <<"Setting autosavefile to name "<<asfname << endl;
      ana->SetAutoSaveFile(asfname.Data());
      if(dosave) ana->AutoSave();
   }
   return 0;
}

const char* TGo4MbsFile::GetActiveName()
{
   return GetCurrentFileName();
}
