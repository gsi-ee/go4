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

#include "TGo4Picture.h"


#include "TClass.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TFrame.h"
#include "TList.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"
#include "THStack.h"
#include "TStyle.h"
#include "TText.h"
#include "TDatime.h"
#include "TArrayL.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttMarker.h"
#include "TPaveStats.h"
#if ROOT_VERSION_CODE > ROOT_VERSION(5,9,0)
#include "TBufferXML.h"
#endif
enum OptionsIdentifiers {
   op_RangeXMin   = 1,
   op_RangeXMax   = 2,
   op_RangeYMin   = 3,
   op_RangeYMax   = 4,

   op_LineColor   = 5,
   op_LineStyle   = 6,
   op_LineWidth   = 7,

   op_FillColor   = 8,
   op_FillStyle   = 9,

   op_MarkerColor = 10,
   op_MarkerSize  = 11,
   op_MarkerStyle = 12,

   op_LogScaleX   = 13,
   op_LogScaleY   = 14,
   op_LogScaleZ   = 15,

   op_TitleTime   = 16,
   op_TitleDate   = 17,
   op_TitleItem   = 18,

   op_HisStatsX1  = 19,
   op_HisStatsY1  = 20,
   op_HisStatsX2  = 21,
   op_HisStatsY2  = 22,

//   op_PicHeader   = 23,

   op_HisStats    = 24,
   op_HisTitle    = 25,
   op_LegendDraw  = 26,
   op_Crosshair   = 27,
   op_AutoScale   = 28,
   op_Superimpose = 29,
   op_ContentModified = 30,
   op_PadModified = 31,

   op_RangeZMin   = 32,
   op_RangeZMax   = 33,

   op_FullDim     = 34,
   op_FullXMin    = 35,
   op_FullXMax    = 36,
   op_FullYMin    = 37,
   op_FullYMax    = 38,
   op_FullZMin    = 39,
   op_FullZMax    = 40,
   op_AxisX       = 41, // 12 parameters
   op_AxisY       = 53, // 12 parameters
   op_AxisZ       = 65, // 12 parameters
   op_Pad         = 77, // 8 parameters
   op_HisStatsOpt = 85,
   op_HisStatsFit = 86,
   op_RebinX      = 87,
   op_RebinY      = 88,

   op_TitleX1     = 89,
   op_TitleY1     = 90,
   op_TitleX2     = 91,
   op_TitleY2     = 92,
   op_TitleTextSz = 93,

   op_FrameLeft   = 95,
   op_FrameTop    = 96,
   op_FrameRight  = 97,
   op_FrameBottom = 98,

   op_TimeAxisX	  = 200,
   //op_TimeAxisXFmt= 201, // must be larger han op_ObjsBound for SetStrOption?
   	   	   	   	   	   	   // check reason for this later JAM

   op_XYRatioOne  = 300,  // JAM2016: 1:1 coordinate ratio
   op_DefaultRatio = 301,  // temporary used to reset ratio to default

   op_ObjsBound   = 0x4000,

   op_Style        = op_ObjsBound,
   op_Draw         = op_ObjsBound+1,
   op_HisStatsOptF = op_ObjsBound+2,
   op_HisStatsFitF = op_ObjsBound+3,
   op_TimeAxisXFmt= op_ObjsBound+4

};

TGo4Picture::TGo4Picture() :
   TNamed(),
   fiNDivX(1),
   fiNDivY(1),
   fiPosX(0),
   fiPosY(0),
   fxNames(0),
   fxObjects(0),
   fxSubPictures(0),
   fiOptSize(0),
   fxOptIndex(),
   fxOptValue(),
   fxOptObjects(0),
   fxSpecialObjects(0),
   fiLastIndex(0)
{
}

TGo4Picture::TGo4Picture(const char* name, const char* title, Int_t ndivy, Int_t ndivx) :
   TNamed(name, title),
   fiNDivX(1),
   fiNDivY(1),
   fiPosX(0),
   fiPosY(0),
   fxNames(0),
   fxObjects(0),
   fxSubPictures(0),
   fiOptSize(0),
   fxOptIndex(),
   fxOptValue(),
   fxOptObjects(0),
   fxSpecialObjects(0),
   fiLastIndex(0)
{
   SetDivision(ndivy, ndivx);
}

TGo4Picture::TGo4Picture(TGo4Picture& picture) :
   TNamed(),
   fiNDivX(1),
   fiNDivY(1),
   fiPosX(0),
   fiPosY(0),
   fxNames(0),
   fxObjects(0),
   fxSubPictures(0),
   fiOptSize(0),
   fxOptIndex(),
   fxOptValue(),
   fxOptObjects(0),
   fxSpecialObjects(0),
   fiLastIndex(0)
{
   UpdateFrom(&picture);
}

TGo4Picture::~TGo4Picture()
{
   if (fxNames) { delete fxNames; fxNames = 0; }
   if (fxObjects) { delete fxObjects; fxObjects = 0; }
   if (fxSubPictures) { delete fxSubPictures; fxSubPictures = 0; }
   if (fxOptObjects) { delete fxOptObjects; fxOptObjects = 0; }
   if (fxSpecialObjects) { delete fxSpecialObjects; fxSpecialObjects = 0; }
}

void TGo4Picture::SetDrawHeader(Bool_t draw)
{
   SetTitleTime(draw);
   SetTitleDate(draw);
}

Bool_t TGo4Picture::IsDrawHeader()
{
   return IsTitleTime() && IsTitleDate();
}

void TGo4Picture::SetDivision(Int_t ndivy, Int_t ndivx)
{
  if (ndivx<1) fiNDivX = 1; else fiNDivX = ndivx;
  if (ndivy<1) fiNDivY = 1; else fiNDivY = ndivy;
}

TGo4Picture* TGo4Picture::FindPic(Int_t posy, Int_t posx)
{
   if (fxSubPictures==0) return 0;
   if (posx<0) posx=0; else { if(posx>=GetDivX()) posx = GetDivX()-1; }
   if (posy<0) posy=0; else { if(posy>=GetDivY()) posy = GetDivY()-1; }
   for (Int_t n=0;n<=fxSubPictures->GetLast();n++) {
      TGo4Picture* sub = dynamic_cast<TGo4Picture*> (fxSubPictures->At(n));
      if (sub!=0)
         if (sub->CheckPosition(posy,posx)) return sub;
   }
   return 0;
}

TGo4Picture* TGo4Picture::Pic(Int_t posy, Int_t posx)
{
   if (!IsDivided()) return this;
   if (posx<0) posx=0; else if(posx>=GetDivX()) posx = GetDivX()-1;
   if (posy<0) posy=0; else if(posy>=GetDivY()) posy = GetDivY()-1;
   TGo4Picture* sub = FindPic(posy, posx);
   if (sub!=0) return sub;
   sub = new TGo4Picture("Sub", "Sub picture");
   sub->SetPosition(posy, posx);
   if (fxSubPictures==0) {
     fxSubPictures = new TObjArray();
     fxSubPictures->SetOwner(kTRUE);
   }
   fxSubPictures->Add(sub);
   return sub;
}

void TGo4Picture::SetLinesDivision(Int_t numlines, const Int_t* numbers)
{
   SetDivision(numlines,1);
   for (Int_t n=0;n<numlines;n++)
     if (numbers[n]>1) Pic(n,0)->SetDivision(1,numbers[n]);
}

void TGo4Picture::SetLinesDivision(Int_t numlines,
                                  Int_t n0, Int_t n1, Int_t n2, Int_t n3, Int_t n4,
                                  Int_t n5, Int_t n6, Int_t n7, Int_t n8, Int_t n9)
{
   const Int_t numbers[10] = { n0, n1, n2, n3, n4, n5, n6, n7, n8, n9};
   SetLinesDivision(numlines, numbers);
}

TGo4Picture* TGo4Picture::LPic(Int_t nline, Int_t ncol)
{
   if (!IsDivided()) return this;
   TGo4Picture* sub = Pic(nline, 0);
   if (sub==0) return 0;
   if (sub->IsDivided()) return sub->Pic(0,ncol);
                    else return sub;
}

void TGo4Picture::SetPosition(Int_t posy, Int_t posx)
{
  fiPosX = posx; fiPosY = posy;
}

Bool_t TGo4Picture::CheckPosition(Int_t posy, Int_t posx)
{
   return (fiPosX == posx) && (fiPosY == posy);
}

void TGo4Picture::AddH1(TH1* histo, Option_t* DrawOption)
{
   AddObject(histo, DrawOption);
}

void TGo4Picture::AddH1(Int_t posy, Int_t posx, TH1* histo, Option_t* DrawOption)
{
   AddObject(posy, posx, histo, DrawOption);
}

void TGo4Picture::AddHStack(THStack* st, Option_t* DrawOption)
{
   AddObject(st, DrawOption);
}

void TGo4Picture::AddHStack(Int_t posy, Int_t posx, THStack* st, Option_t* DrawOption)
{
    AddObject(posy, posx, st, DrawOption);
}

void TGo4Picture::AddGraph(TGraph* gr, Option_t* DrawOption)
{
   AddObject(gr, DrawOption);
}

void TGo4Picture::AddGraph(Int_t posy, Int_t posx, TGraph* gr, Option_t* DrawOption)
{
   AddObject(posy, posx, gr, DrawOption);
}

void TGo4Picture::AddCondition(TNamed* cond)
{
   AddObject(cond);
}

