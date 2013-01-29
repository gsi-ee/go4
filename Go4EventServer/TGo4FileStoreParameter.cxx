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

#include "TGo4FileStoreParameter.h"

#include "TGo4Status.h"
#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4FileStoreParameter::TGo4FileStoreParameter(const char* name,
                                               Int_t splitlevel,
                                               Int_t bufsize,
                                               Int_t compression,
                                               Int_t autosave)
: TGo4EventStoreParameter(name, GO4EV_FILE),
   fiSplit(splitlevel), fiBufsize(bufsize),
   fiCompression(compression), fiAutosavesize(autosave), fbOverwrite(1)
{
   TRACE((14,"TGo4FileStoreParameter::TGo4FileStoreParameter(const char*,...)", __LINE__, __FILE__));
   SetTitle(name);
}

TGo4FileStoreParameter::TGo4FileStoreParameter()
: TGo4EventStoreParameter("Default Go4 FileStore", GO4EV_FILE),
   fiSplit(99), fiBufsize(64000),
   fiCompression(5), fiAutosavesize(5000000), fbOverwrite(1)
{
   TRACE((14,"TGo4FileStoreParameter::TGo4FileStoreParameter()", __LINE__, __FILE__));
   SetTitle("Go4FileStore-Tree");
}

TGo4FileStoreParameter::~TGo4FileStoreParameter()
{
   TRACE((14,"TGo4FileStoreParameter::~TGo4FileStoreParameter()", __LINE__, __FILE__));
}
Int_t TGo4FileStoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
 TRACE((12,"TGo4FileStoreParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Int_t restlen=locallen;
   Text_t* current=localbuf;
   Int_t delta=TGo4EventStoreParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Split level: \t%d \n",fiSplit);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Buffer size: \t%d bytes\n",fiBufsize);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Compression level: \t%d \n",fiCompression);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Tree autosave at: \t%d bytes\n",fiAutosavesize);
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


Bool_t TGo4FileStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4FileStoreParameter::UpdateFrom()",__LINE__, __FILE__));

   TGo4FileStoreParameter* filepar=dynamic_cast<TGo4FileStoreParameter*>(rhs);
   if (filepar==0) return kFALSE;

   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetSplitlevel(filepar->GetSplitlevel());
   SetBufsize(filepar->GetBufsize());
   SetCompression(filepar->GetCompression());
   SetOverwriteMode(filepar->IsOverwriteMode());
   SetAutosaveSize(filepar->GetAutosaveSize());
   return kTRUE;
}
