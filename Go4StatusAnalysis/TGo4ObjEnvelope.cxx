#include "TGo4ObjEnvelope.h"

#include "Riostream.h"

TGo4ObjEnvelope::TGo4ObjEnvelope() :
   TNamed(),
   fxObject(0),
   fxObjName(),
   fxObjFolder(),
   fbOwner(kTRUE)
{
}

TGo4ObjEnvelope::TGo4ObjEnvelope(TObject* obj, const char* name, const char* folder) :
   TNamed("Go4Envelope",""),
   fxObject(obj),
   fxObjName(name),
   fxObjFolder(folder),
   fbOwner(kFALSE)
{
}

TGo4ObjEnvelope::~TGo4ObjEnvelope()
{
   if (fbOwner && fxObject) {
      cout << "Delete object in envelope" << fxObject->GetName() << endl;
      delete fxObject;
   }
}

TObject* TGo4ObjEnvelope::TakeObject()
{
   TObject* res = fxObject;
   fxObject = 0;
   fbOwner = kFALSE;
   return res;
}
