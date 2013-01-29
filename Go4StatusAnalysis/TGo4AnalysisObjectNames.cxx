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

#include "TGo4AnalysisObjectNames.h"

#include "Riostream.h"
#include "TROOT.h"
#include "TFolder.h"
#include "TGo4Log.h"

TGo4AnalysisObjectNames::TGo4AnalysisObjectNames() :
   TGo4Status(),
   fxTopFolder(0)
{
}

TGo4AnalysisObjectNames::TGo4AnalysisObjectNames(const char* name) :
   TGo4Status(name),
   fxTopFolder(0)
{
  TRACE((15,"TGo4AnalysisObjectnames::TGo4AnalysisObjectnames(const char*)",__LINE__, __FILE__));
}

TGo4AnalysisObjectNames::~TGo4AnalysisObjectNames()
{
TRACE((15,"TGo4AnalysisObjectnames::~TGo4AnalysisObjectnames()",__LINE__, __FILE__));
   if(fxTopFolder!=0) {
     delete fxTopFolder;
     fxTopFolder=0;
   }
}

TList* TGo4AnalysisObjectNames::GetFolderList()
{
  if(fxTopFolder!=0)
     return dynamic_cast<TList*> (fxTopFolder->GetListOfFolders());
  else
     return 0;
}

TFolder* TGo4AnalysisObjectNames::GetNamesFolder(Bool_t chown)
{
   TFolder* reval=fxTopFolder;
   if(chown) fxTopFolder=0;
   return reval;
}


Int_t TGo4AnalysisObjectNames::PrintStatus(Text_t* buffer, Int_t buflen)
{
   TRACE((12,"TGo4AnalysisObjectNames::PrintStatus()",__LINE__, __FILE__));
   //
   if(buflen<=0 && buffer!=0)
      return 0;
   gROOT->SetDirLevel(0);
   Int_t locallen=64000;
   Text_t localbuf[64000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   current=PrintBuffer(localbuf,restlen,"G-OOOO-> Analysis Object Names Printout <-OOOO-G\n");
   current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   Int_t delta=PrintFolder(fxTopFolder, current, restlen);
   restlen-=delta;
   current+= delta;
   if(buffer==0)
      {
          std::cout << localbuf << std::endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
            size=buflen-1;
         strncpy(buffer,localbuf,size);
      }
   return size;
}

Int_t TGo4AnalysisObjectNames::PrintFolder(TFolder* fold, Text_t * buf, Int_t buflen)
{
   TRACE((12,"TGo4AnalysisObjectNames::PrintFolder()",__LINE__, __FILE__));
   //
   if(buf!=0 && buflen<=0)
      {
         cout <<"PrintFolder returns nop" << endl;
         return 0;
      }
   gROOT->IncreaseDirLevel();
   Int_t locallen=64000;
   Text_t localbuf[64000];
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   if(fold!=0) {
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen,"G-OOOO-> Status Folder %s Printout <-OOOO-G\n", fold->GetName());
         current=PrintIndent(current,restlen);
         current=PrintBuffer(current,restlen,"G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
         //objectlist=dynamic_cast<TList*> (fold->GetListOfFolders());
         //cout << "----------- processing Print Folder "<< endl;

         TIter iter(fold->GetListOfFolders());
         TObject* entry=0;

         while((entry=iter()) !=0) {
               if(entry->InheritsFrom(TFolder::Class()))
                  {
                      // found subfolder, process in recursion
                      TFolder* subobj= dynamic_cast<TFolder*> (entry);
                      Int_t delta=PrintFolder(subobj, current, restlen);
                      restlen-=delta;
                      current+= delta;
                  }
               else if (entry->InheritsFrom(TGo4Status::Class()))
                  {
                     TGo4Status* subobj= dynamic_cast<TGo4Status*> (entry);
                     Int_t delta=subobj->PrintStatus(current,restlen);
                     restlen-=delta;
                     current+=delta;
                  }
               else
                {
                     entry->ls();
                  // unknown object
                }
            } // while
      current=PrintIndent(current,restlen);
      current=PrintBuffer(current,restlen, "G-OOOO-> ---------------------------------------------- <-OOOO-G\n");
   }

   if(buf==0)
      {
          cout << localbuf << endl;
      }
   else
      {
         size=locallen-restlen;
         if(size>buflen-1)
               size=buflen-1;
         strncpy(buf,localbuf,size);

      }
   gROOT->DecreaseDirLevel();
   return size;
}
