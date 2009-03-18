#include "TGo4OptStatsSettings.h"
#include "TGo4Condition.h"
#include "TGo4Marker.h"
#include "TGo4QSettings.h"

TGo4OptStatsSettings::TGo4OptStatsSettings( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI

    bool showstat=go4sett->getStatBoxVisible();
    DrawStatboxButton->setChecked(showstat);
    OptStatGroupBox->setEnabled(showstat);

    int optstats=go4sett->getOptStat();
    //cout <<"Found options stat: "<<optstats << endl;
    bool k= (optstats/100000000)%10 ;
    bool s=(optstats/10000000)%10;
    bool i=(optstats/1000000)%10;
    bool o=(optstats/100000)%10;
    bool u=(optstats/10000)%10;
    bool r=(optstats/1000)%10;
    bool m=(optstats/100)%10;
    bool e=(optstats/10)%10;
    bool n=(optstats/1)%10;

    DrawNameButton->setChecked(n);
    DrawEntriesButton->setChecked(e);
    DrawIntegralButton->setChecked(i);
    DrawRMSButton->setChecked(r);
    DrawMeanButton->setChecked(m);
    DrawCurtosisButton->setChecked(k);
    DrawUnderflowButton->setChecked(u);
    DrawOverflowButton->setChecked(o);
    DrawSkewnessButton->setChecked(s);

    DrawErrorsButton->setChecked(go4sett->getStatBoxErrors());





}


void TGo4OptStatsSettings::setFlags()
{
    bool drawbox=DrawStatboxButton->isChecked();
    bool drawerrors=DrawErrorsButton->isChecked();
    int leader=1000000000; // leading value to correctly decode "name only" option
    bool k=DrawCurtosisButton->isChecked();
    int kurt=100000000* (int) k;
    if(drawerrors) kurt*=2;
    bool s=DrawSkewnessButton->isChecked();
    int skew=10000000* (int) s;
    if(drawerrors) skew*=2;
    bool i=DrawIntegralButton->isChecked();
    int integ=1000000* (int) i;
    bool o=DrawOverflowButton->isChecked();
    int over=100000* (int) o;
    bool u=DrawUnderflowButton->isChecked();
    int under=10000* (int) u;
    bool r=DrawRMSButton->isChecked();
    int rms=1000* (int) r;
    if(drawerrors) rms*=2;
    bool m=DrawMeanButton->isChecked();
    int mean=100* (int) m;
    if(drawerrors) mean*=2;
    bool e=DrawEntriesButton->isChecked();
    int ent=10* (int) e;
    bool n=DrawNameButton->isChecked();
    int nm= (int) n;
    int optstats=leader+kurt+skew+integ+over+under+rms+mean+ent+nm;
    //cout <<"Set options stat to "<<optstats << endl;
    go4sett->setStatBoxVisible(drawbox);
    go4sett->setStatBoxErrors(drawerrors);
    go4sett->setOptStat(optstats);






}
