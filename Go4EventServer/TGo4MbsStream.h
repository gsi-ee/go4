#ifndef TGO4MBSSTREAM_H
#define TGO4MBSSTREAM_H

#include "TGo4MbsSource.h"

class TGo4MbsStreamParameter;

/**
 * Implements the gsi mbs stream server into the
 * go4 classes. This is an alias for the MbsSource
 * Class with parameter GETEVT__STREAM
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4MbsStream : public TGo4MbsSource {
  public:

    TGo4MbsStream(const char* name);

    TGo4MbsStream();

    TGo4MbsStream(TGo4MbsStreamParameter* par);

    ~TGo4MbsStream();

  ClassDef(TGo4MbsStream,1)
};

#endif //TGO4MBSSTREAM_H
