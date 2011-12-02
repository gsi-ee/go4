
void set_Par()
{
	TXXXParameter *fp = (TXXXParameter*) go4->GetParameter("XXXParameter","TXXXParameter");
	if(fp==0) return;

   // set values here-----------
	fp->frP1    = 100;
	fp->frP2    = 200;
	fp->fbHisto = kTRUE;
}