void TGo4Picture::AddCondition(Int_t posy, Int_t posx, TNamed* cond)
{
   AddObject(posy, posx, cond);
}

void TGo4Picture::AddObjName(const char* name, Option_t* DrawOption)
{
   if (name==0) return;
   if (fxNames==0) {
      fxNames = new TObjArray();
      fxNames->SetOwner(kTRUE);
   }
   fxNames->Add(new TObjString(name));
   SetDrawOption(DrawOption, fxNames->GetLast());
//   if ((DrawOption!=0) && (*DrawOption!=0))
//      SetDrawOption(DrawOption, PictureIndex);

   fiLastIndex = fxNames->GetLast();
}

void TGo4Picture::AddObjName(Int_t posy, Int_t posx, const char* name, Option_t* DrawOption)
{
   if (name!=0)
     Pic(posy,posx)->AddObjName(name, DrawOption);
}

void TGo4Picture::ClearObjNames(Bool_t recursive)
{
   if (fxNames!=0) {
     delete fxNames;
     fxNames = 0;
   }
   fiLastIndex = 0;
   if (recursive && fxSubPictures)
    for (Int_t n=0;n<=fxSubPictures->GetLast();n++) {
      TGo4Picture* sub = dynamic_cast<TGo4Picture*> (fxSubPictures->At(n));
      if (sub) sub->ClearObjNames(recursive);
    }
}

Int_t TGo4Picture::GetNumObjNames()
{
  return (fxNames==0) ? 0 : fxNames->GetLast()+1;
}

Bool_t TGo4Picture::IsObjNames()
{
   if (GetNumObjNames()>0) return kTRUE;

   if (fxSubPictures)
     for (Int_t n=0;n<=fxSubPictures->GetLast();n++) {
       TGo4Picture* sub = dynamic_cast<TGo4Picture*> (fxSubPictures->At(n));
       if (sub && sub->IsObjNames()) return kTRUE;
   }
   return kFALSE;
}

const char* TGo4Picture::GetObjName(Int_t n)
{
  if ((fxNames==0) || (n<0) || (n>fxNames->GetLast())) return 0;
  TObjString* str = dynamic_cast<TObjString*> (fxNames->At(n));
  if (str!=0) return str->String().Data();
         else return 0;
}

void TGo4Picture::AddObject(TObject* obj, Option_t* DrawOption)
{
   if (obj!=0)
      AddObjName(obj->GetName(), DrawOption);
}

void TGo4Picture::AddObject(Int_t posy, Int_t posx, TObject* obj, Option_t* DrawOption)
{
   if (obj!=0)
      AddObjName(posy, posx, obj->GetName(), DrawOption);
}

void TGo4Picture::SetRangeX(Double_t min, Double_t max)
{
  SetOptionD(PictureIndex, op_RangeXMin, min);
  SetOptionD(PictureIndex, op_RangeXMax, max);
}

Bool_t TGo4Picture::GetRangeX(Double_t& min, Double_t& max)
{
   min = 0.; max = 0;
   return GetOptionD(PictureIndex, op_RangeXMin, min) &&
          GetOptionD(PictureIndex, op_RangeXMax, max);
}

void TGo4Picture::ClearRangeX()
{
   ClearOption(PictureIndex, op_RangeXMin);
   ClearOption(PictureIndex, op_RangeXMax);
}

void TGo4Picture::SetRangeY(Double_t min, Double_t max)
{
  SetOptionD(PictureIndex, op_RangeYMin, min);
  SetOptionD(PictureIndex, op_RangeYMax, max);
}

Bool_t TGo4Picture::GetRangeY(Double_t& min, Double_t& max)
{
   min = 0.; max = 0;
   return GetOptionD(PictureIndex, op_RangeYMin, min) &&
          GetOptionD(PictureIndex, op_RangeYMax, max);
}

void TGo4Picture::ClearRangeY()
{
   ClearOption(PictureIndex, op_RangeYMin);
   ClearOption(PictureIndex, op_RangeYMax);
}

void TGo4Picture::SetRangeZ(Double_t min, Double_t max)
{
  SetOptionD(PictureIndex, op_RangeZMin, min);
  SetOptionD(PictureIndex, op_RangeZMax, max);
}

Bool_t TGo4Picture::GetRangeZ(Double_t& min, Double_t& max)
{
   min = 0.; max = 0;
   return GetOptionD(PictureIndex, op_RangeZMin, min) &&
          GetOptionD(PictureIndex, op_RangeZMax, max);
}

void TGo4Picture::ClearRangeZ()
{
   ClearOption(PictureIndex, op_RangeZMin);
   ClearOption(PictureIndex, op_RangeZMax);
}

void TGo4Picture::SetRange(Int_t naxis, Double_t min, Double_t max)
{
   switch (naxis) {
      case 0: SetRangeX(min, max); break;
      case 1: SetRangeY(min, max); break;
      case 2: SetRangeZ(min, max); break;
   }
}

Bool_t TGo4Picture::GetRange(Int_t naxis, Double_t& min, Double_t& max)
{
   switch (naxis) {
      case 0: return GetRangeX(min, max); break;
      case 1: return GetRangeY(min, max); break;
      case 2: return GetRangeZ(min, max); break;
   }
   return kFALSE;
}

void TGo4Picture::ClearRange(Int_t naxis)
{
   switch (naxis) {
      case 0: ClearRangeX(); break;
      case 1: ClearRangeY(); break;
      case 2: ClearRangeZ(); break;
      default:
         ClearRangeX();
         ClearRangeY();
         ClearRangeZ();
         break;
   }
}

void TGo4Picture::SetLogScale(Int_t nscale, Int_t zn)
{
   Int_t typ;
   switch (nscale) {
       case 1: typ = op_LogScaleY; break;
       case 2: typ = op_LogScaleZ; break;
       default: typ = op_LogScaleX;
   }
   SetOption(PictureIndex, typ, zn);
}

Int_t TGo4Picture::GetLogScale(Int_t nscale)
{
   Int_t typ;
   switch (nscale) {
       case 1: typ = op_LogScaleY; break;
       case 2: typ = op_LogScaleZ; break;
       default: typ = op_LogScaleX;
   }
   Long_t zn;
   if (GetOption(PictureIndex, typ, zn)) return zn;
                                    else return 0;
}

void TGo4Picture::GetLogScales(TVirtualPad* pad)
{
   if (pad == 0) return;
   Long_t zn;
   if (GetOption(PictureIndex, op_LogScaleX, zn)) pad->SetLogx(zn);
   if (GetOption(PictureIndex, op_LogScaleY, zn)) pad->SetLogy(zn);
   if (GetOption(PictureIndex, op_LogScaleZ, zn)) pad->SetLogz(zn);
}

void TGo4Picture::ClearLogScales()
{
   ClearOption(PictureIndex, op_LogScaleX);
   ClearOption(PictureIndex, op_LogScaleY);
   ClearOption(PictureIndex, op_LogScaleZ);
}

void TGo4Picture::SetLineAtt(Color_t color, Style_t style, Width_t width, Int_t index)
{
   CheckIndex(index);
   SetOption(index, op_LineColor, color);
   SetOption(index, op_LineStyle, style);
   SetOption(index, op_LineWidth, width);
}

void TGo4Picture::SetLineAtt(TAttLine* line, Int_t index)
{
   if (line==0) return;
   CheckIndex(index);
   SetOption(index, op_LineColor, line->GetLineColor());
   SetOption(index, op_LineStyle, line->GetLineStyle());
   SetOption(index, op_LineWidth, line->GetLineWidth());
}

Bool_t TGo4Picture::GetLineAtt(TAttLine* line, Int_t index)
{
   if (line==0) return kFALSE;
   CheckIndex(index);
   Long_t color = -111, style=-111, width=-111;
   if (GetOption(index, op_LineColor, color)) line->SetLineColor(color);
   if (GetOption(index, op_LineStyle, style)) line->SetLineStyle(style);
   if (GetOption(index, op_LineWidth, width)) line->SetLineWidth(width);
   return (color!=-111) && (style!=-111) && (width!=-111);
}

void TGo4Picture::ClearLineAtt(Int_t index)
{
   CheckIndex(index);
   ClearOption(index, op_LineColor);
   ClearOption(index, op_LineStyle);
   ClearOption(index, op_LineWidth);
}

void TGo4Picture::SetFillAtt(Color_t color, Style_t style, Int_t index)
{
   CheckIndex(index);
   SetOption(index, op_FillColor, color);
   SetOption(index, op_FillStyle, style);
}

void TGo4Picture::SetFillAtt(TAttFill* fill, Int_t index)
{
   if (fill==0) return;
   CheckIndex(index);
   SetOption(index, op_FillColor, fill->GetFillColor());
   SetOption(index, op_FillStyle, fill->GetFillStyle());
}

Bool_t TGo4Picture::GetFillAtt(TAttFill* fill, Int_t index)
{
   if (fill==0) return kFALSE;
   CheckIndex(index);
   Long_t color = -111, style = -111;
   if (GetOption(index, op_FillColor, color)) fill->SetFillColor(color);
   if (GetOption(index, op_FillStyle, style)) fill->SetFillStyle(style);
   return (color!=-111) && (style!=-111);
}

void TGo4Picture::ClearFillAtt(Int_t index)
{
   CheckIndex(index);
   ClearOption(index, op_FillColor);
   ClearOption(index, op_FillStyle);
}

