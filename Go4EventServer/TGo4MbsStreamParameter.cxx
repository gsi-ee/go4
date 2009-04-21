#include "TGo4MbsStreamParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsStreamParameter::TGo4MbsStreamParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_STREAM)
{
  TRACE((14,"TGo4MbsStreamParameter::TGo4MbsStreamParameter(Text_t*,...)", __LINE__, __FILE__));
}

TGo4MbsStreamParameter::TGo4MbsStreamParameter() :
   TGo4MbsSourceParameter("default mbs stream", GO4EV_MBS_STREAM)
{
  TRACE((14,"TGo4MbsStreamParameter::TGo4MbsStreamParameter()", __LINE__, __FILE__));
}

TGo4MbsStreamParameter::~TGo4MbsStreamParameter()
{
  TRACE((14,"TGo4MbsStreamParameter::~TGo4MbsStreamParameter()", __LINE__, __FILE__));
}
