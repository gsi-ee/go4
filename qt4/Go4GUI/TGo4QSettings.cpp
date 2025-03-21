// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4QSettings.h"

#include "TSystem.h"

#include "TGo4Marker.h"
#include "TGo4Condition.h"
#include "TGo4MdiArea.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QFont>
#include <QApplication>
#include <QMainWindow>


const char *MainWindowDefaultState = "000000ff00000000fd000000020000000000000178000002a9fc0200000001fb0000001600420072006f00770073006500720044006f0063006b0100000071000002a90000007e0100000300000003000003d500000082fc0100000001fc00000000000003d50000000000fffffffa000000020200000003fb00000016004c006f00670049006e0066006f0044006f0063006b0000000000ffffffff0000005701000003fb0000001a004d006200730056006900650077006500720044006f0063006b0000000000ffffffff0000006b01000003fb0000001e0044004100420043004d006f006e00690074006f00720044006f0063006b0000000279000000ce000000000000000000000392000002a900000004000000040000000800000008fc0000000800000002000000050000001600460069006c00650054006f006f006c0042006100720100000000ffffffff0000000000000000000000160047006f00340054006f006f006c007300420061007201000000f6ffffffff00000000000000000000001e0041006e0061006c00790073006900730054006f006f006c0042006100720100000226ffffffff00000000000000000000001a0044006900760069006400650050006100640044006f0063006b0000000395000000e400000000000000000000002c0041006e0061006c00790073006900730043006f006d006d0061006e00640054006f006f006c0042006100720000000339000001d50000000000000000000000020000000000000002000000020000002400420072006f0077007300650072004f007000740069006f006e00730044006f0063006b01000000000000010200000000000000000000001e0044007200610077004f007000740069006f006e00730044006f0063006b01000001020000037e00000000000000000000000200000002000000240048006900730044007200610077004f007000740069006f006e00730044006f0063006b0000000000000001290000000000000000000000180047006f0034005300740079006c00650044006f0063006b0000000000000004b10000000000000000000000020000000100000020005300630061006c0065004f007000740069006f006e00730044006f0063006b0100000000ffffffff0000000000000000000000030000000000000003000000010000001e0043006f006d006d0061006e0064004c0069006e00650044006f0063006b0000000000000004b1000000000000000000000003000000010000001c00540072006500650056006900650077006500720044006f0063006b0000000000000004b10000000000000000";

const char *MainWindowDefaultGeometry = "01d9d0cb0003000000000ad90000002d00000fe60000037a00000ad90000004a00000fe60000037a00000000000000000a0000000ad90000004a00000fe60000037a";

TGo4QSettings *go4sett = nullptr;

TGo4QSettings::TGo4QSettings(const QString& filename)
{
   fSettFileName = filename;

   fUseWeb = false;

   Open();
}

TGo4QSettings::~TGo4QSettings()
{
   // do not destroy QSettings - they will be automatically saved
   // delete sett;
}

QString TGo4QSettings::GetSettLoaction()
{
   if (fSettFileName.length() > 0) return fSettFileName;
#ifdef _MSC_VER
   return QString("Registry (default)");
#endif

   return QDir::homePath() + "/.config/GSI/go4.conf (default)";
}

void TGo4QSettings::Open()
{
   if (fSettFileName.length()>0)
      sett = new QSettings(fSettFileName, QSettings::NativeFormat);
   else
      sett = new QSettings("GSI","go4");
}

void TGo4QSettings::Store()
{
   delete sett;
   sett = nullptr;
   Open();
}

void TGo4QSettings::setBool(const QString& name, bool value)
{
   sett->setValue(name, QString(value ? "true" : "false"));
}

bool TGo4QSettings::getBool(const QString& name, bool def)
{
   return sett->value(name, QString(def ? "true" : "false")).toString() == QString("true");
}

void TGo4QSettings::setInt(const QString& name, int value)
{
   sett->setValue(name, value);
}

int TGo4QSettings::getInt(const QString& name, int def)
{
   return sett->value(name, def).toInt();
}

void TGo4QSettings::setStr(const QString& name, const QString& value)
{
   sett->setValue(name, value);
}

QString TGo4QSettings::getStr(const QString& name, const QString& def)
{
   return sett->value(name, def).toString();
}

