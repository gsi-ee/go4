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

#include "TGo4QSettings.h"

#include "Riostream.h"
#include <stdlib.h>

#include "qdir.h"
#include "qfile.h"
#include "qfont.h"
#include "qapplication.h"
#include "qmainwindow.h"

#include "TGo4Log.h"
#include "TGo4Marker.h"
#include "TGo4Condition.h"
#include "TGo4WorkSpace.h"

const char* fxToolsfile = "/go4toolsrc";
const char* fxSettingsfolder = "/.qt";

bool TGo4QSettings::fgbUSEMYLOCATION = false;
QString TGo4QSettings::fgxSETTINGSLOCATION = "";
QString TGo4QSettings::fgxSETTINGSNAME = "/go4";
QString TGo4QSettings::fgxHOMESETTINGSNAME = "/go4";

TGo4QSettings* go4sett = 0;


TGo4QSettings::TGo4QSettings() : QSettings()
{
   SetSettingsPath();
}

TGo4QSettings::TGo4QSettings(Format format) : QSettings(format)
{
   SetSettingsPath();
}


TGo4QSettings::~TGo4QSettings()
{
}

void TGo4QSettings::SetToCurrentDir(bool on)
{
   fgbUSEMYLOCATION=on;
   if(on) {
      fgxSETTINGSLOCATION = QDir::currentDirPath()+ "/.go4";
      fgxSETTINGSNAME = fgxHOMESETTINGSNAME+"local";
   } else
      fgxSETTINGSNAME = fgxHOMESETTINGSNAME;
}


void TGo4QSettings::DumpSettingsLocation()
{
   if(fgbUSEMYLOCATION)
      cout <<"Using Qt Settings at "<<fgxSETTINGSLOCATION+fgxSETTINGSNAME+"rc" << endl;
   else
      cout <<"Using Qt Settings at "<<QDir::homeDirPath()+ "/.qt"+fgxSETTINGSNAME+"rc"<<endl;
}

void TGo4QSettings::SetSettingsPath()
{
   if(fgbUSEMYLOCATION)
      insertSearchPath( QSettings::Unix, fgxSETTINGSLOCATION);
}



// set of Go4 specific functions

void TGo4QSettings::setBasicSettings()
{
   writeEntry(GetSettingsName()+"/Font", QApplication::font().toString() );

   writeEntry(GetSettingsName()+"/markers/Markerlabel", (int) TGo4Marker::fgbHASLABEL);
   writeEntry(GetSettingsName()+"/markers/Connectorline", (int) TGo4Marker::fgbHASCONNECTOR);
   writeEntry(GetSettingsName()+"/markers/ShowX", (int) TGo4Marker::fgbXDRAW);
   writeEntry(GetSettingsName()+"/markers/ShowY", (int) TGo4Marker::fgbYDRAW);
   writeEntry(GetSettingsName()+"/markers/ShowXbin", (int) TGo4Marker::fgbXBINDRAW);
   writeEntry(GetSettingsName()+"/markers/ShowYbin", (int)TGo4Marker::fgbYBINDRAW);
   writeEntry(GetSettingsName()+"/markers/ShowBinContent", (int) TGo4Marker::fgbCONTDRAW);

   writeEntry(GetSettingsName()+"/conditions/Conditionlabel", (int) TGo4Condition::fgbLABELDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowLimits", (int) TGo4Condition::fgbLIMITSDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowIntegral", (int) TGo4Condition::fgbINTDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowXmean", (int) TGo4Condition::fgbXMEANDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowYmean", (int) TGo4Condition::fgbYMEANDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowXrms", (int) TGo4Condition::fgbXRMSDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowYrms", (int) TGo4Condition::fgbYRMSDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowXmax", (int) TGo4Condition::fgbXMAXDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowYmax", (int) TGo4Condition::fgbYMAXDRAW);
   writeEntry(GetSettingsName()+"/conditions/ShowCmax", (int) TGo4Condition::fgbCMAXDRAW);
}

