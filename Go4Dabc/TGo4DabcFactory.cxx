#include "TGo4DabcFactory.h"

#include "TGo4Sniffer.h"

// do not create factory at the moment

// dabc::FactoryPlugin dabc_go4_factory(new TGo4DabcFactory("dabc_go4"));


dabc::Reference TGo4DabcFactory::CreateObject(const std::string& classname, const std::string& objname, dabc::Command cmd)
{
   if (classname=="TGo4Sniffer")
      return new TGo4Sniffer(objname, cmd);

   return dabc::Factory::CreateObject(classname, objname, cmd);
}
