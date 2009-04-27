#include "TGo4QSettings.h"

#include "Riostream.h"
#include <stdlib.h>

#include "qdir.h"
#include <QtCore/QFile>
#include "qfont.h"
#include "qapplication.h"
#include <QtGui/QMainWindow>

#include "TGo4Marker.h"
#include "TGo4Condition.h"

const char* fxToolsfile = "/go4toolsrc";
const char* fxSettingsfolder = "/.config/GSI";
const char* fxOrganisation = "GSI";
const char* fxApplication = "go4";

bool TGo4QSettings::fgbUSEMYLOCATION = false;
QString TGo4QSettings::fgxSETTINGSLOCATION = "";
QString TGo4QSettings::fgxSETTINGSNAME = "/go4";

TGo4QSettings* go4sett = 0;


TGo4QSettings::TGo4QSettings() : QSettings(fxOrganisation,fxApplication)
{
}

TGo4QSettings::TGo4QSettings(Format format) : QSettings(format, QSettings::UserScope, fxOrganisation, fxApplication)
{
}


TGo4QSettings::~TGo4QSettings()
{
}

void TGo4QSettings::SetToCurrentDir(bool on)
{
   fgbUSEMYLOCATION=on;
   if(on) {
      fgxSETTINGSLOCATION = QDir::currentDirPath()+ "/.config/GSI";
   }
}


void TGo4QSettings::DumpSettingsLocation()
{
cout <<"Using Qt Settings at "<<fileName().ascii() <<" ."<<endl;
}

void TGo4QSettings::setBool(const QString& name, bool value)
{
   setValue( GetSettingsName()+name, QString(value ? "true" : "false"));
}

bool TGo4QSettings::getBool(const QString& name, bool def)
{
   return value( GetSettingsName()+name, QString(def ? "true" : "false")).toString() == QString("true");
}

void TGo4QSettings::setInt(const QString& name, int value)
{
   setValue(GetSettingsName()+name, value);
}

int TGo4QSettings::getInt(const QString& name, int def)
{
   return value( GetSettingsName()+name, def).toInt();
}

void TGo4QSettings::setStr(const QString& name, const QString& value)
{
   setValue( GetSettingsName()+name, value);
}

QString TGo4QSettings::getStr(const QString& name, const QString& def)
{
   return value( GetSettingsName()+name, def).toString();
}


// set of Go4 specific functions

void TGo4QSettings::setBasicSettings()
{
   //writeEntry(GetSettingsName()+"/Font", QApplication::font().toString() );

   setBool("/markers/Markerlabel", TGo4Marker::fgbHASLABEL);
   setBool("/markers/Connectorline", TGo4Marker::fgbHASCONNECTOR);
   setBool("/markers/ShowX", TGo4Marker::fgbXDRAW);
   setBool("/markers/ShowY", TGo4Marker::fgbYDRAW);
   setBool("/markers/ShowXbin", TGo4Marker::fgbXBINDRAW);
   setBool("/markers/ShowYbin", TGo4Marker::fgbYBINDRAW);
   setBool("/markers/ShowBinContent", TGo4Marker::fgbCONTDRAW);

   setBool("/conditions/Conditionlabel", TGo4Condition::fgbLABELDRAW);
   setBool("/conditions/ShowLimits", TGo4Condition::fgbLIMITSDRAW);
   setBool("/conditions/ShowIntegral", TGo4Condition::fgbINTDRAW);
   setBool("/conditions/ShowXmean", TGo4Condition::fgbXMEANDRAW);
   setBool("/conditions/ShowYmean", TGo4Condition::fgbYMEANDRAW);
   setBool("/conditions/ShowXrms", TGo4Condition::fgbXRMSDRAW);
   setBool("/conditions/ShowYrms", TGo4Condition::fgbYRMSDRAW);
   setBool("/conditions/ShowXmax", TGo4Condition::fgbXMAXDRAW);
   setBool("/conditions/ShowYmax", TGo4Condition::fgbYMAXDRAW);
   setBool("/conditions/ShowCmax", TGo4Condition::fgbCMAXDRAW);
}

