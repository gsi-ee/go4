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

#include "TGo4Style.h"
#include "TColor.h"
#include <QColorDialog>
#include "TGo4MdiArea.h"
#include "TGo4ViewPanel.h"
#include "TGo4QSettings.h"
#include "TStyle.h"

//#include "Riostream.h"

TGo4Style::TGo4Style( QWidget* parent, const char* name, Qt::WindowFlags fl ) :
   QWidget( parent, fl ), fbMenuLock(false)
{
   setObjectName( name ? name : "Go4Style");
   setupUi(this);




   for(int i=GO4NAMEDPAL_MIN; i<=GO4NAMEDPAL_MAX+2; ++i) // need two indices more for Go4_None and Go4_Default
   {
     PaletteComboBox->addItem("dummy");
   }
   PaletteComboBox->setItemText(Go4_None,                    "unnamed palette");
   PaletteComboBox->setItemText(Go4_Default,                 "ClassicDefault");
   PaletteComboBox->setItemText(Go4_DeepSea,                 "DeepSea");
   PaletteComboBox->setItemText(Go4_GreyScale,               "GreyScale");
   PaletteComboBox->setItemText(Go4_DarkBodyRadiator,        "DarkBodyRadiator");
   PaletteComboBox->setItemText(Go4_BlueYellow,              "BlueYellow");
   PaletteComboBox->setItemText(Go4_RainBow,                 "RainBow");
   PaletteComboBox->setItemText(Go4_InvertedDarkBodyRadiator,"InvertedDarkBodyRadiator");
// below for ROOT 6 only. This is all handled by indices of init loop above
   PaletteComboBox->setItemText(Go4_Bird,                    "Bird");
   PaletteComboBox->setItemText(Go4_Cubehelix,               "Cubehelix");
   PaletteComboBox->setItemText(Go4_GreenRedViolet,          "GreenRedViolet");
   PaletteComboBox->setItemText(Go4_BlueRedYellow,           "BlueRedYellow");
   PaletteComboBox->setItemText(Go4_Ocean,                   "Ocean");
   PaletteComboBox->setItemText(Go4_ColorPrintableOnGrey,    "ColorPrintableOnGrey");
   PaletteComboBox->setItemText(Go4_Alpine,                  "Alpine");
   PaletteComboBox->setItemText(Go4_Aquamarine,              "Aquamarine");
   PaletteComboBox->setItemText(Go4_Army,                    "Army");
   PaletteComboBox->setItemText(Go4_Atlantic,                "Atlantic");
   PaletteComboBox->setItemText(Go4_Aurora,                  "Aurora");
   PaletteComboBox->setItemText(Go4_Avocado,                 "Avocado");
   PaletteComboBox->setItemText(Go4_Beach,                   "Beach");
   PaletteComboBox->setItemText(Go4_BlackBody,               "BlackBody");
   PaletteComboBox->setItemText(Go4_BlueGreenYellow,         "BlueGreenYellow");
   PaletteComboBox->setItemText(Go4_BrownCyan,               "BrownCyan");
   PaletteComboBox->setItemText(Go4_CMYK,                    "CMYK");
   PaletteComboBox->setItemText(Go4_Candy,                   "Candy");
   PaletteComboBox->setItemText(Go4_Cherry,                  "Cherry");
   PaletteComboBox->setItemText(Go4_Coffee,                  "Coffee");
   PaletteComboBox->setItemText(Go4_DarkRainBow,             "DarkRainBow");
   PaletteComboBox->setItemText(Go4_DarkTerrain,             "DarkTerrain");
   PaletteComboBox->setItemText(Go4_Fall,                    "Fall");
   PaletteComboBox->setItemText(Go4_FruitPunch,              "FruitPunch");
   PaletteComboBox->setItemText(Go4_Fuchsia,                 "Fuchsia");
   PaletteComboBox->setItemText(Go4_GreyYellow,              "GreyYellow");
   PaletteComboBox->setItemText(Go4_GreenBrownTerrain,       "GreenBrownTerrain");
   PaletteComboBox->setItemText(Go4_GreenPink,               "GreenPink");
   PaletteComboBox->setItemText(Go4_Island,                  "Island");
   PaletteComboBox->setItemText(Go4_Lake,                    "Lake");
   PaletteComboBox->setItemText(Go4_LightTemperature,        "LightTemperature");
   PaletteComboBox->setItemText(Go4_LightTerrain,            "LightTerrain");
   PaletteComboBox->setItemText(Go4_Mint,                    "Mint");
   PaletteComboBox->setItemText(Go4_Neon,                    "Neon");
   PaletteComboBox->setItemText(Go4_Pastel,                  "Pastel");
   PaletteComboBox->setItemText(Go4_Pearl,                   "Pearl");
   PaletteComboBox->setItemText(Go4_Pigeon,                  "Pigeon");
   PaletteComboBox->setItemText(Go4_Plum,                    "Plum");
   PaletteComboBox->setItemText(Go4_RedBlue,                 "RedBlue");
   PaletteComboBox->setItemText(Go4_Rose,                    "Rose");
   PaletteComboBox->setItemText(Go4_Rust,                    "Rust");
   PaletteComboBox->setItemText(Go4_SandyTerrain,            "SandyTerrain");
   PaletteComboBox->setItemText(Go4_Sienna,                  "Sienna");
   PaletteComboBox->setItemText(Go4_Solar,                   "Solar");
   PaletteComboBox->setItemText(Go4_SouthWest,               "SouthWest");
   PaletteComboBox->setItemText(Go4_StarryNight,             "StarryNight");
   PaletteComboBox->setItemText(Go4_Sunset,                  "Sunset");
   PaletteComboBox->setItemText(Go4_TemperatureMap,          "TemperatureMap");
   PaletteComboBox->setItemText(Go4_Thermometer,             "Thermometer");
   PaletteComboBox->setItemText(Go4_Valentine,               "Valentine");
   PaletteComboBox->setItemText(Go4_VisibleSpectrum,         "VisibleSpectrum");
   PaletteComboBox->setItemText(Go4_WaterMelon,              "WaterMelon");
   PaletteComboBox->setItemText(Go4_Cool,                    "Cool");
   PaletteComboBox->setItemText(Go4_Copper,                  "Copper");
   PaletteComboBox->setItemText(Go4_GistEarth,               "GistEarth");
   PaletteComboBox->setItemText(Go4_Viridis,                 "Virids");

   // here defaults from settings:
   int min,max,def;
   go4sett->getPaletteOpt(min,def,max);
   //std::cout<<"TGo4Style - Palette options are "<<min<<":"<<def<<":"<<max << std::endl;

   SetPaletteRange(min,def,max);


}


