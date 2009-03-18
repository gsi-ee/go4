#ifndef TGO4EXAMPLECOMMAND_H
#define TGO4EXAMPLECOMMAND_H

#include "TGo4Command.h"

class TGo4ExampleCommand : public TGo4Command {
  public:
    TGo4ExampleCommand();
    TGo4ExampleCommand(const char* name, const char* desc);
    virtual ~TGo4ExampleCommand();
    TGo4ExampleCommand(const TGo4ExampleCommand &right);

   ClassDef(TGo4ExampleCommand,1)
};

#endif //TGO4EXAMPLECOMMAND_H
