#include "TGo4EventFactory.h"

#include "TGo4Log.h"

TGo4EventFactory::TGo4EventFactory(const char* name)
: TNamed(name, "This is a Go4 Event Factory")
{
   TRACE((15,"TGo4EventFactory::TGo4EventFactory(const char*)",__LINE__, __FILE__));
}

TGo4EventFactory::~TGo4EventFactory()
{
   TRACE((15,"TGo4EventFactory::~TGo4EventFactory()",__LINE__, __FILE__));
}

TGo4EventFactory::TGo4EventFactory()
{
   TRACE((15,"TGo4EventFactory::~TGo4EventFactory()",__LINE__, __FILE__));
}
