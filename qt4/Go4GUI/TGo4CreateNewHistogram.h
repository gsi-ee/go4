#ifndef TGO4CREATENEWHISTOGRAM_H
#define TGO4CREATENEWHISTOGRAM_H


#include <QDialog>
#include "ui_TGo4CreateNewHistogram.h"

class QButtonGroup;
class TH1;

class TGo4CreateNewHistogram : public QDialog, public Ui::TGo4CreateNewHistogram
 {
     Q_OBJECT

 public:
     TGo4CreateNewHistogram( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
	 void SetAnalysisAvaliable(bool on);

	void SetLocalAvaliable(bool on);

	int GetSelectedCmd();

	TH1* MakeHistogram();

 public slots:

	virtual void CreateLocalHist();

	virtual void CreateRemoteHis();

 protected:
 	int fSelectedCmd;

 	QButtonGroup  *HisTypeGrp;
 	QButtonGroup  *HisClassGrp;

 };
#endif
