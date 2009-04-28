#include "QFitterWidget.h"

#include "QFitItem.h"
#include "TGo4Fitter.h"
#include "TGo4FitData.h"
#include "TGo4FitModel.h"


QFitterWidget::QFitterWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI
}

TGo4Fitter * QFitterWidget::GetFitter()
{
   return dynamic_cast<TGo4Fitter*> (GetObject());
}

void QFitterWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   if (GetFitter()) {
      int typ = GetFitter()->GetFitFunctionType();
      if (typ==TGo4Fitter::ff_user) typ = TGo4Fitter::ff_least_squares;
      FitFunctionTypeCmb->setCurrentIndex(typ);

      const char strend[3] = { 13, 10 , 0 };
      QString info("Associations:");
      info+=strend;
      for (Int_t ndata=0;ndata<GetFitter()->GetNumData();ndata++) {
         TGo4FitData* data = GetFitter()->GetData(ndata);
         info += data->GetName();
         info += ":";
         for(Int_t nmodel=0;nmodel<GetFitter()->GetNumModel();nmodel++) {
            TGo4FitModel* model = GetFitter()->GetModel(nmodel);
            if (model->IsAssignTo(data->GetName())) {
              info+=" ";
              info+=model->GetName();
            }
         }
         info+=strend;
      }
      InfoLbl->setText(info);
      InfoLbl->adjustSize();
   }
}

void QFitterWidget::FitFunctionTypeCmb_activated( int typ )
{
  if (!fbFillWidget && GetFitter())
    GetFitter()->SetFitFunctionType(typ);
}

