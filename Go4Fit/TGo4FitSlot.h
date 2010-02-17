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

#ifndef TGO4FITSLOT_H
#define TGO4FITSLOT_H

#include "TGo4FitNamed.h"

class TSeqCollection;
class TObjArray;

/**
 * Object for managing pointers on specific objects.
 * General purpose of this object - store pointer on any kind of object (derived from TObject) with/without ownership and provide a general interface to set/get and store/restore this object. Slot always belong to another object (has owner). Slot always knows, what kind of object should be assigned to it (via Class property).
 * Slots may be distributed in different places of fitter: in data object (typically for assigning data like TH1, TGraph or other), sometimes in model object (like in TGo4FitModelFromData in slot TGo4FitData object should be placed to provide bins for model component) or complex hierarchy of these objects possible. Fitter has a method to collect a list of all slots from all possible places. Once all slots are collected, fitter know, how many and what kind of objects should be (or can be) assigned to fitter before start fitting. Fitter (see TGo4FitterAbstract description) has a set of function for manipulating with objects in slots.
 */
class TGo4FitSlot : public TGo4FitNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitSlot();

      /**
       * Creates TGo4FitSlot object.
       * This constructor should be called in default constructor of object, where slot situated.
       * Constructor only sets owner of slot, desired class of object and if this object should be always owned.
       * The rest values (name, title, object and so on) should be restored by streamer, which typically calls default constructor.
       */
      TGo4FitSlot(TNamed* iOwner, TClass* iClass);

      /**
       * Creates TGo4FitSlot object.
       * This constructor should be placed in normal constructor of object, where slot is situated.
       * Constructor sets name and title of slot, owner of slot, desired class of object and if this object always should be owned.
       * Optionally, requirements of object (is it always needed or not, default - not), pointer on object and ownership flag can be specified.
       */
      TGo4FitSlot(const char* iName, const char* iTitle,
                  TNamed* iOwner, TClass* iClass,
                  Bool_t iNeeded = kFALSE, TObject* iObject = 0, Bool_t iOwned = kFALSE);

      /**
       * Destroys TFo4FitSlot object.
       */
      virtual ~TGo4FitSlot();

      /**
       * Return pointer on class for desired object.
       */
      TClass* GetClass() { return fxClass; }

      /**
       * Set basic fields of slot.
       * See proper constructor for description. Normally should not be called by user.
       */
      void SetDefaults(TNamed* iOwner, TClass* iClass);

      /**
       * Set save flag for saving of object in slot, when slot stored to streamer:
       * 0 - object will be saved if it owned,
       * 1 - object always will be saved
       * 2 - object never will be saved.
       */
      void SetSaveFlag(Int_t iSaveFlag) { fiSaveFlag = iSaveFlag; }

      /**
       * Returns save flag.
       */
      Int_t GetSaveFlag() { return fiSaveFlag; }

      /**
       * Checks, if object class corresponds to class, specified for slot
       */
      Bool_t IsSuitable(TObject* obj);


      /**
       * Checks, if class corresponds to specified for slot
       */
      Bool_t IsSuitableClass(TClass* cl);

      /**
       * Sets object to slot.
       * First, if before another object was set to slot with ownership flag, it will be destroyd.
       * Then, if object should always be owned by slot, but object is assigning without ownersip, it will be cloned and clone will be assigned with ownership flag.
       * if CheckClass==kTRUE (default), slot will refuse unsuitable object class.
       * If object owned by slot, it always be destroyed by slot.
       */
      Bool_t SetObject(TObject* iObject, Bool_t iOwned = kFALSE, Bool_t CheckClass = kTRUE);

      /**
       * Connect this slot to provided slot.
       * This means, that objects, containing in slot will be also return by GetObject() of current slot.
       * Class of slots should be compatible
       * Returns kTRUE, if operation successfull
       */
      Bool_t ConnectToSlot(TGo4FitSlot* slot);

      /**
       * Checks, if slot can be connected to other
       */
      Bool_t CanConnectToSlot(TGo4FitSlot* slot);

      /**
       * Returns pointer to connected slot or 0, if not connected
       */
      TGo4FitSlot* GetConnectedSlot() const { return dynamic_cast<TGo4FitSlot*>(fxObject); }

      /**
       * Returns kTRUE, if slot connected to another slot
       */
      Bool_t IsConnectedToSlot() const { return GetConnectedSlot()!=0; }

      /**
       * Clear connection to other slot
       */
      void ClearConnectionToSlot();

      /**
       * Return pointer on assigned object.
       */
      TObject* GetObject() const;

      /**
       * Return name of assigned object.
       */
      const char* GetObjectName() const;

      /**
       * Clone object in slot and returns pointer on new object.
       * Name of new object can be optionally specified.
       */
      TObject* CloneObject(const char* newname = 0);

      /**
       * Clear pointer on object in slot.
       * If object owned, it will be destroyed.
       */
      virtual void Clear(Option_t* = "") { SetObject(0, kFALSE); }

      /**
       * Return ownership flag.
       */
      Bool_t GetOwned() { return fbOwned; }

      /**
       * Explicitly sets ownership flag.
       * Normally should not be called by user.
       */
      void SetOwned(Bool_t iOwned = kTRUE) { fbOwned = iOwned; }

      /**
       * Return requirement status for slot.
       * If kTRUE, object should always be sets before any fitting become possible.
       */
      Bool_t GetNeeded() { return fbNeeded; }

      /**
       * Set requirement flag for object.
       */
      void SetNeeded(Bool_t iNeeded) { fbNeeded = iNeeded; }

      /**
       * Return kTRUE, if no object in slot.
       */
      Bool_t IsEmpty() const { return GetObject()==0; }

      /**
       * Return kTRUE, if object assigned to slot.
       */
      Bool_t IsObject() const { return GetObject()!=0; }

      /**
       * Returns kTRUE, if object required, but not yet set.
       */
      Bool_t IsRequired() const { return fbNeeded && IsEmpty(); }

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;


   protected:

      friend class TGo4FitSlotList;

      void SetSaveSettings(Int_t save = -1, Int_t own = -1);

      Bool_t HasSaveSettings();

      Bool_t WillBeSaved();

      /**
       * Sets up ownership relation between object and owner of slot.
       * If object owned by slot and object derived from TGo4FitNamed class, the owner of slot becomes the owned of object.
       * This gives an ability to combine FullName of TGo4FitNamed object according ownership relation.
       */
      void CheckOwnership();

      /**
       * Class of object, which should be assigned to slot.
       */
      TClass* fxClass;        //!

      /**
       * Describe saving of object in slot.
       */
      Int_t fiSaveFlag;       //!   0 - slot save if owned(default), 1 - saved always, 2 - saved never


      /**
       * Ownership flag of object.
       */
      Bool_t fbOwned;         //!

      /**
       * Flag, is this object should always be set.
       */
      Bool_t fbNeeded;         //!

      /**
       * Pointer on assigned object.
       */
      TObject* fxObject;       //!

      Int_t fiSaveSlot;        //!
      Int_t fiSaveOwnership;   //!

   ClassDef(TGo4FitSlot,1)
};


