#ifndef TGO4QSETTINGS_H
#define TGO4QSETTINGS_H

/**
 * @author J. Adamczewski, S.Linev
 * @since 03.11.2004 - adjusted for Qt4.4 10-2008 JAM
 */

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFont>

class QMainWindow;
class QWidget;
class QSettings;

/** Extends the usual QSettings by a global setup for the
* location of the settings file. May switch load/save between
* the user home directory, and the current directory */

class TGo4QSettings {
	   void Open();
	public:

		static void SetSettLocation(const QString& filename);

		static QString GetSettLoaction();

		TGo4QSettings();
		virtual ~TGo4QSettings();

		void Store();

		void RestoreSettings(QMainWindow* tgt);

		void StoreSettings(QMainWindow* src);

		// set generic function

		void    setBool(const QString& name, bool value = true);
		bool    getBool(const QString& name, bool def = true);
		void    setInt(const QString& name, int value = 0);
		int     getInt(const QString& name, int def = 0);
		void    setStr(const QString& name, const QString& value = "");
		QString getStr(const QString& name, const QString& value = "");
		void    setDouble(const QString& name, double value = 0.);
		double  getDouble(const QString& name, double value = 0.);

		// set of Go4Specific functions

		void    setBasicSettings();
		void    getBasicSettings();

		void setAppFont(const QFont&);
		void getAppFont();

		void    setAppStyle(const QString&);
		QString getAppStyle();

		void    setClientName(const QString&);
		QString getClientName();
		void    setClientNode(const QString&);
		QString getClientNode();
		void    setClientDir(const QString&);
		QString getClientDir();
		void    setClientExec(const QString&);
		QString getClientExec();
		void    setClientShellMode(int);
		QString getClientShell();
		int     getClientShellMode();
		void    setClientTermMode(int);
		int     getClientTermMode();
		QString getClientTerm();
		void    setClientIsServer(bool on);
		bool    getClientIsServer();
		void    setClientPort(int);
		int     getClientPort();
		void    setClientControllerMode(int);
		int     getClientControllerMode();
		void    setClientDefaultPass(bool);
		bool    getClientDefaultPass();

		void    setTermHistorySize(int sz);
		int     getTermHistorySize();

		void    setHServName(const QString&);
		QString getHServName();
		void    setHServBase(const QString&);
		QString getHServBase();
		void    setHServFilter(const QString&);
		QString getHServFilter();
		void    setHServPort(int port);
		int     getHServPort();

		void    setPadCrosshair(bool on);
		bool    getPadCrosshair();

		void    setPadEventStatus(bool on);
		bool    getPadEventStatus();

		void    setCanvasColor(int red, int green, int blue);
		void    getCanvasColor(int& red, int& green, int& blue);

		void    setOptStat(int value);
		int     getOptStat();

		void    setStatBoxVisible(bool on = true);
		bool    getStatBoxVisible();

		void    setStatBoxErrors(bool on = true);
		bool    getStatBoxErrors();

		void    setCloneFlag(bool on = true);
		bool    getCloneFlag();

		void    setDrawTimeFlag(bool on = true);
		bool    getDrawTimeFlag();

		void    setDrawDateFlag(bool on = true);
		bool    getDrawDateFlag();

		void    setDrawItemFlag(bool on = true);
		bool    getDrawItemFlag();

		void    storeGeometry(QMainWindow* w);
		void    restoreGeometry(QMainWindow* w);

		void    storePanelSize(QWidget* w);
		void    restorePanelSize(QWidget* w);

		void    setBrowserColumn(const char* name, int width);
		int     getBrowserColumn(const char* name, int defwidth);

		void    setHistName(const QString& value);
		QString getHistName();

		void    setHistType(int value);
		int     getHistType();

		void    setHistAxisPars(int naxis, int npoints, double min, double max);
		void    getHistAxisPars(int naxis, int& npoints, double& min, double& max);

		void    setFetchDataWhenDraw(bool on = true);
		bool    getFetchDataWhenDraw();

		void    setFetchDataWhenCopy(bool on = true);
		bool    getFetchDataWhenCopy();

		void    setFetchDataWhenSave(bool on = true);
		bool    getFetchDataWhenSave();

		void    setRemoteFileSett(const QString& hostname, const QString& filename, const QString& protocol);
		void    getRemoteFileSett(QString& hostname, QString& filename, QString& protocol);

		void    setPrinterSett(const QString& name, const QString& cmd);
		void    getPrinterSett(QString& name, QString& cmd);

		/**some things for the mbs monitor dockwindow: */
		void    setMbsMonitorNode(const QString& name);
		QString getMbsMonitorNode();
		void    setMbsMonitorFreq(int secs);
		int     getMbsMonitorFreq();
		void    setMbsMonitorBins(int num);
		int     getMbsMonitorBins();
		void    setMbsMonitorTrend(bool on = true);
		bool    getMbsMonitorTrend();
		void    setMbsMonitorMore(bool on = true);
		bool    getMbsMonitorMore();
		void    setMbsMonitorMonitorActive(bool on = true);
		bool    getMbsMonitorMonitorActive();
		void    setMbsMonitorBackwardsTrending(bool on = true);
		bool    getMbsMonitorBackwardsTrending();

		/**some things for the DABC monitor dockwindow: */
		void    setDabcMonitorNode(const QString& name);
		QString getDabcMonitorNode();
		void    setDabcMonitorFreq(int secs);
		int     getDabcMonitorFreq();
		void    setDabcMonitorBins(int num);
		int     getDabcMonitorBins();
		void    setDabcMonitorBackwardsTrending(bool on = true);
		bool    getDabcMonitorBackwardsTrending();

		/** implement history for gui commandline*/
		QStringList getCommandsHistoryGUI();
		void setCommandsHistoryGUI(const QStringList & commands);


	private:

		/** Currently opened settings */
		QSettings* sett;

		/** This is the name of the file with settings, if empty - default qt location will be used */
		static QString fgSettFileName;

};


extern TGo4QSettings* go4sett;


#endif // TGO4ANALYSISWINDOW_H
