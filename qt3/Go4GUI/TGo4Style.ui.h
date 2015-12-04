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


#include "RVersion.h"

/** Translate the ROOT palette styles to indices of dropdown menu here:
 * In simplest case this is just an offset by 51*/

/** minimum and maximum ROOT named palette types:*/
#define GO4NAMEDPAL_MIN 51

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,5,0)
#define GO4NAMEDPAL_MAX 112
#elif ROOT_VERSION_CODE >= ROOT_VERSION(6,0,2)
#define GO4NAMEDPAL_MAX 111
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
  Go4_GistEarth,
  Go4_Viridis
};



void TGo4Style::init()
{
 fbMenuLock=false;
 int min,max,def;
   go4sett->getPaletteOpt(min,def,max);
   //std::cout<<"TGo4Style - Palette options are "<<min<<":"<<def<<":"<<max << std::endl;

   SetPaletteRange(min,def,max);

   for(int i=GO4NAMEDPAL_MIN; i<=GO4NAMEDPAL_MAX+1; ++i) // need one index more for Go4_None entry
   {
     PaletteComboBox->insertItem("dummy");
   }
   PaletteComboBox->changeItem("unnamed palette" ,Go4_None);
   PaletteComboBox->changeItem("DeepSea", Go4_DeepSea);
   PaletteComboBox->changeItem("GreyScale", Go4_GreyScale);
   PaletteComboBox->changeItem("DarkBodyRadiator", Go4_DarkBodyRadiator);
   PaletteComboBox->changeItem("BlueYellow", Go4_BlueYellow);
   PaletteComboBox->changeItem("RainBow", Go4_RainBow);
   PaletteComboBox->changeItem("InvertedDarkBodyRadiator", Go4_InvertedDarkBodyRadiator);
// below for ROOT 6 only. This is all handled by indices of init loop above
   PaletteComboBox->changeItem("Bird",                  Go4_Bird);
   PaletteComboBox->changeItem("Cubehelix",             Go4_Cubehelix);
   PaletteComboBox->changeItem("GreenRedViolet",        Go4_GreenRedViolet);
   PaletteComboBox->changeItem("BlueRedYellow",         Go4_BlueRedYellow);
   PaletteComboBox->changeItem("Ocean",                 Go4_Ocean);
   PaletteComboBox->changeItem("ColorPrintableOnGrey",  Go4_ColorPrintableOnGrey);
   PaletteComboBox->changeItem("Alpine",                Go4_Alpine);
   PaletteComboBox->changeItem("Aquamarine",            Go4_Aquamarine);
   PaletteComboBox->changeItem("Army",                  Go4_Army);
   PaletteComboBox->changeItem("Atlantic",              Go4_Atlantic);
   PaletteComboBox->changeItem("Aurora",                Go4_Aurora);
   PaletteComboBox->changeItem("Avocado",               Go4_Avocado);
   PaletteComboBox->changeItem("Beach",                 Go4_Beach);           
   PaletteComboBox->changeItem("BlackBody",             Go4_BlackBody);       
   PaletteComboBox->changeItem("BlueGreenYellow",       Go4_BlueGreenYellow); 
   PaletteComboBox->changeItem("BrownCyan",             Go4_BrownCyan);       
   PaletteComboBox->changeItem("CMYK",                  Go4_CMYK);            
   PaletteComboBox->changeItem("Candy",                 Go4_Candy);           
   PaletteComboBox->changeItem("Cherry",                Go4_Cherry);          
   PaletteComboBox->changeItem("Coffee",                Go4_Coffee);          
   PaletteComboBox->changeItem("DarkRainBow",           Go4_DarkRainBow);     
   PaletteComboBox->changeItem("DarkTerrain",           Go4_DarkTerrain);     
   PaletteComboBox->changeItem("Fall",                  Go4_Fall);            
   PaletteComboBox->changeItem("FruitPunch",            Go4_FruitPunch);      
   PaletteComboBox->changeItem("Fuchsia",               Go4_Fuchsia);         
   PaletteComboBox->changeItem("GreyYellow",            Go4_GreyYellow);      
   PaletteComboBox->changeItem("GreenBrownTerrain",     Go4_GreenBrownTerrain);
   PaletteComboBox->changeItem("GreenPink",             Go4_GreenPink);       
   PaletteComboBox->changeItem("Island",                Go4_Island);          
   PaletteComboBox->changeItem("Lake",                  Go4_Lake);            
   PaletteComboBox->changeItem("LightTemperature",      Go4_LightTemperature);
   PaletteComboBox->changeItem("LightTerrain",          Go4_LightTerrain);    
   PaletteComboBox->changeItem("Mint",                  Go4_Mint);            
   PaletteComboBox->changeItem("Neon",                  Go4_Neon);            
   PaletteComboBox->changeItem("Pastel",                Go4_Pastel);          
   PaletteComboBox->changeItem("Pearl",                 Go4_Pearl);           
   PaletteComboBox->changeItem("Pigeon",                Go4_Pigeon);          
   PaletteComboBox->changeItem("Plum",                  Go4_Plum);            
   PaletteComboBox->changeItem("RedBlue",               Go4_RedBlue);         
   PaletteComboBox->changeItem("Rose",                  Go4_Rose);            
   PaletteComboBox->changeItem("Rust",                  Go4_Rust);            
   PaletteComboBox->changeItem("SandyTerrain",          Go4_SandyTerrain);    
   PaletteComboBox->changeItem("Sienna",                Go4_Sienna);          
   PaletteComboBox->changeItem("Solar",                 Go4_Solar);           
   PaletteComboBox->changeItem("SouthWest",             Go4_SouthWest);       
   PaletteComboBox->changeItem("StarryNight",           Go4_StarryNight);     
   PaletteComboBox->changeItem("Sunset",                Go4_Sunset);          
   PaletteComboBox->changeItem("TemperatureMap",        Go4_TemperatureMap);  
   PaletteComboBox->changeItem("Thermometer",           Go4_Thermometer);     
   PaletteComboBox->changeItem("Valentine",             Go4_Valentine);       
   PaletteComboBox->changeItem("VisibleSpectrum",       Go4_VisibleSpectrum); 
   PaletteComboBox->changeItem("WaterMelon",            Go4_WaterMelon);      
   PaletteComboBox->changeItem("Cool",                  Go4_Cool);            
   PaletteComboBox->changeItem("Copper",                Go4_Copper);          
   PaletteComboBox->changeItem("GistEarth",             Go4_GistEarth);       
   PaletteComboBox->changeItem("Virids",                Go4_Viridis);         

}


