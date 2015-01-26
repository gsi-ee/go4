// $Id: TGo4EllipseCond.cxx 1313 2015-01-06 15:20:32Z adamczew $
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

#include "TGo4EllipseCond.h"

#include "Riostream.h"
#include "RVersion.h"

#include "TMath.h"
#include "TROOT.h"
#include "TH2.h"
#include "TList.h"
#include "TCutG.h"

#include "TGo4Log.h"






// ----------------------------------------------------------
TGo4EllipseCond::TGo4EllipseCond() :
   TGo4PolyCond(), fiResolution(GO4ELLIPSECOND_DEFAULTRESOLUTION),fdCenterX(0),fdCenterY(0), fdRadius1(0),fdRadius2(0), fdTheta(0),fbIsCircle(kFALSE)
{
}
// ----------------------------------------------------------
TGo4EllipseCond::TGo4EllipseCond(const char* name, const char* title) :
    TGo4PolyCond(name,title), fiResolution(GO4ELLIPSECOND_DEFAULTRESOLUTION),fdCenterX(0),fdCenterY(0), fdRadius1(0),fdRadius2(0), fdTheta(0),fbIsCircle(kFALSE)
{
}
// ----------------------------------------------------------
TGo4EllipseCond::~TGo4EllipseCond()
{

}




// ----------------------------------------------------------
void TGo4EllipseCond::PrintCondition(Bool_t points)
{
   TGo4PolyCond::PrintCondition();
   std::cout << "**** Elliptical shape condition with parameters:" << std::endl;
   std::cout << "****  Is circle:  "<<fbIsCircle << std::endl;
   std::cout << "****  Center:    ("<<fdCenterX<<","<< fdCenterY<<")" << std::endl;
   std::cout << "****  Radius:    ("<<fdRadius1<<","<< fdRadius2<<")" << std::endl;
   std::cout << "****  Tilt:       "<<fdTheta<<" deg" << std::endl;
   std::cout << "****  Resolution: "<<fiResolution<<" points" << std::endl;

}

// ----------------------------------------------------------
Bool_t TGo4EllipseCond::UpdateFrom(TGo4Condition * cond, Bool_t counts)
{
  if(!TGo4PolyCond::UpdateFrom(cond,counts)) return kFALSE;
  if(cond->InheritsFrom(TGo4EllipseCond::Class()))
     {
        TGo4EllipseCond* from= (TGo4EllipseCond*)cond;
        fdCenterX = from->fdCenterX;
        fdCenterY = from->fdCenterY;
        fdRadius1 = from->fdRadius1;
        fdRadius2 = from->fdRadius2;
        fdTheta   = from->fdTheta;
        fiResolution   = from->fiResolution;
        fbIsCircle   = from->fbIsCircle;
        // TODO: probably later we only update ellipse paraemters and render polygon points from that.
        PrintCondition();
        return kTRUE;
     }
 else
   {
      std::cout << "Cannot update " << GetName() << " from " << cond->ClassName() << std::endl;
      return kFALSE;
   }
  return kTRUE;
}


Bool_t TGo4EllipseCond::UpdateFromUrl(const char* rest_url_opt){
  if(!TGo4PolyCond::UpdateFromUrl(rest_url_opt)) return kFALSE;
  TString message;
  message.Form("TGo4EllipseCond::UpdateFromUrl - condition %s: with url:%s", GetName(), rest_url_opt);
  TGo4Log::Message(1,message.Data());

  // TODO: evaluate ellipse parameters in url and setup polygon from this.
  return kTRUE;
}


void TGo4EllipseCond::SetEllipse(Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta, Int_t npoints)
{
  fiResolution=npoints;
  fdCenterX=cx;
  fdCenterY=cy;
  fdRadius1=a1;
  fdRadius2=a2;
  fdTheta=theta;
  SetCircle(false);
  ResetPolygon();
}


void TGo4EllipseCond::SetCircle(Double_t cx, Double_t cy, Double_t r, Int_t npoints)
{
  fiResolution=npoints;
  fdCenterX=cx;
  fdCenterY=cy;
  fdRadius1=r;
  fdRadius2=r;
  fdTheta=0.;
  SetCircle(true);
  ResetPolygon();
}

void TGo4EllipseCond::SetCenter(Double_t x, Double_t y)
{
  fdCenterX=x;
  fdCenterY=y;
  ResetPolygon();
}

void TGo4EllipseCond::SetRadius(Double_t a1, Double_t a2)
{
  fdRadius1=a1;
  fdRadius2=a2;
  ResetPolygon();
}

void TGo4EllipseCond::SetTheta(Double_t angle)
{
  fdTheta=angle;
  ResetPolygon();
}

void TGo4EllipseCond::ResetPolygon()
{
  // stolen from ROOT TEllipse rendering code. we use it just to calculate polygon points
  // thanks to Sven Augustin, MPI Heidelberg, for the idea and first version of the code!
  if(fiResolution==0) fiResolution=GO4ELLIPSECOND_DEFAULTRESOLUTION;
  Double_t x[fiResolution+1], y[fiResolution+1];
  Double_t dx,dy;
  Double_t dphi = TMath::TwoPi()/fiResolution;
  Double_t ct   = TMath::Cos(TMath::Pi()*fdTheta/180);
  Double_t st   = TMath::Sin(TMath::Pi()*fdTheta/180);
  for (Int_t i = 0; i < fiResolution; i++)
  {
    dx = fdRadius1 * TMath::Cos((Double_t) i * dphi);
    dy = fdRadius2 * TMath::Sin((Double_t) i * dphi);
    x[i] = fdCenterX + dx * ct - dy * st;
    y[i] = fdCenterY + dx * st + dy * ct;
  }
  x[fiResolution] = x[0];
  y[fiResolution] = y[0];



  // from SvenA. for circle only
//  const Int_t n = 30; //i.e., 12 degrees per slice
//         Double_t x[n+1], y[n+1];
//         Double_t dphi = TMath::TwoPi()/n;
//         for (Int_t i=0; i<n; i++)
//         {
//                 x[i] = x0 + rad * TMath::Cos(i*dphi);
//                 y[i] = y0 + rad * TMath::Sin(i*dphi);
//         }
//         x[n] = x[0];
//         y[n] = y[0];

  SetValues(x,y, fiResolution+1);

}
