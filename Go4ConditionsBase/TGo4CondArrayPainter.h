#ifndef TGO4CONDARRAYPAINTER_H
#define TGO4CONDARRAYPAINTER_H

#include "TGo4ConditionPainter.h"

class TGo4CondArrayPainter : public TGo4ConditionPainter {
  public:
    TGo4CondArrayPainter();
    virtual ~TGo4CondArrayPainter();

    TGo4CondArrayPainter(const Text_t* name);

   /** Display condition with plain root canvas and in go4 viewpanel */
   virtual void PaintCondition(Option_t* opt="");

   /** Erase condition view */
   virtual void UnPaintCondition(Option_t* opt="");

   /** To suppress the label of the array, we overwrite method here. */
   virtual void PaintLabel(Option_t* opt="");

  ClassDef(TGo4CondArrayPainter,6)
};

#endif //TGO4CONDARRAYPAINTER_H
