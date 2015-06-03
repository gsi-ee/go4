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

//////////////////////////////////////////////////////////////
////// DABC structures:
//////////////////////////////////////////////////////////////
// later include this from DABCSYS?

//#ifdef __GO4DIM__



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

      virtual void infoHandler()
        {
            fxOwner->infoUpdated(this);
        }
      const QString& getServiceType(){return fxType;}
   protected:
    TGo4DabcMonitor* fxOwner;
    QString fxType;

};

class TGo4DabcNodesInfo : public TGo4DabcInfo{
    public:
      TGo4DabcNodesInfo(const char* name, int time, char* nolink, TGo4DabcMonitor* owner):TGo4DabcInfo(name,time,nolink,owner){;}
      virtual ~TGo4DabcNodesInfo(){;}

      virtual void infoHandler()
        {
            fxOwner->nodesUpdated(this);
        }

};

class TGo4DabcServiceInfo : public TGo4DabcInfo{
    public:
      TGo4DabcServiceInfo(const char* name, int time, char* nolink, TGo4DabcMonitor* owner):TGo4DabcInfo(name,time,nolink,owner){;}
      virtual ~TGo4DabcServiceInfo(){;}

      virtual void infoHandler()
        {
            fxOwner->servicesUpdated(this);
        }

};

class TGo4DabcStateInfo : public TGo4DabcInfo{
    public:
      TGo4DabcStateInfo(const char* name, int time, void* nolink, int size, TGo4DabcMonitor* owner):TGo4DabcInfo(name, time, nolink, size, "StateInfo", owner){;}
      virtual ~TGo4DabcStateInfo(){;}

      virtual void infoHandler()
        {
            fxOwner->stateUpdated(this);
        }

};

class TGo4DabcRateInfo : public TGo4DabcInfo{
    public:
      TGo4DabcRateInfo(const char* name, int time, void* nolink, int size, TGo4DabcMonitor* owner):TGo4DabcInfo(name, time, nolink, size, "RateInfo" , owner){;}
      virtual ~TGo4DabcRateInfo(){;}

