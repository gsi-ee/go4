// $Id: TGo4AnalysisWebStatus.cxx 1396 2015-02-20 15:11:10Z adamczew $
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

#include "TGo4AnalysisWebStatus.h"

#include "TUrl.h"
#include "TClass.h"

#include "TGo4AnalysisStepStatus.h"

#include "TGo4Analysis.h"
#include "TGo4AnalysisClient.h"
//#include "TGo4LockGuard.h"

TString TGo4AnalysisWebStatus::fgxURL_STARTRUN = "start";
TString TGo4AnalysisWebStatus::fgxURL_CLOSE = "close";

TString TGo4AnalysisWebStatus::fgxURL_ENABLESTEP = "stepenabled";
TString TGo4AnalysisWebStatus::fgxURL_ENABLESOURCE = "sourceenabled";
TString TGo4AnalysisWebStatus::fgxURL_ENABLESTORE = "storeenabled";

TString TGo4AnalysisWebStatus::fgxURL_SOURCE_TYPE = "sourcesel";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_NAME = "sourcename";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_PORT = "sourceport";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_TIMEOUT = "sourcetmout";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_RETRY = "sourceretry";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_FIRST = "sourcefirst";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_LAST = "sourcelast";
TString TGo4AnalysisWebStatus::fgxURL_SOURCE_SKIP = "sourceskip";
TString TGo4AnalysisWebStatus::fgxURL_STORE_TYPE = "storesel";
TString TGo4AnalysisWebStatus::fgxURL_STORE_NAME = "storename";
TString TGo4AnalysisWebStatus::fgxURL_STORE_SPLIT = "storesplit";
TString TGo4AnalysisWebStatus::fgxURL_STORE_BUF = "storebuf";
TString TGo4AnalysisWebStatus::fgxURL_STORE_COMP = "storecomp";
TString TGo4AnalysisWebStatus::fgxURL_STORE_OVERWRITE = "storeover";

TString TGo4AnalysisWebStatus::fgxURL_ASF_SAVE = "saveasf";
TString TGo4AnalysisWebStatus::fgxURL_ASF_ENABLED = "asfenabled";
TString TGo4AnalysisWebStatus::fgxURL_ASF_NAME = "asfname";
TString TGo4AnalysisWebStatus::fgxURL_ASF_INTERVAL = "asftime";
TString TGo4AnalysisWebStatus::fgxURL_ASF_COMP = "asfcomp";
TString TGo4AnalysisWebStatus::fgxURL_ASF_OVERWRITE = "asfoverwrite";

TString TGo4AnalysisWebStatus::fgxURL_PREFS_NAME = "anaprefsname";
TString TGo4AnalysisWebStatus::fgxURL_PREFS_LOAD = "loadprefs";
TString TGo4AnalysisWebStatus::fgxURL_PREFS_SAVE = "saveprefs";


// Method TUrl::HasOption is not available for older ROOT versions, use workaround:
#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,20)
#define HasOption(x) GetValueFromOptions(x)!=0
#endif



