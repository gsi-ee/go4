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

#include "TGo4ComGetEnvelope.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4ObjEnvelope.h"

TGo4ComGetEnvelope::TGo4ComGetEnvelope(const char* obname, const char* foldername) :
   TGo4AnalysisObjectCommand("ANGetEnvelope","Get an arbitrary object from analysis",obname)
{
   GO4TRACE((12,"TGo4ComGetEnvelope::TGo4ComGetEnvelope(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetFolderName(foldername);
}

TGo4ComGetEnvelope::TGo4ComGetEnvelope() :
   TGo4AnalysisObjectCommand()
{
   GO4TRACE((12,"TGo4ComGetEnvelope::TGo4ComGetEnvelope() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetName("ANGetEnvelope");
   SetDescription("Get an arbitrary object from analysis");
}

TGo4ComGetEnvelope::~TGo4ComGetEnvelope()
{
   GO4TRACE((12,"TGo4ComGetEnvelope::~TGo4ComGetEnvelope() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComGetEnvelope::ExeCom()
{
   GO4TRACE((12,"TGo4ComGetEnvelope::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli==0) {
      GO4TRACE((11,"TGo4ComGetEnvelope::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   GO4TRACE((11,"TGo4ComGetEnvelope::ExeCom() - found valid receiver",__LINE__, __FILE__));

   TObject* obj = TGo4Analysis::Instance()->ObjectManager()->GetAsTObject( GetObjectName(), GetFolderName());

   TGo4ObjEnvelope* envelope = new TGo4ObjEnvelope(obj, GetObjectName(), GetFolderName());
   cli->SendObject(envelope, GetTaskName());
   delete envelope;

   return -1;
}
