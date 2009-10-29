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
   TRACE((15,"TGo4TreeStructure::TGo4TreeStructure(const char*)",__LINE__, __FILE__));
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
