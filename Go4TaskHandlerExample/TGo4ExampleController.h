#ifndef TGO4EXAMPLECONTROLLER_H
#define TGO4EXAMPLECONTROLLER_H

#include "Rtypes.h"

class TObject;
class TCanvas;

class TGo4Status;
class TGo4Command;

/**
 * example class for a user control interface (might be a kind of GUI later on);
 * methods of this class are used to request commands from the user and to
 * display results which are sent by the client
 */

class TGo4ExampleController {
  public:

    TGo4ExampleController();

    TGo4Command* GetCommand();

    void DisplayLog(TGo4Status*);

    void DisplayData(TObject*);

    virtual ~TGo4ExampleController();

  private:
    TObject* fxPreviousData;

    TCanvas* fxCanvas;
};

#endif //TGO4EXAMPLECONTROLLER_H
