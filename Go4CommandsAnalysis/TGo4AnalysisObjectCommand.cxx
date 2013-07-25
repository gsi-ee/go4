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

#include "TGo4AnalysisObjectCommand.h"

#include "TGo4Log.h"
#include "TGo4RemoteCommand.h"
#include "TGo4AnalysisObjectManager.h"

TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() :
   TGo4AnalysisCommand()
{
   GO4TRACE((12,"TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() ctor",__LINE__, __FILE__));
   SetFolderName(TGo4AnalysisObjectManager::fgcTOPFOLDER);
}

TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand(const char* comname,
                                                     const char* description,
                                                     const char* obname) :
   TGo4AnalysisCommand(comname,description)
{
   GO4TRACE((12,"TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() ctor",__LINE__, __FILE__));
   SetObjectName(obname);
   SetFolderName(TGo4AnalysisObjectManager::fgcTOPFOLDER);
}

TGo4AnalysisObjectCommand::~TGo4AnalysisObjectCommand()
{
   GO4TRACE((12,"TGo4AnalysisObjectCommand::~TGo4AnalysisObjectCommand() dtor",__LINE__, __FILE__));
}

void TGo4AnalysisObjectCommand::Set(TGo4RemoteCommand* remcom)
{
   if(remcom==0) return;
   SetObjectName(remcom->GetString(0));
   if(strcmp(remcom->GetString(1),TGo4RemoteCommand::Get_fgxREMCOMEMPTYSTRING()))
      SetFolderName(remcom->GetString(1)); // only reset folder if specified!
}
