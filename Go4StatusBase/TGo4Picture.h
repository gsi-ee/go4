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

#ifndef TGO4PICTURE_H
#define TGO4PICTURE_H

#include "TNamed.h"
#include "TArrayL.h"
#include "TArrayF.h"
#include "Riostream.h"
#include "RVersion.h"
#include "Gtypes.h"

class TVirtualPad;
class TPad;
class TObjArray;
class TAttLine;
class TAttFill;
class TAttMarker;
class TStyle;
class TString;
class TAxis;
class TFrame;
class TH1;
class THStack;
class TGraph;
class TPaveStats;
class TPaveText;
class TList;

class TGo4Picture : public TNamed {
   public:
      enum Indexes { UndefIndex = -2, PictureIndex = -1 };

      TGo4Picture();
      TGo4Picture(const char* name, const char* title, Int_t ndivy = 1, Int_t ndivx = 1);
      virtual ~TGo4Picture();

      // ====== USER FUNCTIONS to setup picture =====================

      void SetDrawHeader(Bool_t draw = kTRUE);
      Bool_t IsDrawHeader();

      // division on subpictures

      void SetDivision(Int_t ndivy, Int_t ndivx);
      Int_t GetDivX() { return fiNDivX; }
      Int_t GetDivY() { return fiNDivY; }
      Bool_t IsDivided() { return (GetDivX()!=1) || (GetDivY()!=1); }

      TGo4Picture& operator()(int posy, int posx) { return *Pic(posy,posx); }
      TGo4Picture* Pic(Int_t posy, Int_t posx);
      TGo4Picture* FindPic(Int_t posy, Int_t posx);

      // GOOSY style of picture division

      void SetLinesDivision(Int_t numlines, const Int_t* numbers);
      void SetLinesDivision(Int_t numlines, Int_t n0=1, Int_t n1=1, Int_t n2=1, Int_t n3=1, Int_t n4=1,
                                            Int_t n5=1, Int_t n6=1, Int_t n7=1, Int_t n8=1, Int_t n9=1);
      TGo4Picture* LPic(Int_t nline, Int_t ncol);

      // add histogram (name) to picture or subpicture
      void AddH1(TH1* histo, Option_t* DrawOption = 0);
      void AddH1(Int_t posy, Int_t posx, TH1* histo, Option_t* DrawOption = 0);

      // add stack (name) to picture or subpicture
      void AddHStack(THStack* st, Option_t* DrawOption = 0);
      void AddHStack(Int_t posy, Int_t posx, THStack* st, Option_t* DrawOption = 0);

      // add TGraph object (name) to picture
      void AddGraph(TGraph* gr, Option_t* DrawOption = 0);
      void AddGraph(Int_t posy, Int_t posx, TGraph* gr, Option_t* DrawOption = 0);

      // add condition (name) to picture or subpicture
      void AddCondition(TNamed* cond = 0);
      void AddCondition(Int_t posy, Int_t posx, TNamed* cond = 0);

      // clear names of any objects (include histos and conditions) from subpicture (may be recursive)
      void ClearObjNames(Bool_t recursive = kFALSE);

      // sets viewable range for X and Y axis
      void SetRangeX(Double_t min, Double_t max);
      Bool_t GetRangeX(Double_t& min, Double_t& max);
      void ClearRangeX();
      void SetRangeY(Double_t min, Double_t max);
      Bool_t GetRangeY(Double_t& min, Double_t& max);
      void ClearRangeY();
      void SetRangeZ(Double_t min, Double_t max);
      Bool_t GetRangeZ(Double_t& min, Double_t& max);
      void ClearRangeZ();

      // set logarithmic scale for selected axis
      void SetLogScale(Int_t nscale = 0, Int_t zn = 1);
      Int_t GetLogScale(Int_t nscale = 0);
      void GetLogScales(TVirtualPad* pad);
      void ClearLogScales();

      // select index from object list, to which followinf options will be applied
      void SetSelectedIndex(Int_t index = PictureIndex) { fiLastIndex = index; }

      // sets line attributes
      void SetLineAtt(Color_t color, Style_t style, Width_t width, Int_t index = UndefIndex);
      void SetLineAtt(TAttLine* line, Int_t index = UndefIndex);
      Bool_t GetLineAtt(TAttLine* line, Int_t index = UndefIndex);
      void ClearLineAtt(Int_t index = UndefIndex);

      // sets fill attributes
      void SetFillAtt(Color_t color, Style_t style, Int_t index = UndefIndex);
      void SetFillAtt(TAttFill* fill, Int_t index = UndefIndex);
      Bool_t GetFillAtt(TAttFill* fill, Int_t index = UndefIndex);
      void ClearFillAtt(Int_t index = UndefIndex);

      // set marker attributes
      void SetMarkerAtt(Color_t color, Size_t size, Style_t style, Int_t index = UndefIndex);
      void SetMarkerAtt(TAttMarker* marker, Int_t index = UndefIndex);
      Bool_t GetMarkerAtt(TAttMarker* marker, Int_t index = UndefIndex);
      void ClearMarkerAtt(Int_t index = UndefIndex);

