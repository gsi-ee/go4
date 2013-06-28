#ifndef TGo4DabcFactory_H
#define TGo4DabcFactory_H

#ifndef DABC_Factory
#include "dabc/Factory.h"
#endif

class TGo4DabcFactory : public dabc::Factory {
   public:
      TGo4DabcFactory(const std::string& name) : dabc::Factory(name) {}

      virtual dabc::Reference CreateObject(const std::string& classname, const std::string& objname, dabc::Command cmd);
};

#endif
