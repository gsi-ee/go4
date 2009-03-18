#ifndef TYYYEVENT_H
#define TYYYEVENT_H

#include "TGo4EventElement.h"

class TYYYUnpackProc;
class TGo4FileSource;

class TYYYUnpackEvent : public TGo4EventElement {
   public:
      TYYYUnpackEvent() ;
      TYYYUnpackEvent(const char* name) ;
      virtual ~TYYYUnpackEvent() ;
      /**
       * Method called by the event owner (analysis step) to fill the
       * event element from the set event source. Event source can
       * be the source of the analysis step (if this is a raw event)
       * or the event processor (if this is a reduced event).
       */
      Int_t Fill();

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *t="");
      Int_t Init();

      Double_t fdR[3]; // final particle radius vector
      Double_t fdV[3]; // final velocity vector
      Double_t fdGam[2]; // projected divergence angle
      Int_t fiNumScatt; // number of scatterings
      Double_t fdEmittX; // pi*x*x'
      Double_t fdEmittY; // pi*y*y'
      Double_t fdEmitt4d; //EmittX*Y

   private:
      TYYYUnpackProc * fxYYYEP;  //! Don't put this to file
      TGo4FileSource * fxYYYFS;  //! Don't put this to file

   ClassDef(TYYYUnpackEvent,1)
};
#endif //TYYYEVENT_H



