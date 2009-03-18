#ifndef TGO4DISPLAY_H
#define TGO4DISPLAY_H

#include "TGo4Master.h"

class TGo4Status;
class TGo4DisplayLoggerTimer;
class TGo4DisplayDrawerTimer;
class TGo4AnalysisProxy;

class TGo4Display : public TGo4Master {
  public:

    TGo4Display(Bool_t isserver=kTRUE);
    virtual ~TGo4Display();

    void DisplayLog(TGo4Status * Status);
    void DisplayData(TObject*);

    void SetAnalysis(TGo4AnalysisProxy* an) { fxAnalysis = an; }

 /** Add functionality to reset gui appereance here:*/
    virtual Bool_t DisconnectSlave(const char* name=0,
                                   Bool_t waitforslave=kTRUE );


  private:

    /** Timer for drawing job */
    TGo4DisplayDrawerTimer* fxDrawTimer;         //!

    /** Timer for status logging job */
    TGo4DisplayLoggerTimer* fxLogTimer;          //!

    TGo4AnalysisProxy*      fxAnalysis;          //!
};

#endif //TGO4DISPLAY_H
