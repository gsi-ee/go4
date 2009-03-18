#include "TGo4DynamicListException.h"

#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4DynamicEntry.h"

TGo4DynamicListException & TGo4DynamicListException::operator= (const TGo4DynamicListException & right)
{
 TRACE((14,"TGo4DynamicListException:operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TGo4RuntimeException::operator=(right); // copy base class members
      // put additional member copies here...
      fxEntry=right.fxEntry;
      fxEntryName=right.fxEntryName;
      fxEntryClass=right.fxEntryClass;
      fxStatusMessage=right.fxStatusMessage;
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }
}

TGo4DynamicListException::TGo4DynamicListException(const TGo4DynamicListException &right)
  :TGo4RuntimeException(right)
{
 TRACE((14,"TGo4DynamicListException:TGo4DynamicListException(right)", __LINE__, __FILE__));   ;
 fxEntry=right.fxEntry;
 fxEntryName=right.fxEntryName;
 fxEntryClass=right.fxEntryClass;
 fxStatusMessage=right.fxStatusMessage;

}

Int_t TGo4DynamicListException::Handle()
{
   TRACE((12,"TGo4AnalyisStepException::Handle()", __LINE__, __FILE__));


   TGo4Log::Debug("%s \n Entry %s:%s, %s\n \t--Entry was disabled.",
            What(),GetEntryClass(), GetEntryName(),GetStatusMessage());
   if(fxEntry)
      fxEntry->EnableProcessing(kFALSE);
   return 0;
}
TGo4DynamicListException::~TGo4DynamicListException()
{
 TRACE((14,"TGo4DynamicListException:~TGo4DynamicListException", __LINE__, __FILE__));

}
TGo4DynamicListException::TGo4DynamicListException (TGo4DynamicEntry* entry, const char* message,...)
: fxEntry(0)
   {
   TRACE((14,"TGo4DynamicListException:TGo4DynamicListException (TGo4DynamicEntry*,..)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Dynamic List Exception --!!!";
   if(entry)
      {
         fxEntry=entry;
         fxEntryName=fxEntry->GetName();
         fxEntryClass=fxEntry->ClassName();

      }
   UInt_t lbuflen=256;
   Text_t txtbuf[256];
   va_list args;
   va_start(args, message);
   vsnprintf(txtbuf, lbuflen, message, args);
   va_end(args);
   fxStatusMessage=txtbuf;
}
