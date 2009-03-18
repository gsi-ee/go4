#include "TGo4FileSourceParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4FileSourceParameter::TGo4FileSourceParameter(const char* name)
: TGo4EventSourceParameter(name, GO4EV_FILE)
{
  TRACE((14,"TGo4FileSourceParameter::TGo4FileSourceParameter(const char*,...)", __LINE__, __FILE__));
}

TGo4FileSourceParameter::TGo4FileSourceParameter()
: TGo4EventSourceParameter("Default Go4 FileSource", GO4EV_FILE)
{
  TRACE((14,"TGo4FileSourceParameter::TGo4FileSourceParameter()", __LINE__, __FILE__));
}


TGo4FileSourceParameter::~TGo4FileSourceParameter()
{
   TRACE((14,"TGo4FileSourceParameter::~TGo4FileSourceParameter()", __LINE__, __FILE__));
}
