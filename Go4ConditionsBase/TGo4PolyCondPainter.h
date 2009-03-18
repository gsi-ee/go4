#ifndef TGO4POLYCONDPAINTER_H
#define TGO4POLYCONDPAINTER_H

#include "TGo4ConditionPainter.h"

class TGo4PolyCond;
class TGo4PolyCondView;

class TGo4PolyCondPainter : public TGo4ConditionPainter {
   public:
      TGo4PolyCondPainter();
      TGo4PolyCondPainter(const char* name, const char* title = "Go4 polygon condition");
      virtual ~TGo4PolyCondPainter();

      /** Display condition with plain root canvas and in go4 viewpanel */
      virtual void PaintCondition(Option_t* opt="");

      /** Erase condition view */
      virtual void UnPaintCondition(Option_t* opt="");

        /** pop all components to the front of the pad*/
      virtual void DisplayToFront(Option_t* opt="");


      virtual void SetCondition(TGo4Condition* con);

   private:
      /** Cut used for polygon display. This is buffered copy of condition cutg*/
      TGo4PolyCondView* fxCutView; //!

   ClassDef(TGo4PolyCondPainter,6)
};

#endif //TGO4POLYCONDPAINTER_H