void TGo4QSettings::setDouble(const QString& name, double value)
{
   sett->setValue(name, value);
}

double TGo4QSettings::getDouble(const QString& name, double def)
{
   return sett->value(name, def).toDouble();
}

// set of Go4 specific functions

void TGo4QSettings::setBasicSettings()
{
   //writeEntry( "/Font", QApplication::font().toString() );
   Bool_t HASLABEL, HASCONNECTOR, XDRAW, YDRAW, XBINDRAW, YBINDRAW, CONTDRAW;
   TString NUMFORMAT;
   TGo4Marker::GetGlobalStyle(HASLABEL, HASCONNECTOR, XDRAW, YDRAW,
                              XBINDRAW, YBINDRAW, CONTDRAW, NUMFORMAT);

   setBool("/markers/Markerlabel", HASLABEL);
   setBool("/markers/Connectorline", HASCONNECTOR);
   setBool("/markers/ShowX", XDRAW);
   setBool("/markers/ShowY", YDRAW);
   setBool("/markers/ShowXbin", XBINDRAW);
   setBool("/markers/ShowYbin", YBINDRAW);
   setBool("/markers/ShowBinContent", CONTDRAW);
   setStr("/markers/LabelNumberFormat", NUMFORMAT.Data());

   Bool_t LABELDRAW, LIMITSDRAW, INTDRAW,
          XMEANDRAW, YMEANDRAW, XRMSDRAW, YRMSDRAW,
          XMAXDRAW, YMAXDRAW, CMAXDRAW;
   TGo4Condition::GetGlobalStyle(LABELDRAW, LIMITSDRAW, INTDRAW,
                                 XMEANDRAW, YMEANDRAW, XRMSDRAW, YRMSDRAW,
                                 XMAXDRAW, YMAXDRAW, CMAXDRAW, NUMFORMAT);
   setBool("/conditions/Conditionlabel", LABELDRAW);
   setBool("/conditions/ShowLimits", LIMITSDRAW);
   setBool("/conditions/ShowIntegral", INTDRAW);
   setBool("/conditions/ShowXmean", XMEANDRAW);
   setBool("/conditions/ShowYmean", YMEANDRAW);
   setBool("/conditions/ShowXrms", XRMSDRAW);
   setBool("/conditions/ShowYrms", YRMSDRAW);
   setBool("/conditions/ShowXmax", XMAXDRAW);
   setBool("/conditions/ShowYmax", YMAXDRAW);
   setBool("/conditions/ShowCmax", CMAXDRAW);
   setStr("/conditions/LabelNumberFormat", NUMFORMAT.Data());
}

void TGo4QSettings::getBasicSettings()
{
   //   QString AppStyle = readEntry( "/Style/AppStyle", "windows");
   //   QApplication::setStyle(AppStyle);

   //   QFont font;
   //   font.fromString(readEntry( "/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
   //   QApplication::setFont(font, true );

   TGo4Marker::SetGlobalStyle(getBool("/markers/Markerlabel", 1),
                              getBool("/markers/Connectorline", 1),
                              getBool("/markers/ShowX", 1),
                              getBool("/markers/ShowY", 0),
                              getBool("/markers/ShowXbin", 0),
                              getBool("/markers/ShowYbin", 0),
                              getBool("/markers/ShowBinContent", 1),
                              getStr("/markers/LabelNumberFormat", "%.4E").toLatin1().constData());

   TGo4Condition::SetGlobalStyle(getBool("/conditions/Conditionlabel", 1),
                                 getBool("/conditions/ShowLimits", 1),
                                 getBool("/conditions/ShowIntegral", 1),
                                 getBool("/conditions/ShowXmean", 1),
                                 getBool("/conditions/ShowYmean", 0),
                                 getBool("/conditions/ShowXrms", 1),
                                 getBool("/conditions/ShowYrms", 0),
                                 getBool("/conditions/ShowXmax", 1),
                                 getBool("/conditions/ShowYmax", 0),
                                 getBool("/conditions/ShowCmax", 1),
                                 getStr("/conditions/LabelNumberFormat", "%.4E").toLatin1().constData());
}

void  TGo4QSettings::setAppFont(const QFont& font)
{
   setStr("/Font", font.toString());
}

