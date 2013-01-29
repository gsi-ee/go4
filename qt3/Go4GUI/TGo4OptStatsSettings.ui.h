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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void TGo4OptStatsSettings::init()
{
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
