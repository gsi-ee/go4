#ifndef TGO4DABCMONITOR_H
#define TGO4DABCMONITOR_H

#include <qvariant.h>

#include "QGo4Widget.h"
#include "ui_TGo4DabcMonitor.h"

#include <vector>
#include <deque>
#include "dic.hxx"
#include <QtCore/qtimer.h>
#include <QtGui/QTableWidget>

class TGo4DabcMonitor;
class TGo4DabcInfo;
class TGo4DabcNodesInfo;
class TGo4DabcServiceInfo;
class TGo4DabcStateInfo;
class TGo4DabcRateInfo;
class TGo4DabcState;
class TGo4DabcRate;
class TH1;

#define DABCMON_NODE_NODECOL 0
#define DABCMON_NODE_STATECOL 1
#define DABCMON_NODE_CHECKCOL 3
#define DABCMON_NODE_LOGCOL 4
#define DABCMON_NODE_INDEXCOL 2

#define DABCMON_RATE_NAMECOL 0
#define DABCMON_RATE_RATECOL 1
#define DABCMON_RATE_TRENDCOL 3
#define DABCMON_RATE_STATSCOL 4
#define DABCMON_RATE_INDEXCOL 2


namespace dabc {

typedef struct{
      int severity; // (0=success, 1=warning, 2=error, 3=fatal)
      char color[16]; // (color name: Red, Green, Blue, Cyan, Yellow, Magenta)
      char status[16]; // status name
   } StatusRec;



 typedef struct{
      float value;
      int displaymode; // one of the DISPLAY_x
      float lower; // limit. If limits are equal, use autoscale
      float upper; // limit
      float alarmlower; // alarm
      float alarmupper; // alarm
      char color[16]; // (color name: Red, Green, Blue, Cyan, Yellow, Magenta)
      char alarmcolor[16]; // (color name: Red, Green, Blue, Cyan, Yellow, Magenta)
      char units[16];
   } RateRec;




   enum RateDisplayMode {
        DISPLAY_ARC = 0,
        DISPLAY_BAR = 1,
        DISPLAY_TREND = 2,
        DISPLAY_STAT = 3 };
} // end namespace dabc

static dabc::RateRec gNolinkRateRecord;
static dabc::StatusRec gNolinkStateRecord;

//////////////////////////////////////////////////////////////
////// Subclasses of DimStampledInfo:
//////////////////////////////////////////////////////////////
class TGo4DabcInfo : public DimStampedInfo {
   public:
      TGo4DabcInfo(const char* name, int time, char* nolink, TGo4DabcMonitor* owner)
          : fxOwner(owner), fxType("char"),DimStampedInfo (name, time, nolink) {;}
       TGo4DabcInfo(const char* name, int time, void* nolink, int size, const char* type, TGo4DabcMonitor* owner)
          : fxOwner(owner), fxType(type),DimStampedInfo (name, time, nolink, size) {;}
       TGo4DabcInfo(const char* name, int time, int nolink, TGo4DabcMonitor* owner)
               : fxOwner(owner), fxType("int"),DimStampedInfo (name, time, nolink) {;}
       TGo4DabcInfo(const char* name, int time, float nolink, TGo4DabcMonitor* owner)
               : fxOwner(owner), fxType("float"),DimStampedInfo (name, time, nolink) {;}
         TGo4DabcInfo(const char* name, int time, double nolink, TGo4DabcMonitor* owner)
               : fxOwner(owner), fxType("double"),DimStampedInfo (name, time, nolink) {;}

      virtual ~TGo4DabcInfo(){;}

      virtual void infoHandler();

      const QString& getServiceType(){return fxType;}
   protected:
    TGo4DabcMonitor* fxOwner;
    QString fxType;

};

class TGo4DabcNodesInfo : public TGo4DabcInfo{
    public:
      TGo4DabcNodesInfo(const char* name, int time, char* nolink, TGo4DabcMonitor* owner):TGo4DabcInfo(name,time,nolink,owner){;}
      virtual ~TGo4DabcNodesInfo(){;}

      virtual void infoHandler();

};

class TGo4DabcServiceInfo : public TGo4DabcInfo{
    public:
      TGo4DabcServiceInfo(const char* name, int time, char* nolink, TGo4DabcMonitor* owner):TGo4DabcInfo(name,time,nolink,owner){;}
      virtual ~TGo4DabcServiceInfo(){;}
      virtual void infoHandler();

};

class TGo4DabcStateInfo : public TGo4DabcInfo{
    public:
      TGo4DabcStateInfo(const char* name, int time, void* nolink, int size, TGo4DabcMonitor* owner):TGo4DabcInfo(name, time, nolink, size, "StateInfo", owner){;}
      virtual ~TGo4DabcStateInfo(){;}

      virtual void infoHandler();

};

class TGo4DabcRateInfo : public TGo4DabcInfo{
    public:
      TGo4DabcRateInfo(const char* name, int time, void* nolink, int size, TGo4DabcMonitor* owner):TGo4DabcInfo(name, time, nolink, size, "RateInfo" , owner){;}
      virtual ~TGo4DabcRateInfo(){;}
      virtual void infoHandler();

};