//************************************************************************************************

class TGo4FitSlotList {
   public:
     TGo4FitSlotList();
     virtual ~TGo4FitSlotList();

     /**
      * Fill list of TGo4FitSlot objects to provided collection.
      * This function should be implemented in every derived object, which would provide slots for outside.
      */
     virtual void FillSlotList(TSeqCollection* lst);

     /**
      * Update internal list of slots (if exists).
      */
     void SetUpdateSlotList();

     /**
      * Return pointer in TObjArray with list of all slots in list.
      * This list should be permanently updated in inherited class by SetUpdateSlotList()
      * method to keep list of slots actual.
      */
     const TObjArray* GetSlotList(Bool_t ForceUpdate = kFALSE);

     /**
      * Returns number of slots in list.
      */
     Int_t NumSlots();

     /**
      * Returns slots from list with specified index
      */
     TGo4FitSlot* GetSlot(Int_t nslot);

     /**
      * Find slot in list with given full name;
      */
     TGo4FitSlot* FindSlot(const char* FullSlotName);

     /**
      * Connects first slot to second.
      * Return kFALSE, if failed
      */
     Bool_t ConnectSlots(TGo4FitSlot* slot1, TGo4FitSlot* slot2);

     /**
      * Connects first slot to second.
      * Return kFALSE, if failed
      */
     Bool_t ConnectSlots(const char* Slot1FullName, const char* Slot2FullName);

      /**
       * Set object to first suitable slot with defined ownership flag.
       * Slot must be empty
       * Return pointer on slot, to which object was assigned
       */
     TGo4FitSlot* SetObject(TObject* obj, Bool_t iOwned = kFALSE);

      /**
       * Set object to first suitable slot, which have PlaceName name or owner of which
       * has PlaceName full name with defined ownership flag.
       * If PlaceName==0, SetObject(TObject*, Bool_t) method will be called.
       * If only slot suits to defined condition, object in slot will be overwritten.
       * Return pointer on slot, to which object was assigned
       */
     TGo4FitSlot* SetObject(const char* PlaceName, TObject* obj, Bool_t iOwned = kFALSE);

      /**
       * Checks, if object assign to one of slots.
       * Return pointer on slot, to which object is assigned, otherwise 0.
       */
     TGo4FitSlot* IsObjectInSlots(TObject* obj);

      /**
       * Check, if all necessary objects are provided.
       */
     Bool_t CheckObjects(Bool_t MakeOut = kTRUE);

      /**
       * Return kTRUE, if there are empty slots in list.
       */
     Bool_t IsEmptySlots();

      /**
       * Clear object from slot(s), which are defined by PlaceName condition.
       * If PlaceName==0, all slots will be clead.
       * If NonOwned = kTRUE, only not owned objects will be cleared (pointer will be set to 0)
       * otherwise all pointers will be cleared and owned objects will be deleted
       */
     void ClearObjects(const char* PlaceName = 0, Bool_t NonOwned = kTRUE);

      /**
       * Clear object in specified slot.
       * Checks, if same object is assign to another slot.
       * Used by ClearObjects() method.
       */
     void ClearSlot(TGo4FitSlot* slot, Bool_t NonOwned);

      /**
       * Set save flag for object(s), assigned to fitter.
       * If save flag set to 0, object(s) wil be saved, if it owned by slot.
       * If save flag set to 1, object(s) will be saved in any case.
       * If save flag set to 2, object(s) will never be saved.
       * PlaceName define exact slot name or owner name
       * if (PlaceName==0),  flag will be set for all slots in list.
       */
     void SetSaveFlagForObjects(Int_t iSaveFlag, const char* PlaceName = 0);

   protected:

      /**
       * Resolve situation, when same object assigned to several slots with ownership flag.
       */
     void CheckDuplicatesOnSlot();

     void PrepareSlotsForWriting();

     TObjArray* fxSlotList;     //!
     Bool_t fbUpdateSlotList;   //!


   ClassDef(TGo4FitSlotList,1);

};

#endif // TGO4FITSLOT_H


