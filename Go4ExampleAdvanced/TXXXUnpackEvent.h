// $Id: TXXXUnpackEvent.h 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TXXXUNPACKEVENT_H
#define TXXXUNPACKEVENT_H

#define XXX_NUM_CHAN 8
#define XXX_NUM_MODULES 16
#define XXX_NUM_CRATES 4

// nr of modules in Crate    0  1  2  3
//                           |  |  |  |
#define NR_MODULES          {0, 16, 16, 2}



#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"

/* example for composite event with 2 sublevels:
 * one unpack event consists of up to XXX_NUM_CRATES crates.
 * Each crate has up to XXX_NUM_MODULES modules*/

class TXXXModule : public TGo4EventElement {
   public:
      TXXXModule():TGo4EventElement(), fiData(0) {}
      TXXXModule(const char *name, Short_t id):
         TGo4EventElement(name,name,id), fiData(0) {}
      virtual ~TXXXModule() {}

      void Clear(Option_t *t="") override
      {
         fiData = 0;
         fiTest = 0;
         fiAux = 0;
      }

      Int_t GetData() const { return fiData; }
      void SetData(Int_t dat){ fiData=dat; }

      Int_t GetTest() const {return fiTest;}
      void SetTest(Int_t dat){fiTest=dat;}

      Int_t GetAux() const {return fiAux;}
      void SetAux(Int_t dat){fiAux=dat;}

      /* in this example, each module represents single channel of data.*/
      Int_t fiData{0};

      /* module may have test data channel.*/
      Int_t fiTest{0};

      /* module may have aux data channel.*/
      Int_t fiAux{0};

   ClassDefOverride(TXXXModule,1)

};


class TXXXCrate : public TGo4CompositeEvent {
   public:
      TXXXCrate():TGo4CompositeEvent(){;}
      TXXXCrate(const char *name, Short_t id);
      virtual ~TXXXCrate() {}

   ClassDefOverride(TXXXCrate,1)
};

class TXXXUnpackEvent : public TGo4CompositeEvent {
   public:
      TXXXUnpackEvent():TGo4CompositeEvent(){}
      TXXXUnpackEvent(const char *name);
      virtual ~TXXXUnpackEvent() {}

   ClassDefOverride(TXXXUnpackEvent,1)
};

#endif // TXXXUNPACKEVENT_H