void TGo4Picture::SetMarkerAtt(Color_t color, Size_t size, Style_t style, Int_t index)
{
   CheckIndex(index);
   SetOption(index, op_MarkerColor, color);
   SetOption(index, op_MarkerSize, int(size));
   SetOption(index, op_MarkerStyle, style);
}

void TGo4Picture::SetMarkerAtt(TAttMarker* marker, Int_t index)
{
   if (marker==0) return;
   CheckIndex(index);
   SetOption(index, op_MarkerColor, marker->GetMarkerColor());
   SetOption(index, op_MarkerSize, int(marker->GetMarkerSize()));
   SetOption(index, op_MarkerStyle, marker->GetMarkerStyle());
}

Bool_t TGo4Picture::GetMarkerAtt(TAttMarker* marker, Int_t index)
{
   if (marker==0) return kFALSE;
   CheckIndex(index);
   Long_t color=-111, size=-111, style=-111;
   if (GetOption(index, op_MarkerColor, color)) marker->SetMarkerColor(color);
   if (GetOption(index, op_MarkerSize, size)) marker->SetMarkerSize(size);
   if (GetOption(index, op_MarkerStyle, style)) marker->SetMarkerStyle(style);
   return (color!=-111) && (size!=-111) && (style!=-111);
}

void TGo4Picture::ClearMarkerAtt(Int_t index)
{
   CheckIndex(index);
   ClearOption(index, op_MarkerColor);
   ClearOption(index, op_MarkerSize);
   ClearOption(index, op_MarkerStyle);
}

void TGo4Picture::SetRebinX(Int_t ngroupx, Int_t index)
{
   CheckIndex(index);
   if (ngroupx>0) SetOption(index, op_RebinX, ngroupx);
             else ClearOption(index, op_RebinX);
}

void TGo4Picture::SetRebinY(Int_t ngroupy, Int_t index)
{
   CheckIndex(index);
   if (ngroupy>0) SetOption(index, op_RebinY, ngroupy);
             else ClearOption(index, op_RebinY);
}

Int_t TGo4Picture::GetRebinX(Int_t index)
{
   CheckIndex(index);
   Long_t ngroupx = 0;
   if (!GetOption(index, op_RebinX, ngroupx)) ngroupx = 0;
   return ngroupx;
}

Int_t TGo4Picture::GetRebinY(Int_t index)
{
   CheckIndex(index);
   Long_t ngroupy = 0;
   if (!GetOption(index, op_RebinY, ngroupy)) ngroupy = 0;
   return ngroupy;
}

void TGo4Picture::ClearRebin(Int_t index)
{
   CheckIndex(index);
   ClearOption(index, op_RebinX);
   ClearOption(index, op_RebinY);
}

void TGo4Picture::GetDrawAttributes(TObject* obj, Int_t index)
{
   if (obj==0) return;
   CheckIndex(index);
   GetLineAtt((TAttLine*) Cast(obj, TAttLine::Class()), index);
   GetFillAtt((TAttFill*) Cast(obj, TAttFill::Class()), index);
   GetMarkerAtt((TAttMarker*) Cast(obj, TAttMarker::Class()), index);
   GetH1Att((TH1*) Cast(obj, TH1::Class()), index);
   GetPadAtt((TPad*) Cast(obj, TPad::Class()), index);
}

void TGo4Picture::SetDrawAttributes(TObject* obj, Int_t index)
{
   if (obj==0) return;
   CheckIndex(index);
   SetLineAtt((TAttLine*) Cast(obj, TAttLine::Class()), index);
   SetFillAtt((TAttFill*) Cast(obj, TAttFill::Class()), index);
   SetMarkerAtt((TAttMarker*) Cast(obj, TAttMarker::Class()), index);
   SetH1Att((TH1*) Cast(obj, TH1::Class()), index);
   SetPadAtt((TPad*) Cast(obj, TPad::Class()), index);
}

void TGo4Picture::SetH1Att(TH1* h1, Int_t index)
{
   if (h1==0) return;
   CheckIndex(index);
   SetAxisAtt(0, h1->GetXaxis(), index);
   SetAxisAtt(1, h1->GetYaxis(), index);
   SetAxisAtt(2, h1->GetZaxis(), index);
}

void TGo4Picture::GetH1Att(TH1* h1, Int_t index)
{
   if (h1==0) return;
   CheckIndex(index);
   GetAxisAtt(0, h1->GetXaxis(), index);
   GetAxisAtt(1, h1->GetYaxis(), index);
   GetAxisAtt(2, h1->GetZaxis(), index);
}

void TGo4Picture::SetAxisLabelFontSize(Int_t naxis, Float_t LabelSize, Int_t index)
{
   if ((naxis<0) || (naxis>2)) return;
   CheckIndex(index);
   Int_t op = op_AxisX;
   if (naxis==1) op = op_AxisY; else
   if (naxis==2) op = op_AxisZ;

   SetOptionF(index, op+4, LabelSize);

}

void TGo4Picture::SetAxisTitleFontSize(Int_t naxis, Float_t TitleSize, Int_t index)
{
   if ((naxis<0) || (naxis>2)) return;
   CheckIndex(index);
   Int_t op = op_AxisX;
   if (naxis==1) op = op_AxisY; else
   if (naxis==2) op = op_AxisZ;

   SetOptionF(index, op+10, TitleSize);
}

void TGo4Picture::SetXAxisAttTime(Bool_t timedisplay, const char* format, Int_t index)
{
	 CheckIndex(index);
	 SetOption (index, op_TimeAxisX, timedisplay);
	 SetStrOption(index, op_TimeAxisXFmt, format);
}

void TGo4Picture::SetXAxisTimeDisplay(Bool_t on)
{
	SetOption (PictureIndex, op_TimeAxisX, on);
}

void TGo4Picture::SetXAxisTimeFormat(const char* format)
{
	 SetStrOption(PictureIndex, op_TimeAxisXFmt, format);
}

Bool_t  TGo4Picture::IsXAxisTimeDisplay()
{
	Long_t value=0;
	GetOption(PictureIndex, op_TimeAxisX,value);
	return value;
}

const char* TGo4Picture::GetXAxisTimeFormat()
{
	return GetStrOption(PictureIndex, op_TimeAxisXFmt , "%H:%M:%S");
}

void TGo4Picture::SetXYRatioOne(Bool_t on)
{
    SetOption (PictureIndex, op_XYRatioOne, on);
}

Bool_t  TGo4Picture::IsXYRatioOne()
{
    Long_t value=0;
    GetOption(PictureIndex, op_XYRatioOne, value);
    return value;
}

void TGo4Picture::SetDefaultRatio(Bool_t on)
{
   SetOption (PictureIndex, op_DefaultRatio, on);
}

Bool_t TGo4Picture::CheckDefaultRatio()
{
   Long_t value=0;
   GetOption(PictureIndex, op_DefaultRatio, value);
   return value;
}


void TGo4Picture::SetAxisAtt(Int_t naxis,
                      Color_t AxisColor,
                      Color_t LabelColor,
                      Style_t LabelFont,
                      Float_t LabelOffset,
                      Float_t LabelSize,
                      Int_t Ndivisions,
                      Float_t TickLength,
                      Color_t TitleColor,
                      Style_t TitleFont,
                      Float_t TitleOffset,
                      Float_t TitleSize,
                      Bool_t decimals,
                      const char* ticks,
                      Int_t bits,
                      Int_t index)
{
   if ((naxis<0) || (naxis>2)) return;
   CheckIndex(index);
   Int_t op = op_AxisX;
   if (naxis==1) op = op_AxisY; else
   if (naxis==2) op = op_AxisZ;

   SetOption (index, op+0, AxisColor);
   SetOption (index, op+1, LabelColor);
   SetOption (index, op+2, LabelFont);
   SetOptionF(index, op+3, LabelOffset);
   SetOptionF(index, op+4, LabelSize);
   SetOption (index, op+5, Ndivisions);
   SetOptionF(index, op+6, TickLength);
   SetOption (index, op+7, TitleColor);
   SetOption (index, op+8, TitleFont);
   SetOptionF(index, op+9, TitleOffset);
   SetOptionF(index, op+10, TitleSize);
   // copy all user defined bits
   if (decimals) bits = bits | 1;
   if (ticks!=0) {
      if (strchr(ticks,'+')!=0) bits = bits | 2;
      if (strchr(ticks,'-')!=0) bits = bits | 4;
   }
   SetOption (index, op+11, bits);
}

void TGo4Picture::SetAxisAtt(Int_t naxis, TAxis* axis, Int_t index)
{
   if (axis!=0)
      SetAxisAtt(naxis,
              axis->GetAxisColor(),
              axis->GetLabelColor(),
              axis->GetLabelFont(),
              axis->GetLabelOffset(),
              axis->GetLabelSize(),
              axis->GetNdivisions(),
              axis->GetTickLength(),
              axis->GetTitleColor(),
              axis->GetTitleFont(),
              axis->GetTitleOffset(),
              axis->GetTitleSize(),
              axis->GetDecimals(),
              axis->GetTicks(),
              axis->TestBits(0x0ff0),
              index);
   if(naxis==0)
   	   {
	   	   // support time format only for x axis for the moment
	   	   SetXAxisAttTime(axis->GetTimeDisplay(), axis->GetTimeFormat(), index);
   	   }
}

