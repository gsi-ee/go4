#include "TGo4QSettings.h"

#include <QtCore/QSettings>

#include "Riostream.h"
#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtGui/QFont>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

#include "TGo4Marker.h"
#include "TGo4Condition.h"
#include "TGo4WorkSpace.h"

QString TGo4QSettings::fgSettFileName = "";

const char* MainWindowDefaultState = "000000ff00000000fd00000002000000000000012500000264fc0200000001fb0000001600420072006f00770073006500720044006f0063006b010000005b000002640000005600ffffff000000030000048000000082fc0100000001fc00000000000004800000007f00fffffffa000000000200000003fb00000016004c006f00670049006e0066006f0044006f0063006b0100000000ffffffff0000005400fffffffb0000001a004d006200730056006900650077006500720044006f0063006b0000000000ffffffff0000006600fffffffb0000001e0044004100420043004d006f006e00690074006f00720044006f0063006b0000000279000000ce000000b200ffffff000003550000026400000004000000040000000800000008fc000000080000000200000004000000160047006f00340054006f006f006c007300420061007201000000000000016600000000000000000000001600460069006c00650054006f006f006c0042006100720100000166000000f800000000000000000000001e0041006e0061006c00790073006900730054006f006f006c004200610072010000025e0000013e00000000000000000000001a0044006900760069006400650050006100640044006f0063006b010000039c000000e40000000000000000000000020000000000000002000000020000002400420072006f0077007300650072004f007000740069006f006e00730044006f0063006b01000000000000010200000000000000000000001e0044007200610077004f007000740069006f006e00730044006f0063006b01000001020000037e00000000000000000000000200000002000000240048006900730044007200610077004f007000740069006f006e00730044006f0063006b0000000000000001290000000000000000000000180047006f0034005300740079006c00650044006f0063006b0000000000000004b10000000000000000000000020000000100000020005300630061006c0065004f007000740069006f006e00730044006f0063006b0000000000000004b10000000000000000000000030000000000000003000000010000001e0043006f006d006d0061006e0064004c0069006e00650044006f0063006b0000000000000004b1000000000000000000000003000000010000001c00540072006500650056006900650077006500720044006f0063006b0000000000000004b10000000000000000";

TGo4QSettings* go4sett = 0;

void TGo4QSettings::SetSettLocation(const QString& filename)
{
   fgSettFileName = filename;
}

QString TGo4QSettings::GetSettLoaction()
{
   if (fgSettFileName.length() > 0) return fgSettFileName;
   return QDir::homePath() + "/.config/GSI/go4.conf";
}

TGo4QSettings::TGo4QSettings()
{
   Open();
}

TGo4QSettings::~TGo4QSettings()
{
   delete sett;
}

void TGo4QSettings::Open()
{
   if (fgSettFileName.length()>0)
      sett = new QSettings(fgSettFileName, QSettings::NativeFormat);
   else
      sett = new QSettings("GSI","go4");
}

void TGo4QSettings::Store()
{
   delete sett;
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
   //   QString AppStyle = readEntry( "/Style/AppStyle", "windows");
   //   QApplication::setStyle(AppStyle);

   //   QFont font;
   //   font.fromString(readEntry( "/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
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
   setStr("/Font", font.toString());
}

