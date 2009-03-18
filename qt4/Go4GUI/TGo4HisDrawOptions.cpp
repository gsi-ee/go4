#include "TGo4HisDrawOptions.h"

//#include <QLineEdit>
#include "Riostream.h"
#include "TROOT.h"
#include "TColor.h" 
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttMarker.h"
#include "TGo4Picture.h"
#include "TGo4ViewPanel.h"
#include "TGo4WorkSpace.h"
#include <QColorDialog>

TGo4HisDrawOptions::TGo4HisDrawOptions( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{ 
	setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
   fbSettingPanelData = true;

   connect(TGo4WorkSpace::Instance(), SIGNAL(panelSignal(TGo4ViewPanel*, TPad*, int)),
           this, SLOT(panelSlot(TGo4ViewPanel*, TPad*, int)));

   UpdateView(view_Histo1);

   AutoScaleBox->setShown(false);

   LineColor->setEnabled(false);
   FillColor->setEnabled(false);
   MarkerColor->setEnabled(false);

   fbSettingPanelData = false;
}

void TGo4HisDrawOptions::panelSlot(TGo4ViewPanel* panel, TPad* pad, int signalid)
{
   switch (signalid) {
      case QGo4Widget::panel_Modified:
      case QGo4Widget::panel_Updated:
         break;
      case QGo4Widget::panel_Activated:
      case QGo4Widget::panel_ActiveUpdated: {
         TGo4Picture* padopt = panel->GetPadOptions(pad);
         if (padopt==0) break;

         int ndim = padopt->GetFullRangeDim();
         if (ndim<=0) ndim = 1;

         const char* drawopt = 0;
         TObject* obj = panel->GetSelectedObject(pad, &drawopt);

         int viewtype = view_Histo1;

         TString buf(drawopt);
         buf.ToLower();

         if ((obj!=0) && (obj->InheritsFrom("TGraph") || obj->InheritsFrom("TMultiGraph")))
            viewtype = view_Graph;
         else
         if (ndim==1)
            viewtype = view_Histo1;
         else
         if (buf.Contains("lego") || buf.Contains("surf"))
            viewtype = view_Histo2ext;
         else
            viewtype = view_Histo2;

         fbSettingPanelData = true;

         if (fiLastView!=viewtype) UpdateView(viewtype);

         int ErrorStyle = 0, CoordStyle = 0, DrawStyle = 0;
         DecodeDrawOption(drawopt, ErrorStyle, CoordStyle, DrawStyle);

         DrawOption->setCurrentItem(DrawStyle);
         ErrorBars->setCurrentItem(ErrorStyle);
         Coordinates->setCurrentItem(CoordStyle);
         XStyle->setCurrentItem(padopt->GetLogScale(0));
         YStyle->setCurrentItem(padopt->GetLogScale(1));
         ZStyle->setCurrentItem(padopt->GetLogScale(2));
         ZStyle->setEnabled(ndim>1);
         AutoScaleBox->setChecked(padopt->IsAutoScale());

         LineColor->setEnabled(dynamic_cast<TAttLine*>(obj)!=0);
         FillColor->setEnabled(dynamic_cast<TAttFill*>(obj)!=0);
         MarkerColor->setEnabled(dynamic_cast<TAttMarker*>(obj)!=0);

         fbSettingPanelData = false;

         break; }
      default:
         break;
   }
}

void TGo4HisDrawOptions::UpdateView(int viewtype)
{
   int lines=30;
   ErrorBars->clear();
   DrawOption->clear();
   Coordinates->clear();

   switch(viewtype) {
      case view_Histo1:
         DrawOption->insertItem(QPixmap(":/icons/h1_t.png"),   " scatter",         0);
         DrawOption->insertItem(QPixmap(""),           "AH   no axis",     1);
         DrawOption->insertItem(QPixmap(""),           "*H   stars",       2);
         DrawOption->insertItem(QPixmap(""),           "L    lines",       3);
         DrawOption->insertItem(QPixmap(""),           "LF2  lines+fill",  4);
         DrawOption->insertItem(QPixmap(""),           "C    curve",       5);
         DrawOption->insertItem(QPixmap(""),           "B    barchart",    6);
         DrawOption->insertItem(QPixmap(""),           "P    polymarkers", 7);
         DrawOption->insertItem(QPixmap(""),           "P0   polymarkers", 8);
         DrawOption->insertItem(QPixmap(""),           "9    high resol",  9);
         DrawOption->insertItem(QPixmap(""),           "][   no right",   10);
         DrawOption->insertItem(QPixmap(""),           "TEXT digits b/w", 11);
         DrawOption->insertItem(QPixmap(""),           "BAR  barchart",   12);
         DrawOption->insertItem(QPixmap(":/icons/lego.png"),  " lego  b/w",      13);
         DrawOption->insertItem(QPixmap(":/icons/lego1.png"), " lego1 shadow",   14);
         DrawOption->insertItem(QPixmap(":/icons/lego2.png"), " lego2 color",    15);
         DrawOption->insertItem(QPixmap(":/icons/surf.png"),  " mesh b/w",       16);
         DrawOption->insertItem(QPixmap(":/icons/surf1.png"), " mesh color",     17);
         DrawOption->insertItem(QPixmap(":/icons/surf2.png"), " surf c",         18);
         DrawOption->insertItem(QPixmap(":/icons/surf3.png"), " mesh+contour",   19);
         DrawOption->insertItem(QPixmap(":/icons/surf4.png"), " gourand",        20);
         DrawOption->insertItem(QPixmap(":/icons/surf5.png"), " col contour",    21);
         DrawOption->insertItem(QPixmap(""), "",    22);
         lines=30;
         break;
      case view_Histo2:
      case view_Histo2ext:
         DrawOption->insertItem(QPixmap(":/icons/h1_t.png"), " scatter",       0);
         DrawOption->insertItem(QPixmap(":/icons/col.png"),  " pixel c",       1);
         DrawOption->insertItem(QPixmap(":/icons/cont0.png")," cont0 c",       2);
         DrawOption->insertItem(QPixmap(":/icons/lego2.png")," lego2 color",   3);
         DrawOption->insertItem(QPixmap(":/icons/surf2.png")," surf c",        4);
         DrawOption->insertItem(QPixmap(":/icons/surf1.png")," mesh color",    5);
         DrawOption->insertItem(QPixmap(":/icons/cont1.png")," cont1 c",       6);
         DrawOption->insertItem(QPixmap(":/icons/cont1.png")," cont4",         7);
         DrawOption->insertItem(QPixmap(":/icons/lego1.png")," lego1 shadow",  8);
         DrawOption->insertItem(QPixmap(":/icons/lego.png"), " lego b/w",      9);
         DrawOption->insertItem(QPixmap(":/icons/cont2.png")," cont2 dot b/w",10);
         DrawOption->insertItem(QPixmap(":/icons/cont3.png")," cont3 b/w",    11);
         DrawOption->insertItem(QPixmap(":/icons/surf.png"), " mesh b/w",     12);
         DrawOption->insertItem(QPixmap(":/icons/surf3.png")," mesh+contour", 13);
         DrawOption->insertItem(QPixmap(":/icons/surf4.png")," gourand",      14);
         DrawOption->insertItem(QPixmap(":/icons/surf5.png")," col contour",  15);
         DrawOption->insertItem(QPixmap(""),         "ARR  arrow mode",16);
         DrawOption->insertItem(QPixmap(""),         "BOX  boxes",   17);
         DrawOption->insertItem(QPixmap(""),         "TEXT content", 18);
         lines=30;
#ifdef __GO4ASI__
         DrawOption->insertItem(QPixmap(":/icons/asimage.png"), " ASImage",   19);
         DrawOption->insertItem(QPixmap(""), "",    20);
#else
         DrawOption->insertItem(QPixmap(""), "",    19);
#endif
         break;
      case view_Graph:
         DrawOption->insertItem("P: default");
         DrawOption->insertItem("*: stars");
         DrawOption->insertItem("L: line");
         DrawOption->insertItem("F: fill");
         DrawOption->insertItem("F1: fill 1");
         DrawOption->insertItem("F2: fill 2");
         DrawOption->insertItem("C: smooth");
         DrawOption->insertItem("B: bar");
         DrawOption->insertItem("LP: line + mark");
         DrawOption->insertItem("L*: line + *");
         DrawOption->insertItem("FP: fill + mark");
         DrawOption->insertItem("F*: fill + *");
         DrawOption->insertItem("CP: smooth + mark");
         DrawOption->insertItem("C*: smooth + *");
         DrawOption->insertItem("BP: smooth + mark");
         DrawOption->insertItem("B*: smooth + *");
         lines=30;
         break;
   }
	/////// TODO: port this 
	//DrawOption->setSizeLimit(lines);

   switch(viewtype) {
      case view_Histo1:
         ErrorBars->insertItem("No Errors");
         ErrorBars->insertItem("E: simple");
         ErrorBars->insertItem("E1: edges");
         ErrorBars->insertItem("E2: rectangles");
         ErrorBars->insertItem("E3: fill");
         ErrorBars->insertItem("E4: contour");
         break;
      case view_Histo2:
         ErrorBars->insertItem("No palette");
         ErrorBars->insertItem(QPixmap(":/icons/colz.png")," +scale",1);
         break;
      case view_Histo2ext:
         ErrorBars->insertItem("No palette");
         ErrorBars->insertItem(QPixmap(":/icons/colz.png")," +scale",1);
         ErrorBars->insertItem("    - front",2);
         ErrorBars->insertItem("    - back",3);
         ErrorBars->insertItem("    - fr & bk",4);
         ErrorBars->insertItem(QPixmap(":/icons/colz.png")," scale - fr",5);
         ErrorBars->insertItem(QPixmap(":/icons/colz.png"), " scale - bk",6);
         ErrorBars->insertItem(QPixmap(":/icons/colz.png"), " scale - fr & bk",7);
         break;
      case view_Graph:
         ErrorBars->insertItem("errors as is");
         ErrorBars->insertItem("X: no errors");
         ErrorBars->insertItem(">: arrow");
         ErrorBars->insertItem("|>: full arrow");
         ErrorBars->insertItem("2: err opt 2");
         ErrorBars->insertItem("3: err opt 3");
         ErrorBars->insertItem("4: err opt 4");
         ErrorBars->insertItem("[]: asym err");
         break;
   }

   switch(viewtype) {
      case view_Histo1:
      case view_Histo2:
      case view_Histo2ext:
         Coordinates->insertItem("Cartesian");
         Coordinates->insertItem("Polar");
         Coordinates->insertItem("Spheric");
         Coordinates->insertItem("Rapidity");
         Coordinates->insertItem("Cylindric");
         break;
      case view_Graph:
         Coordinates->insertItem("A: norm axis");
         Coordinates->insertItem("supp. axis");
         Coordinates->insertItem("AX+: top");
         Coordinates->insertItem("AY+: right");
         Coordinates->insertItem("AX+Y+: x & y");
         Coordinates->insertItem("A1: ylow = ymin");
         break;
   }

   fiLastView = viewtype;
   //cout << "Set Viewtype " << fiLastView << endl;
}

void TGo4HisDrawOptions::ChangeDrawOptionForCurrentPanel(int kind, int value)
{
   if (fbSettingPanelData) return;

   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel==0) return;

   TPad* pad = panel->GetActivePad();
//   TGo4Picture* padopt = panel->GetPadOptions(pad);
//   if (padopt==0) return;

   QString buf;
   const char* drawopt = 0;
   const char* pp;

   //cout << "    Viewtype " << fiLastView << endl;
   if ((kind>=0) && (kind<=2)) {
      CodeDrawOptions(ErrorBars->currentItem(),
                      Coordinates->currentItem(),
                      DrawOption->currentItem(), buf);
      if (buf.length()>0) drawopt = buf.latin1();

      //if(drawopt) cout << ">> " << value << " " << drawopt << endl;
      //else cout << "No options" << endl;
   }
   panel->ChangeDrawOption(kind, value, drawopt);
}

