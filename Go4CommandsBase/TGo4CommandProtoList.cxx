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

#include "TGo4CommandProtoList.h"

#include "go4iostream.h"

#include "TMutex.h"
#include "TObjArray.h"

#include "TGo4LockGuard.h"
#include "TGo4Log.h"
#include "TGo4RemoteCommand.h"

TGo4CommandProtoList::TGo4CommandProtoList(const char* name) :
   TNamed(name,"This is a TGo4CommandProtoList")
{
   TRACE((14,"TGo4CommandProtoList::TGo4CommandProtoList(const char*) ctor",__LINE__, __FILE__));

   fxListMutex = new TMutex;
   fxCommandList = new TObjArray;
   AddCommand (new TGo4RemoteCommand);
}

TGo4CommandProtoList::~TGo4CommandProtoList()
{
   TRACE((14,"TGo4CommandProtoList::~TGo4CommandProtoList() dtor",__LINE__, __FILE__));
   {
      TGo4LockGuard listguard(fxListMutex);
      fxCommandList->Delete();
      delete fxCommandList;
   }
   delete fxListMutex;
}

void TGo4CommandProtoList::ShowCommands()
{
   TRACE((12,"TGo4CommandProtoList::ShowCommands()",__LINE__, __FILE__));

   TGo4LockGuard listguard(fxListMutex);
   TGo4Command* com;
   TIter iter(fxCommandList);
   TGo4Log::Debug(" CommandProtoList Showing the known commands:");
   cout << " Name: \t| Description:"<<endl;
   while((com= (TGo4Command*) iter())!=0)
      cout << " "<< com->GetName()<<"\t| "<<com->What()<<endl;
}

TGo4Command* TGo4CommandProtoList::MakeCommand(const char* name)
{
   TRACE((12,"TGo4CommandProtoList::MakeCommand(const char*)",__LINE__, __FILE__));
   TGo4Command* rev=0;
   TGo4LockGuard listguard(fxListMutex);
   TObject* obj=fxCommandList->FindObject(name);
   if(obj==0) {
      //no, do nothing
      TRACE((10,"TGo4CommandProtoList::RemoveCommand(TGo4Command*) command not found in array",__LINE__, __FILE__));
   } else {
      // yes, create it it
      TRACE((10,"TGo4CommandProtoList::RemoveCommand(TGo4Command*) cloning command",__LINE__, __FILE__));
      rev= (TGo4Command*) obj->Clone();
   }

   return rev;

}

TGo4Command* TGo4CommandProtoList::MakeCommand(TGo4RemoteCommand* remcon)
{
   if(remcon==0) return 0;
   TGo4Command* com=MakeCommand(remcon->GetCommandName());
   if(com==0) return 0;
   com->Set(remcon); // copy optional parameters from remote command
   return com;
}


void TGo4CommandProtoList::RemoveCommand(const char* name)
{
   TRACE((12,"TGo4CommandProtoList::RemoveCommand(const char*)",__LINE__, __FILE__));

   TGo4LockGuard listguard(fxListMutex);
   TObject* obj=fxCommandList->FindObject(name);
   if(obj==0) {
      //no, do nothing
      TRACE((10,"TGo4CommandProtoList::RemoveCommand(TGo4Command*) command not found in array",__LINE__, __FILE__));
   } else {
      // yes, remove it
      TRACE((10,"TGo4CommandProtoList::RemoveCommand(TGo4Command*) removing command from array",__LINE__, __FILE__));
      fxCommandList->Remove(obj);
      fxCommandList->Compress();
      fxCommandList->Expand(fxCommandList->GetLast()+1);
      delete obj;
   }
}


void TGo4CommandProtoList::AddCommand(TGo4Command* com)
{
   TRACE((12,"TGo4CommandProtoList::AddCommand(TGo4Command*)",__LINE__, __FILE__));
   TGo4LockGuard listguard(fxListMutex);
   if(fxCommandList->FindObject(com)==0) {
      //no, add new command
      TRACE((10,"TGo4CommandProtoList::AddCommand(TGo4Command*) Adding new go4 commandto array",__LINE__, __FILE__));
      fxCommandList->AddLast(com);
   } else {
      // yes, do nothing
      TRACE((10,"TGo4CommandProtoList::AddCommand(TGo4Command*) command was already in array",__LINE__, __FILE__));
   }
}

TGo4CommandProtoList& TGo4CommandProtoList::operator+=(const TGo4CommandProtoList& two)
{
   if(this!=&two) {
       TGo4LockGuard outerguard(two.fxListMutex);
       TIter iter(two.fxCommandList);
       TGo4Command* com;
       while ( (com=dynamic_cast<TGo4Command*>(iter())) !=0)
              AddCommand(com);
       cout <<"CommandProtoList "<< GetName() <<"used operator += for adding list " << two.GetName() << endl;
   }
   return *this;
}
