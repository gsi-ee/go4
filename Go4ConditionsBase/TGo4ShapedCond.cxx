// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ShapedCond.h"

#include "Riostream.h"

#include "TMath.h"
#include "TROOT.h"
#include "TH2.h"
#include "TList.h"
#include "TCutG.h"

#include "TGo4Log.h"


TString TGo4ShapedCond::fgxURL_RESOLUTION  ="ellinpts";
TString TGo4ShapedCond::fgxURL_CX          ="ellicx";
TString TGo4ShapedCond::fgxURL_CY          ="ellicy";
TString TGo4ShapedCond::fgxURL_A1          ="ellia1";
TString TGo4ShapedCond::fgxURL_A2          ="ellia2";
TString TGo4ShapedCond::fgxURL_TH          ="ellith";
TString TGo4ShapedCond::fgxURL_SHAPE      ="ellishape";

// ----------------------------------------------------------
TGo4ShapedCond::TGo4ShapedCond() :
   TGo4PolyCond(), fiResolution(GO4ELLIPSECOND_DEFAULTRESOLUTION),fdCenterX(0),fdCenterY(0), fdRadius1(0),fdRadius2(0), fdTheta(0),fiShapeType(Go4Cond_Shape_None)
{
}
// ----------------------------------------------------------
TGo4ShapedCond::TGo4ShapedCond(const char* name, const char* title) :
    TGo4PolyCond(name,title), fiResolution(GO4ELLIPSECOND_DEFAULTRESOLUTION),fdCenterX(0),fdCenterY(0), fdRadius1(0),fdRadius2(0), fdTheta(0),fiShapeType(Go4Cond_Shape_None)
{
}
// ----------------------------------------------------------
TGo4ShapedCond::~TGo4ShapedCond()
{

}


// ----------------------------------------------------------
void TGo4ShapedCond::PrintCondition(Bool_t points)
{
   TGo4PolyCond::PrintCondition(points);
   std::cout << "**** Geometrical shaped condition with parameters:" << std::endl;
   std::cout << "****  Shape Type:  "<<fiShapeType << "("<<GetShapeName()<<")"<<std::endl;
   std::cout << "****  Center:    ("<<fdCenterX<<","<< fdCenterY<<")" << std::endl;
   std::cout << "****  Radius:    ("<<fdRadius1<<","<< fdRadius2<<")" << std::endl;
   std::cout << "****  Tilt:       "<<fdTheta<<" deg" << std::endl;
   std::cout << "****  Resolution: "<<fiResolution<<" points" << std::endl;

}

// ----------------------------------------------------------
Bool_t TGo4ShapedCond::UpdateFrom(TGo4Condition * cond, Bool_t counts)
{
  if(!TGo4PolyCond::UpdateFrom(cond,counts)) return kFALSE;
  if(cond->InheritsFrom(TGo4ShapedCond::Class()))
     {
        TGo4ShapedCond* from= (TGo4ShapedCond*)cond;
        fdCenterX = from->fdCenterX;
        fdCenterY = from->fdCenterY;
        fdRadius1 = from->fdRadius1;
        fdRadius2 = from->fdRadius2;
        fdTheta   = from->fdTheta;
        fiResolution   = from->fiResolution;

        fiShapeType   = from->fiShapeType;
        // TODO: probably later we only update ellipse parameters and render polygon points from that.
        //PrintCondition();
        return kTRUE;
     }
 else
   {
      std::cout << "Cannot update " << GetName() << " from " << cond->ClassName() << std::endl;
      return kFALSE;
   }
  return kTRUE;
}


Bool_t TGo4ShapedCond::UpdateFromUrl(const char* rest_url_opt){
  //if(!TGo4PolyCond::UpdateFromUrl(rest_url_opt)) return kFALSE;
  // do not always update polygon points, but first reconstruct from shape parameters!

  if(!TGo4Condition::UpdateFromUrl(rest_url_opt)) return kFALSE; // still may change condition base class flags

  TString message;
  message.Form("TGo4ShapedCond::UpdateFromUrl - condition %s: with url:%s\n", GetName(), rest_url_opt);
     Double_t x=0,y=0,a=0,b=0;
     GetCenter(x,y);
     GetRadius(a,b); // old values as defaults
     Double_t theta=GetTheta();
     x= GetUrlOptionAsDouble(TGo4ShapedCond::fgxURL_CX.Data(), x);
     y = GetUrlOptionAsDouble(TGo4ShapedCond::fgxURL_CY.Data(), y);
     a = GetUrlOptionAsDouble(TGo4ShapedCond::fgxURL_A1.Data(), a);
     b = GetUrlOptionAsDouble(TGo4ShapedCond::fgxURL_A2.Data(), b);
     theta=GetUrlOptionAsInt(TGo4ShapedCond::fgxURL_TH.Data(), theta);
     Int_t resolution = GetUrlOptionAsInt(TGo4ShapedCond::fgxURL_RESOLUTION.Data(), GetResolution());
     Int_t shapetype = GetUrlOptionAsInt(TGo4ShapedCond::fgxURL_SHAPE.Data(), fiShapeType);


     if(shapetype==Go4Cond_Shape_Circle)
       {
         message.Append(TString::Format("Set to Circle shape with x:%f, y:%f, r:%f, points=%d", x,y,a,resolution));
         SetCircle(x,y,a,resolution);

       }
       else if(shapetype==Go4Cond_Shape_Ellipse)
       {
         message.Append(TString::Format("Set  to Ellipse shape with x:%f, y:%f, a:%f b:%f,theta:%f, points=%d",
             x, y, a, b, theta, resolution));
         SetEllipse(x,y,a,b,theta, resolution);
       }
       else if(shapetype==Go4Cond_Shape_Box)
       {
         message.Append(TString::Format("Set to Box shape with x:%f, y:%f, a:%f b:%f,theta:%f",
             x, y, a, b, theta));
         SetBox(x,y,a,b,theta);
       }
       else if(shapetype==Go4Cond_Shape_Free)
       {
         message.Append(TString::Format("Set to Free polygon shape!"));
         SetFreeShape();
         if(!TGo4PolyCond::UpdateFromUrl(rest_url_opt)) return kFALSE; // here we might get also different point coordinates...
       }
       TGo4Log::Message(1,message.Data());
  return kTRUE;
}


