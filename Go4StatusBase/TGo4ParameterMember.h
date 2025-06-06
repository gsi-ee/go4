// $Id$
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

#ifndef TGO4PARAMETERMEMBER_H
#define TGO4PARAMETERMEMBER_H

#include "TNamed.h"

class TGo4ParameterMember : public TNamed {
   public:

      enum { kTString_t = 10001, kTGo4Fitter_t = 10002, kTArray_t = 10003 };

      TGo4ParameterMember();
      TGo4ParameterMember(const char *name, const char *title);
      virtual ~TGo4ParameterMember();

      void SetType(const char *name, Int_t id) { fTypeName = name; fTypeId = id; }
      const char *GetTypeName() const { return fTypeName.Data(); }
      Int_t GetTypeId() const { return fTypeId; }
      Bool_t IsFitterItem() const { return GetTypeId() == kTGo4Fitter_t; }

      void SetMemberId(Int_t id) { fMemberId = id; }
      Int_t GetMemberId() const { return fMemberId; }

      void SetStrValue(const char *value) { fValue = value; }
      void SetIntValue(Int_t value) { fValue.Form("%d", value); }
      const char *GetStrValue() const { return fValue.Data(); }
      Int_t GetIntValue() const { return fValue.Atoi();  };
      Bool_t CanStrEdit() const { return (fTypeId != kTGo4Fitter_t) && (fTypeId != kTArray_t); }

      void SetArrayIndexes(Int_t ndim = 0, Int_t indx1 = -1, Int_t indx2 = -1);
      Bool_t CheckArrayIndexes(Int_t ndim, Int_t indx1, Int_t indx2);
      Bool_t IsArrayItem() const { return fIndex1 >= 0; }
      const char *GetFullName(TString &buf) const;

      void SetVisible(Bool_t on = kTRUE) { fVisible = on; }
      Bool_t IsVisible() const { return fVisible; }

      TObject *GetObject() const { return fObject; }
      void SetObject(TObject *obj, Bool_t owner);

      void SetValue(char *addr);
      void GetValue(char *addr);

      void SetToZero();

      void Clear(Option_t *opt = "") override;

      void Print(Option_t *opt = "") const override;

   protected:

      TString   fTypeName;            ///< type name of data member
      Int_t     fTypeId{0};           ///< type id of data member
      Int_t     fMemberId{0};         ///< data member id (sequence number)
      TString   fValue;               ///< value of data member
      Int_t     fIndex1{-1};          ///< first array index
      Int_t     fIndex2{-1};          ///< second array index
      TObject  *fObject{nullptr};     ///< place for objects like fitter
      Bool_t    fObjectOwner{kTRUE};  ///<! flag indicating ownership of object
      Bool_t    fVisible{kTRUE};      ///<  is visible, used by GUI

   ClassDefOverride(TGo4ParameterMember, 1);
};

#endif
