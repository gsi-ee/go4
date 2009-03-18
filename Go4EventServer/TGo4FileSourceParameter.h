#ifndef TGO4FILESOURCEPARAMETER_H
#define TGO4FILESOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4FileSourceParameter : public TGo4EventSourceParameter {

   public:

      TGo4FileSourceParameter();

      TGo4FileSourceParameter(const char* name);

      virtual ~TGo4FileSourceParameter();

   ClassDef(TGo4FileSourceParameter,1)
};

#endif //TGO4FILESOURCEPARAMETER_H
