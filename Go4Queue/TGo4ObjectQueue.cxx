// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ObjectQueue.h"
#include "TBuffer.h"

#include "TGo4Log.h"

TGo4ObjectQueue::TGo4ObjectQueue() :
   TGo4Queue("Default object queue")
{
   GO4TRACE((14,"TGo4ObjectQueue::TGo4ObjectQueue()", __LINE__, __FILE__));
}

TGo4ObjectQueue::TGo4ObjectQueue(const char* name) :
   TGo4Queue(name)
{
   GO4TRACE((14,"TGo4ObjectQueue::TGo4ObjectQueue(const char*)", __LINE__, __FILE__));
}

TGo4ObjectQueue::~TGo4ObjectQueue()
{
   GO4TRACE((14,"TGo4ObjectQueue::~TTGo4ObjectQueue()", __LINE__, __FILE__));
}

TObject* TGo4ObjectQueue::WaitObject()
{
   GO4TRACE((12,"TGo4ObjectQueue::WaitObject()", __LINE__, __FILE__));
   return (Wait());
}

void TGo4ObjectQueue::AddObject(TObject * object, Bool_t clone)
{
   GO4TRACE((12,"TGo4ObjectQueue::AddObject(TObject*, Bool_t)", __LINE__, __FILE__));
   TObject* entry=0;
   if(clone && (object!=0))
      {
         entry=object->Clone();
      }
   else
      {
         entry=object;
      }
   Add(entry);

}

void TGo4ObjectQueue::AddObjectFromBuffer(TBuffer* buffer)
{
   GO4TRACE((12,"TGo4ObjectQueue::AddObjectFromBuffer(TBuffer*)", __LINE__, __FILE__));
   TObject* entry=0;
   if(buffer!=0)
      {
         buffer->SetReadMode();
         buffer->Reset();
         buffer->InitMap();
         TClass* cl= buffer->ReadClass();
//         std::cout <<"object queue: read class "  << cl << std::endl;
//         if(cl)
//            std::cout <<"object queue: class name "  << cl->GetName() << std::endl;
         buffer->Reset();
         entry = buffer->ReadObject(cl);

//         std::cout <<"object queue: read object "  << entry << std::endl;
//         if(entry)
//            std::cout <<"object queue: classname "  << entry->ClassName() << std::endl;
      }
   else
      {
//              std::cout <<"object queue: zero buffer"  << std::endl;
      }
   Add(entry);
}
