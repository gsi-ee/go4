// $Id: TGo4EllipseCond.h 1313 2015-01-06 15:20:32Z adamczew $
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

#ifndef TGO4ELLIPSECOND_H
#define TGO4ELLIPSECOND_H

#include "TGo4PolyCond.h"

class TH1;
class TH2;

/* default number of points to render ellipse*/
#define GO4ELLIPSECOND_DEFAULTRESOLUTION 180


/*
 * JAM Jan-2015
 * New : specialized polygon condition for circular/elliptical shaped cuts.
 * Benefit is setter methods with dedicated radius/ point parameters
 *
 * */

class TGo4EllipseCond : public TGo4PolyCond {
   public:
      TGo4EllipseCond();

      TGo4EllipseCond(const char* name, const char* title = "Go4 ellipse condition");

      virtual ~TGo4EllipseCond();


      /* Delete old cut values and redefine them as elliptical shape.
       * Parameters: center coordinates       cx,cy
       *             half axes                a1,a2
       *             tilt angle               theta
       *             number of polygon points npoints (0 uses default granularity)*/
      void SetEllipse(Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta=0, Int_t npoints=0);

      /* Delete old cut values and redefine them as circular shape.
       * Parameters: center coordinates       cx,cy
       *             radius                   r
       *             number of polygon points npoints (0 uses default granularity)*/
      void SetCircle(Double_t cx, Double_t cy, Double_t r, Int_t npoints=0);


      /* mark this ellipse as circle type. usefull for condition editor display*/
      void SetCircle(Bool_t yes=true)
      {
        fbIsCircle=yes;
      }
      Bool_t IsCircle()
      {
        return fbIsCircle;
      }


      /* Retrieve current center coordinates*/
      void GetCenter(Double_t& x, Double_t& y)
      {
          x=fdCenterX;
          y=fdCenterY;
      }

      /* Set Center coordinates and recalculate polygon*/
      void SetCenter(Double_t x, Double_t y);

      /* Retrieve ellipse half axis coordinates*/
      void GetRadius(Double_t& a1, Double_t& a2)
      {
          a1=fdRadius1;
          a2=fdRadius2;
      }

      /* Set Center coordinates and recalculate polygon*/
      void SetRadius(Double_t a1, Double_t a2);

      /* get current ellipse tilt angle in degrees*/
      Double_t GetTheta(){
        return fdTheta;
      }

      /* change ellipse tilt angle and recalculate polygon*/
      void SetTheta(Double_t angle);


      /** Printout values. If points is true, printout polygon points. */
      virtual void PrintCondition(Bool_t points = kTRUE);

       /** Copy values from cond to this.
       * Get a clone cut from cond by CloneCut as new cut. If counts is true, copy counters too. */
       Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);

      /** Method used by HTTP server to update some fields, specified in URL syntax */
      virtual Bool_t UpdateFromUrl(const char* rest_url_opt);

      Bool_t IsEllipseType()
      {
         return kTRUE;
      }

      void SetResolution(Int_t npoints)
      {
        fiResolution=npoints;
      }

      Int_t GetResolution(){
        return fiResolution;
      }

      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_RESOLUTION;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_CX;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_CY;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_A1;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_A2;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_TH;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_CIRCLE;



   protected:

      /* This funcion will evaluate new polygon points whenever an ellipse parameter is changed.*/
      void ResetPolygon();


  private:

      Int_t fiResolution; //< number of polygon points to render
      Double_t fdCenterX; //< centroid coordinate X
      Double_t fdCenterY; //< centroid coordinate Y

      Double_t fdRadius1;   //< ellipse first half axis
      Double_t fdRadius2;   //< ellipse second half axis

      Double_t fdTheta;    //< ellipse tilt angle

      Bool_t fbIsCircle; //< mark this ellipse as circle type

   ClassDef(TGo4EllipseCond,1)
};

#endif //TGO4ELLIPSECOND_H
