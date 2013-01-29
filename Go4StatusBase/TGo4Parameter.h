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

#ifndef TGO4PARAMETER_H
#define TGO4PARAMETER_H

#include "TNamed.h"
#include "Riostream.h"

class TObjArray;
class TIterator;
class TDataMember;

/** Base class for all parameter aggregations,
  * e.g. calibration data.
  * @author J. Adamczewski / S. Linev
  * @since Jun-2002 */

class TGo4Parameter : public TNamed {
   public:

      TGo4Parameter();

      TGo4Parameter(const char* name, const char* title = "This is a Go4 Parameter Object");

      virtual ~TGo4Parameter();

      /** Basic method to printout information on stdout;
        * to be overridden by specific subclass  */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      virtual void Print(Option_t* dummy="") const;

      /** Update contents of parameter class with external object.
        * to be implemented in subclass */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

      /** Specifies, if user implement UpdateFrom() method and it should be used
        * when parameter updated from other.
        * Otherwise, default method with help of ParameterStatus will be used */
      virtual Bool_t CustomUpdateFrom() const { return kTRUE; }

      /* We overwrite the default TNamed::Clear that would
      *  erase our name and title!
      * Implement this method in your parameter class
      * if you would like to reset any values with the
      * eraser button in the gui remote browser*/
      virtual void Clear(Option_t* opt="");

      void GetMemberValues(TObjArray* fItems);
      Bool_t SetMemberValues(TObjArray* fItems);

      /** Standard way to store parameter in form of macro,
       * If \param opt == "savemacro", parameter saved in form of macro,
       * which can be rerun in analysis-  see saveparam.C macro for example */
      virtual void SavePrimitive(std::ostream& fs, Option_t* opt= "");

   protected:

      void GetMemberValues(TObjArray* fItems, TClass* cl, char* ptr, unsigned long int cloffset);

      Bool_t SetMemberValues(TObjArray* items, Int_t& itemsindx, TClass* cl, char* ptr, unsigned long int cloffset);

      Int_t FindArrayLength(TObjArray* items, Int_t& itemsindx, TDataMember* member);

   ClassDef(TGo4Parameter,1)
};

#endif //TGO4PARAMETER_H
