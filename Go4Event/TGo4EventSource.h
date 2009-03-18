#ifndef TGO4EVENTSOURCE_H
#define TGO4EVENTSOURCE_H

#include "TNamed.h"
#include "TString.h"

/** The abstract interface class for the raw event source.
 * Raw events are delivered from the raw event source, may be stored in a raw event store, and are processed
 * (unpacked) by a subelement factory to construct the user event (the
 * user detector data, respectively)
 * @interface
 * @author J. Adamczewski
 * @since 12/2000 */

class TGo4EventElement;

class TGo4EventSource : public TNamed {
  public:

   TGo4EventSource(const char* name);

   virtual ~TGo4EventSource();

   TGo4EventSource();

     /* We overwrite the default TNamed::Clear that would
      *  erase our name and title!
      * Implement this method in your parameter class
      * if you would like to reset any values with the
      * eraser button in the gui remote browser*/
      virtual void Clear(Option_t* opt="");

   /** Status value of server/file open. */
   Int_t GetCreateStatus() const { return fiCreateStatus; }

   /** Status of the last event. */
   Int_t GetEventStatus() const { return fiEventStatus; }

   /** Status/ error message of the last event. */
   const char* GetErrMess() const { return fxErrMess.Data(); }

    /** Access to the name of the currently active source.
     * for filelist, this is the active input file */
    virtual const char* GetActiveName();

    /** Fill the destination event dest. */
    virtual Bool_t BuildEvent(TGo4EventElement* dest) { return kTRUE; }

   /** Default timeout in seconds for event input. */
   static const Int_t fgiTIMEOUTDEFAULT;


  protected:
   /**
    * To be used for modification of the message
    * by the event source implementation.
    */
   void SetErrMess(const char* txt) { fxErrMess = txt; }

   /** Status of the last event. */
   void SetEventStatus(Int_t status) { fiEventStatus=status; }

   /** Status value of event source init (file/server open). */
   void SetCreateStatus(Int_t status) { fiCreateStatus=status; }

   /** Exception thrower. */
    void ThrowError(Int_t creastat, Int_t errstat, const char* message,...);

   /** text length */
   enum { fguTXTLEN = 256 };

  private:

   /** Contains return value of eventsource open (e.g. f_evt_get_open) call. May be checked by the creating factory after return from source ctor. */
   Int_t fiCreateStatus;

   /** Contains return value of eventsource getevent (e.g. f_evt_get_event)  call. May be checked by the controlling event loop.*/
   Int_t fiEventStatus;

   /** Most recent error message. May be delivered by f_evt_error, for example. */
   TString fxErrMess; //!

   ClassDef(TGo4EventSource,1)
};

#endif //TGO4EVENTSOURCE_H
