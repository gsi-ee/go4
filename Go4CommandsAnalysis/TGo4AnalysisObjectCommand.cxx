#include "TGo4AnalysisObjectCommand.h"

#include "TGo4Log.h"
#include "TGo4RemoteCommand.h"
#include "TGo4AnalysisObjectManager.h"

TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() :
   TGo4AnalysisCommand()
{
   TRACE((12,"TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() ctor",__LINE__, __FILE__));
   SetFolderName(TGo4AnalysisObjectManager::fgcTOPFOLDER);
}

TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand(const char* comname,
                                                     const char* description,
                                                     const char* obname) :
   TGo4AnalysisCommand(comname,description)
{
   TRACE((12,"TGo4AnalysisObjectCommand::TGo4AnalysisObjectCommand() ctor",__LINE__, __FILE__));
   SetObjectName(obname);
   SetFolderName(TGo4AnalysisObjectManager::fgcTOPFOLDER);
}

TGo4AnalysisObjectCommand::~TGo4AnalysisObjectCommand()
{
   TRACE((12,"TGo4AnalysisObjectCommand::~TGo4AnalysisObjectCommand() dtor",__LINE__, __FILE__));
}

void TGo4AnalysisObjectCommand::Set(TGo4RemoteCommand* remcom)
{
   if(remcom==0) return;
   SetObjectName(remcom->GetString(0));
   if(strcmp(remcom->GetString(1),TGo4RemoteCommand::Get_fgxREMCOMEMPTYSTRING()))
      SetFolderName(remcom->GetString(1)); // only reset folder if specified!
}
