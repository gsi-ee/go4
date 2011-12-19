
void set_Par()
{
	TXXXParameter *fp = (TXXXParameter*) go4->GetParameter("XXXParameter","TXXXParameter");
	if(fp==0) return;

   // set values here-----------
	fp->frP1    = 100;
	fp->frP2    = 200;
	fp->fbHisto = kTRUE;

	fp->fArr.Set(4);
	fp->fArr[0] = 1234;
   fp->fArr[1] = 2345;
   fp->fArr[2] = 3456;
   fp->fArr[3] = 4567;
}