void TGo4Style::SetPaletteRange(int min, int def, int max)
{

  Palette->setMinimum(min);
  Palette->setMaximum(max);
  Palette->setValue(def);
  def=Palette->value(); // spinbox automatic limiting of range
  //gStyle->SetPalette(def,0,0);
  gStyle->SetPalette(def);
  SetPalette(def);
  RefreshPaletteText(min,max);
}

void TGo4Style::RefreshPaletteText(int min, int max)
{
  // mark unavailable entries in palette selection box:
  QString label;
  const QString notavailable="not avail:";
  for (int pt = 0; pt <= GO4NAMEDPAL_MAX + 2 - GO4NAMEDPAL_MIN; ++pt)    // need two indices more for Go4_None and Go4_Default
  {
    int ix = DecodePalette((Go4_Palette_t) pt);
    if (ix < 0)
      continue;
    label = PaletteComboBox->itemText(pt);
    label.remove(notavailable); // clear any old markers
    if ((ix < min) || (ix > max))
    {
      label.prepend(notavailable);
    }
    PaletteComboBox->setItemText(pt, label);
  }


}



void TGo4Style::SetPadColor()
{
   QColor c = QColorDialog::getColor();
   if (!c.isValid()) return;
   Int_t color = TColor::GetColor(c.red(), c.green(), c.blue());
   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(101, color, 0);
}

void TGo4Style::SetPalette(int t)
{
  //std::cout<<"TGo4Style::SetPalette "<<t << std::endl;
  TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
  if (panel != 0)
    panel->ChangeDrawOption(100, t, 0);
  if (!fbMenuLock) // try to avoid feedback of qt signals between widget slots
  {
    fbMenuLock = true;
    PaletteComboBox->setCurrentIndex(CodePalette(t));
    fbMenuLock = false;
  }
}


void TGo4Style::SetNamedPalette(int i)
{
  //std::cout<<"TGo4Style::SetNamedPalette "<<i  << std::endl;
  int ix=DecodePalette((Go4_Palette_t) i);
  if (ix<0) return;
  // range check of current preferences here:
  if(ix< Palette->minimum())
    {
      ix=Palette->minimum();
      PaletteComboBox->setCurrentIndex(CodePalette(ix)); // show correct palette name here.
    }
  if(ix> Palette->maximum())
     {
       ix=Palette->maximum();
       PaletteComboBox->setCurrentIndex(CodePalette(ix)); // show correct palette name here. 
     }
  if (!fbMenuLock)  // try to avoid feedback of qt signals between widget slots
  {
    fbMenuLock = true;
    Palette->setValue(ix);
    fbMenuLock = false;
  }
}

Go4_Palette_t TGo4Style::CodePalette(int i)
{
  Go4_Palette_t rev;
  if(i==1)
    rev=Go4_Default;
  else if(i<GO4NAMEDPAL_MIN || i > GO4NAMEDPAL_MAX)
    rev=Go4_None;
  else
    rev= (Go4_Palette_t) (i +2 -GO4NAMEDPAL_MIN); // account Go4_None and Go4_Default offset
  return rev;
}

int TGo4Style::DecodePalette(Go4_Palette_t key)
{
    if(key==Go4_None) return -1;
    if(key==Go4_Default) return 1; // old default rainbow index
    return (key-2 + GO4NAMEDPAL_MIN ); // account Go4_None and Go4_Default  offset
}


