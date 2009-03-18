#ifndef TGO4TASKHANDLERCOMMAND_H
#define TGO4TASKHANDLERCOMMAND_H

#include "TGo4Command.h"

class TGo4TaskHandlerCommand : public TGo4Command {
  public:

   TGo4TaskHandlerCommand();

   TGo4TaskHandlerCommand(const char* name, const char* description);

   virtual ~TGo4TaskHandlerCommand();

   TGo4TaskHandlerCommand(const TGo4TaskHandlerCommand &right);

   /** Optional action if command is not allowed (warning message e.g.) */
    virtual Int_t RefuseCom();

  ClassDef(TGo4TaskHandlerCommand,1)
};

#endif //TGO4TASKHANDLERCOMMAND_H