//////////////////////////////////////////////////////////////
////// management structures to keep the record contents:
//////////////////////////////////////////////////////////////
class TGo4DabcState{
public:
    TGo4DabcState():fxSeverity(-1),fxState("not defined"), fxColor("Black"){;}
    TGo4DabcState(dabc::StatusRec* rec)
    {
        if(rec==0) return;
        fxSeverity=rec->severity;
        fxState=rec->status;
        fxState.truncate(16);
        fxColor=rec->color;
        fxColor.truncate(16);
    }
    int fxSeverity;
    QString fxState;
    QString fxColor;
};

class TGo4DabcRate{

public:
    TGo4DabcRate():fxRate(-1),fxName("not defined"), fxColor("Black"){;}
    TGo4DabcRate(dabc::RateRec* rec, const QString& name)
    {
        if(rec==0) return;
        fxRate=rec-> value;
        fxDisplaymode=rec->displaymode;
        fxLower=rec->lower;
        fxUpper=rec->upper;
        fxAlarmlower=rec->alarmlower;
        fxAlarmupper=rec->alarmupper;
        fxColor=rec->color;
        fxColor.truncate(16);
        fxAlarmcolor=rec->alarmcolor;
        fxAlarmcolor.truncate(16);
        fxUnits=rec->units;
        fxUnits.truncate(16);
        fxName=name;
    }
      float fxRate;
      int fxDisplaymode; // one of the DISPLAY_x
      float fxLower; // limit. If limits are equal, use autoscale
      float fxUpper; // limit
      float fxAlarmlower; // alarm
      float fxAlarmupper; // alarm
      QString fxColor; // (color name: Red, Green, Blue, Cyan, Yellow, Magenta)
      QString fxAlarmcolor; // (color name: Red, Green, Blue, Cyan, Yellow, Magenta)
      QString fxUnits;
      QString fxName;
  };





class TGo4DabcMonitor : public QGo4Widget , public Ui::TGo4DabcMonitor
{
    Q_OBJECT

public:
    TGo4DabcMonitor( QWidget* parent = 0, const char* name = 0 );
    virtual ~TGo4DabcMonitor();



    virtual void infoUpdated( TGo4DabcInfo * info );
    virtual void nodesUpdated( TGo4DabcNodesInfo * info );
    virtual void servicesUpdated( TGo4DabcServiceInfo * info );
    virtual void stateUpdated( TGo4DabcStateInfo * info );
    virtual void rateUpdated( TGo4DabcRateInfo * info );

public slots:
    virtual void storeSettings();
    virtual void refreshDIMSlot();
    virtual void logDIMSlot();
    virtual void histogramCheckToggled( bool val );
    virtual void averageCheckToggled( bool val );
    virtual void binsizeChanged( int val );
    virtual void nodeTableChangedSlot( int row, int column );
    virtual void rateTableChangedSlot( int row, int column );
    virtual void createLogServices( int nodeindex );
    virtual void displaySampleHistograms();
    virtual void displayAverageHistograms();
    virtual void displayAll();
    virtual void updateTrending( int nodeix, int rateix, int hisix, double value );
    virtual void IncTrending( TH1 * histo, double value, bool forwards );
    virtual void updateStats( int nodeix, int rateix, int hix, double value );

protected:

    bool fbHistogramming;
    bool fbDisplayRateTable;
    bool fbDisplayNodeTable;
    bool fbDisplayHistograms;
    bool fbRebuildNodeTable;
    bool fbRebuildRateTable;
    bool fbTrendingForward;

    unsigned int fxLastTimestamp;
    QStringList fxNodelist;
    QString fxDnsNode;
    TGo4DabcNodesInfo* fxServerInfo;

    std::vector< std::vector <unsigned int> > fxRateCount;
    std::vector< std::vector <float> > fxRateSum;
    // flags and bins for statistics/trending histograms:
    int fiStatBins;
    std::vector < std::vector <std::vector <bool> > > fbStatsInit;
    std::vector< std::vector <bool> > fxStatsFlags;
    int fiTrendBins;
    std::vector< std::vector <std::vector <bool> > > fbTrendingInit;
    std::vector<std::vector<bool> > fxTrendingFlags;


    std::vector<std::vector<TGo4DabcRate> > fxRateRecords;
    std::vector<std::vector<TGo4DabcRateInfo*> > fxRates;
    std::vector<TGo4DabcStateInfo*> fxStates;
    std::vector<TGo4DabcServiceInfo*> fxServices;
    std::vector<TGo4DabcState> fxStateRecords;
    std::vector<QString> fxDabcNodes;
    std::vector<bool> fxShowRateFlags;
    std::vector<bool> fxShowLogFlags;

    std::vector < std::vector <std::vector < QString> > > fxTrendHistoRefnames;
    std::vector < std::vector <std::vector<QString > > > fxStatHistoRefnames;
    std::vector < std::vector < std::deque <float> > > fxRateQueues;


    QTimer* fxDisplayTimer;
    QTimer* fxAverageTimer;

    virtual void refreshNodes();
    virtual void clearStates();
    virtual void clearServices();
    virtual void clearRates();
    virtual void createRateServices( int nodeindex );
    virtual void deleteRateServices( int nodeindex );
    virtual void displayNodeTable();
    virtual void fillNodeTableRow( int tableindex, int nodeindex, bool createnew );
    virtual void displayRateTable();
    virtual void fillRateTableRow( int tableindex, int nodeindex, int rateindex, bool createnew );
    virtual bool getRateIndices( int tablerow, int & nodeix, int & rateix );



};

#endif // TGO4DABCMONITOR_H