void TGo4QSettings::getAppFont()
{
   QFont font;
   font.fromString(getStr("/Font", "Arial,11,-1,5,50,0,0,0,0,0"));
   QApplication::setFont(font);
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

void TGo4QSettings::setClientExec(const QString& v)
{
   setStr(  "/ClientSetting/Exec", v);
}

QString TGo4QSettings::getClientExec()
{
   return getStr("/ClientSetting/Exec", "");
}

void TGo4QSettings::setClientShellMode(int v)
{
   const char* name = "ssh";
   switch (v) {
      case 0: name = "exec"; break;
      case 1: name = "rsh"; break;
      case 2: name = "ssh"; break;
   }

   setStr(  "/ClientSetting/Shell", name);
}

QString TGo4QSettings::getClientShell()
{
   return getStr("/ClientSetting/Shell", "ssh");
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
   setStr(  "/ClientSetting/Term", res);
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

void  TGo4QSettings::setClientIsServer(bool on)
{
   setBool("/ClientSetting/IsServer", on);
}

bool TGo4QSettings::getClientIsServer()
{
   return getBool("/ClientSetting/IsServer", false);
}

void TGo4QSettings::setClientExeMode(int mode)
{
   setInt("/ClientSetting/ExeMode", mode);
}

int TGo4QSettings::getClientExeMode()
{
   return getInt("/ClientSetting/ExeMode", 0);
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
   setInt( "/ClientSetting/Controller", mode);
}

int TGo4QSettings::getClientControllerMode()
{
   return getInt( "/ClientSetting/Controller", 0);
}

void TGo4QSettings::setClientDefaultPass(bool on)
{
   setBool( "/ClientSetting/DefaultPass", on);
}

bool TGo4QSettings::getClientDefaultPass()
{
   return getBool( "/ClientSetting/DefaultPass", true);
}

void TGo4QSettings::setTermHistorySize(int sz)
{
   setInt( "/AnalisysTerminal/HistorySize", sz);
}

int TGo4QSettings::getTermHistorySize()
{
   return getInt( "/AnalisysTerminal/HistorySize", 100000);
}

void TGo4QSettings::setHServName(const QString& v)
{
   setStr( "/HistogramClient/server", v);
}

QString TGo4QSettings::getHServName()
{
   return getStr( "/HistogramClient/server", "");
}

void TGo4QSettings::setHServBase(const QString& v)
{
   setStr( "/HistogramClient/base", v);
}

QString TGo4QSettings::getHServBase()
{
   return getStr( "/HistogramClient/base", "");
}

void TGo4QSettings::setHServFilter(const QString& v)
{
   setStr( "/HistogramClient/filter", v);
}

QString TGo4QSettings::getHServFilter()
{
   return getStr( "/HistogramClient/filter", "*");
}

void TGo4QSettings::setHServPort(int port)
{
   setInt( "/HistogramClient/port", port);
}

int TGo4QSettings::getHServPort()
{
   return getInt( "/HistogramClient/port", 6009);
}


void TGo4QSettings::setPadCrosshair(bool on)
{
   setBool(  "/geometry/Crosshairmode", on);
}

bool TGo4QSettings::getPadCrosshair()
{
   return getBool(  "/geometry/Crosshairmode", false);
}

void TGo4QSettings::setPadEventStatus(bool on)
{
   setBool(  "/geometry/Eventstatusmode", on);
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

void TGo4QSettings::setCloneFlag(bool on)
{
   setBool("/ViewPanel/CloneFlag", on);
}

bool TGo4QSettings::getCloneFlag()
{
   return getBool(  "/ViewPanel/CloneFlag", true);
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

void TGo4QSettings::RestoreSettings(QMainWindow* tgt)
{
   tgt->restoreState(sett->value("/MainWindow/State", QByteArray::fromHex(MainWindowDefaultState)).toByteArray());
}

void TGo4QSettings::StoreSettings(QMainWindow* src)
{
   // cout << "res: " << src->saveState().toHex().constData() << endl;

   sett->setValue( "/MainWindow/State", src->saveState());
}

void TGo4QSettings::storeGeometry(QMainWindow* w)
{
   setInt( "/geometry/MainWinWidth", w->width() );
   setInt( "/geometry/MainWinHeight", w->height() );
   setInt( "/geometry/MainWinX", w->x() );
   setInt( "/geometry/MainWinY", w->y() );
}

void TGo4QSettings::restoreGeometry(QMainWindow* w)
{
   int Width, Height, PosX, PosY;
   Width  = getInt( "/geometry/MainWinWidth", 997 );
   Height = getInt( "/geometry/MainWinHeight", 690 );
   PosX   = getInt( "/geometry/MainWinX", 129 );
   PosY   = getInt( "/geometry/MainWinY", 132 );
   w->resize( Width, Height );
   w->move( PosX, PosY );
}

void TGo4QSettings::storePanelSize(QWidget* w)
{
   setInt( "/ViewPanel/Width", w->width() );
   setInt( "/ViewPanel/Height", w->height() );
}

QSize TGo4QSettings::lastPanelSize()
{
   QSize rect(getInt( "/ViewPanel/Width", 450), getInt( "/ViewPanel/Height", 250));

   if (rect.height() > TGo4WorkSpace::Instance()->height()*4/5)
      rect.setHeight(TGo4WorkSpace::Instance()->height()*4/5);

   if (rect.width() > TGo4WorkSpace::Instance()->width()*4/5)
      rect.setWidth(TGo4WorkSpace::Instance()->width()*4/5);

   return rect;
}

void TGo4QSettings::setBrowserColumn(const char* name, int width)
{
   setInt( QString("/Browser/") + name, width);
}

int TGo4QSettings::getBrowserColumn(const char* name, int defwidth)
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
   setStr( "/NewHist/Name", value);
}

QString TGo4QSettings::getHistName()
{
   return getStr( "/NewHist/Name", "histo1");
}

void TGo4QSettings::setHistType(int value)
{
   setInt( "/NewHist/HType", value);
}

int TGo4QSettings::getHistType()
{
   return getInt( "/NewHist/HType", 11);
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

void TGo4QSettings::setRemoteFileSett(const QString& hostname, const QString& filename, const QString& protocol)
{
   setStr( "/OpenRemoteFile/Host", hostname);
   setStr( "/OpenRemoteFile/File", filename);
   setStr( "/OpenRemoteFile/Type", protocol);
}

void TGo4QSettings::getRemoteFileSett(QString& hostname, QString& filename, QString& protocol)
{
   hostname = getStr( "/OpenRemoteFile/Host", "machine.gsi.de");
   filename = getStr( "/OpenRemoteFile/File", "tmp/hsimple.root");
   protocol = getStr( "/OpenRemoteFile/Type", "root:");
}

void TGo4QSettings::setPrinterSett(const QString& name, const QString& cmd)
{
   setStr( "/Printer/Name", name);
   setStr( "/Printer/Prog", cmd);
}

void TGo4QSettings::getPrinterSett(QString& name, QString& cmd)
{
   name = getStr( "/Printer/Name", getenv("PRINTER"));
   cmd  = getStr( "/Printer/Prog","lpr");
}

void    TGo4QSettings::setMbsMonitorNode(const QString& name)
{
   setStr( "/MbsMonitor/Node", name);
}

QString TGo4QSettings::getMbsMonitorNode()
{
   return getStr( "/MbsMonitor/Node", "r2-d2");
}

void    TGo4QSettings::setMbsMonitorFreq(int secs)
{
   setInt( "/MbsMonitor/Frequency", secs);
}

int     TGo4QSettings::getMbsMonitorFreq()
{
   return getInt( "/MbsMonitor/Frequency", 5);
}

void    TGo4QSettings::setMbsMonitorBins(int num)
{
   setInt( "/MbsMonitor/Bins", num);
}

int     TGo4QSettings::getMbsMonitorBins()
{
   return getInt( "/MbsMonitor/Bins", 200);
}

void    TGo4QSettings::setMbsMonitorTrend(bool on)
{
   setBool(  "/MbsMonitor/TrendOn", on);
}

bool    TGo4QSettings::getMbsMonitorTrend()
{
   return getBool("/MbsMonitor/TrendOn", false);
}

void    TGo4QSettings::setMbsMonitorMore(bool on)
{
   setBool("/MbsMonitor/ShowMore", on);
}

bool    TGo4QSettings::getMbsMonitorMore()
{
   return getBool("/MbsMonitor/ShowMore", true);
}

void    TGo4QSettings::setMbsMonitorMonitorActive(bool on)
{
   setBool("/MbsMonitor/MonitorActive", on);
}

bool    TGo4QSettings::getMbsMonitorMonitorActive()
{
   return getBool("/MbsMonitor/MonitorActive", false);
}

void    TGo4QSettings::setMbsMonitorBackwardsTrending(bool on)
{
   setBool("/MbsMonitor/BackwardsTrending", on);
}

bool    TGo4QSettings::getMbsMonitorBackwardsTrending()
{
   return getBool("/MbsMonitor/BackwardsTrending", true);
}

void    TGo4QSettings::setDabcMonitorNode(const QString& name)
{
   setStr( "/DabcMonitor/Node", name);
}

QString TGo4QSettings::getDabcMonitorNode()
{
   return getStr( "/DabcMonitor/Node", "dimDns.gsi.de");
}

void    TGo4QSettings::setDabcMonitorFreq(int secs)
{
   setInt( "/DabcMonitor/Frequency", secs);
}

int     TGo4QSettings::getDabcMonitorFreq()
{
   return getInt( "/DabcMonitor/Frequency", 5);
}

void    TGo4QSettings::setDabcMonitorBins(int num)
{
   setInt( "/DabcMonitor/Bins", num);
}

int     TGo4QSettings::getDabcMonitorBins()
{
   return getInt( "/DabcMonitor/Bins", 200);
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
   return sett->value("/CommandsHistoryGUI").toStringList();
}

void TGo4QSettings::setCommandsHistoryGUI(const QStringList & commands)
{
   sett->setValue("/CommandsHistoryGUI", commands);
}

QStringList TGo4QSettings::getCommandsHistoryAnalysis()
{
   return sett->value("/CommandsHistoryAnalysis").toStringList();
}

void TGo4QSettings::setCommandsHistoryAnalysis(const QStringList & commands)
{
   sett->setValue("/CommandsHistoryAnalysis", commands);
}
