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

#include "TGo4OptStatsSettings.h"

#include "TStyle.h"
#include "TMath.h"

#include "TGo4Condition.h"
#include "TGo4Marker.h"
#include "TGo4QSettings.h"


TGo4OptStatsSettings::TGo4OptStatsSettings( QWidget* parent ) :
   QDialog( parent )
{
   //setObjectName(name);
   setupUi(this);

   bool showstat = go4sett->getStatBoxVisible();
   DrawStatboxButton->setChecked(showstat);
   OptStatGroupBox->setEnabled(showstat);

   fStatW = go4sett->getOptStatW()*0.01;
   fStatH = go4sett->getOptStatH()*0.01;

   if (fStatW<0.005) fStatW = gStyle->GetStatW();
   if (fStatH<0.005) fStatH = gStyle->GetStatH();

   StatWidthBox->setValue(fStatW);
   StatHeightBox->setValue(fStatH);

   int optstats = go4sett->getOptStat();
   //std::cout <<"Found options stat: "<<optstats << std::endl;
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
   //std::cout <<"Set options stat to "<<optstats << std::endl;
   go4sett->setStatBoxVisible(drawbox);
   go4sett->setStatBoxErrors(drawerrors);
   go4sett->setOptStat(optstats);

   if (TMath::Abs(fStatW-StatWidthBox->value())>0.005)
      go4sett->setOptStatW((int)(StatWidthBox->value()*100));

   if (TMath::Abs(fStatH-StatHeightBox->value())>0.005)
      go4sett->setOptStatH((int)(StatHeightBox->value()*100));
}