      // set rebin attributes
      void SetRebinX(Int_t ngroupx, Int_t index = UndefIndex);
      void SetRebinY(Int_t ngroupy, Int_t index = UndefIndex);
      Int_t GetRebinX(Int_t index = UndefIndex);
      Int_t GetRebinY(Int_t index = UndefIndex);
      void ClearRebin(Int_t index = UndefIndex);

      // copy all available attributes from object to picture
      void SetDrawAttributes(TObject* obj, Int_t index = UndefIndex);
      // assign all available attributes from picture to object
      void GetDrawAttributes(TObject* obj, Int_t index = UndefIndex);

      // set string draw options
      virtual void SetDrawOption(Option_t* option = "")
      {
         SetDrawOption(option, UndefIndex);
      }

      virtual void SetDrawOption(Option_t* option, Int_t index);

      virtual Option_t* GetDrawOption()
      {
         return GetDrawOption(UndefIndex);
      }

      virtual Option_t* GetDrawOption(Int_t index);

      // set draw style (Go4GUI index like)
      void SetHisStats(Bool_t on);
      Bool_t IsHisStats() const;

      void SetFrameAttr(Double_t left, Double_t top, Double_t right, Double_t bottom);
      void SetFrameAttr(TPad* pad);
      Bool_t GetFrameAttr(TPad* pad);

      void SetStatsAttr(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                        Int_t optstat = 1111, const char* statformat = "6.4g",
                        Int_t optfit = 1111, const char* fitformat = "5.4g");
      void SetStatsAttr(TPaveStats* stats);
      Bool_t GetStatsAttr(TPaveStats* stats);

      void SetHisContour(Int_t nlvl);
      Int_t GetHisContour() const;

      void SetHisTitle(bool on);
      Bool_t IsHisTitle() const;

      void SetTitleAttr(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t textsize = 0.);
      void SetTitleAttr(TPaveText* titl);
      Bool_t HasTitleAttr();
      Bool_t GetTitleAttr(TPaveText* titl);

      void SetTitleTime(Bool_t on = kTRUE);
      Bool_t IsTitleTime() const;

      void SetTitleDate(Bool_t on = kTRUE);
      Bool_t IsTitleDate() const;

      void SetTitleItem(Bool_t on = kTRUE);
      Bool_t IsTitleItem() const;

      void SetLegendDraw(bool on);
      bool IsLegendDraw() const;

      void SetCrosshair(bool on);
      bool IsCrosshair() const;

      void SetAutoScale(bool on);
      bool IsAutoScale() const;

      void SetSuperimpose(bool on);
      bool IsSuperimpose() const;

      void SetApplyToAll(bool on);
      bool IsApplyToAll() const;

      void SetAutoZoom(bool on);
      bool IsAutoZoom() const;

      virtual void Draw(Option_t* option = "");     //  *MENU*

      virtual void Clear(Option_t* option = "");    //  *MENU*

      virtual void Print(Option_t* option = "") const;    //  *MENU*

#ifndef __CINT__

      #if ROOT_VERSION_CODE > ROOT_VERSION(5,11,6)
      virtual void SavePrimitive(std::ostream& fs, Option_t* = "");
      #else
      virtual void SavePrimitive(std::ofstream& fs, Option_t* = "");
      #endif

#endif

      // ====== end of USER FUNCTIONS ========================

      // add name of any object, which should be drawn in picture or subpcture
      void AddObjName(const char* name, Option_t* DrawOption = 0);
      void AddObjName(Int_t posy, Int_t posx, const char* name, Option_t* DrawOption = 0);

      // adds only name of provided object
      void AddObject(TObject* obj, Option_t* DrawOption = 0);
      void AddObject(Int_t posy, Int_t posx, TObject* obj, Option_t* DrawOption = 0);

      void Reset() {}

      // move content from source picture
      void UpdateFrom(TGo4Picture* source, TClass* selectedobjclass = 0);
      void CopyOptionsFrom(TGo4Picture* source);

      Int_t GetNumObjNames();
      const char* GetObjName(Int_t n);
      Bool_t IsObjNames();

      // manipulation with special objects,
      // which must be drawn on the panel / canvas
      // This must be objects like markers, labels and so on
      void AddSpecialObject(TObject* obj, Option_t* drawopt = 0);
      void AddSpecialObjectXml(const char* xmlcode, Option_t* drawopt = 0);
      TList* GetSpecialObjects() const { return fxSpecialObjects; }

      // set style object
      void SetStyle(TStyle* style, Int_t index = UndefIndex);
      TStyle* FindStyle(Int_t index = UndefIndex);
      TStyle* GetStyle(Int_t index = UndefIndex);


