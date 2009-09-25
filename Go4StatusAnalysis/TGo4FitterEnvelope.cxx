#include "TGo4FitterEnvelope.h"

#include "Riostream.h"

TGo4FitterEnvelope::TGo4FitterEnvelope()
: TGo4Parameter("Fitter envelope"), fxFitter(0)
{
}

TGo4FitterEnvelope::TGo4FitterEnvelope(const char* name, TGo4Fitter* fitter)
: TGo4Parameter(name)
{
   fxFitter=fitter;
}

TGo4FitterEnvelope::~TGo4FitterEnvelope()
{
   if(fxFitter) delete fxFitter;
}

Int_t TGo4FitterEnvelope::PrintParameter(Text_t * buffer, Int_t buflen)
{
   Int_t locallen=128000;
   Text_t localbuf[128000];
   if(buflen<0 && buffer!=0)
      return 0;
   Int_t size=0;
   Text_t* current=localbuf;
   size+=TGo4Parameter::PrintParameter(current,locallen-size-1);
   current= localbuf + size;
   if(fxFitter)
      size+=sprintf(current," \t This object contains the fitter: %s \n",fxFitter->GetName());
   else
      size+=sprintf(current," \t This object contains no fitter\n");
   current=localbuf+size;
   // here we optionally might put printout of fitter if we like...
   if(buffer==0)
   {
      cout << localbuf << endl;
   }
   else
   {
      if(size>buflen-1)
         size=buflen-1;
      strncpy(buffer,localbuf,size);
   }
   return size;
}

Bool_t TGo4FitterEnvelope::UpdateFrom(TGo4Parameter *pp)
{
   TGo4FitterEnvelope * from = dynamic_cast<TGo4FitterEnvelope *> (pp);

   if(from==0) return kFALSE;

   if(fxFitter!=0) delete fxFitter;
   fxFitter = from->GetFitter(kTRUE);// change ownership to us
   return kTRUE;
}

void TGo4FitterEnvelope::Clear(Option_t* opt)
{
   if(fxFitter) fxFitter->Clear(opt);
}

TGo4Fitter* TGo4FitterEnvelope::GetFitter(Bool_t chown)
{
   TGo4Fitter* rev=fxFitter;
   if(chown) fxFitter=0;
   return rev;
}

void TGo4FitterEnvelope::SetFitter(TGo4Fitter* fitter)
{
   if(fxFitter!=0) delete fxFitter;
   fxFitter=fitter;
}
