#ifndef TGO4TASKHANDLERCOMMANDLIST_H
#define TGO4TASKHANDLERCOMMANDLIST_H

#include "TGo4CommandProtoList.h"
/**
 * Class containing all command prototypes of the
 * TaskHandler system commands (and the basic test commands);
 * may be derived by application or user command list class which
 * can add own commands into the list using AddCommand Method
 * of TGo4CommandProtoList Base Class
 */
class TGo4TaskHandlerCommandList : public TGo4CommandProtoList {
  public:

    TGo4TaskHandlerCommandList(const char* name);
    virtual ~TGo4TaskHandlerCommandList();

  private:
    TGo4TaskHandlerCommandList();

   ClassDef(TGo4TaskHandlerCommandList,1)
};

#endif //TGO4TASKHANDLERCOMMANDLIST_H
