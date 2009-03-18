#include "Riostream.h"
#include <stdlib.h>

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
   cout << "deleted the first histo after cloning" << endl;
   objectq.AddObject(com);
   TObject* output=0;
   while(! (objectq.IsEmpty()) )
       {
           output= objectq.WaitObject();
           cout << "object from object queue:"<< endl;
           cout << output->ClassName() << endl;
           cout << output->GetName() << endl;
           if (output->InheritsFrom(TH1::Class()))
               {
                    histo3= dynamic_cast<TH1*>(output);
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
           cout << "got buffer from buffer queue:"<< endl;
           mess->SetReadMode();
           mess->Reset();
           mess->InitMap();
           TClass* cl = mess->ReadClass();
           cout << "Class: " << cl << endl;
           if(cl)
              cout << "Classname: " << cl->GetName() << endl;
           cout << "Reading object from buffer..."<< endl;
           mess->Reset();
           output=mess->ReadObject(cl);
           if(output==0)
              {
                  cout << "zero object" <<endl;
                 exit(0);
              }

           cout << output->ClassName() << endl;
           cout << output->GetName() << endl;
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
           cout << "object from object queue second pass:"<< endl;
           cout << output->ClassName() << endl;
           cout << output->GetName() << endl;
           if (output->InheritsFrom(TH1::Class()))
              output->Draw();
       }


    while(!bufferq.IsEmpty())
       {
           mess= bufferq.WaitBuffer();

           //TMessage* mess = dynamic_cast<TMessage*> (buf);
           //TClass* cl= buf->ReadClass();
           cout << "got buffer from buffer queue second time:"<< endl;
           mess->SetReadMode();
           mess->Reset();
           mess->InitMap();
           TClass* cl = mess->ReadClass();
           cout << "Class: " << cl << endl;
           if(cl)
              cout << "Classname: " << cl->GetName() << endl;
           cout << "Reading object from buffer..."<< endl;
           mess->Reset();
           output=mess->ReadObject(cl);
           if(output==0)
              {
                  cout << "zero object" <<endl;
                 exit(0);
              }

           cout << output->ClassName() << endl;
           cout << output->GetName() << endl;
           if (output->InheritsFrom(TH1::Class()))
              output->Draw();
      }


   theApp.Run();
   return 0;
}

void usage(char **args)
{
  cout << "Usage: " << args[0] << " \n";
  exit(1);
}