void TGo4Picture::GetAxisAtt(Int_t naxis, TAxis* axis, Int_t index)
{
   if ((axis==0) || (naxis<0) || (naxis>2)) return;
   CheckIndex(index);
   Int_t op = op_AxisX;
   if (naxis==1) op = op_AxisY; else
   if (naxis==2) op = op_AxisZ;

   Long_t lv;
   Float_t fv;
   if (GetOption (index, op+0, lv)) axis->SetAxisColor(lv);
   if (GetOption (index, op+1, lv)) axis->SetLabelColor(lv);
   if (GetOption (index, op+2, lv)) axis->SetLabelFont(lv);
   if (GetOptionF(index, op+3, fv)) axis->SetLabelOffset(fv);
   if (GetOptionF(index, op+4, fv)) axis->SetLabelSize(fv);
   if (GetOption (index, op+5, lv)) axis->SetNdivisions(lv);
   if (GetOptionF(index, op+6, fv)) axis->SetTickLength(fv);
   if (GetOption (index, op+7, lv)) axis->SetTitleColor(lv);
   if (GetOption (index, op+8, lv)) axis->SetTitleFont(lv);
   if (GetOptionF(index, op+9, fv)) axis->SetTitleOffset(fv);
   if (GetOptionF(index, op+10, fv)) axis->SetTitleSize(fv);
   if (GetOption (index, op+11, lv)) {
      axis->SetDecimals((lv & 1) != 0);
      TString ticks;
      if ((lv & 2) != 0) ticks+="+";
      if ((lv & 4) != 0) ticks+="-";
      axis->SetTicks(ticks.Data());
      for(int n=9;n<24;n++) {
         if (n==13) continue;
         Int_t mask = BIT(n);
         axis->SetBit(mask, (lv & mask) != 0);
      }
   }

   if(naxis==0)
   {
	   // time format x axis
	   if (GetOption (index, op_TimeAxisX, lv)) axis->SetTimeDisplay(lv);
	   axis->SetTimeFormat(GetStrOption(index, op_TimeAxisXFmt , "%H:%M:%S"));
   }

}

void TGo4Picture::SetPadAtt(Int_t BorderMode,
                            Int_t BorderSize,
                            Int_t Gridx,
                            Int_t Gridy,
                            Double_t Phi,
                            Double_t Theta,
                            Int_t Tickx,
                            Int_t Ticky,
                            Int_t index)
{
   CheckIndex(index);
   SetOption (index, op_Pad+ 0, BorderMode);
   SetOption (index, op_Pad+ 1, BorderSize);
   SetOption (index, op_Pad+ 2, Gridx);
   SetOption (index, op_Pad+ 3, Gridy);
   SetOptionD(index, op_Pad+ 4, Phi);
   SetOptionD(index, op_Pad+ 5, Theta);
   SetOption (index, op_Pad+ 6, Tickx);
   SetOption (index, op_Pad+ 7, Ticky);
}

void TGo4Picture::SetPadAtt(TPad* pad, Int_t index)
{
   if (pad==0) return;
   CheckIndex(index);
   SetPadAtt(pad->GetBorderMode(),
             pad->GetBorderSize(),
             pad->GetGridx(),
             pad->GetGridy(),
             pad->GetPhi(),
             pad->GetTheta(),
             pad->GetTickx(),
             pad->GetTicky(),
             index);
   SetDrawAttributes(pad->GetFrame(), index);
}

void TGo4Picture::GetPadAtt(TPad* pad, Int_t index)
{
   if (pad==0) return;
   CheckIndex(index);

   Long_t lv;
   Double_t dv;
   if (GetOption (index, op_Pad+ 0, lv)) pad->SetBorderMode(lv);
   if (GetOption (index, op_Pad+ 1, lv)) pad->SetBorderSize(lv);
   if (GetOption (index, op_Pad+ 2, lv)) pad->SetGridx(lv);
   if (GetOption (index, op_Pad+ 3, lv)) pad->SetGridy(lv);
   if (GetOptionD(index, op_Pad+ 4, dv)) pad->SetPhi(dv);
   if (GetOptionD(index, op_Pad+ 5, dv)) pad->SetTheta(dv);
   if (GetOption (index, op_Pad+ 6, lv)) pad->SetTickx(lv);
   if (GetOption (index, op_Pad+ 7, lv)) pad->SetTicky(lv);
   GetDrawAttributes(pad->GetFrame(), index);
}

void* TGo4Picture::Cast(TObject* obj, TClass* cl)
{
   if ((obj==0) || (cl==0)) return 0;
   Int_t shift = obj->IsA()->GetBaseClassOffset(cl);
   if (shift<0) return 0;
   return (char*) obj + shift;
}

void TGo4Picture::SetFrameAttr(Double_t left, Double_t top, Double_t right, Double_t bottom)
{
   SetOptionD(PictureIndex, op_FrameLeft, left);
   SetOptionD(PictureIndex, op_FrameTop, top);
   SetOptionD(PictureIndex, op_FrameRight, right);
   SetOptionD(PictureIndex, op_FrameBottom, bottom);
}

void TGo4Picture::SetFrameAttr(TPad* pad)
{
   if (pad==0) return;

   if ((TMath::Abs( pad->GetLeftMargin() - gStyle->GetPadLeftMargin()) > 0.001) ||
       (TMath::Abs( pad->GetTopMargin() - gStyle->GetPadTopMargin()) > 0.001) ||
       (TMath::Abs( pad->GetRightMargin() - gStyle->GetPadRightMargin()) > 0.001) ||
       (TMath::Abs( pad->GetBottomMargin() - gStyle->GetPadBottomMargin()) > 0.001))
       SetFrameAttr(pad->GetLeftMargin(), pad->GetTopMargin(), pad->GetRightMargin(), pad->GetBottomMargin());
}

Bool_t TGo4Picture::GetFrameAttr(TPad* pad)
{
   if (pad==0) return kFALSE;

   Double_t v;
   if (GetOptionD(PictureIndex, op_FrameLeft, v)) pad->SetLeftMargin(v);
   if (GetOptionD(PictureIndex, op_FrameTop, v)) pad->SetTopMargin(v);
   if (GetOptionD(PictureIndex, op_FrameRight, v)) pad->SetRightMargin(v);
   if (GetOptionD(PictureIndex, op_FrameBottom, v)) pad->SetBottomMargin(v);
   return kTRUE;
}

void TGo4Picture::SetHisStats(Bool_t on)
{
   SetOption(PictureIndex, op_HisStats, on ? 1 : 0);
}

Bool_t TGo4Picture::IsHisStats() const
{
   Long_t zn = kTRUE;
   GetOption(PictureIndex, op_HisStats, zn);
   return zn!=0;
}

void TGo4Picture::SetStatsAttr(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                               Int_t optstat, const char* statformat,
                               Int_t optfit, const char* fitformat)
{
   SetOptionD(PictureIndex, op_HisStatsX1, x1);
   SetOptionD(PictureIndex, op_HisStatsY1, y1);
   SetOptionD(PictureIndex, op_HisStatsX2, x2);
   SetOptionD(PictureIndex, op_HisStatsY2, y2);

   SetOption(PictureIndex, op_HisStatsOpt, optstat);
   SetStrOption(PictureIndex, op_HisStatsOptF, statformat);
   SetOption(PictureIndex, op_HisStatsFit, optfit);
   SetStrOption(PictureIndex, op_HisStatsFitF, fitformat);
}

void TGo4Picture::SetStatsAttr(TPaveStats* stats)
{
   if (stats!=0)
      SetStatsAttr(stats->GetX1NDC(), stats->GetY1NDC(),
                   stats->GetX2NDC(), stats->GetY2NDC(),
                   stats->GetOptStat(), stats->GetStatFormat(),
                   stats->GetOptFit(), stats->GetFitFormat());
}

Bool_t TGo4Picture::GetStatsAttr(TPaveStats* stats)
{
   if (stats==0) return kFALSE;

   Double_t x1, y1, x2, y2;
   if (GetOptionD(PictureIndex, op_HisStatsX1, x1) &&
       GetOptionD(PictureIndex, op_HisStatsY1, y1) &&
       GetOptionD(PictureIndex, op_HisStatsX2, x2) &&
       GetOptionD(PictureIndex, op_HisStatsY2, y2)) {
         stats->SetX1NDC(x1);
         stats->SetY1NDC(y1);
         stats->SetX2NDC(x2);
         stats->SetY2NDC(y2);
         stats->ConvertNDCtoPad();
       }

   Long_t optstat, optfit;
   if (GetOption(PictureIndex, op_HisStatsOpt, optstat))
     stats->SetOptStat(optstat);

   if (GetOption(PictureIndex, op_HisStatsFit, optfit))
     stats->SetOptFit(optfit);

   const char* fmtstat = GetStrOption(PictureIndex, op_HisStatsOptF);
   if (fmtstat!=0) stats->SetStatFormat(fmtstat);

   const char* fmtfit = GetStrOption(PictureIndex, op_HisStatsFitF);
   if (fmtfit!=0) stats->SetFitFormat(fmtfit);

   return kTRUE;
}

void TGo4Picture::SetHisTitle(bool on)
{
   SetOption(PictureIndex, op_HisTitle, on ? 1 : 0);
}

Bool_t TGo4Picture::IsHisTitle() const
{
   Long_t zn = kTRUE;
   GetOption(PictureIndex, op_HisTitle, zn);
   return zn!=0;
}