void TGo4HisDrawOptions::SetDrawOptions( int t )
{
   ChangeDrawOptionForCurrentPanel(0, t);
}

void TGo4HisDrawOptions::SetErrorBars( int t )
{
   ChangeDrawOptionForCurrentPanel(1, t);
}

void TGo4HisDrawOptions::SetCoordinates( int t )
{
   ChangeDrawOptionForCurrentPanel(2, t);
}

void TGo4HisDrawOptions::XaxisStyle( int t )
{
   ChangeDrawOptionForCurrentPanel(3, t);
}

void TGo4HisDrawOptions::YaxisStyle( int t )
{
    ChangeDrawOptionForCurrentPanel(4, t);
}

void TGo4HisDrawOptions::ZaxisStyle( int t )
{
   ChangeDrawOptionForCurrentPanel(5, t);
}

void TGo4HisDrawOptions::SetAutoScale( bool on )
{
   ChangeDrawOptionForCurrentPanel(10, on ? 1 : 0);
}

void TGo4HisDrawOptions::DecodeDrawOption(const char* drawopt,
                                          int& HisErrorStyle,
                                          int& HisCoordStyle,
                                          int& HisDrawStyle)
{
   HisDrawStyle = 0;
   HisErrorStyle = 0;
   HisCoordStyle = 0;

   if ((drawopt==0) || (*drawopt==0)) return;

   TString buf(drawopt);
   buf.ToLower();

// check for error options in string:

   if (fiLastView==view_Graph) {

      if (buf.Contains("b") && buf.Contains("*")) HisDrawStyle = 15; else
      if (buf.Contains("b") && buf.Contains("p")) HisDrawStyle = 14; else
      if (buf.Contains("c") && buf.Contains("*")) HisDrawStyle = 13; else
      if (buf.Contains("c") && buf.Contains("p")) HisDrawStyle = 12; else
      if (buf.Contains("f") && buf.Contains("*")) HisDrawStyle = 11; else
      if (buf.Contains("f") && buf.Contains("p")) HisDrawStyle = 10; else
      if (buf.Contains("l") && buf.Contains("*")) HisDrawStyle = 9; else
      if (buf.Contains("l") && buf.Contains("p")) HisDrawStyle = 8; else
      if (buf.Contains("b")) HisDrawStyle = 7; else
      if (buf.Contains("c")) HisDrawStyle = 6; else
      if (buf.Contains("f2")) HisDrawStyle = 5; else
      if (buf.Contains("f1")) HisDrawStyle = 4; else
      if (buf.Contains("f")) HisDrawStyle = 3; else
      if (buf.Contains("l")) HisDrawStyle = 2; else
      if (buf.Contains("*")) HisDrawStyle = 1;

      if (buf.Contains("x+") && buf.Contains("y+")) HisCoordStyle = 4; else
      if (buf.Contains("y+")) HisCoordStyle = 3; else
      if (buf.Contains("x+")) HisCoordStyle = 2; else
      if (buf.Contains("a1")) HisCoordStyle = 5; else
      if (buf.Contains("a")) HisCoordStyle = 0; else HisCoordStyle = 1;

      if (buf.Contains("x") && (HisCoordStyle!=4) && (HisCoordStyle!=2)) HisErrorStyle = 1; else
      if (buf.Contains("|>")) HisErrorStyle = 3; else
      if (buf.Contains(">")) HisErrorStyle = 2; else
      if (buf.Contains("2") && (HisDrawStyle != 5)) HisErrorStyle = 4; else
      if (buf.Contains("3")) HisErrorStyle = 5; else
      if (buf.Contains("4")) HisErrorStyle = 6; else
      if (buf.Contains("[]")) HisErrorStyle = 7;

      return;
   }

   if (fiLastView==view_Histo1) {
      if(buf.Contains("e1")) HisErrorStyle = 2; else
      if(buf.Contains("e2")) HisErrorStyle = 3; else
      if(buf.Contains("e3")) HisErrorStyle = 4; else
      if(buf.Contains("e4")) HisErrorStyle = 5; else
      if(buf.Contains("e") && !buf.Contains("text") && !buf.Contains("lego") && !buf.Contains("image")) HisErrorStyle = 1;
   } else { // Histo2
      if (buf.Contains("lego") || buf.Contains("surf")) {
         if (buf.Contains("fb") && buf.Contains("bb") && buf.Contains("z")) HisErrorStyle = 7; else
         if (buf.Contains("bb") && buf.Contains("z")) HisErrorStyle = 6; else
         if (buf.Contains("fb") && buf.Contains("z")) HisErrorStyle = 5; else
         if (buf.Contains("fb") && buf.Contains("bb")) HisErrorStyle = 4; else
         if (buf.Contains("bb")) HisErrorStyle = 3; else
         if (buf.Contains("fb")) HisErrorStyle = 2; else
         if (buf.Contains("z")) HisErrorStyle = 1;
      } else
         if (buf.Contains("z")) HisErrorStyle = 1;
   }

   // check coordinate specs in string:
   if(buf.Contains(",pol")) HisCoordStyle = 1; else
   if(buf.Contains(",sph")) HisCoordStyle = 2; else
   if(buf.Contains(",psr")) HisCoordStyle = 3; else
   if(buf.Contains(",cyl")) HisCoordStyle = 4;

   // check draw options itself:

   if (fiLastView==view_Histo1) {
      if(buf.Contains("surf5")) HisDrawStyle = 21; else
      if(buf.Contains("surf4")) HisDrawStyle = 20; else
      if(buf.Contains("surf3")) HisDrawStyle = 19; else
      if(buf.Contains("surf2")) HisDrawStyle = 18; else
      if(buf.Contains("surf1")) HisDrawStyle = 17; else
      if(buf.Contains("surf"))  HisDrawStyle = 16; else
      if(buf.Contains("lego2")) HisDrawStyle = 15; else
      if(buf.Contains("lego1")) HisDrawStyle = 14; else
      if(buf.Contains("lego"))  HisDrawStyle = 13; else
      if(buf.Contains("lego3")) HisDrawStyle = 13; else // from gededitor
      if(buf.Contains("bar"))   HisDrawStyle = 12; else
      if(buf.Contains("text"))  HisDrawStyle = 11; else
      if(buf.Contains("]["))    HisDrawStyle = 10; else
      if(buf.Contains("9"))     HisDrawStyle =  9; else
      if(buf.Contains("p0"))    HisDrawStyle =  8; else
      if(*drawopt == 'p')       HisDrawStyle =  7; else
      if(*drawopt == 'P')       HisDrawStyle =  7; else
      if(*drawopt == 'b')       HisDrawStyle =  6; else
      if(*drawopt == 'B')       HisDrawStyle =  6; else
      if((*drawopt == 'c') && !buf.Contains("nostack")) HisDrawStyle =  5; else
      if((*drawopt == 'C') && !buf.Contains("nostack")) HisDrawStyle =  5; else
      if(buf.Contains("lf2"))   HisDrawStyle =  4; else
      if(*drawopt == 'l')       HisDrawStyle =  3; else
      if(*drawopt == 'L')       HisDrawStyle =  3; else
      if(buf.Contains("*h"))    HisDrawStyle =  2; else
      if(buf.Contains("ah"))    HisDrawStyle =  1;

      if ((HisDrawStyle>=16) && ((HisCoordStyle==0) || (HisCoordStyle==2)))
        HisCoordStyle = 1;
   } else { // Histo2
      if(buf.Contains("col"))   HisDrawStyle =  1; else
      if(buf.Contains("lego3")) HisDrawStyle =  9; else // from gededitor
      if(buf.Contains("lego1")) HisDrawStyle =  8; else
      if(buf.Contains("lego2")) HisDrawStyle =  3; else
      if(buf.Contains("lego"))  HisDrawStyle =  9; else
      if(buf.Contains("cont0")) HisDrawStyle =  2; else
      if(buf.Contains("cont1")) HisDrawStyle =  6; else
      if(buf.Contains("cont2")) HisDrawStyle = 10; else
      if(buf.Contains("cont3")) HisDrawStyle = 11; else
      if(buf.Contains("cont4")) HisDrawStyle =  7; else
      if(buf.Contains("cont"))  HisDrawStyle =  2; else
      if(buf.Contains("surf1")) HisDrawStyle =  5; else
      if(buf.Contains("surf2")) HisDrawStyle =  4; else
      if(buf.Contains("surf3")) HisDrawStyle = 13; else
      if(buf.Contains("surf4")) HisDrawStyle = 14; else
      if(buf.Contains("surf5")) HisDrawStyle = 15; else
      if(buf.Contains("surf"))  HisDrawStyle = 12; else
      if(buf.Contains("arr"))   HisDrawStyle = 16; else
      if(buf.Contains("box"))   HisDrawStyle = 17; else
      if(buf.Contains("text"))  HisDrawStyle = 18; else
      if(buf.Contains("asimage")) HisDrawStyle=19;
   }
   //cout << *drawopt << " Decode " << buf << " Error " << HisErrorStyle
   //     << " Coord " << HisCoordStyle  << " Draw " <<HisDrawStyle<<endl;
}