Bool_t TGo4AnalysisWebStatus::UpdateFromUrl(const char* rest_url_opt)
{
  std::cout << "\nTGo4AnalysisWebStatus Update From Url with " << rest_url_opt << std::endl;
//  TGo4LockGuard gard; // test: can we avoid deadlock with monitoring sniffer here?
//  std::cout << "\nTGo4AnalysisWebStatus AFTER lockguard." << std::endl;
  TString message;
  message.Form("TGo4AnalysisWebStatus::UpdateFromUrl ");
  TGo4Analysis* ana = TGo4Analysis::Instance();
  TUrl url;
  url.SetOptions(rest_url_opt);    // this time try to use ROOT default class
  TString theKey;

  //handle first  pseudo commands that should not initialize analysis:

  // close analysis:
  theKey = TGo4AnalysisWebStatus::fgxURL_CLOSE;
  if (url.HasOption(theKey.Data()))
  {
     ana->StopAnalysis();
     ana->CloseAnalysis();
     message.Append(TString::Format(", CLOSING analysis"));
     TGo4Log::Message(1, message.Data());
     return kTRUE;
  }

  // save asf:
  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_SAVE;
  if (url.HasOption(theKey.Data()))
  {
    const char* filename = url.GetValueFromOptions(theKey.Data());
    WriteAutoSave(filename, fbAutoSaveOverwrite, fiAutoSaveCompression);
    message.Append(TString::Format(", saving autosave file %s", filename));
    TGo4Log::Message(1, message.Data());
    return kTRUE;
  }

  // save setup:
  theKey = TGo4AnalysisWebStatus::fgxURL_PREFS_SAVE;
  if (url.HasOption(theKey.Data()))
  {
    const char* filename = url.GetValueFromOptions(theKey.Data());
    SaveStatus(filename);
    message.Append(TString::Format(", saving configuration file %s", filename));
    TGo4Log::Message(1, message.Data());
    return kTRUE;
  }

  // load setup and initialize:
  theKey = TGo4AnalysisWebStatus::fgxURL_PREFS_LOAD;
  if (url.HasOption(theKey.Data()))
  {
    const char* filename = url.GetValueFromOptions(theKey.Data());
    ana->StopAnalysis();
    if (LoadStatus(filename))
    {
      ana->InitEventClasses();
      message.Append(TString::Format(", loaded configuration file %s", filename));
    }
    else
    {
      message.Append(TString::Format(", /!\\ FAILED to load configuration file %s !!!", filename));
    }
    TGo4Log::Message(1, message.Data());
    return kTRUE;
  }

  // first stepwise options here:

  ResetStepIterator();
  TGo4AnalysisStepStatus* step = 0;
  Int_t stepindex = 0;
  while ((step = NextStepStatus()) != 0)
  {
    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_ENABLESTEP.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {

      //std::cout <<"---- found stepenabled key:"<<theKey.Data()<< std::endl;
      Bool_t enablestep = ((TString(url.GetValueFromOptions(theKey.Data())) == "true") ? kTRUE : kFALSE);
      message.Append(TString::Format(", %s=%d", theKey.Data(), enablestep));
      step->SetProcessEnabled(enablestep);
    }
    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_ENABLESOURCE.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      //std::cout <<"---- found sourceenabled key:"<<theKey.Data()<< std::endl;
      Bool_t enablesource = ((TString(url.GetValueFromOptions(theKey.Data())) == "true") ? kTRUE : kFALSE);
      message.Append(TString::Format(", %s=%d", theKey.Data(), enablesource));
      step->SetSourceEnabled(enablesource);
    }
    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_ENABLESTORE.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      //std::cout <<"---- found storeenabled key:"<<theKey.Data()<< std::endl;
      Bool_t enablestore = ((TString(url.GetValueFromOptions(theKey.Data())) == "true") ? kTRUE : kFALSE);
      message.Append(TString::Format(", %s=%d", theKey.Data(), enablestore));
      step->SetStoreEnabled(enablestore);
    }

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_TYPE.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t srctype = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), srctype));

      // extract values of old source parameter and save them into the new if matching:
      TGo4EventSourceParameter* oldsrcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* oldmbspar = dynamic_cast<TGo4MbsSourceParameter*>(oldsrcpar);
      TGo4UserSourceParameter* olduserpar = dynamic_cast<TGo4UserSourceParameter*>(oldsrcpar);
      TString srcname = oldsrcpar->GetName();
      Int_t port = oldmbspar ? oldmbspar->GetPort() : 0;
      if (port == 0)
        port = olduserpar ? olduserpar->GetPort() : 0;
      // JAM: the above shows that event source hierarchy interface is still not optimized
      // however, currently (March 2015) no manpower to improve or redesign Go4 if it is working...

      TGo4EventSourceParameter* newpar = 0;
      switch (srctype)
      {
        case GO4EV_FILE:
          newpar = new TGo4FileSourceParameter(srcname);
          break;

        case GO4EV_MBS_FILE:
          newpar = new TGo4MbsFileParameter(srcname);
          break;

        case GO4EV_MBS_STREAM:
          newpar = new TGo4MbsStreamParameter(srcname);
          break;

        case GO4EV_MBS_TRANSPORT:
          newpar = new TGo4MbsTransportParameter(srcname);
          break;

        case GO4EV_MBS_EVENTSERVER:
          newpar = new TGo4MbsEventServerParameter(srcname);
          break;

        case GO4EV_MBS_REVSERV:
          newpar = new TGo4RevServParameter(srcname);
          break;

        case GO4EV_MBS_RANDOM:
          newpar = new TGo4MbsRandomParameter("random");
          break;

        case GO4EV_USER:
          newpar = new TGo4UserSourceParameter(srcname, "", port);
          break;
      };

      TGo4MbsSourceParameter* newmbspar = dynamic_cast<TGo4MbsSourceParameter*>(newpar);
      if (newmbspar)
      {
        newmbspar->SetPort(port);
        if (oldmbspar)
        {
          newmbspar->SetStartEvent(oldmbspar->GetStartEvent());
          newmbspar->SetStopEvent(oldmbspar->GetStopEvent());
          newmbspar->SetEventInterval(oldmbspar->GetEventInterval());
        }
      }
      newpar->SetTimeout(oldsrcpar->GetTimeout());

      step->SetSourcePar(newpar);
      delete newpar;    // SetSourcePar does clone argument, we do not need it further
    }    // fgxURL_SOURCE_TYPE

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_NAME.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      TString srcname = url.GetValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%s", theKey.Data(), srcname.Data()));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      srcpar->SetName(srcname.Data());

    }    // fgxURL_SOURCE_NAME;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_PORT.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t port = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), port));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*>(srcpar);
      TGo4UserSourceParameter* userpar = dynamic_cast<TGo4UserSourceParameter*>(srcpar);
      if (mbspar)
        mbspar->SetPort(port);
      else if (userpar)
        userpar->SetPort(port);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: Could not set port to type %s ", srcpar->ClassName()));
    }    //fgxURL_SOURCE_PORT;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_TIMEOUT.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t timeout = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), timeout));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      srcpar->SetTimeout(timeout);
    }    //fgxURL_SOURCE_TIMEOUT;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_RETRY.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t retry = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), retry));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*>(srcpar);
      if (mbspar)
        mbspar->SetRetryCnt(retry);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: Could not set retry count to type %s ",
                srcpar->ClassName()));
    }    //fgxURL_SOURCE_RETRY;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_FIRST.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t startevent = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), startevent));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*>(srcpar);
      if (mbspar)
        mbspar->SetStartEvent(startevent);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: Could not set start event to type %s ",
                srcpar->ClassName()));
    }    //fgxURL_SOURCE_FIRST;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_LAST.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t stopevent = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), stopevent));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*>(srcpar);
      if (mbspar)
        mbspar->SetStopEvent(stopevent);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: Could not set stop event to type %s ",
                srcpar->ClassName()));
    }    //fgxURL_SOURCE_LAST;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_SOURCE_SKIP.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t skipevents = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), skipevents));
      TGo4EventSourceParameter* srcpar = step->GetSourcePar();
      TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*>(srcpar);
      if (mbspar)
        mbspar->SetEventInterval(skipevents);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: Could not set event interval to type %s ",
                srcpar->ClassName()));
    }    //fgxURL_SOURCE_SKIP;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_TYPE.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t storetype = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), storetype));

      // extract values of old store parameter and save them into the new if matching:
      TGo4EventStoreParameter* oldstorepar = step->GetStorePar();
      TString storename = oldstorepar->GetName();
      TGo4FileStoreParameter* oldfilepar = dynamic_cast<TGo4FileStoreParameter*>(oldstorepar);
      TGo4BackStoreParameter* oldbackpar = dynamic_cast<TGo4BackStoreParameter*>(oldstorepar);

      // JAM NOTE: what is missing here completely is support of user defined storages!
      // this does not exist in Qt GUI either, because nobody has ever used this feature... ;-(
      // shall we provide this  in the future?

      Int_t oldsplit = -1, oldbuf = -1, oldcomp = -1, oldauto = -1, oldover = -1;
      if (oldfilepar)
      {
        oldsplit = oldfilepar->GetSplitlevel();
        oldbuf = oldfilepar->GetBufsize();
        oldcomp = oldfilepar->GetCompression();
        oldauto = oldfilepar->GetAutosaveSize();
        oldover = oldfilepar->IsOverwriteMode();
      }
      else if (oldbackpar)
      {
        oldsplit = oldbackpar->GetSplitlevel();
        oldbuf = oldbackpar->GetBufsize();
      }
      else
      {
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: old event store parameter is type %s ",
                oldstorepar ? oldstorepar->ClassName() : "nullpointer"));
      }

      TGo4EventStoreParameter* newpar = 0;
      switch (storetype)
      {
        default:
        case GO4EV_FILE:
          {
            TGo4FileStoreParameter* filestore = new TGo4FileStoreParameter(storename);
            if (oldsplit >= 0)
              filestore->SetSplitlevel(oldsplit);    // restore my soul...
            if (oldbuf >= 0)
              filestore->SetBufsize(oldbuf);
            if (oldcomp >= 0)
              filestore->SetCompression(oldcomp);
            if (oldauto >= 0)
              filestore->SetAutosaveSize(oldauto);
            if (oldover >= 0)
              filestore->SetOverwriteMode(oldover);
            newpar = filestore;
          }
          break;
        case GO4EV_BACK:
          {
            TGo4BackStoreParameter* bckstore = new TGo4BackStoreParameter(storename);
            if (oldsplit >= 0)
              bckstore->SetSplitlevel(oldsplit);
            if (oldbuf >= 0)
              bckstore->SetBufsize(oldbuf);
            newpar = bckstore;
          }
          break;
      };
      step->SetStorePar(newpar);
      delete newpar;    // SetStorePar does clone argument, we do not need it further

    }    //fgxURL_STORE_TYPE;

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_NAME.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      TString storename = url.GetValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%s", theKey.Data(), storename.Data()));
      TGo4EventStoreParameter* storepar = step->GetStorePar();
      storepar->SetName(storename.Data());
    }    // fgxURL_STORE_NAME

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_SPLIT.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t split = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), split));
      TGo4EventStoreParameter* storepar = step->GetStorePar();
      TGo4FileStoreParameter* filepar = dynamic_cast<TGo4FileStoreParameter*>(storepar);
      TGo4BackStoreParameter* backpar = dynamic_cast<TGo4BackStoreParameter*>(storepar);
      if (filepar)
        filepar->SetSplitlevel(split);
      else if (backpar)
        backpar->SetSplitlevel(split);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: can not set split level to eventstore type %s ",
                storepar ? storepar->ClassName() : "nullpointer"));

    }    //ffgxURL_STORE_SPLIT

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_BUF.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t bufsize = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), bufsize));
      TGo4EventStoreParameter* storepar = step->GetStorePar();
      TGo4FileStoreParameter* filepar = dynamic_cast<TGo4FileStoreParameter*>(storepar);
      TGo4BackStoreParameter* backpar = dynamic_cast<TGo4BackStoreParameter*>(storepar);
      if (filepar)
        filepar->SetBufsize(bufsize);
      else if (backpar)
        backpar->SetBufsize(bufsize);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: can not set bufsize to eventstore type %s ",
                storepar ? storepar->ClassName() : "nullpointer"));
    }    //fgxURL_STORE_BUF

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_COMP.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t bufsize = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), bufsize));
      TGo4EventStoreParameter* storepar = step->GetStorePar();
      TGo4FileStoreParameter* filepar = dynamic_cast<TGo4FileStoreParameter*>(storepar);
      if (filepar)
        filepar->SetBufsize(bufsize);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: can not set compression to eventstore type %s ",
                storepar ? storepar->ClassName() : "nullpointer"));
    }    //fgxURL_STORE_COMP

    theKey.Form("%s_%d", TGo4AnalysisWebStatus::fgxURL_STORE_OVERWRITE.Data(), stepindex);
    if (url.HasOption(theKey.Data()))
    {
      Int_t overwrite = url.GetIntValueFromOptions(theKey.Data());
      message.Append(TString::Format(", %s=%d", theKey.Data(), overwrite));
      TGo4EventStoreParameter* storepar = step->GetStorePar();
      TGo4FileStoreParameter* filepar = dynamic_cast<TGo4FileStoreParameter*>(storepar);
      if (filepar)
        filepar->SetOverwriteMode(overwrite);
      else
        message.Append(
            TString::Format(" - /!\\ NEVER COME HERE: can not set overwrite mode to eventstore type %s ",
                storepar ? storepar->ClassName() : "nullpointer"));
    }    //fgxURL_STORE_OVERWRITE

    step->PrintStatus();
    stepindex++;
  }    // while step

  /////////////////////////////////////////////////////////////////////////////
  // step independent options:

  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_ENABLED;
  if (url.HasOption(theKey.Data()))
  {
    Int_t asfenabled = url.GetIntValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%d", theKey.Data(), asfenabled));
    SetAutoSaveOn(asfenabled);
  }    //fgxURL_ASF_ENABLED

  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_NAME;
  if (url.HasOption(theKey.Data()))
  {
    TString asfname = url.GetValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%s", theKey.Data(), asfname.Data()));
    SetAutoFileName(asfname.Data());
  }    //fgxURL_ASF_NAME

  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_INTERVAL;
  if (url.HasOption(theKey.Data()))
  {
    Int_t asftime = url.GetIntValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%d", theKey.Data(), asftime));
    SetAutoSaveInterval(asftime);
  }    //fgxURL_ASF_INTERVAL

  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_COMP;
  if (url.HasOption(theKey.Data()))
  {
    Int_t asfcomp = url.GetIntValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%d", theKey.Data(), asfcomp));
    SetAutoSaveCompression(asfcomp);
  }    //fgxURL_ASF_COMP

  theKey = TGo4AnalysisWebStatus::fgxURL_ASF_OVERWRITE;
  if (url.HasOption(theKey.Data()))
  {
    Int_t asfover = url.GetIntValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%d", theKey.Data(), asfover));
    SetAutoSaveOverwrite(asfover);
  }    //fgxURL_ASF_OVERWRITE

  theKey = TGo4AnalysisWebStatus::fgxURL_PREFS_NAME;
  if (url.HasOption(theKey.Data()))
  {
    TString prefsname = url.GetValueFromOptions(theKey.Data());
    message.Append(TString::Format(", %s=%s", theKey.Data(), prefsname.Data()));
    SetConfigFileName(prefsname.Data());
  }    //fgxURL_PREFS_NAME

  // apply this setup to analysis:
  std::cout << "\nTGo4AnalysisWebStatus Update From Url  :StopAnalysis..."<< std::endl;
  ana->StopAnalysis(); // need this to correctly invoke UserPreLoop later!
  std::cout << "\nTGo4AnalysisWebStatus Update From Url  :SetStatus..."<< std::endl;
  ana->SetStatus(this);
  std::cout << "\nTGo4AnalysisWebStatus Update From Url  :InitEventClasses..."<< std::endl;
  ana->InitEventClasses();

  // if we find start key restart analysis for submit and start button:
  if (url.HasOption(TGo4AnalysisWebStatus::fgxURL_STARTRUN.Data()))
  {
    std::cout << "\nTGo4AnalysisWebStatus Update From Url  :StartAnalysis..."<< std::endl;
    ana->StartAnalysis(); // this can cause deadlock ?!
    std::cout << "\nTGo4AnalysisWebStatus Update From Url  :After StartAnalysis"<< std::endl;
    message.Append("\n Analysis was started!");
  }

  TGo4Log::Message(1, message.Data());
  return kTRUE;
}

