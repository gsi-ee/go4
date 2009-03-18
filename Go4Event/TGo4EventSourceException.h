#ifndef TGO4EVENTSOURCEEXCEPTION_H
#define TGO4EVENTSOURCEEXCEPTION_H

#include "TGo4RuntimeException.h"
#include "TString.h"

class TGo4EventSource;

class TGo4EventSourceException : public TGo4RuntimeException {

  public:

      TGo4EventSourceException (TGo4EventSource* eventsource);

      virtual ~TGo4EventSourceException();

      virtual Int_t Handle();

      TGo4EventSourceException(const TGo4EventSourceException &right);

      TGo4EventSourceException & operator = (const TGo4EventSourceException & right);

      /** Status value of server/file open. */
      Int_t GetCreateStatus() const { return fiCreateStatus; }

      /** Status of the last event. */
      Int_t GetEventStatus() const { return fiEventStatus; }

      /** Status/ error message of the last event. */
      const char* GetErrMess() const { return fxErrMess.Data(); }

      /** Name of the throwing event source. */
      const char* GetSourceName() const { return fxSourceName.Data(); }

      /** Name of the throwing event source. */
      const char* GetSourceClass() const { return fxSourceClass.Data(); }

  protected:
      /** @supplierCardinality 1 */
      TGo4EventSource * fxEventSource;     //!

      /** Contains return value of eventsource open (e.g. f_evt_get_open) call. May be checked by the creating factory after return from source ctor. */
      Int_t fiCreateStatus;

      /** Contains return value of eventsource getevent (e.g. f_evt_get_event)  call. May be checked by the controlling event loop.*/
      Int_t fiEventStatus;

      /** Error message at exception throw time*/
      TString fxErrMess; //!

      /** Name of the throwing event source*/
      TString fxSourceName; //!

      /** Class of the throwing event source*/
      TString fxSourceClass; //!

  private:
      TGo4EventSourceException();
};

#endif // #define TGO4TASKHANDLEREXCEPTION_H



