#ifndef TGO4MBSEVENTSERVER_H
#define TGO4MBSEVENTSERVER_H

#include "TGo4MbsSource.h"

class TGo4MbsEventServerParameter;

/**
 * Implements the gsi mbs event server into the
 * go4 classes. This is an alias for the MbsSource
 * Class with parameter GETEVT__EVENT
 * @author J. Adamczewski
 * @since 1/2001
 */

class TGo4MbsEventServer : public TGo4MbsSource {
  public:

    TGo4MbsEventServer(const char* name);

    TGo4MbsEventServer();

    TGo4MbsEventServer(TGo4MbsEventServerParameter* par);

    ~TGo4MbsEventServer();

  ClassDef(TGo4MbsEventServer,1)
};

#endif //TGO4MBSEVENTSERVER_H
