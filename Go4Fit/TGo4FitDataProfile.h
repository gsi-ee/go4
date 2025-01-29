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
      TGo4FitDataProfile(const char *iName, TProfile *iProfile = nullptr, Bool_t iGraphOwned = kFALSE, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitDataProfile object.
       */
      virtual ~TGo4FitDataProfile();

      /**
       * Returns pointer on TProfile object.
       */
      TProfile *GetProfile() const { return (TProfile *) fxProfile.GetObject(); }

      /**
       * Set pointer on TProfile object.
       * Ownership flag also can be specified. By default TProfile object not owned.
       */
      void SetProfile(TProfile *iProfile, Bool_t iProfileOwned = kFALSE);

      /**
       * Create TGo4FitDataProfileIter iterator, associated with given data object..
       */
      std::unique_ptr<TGo4FitDataIter> MakeIter() override;

      /**
       * Add pointer on slot, which should contain TProfile object, to list of slots.
       */
      void FillSlotList(TSeqCollection *list)  override;

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;

   protected:

      /**
       * Slot for TProfile object.
       */
      TGo4FitSlot fxProfile;

   ClassDefOverride(TGo4FitDataProfile,1)
};

// **************************************************************************

class TGo4FitDataProfileIter : public TGo4FitDataIter {
   public:
      TGo4FitDataProfileIter();
      TGo4FitDataProfileIter(TGo4FitDataProfile *Data);
      ~TGo4FitDataProfileIter();

   protected:
      TGo4FitData *GetData() const override { return fxData; }
      Bool_t StartReset() override;
      Bool_t ReadCurrentPoint() override;
      Bool_t ShiftToNextPoint() override;

   private:
      TGo4FitDataProfile *fxData{nullptr};
      Int_t fiNumPoints{0};

   ClassDefOverride(TGo4FitDataProfileIter,1)
};


#endif // TGO4FITDATAPROFILE_H