      virtual void infoHandler()
        {
            fxOwner->rateUpdated(this);
        }

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

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////// Methods of TGo4DabcMonitor:
//////////////////////////////////////////////////////////////


void TGo4DabcMonitor::init()
{
strncpy(gNolinkStateRecord.status,"not available",16);
gNolinkRateRecord.value=-1.0;
fxServerInfo=0;
fxLastTimestamp=0;
fbRebuildNodeTable=true;
fbDisplayNodeTable=false;
fbNodeTableLocked=false;
fbRebuildRateTable=true;
fbDisplayRateTable=false;
fbRateTableLocked=false;

NodeTable->setLeftMargin(0);
NodeTable->hideColumn ( DABCMON_NODE_INDEXCOL) ;
NodeTable->setColumnWidth ( DABCMON_NODE_INDEXCOL,1) ;
NodeTable->setColumnReadOnly (DABCMON_NODE_NODECOL,true );
NodeTable->setColumnReadOnly (DABCMON_NODE_STATECOL,true );
NodeTable->setColumnReadOnly (DABCMON_NODE_INDEXCOL,true );
NodeTable->horizontalHeader()->setLabel( DABCMON_NODE_NODECOL, tr( "Context" ) );
NodeTable->horizontalHeader()->setLabel( DABCMON_NODE_STATECOL, tr( "State" ) );
NodeTable->horizontalHeader()->setLabel( DABCMON_NODE_CHECKCOL , tr( "Ratemeters" ) );
NodeTable->horizontalHeader()->setLabel(DABCMON_NODE_LOGCOL, tr( "Service list" ) );

RateTable->setLeftMargin(0);
RateTable->hideColumn ( DABCMON_RATE_INDEXCOL) ;
RateTable->setColumnWidth ( DABCMON_RATE_INDEXCOL,1) ;
RateTable->setColumnReadOnly (DABCMON_RATE_NAMECOL,true );
RateTable->setColumnReadOnly (DABCMON_RATE_RATECOL,true );
RateTable->setColumnReadOnly (DABCMON_RATE_INDEXCOL,true );
RateTable->horizontalHeader()->setLabel( DABCMON_RATE_NAMECOL, tr( "Name" ) );
RateTable->horizontalHeader()->setLabel( DABCMON_RATE_RATECOL, tr( "Value" ) );
RateTable->horizontalHeader()->setLabel( DABCMON_RATE_TRENDCOL, tr( "Trending" ) );
RateTable->horizontalHeader()->setLabel( DABCMON_RATE_STATSCOL, tr( "Statistics" ) );
FullPrintDIMButton->setDisabled(true);
dimServiceFilterEdit->setDisabled(true);




fxDabcNodes.clear();
fxServices.clear();
fxRates.clear();
fxStates.clear();
fxShowRateFlags.clear();
fxShowLogFlags.clear();
fxTrendingFlags.clear();
fbTrendingInit.clear();
fxStatsFlags.clear();
fbStatsInit.clear();
fxRateQueues.clear();
fxTrendHistoRefnames.clear();
fxStatHistoRefnames.clear();
fxStateRecords.clear();
fxRateRecords.clear();
fxRateSum.clear();
fxRateCount.clear();
fxDisplayTimer=new QTimer(this);
fxDisplayTimer->connect( fxDisplayTimer, SIGNAL(timeout()), this, SLOT(displayAll()) );
fxDisplayTimer->start(500); // ms period
fxAverageTimer=new QTimer(this);
fxAverageTimer->connect( fxAverageTimer, SIGNAL(timeout()), this, SLOT(displayAverageHistograms()));


fiTrendBins=100;
fiStatBins=100;
//TrendBinsBox->setValue(fiTrendBins);
fbTrendingForward=false;
fbHistogramming=false;
fbDisplayHistograms=false;


QString node=go4sett->getDabcMonitorNode();

dimDnsNodeEdit->setText(node);
fiTrendBins=go4sett->getDabcMonitorBins();
fiStatBins=fiTrendBins;
TrendBinsBox->setValue(fiTrendBins);
FrequencyBox->setValue(go4sett->getDabcMonitorFreq());
fbTrendingForward=!(go4sett->getMbsMonitorBackwardsTrending());



controlBox->setShown(false);

//std::cout<<"found settings: node="<<node<<", bins="<<fiTrendBins<<", forward="<<fbTrendingForward <<std::endl;

//polish();
//update();
//show();
}

//////////////////////////////////////////////////////////////
////// Qt slots :
//////////////////////////////////////////////////////////////



void TGo4DabcMonitor::storeSettings()
{
//std::cout<<"storeSettings" <<std::endl;
go4sett->setDabcMonitorNode(fxDnsNode);
go4sett->setDabcMonitorBins(fiTrendBins);
go4sett->setDabcMonitorFreq(FrequencyBox->value());
go4sett->setDabcMonitorBackwardsTrending(!fbTrendingForward);
}



void TGo4DabcMonitor::refreshDIMSlot()
{
TGo4LockGuard gard;
//std::cout<<"refreshDIMSlot()" <<std::endl;
// first get list of dabc nodes from DIM server
fxDnsNode=dimDnsNodeEdit->text().stripWhiteSpace();
if(fxDnsNode.isEmpty()) return; // avoid crash of DIM with fatal error ;-)
DimClient::setDnsNode (fxDnsNode);
delete fxServerInfo;
FullPrintDIMButton->setDisabled(true);
dimServiceFilterEdit->setDisabled(true);
fxServerInfo=new TGo4DabcNodesInfo("DIS_DNS/SERVER_LIST",0,"Name server not available", this);
storeSettings();

}


void TGo4DabcMonitor::logDIMSlot()
{
TGo4LockGuard gard;
//std::cout<<"logDIMSlot()" <<std::endl;
for(int nix=0; nix<fxDabcNodes.size(); ++nix)
    {
        if(fxShowLogFlags[nix] )
            {
//                std::cout<<std::endl<< "retrieving current DIM variables from node "<<fxDabcNodes[nix]<<" ..." <<std::endl;
                createLogServices(nix);
            }
    }
}


void TGo4DabcMonitor::histogramCheckToggled( bool val )
{
TGo4LockGuard gard;
//std::cout <<"---------------- histogramCheckToggled to "<<val<< std::endl;
fbHistogramming=val;
// better: reset all existing init flags of services!
if(val)
    {
        for(int nodeix=0;nodeix<fbTrendingInit.size();++nodeix) // dim node index
            {
                std::vector <std::vector <bool> > & nodevec=fbTrendingInit[nodeix];
                for(int rateix=0; rateix<nodevec.size();++rateix) // raterecord index
                    {
                        std::vector <bool>  & hisvec= nodevec[rateix];
                        for (int hisix=0; hisix<hisvec.size();++hisix) // histogram index
                            {
                                hisvec[hisix]=val;
                            }
                    }
            }
         for(int nodeix=0;nodeix<fbStatsInit.size();++nodeix)
            {
                std::vector <std::vector <bool> > & nodevec=fbStatsInit[nodeix];
                for(int rateix=0; rateix<nodevec.size();++rateix)
                    {
                        std::vector <bool>  & hisvec= nodevec[rateix];
                        for (int hisix=0; hisix<hisvec.size();++hisix)
                            {
                                hisvec[hisix]=val;
                            }
                    }
            }
    } // if val
// reset average counters:
for (int nodeix=0;nodeix<fxRateSum.size();++nodeix)
    {
        std::vector<float> & nodevec=fxRateSum[nodeix];
        for(int rateix=0; rateix<nodevec.size();++rateix)
            {
                nodevec[rateix]=0;
            }
    }
for(int nodeix=0;nodeix<fxRateCount.size();++nodeix)
    {
        std::vector<unsigned int> & nodevec=fxRateCount[nodeix];
        for(int rateix=0; rateix<nodevec.size();++rateix)
            {
                nodevec[rateix]=0;
            }
    }
TrendBinsBox->setDisabled(val);
storeSettings();
}


void TGo4DabcMonitor::averageCheckToggled( bool val )
{
//std::cout <<"---------------- averageCheckToggled to "<<val<< std::endl;
FrequencyBox->setDisabled(val);
if(val)
    {
        //std::cout <<"---------------- start timer here"<< std::endl;
        float milsecs=1000*FrequencyBox->value();
        fxAverageTimer->start(milsecs);

    }
else
    {
        //std::cout <<"---------------- stop timer here"<< std::endl;
        fxAverageTimer->stop();
    }
storeSettings();
}


void TGo4DabcMonitor::binsizeChanged( int val )
{
    //std::cout <<"---------------- binsizeChanged to "<<val<< std::endl;
    fiTrendBins=val;
    fiStatBins=val;
    //storeSettings();
}





void TGo4DabcMonitor::nodeTableChangedSlot( int row, int column )
{
TGo4LockGuard gard;
    //std::cout<<"nodeTableChangedSlot for " << row<<","<<column <<std::endl;
// get index from table:
QString ixtext= NodeTable->text(row,  DABCMON_NODE_INDEXCOL);
int ix=ixtext.toInt();

if(column==DABCMON_NODE_CHECKCOL)
    {
        // get value from table:
        bool on=false;
        QCheckTableItem* checkitem= dynamic_cast<QCheckTableItem*>(NodeTable->item(row,  DABCMON_NODE_CHECKCOL));
        if(checkitem)
           on=checkitem->isChecked();
        else
           std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong rate checktableitem at tablerow "<<row <<std::endl;

        // set value to vector
        fxShowRateFlags[ix]=on;
        // now create/delete rate services for this node:
        if(on)
            createRateServices(ix);
        else
            deleteRateServices(ix);
    }
else if (column==DABCMON_NODE_LOGCOL)
    {
        QCheckTableItem* checkitem= dynamic_cast<QCheckTableItem*>(NodeTable->item(row,  DABCMON_NODE_LOGCOL));
        bool on=false;
        if(checkitem)
           on=checkitem->isChecked();
        else
           std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong log checktableitem at tablerow "<<row <<std::endl;
        // set value to vector
        fxShowLogFlags[ix]=on;
        // switch filter and log buttons:
        bool disablebutton=true;
        for(int t=0;t<fxShowLogFlags.size();++t)
            {
                if(fxShowLogFlags[t])
                    {
                        disablebutton=false;
                        break;
                    }
            }
         FullPrintDIMButton->setDisabled(disablebutton);
         dimServiceFilterEdit->setDisabled(disablebutton);

    }

}


void TGo4DabcMonitor::rateTableChangedSlot(int row, int column)
{
TGo4LockGuard gard;
    //std::cout<<"rateTableChangedSlot for " << row<<","<<column <<std::endl;
if(column==DABCMON_RATE_TRENDCOL)
    {
        // get value from table:
        bool on=false;
        QCheckTableItem* checkitem= dynamic_cast<QCheckTableItem*>(RateTable->item(row,  DABCMON_RATE_TRENDCOL));
        if(checkitem)
           on=checkitem->isChecked();
        else
           std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong checktableitem at tablerow "<<row <<std::endl;
        // get index from table:
        int  nix=0;
        int rix=0;
        if(!getRateIndices(row, nix, rix))
             std::cout<<"!!!!!!!!!! NEVER COME HERE: get RateIndices could not get indices for row "<<row <<std::endl;
        fxTrendingFlags[nix].at(rix)=on;
        //std::cout<<"  rrrrrrrrrrrr set fxTrendingFlags["<<nix<<"]["<<rix<<"] to" << on<<std::endl;
        fbTrendingInit[nix].at(rix).at(0)=on; // sample trending init
         fbTrendingInit[nix].at(rix).at(1)=on; // average trending init
    }
else if(column==DABCMON_RATE_STATSCOL)
    {
        // get value from table:
        bool on=false;
        QCheckTableItem* checkitem= dynamic_cast<QCheckTableItem*>(RateTable->item(row,  DABCMON_RATE_STATSCOL));
        if(checkitem)
           on=checkitem->isChecked();
        else
           std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong checktableitem at tablerow "<<row <<std::endl;
        // get index from table:
        int  nix=0;
        int rix=0;
        if(!getRateIndices(row, nix, rix))
             std::cout<<"!!!!!!!!!! NEVER COME HERE: get RateIndices could not get indices for row "<<row <<std::endl;
        // set value to vector
        fxStatsFlags[nix].at(rix)=on;
        //std::cout<<"  rrrrrrrrrrrr set fxStatsFlags["<<nix<<"]["<<rix<<"] to" << on<<std::endl;
         fbStatsInit[nix].at(rix).at(0)=on; // direct sample
         fbStatsInit[nix].at(rix).at(1)=on; // average
    }

}

//////////////////////////////////////////////////////////////
////// DIM callback routines:
//////////////////////////////////////////////////////////////


void TGo4DabcMonitor::infoUpdated( TGo4DabcInfo * info )
{
// handle log printout of all services here:
TGo4LockGuard gard;
//std::cout<"iiiiiiiiiii infoUpdated() for " << info->getName() <<std::endl;
QDateTime timestamp;
timestamp.setTime_t (info->getTimestamp());
std::cout<<"DIM service " << info->getName() <<":"<<std::endl;
    //" at "<<timestamp.toString()<<":" <<std::endl;
if(info->getServiceType()=="int")
    std::cout<<"  - "<< info->getInt();//<<std::endl;
else if (info->getServiceType()=="float")
    std::cout<<"  - "<< info->getFloat();//<<std::endl;
else if (info->getServiceType()=="double")
    std::cout<<"  - "<< info->getDouble();//<<std::endl;
else if (info->getServiceType()=="char")
    std::cout<<"  - "<< info->getString();//<<std::endl;
else
    {
         // service is structure, parse the format string:
         char* ptr=(char*) info->getData();
         int size=info->getSize();
         QString format=info->getServiceType();
         std::cout<<" - structure of format " << format<<std::endl;
         QStringList elements=QStringList::split(";",format);
         for ( QStringList::Iterator it = elements.begin(); it != elements.end(); ++it )
             {
                 QString component=*it;
                 QString type=component.section(':',0,0);
                 QString slen=component.section(':',1,1);
                 int length=slen.toInt();
                 //std::cout<<" - found member:"  <<type<<" of length "<<slen<<" (value="<<length<<")"<<std::endl;
                 QString content="";
                 QString prompt="  - "+type+" ("+slen+") \t>";
                 if(type=="C")
                     {
                         content=ptr;
                         content.truncate(length);
                         ptr+=length;

                     }
                 else if(type=="L")
                     {
                         int* cursor= (int*) ptr;
                         for(int t=0; t<length; ++t)
                             {
                                 QString val;
                                 content+= val.setNum(*cursor,10); // decimal base
                                 cursor++;
                             }
                         ptr= (char*) cursor;
                     }
                 else if(type=="F")
                     {
                         float* cursor= (float*) ptr;
                         for(int t=0; t<length; ++t)
                             {
                                 QString val;
                                 content+= val.setNum( *cursor, 'g',5);
                                 cursor++;
                             }
                         ptr= (char*) cursor;
                     }

                 else
                     {
                         // treat unknown type as list of char values:
                         for(int t=0; t<length; ++t)
                             {
                                 QString val;
                                 content+= val.setNum(*ptr,10);
                                 content+=":";
                                 ptr++;
                             }
                     }// if(type==)
                 std::cout<<prompt<<content<< "< "<<std::endl;;
                 int currentposition= (long) ptr - (long) info->getData();
                 if(currentposition > size)
                     {
                         std::cout<<"    !!!!!!!!!!!!!!!! structure iterator exceeds service size "<<size <<std::endl;
                         break;
                     }
            }//  for ( QStringList::Iterator it
    }//   if(info->getType()=="int")
std::cout<<" - (timestamp:"<<timestamp.toString()<<")" <<std::endl;
delete info; // discard service after first update!
}


void TGo4DabcMonitor::nodesUpdated( TGo4DabcNodesInfo * info )
{
TGo4LockGuard gard;
//std::cout<<"nodes Updated()" <<std::endl;
QString servers=info->getString();
fxNodelist=QStringList::split("|",servers);
refreshNodes();
fbDisplayNodeTable=true;
fbDisplayRateTable=true;

}

void TGo4DabcMonitor::servicesUpdated( TGo4DabcServiceInfo * info )
{
TGo4LockGuard gard;
//std::cout<<"servicesUpdated() for " << info->getName() <<std::endl;
//first find out index of this service:
unsigned int index=-1;
for(unsigned int ix=0; ix<fxServices.size(); ++ix)
    {
        if(info==fxServices[ix])
            {
                index=ix;
                break;
            }
    }
if(index<0)
    {
        std::cout<<"!!!!!!!!!!!servicesUpdated: unknown service info" << (int*) info<<std::endl;
    }
else
    {
    if(index>fxStates.size() )
        {
            std::cout<<"!!!!!!!!!!!NEVER COME HERE: services updated out of bounds, index=" << index<<std::endl;
            return;
        }
    QString services=info->getString();
     //std::cout<<"+++ got service list: " << services <<std::endl;

               // find full name of nodestate in services:
    QStringList servlist=QStringList::split(0x0A,services); // separator is return
    QString stateservice="";
    for ( QStringList::Iterator sit = servlist.begin(); sit != servlist.end(); ++sit )
        {
            QString service=*sit;
            //std::cout<<"++++++ scanning service" << service <<std::endl;
            if(service.contains("RunStatus") || service.contains("Acquisition/State"))
                {
                    stateservice=service.section('|',0,0); // strip additional service info from name
                    break;
                }
        } // for
    // now create info for node's state:
    if(!stateservice.isEmpty())
        {
            // check if we already have state info for index:
            bool recreate=false;
            if(fxStates[index]!=0)
                {
                    //std::cout<<"found existing state service("<<index<<"): " << fxStates[index]->getName() <<std::endl;
                    if(stateservice==fxStates[index]->getName())
                    {
                        //std::cout<<"-------Name is the same, do nothing."<<std::endl;
                        recreate=false;
                    }
                    else
                    {
                        //std::cout<<"-------Name has changed, recreate service!"<<std::endl;
                        delete fxStates[index];
                        fxStates[index]=0;
                        recreate=true;
                    }

                }
            else
                {
                    //std::cout<<"------ Service not existing, create it!."<<std::endl;
                    recreate= true;
                } // if(fxStates[index]!=0)

            if(recreate)
                {
                    //std::cout<<"+++ creating state service: " << stateservice <<std::endl;
                    TGo4DabcStateInfo* sinfo=    new TGo4DabcStateInfo(stateservice, 0,  &gNolinkStateRecord,    sizeof(dabc::StatusRec), this);
                    fxStates[index]=sinfo;
                } //if recreate
        } // if(!stateservice.isEmpty())
    else
        {
            //std::cout<<"+++ empty state service for index  " << index<<", ignored."<<std::endl;
            //std::cout<<"+++ empty state service for index  " << index<<", clear old entry" <<std::endl;
            //delete fxStates[index];
            //fxStates[index]=0;
            //<- problematic if by some glitch state service is temporarily not found in service list!
        }  // if(!stateservice.isEmpty())
}//if(index<0)
}






void TGo4DabcMonitor::stateUpdated( TGo4DabcStateInfo * info )
{
TGo4LockGuard gard;
//std::cout<<"stateUpdated() for " << info->getName() <<std::endl;
// first find out index of this state:
unsigned int index=-1;
for(unsigned int ix=0; ix<fxStates.size(); ++ix)
    {
        if(info==fxStates[ix])
            {
                index=ix;
                break;
            }
    }
//std::cout<<"- state index= " << index  <<std::endl;
if(index<0)
    {
        std::cout<<"ERROR in stateUpdated(): object not in list, pointer " << (int*) info<<std::endl;
    }
else
    {
        dabc::StatusRec* currentstate=(dabc::StatusRec*) info->getData();
        if(index>fxStateRecords.size())
            {
                std::cout<<"!!!!!!!!!!!NEVER COME HERE: state updated out of bounds, index=" << index<<std::endl;
                return;
            }
        fxStateRecords[index]=TGo4DabcState(currentstate);
        fbDisplayNodeTable=true;
        fxLastTimestamp=info->getTimestamp( );
    }
}


void TGo4DabcMonitor::rateUpdated( TGo4DabcRateInfo * info )
{
TGo4LockGuard gard;
//std::cout<<"rateUpdated() for " << info->getName() <<std::endl;

// first find out indexes for this rate info
unsigned int nodeindex=-1;
unsigned int rateindex=-1;
for(unsigned int ix=0; ix<fxRates.size(); ++ix)
    {
    std::vector<TGo4DabcRateInfo*> & rvec=fxRates[ix];
    for(unsigned int rix=0;rix< rvec.size();++rix)
    {

        if(info==rvec[rix])
            {
                nodeindex=ix;
                rateindex=rix;
                break;
            }
    } // for rix
}// for ix
//std::cout<<"- state index= " << index  <<std::endl;
if(nodeindex<0)
    {
        std::cout<<"ERROR in rateUpdated(): object not in list, pointer " << (int*) info<<std::endl;
    }
else
    {
        dabc::RateRec* currentrate=(dabc::RateRec*) info->getData();
        if(nodeindex>fxRateRecords.size())
            {
                std::cout<<"!!!!!!!!!!!NEVER COME HERE: rate updated out of bounds, nodeindex=" << nodeindex<<std::endl;
                return;
            }
        std::vector<TGo4DabcRate> & recordvec= fxRateRecords[nodeindex];
        if(rateindex>recordvec.size())
            {
                std::cout<<"!!!!!!!!!!!NEVER COME HERE: rate updated out of bounds, rateindex=" << rateindex<<std::endl;
                return;
            }
         recordvec[rateindex]=TGo4DabcRate(currentrate, info->getName());
         bool trendon=fxTrendingFlags[nodeindex].at(rateindex);
         bool statson=fxStatsFlags[nodeindex].at(rateindex);
         if(fbHistogramming && (trendon || statson))
            {
                std::vector< std::deque <float> > & qvec= fxRateQueues[nodeindex];
                if(rateindex>qvec.size())
                    {
                        std::cout<<"!!!!!!!!!!!NEVER COME HERE: rate queue vector out of bounds, rateindex=" << rateindex<<std::endl;
                        return;
                    }
                qvec[rateindex].push_back(currentrate->value); // aquire values, display later!
                fbDisplayHistograms=true;
            }
         fxLastTimestamp=info->getTimestamp( );
         fbDisplayRateTable=true;
     } //if(nodeindex<0)
}


//////////////////////////////////////////////////////////////
////// Internal management routines:
//////////////////////////////////////////////////////////////

void TGo4DabcMonitor::refreshNodes()
{
TGo4LockGuard gard;
//fbNodeTableLocked=true;

//std::cout<<"*********refreshNodes()" <<std::endl;
fbRebuildNodeTable=true;
fbRebuildRateTable=true;
fxDabcNodes.clear();

clearRates();
clearServices();
clearStates();
for ( QStringList::Iterator it = fxNodelist.begin(); it != fxNodelist.end(); ++it )
    {
        QString current=*it;
        if(current.contains("DIS_DNS")) continue; // skip name server
        //std::cout<<"+++ processing node entry " << current <<std::endl;
        QString prefix=current.section('/', 0, 0 );
        QString rest=current.section('/', 1, 1 );
        QString reducednode=rest.section('@',0,0); // include port number into node name!
        //std::cout<<"+++ prefix is " << prefix <<std::endl;
        //if(prefix=="DABC")
        if(prefix==current)
            {
                // no dabcnode (i.e. mbs)
                reducednode=current.section('@',0,0);;
            }
        //std::cout<<"++++ found reduced node "<<reducednode <<std::endl;

                //std::cout<<"++++ found DABC prefix " <<std::endl;

                fxDabcNodes.push_back(reducednode);
                // get full service list for this node:
                QString sinfoname=current.section('@',0,0) + "/SERVICE_LIST";
                //std::cout<<"++++ creating service info "<<sinfoname <<std::endl;
                TGo4DabcServiceInfo* servinfo= new TGo4DabcServiceInfo(sinfoname, 0, "not available", this);
                fxServices.push_back(servinfo);
                fxStates.push_back(0); // make sure that for our index a slot in state service vector exists!
                fxStateRecords.push_back(TGo4DabcState()); // dito for state record vector
                // provide empty vectors of rateinfos and raterecords etc for this node:
                std::vector<TGo4DabcRateInfo*> rinfovec;
                rinfovec.clear();
                fxRates.push_back(rinfovec);
                std::vector<TGo4DabcRate> ratevec;
                ratevec.clear();
                fxRateRecords.push_back(ratevec);
                fxShowRateFlags.push_back(false);
                std::vector<float> sumvec;
                fxRateSum.push_back(sumvec);
                std::vector<unsigned int> countvec;
                fxRateCount.push_back(countvec);
                std::vector<bool> trendvec;
                trendvec.clear();
                fxTrendingFlags.push_back(trendvec);
                std::vector <std::vector<bool> >initvec;
                fbTrendingInit.push_back(initvec);
                fxStatsFlags.push_back(trendvec);
                fbStatsInit.push_back(initvec);
                std::vector < std::vector <QString>  >namesvec;
                fxTrendHistoRefnames.push_back(namesvec);
                fxStatHistoRefnames.push_back(namesvec);
                fxShowLogFlags.push_back(false);
                std::vector< std::deque <float> > rateqvec;
                fxRateQueues.push_back(rateqvec);
            //} // if XDAQ
  } // for
    //fbNodeTableLocked=false;
}








void TGo4DabcMonitor::clearStates()
{
TGo4LockGuard gard;
std::vector<TGo4DabcStateInfo*>::iterator iter;
for(iter=fxStates.begin(); iter!=fxStates.end(); ++iter)
    {
     delete *iter;
    }
fxStates.clear();
fxStateRecords.clear();
fxShowRateFlags.clear();
fxShowLogFlags.clear();
}

void TGo4DabcMonitor::clearServices()
{
TGo4LockGuard gard;
std::vector<TGo4DabcServiceInfo*>::iterator iter;
for(iter=fxServices.begin(); iter!=fxServices.end(); ++iter)
    {
      delete *iter;
    }
fxServices.clear();
}


void TGo4DabcMonitor::clearRates()
{
TGo4LockGuard gard;
//fbRateTableLocked=true;
std::vector<std::vector<TGo4DabcRateInfo*> >::iterator nodeiter;
for(nodeiter=fxRates.begin(); nodeiter!=fxRates.end(); ++nodeiter)
    {
        std::vector<TGo4DabcRateInfo*> nodevec=*nodeiter; // each node has vector of rate services!
        std::vector<TGo4DabcRateInfo*>::iterator iter;
        for(iter=nodevec.begin(); iter!=nodevec.end(); ++iter)
            {
              delete *iter;
            }
    }
fxRates.clear();
fxTrendingFlags.clear();
fbTrendingInit.clear();
fxStatsFlags.clear();
fbStatsInit.clear();
fxTrendHistoRefnames.clear();
fxStatHistoRefnames.clear();
fxRateRecords.clear();
fxRateQueues.clear();
fxRateSum.clear();
fxRateCount.clear();
//fbRateTableLocked=false;
}


void TGo4DabcMonitor::createRateServices( int nodeindex )
{
TGo4LockGuard gard;
//fbRateTableLocked=true;
//std::cout<<"rrrrrrrr createRateServices for "<<nodeindex <<std::endl;
//search the service list for our node for all rate services:
QString services=fxServices[nodeindex]->getString();
 //std::cout<<"+++ createRateServices got service list: " << services <<std::endl;

           // find full name of nodestate in services:
QStringList servlist=QStringList::split(0x0A,services); // separator is return
QString rateservice="";
for ( QStringList::Iterator sit = servlist.begin(); sit != servlist.end(); ++sit )
    {
        QString service=*sit;
        //std::cout<<"++++++ scanning service" << service <<std::endl;
        //if(service.contains("F:1;L:1;F:1;F:1;F:1;F:1;C:16;C:16;C:16")
          if(service.contains("F:1;L:1;F:1;F:1;F:1;F:1;C:16;C:16;C")) //check for dabc rate structure info
            {
            // provide slots for aux vectors first, since dim info may call update on creation time!
                fxRateRecords[nodeindex].push_back(TGo4DabcRate()); // need initial dummy record
                std::deque <float> ratequeue;
                fxRateQueues[nodeindex].push_back(ratequeue);
                fxRateSum[nodeindex].push_back(0); // init average rate variables
                fxRateCount[nodeindex].push_back(0);

                fxTrendingFlags[nodeindex].push_back(false); // initial value for trending flag
                std::vector <bool> hisvec;
                hisvec.push_back(false); // flags for 2 histograms each rate
                hisvec.push_back(false);
                fbTrendingInit[nodeindex].push_back(hisvec);
                std::vector <QString> namesvec;
                namesvec.push_back("undefined histogram"); // dito for reference names
                namesvec.push_back("undefined histogram");
                fxTrendHistoRefnames[nodeindex].push_back(namesvec);

                fxStatsFlags[nodeindex].push_back(false); // initial value for statistic hisot flag
                fbStatsInit[nodeindex].push_back(hisvec);
                fxStatHistoRefnames[nodeindex].push_back(namesvec);
                QString rname=service.section('|',0,0); // strip additional service info from name
                //std::cout<<"++++++ creating rate info " << rname <<std::endl;
                TGo4DabcRateInfo* rinfo=new TGo4DabcRateInfo(rname, 0,  &gNolinkRateRecord , sizeof(dabc:: RateRec), this);
                fxRates[nodeindex].push_back(rinfo); // keep dim info here
            }
    } // for
//fbRateTableLocked=false;
}


void TGo4DabcMonitor::deleteRateServices( int nodeindex )
{
TGo4LockGuard gard;
//fbRateTableLocked=true;
//std::cout<<"rrrrrrrr deleteRateServices for "<<nodeindex <<std::endl;
std::vector<TGo4DabcRateInfo*> &  nodevec=fxRates[nodeindex];
std::vector<TGo4DabcRateInfo*>::iterator iter;
for(iter=nodevec.begin(); iter!=nodevec.end(); ++iter)
    {
      delete *iter;
    }
nodevec.clear();
fxTrendingFlags[nodeindex].clear();
fbTrendingInit[nodeindex].clear();
fxTrendHistoRefnames[nodeindex].clear();
fxStatsFlags[nodeindex].clear();
fbStatsInit[nodeindex].clear();
fxStatHistoRefnames[nodeindex].clear();
fxRateRecords[nodeindex].clear();
fxRateSum[nodeindex].clear();
fxRateCount[nodeindex].clear();
//fbRateTableLocked=false;
fbDisplayRateTable=true; // update view in case of last service is gone!

}

void TGo4DabcMonitor::createLogServices(int nodeindex)
{
TGo4LockGuard gard;
//std::cout<<"rrrrrrrr createLogServices for "<<nodeindex <<std::endl;
QRegExp filter(dimServiceFilterEdit->text().stripWhiteSpace());
filter.setWildcard(true); // use simple wildcard matching, like shell
std::cout<<std::endl<< "---- Retrieving current DIM variables from node "<<fxDabcNodes[nodeindex]<<" with filter:"<<filter.pattern() <<std::endl;

//search the service list for our node for all rate services:
QString services=fxServices[nodeindex]->getString();
//std::cout<<"+++ createLogServices got service list: " << services <<std::endl;

       // find full name of nodestate in services:
QStringList servlist=QStringList::split(0x0A,services); // separator is return
QString logservice="";
for ( QStringList::Iterator sit = servlist.begin(); sit != servlist.end(); ++sit )
    {
        QString service=*sit;
        QString sformat=service.section('|',1,1);
        QString sname=service.section('|',0,0); // strip additional service info from name
        QString scom=service.section('|',2,2); // command indicator
        if(sname.contains("SERVICE_LIST")) continue; // skip service list
        if(scom.contains("CMD")) continue; // skip command services
        if(filter.search(sname)<0) continue; // regexp does not match name, skip

        //std::cout<<"++++++ creating log info " << sname <<" for format "<<sformat <<std::endl;
        if(sformat=="C")
            {
                TGo4DabcInfo* info= new TGo4DabcInfo(sname,1,"not available", this);
            }
        else if(sformat=="L")
            {
                TGo4DabcInfo* info= new TGo4DabcInfo(sname,1, (int) -1, this);
            }
        else
            {
                TGo4DabcInfo* info= new TGo4DabcInfo(sname,1,  &gNolinkRateRecord , sizeof(dabc:: RateRec), sformat, this);
            }
    } //   for ( QStringList::Iterator sit =
}







//////////////////////////////////////////////////////////////
////// Display and drawing routines:
//////////////////////////////////////////////////////////////


void TGo4DabcMonitor::displayNodeTable()
{
TGo4LockGuard gard;
//std::cout<<"*********displayNodeTable()" <<std::endl;
if(fbRebuildNodeTable)
    {
        //std::cout<<"*********                               - rebuild table" <<std::endl;
        // first case: create table completely new
        int tablesize  =NodeTable->numRows();
        for(int row=0; row< tablesize;++row)
            {
                NodeTable->removeRow(0);
            }
        int maxnodes=fxDabcNodes.size();
        NodeTable->insertRows(0,maxnodes);
        for(int ix=0; ix<maxnodes; ++ix)
            {
            fillNodeTableRow(ix, ix, true);
            }
        fbRebuildNodeTable=false;
    } //    if(fbRebuildNodeTable)
else
    {
        //std::cout<<"*********                               - update table contents" <<std::endl;
        int tablesize  =NodeTable->numRows();
        int maxnodes=fxDabcNodes.size();
        if(maxnodes!=tablesize)
            {
             std::cout<<"!!!!!!!!!! NEVER COME HERE: table size "<<tablesize<<" does not match number of nodes "<<maxnodes <<std::endl;
                return;
            }

        for(int ix=0; ix<maxnodes; ++ix)
            {
                int tableindex=-1;
                for(int j=0; j<tablesize;++j)
                    {
                        QString ixtext= NodeTable->text(j,  DABCMON_NODE_INDEXCOL);
                        int tix=ixtext.toInt();
                        if(tix==ix)
                            {
                                tableindex=j;
                                break;
                            }
                    }
                if(tableindex<0)
                    {
                        std::cout<<"!!!!!!!!!! NEVER COME HERE: could not find tableindex for "<<ix <<std::endl;
                        continue;
                    }
                else
                    {
                         //std::cout<<"***********tableindex for "<<ix<<" is "<<tableindex <<std::endl;
                    }
               fillNodeTableRow(tableindex, ix, false);
            }//for(int ix=0; ix<maxnodes; ++ix)
    }//if(fbRebuildNodeTable)
polish();
update();
show();
}


void TGo4DabcMonitor::fillNodeTableRow( int tableindex, int nodeindex, bool createnew )
{
NodeTable->setText(tableindex, DABCMON_NODE_NODECOL ,fxDabcNodes[nodeindex]);
NodeTable->setText(tableindex, DABCMON_NODE_STATECOL,fxStateRecords[nodeindex].fxState);
QImage img=QImage::fromMimeSource( "eventitem.png" );
QPixmap pixmap = img.scaleHeight( NodeTable->rowHeight(tableindex) );
QString col =fxStateRecords[nodeindex].fxColor.lower();
//////////////////// optional check if color name is known:
//                QStringList knowncols=QColor::colorNames();
//                QString listcolor="black";
//                 for ( QStringList::Iterator sit = knowncols.begin(); sit != knowncols.end(); ++sit )
//                {
//                     //std::cout <<" ... fillNodetableRow compares color "<<*sit <<" with "<<col << std::endl;
//                     if(*sit==col) {listcolor=*sit; break;}
//                 }
//                pixmap.fill(listcolor);
///////////////////////
pixmap.fill(fxStateRecords[nodeindex].fxColor.lower());
NodeTable->setPixmap (tableindex, DABCMON_NODE_STATECOL, pixmap);
if(createnew)
    {
        NodeTable->setText(tableindex,  DABCMON_NODE_INDEXCOL ,QString::number(nodeindex));
        QCheckTableItem* checkitem= new QCheckTableItem (NodeTable, "Show" );
        checkitem->setChecked(fxShowRateFlags[nodeindex]);
        NodeTable->setItem( tableindex,DABCMON_NODE_CHECKCOL,checkitem);
        QCheckTableItem* checklogitem= new QCheckTableItem (NodeTable, "Dump" );
        checklogitem->setChecked(fxShowLogFlags[nodeindex]);
        NodeTable->setItem( tableindex,DABCMON_NODE_LOGCOL,checklogitem);
    }
else
    {
        QCheckTableItem* checkitem= dynamic_cast<QCheckTableItem*>(NodeTable->item(tableindex,  DABCMON_NODE_CHECKCOL));
        if(checkitem)
            checkitem->setChecked(fxShowRateFlags[nodeindex]);
        else
            std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong rate checktableitem at index "<<nodeindex <<std::endl;
        QCheckTableItem* checklogitem= dynamic_cast<QCheckTableItem*>(NodeTable->item(tableindex,  DABCMON_NODE_LOGCOL));
        if(checklogitem)
            checklogitem->setChecked(fxShowLogFlags[nodeindex]);
        else
            std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong logchecktableitem at index "<<nodeindex <<std::endl;
    }

}




void TGo4DabcMonitor::displayRateTable()
{
TGo4LockGuard gard;
//std::cout<<"*********displayRateTable()" <<std::endl;
 // find number of existing rate services
int numrates=0;
int maxnodes=fxRateRecords.size();
for(int ix=0; ix<maxnodes; ++ix)
    {
        numrates+=fxRateRecords[ix].size();
    }
int tablesize  =RateTable->numRows();
int tablecursor=-1;
if(fbRebuildRateTable)
    {
        //std::cout<<"*********                               - rebuild table" <<std::endl;
        // first case: create table completely new
        for(int row=0; row< tablesize;++row)
            {
                RateTable->removeRow(0);
            }
       //std::cout<<"*                    inserting "<<numrates<<" rows" <<std::endl;
        RateTable->insertRows(0,numrates);
        int tableindex=0;
        // now fill table with services:
        for(int ix=0; ix<maxnodes; ++ix)
            {
                std::vector<TGo4DabcRate> & rvec=fxRateRecords[ix];
                //std::cout<<"*                    looping rvec[ "<<ix<<"] with "<<rvec.size()<<" rate entries" <<std::endl;
                for(int rix=0; rix<rvec.size();++rix)
                    {
                        //std::cout<<"*                    found rvec[ "<<ix<<"]["<<rix <<"] with name "<< rvec[rix].fxName<< std::endl;
                        fillRateTableRow(tableindex, ix, rix, true);
                        tableindex++;
                        //std::cout<<"*                    tableindex gets "<<tableindex<<std::endl;
                    } // for rix
            }// for ix
       //std::cout<<"*                    after loop over maxnodes="<<maxnodes<<std::endl;
        fbRebuildRateTable=false;
    } //    if(fbRebuildRateTable)
else
    {
        //std::cout<<"*********                               - update table contents" <<std::endl;
        if(numrates>tablesize)
            {
                 //std::cout<<"table size "<<tablesize<<" smaller than number of rate records "<<numrates <<std::endl;
                 int diff=numrates-tablesize;
                 tablecursor=tablesize;
                 //std::cout<<"*                    inserting "<<diff<<" rows" <<std::endl;
                 RateTable->insertRows(tablesize,diff);
            }
        else if(numrates<tablesize)
            {
                 //std::cout<<"table size "<<tablesize<<" bigger than number of rate records "<<numrates <<std::endl;
                 // here we have to find the obsolete rows and remove it! do this after updating contents
            }
        else
            {
                // size is the same, do nothing
            }
        tablesize=RateTable->numRows();
        bool usedrows[tablesize]; // keep track of unused table rows for cleanup
        for(int t=0;t<tablesize;++t){usedrows[t]=false;}
        for(int nix=0; nix<maxnodes; ++nix)
            {
                std::vector<TGo4DabcRate> & rvec=fxRateRecords[nix];
                for(int rix=0; rix<rvec.size();++rix)
                    {
                        // find tableindex for nix and rix:
                        int tableindex=-1;
                        for(int j=0; j<tablesize;++j)
                            {
                            int tablenodeix=-1;
                            int tablerateix=-1;
                            if(!getRateIndices(j, tablenodeix, tablerateix)) continue; // skip newly created empty rows
                                 if(tablenodeix==nix && tablerateix==rix)
                                    {
                                        tableindex=j;
                                        break;
                                    }
                            }// for j
                        if(tableindex<0)
                            {
                                //std::cout<<"!!!!!!!!!! could not find tableindex for ("<<nix<<","<<rix<<")" <<std::endl;
                                //std::cout<<"*********creating new table entry at "<<tablecursor;
                                if(tablecursor<0)
                                    {
                                        std::cout<<"!!!!!!!!!! NEVER COME HERE: tablecursor undefined for new entry ("<<nix<<","<<rix<<")" <<std::endl;
                                        continue;
                                    }
                                fillRateTableRow(tablecursor, nix, rix, true);
                                usedrows[tablecursor]=true;
                                tablecursor++;
                            }
                         else
                            {
                                 //std::cout<<"***********tableindex for ("<<nix<<","<<rix<<") was "<<tableindex <<std::endl;
                                 usedrows[tableindex]=true;
                                 fillRateTableRow(tableindex, nix, rix, false);
                             } //  if(tableindex<0)
                    }// for rix
            }// for nix
        int offset=0;
        for(int rownum=0;rownum<tablesize;++rownum)
            {
                if(usedrows[rownum]==false)
                    {
                        //std::cout<<"***********displayRateTable found unused row "<<rownum<<", remove it!"<<std::endl;
                        RateTable->removeRow(rownum-offset);
                        //std::cout<<"*********** removed at"<<rownum-offset<<std::endl;
                        offset++; // correct index for already removed rows above!
                    }
            }// for rownum
    }//if(fbRebuildRateTable)
polish();
update();
show();
}

void TGo4DabcMonitor::fillRateTableRow( int tableindex, int nodeindex, int rateindex, bool createnew )
{
    //std::cout<<":fillRateTableRow for "<<tableindex<<std::endl;
std::vector<TGo4DabcRate> & rvec=fxRateRecords[nodeindex];
QString val;
val.setNum(rvec[rateindex].fxRate);//setNum ( float n, char f = 'g', int prec = 6 )
val+=" "+rvec[rateindex].fxUnits;
RateTable->setText(tableindex,  DABCMON_RATE_RATECOL , val);
if(rvec[rateindex].fxRate<0)
    {
        QImage img=QImage::fromMimeSource( "info1.png" );
        QPixmap pix = img.scaleHeight( RateTable->rowHeight(tableindex) );
        RateTable->setPixmap (tableindex, DABCMON_RATE_RATECOL, pix);
    }
else
    {
        RateTable->setPixmap (tableindex, DABCMON_RATE_RATECOL, QPixmap() );
    }
if(createnew)
    {
        QString namestring=rvec[rateindex].fxName.section('/',1); // strip DABC prefix
        RateTable->setText(tableindex,  DABCMON_RATE_NAMECOL , namestring);
        QString indexstring=QString::number(nodeindex)+":"+QString::number(rateindex);
        RateTable->setText(tableindex,  DABCMON_RATE_INDEXCOL,indexstring);
        QCheckTableItem* trendcheckitem= new QCheckTableItem (RateTable, "histogram" );
        trendcheckitem->setChecked(fxTrendingFlags[nodeindex].at(rateindex));
        RateTable->setItem( tableindex,DABCMON_RATE_TRENDCOL,trendcheckitem);
        QCheckTableItem* statcheckitem= new QCheckTableItem (RateTable, "histogram" );
        statcheckitem->setChecked(fxStatsFlags[nodeindex].at(rateindex));
        RateTable->setItem( tableindex,DABCMON_RATE_STATSCOL,statcheckitem);
    }
else
    {
        QCheckTableItem* trendcheckitem= dynamic_cast<QCheckTableItem*>(RateTable->item(tableindex,  DABCMON_RATE_TRENDCOL));
        if(trendcheckitem)
            trendcheckitem->setChecked(fxTrendingFlags[nodeindex].at(rateindex));
        else
            std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong trend checktableitem at table index "<<tableindex <<std::endl;
          QCheckTableItem* statcheckitem= dynamic_cast<QCheckTableItem*>(RateTable->item(tableindex,  DABCMON_RATE_STATSCOL));
        if(statcheckitem)
            statcheckitem->setChecked(fxStatsFlags[nodeindex].at(rateindex));
        else
            std::cout<<"!!!!!!!!!! NEVER COME HERE: wrong stat checktableitem at table index "<<tableindex <<std::endl;
    } // if(createnew)
}


bool TGo4DabcMonitor::getRateIndices( int tablerow, int & nodeix, int & rateix )
{
QString ixtext= RateTable->text(tablerow,  DABCMON_RATE_INDEXCOL);
QString nstring=ixtext.section( ':', 0,0);
if(nstring.isEmpty()) return false; // for newly created empty row, we would get 0,0 indices!
nodeix=nstring.toInt();
QString rstring=ixtext.section( ':', 1,1);
rateix=rstring.toInt();
return true;
}




void TGo4DabcMonitor::displaySampleHistograms()
{
TGo4LockGuard gard;
// iterate all rate indices:
for(int nodeix=0;nodeix<fxTrendingFlags.size();++nodeix)
    {
        std::vector<bool> & trendvec=fxTrendingFlags[nodeix];
        if(nodeix>fxStatsFlags.size())
            {
                std::cout <<"--NEVER COME HERE: node index mismatch in displaySampleHistograms for nix="<< nodeix<<std::endl;
                return;
            }
        std::vector<bool> & statsvec=fxStatsFlags[nodeix];
        for(int rateix=0; rateix<trendvec.size();++rateix)
            {
                bool trending=trendvec[rateix];
                if(rateix>statsvec.size())
                    {
                        std::cout <<"--NEVER COME HERE: rate index mismatch in displaySampleHistograms for rix)"<< rateix<<std::endl;;
                        return;
                    }
                bool statisting=statsvec[rateix];
                if(trending ||  statisting)
                    {
                        // check if value queue for this index exists
                        if(nodeix>fxRateQueues.size())
                            {
                                std::cout <<"--NEVER COME HERE: node index mismatch in displaySampleHistograms queues for nix="<< nodeix<<std::endl;
                                return;
                            }
                        std::vector< std::deque <float> > & qvec =fxRateQueues[nodeix];
                        if(rateix>qvec.size())
                            {
                                std::cout <<"--NEVER COME HERE: rate index mismatch in displaySampleHistograms queues for rix)"<< rateix<<std::endl;;
                                return;
                            }
                        std::deque<float> &ratequeue = qvec[rateix];
                        // read all values from queue and display in histogram
                        while (!ratequeue.empty())
                            {
                                 float val=ratequeue.front();
                                 ratequeue.pop_front();
                                 //  sum value and update counter:
                                 fxRateSum[nodeix].at(rateix)+=val;
                                 fxRateCount[nodeix].at(rateix)++;
                                 if(!TrendSampleCheck->isChecked()) continue; // clear queue, but do not display sample histograms
                                 if(trending)
                                      updateTrending(nodeix,rateix, 0, val);
                                 if(statisting)
                                     updateStats(nodeix,rateix,0, val);
                             }// while ratequeue
                    } // if trending or statisting
            } // for rateix
    }//for nodeix
}


void TGo4DabcMonitor::displayAverageHistograms()
{
TGo4LockGuard gard;
if(!fbHistogramming) return;
// iterate all rate indices:
for(int nodeix=0;nodeix<fxTrendingFlags.size();++nodeix)
    {
        std::vector<bool> & trendvec=fxTrendingFlags[nodeix];
        if(nodeix>fxStatsFlags.size())
            {
                std::cout <<"--NEVER COME HERE: node index mismatch in displaySampleHistograms for nix="<< nodeix<<std::endl;
                return;
            }
        std::vector<bool> & statsvec=fxStatsFlags[nodeix];
        for(int rateix=0; rateix<trendvec.size();++rateix)
            {
                bool trending=trendvec[rateix];
                if(rateix>statsvec.size())
                {
                    std::cout <<"--NEVER COME HERE: rate index mismatch in displaySampleHistograms for rix)"<< rateix<<std::endl;;
                    return;
                }
                bool statisting=statsvec[rateix];
                if(trending ||  statisting)
                    {
                        float val=-2;
                        float sum= fxRateSum[nodeix].at(rateix);
                        unsigned int count= fxRateCount[nodeix].at(rateix);
                        if(count!=0) val=sum/count;
                        fxRateSum[nodeix].at(rateix)=0.;
                        fxRateCount[nodeix].at(rateix)=0;
                        if(trending)
                         updateTrending(nodeix,rateix,1, val);
                        if(statisting)
                         updateStats(nodeix,rateix,1,val);
                    }
            }// for rateix
    }// for nodeix
}



void TGo4DabcMonitor::displayAll()
{
    //std::cout<<"ttttttt timer fired displayAll()" <<std::endl;
QDateTime timestamp;
timestamp.setTime_t (fxLastTimestamp);
//DateLabel->setText(timestamp.toString(Qt::ISODate));
DateLabel->setText(timestamp.toString());
//if(fbDisplayNodeTable && !fbNodeTableLocked)
if(fbDisplayNodeTable)
    {
        displayNodeTable();
        fbDisplayNodeTable=false;
    }
//if(fbDisplayRateTable && !fbRateTableLocked)
if(fbDisplayRateTable)
    {
        displayRateTable();
        fbDisplayRateTable=false;
    }

// now treat direct sample histograms:
if(fbDisplayHistograms)
    {
        displaySampleHistograms();
        fbDisplayHistograms=false;
    }

}

//////////////////////////////////////////////////////////////
////// Histogramming routines:
//////////////////////////////////////////////////////////////



void TGo4DabcMonitor::updateTrending( int nodeix, int rateix, int hisix, double value)
{
QString & refname= fxTrendHistoRefnames[nodeix].at(rateix).at(hisix);
QString name=fxRateRecords[nodeix].at(rateix).fxName;
QString foldername=name.section( '/', 1,2);
name=name.section( '/', 3);
name.replace( QChar('/'), "-" );
switch(hisix)
    {
        case 0:
        default:
            name=name+"-TrendingFast";
            break;
        case 1:
            name=name+"-TrendingAverage";
            break;
    };
foldername.replace( QChar('/'), "-" );
QString  & title=fxRateRecords[nodeix].at(rateix).fxUnits;

//std::cout<<"    val="<<value<<", name="<<name<<", refname="<<refname <<", folder="<<foldername<<std::endl;
TH1* his=0;
TGo4Slot* histoslot=0;
if(!fbTrendingInit[nodeix].at(rateix).at(hisix)) histoslot=Browser()->BrowserSlot(refname);
if(histoslot==0)
    {
        Axis_t lo,up;
        if(fbTrendingForward)
            {
                lo=0;
                switch(hisix)
                    {
                        case 0:
                        default:
                            up=1*fiTrendBins;
                            break;
                        case 1:
                            up=1*fiTrendBins*FrequencyBox->value();
                            break;
                    };
            }
        else
            {
                 switch(hisix)
                    {
                        case 0:
                        default:
                            lo=-1*fiTrendBins;
                            break;
                        case 1:
                            lo=-1*fiTrendBins*FrequencyBox->value();
                            break;
                    };
                 up=0;
            }
        his=new TH1F(name,title,fiTrendBins,lo,up);
        TAxis* xax=his->GetXaxis();
        switch(hisix)
            {
                case 0:
                default:
                    xax->SetTitle("updates");
                    break;
                case 1:
                    xax->SetTitle("s");
                    break;
            };
        xax->CenterTitle();
        //xax->SetLimits(0,lo,up);

        TGo4Slot* hisdataslot=Browser()->DataSlot(refname);
        if(hisdataslot)
            {
                hisdataslot->AssignObject(his,true);
            }
        else
            {
                //QString folder="Dabc/"+fxNodelist[nodeix];
                QString folder="DABC/"+foldername;
                refname=Browser()->SaveToMemory(folder, his, true);
            }
        histoslot=Browser()->BrowserSlot(refname);
    }
else
    {
       his=dynamic_cast<TH1*>(histoslot->GetAssignedObject());
    }
IncTrending(his,value,fbTrendingForward);
if(histoslot)
    {
        histoslot->ForwardEvent(histoslot, TGo4Slot::evObjUpdated);
        Browser()->SetItemTimeDate(histoslot);
        fbTrendingInit[nodeix].at(rateix).at(hisix)=false;
    }
//fbRateTableLocked=false;
}


void TGo4DabcMonitor::IncTrending( TH1 * histo, double value, bool forwards )
{
if(histo==0) return;
int bins=histo->GetNbinsX();
//bool forwards=true;
int j,dj;
if(forwards)
       dj=-1;
else
       dj=+1;
for(int i=0;i<bins;i++)
    {
    if(forwards)
        j=bins-i;
    else
        j=i;
    double oldval=histo->GetBinContent(j+dj);
    histo->SetBinContent(j,oldval);
    }
histo->SetBinContent(j+dj,value);
}



void TGo4DabcMonitor::updateStats( int nodeix, int rateix, int hix , double value)
{

QString & refname= fxStatHistoRefnames[nodeix].at(rateix).at(hix);
QString name=fxRateRecords[nodeix].at(rateix).fxName;
QString foldername=name.section( '/', 1,2);
name=name.section( '/', 3);
name.replace( QChar('/'), "-" );
switch(hix)
    {
        case 0:
        default:
            name=name+"-StatsFast";
            break;
        case 1:
            name=name+"-StatsAverage";
            break;
    };
foldername.replace( QChar('/'), "-" );
QString title="counts";
QString  & xtitle=fxRateRecords[nodeix].at(rateix).fxUnits;
//std::cout<<"    val="<<value<<", name="<<name<<", refname="<<refname <<", folder="<<foldername<<std::endl;
TH1* his=0;
TGo4Slot* histoslot=0;
if(! fbStatsInit[nodeix].at(rateix).at(hix)) histoslot=Browser()->BrowserSlot(refname);
if(histoslot==0)
    {
        Axis_t lo,up;
        lo=fxRateRecords[nodeix].at(rateix).fxLower;
        up=fxRateRecords[nodeix].at(rateix).fxUpper;
        if(lo==up)
            {
                lo=0;
                up=100;
                //std::cout <<"using default histogram range for name: "<<name<<" ["<<lo<<","<<up<<"]" << std::endl;
            }
        his=new TH1F(name,title,fiStatBins,lo,up);
        TAxis* xax=his->GetXaxis();
        xax->SetTitle(xtitle);
        xax->CenterTitle();
        TGo4Slot* hisdataslot=Browser()->DataSlot(refname);
        if(hisdataslot)
            {
                hisdataslot->AssignObject(his,true);
            }
        else
            {
                //QString folder="Dabc/"+fxNodelist[nodeix];
                QString folder="DABC/"+foldername;
                refname=Browser()->SaveToMemory(folder, his, true);
            }
        histoslot=Browser()->BrowserSlot(refname);
    }
else
    {
       his=dynamic_cast<TH1*>(histoslot->GetAssignedObject());
    } // if(histoslot==0)
his->Fill(value);
if(histoslot)
    {
        histoslot->ForwardEvent(histoslot, TGo4Slot::evObjUpdated);
        Browser()->SetItemTimeDate(histoslot);
        fbStatsInit[nodeix].at(rateix).at(hix)=false;
    }
}


//#endif


