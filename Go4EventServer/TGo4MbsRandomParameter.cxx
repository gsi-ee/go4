#include "TGo4MbsRandomParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsRandomParameter::TGo4MbsRandomParameter() :
   TGo4MbsSourceParameter("Default Go4 MbsRandom", GO4EV_MBS_RANDOM)
{
   TRACE((14,"TGo4MbsRandomParameter::TGo4MbsRandomParameter()", __LINE__, __FILE__));
}

TGo4MbsRandomParameter::TGo4MbsRandomParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_RANDOM)
{
   TRACE((14,"TGo4MbsRandomParameter::TGo4MbsRandomParameter(onst char*,...)", __LINE__, __FILE__));
}

TGo4MbsRandomParameter::~TGo4MbsRandomParameter()
{
  TRACE((14,"TGo4MbsRandomParameter::~TGo4MbsRandomParameter()", __LINE__, __FILE__));
}
