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

#include "TGo4ComSaveAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus(const char* filename)
:TGo4AnalysisCommand("ANSave","Save settings to file")
{
   GO4TRACE((12,"TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetFileName(filename);
   SetProtection(kGo4ComModeController);
}


TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus()
:TGo4AnalysisCommand("ANSave","Save settings to file")
{
   GO4TRACE((12,"TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetFileName(TGo4Analysis::fgcDEFAULTSTATUSFILENAME);
   SetProtection(kGo4ComModeController);
}


TGo4ComSaveAnalysisStatus::~TGo4ComSaveAnalysisStatus()
{
   GO4TRACE((12,"TGo4ComSaveAnalysisStatus::~TGo4ComSaveAnalysisStatus() dtor",__LINE__, __FILE__));
}

void TGo4ComSaveAnalysisStatus::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
SetFileName(remcom->GetString(0));
}



Int_t TGo4ComSaveAnalysisStatus::ExeCom()
{
   GO4TRACE((12,"TGo4ComSaveAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      GO4TRACE((11,"TGo4ComSaveAnalysisStatus::ExeCom() - found valid receiver",__LINE__, __FILE__));
//         TGo4Log::Debug(" Executing ComSaveAnalysisStatus...  ");
         TGo4Analysis* ana=TGo4Analysis::Instance();
         if(ana)
            {
               Bool_t ok=ana->SaveStatus( GetFileName() );
               if(ok)
                  {
                   cli->SendStatusMessage(1, kFALSE,"Saved analysis status to file %s.",
                                                GetFileName());
                  }
               else
                  {
                   cli->SendStatusMessage(3, kFALSE,"ERROR on Saving analysis status to file %s.",
                                                GetFileName());
                  } // if(ok)
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",
                                                 GetName());
            } // if(ana)
      }
   else
      {
      GO4TRACE((11,"TGo4ComSaveAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComSaveAnalysisStatus ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