void TGo4HisDrawOptions::CodeDrawOptions(int HisErrorStyle,
                                         int HisCoordStyle,
                                         int HisDrawStyle,
                                         QString& buf)
{
   buf = "";

   if (fiLastView==view_Graph) {

      switch (HisDrawStyle) {
         case 0: buf = "P"; break;
         case 1: buf = "*"; break;
         case 2: buf = "L"; break;
         case 3: buf = "F"; break;
         case 4: buf = "F1"; break;
         case 5: buf = "F2"; break;
         case 6: buf = "C"; break;
         case 7: buf = "B"; break;
         case 8: buf = "LP"; break;
         case 9: buf = "L*"; break;
         case 10: buf = "FP"; break;
         case 11: buf = "F*"; break;
         case 12: buf = "CP"; break;
         case 13: buf = "C*"; break;
         case 14: buf = "BP"; break;
         case 15: buf = "B*"; break;
         default: buf = "P";
      }

      switch (HisErrorStyle) {
         case 0: break;
         case 1: buf.append("X"); break;
         case 2: buf.append(">"); break;
         case 3: buf.append("|>"); break;
         case 4: buf.append("2"); break;
         case 5: buf.append("3"); break;
         case 6: buf.append("4"); break;
         case 7: buf.append("[]"); break;
      }

      switch (HisCoordStyle) {
         case 0: buf.append("A"); break;
         case 1: break;
         case 2: buf.append("AX+"); break;
         case 3: buf.append("AY+"); break;
         case 4: buf.append("AX+Y+"); break;
         case 5: buf.append("A1"); break;
      }

      return;
   }

   if (fiLastView==view_Histo1) {

    switch (HisDrawStyle) {
       case  0: buf="";      break;
       case  1: buf="ah";    break;
       case  2: buf="*h";    break;
       case  3: buf="l";     break;
       case  4: buf="lf2";   break;
       case  5: buf="c";     break;
       case  6: buf="b";     break;
       case  7: buf="p";     break;
       case  8: buf="p0";    break;
       case  9: buf="9";     break;
       case 10: buf="][";    break;
       case 11: buf="text";  break;
       case 12: buf="bar";   break;
       case 13: buf="lego3"; break; // for geditor, "lego"
       case 14: buf="lego1"; break;
       case 15: buf="lego2"; break;
       case 16: buf="surf";  break;
       case 17: buf="surf1"; break;
       case 18: buf="surf2"; break;
       case 19: buf="surf3"; break;
       case 20: buf="surf4"; break;
       case 21: buf="surf5"; break;
    }
    if ((HisDrawStyle>=16) && ((HisCoordStyle==0) || (HisCoordStyle==2)))
       HisCoordStyle = 1;
   } else // Histo2

    switch (HisDrawStyle) {
       case  0: buf="";  break;
       case  1: buf="col";   break;
       case  9: buf="lego3"; break;
       case  8: buf="lego1"; break;
       case  3: buf="lego2"; break;
       case  2: buf="cont0"; break;
       case  6: buf="cont1"; break;
       case 10: buf="cont2"; break;
       case 11: buf="cont3"; break;
       case  7: buf="cont4"; break;
       case 12: buf="surf";  break;
       case  5: buf="surf1"; break;
       case  4: buf="surf2"; break;
       case 13: buf="surf3"; break;
       case 14: buf="surf4"; break;
       case 15: buf="surf5"; break;
       case 16: buf="arr";   break;
       case 17: buf="box";   break;
       case 18: buf="text";  break;
#ifdef __GO4ASI__
       case 19: buf="asimage"; return; break;
#else
       case 19: buf=""; break;
#endif
    }

    switch (HisCoordStyle) {
       case 0:  break;
       case 1:  buf.append(",pol"); break;
       case 2:  buf.append(",sph"); break;
       case 3:  buf.append(",psr"); break;
       case 4:  buf.append(",cyl"); break;
    }

    if (fiLastView==view_Histo1)
       switch (HisErrorStyle) {
          case 0:  break;
          case 1:  buf.append("E"); break;
          case 2:  buf.append("E1"); break;
          case 3:  buf.append("E2"); break;
          case 4:  buf.append("E3"); break;
          case 5:  buf.append("E4"); break;
       }
    else
       switch (HisErrorStyle) {
          case 0:  break;
          case 1:  buf.append("Z"); break;
          case 2:  buf.append("FB"); break;
          case 3:  buf.append("BB"); break;
          case 4:  buf.append("BBFB"); break;
          case 5:  buf.append("ZFB"); break;
          case 6:  buf.append("ZBB"); break;
          case 7:  buf.append("ZBBFB"); break;
       }
}


