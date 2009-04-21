#include "TGo4MbsTransportParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsTransportParameter::TGo4MbsTransportParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_TRANSPORT)
{
   TRACE((14,"TGo4MbsTransportParameter::TGo4MbsTransportParameter(Text_t*,...)", __LINE__, __FILE__));
}

TGo4MbsTransportParameter::TGo4MbsTransportParameter() :
   TGo4MbsSourceParameter("default mbs transport", GO4EV_MBS_TRANSPORT)
{
   TRACE((14,"TGo4MbsTransportParameter::TGo4MbsTransportParameter()", __LINE__, __FILE__));
}

TGo4MbsTransportParameter::~TGo4MbsTransportParameter()
{
   TRACE((14,"TGo4MbsTransportParameter::~TGo4MbsTransportParameter()", __LINE__, __FILE__));
}
