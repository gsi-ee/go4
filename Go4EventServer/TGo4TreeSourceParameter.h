#ifndef TGO4TREESOURCEPARAMETER_H
#define TGO4TREESOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4TreeSourceParameter : public TGo4EventSourceParameter {
  public:
     TGo4TreeSourceParameter();

     TGo4TreeSourceParameter(const char* name);

     virtual ~TGo4TreeSourceParameter() ;

  ClassDef(TGo4TreeSourceParameter,1)
};

#endif //TGO4TREESOURCEPARAMETER_H
