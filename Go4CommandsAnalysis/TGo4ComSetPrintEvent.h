#ifndef TGO4COMSETPRINTEVENT_H
#define TGO4COMSETPRINTEVENT_H

#include "TGo4AnalysisObjectCommand.h"
#include "TGo4MbsSource.h"

/** Sets explicit printout for the next n events to come
* Event is searched by object name in the EventObjects folder.
* Currently, this feature is only supported for mbs events.
* may be extended to all kinds of TGo4EventElements...
 * @author J. Adamczewski
 * @since 27-May-2004
 */
class TGo4ComSetPrintEvent : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComSetPrintEvent();

    TGo4ComSetPrintEvent(const char* obname);

    virtual ~TGo4ComSetPrintEvent();

    Int_t ExeCom();
      /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);

    void SetEventNum(Int_t i) {fxPrintPar.fiNum=i;}
    void SetSubId(Int_t i) {fxPrintPar.fiSid=i;}
    void SetLong(Bool_t on) {fxPrintPar.fiLong=on;}
    void SetHex(Bool_t on) {fxPrintPar.fiHex=on; }
    void SetData(Bool_t on) {fxPrintPar.fiData=on;}

  private:

    TGo4MbsSourcePrintPar fxPrintPar;

  ClassDef(TGo4ComSetPrintEvent,1)
};

#endif //TGO4COMSETPRINTEVENT_H
