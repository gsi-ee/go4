#include "TGo4PrintWidget.h"
#include "TGo4QSettings.h"

TGo4PrintWidget::TGo4PrintWidget( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
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