void TGo4QSettings::getBasicSettings()
{
//   QString AppStyle = readEntry(GetSettingsName()+"/Style/AppStyle", "windows");
//   MakeStyleSlot(AppStyle);

   QFont font;
   font.fromString(readEntry(GetSettingsName()+"/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
   QApplication::setFont(font, true );

   TGo4Marker::fgbHASLABEL     = readNumEntry(GetSettingsName()+"/markers/Markerlabel", 1);
   TGo4Marker::fgbHASCONNECTOR = readNumEntry(GetSettingsName()+"/markers/Connectorline", 1);;
   TGo4Marker::fgbXDRAW        = readNumEntry(GetSettingsName()+"/markers/ShowX", 1);
   TGo4Marker::fgbYDRAW        = readNumEntry(GetSettingsName()+"/markers/ShowY", 0);
   TGo4Marker::fgbXBINDRAW     = readNumEntry(GetSettingsName()+"/markers/ShowXbin", 0);
   TGo4Marker::fgbYBINDRAW     = readNumEntry(GetSettingsName()+"/markers/ShowYbin", 0);
   TGo4Marker::fgbCONTDRAW     = readNumEntry(GetSettingsName()+"/markers/ShowBinContent", 1);

   TGo4Condition::fgbLABELDRAW = readNumEntry(GetSettingsName()+"/conditions/Conditionlabel", 1);
   TGo4Condition::fgbLIMITSDRAW= readNumEntry(GetSettingsName()+"/conditions/ShowLimits", 1);
   TGo4Condition::fgbINTDRAW   = readNumEntry(GetSettingsName()+"/conditions/ShowIntegral", 1);
   TGo4Condition::fgbXMEANDRAW = readNumEntry(GetSettingsName()+"/conditions/ShowXmean", 1);
   TGo4Condition::fgbYMEANDRAW = readNumEntry(GetSettingsName()+"/conditions/ShowYmean", 0);
   TGo4Condition::fgbXRMSDRAW  = readNumEntry(GetSettingsName()+"/conditions/ShowXrms", 1);
   TGo4Condition::fgbYRMSDRAW  = readNumEntry(GetSettingsName()+"/conditions/ShowYrms", 0);
   TGo4Condition::fgbXMAXDRAW  = readNumEntry(GetSettingsName()+"/conditions/ShowXmax", 1);
   TGo4Condition::fgbYMAXDRAW  = readNumEntry(GetSettingsName()+"/conditions/ShowYmax", 0);
   TGo4Condition::fgbCMAXDRAW  = readNumEntry(GetSettingsName()+"/conditions/ShowCmax", 1);
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

void TGo4QSettings::setClientExeMode(int mode)
{
   setInt("/ClientSetting/ExeMode", mode);
}

int TGo4QSettings::getClientExeMode()
{
   return getInt("/ClientSetting/ExeMode", 0);
}

void TGo4QSettings::setClientExec(const QString& v)
{
   if (getClientExeMode()==0)
      writeEntry( GetSettingsName()+"/ClientSetting/Exec", v);
   else
      writeEntry( GetSettingsName()+"/ClientSetting/Lib", v);
}

QString TGo4QSettings::getClientExec(int mode)
{
   if (mode<0) mode = getClientExeMode();

   return (mode==0) ? readEntry( GetSettingsName()+"/ClientSetting/Exec", "./MainUserAnalysis") :
                      readEntry( GetSettingsName()+"/ClientSetting/Lib", "libGo4UserAnalysis");
}

void TGo4QSettings::setClientArgs(const QString& v)
{
   writeEntry(GetSettingsName() + "/ClientSetting/Args", v);
}

QString TGo4QSettings::getClientArgs()
{
   return readEntry( GetSettingsName() + "/ClientSetting/Args", "");
}

void TGo4QSettings::setClientShellMode(int v)
{
   QString res = "ssh";
   switch(v) {
      case 0: res = "exec"; break;
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
   QString fn = getSettingsFileName();
   QFile f( fn );
   if (f.open(IO_ReadOnly)) {
     QTextStream ts( &f );
     ts >> *tgt;
     f.close();
     cout <<"Loaded Go4 toolbar settings from "<<fn << endl;
   } else {
     QString fndefault = TGo4Log::subGO4SYS("qt3/etc/go4defaulttoolsrc.txt").Data();
     QFile fdefault( fndefault );
     if (fdefault.open( IO_ReadOnly )) {
       QTextStream ts( &fdefault );
       ts >> *tgt;
       fdefault.close();
       cout <<"Loaded Go4 toolbar settings from "<<fndefault << endl;
     } else
       cout <<"Could not load default toolbar settings from "<<fndefault << endl;
   }
}

void TGo4QSettings::StoreSettings(QMainWindow* src)
{
   QString fn = getSettingsFileName();
   QFile f( fn );
   f.open( IO_WriteOnly );
   QTextStream ts( &f );
   ts << *src;
   f.close();
   cout <<"Saved Go4 toolbar settings to " << fn << endl;
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

   if (rect.height() > TGo4WorkSpace::Instance()->height()*4/5)
      rect.setHeight(TGo4WorkSpace::Instance()->height()*4/5);

   if (rect.width() > TGo4WorkSpace::Instance()->width()*4/5)
      rect.setWidth(TGo4WorkSpace::Instance()->width()*4/5);

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
   writeEntry( GetSettingsName()+"/DabcMonitor/BackwardsTrending", on ? 1: 0);
}

bool    TGo4QSettings::getDabcMonitorBackwardsTrending()
{
   return readNumEntry( GetSettingsName()+"/DabcMonitor/BackwardsTrending", 1) != 0;
}




void    TGo4QSettings::setBool(const QString& name, bool value)
{
   writeEntry( GetSettingsName()+name, value ? 1: 0);
}

bool    TGo4QSettings::getBool(const QString& name, bool def)
{
   return readNumEntry( GetSettingsName()+name, def ? 1 : 0) != 0;
}

void    TGo4QSettings::setInt(const QString& name, int value)
{
   writeEntry( GetSettingsName()+name, value);
}

int     TGo4QSettings::getInt(const QString& name, int def)
{
   return readNumEntry( GetSettingsName()+name, def);
}

void    TGo4QSettings::setStr(const QString& name, const QString& value)
{
   writeEntry( GetSettingsName()+name, value);
}

QString TGo4QSettings::getStr(const QString& name, const QString& def)
{
   return readEntry( GetSettingsName()+name, def);
}


QStringList TGo4QSettings::getCommandsHistoryGUI()
{
  return readListEntry( GetSettingsName()+"/CommandsHistoryGUI");
}

void TGo4QSettings::setCommandsHistoryGUI(QStringList & commands)
{
   writeEntry( GetSettingsName()+"/CommandsHistoryGUI", commands);

}

