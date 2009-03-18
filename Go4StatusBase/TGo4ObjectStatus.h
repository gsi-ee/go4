#ifndef TGO4OBJECTSTATUS_H
#define TGO4OBJECTSTATUS_H

#include "TGo4Status.h"
#include "TString.h"

/**
 * One entry of the object names folder. Contains the go4 object structural information,
 * such as name, title, classname, date, size. May be subclassed later to include
 * further information on special objects, e.g. histogram dimensions and contents.
 * @since 7/2001
 * @author J. Adamczewski
 */

class TGo4ObjectStatus : public TGo4Status {
   public:

      /** default ctor for streamer. */
      TGo4ObjectStatus();

      /** Create status information from given TObject.
        * Flag withtime indicates if the status creation time
        * shall be noted inside the status object. */
      TGo4ObjectStatus(TObject* object, Bool_t withtime=kTRUE);

      virtual ~TGo4ObjectStatus();

      /** ClassName of the described object. */
      const char* GetObjectClass() const { return fxObjectClass.Data(); }

      /** Size of described object in byte. */
      Int_t GetObjectSize() const { return fiObjectSize; }

      /** Object's reset protection state. */
      Bool_t IsResetProtect() const { return fbResetProtect; }

      /** Object's delete protection state. */
      Bool_t IsDeleteProtect() const { return fbDeleteProtect; }

      /** Creation time string; sql format. */
      const char* GetTimeString() const { return fxStatusTime.Data(); }

      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

   protected:
      /** Size of the described object in byte. */
      UInt_t fiObjectSize;

      /** If true, described object is protected against reset (Clear()) from gui. */
      Bool_t fbResetProtect;

      /** If true, described object is protected against deletion from gui. */
      Bool_t fbDeleteProtect;

      /** Classname of the described object. */
      TString fxObjectClass;

      /** creation time of status object */
      TString fxStatusTime;

   ClassDef(TGo4ObjectStatus,1)
};

#endif //TGO4OBJECTSTATUS_H