void TGo4QSettings::getAppFont()
{
   QFont font;
   font.fromString(getStr("/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
   QApplication::setFont(font);
}

void  TGo4QSettings::setTermFont(const QFont& font)
{
   setStr("/TermFont", font.toString());

}
QFont TGo4QSettings::getTermFont()
{
   QFont font;
//   font.fromString(getStr("/TermFont", "Arial,11,-1,5,50,0,0,0,0,0"));
   font.fromString(getStr("/TermFont", ""));
   return font;
}

void TGo4QSettings::setAppStyle(const QString& v)
{
   setStr("/Style/AppStyle", v);
}

QString TGo4QSettings::getAppStyle()
{
   return getStr("/Style/AppStyle", "windows");
}

void TGo4QSettings::setClientName(const QString& v)
{
   setStr("/ClientSetting/Name", v);
}

QString TGo4QSettings::getClientName()
{
   return getStr("/ClientSetting/Name", "MyAnalysis");
}

void TGo4QSettings::setClientNode(const QString& v)
{
   setStr(  "/ClientSetting/Node", v);
}

QString TGo4QSettings::getClientNode()
{
   return getStr("/ClientSetting/Node", "localhost");
}

void TGo4QSettings::setClientDir(const QString& v)
{
   setStr("/ClientSetting/Dir", v);
}

QString TGo4QSettings::getClientDir()
{
   return getStr("/ClientSetting/Dir", QDir::currentPath());
}

void TGo4QSettings::setClientExeMode(int mode)
{
   setInt("/ClientSetting/ExeMode", mode);
}

int TGo4QSettings::getClientExeMode()
{
   return getInt("/ClientSetting/ExeMode", 1);
}

void TGo4QSettings::setClientExec(const QString& v)
{
   if (getClientExeMode() == 0) setStr("/ClientSetting/Exec", v);
                           else setStr("/ClientSetting/Lib", v);
}

QString TGo4QSettings::getClientExec(int mode)
{
   if (mode < 0) mode = getClientExeMode();
   return mode == 0 ? getStr("/ClientSetting/Exec", "./MainUserAnalysis") : getStr("/ClientSetting/Lib", "libGo4UserAnalysis");
}

void TGo4QSettings::setClientArgs(const QString& v)
{
   setStr("/ClientSetting/Args", v);
}

QString TGo4QSettings::getClientArgs()
{
   return getStr("/ClientSetting/Args", "");
}

void TGo4QSettings::setClientShellMode(int v)
{
   const char *name = "exec";
   switch (v) {
      case 0: name = "exec"; break;
      case 1: name = "rsh"; break;
      case 2: name = "ssh"; break;
   }

   setStr(  "/ClientSetting/Shell", name);
}

QString TGo4QSettings::getClientShell()
{
   return getStr("/ClientSetting/Shell", "exec");
}

int TGo4QSettings::getClientShellMode()
{
   QString v = getClientShell();
   int shellmode = 2;
   if(v.contains("exec")) shellmode = 0; else
   if(v.contains("rsh"))  shellmode = 1; else
   if(v.contains("ssh"))  shellmode = 2;
   return shellmode;
}


void TGo4QSettings::setClientTermMode(int v)
{
   QString res = "qt";
   switch(v) {
      case 1: res = "qt"; break;
      case 2: res = "xterm"; break;
      case 3: res = "konsole"; break;
   }
   setStr("/ClientSetting/Term", res);
}

QString TGo4QSettings::getClientTerm()
{
   return getStr("/ClientSetting/Term", "qt");
}

int TGo4QSettings::getClientTermMode()
{
   QString v = getClientTerm();
   int termmode = 1;
   if(v.contains("qt")) termmode = 1; else
      if(v.contains("xterm")) termmode = 2; else
         if(v.contains("konsole")) termmode = 3;
   return termmode;
}

void  TGo4QSettings::setClientIsServer(int kind)
{
   QString v = "false";
   if (kind==1) v = "true";
   if (kind==2) v = "http";
   setStr("/ClientSetting/IsServer", v);
}

int TGo4QSettings::getClientIsServer()
{
   QString v = getStr("/ClientSetting/IsServer", "false");
   if (v == "true") return 1;
   if (v == "http") return 2;
   return 0;
}

void TGo4QSettings::setClientPort(int nport)
{
   setInt( "/ClientSetting/ClientPort", nport);
}

int TGo4QSettings::getClientPort()
{
   return getInt( "/ClientSetting/ClientPort", 5000);
}

void TGo4QSettings::setClientControllerMode(int mode)
{
   setInt("/ClientSetting/Controller", mode);
}

int TGo4QSettings::getClientControllerMode()
{
   return getInt("/ClientSetting/Controller", 0);
}

void TGo4QSettings::setClientDefaultPass(bool on)
{
   setBool("/ClientSetting/DefaultPass", on);
}

bool TGo4QSettings::getClientDefaultPass()
{
   return getBool("/ClientSetting/DefaultPass", true);
}

void TGo4QSettings::setClientConnectMode(int mode)
{
   setInt("/ClientSetting/Connection", mode);
}

int TGo4QSettings::getClientConnectMode()
{
   return getInt("/ClientSetting/Connection", 0);
}

QString TGo4QSettings::getClientAccountName()
{
   return getStr("/ClientSetting/Account", "observer");
}

void TGo4QSettings::setClientAccountName(const QString &v)
{
   setStr("/ClientSetting/Account", v);
}

void TGo4QSettings::setTermHistorySize(int sz)
{
   setInt("/AnalisysTerminal/HistorySize", sz);
}

int TGo4QSettings::getTermHistorySize()
{
   return getInt("/AnalisysTerminal/HistorySize", 100000);
}

void TGo4QSettings::setHServName(const QString &v)
{
   setStr("/HistogramClient/server", v);
}

QString TGo4QSettings::getHServName()
{
   return getStr("/HistogramClient/server", "");
}

void TGo4QSettings::setHServBase(const QString &v)
{
   setStr("/HistogramClient/base", v);
}

QString TGo4QSettings::getHServBase()
{
   return getStr("/HistogramClient/base", "");
}

void TGo4QSettings::setHServFilter(const QString &v)
{
   setStr("/HistogramClient/filter", v);
}

QString TGo4QSettings::getHServFilter()
{
   return getStr("/HistogramClient/filter", "*");
}

void TGo4QSettings::setHServPort(int port)
{
   setInt("/HistogramClient/port", port);
}

int TGo4QSettings::getHServPort()
{
   return getInt("/HistogramClient/port", 6009);
}

void TGo4QSettings::setPadCrosshair(bool on)
{
   setBool("/geometry/Crosshairmode", on);
}

bool TGo4QSettings::getPadCrosshair()
{
   return getBool(  "/geometry/Crosshairmode", false);
}

void TGo4QSettings::setPadSuperimpose(bool on)
{
   setBool("/geometry/Superimpose", on);
}

bool TGo4QSettings::getPadSuperimpose()
{
   return getBool("/geometry/Superimpose", false);
}

void TGo4QSettings::setPadEventStatus(bool on)
{
   setBool("/geometry/Eventstatusmode", on);
}

bool TGo4QSettings::getPadEventStatus()
{
   return getBool("/geometry/Eventstatusmode", false);
}

void TGo4QSettings::setCanvasColor(int red, int green, int blue)
{
   setInt( "/ViewPanel/BackRed", red );
   setInt( "/ViewPanel/BackGrn", green);
   setInt( "/ViewPanel/BackBlu", blue );
}

void TGo4QSettings::getCanvasColor(int& red, int& green, int& blue)
{
   red   = getInt( "/ViewPanel/BackRed", 220);
   green = getInt( "/ViewPanel/BackGrn", 220);
   blue  = getInt( "/ViewPanel/BackBlu", 220);
}


void TGo4QSettings::setPaletteOpt(int min, int def, int max)
{
   setInt( "/ViewPanel/PaletteMin", min);
   setInt( "/ViewPanel/PaletteDef", def);
   setInt( "/ViewPanel/PaletteMax", max);
}

void TGo4QSettings::getPaletteOpt(int& min, int& def, int& max)
{
   min = getInt( "/ViewPanel/PaletteMin", 0);
   def = getInt( "/ViewPanel/PaletteDef", 1);
   max = getInt( "/ViewPanel/PaletteMax", 56);
   if (max == 56) max = 112; // default values for old ROOT can be overwritten
}

void TGo4QSettings::setCloneFlag(bool on)
{
   setBool("/ViewPanel/CloneFlag", on);
}

bool TGo4QSettings::getCloneFlag()
{
   return getBool(  "/ViewPanel/CloneFlag", true);
}

void TGo4QSettings::setDrawOnceFlag(bool on)
{
   setBool("/ViewPanel/DrawOnce", on);
}

bool TGo4QSettings::getDrawOnceFlag()
{
   return getBool("/ViewPanel/DrawOnce", false);
}

void TGo4QSettings::setDrawTimeFlag(bool on)
{
   setBool("/ViewPanel/DrawTime", on);
}

bool TGo4QSettings::getDrawTimeFlag()
{
   return getBool("/ViewPanel/DrawTime", true);
}

void TGo4QSettings::setDrawDateFlag(bool on)
{
   setBool("/ViewPanel/DrawDate", on);
}

bool TGo4QSettings::getDrawDateFlag()
{
   return getBool("/ViewPanel/DrawDate");
}

void TGo4QSettings::setDrawItemFlag(bool on)
{
   setBool("/ViewPanel/DrawItem", on);
}

bool TGo4QSettings::getDrawItemFlag()
{
   return getBool("/ViewPanel/DrawItem");
}

void TGo4QSettings::setDrawLineWidth(int w)
{
   setInt("/ViewPanel/DrawLineWidth", w);
}

int TGo4QSettings::getDrawLineWidth()
{
   return getInt("/ViewPanel/DrawLineWidth", 1);
}

void TGo4QSettings::setDrawFillColor(int col)
{
   setInt("/ViewPanel/DrawFillColor", col);
}

int TGo4QSettings::getDrawFillColor()
{
   return getInt("/ViewPanel/DrawFillColor", 0);
}

void TGo4QSettings::setDrawFillStyle(int style)
{
   setInt("/ViewPanel/DrawFillStyle", style);
}

int TGo4QSettings::getDrawFillStyle()
{
   return getInt("/ViewPanel/DrawFillStyle", 1001);
}

void  TGo4QSettings::setTH1DrawOpt(const QString& v)
{
   setStr("/ViewPanel/TH1DrawOpt", v);
}

QString TGo4QSettings::getTH1DrawOpt()
{
   return getStr("/ViewPanel/TH1DrawOpt", "hist");
}

void  TGo4QSettings::setTH2DrawOpt(const QString& v)
{
   setStr("/ViewPanel/TH2DrawOpt", v);
}

QString TGo4QSettings::getTH2DrawOpt()
{
   return getStr("/ViewPanel/TH2DrawOpt", "col");
}

void  TGo4QSettings::setTH3DrawOpt(const QString& v)
{
   setStr("/ViewPanel/TH3DrawOpt", v);
}

QString TGo4QSettings::getTH3DrawOpt()
{
   return getStr("/ViewPanel/TH3DrawOpt", "hist");
}

void  TGo4QSettings::setTGraphDrawOpt(const QString& v)
{
   setStr("/ViewPanel/TGraphDrawOpt", v);
}

QString TGo4QSettings::getTGraphDrawOpt()
{
   return getStr("/ViewPanel/TGraphDrawOpt", "al");
}

void TGo4QSettings::setGStyleStatFormat(const QString& v)
{
   setStr("/ViewPanel/GStyleStatFormat", v);
}

QString TGo4QSettings::getGStyleStatFormat()
{
   return getStr("/ViewPanel/GStyleStatFormat", "");
}

void TGo4QSettings::restoreMainWindowState(QMainWindow* tgt)
{
   tgt->restoreState(sett->value("/MainWindow/State", QByteArray::fromHex(MainWindowDefaultState)).toByteArray());
   tgt->restoreGeometry(sett->value("/MainWindow/Geometry", QByteArray::fromHex(MainWindowDefaultGeometry)).toByteArray());
}

void TGo4QSettings::storeMainWindowState(QMainWindow* src)
{
   // printf("state: %s\n", src->saveState().toHex().data());
   // printf("geom: %s\n", src->saveGeometry().toHex().data());

   sett->setValue("/MainWindow/State", src->saveState());
   sett->setValue("/MainWindow/Geometry", src->saveGeometry());
}

void TGo4QSettings::storePanelSize(QWidget *w, const QString& kind)
{
   setInt(QString("/") + kind + QString("/Width"), w->width());
   setInt(QString("/") + kind + QString("/Height"), w->height());
}

QSize TGo4QSettings::lastPanelSize(const QString& kind, int dfltwidth, int dfltheight)
{
   QSize rect(getInt(QString("/") + kind + QString("/Width"), dfltwidth),
              getInt(QString("/") + kind + QString("/Height"), dfltheight));

   if (rect.height() > 1.5*TGo4MdiArea::Instance()->height())
      rect.setHeight(1.5*TGo4MdiArea::Instance()->height());

   if (rect.width() > 1.5*TGo4MdiArea::Instance()->width())
      rect.setWidth(1.5*TGo4MdiArea::Instance()->width());

   return rect;
}

void TGo4QSettings::setBrowserColumn(const char *name, int width)
{
   setInt( QString("/Browser/") + name, width);
}

int TGo4QSettings::getBrowserColumn(const char *name, int defwidth)
{
   return getInt( QString("/Browser/") + name, defwidth);
}


void TGo4QSettings::setOptStat(int value)
{
   setInt( "/HistStatsBox/OptStats", value);
}

int TGo4QSettings::getOptStat()
{
   return getInt( "/HistStatsBox/OptStats", 11111111);
}

void TGo4QSettings::setOptStatW(int value)
{
   setInt( "/HistStatsBox/OptStatsW", value);
}

int TGo4QSettings::getOptStatW()
{
   return getInt( "/HistStatsBox/OptStatsW", 0);
}

void TGo4QSettings::setOptStatH(int value)
{
   setInt( "/HistStatsBox/OptStatsH", value);
}

int TGo4QSettings::getOptStatH()
{
   return getInt( "/HistStatsBox/OptStatsH", 0);
}

void TGo4QSettings::setStatBoxVisible(bool on)
{
   setBool( "/HistStatsBox/Visible", on);
}

bool TGo4QSettings::getStatBoxVisible()
{
   return getBool( "/HistStatsBox/Visible", true);
}

void TGo4QSettings::setStatBoxErrors(bool on)
{
   setBool( "/HistStatsBox/Errors", on);
}

bool TGo4QSettings::getStatBoxErrors()
{
   return getBool( "/HistStatsBox/Errors", false);
}

void  TGo4QSettings::setHistName(const QString& value)
{
   setStr("/NewHist/Name", value);
}

QString TGo4QSettings::getHistName()
{
   return getStr("/NewHist/Name", "histo1");
}

void  TGo4QSettings::setHistTitle(const QString& value)
{
   setStr("/NewHist/Title", value);
}

QString TGo4QSettings::getHistTitle()
{
   return getStr("/NewHist/Title", "histogram title");
}

void TGo4QSettings::setHistType(int value)
{
   setInt( "/NewHist/HType", value);
}

int TGo4QSettings::getHistType()
{
   return getInt( "/NewHist/HType", 14);
}

void TGo4QSettings::setHistAxisPars(int naxis, int npoints, double min, double max)
{
   QString sname =  "/NewHist/Axis";
   switch (naxis) {
      case 0: sname+="X_"; break;
      case 1: sname+="Y_"; break;
      case 2: sname+="Z_"; break;
      default: sname+="X_"; break;
   }
   setInt(sname+"NPoints", npoints);
   setDouble(sname+"Min", min);
   setDouble(sname+"Max", max);
}

void TGo4QSettings::getHistAxisPars(int naxis, int& npoints, double& min, double& max)
{
   QString sname =  "/NewHist/Axis";
   switch (naxis) {
      case 0: sname+="X_"; break;
      case 1: sname+="Y_"; break;
      case 2: sname+="Z_"; break;
      default: sname+="X_"; break;
   }
   npoints = getInt(sname+"NPoints", 100);
   min = getDouble(sname+"Min", 0.);
   max = getDouble(sname+"Max", 100.);
}

void TGo4QSettings::setFetchDataWhenDraw(bool on)
{
   setBool("/preferences/FetchWhenDraw", on);
}

bool TGo4QSettings::getFetchDataWhenDraw()
{
   return getBool("/preferences/FetchWhenDraw", true);
}

void TGo4QSettings::setFetchDataWhenCopy(bool on)
{
   setBool("/preferences/FetchWhenCopy", on);
}

bool TGo4QSettings::getFetchDataWhenCopy()
{
   return getBool("/preferences/FetchWhenCopy", false);
}

void TGo4QSettings::setFetchDataWhenSave(bool on)
{
   setBool("/preferences/FetchWhenSave", on);
}

bool TGo4QSettings::getFetchDataWhenSave()
{
   return getBool("/preferences/FetchWhenSave", false);
}

void TGo4QSettings::setHideTGo4EventElement(bool on)
{
   setBool("/preferences/HideEventElement", on);
}

bool TGo4QSettings::getHideTGo4EventElement()
{
   return getBool("/preferences/HideEventElement", false);
}

void TGo4QSettings::setMoveSubwindowRubberBand(bool on)
{
   setBool("/preferences/MoveSubwindowRubberBand", on);
}

bool TGo4QSettings::getMoveSubwindowRubberBand()
{
   return getBool("/preferences/MoveSubwindowRubberBand", true);
}

void TGo4QSettings::setNativeMenuBar(bool on)
{
   setBool("/preferences/NativeMenuBar", on);
}

bool TGo4QSettings::getNativeMenuBar()
{
#ifdef Darwin
   bool dflt = true;
#else
   bool dflt = false;
#endif
   return getBool("/preferences/NativeMenuBar", dflt);
}


void TGo4QSettings::setRemoteFileSett(const QString& hostname, const QString& filename, const QString& protocol)
{
   setStr( "/OpenRemoteFile/Host", hostname);
   setStr( "/OpenRemoteFile/File", filename);
   setStr( "/OpenRemoteFile/Type", protocol);
}

void TGo4QSettings::getRemoteFileSett(QString& hostname, QString& filename, QString& protocol)
{
   hostname = getStr( "/OpenRemoteFile/Host", "jsroot.gsi.de");
   filename = getStr( "/OpenRemoteFile/File", "files/hsimple.root");
   protocol = getStr( "/OpenRemoteFile/Type", "http:");
}

void TGo4QSettings::setPrinterSett(const QString& name, const QString& cmd)
{
   setStr( "/Printer/Name", name);
   setStr( "/Printer/Prog", cmd);
}

void TGo4QSettings::getPrinterSett(QString &name, QString &cmd)
{
   name = getStr("/Printer/Name", gSystem->Getenv("PRINTER"));
   cmd = getStr("/Printer/Prog", "lpr");
}

void TGo4QSettings::setMbsMonitorNode(const QString &name)
{
   setStr("/MbsMonitor/Node", name);
}

QString TGo4QSettings::getMbsMonitorNode()
{
   return getStr("/MbsMonitor/Node", "r2-d2");
}

void TGo4QSettings::setMbsMonitorFreq(int secs)
{
   setInt("/MbsMonitor/Frequency", secs);
}

int TGo4QSettings::getMbsMonitorFreq()
{
   return getInt("/MbsMonitor/Frequency", 5);
}

void TGo4QSettings::setMbsMonitorBins(int num)
{
   setInt("/MbsMonitor/Bins", num);
}

int TGo4QSettings::getMbsMonitorBins()
{
   return getInt("/MbsMonitor/Bins", 200);
}

void TGo4QSettings::setMbsMonitorTrend(bool on)
{
   setBool("/MbsMonitor/TrendOn", on);
}

bool TGo4QSettings::getMbsMonitorTrend()
{
   return getBool("/MbsMonitor/TrendOn", false);
}

void TGo4QSettings::setMbsMonitorMore(bool on)
{
   setBool("/MbsMonitor/ShowMore", on);
}

bool TGo4QSettings::getMbsMonitorMore()
{
   return getBool("/MbsMonitor/ShowMore", true);
}

void TGo4QSettings::setMbsMonitorMonitorActive(bool on)
{
   setBool("/MbsMonitor/MonitorActive", on);
}

bool TGo4QSettings::getMbsMonitorMonitorActive()
{
   return getBool("/MbsMonitor/MonitorActive", false);
}

void TGo4QSettings::setMbsMonitorBackwardsTrending(bool on)
{
   setBool("/MbsMonitor/BackwardsTrending", on);
}

bool TGo4QSettings::getMbsMonitorBackwardsTrending()
{
   return getBool("/MbsMonitor/BackwardsTrending", true);
}

void TGo4QSettings::setDabcMonitorNode(const QString &name)
{
   setStr("/DabcMonitor/Node", name);
}

QString TGo4QSettings::getDabcMonitorNode()
{
   return getStr("/DabcMonitor/Node", "dimDns.gsi.de");
}

void TGo4QSettings::setDabcMonitorFreq(int secs)
{
   setInt("/DabcMonitor/Frequency", secs);
}

int TGo4QSettings::getDabcMonitorFreq()
{
   return getInt("/DabcMonitor/Frequency", 5);
}

void TGo4QSettings::setDabcMonitorBins(int num)
{
   setInt("/DabcMonitor/Bins", num);
}

int TGo4QSettings::getDabcMonitorBins()
{
   return getInt("/DabcMonitor/Bins", 200);
}

void TGo4QSettings::setDabcMonitorBackwardsTrending(bool on)
{
   setBool("/DabcMonitor/BackwardsTrending", on);
}

bool TGo4QSettings::getDabcMonitorBackwardsTrending()
{
   return getBool("/DabcMonitor/BackwardsTrending", true);
}

QStringList TGo4QSettings::getCommandsHistoryGUI()
{
   return sett->value("/CommandsHistoryGUI").toStringList();
}

void TGo4QSettings::setCommandsHistoryGUI(const QStringList &commands)
{
   sett->setValue("/CommandsHistoryGUI", commands);
}

QStringList TGo4QSettings::getCommandsHistoryAnalysis()
{
   return sett->value("/CommandsHistoryAnalysis").toStringList();
}

void TGo4QSettings::setCommandsHistoryAnalysis(const QStringList &commands)
{
   sett->setValue("/CommandsHistoryAnalysis", commands);
}

QString TGo4QSettings::getAnalysisMacroCommand(int id)
{
   QString descriptor = QString("/AnalysisMacro%1/Command").arg(id);

   return getStr(descriptor, "");
}

QString TGo4QSettings::getAnalysisMacroTip(int id)
{
   QString descriptor = QString("/AnalysisMacro%1/Tip").arg(id);
   QString deftip = QString("Ctrl - %1").arg(id + 1);
   return getStr(descriptor, deftip);
}

bool TGo4QSettings::getAnalysisMacroAutomode(int id)
{
	 QString descriptor=QString("/AnalysisMacro%1/Automode").arg(id);
	 return getBool(descriptor, false);
}

void TGo4QSettings::setAnalysisMacroAutomode(int id, bool on)
{
	 QString descriptor=QString("/AnalysisMacro%1/Automode").arg(id);
	 setBool(descriptor, on);
}


void    TGo4QSettings::setAnalysisMacroCommand(int id, const QString& com)
{
  QString descriptor=QString("/AnalysisMacro%1/Command").arg(id);
  setStr(descriptor, com);
}

void    TGo4QSettings::setAnalysisMacroTip(int id, const QString& com)
{
  QString descriptor=QString("/AnalysisMacro%1/Tip").arg(id);
  setStr(descriptor, com);
}

double  TGo4QSettings::getScreenScaleFactor()
{
  return getDouble("/Screen/ScaleFactor", 1.0);
}
void    TGo4QSettings::setScreenScaleFactor(double factor)
{
  setDouble( "/Screen/ScaleFactor", factor);
}

bool TGo4QSettings::getSavePadWhiteBackground()
{
   return getBool("/ViewPanel/SavePadWhiteBackground", false);
}

void TGo4QSettings::setSavePadWhiteBackground(bool on)
{

   setBool("/ViewPanel/SavePadWhiteBackground", on);
}

bool TGo4QSettings::getTermShowTimestamp()
{
  return getBool("/AnalisysTerminal/ShowTimestamp", false);
}

void TGo4QSettings::setTermShowTimestamp(bool on)
{
  setBool("/AnalisysTerminal/ShowTimestamp", on);
}

QString   TGo4QSettings::getTermTimeFormat()
{
  return getStr( "/AnalisysTerminal/TimeFormat", "yyyy-MM-dd hh mm:ss.zzz");
}

void  TGo4QSettings::setTermTimeFormat(const QString& form)
{
  setStr( "/AnalisysTerminal/TimeFormat", form);
}