void TGo4Picture::SetTitleAttr(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t textsize)
{
   SetOptionD(PictureIndex, op_TitleX1, x1);
   SetOptionD(PictureIndex, op_TitleY1, y1);
   SetOptionD(PictureIndex, op_TitleX2, x2);
   SetOptionD(PictureIndex, op_TitleY2, y2);

   if (textsize!=0.)
      SetOptionD(PictureIndex, op_TitleTextSz, textsize);
}

void TGo4Picture::SetTitleAttr(TPaveText* titl)
{
   if (titl!=0) {
      SetTitleAttr(titl->GetX1NDC(), titl->GetY1NDC(),
                   titl->GetX2NDC(), titl->GetY2NDC(),
                   titl->GetTextSize());
   }
}

Bool_t TGo4Picture::HasTitleAttr()
{
   return (FindOptPos(PictureIndex, op_TitleX1)>=0) &&
          (FindOptPos(PictureIndex, op_TitleX2)>=0) &&
          (FindOptPos(PictureIndex, op_TitleY1)>=0) &&
          (FindOptPos(PictureIndex, op_TitleY2)>=0);
}


Bool_t TGo4Picture::GetTitleAttr(TPaveText* titl)
{
   if (titl==0) return kFALSE;

   Double_t x1, y1, x2, y2, sz(0.);
   if (GetOptionD(PictureIndex, op_TitleX1, x1) &&
       GetOptionD(PictureIndex, op_TitleY1, y1) &&
       GetOptionD(PictureIndex, op_TitleX2, x2) &&
       GetOptionD(PictureIndex, op_TitleY2, y2)) {
         titl->SetX1NDC(x1);
         titl->SetY1NDC(y1);
         titl->SetX2NDC(x2);
         titl->SetY2NDC(y2);
         titl->ConvertNDCtoPad();
       }

   if (GetOptionD(PictureIndex, op_TitleTextSz, sz))
      titl->SetTextSize(sz);
   else
      titl->SetTextSize(0);

   return kTRUE;
}


void TGo4Picture::SetTitleTime(Bool_t on)
{
   SetOption(PictureIndex, op_TitleTime, on ? 1 : 0);
}

Bool_t TGo4Picture::IsTitleTime() const
{
   Long_t zn = kTRUE;
   GetOption(PictureIndex, op_TitleTime, zn);
   return zn!=0;
}

void TGo4Picture::SetTitleDate(Bool_t on)
{
   SetOption(PictureIndex, op_TitleDate, on ? 1 : 0);
}

Bool_t TGo4Picture::IsTitleDate() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_TitleDate, zn);
   return zn!=0;
}

void TGo4Picture::SetTitleItem(Bool_t on)
{
   SetOption(PictureIndex, op_TitleItem, on ? 1 : 0);
}

Bool_t TGo4Picture::IsTitleItem() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_TitleItem, zn);
   return zn!=0;
}

void TGo4Picture::SetLegendDraw(bool on)
{
   SetOption(PictureIndex, op_LegendDraw, on ? 1 : 0);
}

bool TGo4Picture::IsLegendDraw() const
{
   Long_t zn = kTRUE;
   GetOption(PictureIndex, op_LegendDraw, zn);
   return zn!=0;
}

void TGo4Picture::SetCrosshair(bool on)
{
   SetOption(PictureIndex, op_Crosshair, on ? 1 : 0);
}

bool TGo4Picture::IsCrosshair() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_Crosshair, zn);
   return zn!=0;
}

void TGo4Picture::SetAutoScale(bool on)
{
   SetOption(PictureIndex, op_AutoScale, on ? 1 : 0);
}

bool TGo4Picture::IsAutoScale() const
{
   Long_t zn = kTRUE;
   GetOption(PictureIndex, op_AutoScale, zn);
   return zn!=0;
}

void TGo4Picture::SetSuperimpose(bool on)
{
   SetOption(PictureIndex, op_Superimpose, on ? 1 : 0);
}

bool TGo4Picture::IsSuperimpose() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_Superimpose, zn);
   return zn!=0;
}

void TGo4Picture::SetContentModified(bool on)
{
   SetOption(PictureIndex, op_ContentModified, on ? 1 : 0);
}

bool TGo4Picture::IsContentModified() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_ContentModified, zn);
   return zn!=0;
}

void TGo4Picture::SetPadModified(bool on)
{
   SetOption(PictureIndex, op_PadModified, on ? 1 : 0);
}

bool TGo4Picture::IsPadModified() const
{
   Long_t zn = kFALSE;
   GetOption(PictureIndex, op_PadModified, zn);
   return zn!=0;
}

void TGo4Picture::ChangeDrawOption(Int_t kind, Int_t value)
{
   switch(kind) {
      case 0: break;
      case 1: break;
      case 2: break;
      case 3: SetLogScale(0, value); break;
      case 4: SetLogScale(1, value); break;
      case 5: SetLogScale(2, value); break;
      case 6: SetHisStats(value!=0); break;
      case 7: SetHisTitle(value!=0); break;
      case 8: SetLegendDraw(value!=0); break;
      case 9: SetCrosshair(value!=0); break;
      case 10: {
         Int_t numdim = GetFullRangeDim();
         SetAutoScale(value!=0);
         Double_t min, max;
         if (!IsAutoScale()) { // keep current selection if not autosacle
            if ((numdim==1) && !GetRange(1, min, max)) {
               GetFullRange(1, min, max);
               SetRange(1, min, max);
            } else
            if ((numdim==2) && !GetRange(1, min, max)) {
               GetFullRange(2, min, max);
               SetRange(2, min, max);
            }
         }
         break;
      }
      case 11: SetSuperimpose(value!=0); break;
      case 12: SetTitleTime(value!=0); break;
      case 13: SetTitleDate(value!=0); break;
      case 14: SetTitleItem(value!=0); break;
      case 15: SetXAxisTimeDisplay(value!=0); break;
      case 17:
         SetXYRatioOne(value!=0);
         if (value==0) SetDefaultRatio(true); //
         break;

   }
   SetPadModified();
}


void TGo4Picture::SetFullRangeDim(Int_t ndim)
{
   SetOption(PictureIndex, op_FullDim, ndim);
}

Int_t TGo4Picture::GetFullRangeDim() const
{
   Long_t zn = 0;
   GetOption(PictureIndex, op_FullDim, zn);
   return zn;
}

void TGo4Picture::SetFullRange(Int_t naxis, Double_t min, Double_t max)
{
   Int_t op;
   switch (naxis) {
      case 1: op = op_FullYMin; break;
      case 2: op = op_FullZMin; break;
      default: op = op_FullXMin; break;
   }
   SetOptionD(PictureIndex, op, min);
   SetOptionD(PictureIndex, op+1, max);
}

Bool_t TGo4Picture::GetFullRange(Int_t naxis, Double_t& min, Double_t& max)
{
   Int_t op;
   switch (naxis) {
      case 1: op = op_FullYMin; break;
      case 2: op = op_FullZMin; break;
      default: op = op_FullXMin; break;
   }
   return GetOptionD(PictureIndex, op, min) && GetOptionD(PictureIndex, op+1, max);
}

void TGo4Picture::ClearFullRange(Int_t naxis)
{
   if (naxis<0) {
     for(int n=0;n<3;n++)
       ClearFullRange(n);
     ClearOption(PictureIndex, op_FullDim);
   }
   Int_t op;
   switch (naxis) {
      case 1: op = op_FullYMin; break;
      case 2: op = op_FullZMin; break;
      default: op = op_FullXMin; break;
   }
   ClearOption(PictureIndex, op);
   ClearOption(PictureIndex, op+1);
}

void TGo4Picture::UpdateFrom(TGo4Picture* source, TClass* selectedobjclass)
{
   if (source==0) return;
   Clear();

   SetName(source->GetName());
   SetTitle(source->GetTitle());

   fiNDivX = source->fiNDivX;
   fiNDivY = source->fiNDivY;
   fiPosX = source->fiPosX;
   fiPosY = source->fiPosY;

   for (Int_t n=0; n<source->GetNumObjNames();n++)
      AddObjName(source->GetObjName(n));

   if (source->GetSpecialObjects()!=0)
      fxSpecialObjects = (TList*) source->GetSpecialObjects()->Clone();

   CopyOptionsFrom(source);

   if (source->fxSubPictures!=0) {
     fxSubPictures = new TObjArray();
     fxSubPictures->SetOwner(kTRUE);
     for (Int_t n=0;n<=source->fxSubPictures->GetLast();n++) {
       TGo4Picture* sub = dynamic_cast<TGo4Picture*> (source->fxSubPictures->At(n));
       if (sub!=0) {
         TGo4Picture* newsub = new TGo4Picture;
         newsub->UpdateFrom(sub, selectedobjclass);
         fxSubPictures->Add(newsub);
       }
     }
   }
}

void TGo4Picture::CopyOptionsFrom(TGo4Picture* source)
{
   fiOptSize = source->fiOptSize;
   source->fxOptIndex.Copy(fxOptIndex);
   source->fxOptValue.Copy(fxOptValue);
   if (fxOptObjects!=0) delete fxOptObjects;
   fxOptObjects = 0;
   if (source->fxOptObjects)
      fxOptObjects = (TObjArray*) source->fxOptObjects->Clone();
}


void TGo4Picture::Clear(Option_t* option)
{
   fiNDivX = 1;
   fiNDivY = 1;
   fiPosX = 0;
   fiPosY = 0;
   if (fxNames!=0) { delete fxNames; fxNames = 0; }
   if (fxSubPictures!=0) { delete fxSubPictures; fxSubPictures = 0; }
   fiLastIndex = 0;
   fiOptSize = 0;
   fxOptIndex.Set(0);
   fxOptValue.Set(0);
   if (fxOptObjects!=0) { delete fxOptObjects; fxOptObjects = 0; }
}