void TGo4HisDrawOptions::SetLineColor()
{
   ChangeColor(0);
}

void TGo4HisDrawOptions::SetFillColor()
{
   ChangeColor(1);
}

void TGo4HisDrawOptions::SetMarkerColor()
{
   ChangeColor(2);
}

void TGo4HisDrawOptions::ChangeColor(int kind)
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel==0) return;

   TPad* pad = panel->GetActivePad();
   TObject* obj = panel->GetSelectedObject(pad, 0);

   Color_t icol = 0;

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (att==0) return;
        icol = att->GetLineColor();
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (att==0) return;
        icol = att->GetFillColor();
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (att==0) return;
        icol = att->GetMarkerColor();
        break;
      }
   }

   TColor* rcol = gROOT->GetColor(icol);

   QColor qcol;

   if (rcol!=0)
      qcol.setRgb(rcol->GetRed(), rcol->GetGreen(), rcol->GetBlue());

   QColor c = QColorDialog::getColor(qcol);
   if (!c.isValid()) return;

   icol = TColor::GetColor(c.red(), c.green(), c.blue());

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (att!=0) att->SetLineColor(icol);
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (att!=0) att->SetFillColor(icol);
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (att!=0) att->SetMarkerColor(icol);
        break;
      }
   }

   panel->MarkPadModified(pad);

   panel->ShootRepaintTimer(pad);

   panel->ActivateInGedEditor(obj);
}
