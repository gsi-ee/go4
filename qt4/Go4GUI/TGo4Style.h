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

#ifndef TGO4STYLE_H
#define TGO4STYLE_H

#include <QWidget>
#include "ui_TGo4Style.h"

#include "RVersion.h"

/** Translate the ROOT palette styles to indices of dropdown menu here:
 * In simplest case this is just an offset by 51*/

/** minimum and maximum ROOT named palette types:*/
#define GO4NAMEDPAL_MIN 51

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
#define GO4NAMEDPAL_MAX 112
#elif ROOT_VERSION_CODE >= ROOT_VERSION(5,34,0)
#define GO4NAMEDPAL_MAX 56
#else
#define GO4NAMEDPAL_MAX 50 // no named entries for old ROOT
#endif


enum Go4_Palette_t
{
  Go4_None,
  Go4_DeepSea,
  Go4_GreyScale,
  Go4_DarkBodyRadiator,
  Go4_BlueYellow,
  Go4_RainBow,
  Go4_InvertedDarkBodyRadiator,
  Go4_Bird,
  Go4_Cubehelix,
  Go4_GreenRedViolet,
  Go4_BlueRedYellow,
  Go4_Ocean,
  Go4_ColorPrintableOnGrey,
  Go4_Alpine,
  Go4_Aquamarine,
  Go4_Army,
  Go4_Atlantic,
  Go4_Aurora,
  Go4_Avocado,
  Go4_Beach,
  Go4_BlackBody,
  Go4_BlueGreenYellow,
  Go4_BrownCyan,
  Go4_CMYK,
  Go4_Candy,
  Go4_Cherry,
  Go4_Coffee,
  Go4_DarkRainBow,
  Go4_DarkTerrain,
  Go4_Fall,
  Go4_FruitPunch,
  Go4_Fuchsia,
  Go4_GreyYellow,
  Go4_GreenBrownTerrain,
  Go4_GreenPink,
  Go4_Island,
  Go4_Lake,
  Go4_LightTemperature,
  Go4_LightTerrain,
  Go4_Mint,
  Go4_Neon,
  Go4_Pastel,
  Go4_Pearl,
  Go4_Pigeon,
  Go4_Plum,
  Go4_RedBlue,
  Go4_Rose,
  Go4_Rust,
  Go4_SandyTerrain,
  Go4_Sienna,
  Go4_Solar,
  Go4_SouthWest,
  Go4_StarryNight,
  Go4_Sunset,
  Go4_TemperatureMap,
  Go4_Thermometer,
  Go4_Valentine,
  Go4_VisibleSpectrum,
  Go4_WaterMelon,
  Go4_Cool,
  Go4_Copper,
  Go4_GistEarth

};



class TGo4Style : public QWidget, public Ui::TGo4Style
 {
     Q_OBJECT

 public:

    TGo4Style( QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );

    /** Set default palette and minimum maximum indices */
    virtual void SetPaletteRange(int min, int def, int max);

 public slots:

   virtual void SetPadColor();

   virtual void SetPalette( int t );

   virtual void SetNamedPalette(int i);

 protected:
   /** Transform ROOT palette index in named palette combobox index */
   Go4_Palette_t CodePalette(int i);

   /** Transform combobox index to ROOT palette index*/
   int DecodePalette(Go4_Palette_t key);

   bool fbMenuLock;


 };

#endif
