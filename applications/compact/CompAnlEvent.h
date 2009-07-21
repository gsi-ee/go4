//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompANLEVENT_H
#define CompANLEVENT_H

#include "TGo4EventElement.h"

class CompAnlProc;
class TGo4FileSource;

class CompAnlEvent : public TGo4EventElement {
   public:
      CompAnlEvent() ;
      CompAnlEvent(CompAnlEvent* source) ;
      CompAnlEvent(const char* name) ;
      virtual ~CompAnlEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t=""){
      }

   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      CompAnlProc   * fxCompCP;  //! Don't put this to file
      TGo4FileSource * fxCompFS;  //! Don't put this to file

      UInt_t fiTest[10];

   ClassDef(CompAnlEvent,1)
};
#endif //CompANLEVENT_H



