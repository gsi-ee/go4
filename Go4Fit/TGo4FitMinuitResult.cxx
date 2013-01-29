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

#include "TGo4FitMinuitResult.h"
#include "Riostream.h"

TGo4FitMinuitResult::TGo4FitMinuitResult() :
    TNamed(),
    ParValues(0),
    ParError(0),
    EPLUS(0),
    EMINUS(0),
    EPARAB(0),
    GLOBCC(0),
    ERRORMATRIX(0),
    CONTOX(0),
    CONTOY(0),
    CONTOCH(0) {
}

TGo4FitMinuitResult::TGo4FitMinuitResult(const char* iName, const char* iTitle) :
    TNamed(iName,iTitle),
    ParValues(0),
    ParError(0),
    EPLUS(0),
    EMINUS(0),
    EPARAB(0),
    GLOBCC(0),
    ERRORMATRIX(0),
    CONTOX(0),
    CONTOY(0),
    CONTOCH(0)  {
}

TGo4FitMinuitResult::~TGo4FitMinuitResult() {
   if(ParValues) delete ParValues;
   if(ParError) delete  ParError;
   if(EPLUS) delete EPLUS;
   if(EMINUS) delete EMINUS;
   if(EPARAB) delete EPARAB;
   if(GLOBCC) delete GLOBCC;
   if(ERRORMATRIX) delete ERRORMATRIX;
   if(CONTOX) delete CONTOX;
   if(CONTOY) delete CONTOY;
   if(CONTOCH) delete CONTOCH;
}

void TGo4FitMinuitResult::CallMNSTAT(TMinuit* fMinuit) {
    fMinuit->mnstat(FMIN,FEDM,ERRDEF,NPARI,NPARX,ISTAT);
}

void TGo4FitMinuitResult::CallMNPOUT(TMinuit* fMinuit, Int_t nPars) {
    if(ParValues) delete ParValues;
    if(ParError) delete  ParError;

    ParValues = new TArrayD(nPars);
    ParError = new TArrayD(nPars);
    for(Int_t n=0;n<nPars;n++)
       fMinuit->GetParameter(n, (*ParValues)[n], (*ParError)[n]);
}

void TGo4FitMinuitResult::CallMNERRS(TMinuit* fMinuit, Int_t nPars) {
   if(EPLUS) delete EPLUS;
   if(EMINUS) delete EMINUS;
   if(EPARAB) delete EPARAB;
   if(GLOBCC) delete GLOBCC;

   EPLUS = new TArrayD(nPars);
   EMINUS = new TArrayD(nPars);
   EPARAB = new TArrayD(nPars);
   GLOBCC = new TArrayD(nPars);

   for(Int_t n=0;n<nPars;n++)
       fMinuit->mnerrs(n, (*EPLUS)[n], (*EMINUS)[n], (*EPARAB)[n], (*GLOBCC)[n]);

}

void TGo4FitMinuitResult::CallMNEMAT(TMinuit* fMinuit, Int_t nPars, Bool_t DoTransform)
{
   if(ERRORMATRIX) delete ERRORMATRIX;

//   ERRORMATRIX = new TMatrixD(nPars,nPars);
//   fMinuit->mnemat(ERRORMATRIX->GetElements(), nPars);

   Double_t* matrix = new Double_t[nPars*nPars];
   fMinuit->mnemat(matrix, nPars);
   ERRORMATRIX = new TMatrixD(nPars, nPars, matrix);

   if(DoTransform)
   for(Int_t npar=0;npar<nPars;npar++) {
       TString name;
       Double_t val,err,bnd1,bnd2;
       Int_t fixed = 0;
       fMinuit->mnpout(npar,name,val,err,bnd1,bnd2,fixed);

       if (fixed==0) {
          for(Int_t n=nPars-2;n>=npar-1;n--)
            for(Int_t i=0;i<nPars;i++)
               (*ERRORMATRIX)(i,n+1) = (*ERRORMATRIX)(i,n);
          for(Int_t i=0;i<nPars;i++)
             (*ERRORMATRIX)(i,npar-1) = 0;

          for(Int_t n=nPars-2;n>=npar-1;n--)
             for(Int_t i=0;i<nPars;i++)
               (*ERRORMATRIX)(n+1,i) = (*ERRORMATRIX)(n,i);
          for(Int_t i=0;i<nPars;i++)
             (*ERRORMATRIX)(npar-1,i) = 0;
       }
   }

   delete[] matrix;
}

void TGo4FitMinuitResult::GetContourPlot(TMinuit* fMinuit) {
   Int_t sz = strlen(fMinuit->fChpt);
   if (sz == 0) return;
   CONTOX = new TArrayD(sz);
   CONTOY = new TArrayD(sz);
   CONTOCH = new TArrayC(sz);
   for (Int_t i=0;i<sz;i++) {
      (*CONTOX)[i] = fMinuit->fXpt[i];
      (*CONTOY)[i] = fMinuit->fYpt[i];
      (*CONTOCH)[i] = fMinuit->fChpt[i];
   }

}

void TGo4FitMinuitResult::Print(Option_t* option) const {
    TNamed::Print(option);
    std::cout << std::endl << "Minuit status:" << std::endl;
    std::cout << "  The best function value found so far " << FMIN << std::endl;
    std::cout << "  The estimated vertical distance remaining to minimum " << FEDM << std::endl;
    std::cout << "  The value of UP defining parameter uncertaintiesd " << ERRDEF << std::endl;
    std::cout << "  The number of currently variable parameters " << NPARI << std::endl;
    std::cout << "  The highest defined parameter number " << NPARX  << std::endl;
    std::cout << "  Goodness of covariance matrix: " << std::endl << "    ";
    switch (ISTAT) {
       case 1: std::cout << "Diagonal approximation only, not accurate" << std::endl; break;
       case 2: std::cout << "Full matrix, but forced positive-defined" << std::endl; break;
       case 3: std::cout << "Full accurate covariance matrix (after MIGRAD)" << std::endl; break;
       default: std::cout << "Not calculated at all" << std::endl;
    }

    if (ParValues && ParError) {
       std::cout << "Current parameters valules and errors:" << std::endl;
       for(Int_t n=0;n<ParValues->GetSize();n++)
         std::cout << "  " << n << "   " << ParValues->At(n) << "     " << ParError->At(n) << std::endl;
    }

    if (EPLUS && EMINUS && EPARAB && GLOBCC) {
       std::cout << "Parameters errors, defined by MINOS routine" << std::endl;
       std::cout << "  NUM   EPLUS   EMINUS   EPARAB    GLOBCC" << std::endl;
       for(Int_t n=0;n<EPLUS->GetSize();n++)
         std::cout << "  " << n << "   " << EPLUS->At(n) << "  " << EMINUS->At(n) << "  " << EPARAB->At(n) << "  " << GLOBCC->At(n) << std::endl;
    }

    if (ERRORMATRIX) {
       std::cout << "Error matrix" << std::endl;
       ERRORMATRIX->Print();
    }

    std::cout << std::endl;
}
