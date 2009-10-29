// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXControl.h"

#include <stdio.h>
#include "Riostream.h"

//***********************************************************
TXXXControl::TXXXControl(const char* name) :
   TGo4Parameter(name),
   fill(kTRUE)
{
}
//***********************************************************
TXXXControl::~TXXXControl()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXControl::PrintParameter(Text_t * n, Int_t)
{
  cout << "**** TXXXControl: ";
  cout << "Histogram filling ";
  if(fill) cout << "enabled " << endl;
      else cout << "disabled " << endl;
  return 0;

}
//-----------------------------------------------------------
Bool_t TXXXControl::UpdateFrom(TGo4Parameter *source)
{
  cout << "**** TXXXControl " << GetName() << " updated from auto save file" << endl;

  TXXXControl * from = dynamic_cast<TXXXControl *>(source);

  if(from==0) {
     cout << "Wrong parameter class: " << source->ClassName() << endl;
     return kFALSE;
  }

  fill = from->fill;

  SaveMacro();
  PrintParameter(0,0);
  return kTRUE;
}
//-----------------------------------------------------------
void TXXXControl::SaveMacro()
{
   FILE *pf = fopen("histofill.C","w+");
   if(pf) {
      fputs("// written by setfill.C macro\n", pf);
      fputs("// executed in TXXXProc\n", pf);
      fputs("{\n", pf);
      fputs("TXXXControl * fCtl;\n", pf);
      fputs("fCtl = (TXXXControl *)(TGo4Analysis::Instance()->GetParameter(\"Control\"));\n", pf);
      fputs("if(fCtl!=0) ", pf);
      if(fill)fputs("fCtl->fill=kTRUE;\n", pf);
      else    fputs("fCtl->fill=kFALSE;\n", pf);
      fputs("}\n",pf);
      fclose(pf);
      cout << "**** TXXXControl: new histofill.C written" << endl;
   } else
      cout << "**** TXXXControl: Error writing histofill.C!" << endl;

   PrintParameter(0,0);
}