void TGo4ShapedCond::SetEllipse(Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta, Int_t npoints)
{
  fiResolution=npoints;
  fdCenterX=cx;
  fdCenterY=cy;
  fdRadius1=a1;
  fdRadius2=a2;
  fdTheta=theta;
  SetEllipse();
  ResetPolygon();
}


void TGo4ShapedCond::SetCircle(Double_t cx, Double_t cy, Double_t r, Int_t npoints)
{
  fiResolution=npoints;
  fdCenterX=cx;
  fdCenterY=cy;
  fdRadius1=r;
  fdRadius2=r;
  fdTheta=0.;
  SetCircle();
  ResetPolygon();
}

void TGo4ShapedCond::SetBox(Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta)
{
  fiResolution=4;
  fdCenterX=cx;
  fdCenterY=cy;
  fdRadius1=a1;
  fdRadius2=a2;
  fdTheta=theta;
  SetBox();
  ResetPolygon();
}

void TGo4ShapedCond::SetCenter(Double_t x, Double_t y)
{
  fdCenterX=x;
  fdCenterY=y;
  ResetPolygon();
}

void TGo4ShapedCond::SetRadius(Double_t a1, Double_t a2)
{
  fdRadius1=a1;
  fdRadius2=a2;
  ResetPolygon();
}

void TGo4ShapedCond::SetTheta(Double_t angle)
{
  fdTheta=angle;
  ResetPolygon();
}

void TGo4ShapedCond::ResetPolygon()
{
  // stolen from ROOT TEllipse rendering code. we use it just to calculate polygon points
  // thanks to Sven Augustin, MPI Heidelberg, for the idea and first version of the code!
  if(fiResolution==0) fiResolution=GO4ELLIPSECOND_DEFAULTRESOLUTION;
  Double_t *x = new Double_t[fiResolution+1];
  Double_t *y = new Double_t[fiResolution+1];
  if(IsEllipse() || IsCircle())
    DefineEllipse(x,y,fiResolution);
  else if(IsBox())
    DefineBox(x,y,fiResolution);
  else
    {
      delete[] x;
      delete[] y;
      return;
    }
  SetValues(x,y, fiResolution+1);

  delete[] x;
  delete[] y;

}


void TGo4ShapedCond::DefineEllipse(Double_t *x, Double_t *y, Int_t n)
{
  Double_t dx,dy;
   Double_t dphi = TMath::TwoPi()/n;
   Double_t ct   = TMath::Cos(TMath::Pi()*fdTheta/180);
   Double_t st   = TMath::Sin(TMath::Pi()*fdTheta/180);
   for (Int_t i = 0; i < n; i++)
   {
     dx = fdRadius1 * TMath::Cos((Double_t) i * dphi);
     dy = fdRadius2 * TMath::Sin((Double_t) i * dphi);
     x[i] = fdCenterX + dx * ct - dy * st;
     y[i] = fdCenterY + dx * st + dy * ct;
   }
   x[n] = x[0];
   y[n] = y[0];

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



}

void TGo4ShapedCond::DefineBox(Double_t *x, Double_t *y, Int_t n)
{
  Double_t ct   = TMath::Cos(TMath::Pi()*fdTheta/180);
  Double_t st   = TMath::Sin(TMath::Pi()*fdTheta/180);
  x[0] = fdCenterX + fdRadius1 * ct + fdRadius2 * st;
  y[0] = fdCenterY + fdRadius1 * st - fdRadius2 * ct;
  x[1] = fdCenterX + fdRadius1 * ct - fdRadius2 * st;
  y[1] = fdCenterY + fdRadius1 * st + fdRadius2 * ct;
  x[2] = fdCenterX - (fdRadius1 * ct + fdRadius2 * st);
  y[2] = fdCenterY - (fdRadius1 * st - fdRadius2 * ct);
  x[3] = fdCenterX - (fdRadius1 * ct - fdRadius2 * st);
  y[3] = fdCenterY - (fdRadius1 * st + fdRadius2 * ct);
  for(Int_t i=4; i< n+1; ++i)
  {
    x[i] = x[0];
    y[i] = y[0];
  }
}
