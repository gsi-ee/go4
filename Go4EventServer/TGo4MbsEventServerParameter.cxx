#include "TGo4MbsEventServerParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsEventServerParameter::TGo4MbsEventServerParameter() :
  TGo4MbsSourceParameter("default mbs eventserver", GO4EV_MBS_EVENTSERVER)
{
   TRACE((14,"TGo4MbsEventServerParameter::TGo4MbsEventServerParameter()", __LINE__, __FILE__));
}

TGo4MbsEventServerParameter::TGo4MbsEventServerParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_EVENTSERVER)
{
  TRACE((14,"TGo4MbsEventServerParameter::TGo4MbsEventServerParameter(const char*)", __LINE__, __FILE__));
}

TGo4MbsEventServerParameter::~TGo4MbsEventServerParameter()
{
   TRACE((14,"TGo4MbsEventServerParameter::~TGo4MbsEventServerParameter()", __LINE__, __FILE__));
}
