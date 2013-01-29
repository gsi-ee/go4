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

#include "TGo4Command.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4CommandReceiver.h"

const char* TGo4Command::fgcMODEDESCRIPTION[]=
   {"NOMODE","Observer","Controller","Administrator"};


TGo4Command::TGo4Command(const char* name, const char* description, Int_t id) :
   TNamed(name, "This is a TGo4Command")
{
   TRACE((12,"TGo4Command::TGo4Command(const char* name, const char* description, Int_t id)", __LINE__, __FILE__));
   SetDescription(description);
   SetReceiverName("NoNameReceiver");
   SetTaskName("NoNameTask");
   fiCommandID = id;
   fiVersion = 1;
   fbIsSynchron = kFALSE;
   fbIsEnabled = kTRUE;
   fbIsExclusive = kFALSE;
   fbIsLocal = kFALSE;
   fiProtection=kGo4ComModeObserver;
   fiMode=kGo4ComModeRefused;
}

TGo4Command::TGo4Command() : TNamed()
{
  TRACE((12,"TGo4Command::TGo4Command()", __LINE__, __FILE__));
  SetDescription("Default command");
  SetReceiverName("NoNameReceiver");
  SetTaskName("NoNameTask");
  fiCommandID = -1;
  fiVersion = 1;
  fbIsSynchron = kFALSE;
  fbIsEnabled = kTRUE;
  fbIsExclusive = kFALSE;
  fbIsLocal = kFALSE;
  fiProtection=kGo4ComModeObserver;
  fiMode=kGo4ComModeRefused;
}

TGo4Command::~TGo4Command()
{
  TRACE((12,"TGo4Command::~TGo4Command()", __LINE__, __FILE__));
}

Int_t TGo4Command::GetCommandID() const
{
    TRACE((10,"Int_t TGo4Command::GetCommandID() const ", __LINE__, __FILE__));
    return fiCommandID;
}

void TGo4Command::Help()
{
    TRACE((10,"void TGo4Command::Help()", __LINE__, __FILE__));
}

Bool_t TGo4Command::IsEnabled() const
{
    TRACE((10,"Bool_t TGo4Command::IsEnabled() const", __LINE__, __FILE__));
    return fbIsEnabled;
}

Bool_t TGo4Command::IsExclusive() const
{
    TRACE((10,"Bool_t TGo4Command::IsExclusive() const", __LINE__, __FILE__));
    return fbIsExclusive;
}

const char* TGo4Command::GetReceiverName() const
{
  TRACE((10,"const char * TGo4Command::GetReceiverName()", __LINE__, __FILE__));
  return fxReceiverName.Data();
}

void TGo4Command::SetReceiverName(const char* name)
{
  TRACE((10,"void TGo4Command::SetReceiverName(const char*)", __LINE__, __FILE__));
  fxReceiverName = name;
}

const char* TGo4Command::GetTaskName() const
{
  TRACE((10,"const char * TGo4Command::GetTaskName()", __LINE__, __FILE__));
  return fxTaskName.Data();
}

void TGo4Command::SetTaskName(const char* name)
{
  TRACE((10,"void TGo4Command::SetTaskName(const char*)", __LINE__, __FILE__));
  fxTaskName=name;
}

void TGo4Command::SetDescription(const char* txt)
{
   TRACE((10,"void TGo4Command::SetDescription(const char*)", __LINE__, __FILE__));
   fxDescription = txt;
}
const char* TGo4Command::What() const
{
  TRACE((10,"const char * TGo4Command::What()", __LINE__, __FILE__));
  return fxDescription.Data();
}

Int_t TGo4Command::ExeCom()
{
  TRACE((10,"Int_t TGo4Command::ExeCom()", __LINE__, __FILE__));
  std::cout << What() << std::endl;
  return 0;
}

Int_t TGo4Command::UnExeCom()
{
  TRACE((10,"Int_t TGo4Command::UnExeCom()", __LINE__, __FILE__));
  return 0;
}

Int_t TGo4Command::RefuseCom()
{
   TGo4Log::Warn("Not allowed to execute %s in mode %d:%s",
                       GetName(), GetMode(), GetModeDescription(GetMode()));
   return 0;
}

Bool_t TGo4Command::Log(Int_t mode)
{
   TRACE((10,"Bool_t TGo4Command::Log(Int_t mode)", __LINE__, __FILE__));
   switch (mode) {
      case 0:
         std::cout << What() << std::endl;
         break;
      default:
         break;
   }
  return kTRUE;
}

Bool_t TGo4Command::IsSynchron() const
{
    TRACE((10,"Bool_t TGo4Command::IsSynchron() const", __LINE__, __FILE__));
    return fbIsSynchron;
}

Bool_t TGo4Command::IsLocal() const
{
    return fbIsLocal;
}


void TGo4Command::SetReceiver(TGo4CommandReceiver * rec)
{
    TRACE((12,"void TGo4Command::SetReceiver(TGo4CommandReceiver * rec)", __LINE__, __FILE__));
    fxReceiverBase = rec;
}


const char* TGo4Command::GetModeDescription(Go4CommandMode_t mode)
{
   if(mode<0 || mode>3) return 0;
   return fgcMODEDESCRIPTION[mode];
}


void TGo4Command::Set(TGo4RemoteCommand*)
{
// please override this function!

}