Int_t TGo4Picture::FindOptPos(Short_t index, Short_t typ) const
{
   Long_t zn = index + 1 + (typ << 16);
   for (Int_t i=0;i<fiOptSize;i++)
     if (fxOptIndex[i]==zn) return i;
   return -1;
}

Int_t TGo4Picture::ExtendOptPos(Short_t index, Short_t typ)
{
   Long_t zn = index + 1 + (typ << 16);
   if (fxOptIndex.GetSize()==fiOptSize) {
     fxOptIndex.Set(fiOptSize+8);
     fxOptValue.Set(fiOptSize+8);
   }
   fxOptIndex[fiOptSize] = zn;
   fiOptSize++;
   return fiOptSize-1;
}

void TGo4Picture::SetOption(Short_t index, Short_t typ, Long_t value)
{
   if (typ>=op_ObjsBound) return;
   Int_t pos = FindOptPos(index,typ);
   if (pos<0) pos = ExtendOptPos(index, typ);
   fxOptValue[pos] = value;
}

Bool_t TGo4Picture::GetOption(Short_t index, Short_t typ, Long_t& value) const
{
   if (typ>=op_ObjsBound) return kFALSE;
   Int_t pos = FindOptPos(index, typ);
   if (pos<0) return kFALSE;
   value = fxOptValue[pos];
   return kTRUE;
}

Long_t TGo4Picture::GetI(Short_t index, Short_t typ, Long_t def) const
{
   Long_t value = 0;
   if (!GetOption(index, typ, value)) return def;
   return value;
}

void TGo4Picture::SetOptionF(Short_t index, Short_t typ, Float_t value)
{
   Long_t buf;
   if (sizeof(Long_t)==sizeof(Float_t))
      memcpy(&buf, &value, sizeof(Long_t));
   else {
      Double_t v(value);
      memcpy(&buf, &v, sizeof(Long_t));
   }
   SetOption(index, typ, buf);
}

Bool_t TGo4Picture::GetOptionF(Short_t index, Short_t typ, Float_t& value)
{
   Long_t buf;
   Bool_t res = GetOption(index, typ, buf);

   if (res){
      if (sizeof(Long_t)==sizeof(Float_t))
         memcpy(&value, &buf, sizeof(Long_t));
      else {
         Double_t v;
         memcpy(&v, &buf, sizeof(Long_t));
         value = v;
      }
   }
   return res;
}

void TGo4Picture::SetOptionD(Short_t index, Short_t typ, Double_t value)
{
   Long_t buf;
   if (sizeof(Long_t)==sizeof(Float_t))  {
      Float_t v(value);
      memcpy(&buf, &v, sizeof(Long_t));
   } else
      memcpy(&buf, &value, sizeof(Long_t));
   SetOption(index, typ, buf);
}

Bool_t TGo4Picture::GetOptionD(Short_t index, Short_t typ, Double_t& value)
{
   Long_t buf;
   Bool_t res = GetOption(index, typ, buf);

   if (res){
      if (sizeof(Long_t)==sizeof(Float_t)) {
         Float_t v;
         memcpy(&v, &buf, sizeof(Long_t));
         value = v;
      } else
         memcpy(&value, &buf, sizeof(Long_t));
   }
   return res;
}

Double_t TGo4Picture::GetD(Short_t index, Short_t typ, Double_t def)
{
   Double_t value;
   if (!GetOptionD(index, typ, value)) return def;
   return value;
}


void TGo4Picture::SetObjOption(Short_t index, Short_t typ, TObject* obj)
{
   if (obj==0) return;
   if (typ<op_ObjsBound) { delete obj; return; }
   Int_t pos = FindOptPos(index, typ);
   if (pos>=0) {
     TObject* old = fxOptObjects->RemoveAt(fxOptValue[pos]);
     delete old;
     fxOptObjects->AddAt(obj, fxOptValue[pos]);
   } else {
     pos = ExtendOptPos(index, typ);
     if (fxOptObjects==0) {
        fxOptObjects = new TObjArray();
        fxOptObjects->SetOwner(kTRUE);
     }
     fxOptObjects->Add(obj);
     fxOptValue[pos] = fxOptObjects->GetLast();
   }
}

TObject* TGo4Picture::GetObjOption(Short_t index, Short_t typ)
{
   if (typ<op_ObjsBound) return 0;
   Int_t pos = FindOptPos(index, typ);
   if (pos<0) return 0;
   return fxOptObjects->At(fxOptValue[pos]);
}

void TGo4Picture::SetStrOption(Short_t index, Short_t typ, const char* value)
{
  if (value==0) return;
  SetObjOption(index, typ, new TObjString(value));
}

const char* TGo4Picture::GetStrOption(Short_t index, Short_t typ, const char* defvalue)
{
  TObjString* ostr = dynamic_cast<TObjString*> (GetObjOption(index, typ));
  if (ostr==0) return defvalue;
  return ostr->String().Data();
}

void TGo4Picture::SetDrawOption(Option_t* option, Int_t index)
{
   CheckIndex(index);
   if (option==0) ClearOption(index, op_Draw);
             else SetStrOption(index, op_Draw, option);
}

Option_t* TGo4Picture::GetDrawOption(Int_t index)
{
   CheckIndex(index);
   return (Option_t*) GetStrOption(index, op_Draw);
}

void TGo4Picture::SetStyle(TStyle* style, Int_t index)
{
   if (style==0) return;
   CheckIndex(index);
   SetObjOption(index, op_Style, new TStyle(*style));
}

TStyle* TGo4Picture::FindStyle(Int_t index)
{
   CheckIndex(index);
   Int_t pos = FindOptPos(index, op_Style);
   if (pos<0) return 0;
   return dynamic_cast<TStyle*> (GetObjOption(index, op_Style));
}

TStyle* TGo4Picture::GetStyle(Int_t index)
{
   CheckIndex(index);
   TStyle* style = FindStyle(index);
   if (style==0) {
     style = new TStyle(*gStyle);
     SetObjOption(index, op_Style, style);
   }
   return style;
}

void TGo4Picture::ClearOption(Short_t index, Short_t typ)
{
  ClearOption(FindOptPos(index,typ));
}

void TGo4Picture::ClearOption(Int_t pos)
{
  if (pos<0) return;
  Int_t bnd = op_ObjsBound << 16;

  if (fxOptIndex[pos]>=bnd) {
    for (Int_t i=0;i<fiOptSize;i++)
      if (fxOptIndex[i]>=bnd)
        if (fxOptValue[i]>fxOptValue[pos]) fxOptValue[i]-=1;
    TObject* obj = fxOptObjects->RemoveAt(fxOptValue[pos]);
    delete obj;
    fxOptObjects->Compress();
    if (fxOptObjects->GetLast()<0) {
       delete fxOptObjects;
       fxOptObjects = 0;
    }
  }

  for (Int_t i=pos;i<fiOptSize-1;i++) {
    fxOptIndex[i] = fxOptIndex[i+1];
    fxOptValue[i] = fxOptValue[i+1];
  }
  fiOptSize--;
}

void TGo4Picture::CheckIndex(Int_t& index)
{
  if(index==UndefIndex){
    if (fiLastIndex>=PictureIndex) index = fiLastIndex;
                              else index = PictureIndex;
  }
}

void TGo4Picture::ClearAllOptions(Short_t index)
{
  if (index<0) {
     fxOptIndex.Set(0);
     fxOptValue.Set(0);
     if (fxOptObjects) {
       delete fxOptObjects;
       fxOptObjects = 0;
    }
  } else {
     Int_t pos = 0;
     while (pos<fiOptSize)
       if ((fxOptIndex[pos] & 0x7fff)==index+1) ClearOption(pos);
                                           else pos++;
  }
}

Int_t TGo4Picture::GetObjAttIndex(TObject* obj)
{
   if (obj==0) return UndefIndex;
   for (Int_t n=0;n<GetNumObjNames();n++)
     if (strcmp(GetObjName(n),obj->GetName())==0) return n;
   return UndefIndex;
}

void TGo4Picture::DrawPic(TVirtualPad* pad)
{
   if (pad==0) return;
   double txt_y = 0.9;
   if (IsDivided()) {
     pad->Divide(GetDivX(), GetDivY());
     for(Int_t posx=0; posx<GetDivX(); posx++)
       for(Int_t posy=0; posy<GetDivY(); posy++) {
           TGo4Picture* sub = FindPic(posy,posx);
           if (sub) sub->DrawPic(pad->GetPad(posy*GetDivX() + posx + 1));
       }
   } else
   for (Int_t indx=0;indx<GetNumObjNames();indx++) {
      TString str = "Obj: ";
      str += GetObjName(indx);
      const char* opt = GetDrawOption(indx);
      if ((opt==0) && (indx==0))
        opt = GetDrawOption(PictureIndex);
      if (opt!=0) {
         str += "  Opt: ";
         str += opt;
      }
      TText* txt = new TText(0.1, txt_y, str.Data());
      pad->cd();
      txt->Draw();
      if (GetNumObjNames()<8) txt_y -= 0.1;
                         else txt_y -= 0.8/GetNumObjNames();
   }
}

