/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void TGo4PrintWidget::init()
{
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
