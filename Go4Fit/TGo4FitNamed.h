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

#ifndef TGO4FITNAMED_H
#define TGO4FITNAMED_H

#include "TNamed.h"

/**
 * Extension of ROOT TNamed class.
 * In addition to TNamed class property has owner and provides additional GetFullName() method.
 * Full name consist of full name (if exists) of owner and name of object itself, divided by dot. For instance: "OwnerName.ObjectName". If owner has its owner, the full name will look like: "Owner1Name.Owner2Name.ObjectName". Any level of ownership is supported. Thus, full name shows hierarchy of ownership for given object.
 */
class TGo4FitNamed : public TNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitNamed();

      /**
       * Creates TGo4FitNamed object and set name and title.
       */
      TGo4FitNamed(const char* Name, const char* Title, TNamed* Owner = 0);

      /**
       * Destroy TGo4FitNamed object.
       */
      virtual ~TGo4FitNamed();

      /**
       * Returns full name of object.
       * Consist of name or full name (if exist) of owner, divided by dot.
       */
      const char* GetFullName();

      /**
       * Return full name of owner.
       * If not exist, return just name of owner.
       */
      const char* GetOwnerFullName();

      /**
       * Sets owner of object.
       * Owner should be a object of TNamed or derived classes. If owner derived from TGo4FitNamed class, full name of owner used for constracting of full name of object.
       */
      void SetOwner(TNamed* iOwner) { fxOwner = iOwner; }

      /**
       * Return owner of object.
       */
      TNamed* GetOwner() { return fxOwner; }

      void Print(Option_t* option) const;

    private:

      /**
       * Owner of object
       */
      TNamed* fxOwner;         //!

      /**
       * String, containing full name of object.
       */
      TString fxFullName;      //!

    ClassDef(TGo4FitNamed,1)
};
#endif // TGO4FITNAMED_H
