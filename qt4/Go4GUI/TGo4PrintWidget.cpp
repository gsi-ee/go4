#include "TGo4PrintWidget.h"
#include "TGo4QSettings.h"

TGo4PrintWidget::TGo4PrintWidget( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4PrintWidget");
   setupUi(this);
   QString PdefName,PdefProg;
   go4sett->getPrinterSett(PdefName,PdefProg);
   PrinterName->setText(PdefName);
   PrintProg->setText(PdefProg);
}

QString TGo4PrintWidget::GetPrinter()
{
    return PrinterName->text();
}

QString TGo4PrintWidget::GetPrintProg()
{
    return PrintProg->text();
}
