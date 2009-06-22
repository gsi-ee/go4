#include "TGo4CreateNewHistogram.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TGo4QSettings.h"
#include <QButtonGroup>

TGo4CreateNewHistogram::TGo4CreateNewHistogram( QWidget* parent)
    : QDialog( parent)
{
	setObjectName("Go4CreateNewHistogram");
	setupUi(this);
   HisName->setText(go4sett->getHistName());
   HisTitle->setText("histogram title");

   HisClassGrp = new QButtonGroup(this);
   HisClassGrp->addButton(TH1_b, 1);
   HisClassGrp->addButton(TH2_b, 2);
   HisClassGrp->addButton(TH3_b, 3);

   HisTypeGrp = new QButtonGroup(this);
   HisTypeGrp->addButton(S_b, 1);
   HisTypeGrp->addButton(D_b, 2);
   HisTypeGrp->addButton(F_b, 3);
   HisTypeGrp->addButton(I_b, 4);
   HisTypeGrp->addButton(C_b, 5);

   int htype = go4sett->getHistType();
   HisClassGrp->button(htype / 10)->setChecked(true);
   HisTypeGrp->button(htype % 10)->setChecked(true);

   int npoints;
   double min, max;

   go4sett->getHistAxisPars(0, npoints, min, max);
   XNoOfBins->setText(QString::number(npoints));
   Xmin->setText(QString::number(min));
   Xmax->setText(QString::number(max));

   go4sett->getHistAxisPars(1, npoints, min, max);
   YNoOfBins->setText(QString::number(npoints));
   Ymin->setText(QString::number(min));
   Ymax->setText(QString::number(max));

   go4sett->getHistAxisPars(2, npoints, min, max);
   ZNoOfBins->setText(QString::number(npoints));
   Zmin->setText(QString::number(min));
   Zmax->setText(QString::number(max));

   fSelectedCmd = 0;
}

void TGo4CreateNewHistogram::SetAnalysisAvaliable(bool on)
{
    CearteRemote->setEnabled(on);
}

void TGo4CreateNewHistogram::SetLocalAvaliable(bool on)
{
    CreateHistogram->setEnabled(on);
}

int TGo4CreateNewHistogram::GetSelectedCmd()
{
   return fSelectedCmd;
}

TH1* TGo4CreateNewHistogram::MakeHistogram()
{
   QByteArray ba_hname = HisName->text().toAscii();
   QByteArray ba_htitle = HisTitle->text().toAscii();
   const char* hname = ba_hname.constData();
   const char* htitle = ba_htitle.constData();

   int htype = HisClassGrp->checkedId()*10 + HisTypeGrp->checkedId();

   int nxbins = XNoOfBins->text().toInt();
   int nybins = YNoOfBins->text().toInt();
   int nzbins = ZNoOfBins->text().toInt();

   double xmin = Xmin->text().toDouble();
   double xmax = Xmax->text().toDouble();
   double ymin = Ymin->text().toDouble();
   double ymax = Ymax->text().toDouble();
   double zmin = Zmin->text().toDouble();
   double zmax = Zmax->text().toDouble();

   go4sett->setHistName(hname);
   go4sett->setHistType(htype);
   go4sett->setHistAxisPars(0, nxbins, xmin, xmax);
   go4sett->setHistAxisPars(1, nybins, ymin, ymax);
   go4sett->setHistAxisPars(2, nzbins, zmin, zmax);

   TDirectory* savdir = gDirectory;
   gROOT->cd();
   TH1* h = 0;
   switch(htype) {
      case 11: h = new TH1S(hname, htitle, nxbins, xmin, xmax); break;
      case 12: h = new TH1D(hname, htitle, nxbins, xmin, xmax); break;
      case 13: h = new TH1F(hname, htitle, nxbins, xmin, xmax); break;
      case 14: h = new TH1I(hname, htitle, nxbins, xmin, xmax); break;
      case 15: h = new TH1C(hname, htitle, nxbins, xmin, xmax); break;
      case 21: h = new TH2S(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 22: h = new TH2D(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 23: h = new TH2F(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 24: h = new TH2I(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 25: h = new TH2C(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax); break;
      case 31: h = new TH3S(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 32: h = new TH3D(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 33: h = new TH3F(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 34: h = new TH3I(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
      case 35: h = new TH3C(hname, htitle, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); break;
   }

   if (savdir!=0) savdir->cd();

   return h;
}

void TGo4CreateNewHistogram::CreateLocalHist()
{
   fSelectedCmd = 1;
   accept();
}

void TGo4CreateNewHistogram::CreateRemoteHis()
{
   fSelectedCmd = 2;
   accept();
}