TGo4AnalysisStatus* TGo4AnalysisWebStatus::CreateStatus()
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   return an ? an->CreateStatus() : 0;
}


Bool_t TGo4AnalysisWebStatus::ApplyStatus(TGo4AnalysisStatus* status)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   printf("TGo4AnalysisWebStatus::ApplyAnalysisStatus %p\n", status);

   if ((an==0) || (status==0)) return kFALSE;

   if(cli) {
      if (cli->MainIsRunning()) an->PostLoop(); // if submit is done on running analysis,
   } else {
      if (!an->IsStopWorking()) an->PostLoop();
   }

   an->SetStatus(status);

   if (an->InitEventClasses()) {
      if(cli) {
         if (cli->MainIsRunning()) an->PreLoop(); // if submit is done on running analysis,
      } else {
         if (!an->IsStopWorking()) an->PreLoop();
      }
   }

   return kTRUE;
}


Bool_t TGo4AnalysisWebStatus::LoadStatus(const char* fname)
{
   // make it here to be able use via http interface

   TGo4Analysis* ana = TGo4Analysis::Instance();

   return ana ? ana->LoadStatus(fname) : kFALSE;

}

Bool_t TGo4AnalysisWebStatus::SaveStatus(const char* fname)
{
   // make it here to be able use via http interface

   TGo4Analysis* ana = TGo4Analysis::Instance();

   return ana ? ana->SaveStatus(fname) : kFALSE;
}


Bool_t TGo4AnalysisWebStatus::WriteAutoSave(const char* fname,
                                            Bool_t overwrite,
                                            Int_t complevel)
{
   // make it here to be able use via http interface

   TGo4Analysis* ana = TGo4Analysis::Instance();

   if (ana==0) return kFALSE;

   ana->SetAutoSaveFile(fname, overwrite, complevel);
   ana->AutoSave();

   return kTRUE;
}

