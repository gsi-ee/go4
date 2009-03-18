#include "TGo4TreeSourceParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4TreeSourceParameter::TGo4TreeSourceParameter(const char* name)
: TGo4EventSourceParameter(name, GO4EV_TREE)
{
  TRACE((14,"TGo4TreeSourceParameter::TGo4TreeSourceParameter(Text_t*,...)", __LINE__, __FILE__));
}


TGo4TreeSourceParameter::TGo4TreeSourceParameter()
: TGo4EventSourceParameter("Default Go4 TreeSource", GO4EV_TREE)
{
  TRACE((14,"TGo4TreeSourceParameter::TGo4TreeSourceParameter()", __LINE__, __FILE__));
}

TGo4TreeSourceParameter::~TGo4TreeSourceParameter()
{
  TRACE((14,"TGo4TreeSourceParameter::~TGo4TreeSourceParameter()", __LINE__, __FILE__));
}
