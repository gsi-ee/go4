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

#ifndef TGO4FITDATAPROFILE_H
#define TGO4FITDATAPROFILE_H

#include "TGo4FitData.h"
#include "TGo4FitSlot.h"

class TProfile;

/**
 * Data object, which provides access to TProfile ROOT class.
 *
 * The TProfile object can be assigned to TGo4FitDataProfile object in constructor, in SetProfile() method or in SetObject() method of fitter. TProfile object may owned, or may not owned by data object.
 */
class TGo4FitDataProfile : public TGo4FitData {
   public:

      /**
       * Default constructor.
       */
      TGo4FitDataProfile();

      /**
       * Create TGo4FitDataProfile object with given name.
       * Pointer on TProfile object and ownership flag can be specified.
       * Scaling factor (amplitude) also can be added.
       */
      TGo4FitDataProfile(const char* iName, TProfile* = 0, Bool_t iGraphOwned = kFALSE, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitDataProfile object.
       */
      virtual ~TGo4FitDataProfile();

      /**
       * Returns pointer on TProfile object.
       */
      TProfile* GetProfile() { return (TProfile*) fxProfile.GetObject(); }

      /**
       * Set pointer on TProfile object.
       * Ownership flag also can be specified. By default TProfile object not owned.
       */
      void SetProfile(TProfile *iProfile, Bool_t iProfileOwned = kFALSE);

      /**
       * Create TGo4FitDataProfileIter iterator, assosiated with given data object..
       */
      TGo4FitDataIter* MakeIter();

      /**
       * Add pointer on slot, which should contain TProfile object, to list of slots.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Slot for TProfile object.
       * @label fxProfile
       */
      TGo4FitSlot fxProfile;

   ClassDef(TGo4FitDataProfile,1)
};

// **************************************************************************

class TGo4FitDataProfileIter : public TGo4FitDataIter {
   public:
      TGo4FitDataProfileIter();
      TGo4FitDataProfileIter(TGo4FitDataProfile* Data);
      ~TGo4FitDataProfileIter();

   protected:
      virtual TGo4FitData* GetData() const { return fxData; }
      virtual Bool_t StartReset();
      virtual Bool_t ReadCurrentPoint();
      virtual Bool_t ShiftToNextPoint();

   private:
      TGo4FitDataProfile* fxData;
      Int_t fiNumPoints;

   ClassDef(TGo4FitDataProfileIter,1)
};


#endif // TGO4FITDATAPROFILE_H
