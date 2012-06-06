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

#include "TGo4TreeStoreParameter.h"

#include "go4iostream.h"

#include "Go4EventServerTypes.h"
#include "TGo4Status.h"
#include "TGo4Log.h"

TGo4TreeStoreParameter::TGo4TreeStoreParameter(const char* name,
                                                Int_t splitlevel,
                                                Int_t bufsize,
                                                const char* filename,
                                                Int_t compression)
: TGo4EventStoreParameter(name, GO4EV_TREE),
   fiSplit(splitlevel), fiBufsize(bufsize), fiCompression(compression)
{
   TRACE((14,"TGo4TreeStoreParameter::TGo4TreeStoreParameter(const char*,...)", __LINE__, __FILE__));
   if(filename) fxBranchFile = filename;
          else  fxBranchFile="";
}

TGo4TreeStoreParameter::TGo4TreeStoreParameter()
: TGo4EventStoreParameter("default go4 treestore", GO4EV_TREE),
   fiSplit(1), fiBufsize(64000), fiCompression(5), fxBranchFile("")
{
  TRACE((14,"TGo4TreeStoreParameter::TGo4TreeStoreParameter()", __LINE__, __FILE__));


}


TGo4TreeStoreParameter::~TGo4TreeStoreParameter()
{
  TRACE((14,"TGo4TreeStoreParameter::~TGo4TreeStoreParameter()", __LINE__, __FILE__));
}

Int_t TGo4TreeStoreParameter::PrintParameter(Text_t* buffer, Int_t buflen)
{
 TRACE((12,"TGo4TreeStoreParameter::PrintParameter()",__LINE__, __FILE__));
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Text_t* current=localbuf;
   Int_t restlen=locallen;
   Int_t delta=TGo4EventStoreParameter::PrintParameter(current,restlen);
   restlen-=delta;
   current+=delta;
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Split level: \t%d \n",fiSplit);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Compression level: \t%d \n",fiCompression);
   current=TGo4Status::PrintIndent(current,restlen);
   current=TGo4Status::PrintBuffer(current,restlen, "  Buffer size: \t%d \n",fiBufsize);
   if(!fxBranchFile.IsNull()) {
       current=TGo4Status::PrintIndent(current,restlen);
       current=TGo4Status::PrintBuffer(current,restlen, "  Branch file name: \t%s \n",fxBranchFile.Data());
   }
   if(buffer==0)
      {
          cout << localbuf << endl;
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

Bool_t TGo4TreeStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   TRACE((12,"TGo4TreeStoreParameter::UpdateFrom()",__LINE__, __FILE__));
   if((rhs!=0) && rhs->InheritsFrom(TGo4TreeStoreParameter::Class()))
      {
         TGo4TreeStoreParameter* treepar=dynamic_cast<TGo4TreeStoreParameter*>(rhs);
         if(!treepar) return kFALSE;
         if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
         fiSplit=treepar->fiSplit;
         fiBufsize=treepar->fiBufsize;
         fiCompression=treepar->fiCompression;
         return kTRUE;
      }
   else
      {
          return kFALSE;
      }
   return kFALSE;
}
