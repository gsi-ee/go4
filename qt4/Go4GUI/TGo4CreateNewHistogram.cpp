#include "TGo4CreateNewHistogram.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TGo4QSettings.h"

TGo4CreateNewHistogram::TGo4CreateNewHistogram( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
   HisName->setText(go4sett->getHistName());
   HisTitle->setText("histogram title");

   int htype = go4sett->getHistType();
   HisClassType->setButton(htype / 10);
   HisType->setButton(htype % 10);

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
   const char* hname = HisName->text().latin1();
   const char* htitle = HisTitle->text().latin1();
   int htype = HisClassType->selectedId()*10 + HisType->selectedId();

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