      void SetAxisLabelFontSize(Int_t naxis, Float_t LabelSize, Int_t index = UndefIndex);
      void SetAxisTitleFontSize(Int_t naxis, Float_t TitleSize, Int_t index = UndefIndex);
      void SetAxisAtt(Int_t naxis,
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
                      Int_t index = UndefIndex);

      // for the moment, use separate function to define x axis time format JAM
      void SetXAxisAttTime(Bool_t timedisplay, const char* format, Int_t index = UndefIndex);

      Bool_t IsXAxisTimeDisplay();

      void SetXAxisTimeDisplay(Bool_t on);

      void SetXAxisTimeFormat(const char* format);

      const char* GetXAxisTimeFormat();

      void SetXYRatioOne(Bool_t on);
      Bool_t IsXYRatioOne();

//      void SetDefaultRatio(Bool_t on);
//      Bool_t CheckDefaultRatio();

      void SetPadAtt(Int_t BorderMode,
                     Int_t BorderSize,
                     Int_t Gridx,
                     Int_t Gridy,
                     Double_t Phi,
                     Double_t Theta,
                     Int_t Tickx,
                     Int_t Ticky,
                     Int_t index = PictureIndex);

      // fields, which are used in MainGUI to handle drawing

      void SetContentModified(bool on = true);
      bool IsContentModified() const;

      void SetPadModified(bool on = true);
      bool IsPadModified() const;

      void SetFullRangeDim(Int_t ndim);
      Int_t GetFullRangeDim() const;
      void SetFullRange(Int_t naxis, Double_t min, Double_t max);
      Bool_t GetFullRange(Int_t naxis, Double_t& min, Double_t& max);
      void ClearFullRange(Int_t naxis = -1);

      void SetRange(Int_t naxis, Double_t min, Double_t max);
      Bool_t GetRange(Int_t naxis, Double_t& min, Double_t& max);
      void ClearRange(Int_t naxis = -1);

      void ChangeDrawOption(Int_t kind, Int_t value);

      /** Returns total size, used by picture in memory */
      Long_t GetTotalSize();

      static void Add(TString& s, const char* buf) { s+=buf; }

   protected:
      TGo4Picture(TGo4Picture& picture);
      void AddSubPicture(TGo4Picture* pic);

      Int_t GetObjAttIndex(TObject* obj);

      void SetPosition(Int_t posy, Int_t posx);
      Bool_t CheckPosition(Int_t posy, Int_t posx);

      Int_t FindOptPos(Short_t index, Short_t typ) const;
      Int_t ExtendOptPos(Short_t index, Short_t typ);

      void SetOption(Short_t index, Short_t typ, Long_t value);
      Bool_t GetOption(Short_t index, Short_t typ, Long_t& value) const;
      Long_t GetI(Short_t index, Short_t typ, Long_t def = 0) const;

      void SetOptionF(Short_t index, Short_t typ, Float_t value);
      Bool_t GetOptionF(Short_t index, Short_t typ, Float_t& value);

      void SetOptionD(Short_t index, Short_t typ, Double_t value);
      Bool_t GetOptionD(Short_t index, Short_t typ, Double_t& value);
      Double_t GetD(Short_t index, Short_t typ, Double_t def = 0.);

      void SetObjOption(Short_t index, Short_t typ, TObject* obj);
      TObject* GetObjOption(Short_t index, Short_t typ);

      void SetStrOption(Short_t index, Short_t typ, const char* value);
      const char* GetStrOption(Short_t index, Short_t typ, const char* defvalue = 0);

      void ClearOption(Short_t index, Short_t typ);
      void ClearOption(Int_t pos);
      void ClearAllOptions(Short_t index = UndefIndex);

      void* Cast(TObject* obj, TClass* cl);

      void CheckIndex(Int_t& index);

      void DrawPic(TVirtualPad* pad);

      void PrintPic(int shift, Bool_t showopt);

      void SetH1Att(TH1* h1, Int_t index = UndefIndex);
      void GetH1Att(TH1* h1, Int_t index = UndefIndex);
      void SetAxisAtt(Int_t naxis, TAxis* axis, Int_t index = UndefIndex);
      void GetAxisAtt(Int_t naxis, TAxis* axis, Int_t index = UndefIndex);
      void SetPadAtt(TPad* pad, Int_t index = UndefIndex);
      void GetPadAtt(TPad* pad, Int_t index = UndefIndex);

      void MakeScript(std::ostream& fs, const char* name);

      void MakeAxisScript(std::ostream& fs, const char* name, Int_t indx, Int_t naxis);

   private:
      Int_t fiNDivX;
      Int_t fiNDivY;

      Int_t fiPosX;
      Int_t fiPosY;

      TObjArray* fxNames;

      TObjArray* fxObjects;

      TObjArray* fxSubPictures;

      Int_t fiOptSize;
      TArrayL fxOptIndex;
      TArrayL fxOptValue;
      TObjArray* fxOptObjects;

      TList* fxSpecialObjects;

      Int_t fiLastIndex;        //!

   ClassDef(TGo4Picture, 3)
};

#endif //TGO4PICTURE_H


