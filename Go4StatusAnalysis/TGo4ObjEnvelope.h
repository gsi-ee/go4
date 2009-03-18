#ifndef TGO4OBJENVELOPE_H
#define TGO4OBJENVELOPE_H

#include "TNamed.h"

class TGo4ObjEnvelope : public TNamed {
   public:
      TGo4ObjEnvelope();
      TGo4ObjEnvelope(TObject* obj, const char* name, const char* folder);
      virtual ~TGo4ObjEnvelope();

      const char* GetObjName() const { return fxObjName.Data(); }
      const char* GetObjFolder() const { return fxObjFolder.Data(); }

      TObject* GetObject() const { return fxObject; }
      TObject* TakeObject();

   protected:

      TObject*  fxObject;
      TString   fxObjName;
      TString   fxObjFolder;

      /** this flag true when envelope created via default constructor
        * in this case object, included in envelope should be destroyed in
        * envelope itself */
      Bool_t    fbOwner;      //!

   ClassDef(TGo4ObjEnvelope, 1);
};


#endif
