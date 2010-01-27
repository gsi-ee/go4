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

#ifndef TGO4PARAMETERMEMBER_H
#define TGO4PARAMETERMEMBER_H

#include "TNamed.h"

class TGo4ParameterMember : public TNamed {
   public:

      enum { kTString_t = 10001, kTGo4Fitter_t = 10002 };

      TGo4ParameterMember();
      TGo4ParameterMember(const char* name, const char* title);
      virtual ~TGo4ParameterMember();

      void SetType(const char* name, Int_t id) { fTypeName = name; fTypeId = id; }
      const char* GetTypeName() const { return fTypeName.Data(); }
      Int_t GetTypeId() const { return fTypeId; }
      Bool_t IsFitterItem() const { return GetTypeId()==kTGo4Fitter_t; }

      void SetMemberId(Int_t id) { fMemberId = id; }
      Int_t GetMemberId() const { return fMemberId; }

      void SetStrValue(const char* value) { fValue = value; }
      const char* GetStrValue() const { return fValue.Data(); }
      Bool_t CanStrEdit() const { return fTypeId!=kTGo4Fitter_t; }

      void SetArrayIndexes(Int_t ndim=0, Int_t indx1 = -1, Int_t indx2 = -1);
      Bool_t CheckArrayIndexes(Int_t ndim, Int_t indx1, Int_t indx2);
      Bool_t IsArrayItem() const { return (fIndex1>=0); }
      const char* GetFullName(TString& buf);

      void SetVisible(Bool_t on = kTRUE) { fVisible = on; }
      Bool_t IsVisible() const { return fVisible; }

      TObject* GetObject() const { return fObject; }
      void SetObject(TObject* obj, Bool_t owner);

      void SetValue(char* addr);
      void GetValue(char* addr);

      void SetToZero();
      Int_t PrintMember(Text_t* buffer = 0, Int_t buflen = 0) const;

      virtual void Clear(Option_t* opt="");

      virtual void Print(Option_t* dummy="") const;

   protected:

      TString   fTypeName;   // type name of data member
      Int_t     fTypeId;     // type id of data member
      Int_t     fMemberId;   // data member id (sequence number)
      TString   fValue;      // value of data member
      Int_t     fIndex1;     // first array index
      Int_t     fIndex2;     // second array index
      TObject*  fObject;     // place for objects like fitter
      Bool_t    fObjectOwner;//! flag indicating ownership of object
      Bool_t    fVisible;    //  is visible, used by GUI

   ClassDef(TGo4ParameterMember, 1);
};


#endif