void TGo4Picture::Draw(Option_t* option)
{
   TCanvas* c = 0;
   if (gPad==0) {
      c = new TCanvas();
      c->cd();
   } else {
      c = gPad->GetCanvas();
      c->Clear();
      c->cd();
   }
   if (c==0) return;

   if (IsDrawHeader()) {
      TString txt = c->GetName();
      txt += "  Pic: ";
      txt += GetName();
      if (strcmp(GetName(), GetTitle())!=0) {
         txt += "  Title: ";
         txt += GetTitle();
      }
      c->SetTitle(txt.Data());
   }

   DrawPic(c);
}

void TGo4Picture::PrintPic(int shift, Bool_t showopt)
{
   for (int n=0;n<shift;n++) std::cout << ' ';
   if (IsDivided()) {
      std::cout << "Divided numx:" << GetDivX() << "  numy:" << GetDivY() << std::endl;
      for(Int_t posx=0; posx<GetDivX(); posx++)
         for(Int_t posy=0; posy<GetDivY(); posy++) {
            for (int n=0;n<shift+2;n++) std::cout << ' ';
            std::cout << "subpic x:" << posx << " y:" << posy << std::endl;
            TGo4Picture* sub = FindPic(posy,posx);
            if (sub) sub->PrintPic(shift+4, showopt);
         }
   } else {
      std::cout << "Objects:" << std::endl;
      for (Int_t num=0; num<GetNumObjNames(); num++) {
        const char* objname = GetObjName(num);
        for (int n=0;n<shift+2;n++) std::cout << ' ';
        std::cout << objname;

        Option_t* drawopt = GetDrawOption(num);
        if (drawopt) std::cout << "   opt: " << drawopt;
        std::cout << std::endl;
      }
      if (!showopt) return;
      for (int n=0;n<shift;n++) std::cout << ' ';
      std::cout << "Options:" << std::endl;
      for (Int_t nop=0;nop<fiOptSize;nop++) {
         Int_t indx = (fxOptIndex[nop] & 0x00ff);
         Int_t op = fxOptIndex[nop] >> 16;
         for (int n=0;n<shift+2;n++) std::cout << ' ';
         std::cout << indx << "  " << op << "  " << fxOptValue[nop] << std::endl;
      }
   }
}

void TGo4Picture::Print(Option_t* option) const
{
   std::cout << "Picture  " << GetName() << std::endl;
   ((TGo4Picture*)this)->PrintPic(2, (strstr(option,"attr")!=0));
}

void TGo4Picture::AddSubPicture(TGo4Picture* pic)
{
   if (pic == 0) return;
   if (fxSubPictures==0) {
      fxSubPictures = new TObjArray();
      fxSubPictures->SetOwner(kTRUE);
   }
   fxSubPictures->Add(pic);
}

void TGo4Picture::AddSpecialObject(TObject* obj, Option_t* drawopt)
{
   if (fxSpecialObjects==0) {
      fxSpecialObjects = new TList;
      fxSpecialObjects->SetOwner(kTRUE);
   }

   fxSpecialObjects->Add(obj, drawopt);
}

void TGo4Picture::AddSpecialObjectXml(const char* xmlcode, Option_t* drawopt)
{
   #if ROOT_VERSION_CODE > ROOT_VERSION(5,9,0)
   TObject* obj = TBufferXML::ConvertFromXML(xmlcode);
   if (obj!=0) AddSpecialObject(obj, drawopt);
   #endif
}

Long_t TGo4Picture::GetTotalSize()
{
   Long_t sz = IsA()->Size();

   if (fxNames) {
      sz += TObjArray::Class()->Size() + fxNames->GetSize() * sizeof(void*);
      for (int n=0;n<=fxNames->GetLast();n++) {
         TObjString* str = (TObjString*) fxNames->At(n);
         if (str) sz += str->IsA()->Size() + str->GetString().Length();
      }
   }

   if (fxSubPictures)
      sz += TObjArray::Class()->Size() + fxSubPictures->GetSize() * sizeof(void*);

   if (fxOptObjects) {
      sz += TObjArray::Class()->Size() + fxOptObjects->GetSize() * sizeof(void*);
      for (int n=0;n<=fxOptObjects->GetLast();n++) {
         TObject* obj = fxOptObjects->At(n);
         if (obj) sz += obj->IsA()->Size();
      }
   }

   if (fxSpecialObjects) {
      sz += TList::Class()->Size();
      TListIter iter(fxSpecialObjects);
      TObject* obj = 0;

      while ((obj = iter()) != 0)
         sz += sizeof(TObjLink) + obj->IsA()->Size();
   }

   sz += fxOptIndex.GetSize() * sizeof(Long_t);
   sz += fxOptValue.GetSize() * sizeof(Long_t);

   if (IsDivided())
      for(int ny=0;ny<GetDivY();ny++)
         for(int nx=0;nx<GetDivX();nx++)
            sz += Pic(ny,nx)->GetTotalSize();

   return sz;
}


#if ROOT_VERSION_CODE > ROOT_VERSION(5,11,6)
void TGo4Picture::SavePrimitive(std::ostream& fs, Option_t*)
#else
void TGo4Picture::SavePrimitive(std::ofstream& fs, Option_t*)
#endif
{
   fs << "TGo4Picture *" << GetName() << " = new TGo4Picture(\"" << GetName()
      << "\", \"" << GetTitle() << "\");" << std::endl;
   TString name = GetName();
   name+="->";

   fs << name << "SetCrosshair(" << (IsCrosshair() ? "true" : "false") << ");" << std::endl;

   MakeScript(fs, name.Data());
}

void TGo4Picture::MakeAxisScript(std::ostream& fs, const char* name, Int_t index, Int_t naxis)
{
   if ((naxis<0) || (naxis>2)) return;
   const char* axisname = "X";
   Int_t op = op_AxisX;
   if (naxis==1) { op = op_AxisY; axisname = "Y"; } else
   if (naxis==2) { op = op_AxisZ; axisname = "Z"; }

   Long_t lv;
   Float_t fv;
   if (!GetOption (index, op+0, lv)) return;

   fs << name << "SetAxisAtt(" << naxis << ", ";
   fs << lv << ", ";
   if (!GetOption (index, op+1, lv)) lv = gStyle->GetLabelColor(axisname);
   fs << lv << ", ";
   if (!GetOption (index, op+2, lv)) lv = gStyle->GetLabelFont(axisname);
   fs << lv << ", ";
   if (!GetOptionF(index, op+3, fv)) fv = gStyle->GetLabelOffset(axisname);
   fs << fv << ", ";
   if (!GetOptionF(index, op+4, fv)) fv = gStyle->GetLabelSize(axisname);
   fs << fv << ", ";
   if (!GetOption (index, op+5, lv)) lv = gStyle->GetNdivisions(axisname);
   fs << lv << ", ";
   if (!GetOptionF(index, op+6, fv)) fv = gStyle->GetTickLength(axisname);
   fs << fv << ", ";
   if (!GetOption (index, op+7, lv)) lv = gStyle->GetTitleColor(axisname);
   fs << lv << ", ";
   if (!GetOption (index, op+8, lv)) lv = gStyle->GetTitleFont(axisname);
   fs << lv << ", ";
   if (!GetOptionF(index, op+9, fv)) fv = gStyle->GetTitleOffset(axisname);
   fs << fv << ", ";
   if (!GetOptionF(index, op+10, fv)) fv = gStyle->GetTitleSize(axisname);
   fs << fv << ", ";

   if (GetOption (index, op+11, lv)) {
      if ((lv & 1) != 0) fs << "kTRUE, ";
                    else fs << "kFALSE, ";
      TString ticks;
      if ((lv & 2) != 0) ticks+="+";
      if ((lv & 4) != 0) ticks+="-";
      fs << "\"" << ticks <<"\", ";
      fs << (lv & 0x0ff0) << ", ";
   } else
      fs << "kTRUE, \"+\", 0, ";

   fs << index << ");" << std::endl;

   // TODO: add this to script
// note: take this attribute independent of displayed object
// this is necessary to correctly restore TGraph axis
//SetXAxisAttTime(axis->GetTimeDisplay(), axis->GetTimeFormat(), index);
//   if (naxis==0)
//   {
//	   Bool_t tdisp=kFALSE;
//	   if (GetOption (index, op_TimeAxisX, lv) && lv) tdisp=kTRUE;
//	   TString format=GetStrOption(index, op_TimeAxisXFmt , "%H:%M:%S");
//	   fs << name <<  "SetXAxisAttTime(";
//	   fs << tdisp << ", ";
//	   fs << "\"" << format.Data()<< "\"" <<", ";
//	   //fs << index << ");" << std::endl; // does not work?
//	   fs << PictureIndex << ");" << std::endl; // this works
//   }

}

