// written by setfill.C macro
// executed in TXXXProc
{
TXXXControl * fCtl;
fCtl = (TXXXControl *)(TGo4Analysis::Instance()->GetParameter("Control"));
if(fCtl!=0) fCtl->fill=kTRUE;
}
