#ifndef TGO4FITTERACTION_H
#define TGO4FITTERACTION_H

#include "TGo4FitNamed.h"

class TGo4FitterAbstract;

/**
 * Basic class for objects, which performs actions on fitter.
 */
class TGo4FitterAction : public TGo4FitNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitterAction();

      /**
       * Creates TGo4FitterAction object with given name and title.
       */
      TGo4FitterAction(const char* Name, const char* Title, TNamed* Owner = 0);
      virtual ~TGo4FitterAction();

      virtual void DoAction(TGo4FitterAbstract* Fitter) = 0;

      virtual Bool_t CanChangeFitter() const { return kFALSE; }
      virtual Bool_t NeedBuffers() const { return kFALSE; }

   ClassDef(TGo4FitterAction,1)
};
#endif // TGO4FITTERACTION_H
