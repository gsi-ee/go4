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

#ifndef TGO4SHAPEDCOND_H
#define TGO4SHAPEDCOND_H

#include "TGo4PolyCond.h"

class TH1;
class TH2;

/* default number of points to render ellipse*/
#define GO4ELLIPSECOND_DEFAULTRESOLUTION 180

enum Go4CondShape_t
{
  Go4Cond_Shape_None = 0,
  Go4Cond_Shape_Free = 1,
  Go4Cond_Shape_Circle = 2,
  Go4Cond_Shape_Ellipse = 3,
  Go4Cond_Shape_Box = 4
};
/*
 * JAM Jan-2015
 * New : specialized polygon condition for circular/elliptical shaped cuts.
 * Benefit is setter methods with dedicated radius/ point parameters
 *
 * */

class TGo4ShapedCond : public TGo4PolyCond {
   public:
      TGo4ShapedCond();

      TGo4ShapedCond(const char* name, const char* title = "Go4 ellipse condition");

      virtual ~TGo4ShapedCond();


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



      /* Delete old cut values and redefine them as tilted box shape.
       * Parameters: center coordinates       cx,cy
       *             half axes                a1,a2
       *             tilt angle               theta
       *             number of polygon points is always 5 for box*/
      void SetBox(Double_t cx, Double_t cy, Double_t a1, Double_t a2, Double_t theta=0);



      /* mark this ellipse as circle type. usefull for condition editor display*/
      void SetCircle()
      {
        fiShapeType=Go4Cond_Shape_Circle;
      }
      Bool_t IsCircle()
      {
        return (fiShapeType==Go4Cond_Shape_Circle);
      }
      void SetEllipse()
        {
        fiShapeType=Go4Cond_Shape_Ellipse;
        }
      Bool_t IsEllipse()
      {
        return (fiShapeType==Go4Cond_Shape_Ellipse);
      }

      void SetBox()
        {
        fiShapeType=Go4Cond_Shape_Box;
        }
      Bool_t IsBox()
        {
          return (fiShapeType==Go4Cond_Shape_Box);
        }

      void SetFreeShape()
        {
        fiShapeType=Go4Cond_Shape_Free;
        }
      Bool_t IsFreeShape()
        {
        return (fiShapeType==Go4Cond_Shape_Free);
        }


      const char* GetShapeName()
      {
        switch(fiShapeType)
        {
          case Go4Cond_Shape_Free:
            return "Free style polygon";
            break;
          case Go4Cond_Shape_Circle:
            return "Circle shaped polygon";
            break;
          case Go4Cond_Shape_Ellipse:
            return "Ellipse shaped polygon";
            break;
          case Go4Cond_Shape_Box:
            return "Rectangular box shaped polygon";
            break;
          default:
           return "Shape not defined!";
           break;
        };
        return 0;

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

      Bool_t IsShapedType()
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
      static TString fgxURL_SHAPE;



   protected:

      /* This funcion will evaluate new polygon points whenever an ellipse parameter is changed.*/
      void ResetPolygon();

      /* evaluate polygon array to form elliptical/circular shapes*/
      void DefineEllipse(Double_t *x, Double_t *y, Int_t n);

      /* evaluate polygon array to form box shapes*/
      void DefineBox(Double_t *x, Double_t *y, Int_t n);

  private:

      Int_t fiResolution; //< number of polygon points to render
      Double_t fdCenterX; //< centroid coordinate X
      Double_t fdCenterY; //< centroid coordinate Y

      Double_t fdRadius1;   //< ellipse/box first half axis
      Double_t fdRadius2;   //< ellipse/box second half axis

      Double_t fdTheta;    //< ellipse tilt angle
      Go4CondShape_t fiShapeType; //< type of shaped form

   ClassDef(TGo4ShapedCond,1)
};

#define TGo4EllipseCond TGo4ShapedCond

/* < this is for simple backward compatibility and better than typedef*/

#endif //TGO4SHAPEDCOND_H
