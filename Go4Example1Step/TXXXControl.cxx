#include "TXXXControl.h"

#include <stdio.h>
#include "Riostream.h"

//***********************************************************
TXXXControl::TXXXControl() :
   TGo4Parameter("Control"),
   fill(kTRUE)
{
}
//***********************************************************
TXXXControl::TXXXControl(const char* name) :
   TGo4Parameter(name),
   fill(kTRUE)
{
}
//***********************************************************
TXXXControl::~TXXXControl() {}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXControl::PrintParameter(Text_t * n, Int_t){
  cout << "**** TXXXControl: ";
  cout << "Histogram filling ";
  if(fill == kFALSE) cout << "disabled " << endl;
  else               cout << "enabled " << endl;
  return 0;

}
//-----------------------------------------------------------
Bool_t TXXXControl::UpdateFrom(TGo4Parameter *pp)
{
  cout << "**** TXXXControl " << GetName() << " updated from auto save file" << endl;
  if(pp->InheritsFrom(TXXXControl::Class()))
    {
      TXXXControl * from = (TXXXControl *) pp;
      fill=from->fill;
    }
  else
    cout << "Wrong parameter object: " << pp->ClassName() << endl;

  SaveMacro();
  PrintParameter(0,0);
  return kTRUE;
}
//-----------------------------------------------------------
void TXXXControl::SaveMacro()
{
  FILE *pf;
  pf=fopen("histofill.C","w+");
  if(pf)
    {
      fputs("// written by setfill.C macro\n",pf);
      fputs("// executed in TXXXProc\n",pf);
      fputs("{\n",pf);
      fputs("TXXXControl * fCtl;\n",pf);
      fputs("fCtl = (TXXXControl *)(TGo4Analysis::Instance()->GetParameter(\"Control\"));\n",pf);
      if(fill)fputs("fCtl->fill=kTRUE;\n",pf);
      else    fputs("fCtl->fill=kFALSE;\n",pf);
      fputs("}\n",pf);
      fclose(pf);
      cout << "**** TXXXControl: new histofill.C written" << endl;
    }
  else cout << "**** TXXXControl: Error writing histofill.C!" << endl;

  PrintParameter(0,0);
}
