#include "TGo4TreeStructure.h"

#include "TList.h"

#include "TGo4Log.h"

TGo4TreeStructure::TGo4TreeStructure() :
   TGo4Status(),
   fxTopFolder(0)
{
}

TGo4TreeStructure::TGo4TreeStructure(const char* name) :
   TGo4Status(name),
   fxTopFolder(0)
{
   TRACE((15,"TGo4TreeStructure::TGo4TreeStructure(Text_t*)",__LINE__, __FILE__));
}

TGo4TreeStructure::~TGo4TreeStructure()
{
   TRACE((15,"TGo4TreeStructure::~TGo4TreeStructure()",__LINE__, __FILE__));
   delete fxTopFolder;
}

TList* TGo4TreeStructure::GetFolderList()
{
   return (fxTopFolder==0) ? 0 : dynamic_cast<TList*> (fxTopFolder->GetListOfFolders());
}

TFolder* TGo4TreeStructure::GetNamesFolder(Bool_t chown)
{
   TFolder* reval=fxTopFolder;
   if(chown) fxTopFolder=0;
   return reval;
}
