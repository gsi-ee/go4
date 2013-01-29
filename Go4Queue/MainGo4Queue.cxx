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

#include <stdlib.h>

#include "Riostream.h"
#include "TApplication.h"
#include "TH1.h"
#include "TClass.h"

#include "TGo4BufferQueue.h"
#include "TGo4ObjectQueue.h"


void usage(char **args);

//______________________________________________________________________________
int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);

   TNamed *com = new TNamed("named object", "this is a test");

   TH1F *histo = new TH1F("histogram", "this is a test histogram", 100, 0., 1.);
   histo->Fill(0.2);
   TH1F *histo2 = new TH1F("histogram 2", "this is a test histogram", 100, 0., 1.);
   histo2->Fill(0.7);
   TH1* histo3=0;
   TGo4ObjectQueue objectq("My object queue");
   TGo4BufferQueue bufferq("My buffer queue");

   objectq.AddObject(histo,kTRUE);
   delete histo;
   std::cout << "deleted the first histo after cloning" << std::endl;
   objectq.AddObject(com);
   TObject* output = 0;
   while(! (objectq.IsEmpty()) )
   {
      output = objectq.WaitObject();
      std::cout << "object from object queue:"<< std::endl;
      std::cout << output->ClassName() << std::endl;
      std::cout << output->GetName() << std::endl;
      if (output->InheritsFrom(TH1::Class())) {
         histo3 = dynamic_cast<TH1*>(output);
         dynamic_cast<TH1*>(output)->Draw();
      }
   }


    bufferq.AddBufferFromObject(histo2);
    bufferq.AddBufferFromObject(com);
    bufferq.AddBufferFromObject(histo3);


   TBuffer* mess=0;
   while(!bufferq.IsEmpty())
       {
           mess= bufferq.WaitBuffer();

           //TMessage* mess = dynamic_cast<TMessage*> (buf);
           //TClass* cl= buf->ReadClass();
           std::cout << "got buffer from buffer queue:"<< std::endl;
           mess->SetReadMode();
           mess->Reset();
           mess->InitMap();
           TClass* cl = mess->ReadClass();
           std::cout << "Class: " << cl << std::endl;
           if(cl)
              std::cout << "Classname: " << cl->GetName() << std::endl;
           std::cout << "Reading object from buffer..."<< std::endl;
           mess->Reset();
           output=mess->ReadObject(cl);
           if(output==0)
              {
                  std::cout << "zero object" <<std::endl;
                 exit(0);
              }

           std::cout << output->ClassName() << std::endl;
           std::cout << output->GetName() << std::endl;
           if (output->InheritsFrom(TH1::Class()))
              output->Draw();
         objectq.AddObjectFromBuffer(mess);
         objectq.AddObjectFromBuffer(mess);
         objectq.AddObject(com);
         objectq.AddObjectFromBuffer(mess);
      }

       bufferq.AddBuffer(mess);
       bufferq.AddBuffer(mess);
       bufferq.AddBuffer(mess);

    while(! (objectq.IsEmpty()) )
       {
           output= objectq.WaitObject();
           std::cout << "object from object queue second pass:"<< std::endl;
           std::cout << output->ClassName() << std::endl;
           std::cout << output->GetName() << std::endl;
           if (output->InheritsFrom(TH1::Class()))
              output->Draw();
       }


    while(!bufferq.IsEmpty())
       {
           mess= bufferq.WaitBuffer();

           //TMessage* mess = dynamic_cast<TMessage*> (buf);
           //TClass* cl= buf->ReadClass();
           std::cout << "got buffer from buffer queue second time:"<< std::endl;
           mess->SetReadMode();
           mess->Reset();
           mess->InitMap();
           TClass* cl = mess->ReadClass();
           std::cout << "Class: " << cl << std::endl;
           if(cl)
              std::cout << "Classname: " << cl->GetName() << std::endl;
           std::cout << "Reading object from buffer..."<< std::endl;
           mess->Reset();
           output=mess->ReadObject(cl);
           if(output==0)
              {
                  std::cout << "zero object" <<std::endl;
                 exit(0);
              }

           std::cout << output->ClassName() << std::endl;
           std::cout << output->GetName() << std::endl;
           if (output->InheritsFrom(TH1::Class()))
              output->Draw();
      }


   theApp.Run();
   return 0;
}

void usage(char **args)
{
  std::cout << "Usage: " << args[0] << " \n";
  exit(1);
}
