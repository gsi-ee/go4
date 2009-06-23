// Macro to set histogram fill status in TXXXcontrol.
// Executed in Analysis terminal input line by .x <filename>.
// Creates macro histofill.C executed in TXXXProc (at submit)
// This macro, not the auto save file, will set status
// Changing status in parameter editor is valid only until next
// submit.

void setfill(Int_t value)
{
  TXXXControl * fCtl;
  fCtl = (TXXXControl *)(go4->GetParameter("Control"));
  if(fCtl != 0)
    {
      if(value) fCtl->fill = kTRUE;
      else     fCtl->fill = kFALSE;
      fCtl->SaveMacro();
    }
  else cout "Parameter Control not found!" << endl;
}
