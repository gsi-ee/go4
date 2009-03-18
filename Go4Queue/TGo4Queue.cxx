#include "TGo4Queue.h"

#include "Riostream.h"

#include "TList.h"
#include "TMutex.h"
#include "TCondition.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4RuntimeException.h"

TGo4Queue::TGo4Queue() :  TNamed("Default Queue", "This is a Go4 Queue"),
  fiEntries(0), fiMaxEntries(100), fbWakeUpCall(kFALSE)
{
   fxMutex=new TMutex;
   fxCondition=new TCondition;
   fxList= new TList;

}

TGo4Queue::TGo4Queue(const char* name) : TNamed(name, "This is a Go4 Queue"),
      fiEntries(0), fiMaxEntries(100), fbWakeUpCall(kFALSE)
{
   fxMutex=new TMutex;
   fxCondition=new TCondition;
   fxList= new TList;
}

TGo4Queue::~TGo4Queue()
{
   delete fxList;
   delete fxCondition;
   delete fxMutex;
}

void TGo4Queue::Clear(Option_t* opt)
{
   TGo4LockGuard qguard(fxMutex);
   fxList->Clear();
}

TObject* TGo4Queue::Wait()
{
   if(IsEmpty())
      {
         //cout <<"WWWWWWWWWWWWWWWWWW --- TGo4Queue "<< GetName() <<" is in Condition Wait..." << endl;
         fxCondition->Wait();
         if(fbWakeUpCall)
            {
               fbWakeUpCall=kFALSE;
               return 0; // signal by Wake(), give null back!
            }
      }
   return Next();
}

TObject* TGo4Queue::Next()
{
   TGo4LockGuard qguard(fxMutex);
   return (fxList->Remove(fxList->LastLink()) );
}

void TGo4Queue::Add(TObject* ob)
{
   {
    TGo4LockGuard qguard(fxMutex);
       if(fxList->GetSize() <= fiMaxEntries)
         {
            fxList->AddFirst(ob);
            //cout <<"QA Queue " <<GetName() <<" added obj, size="<<fxList->GetSize()<<endl;
         }
       else
          {
            //cout <<" Queue Add Error: queue "<< GetName() <<" is full !!!"<<endl;
            throw TGo4RuntimeException();
          }
   } //TGo4LockGuard qguard(fxMutex);
   fxCondition->Signal();

}

Bool_t TGo4Queue::IsEmpty()
{
   TGo4LockGuard qguard(fxMutex);
   return (fxList->IsEmpty());
}

void TGo4Queue::Wake()
{
   //cout <<"TGo4Queue::Wake() in "<<GetName() << endl;
   fbWakeUpCall=kTRUE;
   fxCondition->Signal();
}