void TGo4QSettings::getBasicSettings()
{
//   QString AppStyle = readEntry(GetSettingsName()+"/Style/AppStyle", "windows");
//   QApplication::setStyle(AppStyle);

//   QFont font;
//   font.fromString(readEntry(GetSettingsName()+"/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
//   QApplication::setFont(font, true );

   TGo4Marker::fgbHASLABEL     = getBool("/markers/Markerlabel", 1);
   TGo4Marker::fgbHASCONNECTOR = getBool("/markers/Connectorline", 1);;
   TGo4Marker::fgbXDRAW        = getBool("/markers/ShowX", 1);
   TGo4Marker::fgbYDRAW        = getBool("/markers/ShowY", 0);
   TGo4Marker::fgbXBINDRAW     = getBool("/markers/ShowXbin", 0);
   TGo4Marker::fgbYBINDRAW     = getBool("/markers/ShowYbin", 0);
   TGo4Marker::fgbCONTDRAW     = getBool("/markers/ShowBinContent", 1);

   TGo4Condition::fgbLABELDRAW = getBool("/conditions/Conditionlabel", 1);
   TGo4Condition::fgbLIMITSDRAW= getBool("/conditions/ShowLimits", 1);
   TGo4Condition::fgbINTDRAW   = getBool("/conditions/ShowIntegral", 1);
   TGo4Condition::fgbXMEANDRAW = getBool("/conditions/ShowXmean", 1);
   TGo4Condition::fgbYMEANDRAW = getBool("/conditions/ShowYmean", 0);
   TGo4Condition::fgbXRMSDRAW  = getBool("/conditions/ShowXrms", 1);
   TGo4Condition::fgbYRMSDRAW  = getBool("/conditions/ShowYrms", 0);
   TGo4Condition::fgbXMAXDRAW  = getBool("/conditions/ShowXmax", 1);
   TGo4Condition::fgbYMAXDRAW  = getBool("/conditions/ShowYmax", 0);
   TGo4Condition::fgbCMAXDRAW  = getBool("/conditions/ShowCmax", 1);
}


void  TGo4QSettings::setAppFont(const QFont& font)
{
   writeEntry(GetSettingsName()+"/Font", font.toString() );
}

