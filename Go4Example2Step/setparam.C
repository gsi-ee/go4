
#include <Riostream.h>

void setparam(Bool_t print)
{
// set here values -----------
Float_t Par1P1   = 100;
Float_t Par1P2   = 200;
Bool_t  Par1Enab = kTRUE;
Float_t Par2P1   = 1000;
Float_t Par2P2   = 2000;
Bool_t  Par2Enab = kTRUE;
//-----------------------------

	TXXXParameter *fp;
	fp = (TXXXParameter *) go4->GetObject("Parameters/XXXPar1","Go4");
	if(fp==0) return;
	fp->frP1 = Par1P1;
	fp->frP2 = Par1P2;
	fp->fbEnab=Par1Enab;
	if(print)fp->Print();
	fp = (TXXXParameter *) go4->GetObject("Parameters/XXXPar2","Go4");
	if(fp==0) return;
	fp->frP1 = Par2P1;
	fp->frP2 = Par2P2;
	fp->fbEnab=Par2Enab;
	if(print)fp->Print();
}
