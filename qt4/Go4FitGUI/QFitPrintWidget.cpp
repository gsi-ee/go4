#include "QFitPrintWidget.h"

#include "Riostream.h"
#include "Rstrstream.h"
#include "TObject.h"

#include <sstream>
using namespace std;



QFitPrintWidget::QFitPrintWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{
   setupUi(this);
}


void QFitPrintWidget::SetDrawOption( const QString & option )
{
   fxDrawOption = option;
}

void QFitPrintWidget::FillSpecificData()
{
  if (GetObject()) {

      cout.flush();

      ostringstream strout;

      streambuf* cout_buffer = cout.rdbuf();

      cout.rdbuf(strout.rdbuf());

      GetObject()->Print(fxDrawOption.toAscii());

      cout << endl;

      cout.flush();

      cout.rdbuf(cout_buffer);

      PrintText->setText(strout.str().c_str());
  }
}