void TGo4QSettings::getAppFont()
{
 QFont font;
 font.fromString(readEntry(GetSettingsName()+"/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
 QApplication::setFont(font, true );
}



void TGo4QSettings::setAppStyle(const QString& v)
{
   writeEntry(GetSettingsName()+"/Style/AppStyle", v);
}

QString TGo4QSettings::getAppStyle()
{
   return readEntry(GetSettingsName()+"/Style/AppStyle", "windows");
}

void TGo4QSettings::setClientName(const QString& v)
{
   writeEntry(GetSettingsName()+"/ClientSetting/Name", v);
}

QString TGo4QSettings::getClientName()
{
   return readEntry(GetSettingsName()+"/ClientSetting/Name", "MyAnalysis");
}

void TGo4QSettings::setClientNode(const QString& v)
{
   writeEntry( GetSettingsName()+"/ClientSetting/Node", v);
}

QString TGo4QSettings::getClientNode()
{
   return readEntry(GetSettingsName()+"/ClientSetting/Node", "localhost");
}

void TGo4QSettings::setClientDir(const QString& v)
{
   writeEntry( GetSettingsName()+"/ClientSetting/Dir", v);
}

QString TGo4QSettings::getClientDir()
{
   return readEntry(GetSettingsName()+"/ClientSetting/Dir", QDir::currentDirPath());
}

void TGo4QSettings::setClientExec(const QString& v)
{
   writeEntry( GetSettingsName()+"/ClientSetting/Exec", v);
}

QString TGo4QSettings::getClientExec()
{
   return readEntry( GetSettingsName()+"/ClientSetting/Exec", "MainUserAnalysis");
}

void TGo4QSettings::setClientShellMode(int v)
{
   QString res = "ssh";
   switch(v) {
      case 1: res = "rsh"; break;
      case 2: res = "ssh"; break;
   }
   writeEntry( GetSettingsName()+"/ClientSetting/Shell", res);
}

QString TGo4QSettings::getClientShell()
{
   return readEntry( GetSettingsName()+"/ClientSetting/Shell", "ssh");
}

int TGo4QSettings::getClientShellMode()
{
   QString v = getClientShell();
   int shellmode = 2;
   if(v.contains("rsh")) shellmode=1; else
   if(v.contains("ssh")) shellmode=2;
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
   writeEntry( GetSettingsName()+"/ClientSetting/Term", res);
}

QString TGo4QSettings::getClientTerm()
{
   return readEntry( GetSettingsName()+"/ClientSetting/Term", "qt");
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

void  TGo4QSettings::setClientIsServer(bool on)
{
   writeEntry(GetSettingsName()+"/ClientSetting/IsServer", on ? "on" : "off");
}

bool TGo4QSettings::getClientIsServer()
{
   QString value = readEntry(GetSettingsName()+"/ClientSetting/IsServer", "off");
   return  value == QString("on");
}

void TGo4QSettings::setClientPort(int nport)
{
   writeEntry(GetSettingsName()+"/ClientSetting/ClientPort", nport);
}

int TGo4QSettings::getClientPort()
{
   return readNumEntry(GetSettingsName()+"/ClientSetting/ClientPort", 5000);
}

void TGo4QSettings::setClientControllerMode(int mode)
{
   writeEntry(GetSettingsName()+"/ClientSetting/Controller", mode);
}

int TGo4QSettings::getClientControllerMode()
{
   return readNumEntry(GetSettingsName()+"/ClientSetting/Controller", 0);
}

void TGo4QSettings::setClientDefaultPass(bool on)
{
   writeEntry(GetSettingsName()+"/ClientSetting/DefaultPass", on ? "on" : "off");
}

bool TGo4QSettings::getClientDefaultPass()
{
   QString value = readEntry(GetSettingsName()+"/ClientSetting/DefaultPass", "on");
   return value == QString("on");
}

void TGo4QSettings::setTermHistorySize(int sz)
{
   writeEntry(GetSettingsName()+"/AnalisysTerminal/HistorySize", sz);
}

int TGo4QSettings::getTermHistorySize()
{
   return readNumEntry(GetSettingsName()+"/AnalisysTerminal/HistorySize", 100000);
}

void TGo4QSettings::setHServName(const QString& v)
{
   writeEntry(GetSettingsName()+"/HistogramClient/server", v);
}

QString TGo4QSettings::getHServName()
{
   return readEntry(GetSettingsName()+"/HistogramClient/server", "");
}

void TGo4QSettings::setHServBase(const QString& v)
{
   writeEntry(GetSettingsName()+"/HistogramClient/base", v);
}

QString TGo4QSettings::getHServBase()
{
   return readEntry(GetSettingsName()+"/HistogramClient/base", "");
}

void TGo4QSettings::setHServFilter(const QString& v)
{
   writeEntry(GetSettingsName()+"/HistogramClient/filter", v);
}

QString TGo4QSettings::getHServFilter()
{
   return readEntry(GetSettingsName()+"/HistogramClient/filter", "*");
}

void TGo4QSettings::setHServPort(int port)
{
   writeEntry(GetSettingsName()+"/HistogramClient/port", port);
}

int TGo4QSettings::getHServPort()
{
   return readNumEntry(GetSettingsName()+"/HistogramClient/port", 6009);
}


void TGo4QSettings::setPadCrosshair(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/Crosshairmode", on ? 1: 0);
}

bool TGo4QSettings::getPadCrosshair()
{
   return readNumEntry( GetSettingsName()+"/geometry/Crosshairmode", 0) != 0;
}

void TGo4QSettings::setPadEventStatus(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/Eventstatusmode", on ? 1: 0);
}

bool TGo4QSettings::getPadEventStatus()
{
   return readNumEntry( GetSettingsName()+"/geometry/Eventstatusmode", 0) != 0;
}

void TGo4QSettings::setCanvasColor(int red, int green, int blue)
{
   writeEntry(GetSettingsName()+"/geometry/ViewPanelBackRed", red );
   writeEntry(GetSettingsName()+"/geometry/ViewPanelBackGrn", green);
   writeEntry(GetSettingsName()+"/geometry/ViewPanelBackBlu", blue );
}

void TGo4QSettings::getCanvasColor(int& red, int& green, int& blue)
{
   red   = readNumEntry(GetSettingsName()+"/geometry/ViewPanelBackRed", 220);
   green = readNumEntry(GetSettingsName()+"/geometry/ViewPanelBackGrn", 220);
   blue  = readNumEntry(GetSettingsName()+"/geometry/ViewPanelBackBlu", 220);
}

void TGo4QSettings::setCloneFlag(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/CloneFlag", on ? 1: 0);
}

bool TGo4QSettings::getCloneFlag()
{
   return readNumEntry( GetSettingsName()+"/geometry/CloneFlag", 1);
}

void TGo4QSettings::setDrawTimeFlag(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/DrawTime", on ? 1: 0);
}

bool TGo4QSettings::getDrawTimeFlag()
{
   return readNumEntry( GetSettingsName()+"/geometry/DrawTime", 1);
}

void TGo4QSettings::setDrawDateFlag(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/DrawDate", on ? 1: 0);
}

bool TGo4QSettings::getDrawDateFlag()
{
   return readNumEntry( GetSettingsName()+"/geometry/DrawDate", 0);
}

void TGo4QSettings::setDrawItemFlag(bool on)
{
   writeEntry( GetSettingsName()+"/geometry/DrawItem", on ? 1: 0);
}

bool TGo4QSettings::getDrawItemFlag()
{
   return readNumEntry( GetSettingsName()+"/geometry/DrawItem", 0);
}

QString TGo4QSettings::getSettingsFileName()
{
   if(UsesUserPath())
      return GetUserPath() + fxToolsfile;
   else
      return QDir::homeDirPath() + fxSettingsfolder+ fxToolsfile;
}

void TGo4QSettings::RestoreSettings(QMainWindow* tgt)
{
   tgt->restoreState(value(GetSettingsName()+"/MainWindow").toByteArray());
}

void TGo4QSettings::StoreSettings(QMainWindow* src)
{
   setValue(GetSettingsName()+"/MainWindow", src->saveState());
}

void TGo4QSettings::storeGeometry(QMainWindow* w)
{
   writeEntry(GetSettingsName()+"/geometry/MainWinWidth", w->width() );
   writeEntry(GetSettingsName()+"/geometry/MainWinHeight", w->height() );
   writeEntry(GetSettingsName()+"/geometry/MainWinX", w->x() );
   writeEntry(GetSettingsName()+"/geometry/MainWinY", w->y() );
}

void TGo4QSettings::restoreGeometry(QMainWindow* w)
{
   int Width, Height, PosX, PosY;
   Width  = readNumEntry(GetSettingsName()+"/geometry/MainWinWidth", 997 );
   Height = readNumEntry(GetSettingsName()+"/geometry/MainWinHeight", 690 );
   PosX   = readNumEntry(GetSettingsName()+"/geometry/MainWinX", 129 );
   PosY   = readNumEntry(GetSettingsName()+"/geometry/MainWinY", 132 );
   w->resize( Width, Height );
   w->move( PosX, PosY );
}

void TGo4QSettings::storePanelSize(QWidget* w)
{
   writeEntry(GetSettingsName()+"/geometry/ViewPanelWidth", w->width() );
   writeEntry(GetSettingsName()+"/geometry/ViewPanelHeight", w->height() );
}

void TGo4QSettings::restorePanelSize(QWidget* w)
{
   QSize rect;
   rect.setWidth(readNumEntry(GetSettingsName()+"/geometry/ViewPanelWidth", 300));
   rect.setHeight(readNumEntry(GetSettingsName()+"/geometry/ViewPanelHeight", 300));
   w->resize(rect);
}

void TGo4QSettings::setBrowserColumn(const char* name, int width)
{
   writeEntry(GetSettingsName()+"/Browser/" + name, width);
}

int TGo4QSettings::getBrowserColumn(const char* name, int defwidth)
{
   return readNumEntry(GetSettingsName()+"/Browser/" + name, defwidth);
}


void TGo4QSettings::setOptStat(int value)
{
   writeEntry(GetSettingsName()+"/HistStatsBox/OptStats", value);
}

int TGo4QSettings::getOptStat()
{
   return readNumEntry(GetSettingsName()+"/HistStatsBox/OptStats", 11111111);
}

void TGo4QSettings::setStatBoxVisible(bool on)
{
   writeEntry(GetSettingsName()+"/HistStatsBox/Visible", on ? 1: 0);
}

bool TGo4QSettings::getStatBoxVisible()
{
   return readNumEntry(GetSettingsName()+"/HistStatsBox/Visible", 1);
}

void TGo4QSettings::setStatBoxErrors(bool on)
{
   writeEntry(GetSettingsName()+"/HistStatsBox/Errors", on ? 1: 0);
}

bool TGo4QSettings::getStatBoxErrors()
{
   return readNumEntry(GetSettingsName()+"/HistStatsBox/Errors", 0);
}


void  TGo4QSettings::setHistName(const QString& value)
{
   writeEntry(GetSettingsName()+"/NewHist/Name", value);
}

QString TGo4QSettings::getHistName()
{
   return readEntry(GetSettingsName()+"/NewHist/Name", "histo1");
}

void TGo4QSettings::setHistType(int value)
{
   writeEntry(GetSettingsName()+"/NewHist/HType", value);
}

int TGo4QSettings::getHistType()
{
   return readNumEntry(GetSettingsName()+"/NewHist/HType", 11);
}




void TGo4QSettings::setHistAxisPars(int naxis, int npoints, double min, double max)
{
   QString sname = GetSettingsName()+"/NewHist/Axis";
   switch (naxis) {
      case 0: sname+="X_"; break;
      case 1: sname+="Y_"; break;
      case 2: sname+="Z_"; break;
      default: sname+="X_"; break;
   }
   writeEntry(sname+"NPoints", npoints);
   writeEntry(sname+"Min", min);
   writeEntry(sname+"Max", max);
}

void TGo4QSettings::getHistAxisPars(int naxis, int& npoints, double& min, double& max)
{
   QString sname = GetSettingsName()+"/NewHist/Axis";
   switch (naxis) {
      case 0: sname+="X_"; break;
      case 1: sname+="Y_"; break;
      case 2: sname+="Z_"; break;
      default: sname+="X_"; break;
   }
   npoints = readDoubleEntry(sname+"NPoints", 100);
   min = readDoubleEntry(sname+"Min", 0.);
   max = readDoubleEntry(sname+"Max", 100.);
}

void TGo4QSettings::setFetchDataWhenDraw(bool on)
{
   writeEntry( GetSettingsName()+"/preferences/FetchWhenDraw", on ? 1: 0);
}

bool TGo4QSettings::getFetchDataWhenDraw()
{
   return readNumEntry( GetSettingsName()+"/preferences/FetchWhenDraw", 1) != 0;
}

void TGo4QSettings::setFetchDataWhenCopy(bool on)
{
   writeEntry( GetSettingsName()+"/preferences/FetchWhenCopy", on ? 1: 0);
}

bool TGo4QSettings::getFetchDataWhenCopy()
{
   return readNumEntry( GetSettingsName()+"/preferences/FetchWhenCopy", 0) != 0;
}

void TGo4QSettings::setFetchDataWhenSave(bool on)
{
   writeEntry( GetSettingsName()+"/preferences/FetchWhenSave", on ? 1 : 0);
}

bool TGo4QSettings::getFetchDataWhenSave()
{
   return readNumEntry( GetSettingsName()+"/preferences/FetchWhenSave", 0) != 0;
}

void TGo4QSettings::setRemoteFileSett(const QString& hostname, const QString& filename, const QString& protocol)
{
   writeEntry(GetSettingsName()+"/OpenRemoteFile/Host", hostname);
   writeEntry(GetSettingsName()+"/OpenRemoteFile/File", filename);
   writeEntry(GetSettingsName()+"/OpenRemoteFile/Type", protocol);
}

void TGo4QSettings::getRemoteFileSett(QString& hostname, QString& filename, QString& protocol)
{
   hostname = readEntry(GetSettingsName()+"/OpenRemoteFile/Host", "machine.gsi.de");
   filename = readEntry(GetSettingsName()+"/OpenRemoteFile/File", "tmp/hsimple.root");
   protocol = readEntry(GetSettingsName()+"/OpenRemoteFile/Type", "root:");
}

void TGo4QSettings::setPrinterSett(const QString& name, const QString& cmd)
{
   writeEntry(GetSettingsName()+"/Printer", name);
   writeEntry(GetSettingsName()+"/PrintProg", cmd);
}

void TGo4QSettings::getPrinterSett(QString& name, QString& cmd)
{
   name = readEntry(GetSettingsName()+"/Printer", getenv("PRINTER"));
   cmd  = readEntry(GetSettingsName()+"/PrintProg","lpr");
}

void    TGo4QSettings::setMbsMonitorNode(const QString& name)
{
   writeEntry(GetSettingsName()+"/MbsMonitor/Node", name);
}

QString TGo4QSettings::getMbsMonitorNode()
{
    return readEntry(GetSettingsName()+"/MbsMonitor/Node", "r2-d2");
}

void    TGo4QSettings::setMbsMonitorFreq(int secs)
{
   writeEntry(GetSettingsName()+"/MbsMonitor/Frequency", secs);
}

int     TGo4QSettings::getMbsMonitorFreq()
{
   return readNumEntry(GetSettingsName()+"/MbsMonitor/Frequency", 5);
}

void    TGo4QSettings::setMbsMonitorBins(int num)
{
   writeEntry(GetSettingsName()+"/MbsMonitor/Bins", num);
}

int     TGo4QSettings::getMbsMonitorBins()
{
   return readNumEntry(GetSettingsName()+"/MbsMonitor/Bins", 200);
}

void    TGo4QSettings::setMbsMonitorTrend(bool on)
{
   writeEntry( GetSettingsName()+"/MbsMonitor/TrendOn", on ? 1: 0);
}

bool    TGo4QSettings::getMbsMonitorTrend()
{
    return readNumEntry( GetSettingsName()+"/MbsMonitor/TrendOn", 0) != 0;
}

void    TGo4QSettings::setMbsMonitorMore(bool on)
{
   writeEntry( GetSettingsName()+"/MbsMonitor/ShowMore", on ? 1: 0);
}

bool    TGo4QSettings::getMbsMonitorMore()
{
   return readNumEntry( GetSettingsName()+"/MbsMonitor/ShowMore", 1) != 0;
}

void    TGo4QSettings::setMbsMonitorMonitorActive(bool on)
{
   writeEntry( GetSettingsName()+"/MbsMonitor/MonitorActive", on ? 1: 0);
}

bool    TGo4QSettings::getMbsMonitorMonitorActive()
{
   return readNumEntry( GetSettingsName()+"/MbsMonitor/MonitorActive", 0) != 0;
}

void    TGo4QSettings::setMbsMonitorBackwardsTrending(bool on)
{
   writeEntry( GetSettingsName()+"/MbsMonitor/BackwardsTrending", on ? 1: 0);
}

bool    TGo4QSettings::getMbsMonitorBackwardsTrending()
{
   return readNumEntry( GetSettingsName()+"/MbsMonitor/BackwardsTrending", 1) != 0;
}


void    TGo4QSettings::setDabcMonitorNode(const QString& name)
{
   writeEntry(GetSettingsName()+"/DabcMonitor/Node", name);
}

QString TGo4QSettings::getDabcMonitorNode()
{
    return readEntry(GetSettingsName()+"/DabcMonitor/Node", "dimDns.gsi.de");
}

void    TGo4QSettings::setDabcMonitorFreq(int secs)
{
   writeEntry(GetSettingsName()+"/DabcMonitor/Frequency", secs);
}

int     TGo4QSettings::getDabcMonitorFreq()
{
   return readNumEntry(GetSettingsName()+"/DabcMonitor/Frequency", 5);
}

void    TGo4QSettings::setDabcMonitorBins(int num)
{
   writeEntry(GetSettingsName()+"/DabcMonitor/Bins", num);
}

int     TGo4QSettings::getDabcMonitorBins()
{
   return readNumEntry(GetSettingsName()+"/DabcMonitor/Bins", 200);
}

void    TGo4QSettings::setDabcMonitorBackwardsTrending(bool on)
{
   setBool("/DabcMonitor/BackwardsTrending", on);
}

bool TGo4QSettings::getDabcMonitorBackwardsTrending()
{
   return getBool("/DabcMonitor/BackwardsTrending", true);
}

QStringList TGo4QSettings::getCommandsHistoryGUI()
{
  return readListEntry( GetSettingsName()+"/CommandsHistoryGUI");
}

void TGo4QSettings::setCommandsHistoryGUI(QStringList & commands)
{
   writeEntry( GetSettingsName()+"/CommandsHistoryGUI", commands);

}

