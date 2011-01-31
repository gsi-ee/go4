/****************************************************************************
** Form interface generated from reading ui file 'TGo4DabcMonitor.ui'
**
** Created: Mo Jan 31 12:37:30 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef TGO4DABCMONITOR_H
#define TGO4DABCMONITOR_H

#include <qvariant.h>
#include <vector>
#include <deque>
#include "QGo4Widget.h"
#include "dic.hxx"
#include "qtimer.h"
#include "QGo4Table.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QGo4Table;
class QGo4LineEdit;
class QPushButton;
class QLabel;
class QCheckBox;
class QSpinBox;
class TGo4DabcInfo;
class TGo4DabcNodesInfo;
class TGo4DabcServiceInfo;
class TGo4DabcStateInfo;
class TGo4DabcRateInfo;
class TGo4DabcState;
class TGo4DabcRate;
class TH1;

class TGo4DabcMonitor : public QGo4Widget
{
    Q_OBJECT

public:
    TGo4DabcMonitor( QWidget* parent = 0, const char* name = 0 );
    ~TGo4DabcMonitor();

    QGroupBox* dabcNodeBox;
    QGo4Table* NodeTable;
    QGroupBox* ratemeterBox;
    QGo4Table* RateTable;
    QGroupBox* dnsBox;
    QGo4LineEdit* dimDnsNodeEdit;
    QPushButton* RefreshDIMButton;
    QLabel* DateLabel;
    QCheckBox* MoreCheck;
    QGroupBox* controlBox;
    QGo4LineEdit* dimServiceFilterEdit;
    QPushButton* FullPrintDIMButton;
    QCheckBox* HistogramCheck;
    QCheckBox* TrendAverageCheck;
    QSpinBox* FrequencyBox;
    QSpinBox* TrendBinsBox;
    QCheckBox* TrendSampleCheck;

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
    std::vector< std::vector <unsigned int> > fxRateCount;
    std::vector< std::vector <float> > fxRateSum;
    bool fbHistogramming;
    int fiStatBins;
    std::vector < std::vector <std::vector <bool> > > fbStatsInit;
    std::vector< std::vector <bool> > fxStatsFlags;
    int fiTrendBins;
    std::vector< std::vector <std::vector <bool> > > fbTrendingInit;
    bool fbNodeTableLocked;
    bool fbRateTableLocked;
    std::vector<std::vector<bool> > fxTrendingFlags;
    unsigned int fxLastTimestamp;
    bool fbDisplayRateTable;
    bool fbDisplayNodeTable;
    bool fbRebuildNodeTable;
    std::vector<std::vector<TGo4DabcRate> > fxRateRecords;
    TGo4DabcNodesInfo* fxServerInfo;
    std::vector<std::vector<TGo4DabcRateInfo*> > fxRates;
    std::vector<TGo4DabcStateInfo*> fxStates;
    QStringList fxNodelist;
    QString fxDnsNode;
    std::vector<TGo4DabcServiceInfo*> fxServices;
    std::vector<TGo4DabcState> fxStateRecords;
    std::vector<QString> fxDabcNodes;
    QTimer* fxDisplayTimer;
    std::vector<bool> fxShowRateFlags;
    bool fbRebuildRateTable;
    std::vector<bool> fxShowLogFlags;
    bool fbTrendingForward;
    std::vector < std::vector <std::vector < QString> > > fxTrendHistoRefnames;
    std::vector < std::vector <std::vector<QString > > > fxStatHistoRefnames;
    std::vector < std::vector < std::deque <float> > > fxRateQueues;
    bool fbDisplayHistograms;
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

    QGridLayout* TGo4DabcMonitorLayout;
    QSpacerItem* spacer2;
    QGridLayout* dabcNodeBoxLayout;
    QGridLayout* ratemeterBoxLayout;
    QVBoxLayout* dnsBoxLayout;
    QHBoxLayout* layout2;
    QGridLayout* controlBoxLayout;
    QHBoxLayout* serviceFilterlayout;
    QHBoxLayout* layout64;
    QHBoxLayout* layout67;
    QSpacerItem* spacer12;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // TGO4DABCMONITOR_H