void TGo4Picture::MakeScript(std::ostream& fs, const char* name)
{
   for(Int_t naxis=0;naxis<3;naxis++)
     if (GetLogScale(naxis)>0)
       fs << name << "SetLogScale(" << naxis << ", 1);" << std::endl;

   Double_t min, max;
   if (GetRangeX(min, max))
     fs << name << "SetRangeX(" << min << ", " << max << ");" << std::endl;
   if (GetRangeY(min, max))
     fs << name << "SetRangeY(" << min << ", " << max << ");" << std::endl;
   if (GetRangeZ(min, max))
     fs << name << "SetRangeZ(" << min << ", " << max << ");" << std::endl;

   Double_t v1,v2,v3,v4;
   if (GetOptionD(PictureIndex, op_FrameLeft, v1) &&
       GetOptionD(PictureIndex, op_FrameTop, v2) &&
       GetOptionD(PictureIndex, op_FrameRight, v3) &&
       GetOptionD(PictureIndex, op_FrameBottom, v4))
      fs << name << "SetFrameAttr(" << v1 << ", " << v2 << ", " << v3 << ", " << v4 << ");" << std::endl;


   TAttLine latt;
   TAttFill fatt;
   TAttMarker matt;

   if (GetLineAtt(&latt,PictureIndex))
     fs << name << "SetLineAtt("
        << latt.GetLineColor() << ", "
        << latt.GetLineStyle() << ", "
        << latt.GetLineWidth() << ", " << PictureIndex << ");" << std::endl;
   if (GetFillAtt(&fatt, PictureIndex))
      fs << name << "SetFillAtt("
         << fatt.GetFillColor() << ", "
         << fatt.GetFillStyle() << ", " << PictureIndex << ");" << std::endl;
   if (GetMarkerAtt(&matt, PictureIndex))
      fs << name << "SetMarkerAtt("
         << matt.GetMarkerColor() << ", "
         << matt.GetMarkerSize() << ", "
         << matt.GetMarkerStyle()<< ", " << PictureIndex << ");" << std::endl;

   fs << name << "SetHisStats(" << (IsHisStats() ? "true" : "false") << ");" << std::endl;

   if (IsHisStats() && (FindOptPos(PictureIndex, op_HisStatsX1) >= 0)) {
     fs << name << "SetStatsAttr("
        << GetD(PictureIndex, op_HisStatsX1, gStyle->GetStatX()-gStyle->GetStatW()) << ", "
        << GetD(PictureIndex, op_HisStatsY1, gStyle->GetStatY()-gStyle->GetStatH()) << ", "
        << GetD(PictureIndex, op_HisStatsX2, gStyle->GetStatX()) << ", "
        << GetD(PictureIndex, op_HisStatsY2, gStyle->GetStatY()) << ", "
        << GetI(PictureIndex, op_HisStatsOpt, 1111) << ", \""
        << GetStrOption(PictureIndex, op_HisStatsOptF, "6.4g") << "\", "
        << GetI(PictureIndex, op_HisStatsFit, 111) << ", \""
        << GetStrOption(PictureIndex, op_HisStatsFitF, "5.4g") << "\");" << std::endl;
   }

   fs << name << "SetHisTitle(" << (IsHisTitle() ? "true" : "false") << ");" << std::endl;
   if (HasTitleAttr()) {
     fs << name << "SetTitleAttr("
        << GetD(PictureIndex, op_TitleX1, gStyle->GetTitleX()-gStyle->GetTitleW()) << ", "
        << GetD(PictureIndex, op_TitleY1, gStyle->GetTitleY()-gStyle->GetTitleH()) << ", "
        << GetD(PictureIndex, op_TitleX2, gStyle->GetTitleX()) << ", "
        << GetD(PictureIndex, op_TitleY2, gStyle->GetTitleY());

     Double_t sz(0.);
     if (GetOptionD(PictureIndex, op_TitleTextSz, sz))
        fs << ", " << sz;

     fs << ");" << std::endl;

     fs << name << "SetTitleTime(" << (IsTitleTime() ? "true" : "false") << ");" << std::endl;
     fs << name << "SetTitleDate(" << (IsTitleDate() ? "true" : "false") << ");" << std::endl;
     fs << name << "SetTitleItem(" << (IsTitleItem() ? "true" : "false") << ");" << std::endl;
   }

   fs << name << "SetAutoScale(" << (IsAutoScale() ? "true" : "false") << ");" << std::endl;

   if (IsSuperimpose()) {
      fs << name << "SetSuperimpose(true);" << std::endl;
      fs << name << "SetLegendDraw(" << (IsLegendDraw() ? "true" : "false") << ");" << std::endl;
   }

   // JAM2016 add 1:1 coordinate ratio property:
   fs << name << "SetXYRatioOne(" << (IsXYRatioOne() ? "true" : "false") << ");" << std::endl;


   Long_t lv;
   Double_t dv;
   fs << name << "SetPadAtt(";
   if (!GetOption (PictureIndex, op_Pad+ 0, lv)) lv = gStyle->GetPadBorderMode();
   fs << lv << ", ";
   if (!GetOption (PictureIndex, op_Pad+ 1, lv)) lv = gStyle->GetPadBorderSize();
   fs << lv << ", ";
   if (!GetOption (PictureIndex, op_Pad+ 2, lv)) lv = gStyle->GetPadGridX();
   fs << lv << ", ";
   if (!GetOption (PictureIndex, op_Pad+ 3, lv)) lv = gStyle->GetPadGridY();
   fs << lv << ", ";
   if (!GetOptionD(PictureIndex, op_Pad+ 4, dv)) dv = 0.;
   fs << dv << ", ";
   if (!GetOptionD(PictureIndex, op_Pad+ 5, dv)) dv = 0.;
   fs << dv << ", ";
   if (!GetOption (PictureIndex, op_Pad+ 6, lv)) lv = gStyle->GetPadTickX();
   fs << lv << ", ";
   if (!GetOption (PictureIndex, op_Pad+ 7, lv)) lv = gStyle->GetPadTickY();
   fs << lv << ");" << std::endl;

   const char* drawopt = GetDrawOption(PictureIndex);
   if (drawopt!=0)
      fs << name << "SetDrawOption(\"" << drawopt << "\", " << PictureIndex << ");" << std::endl;

   // export x axis time attribute independent of objects. needed for TGraph pads
   Bool_t tdisp=kFALSE;
   if (GetOption (PictureIndex, op_TimeAxisX, lv) && lv) tdisp=kTRUE;
   TString format=GetStrOption(PictureIndex, op_TimeAxisXFmt , "%H:%M:%S");
   fs << name <<  "SetXAxisAttTime(";
   fs << tdisp << ", ";
   fs << "\"" << format.Data()<< "\"" <<", ";
   fs << PictureIndex << ");" << std::endl;


   for(Int_t indx=0;indx<GetNumObjNames();indx++) {
      const char* objname = GetObjName(indx);
      const char* objopt = GetDrawOption(indx);

      fs << name << "AddObjName(\"" << objname << "\"";
      if (objopt!=0) fs << ", \"" << objopt << "\"";
      fs << ");" << std::endl;
      if (GetLineAtt(&latt,indx))
        fs << name << "SetLineAtt("
           << latt.GetLineColor() << ", "
           << latt.GetLineStyle() << ", "
           << latt.GetLineWidth() << ", " << indx << ");" << std::endl;
      if (GetFillAtt(&fatt, indx))
         fs << name << "SetFillAtt("
            << fatt.GetFillColor() << ", "
            << fatt.GetFillStyle() << ", " << indx << ");" << std::endl;
      if (GetMarkerAtt(&matt, indx))
         fs << name << "SetMarkerAtt("
            << matt.GetMarkerColor() << ", "
            << matt.GetMarkerSize() << ", "
            << matt.GetMarkerStyle() << ", " << indx << ");" << std::endl;

      MakeAxisScript(fs, name, indx, 0);
      MakeAxisScript(fs, name, indx, 1);
      MakeAxisScript(fs, name, indx, 2);

      if (GetRebinX(indx)>0)
         fs << name << "SetRebinX(" << GetRebinX(indx) << ", " << indx << ");" << std::endl;

      if (GetRebinY(indx)>0)
         fs << name << "SetRebinY(" << GetRebinY(indx) << ", " << indx << ");" << std::endl;
   }

   #if ROOT_VERSION_CODE > ROOT_VERSION(5,9,0)
   TListIter iter(fxSpecialObjects);
   TObject* obj = 0;
   while ((obj=iter())!=0) {

      TString xmlbuf = TBufferXML::ConvertToXML(obj);

      TString buf = xmlbuf;
      Int_t len = 0;
      do {
        len = buf.Length();
        buf.ReplaceAll("\n ","\n");
      } while (len!=buf.Length());

      buf.ReplaceAll("\n"," ");
      buf.ReplaceAll("\"","\\\"");

      buf.Prepend("\"");
      buf.Append("\"");

      if (buf.Length()>950) {
         fs << "TString sbuf = \"\";" << std::endl;
         const char* pos = xmlbuf.Data();
         while (*pos!=0) {
            const char* lastpos = pos;
            while ((*pos !=0) && (*pos!='\n')) pos++;
            TString subbuf(lastpos, pos-lastpos);
            subbuf.ReplaceAll("\"","\\\"");
            fs << "TGo4Picture::Add(sbuf,\"" << subbuf << "\");" << std::endl;
            if (*pos==0) break;
            pos++;
         }
         buf = "sbuf";
      }

      fs << name << "AddSpecialObjectXml(" << buf;

      Option_t* opt = iter.GetOption();

      if ((opt!=0) && (*opt!=0))
         fs << ", \"" << opt << "\"";
      fs << ");" << std::endl;
   }
   #endif

   if (IsDivided()) {
      fs << name << "SetDivision(" << GetDivY() << ", " << GetDivX() << ");" << std::endl;
      for(int ny=0;ny<GetDivY();ny++)
        for(int nx=0;nx<GetDivX();nx++) {
           TString subname = name;
           subname+="Pic(";
           subname+=ny;
           subname+=",";
           subname+=nx;
           subname+=")->";
           Pic(ny,nx)->MakeScript(fs, subname.Data());
        }
   }
}


