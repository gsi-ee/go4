#include "TGo4DynamicEntry.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Status.h"

TGo4DynamicEntry::TGo4DynamicEntry() :
   TNamed(),
   fbProcessEntry(kTRUE)
{
   TRACE((15,"TGo4DynamicEntry::TGo4DynamicEntry()",__LINE__, __FILE__));
}

TGo4DynamicEntry::TGo4DynamicEntry(const char* name, const char* title) :
   TNamed(name, title),
   fbProcessEntry(kTRUE)
{
   TRACE((15,"TGo4DynamicEntry::TGo4DynamicEntry(char*)",__LINE__, __FILE__));
   SetBit(TGo4Status::kGo4CanDelete);
}

TGo4DynamicEntry::~TGo4DynamicEntry()
{
   TRACE((15,"TGo4DynamicEntry::~TGo4DynamicEntry()",__LINE__, __FILE__));
}