void TGo4Style::SetPaletteRange(int min, int def, int max)
{
    Palette->setMinValue(min);
    Palette->setValue(def);
    Palette->setMaxValue(max);
    gStyle->SetPalette(def,0,0);
}

void TGo4Style::SetPadColor()
{
   QColor c = QColorDialog::getColor();
   if (!c.isValid()) return;

   Int_t color = TColor::GetColor(c.red(), c.green(), c.blue());

   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(101, color, 0);
}

void TGo4Style::SetPalette( int t )
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(100, t, 0);
      
  if (!fbMenuLock) // try to avoid feedback of qt signals between widget slots
  {
    fbMenuLock = true;
    PaletteComboBox->setCurrentItem(CodePalette(t));
    fbMenuLock = false;
  }
      
}


void TGo4Style::SetNamedPalette(int i)
{
  //std::cout<<"TGo4Style::SetNamedPalette "<<i  << std::endl;
  int ix=DecodePalette((Go4_Palette_t) i);
  if (ix<0) return;
  if (!fbMenuLock)  // try to avoid feedback of qt signals between widget slots
  {
    fbMenuLock = true;
    Palette->setValue(ix);
    fbMenuLock = false;
  }
}

int TGo4Style::CodePalette(int i)
{
  Go4_Palette_t rev;
  if(i<GO4NAMEDPAL_MIN || i > GO4NAMEDPAL_MAX)
    rev=Go4_None;
  else
    rev= (Go4_Palette_t) (i +1 -GO4NAMEDPAL_MIN); // account Go4_None offset
  return rev;
}

int TGo4Style::DecodePalette(int key)
{
    if(key==Go4_None) return -1;
    return (key-1 + GO4NAMEDPAL_MIN ); // account Go4_None offset
}


