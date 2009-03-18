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




void TGo4MacroDialog::setCommand( const QString & selection )
{
    if(selection.contains("Add"))
    {
        fxCommand="addhistos(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=",";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", 1, kTRUE);";
        SyntaxLabel->setText("Bool_t addhistos(const char* name1, const char* name2, Double_t factor, Bool_t draw)");
        QToolTip::add(this,"Add histograms: result=name1 + factor * name2. \n If draw==true, display in new viewpanel, otherwise just update existing displays");
    }
    else if (selection.contains("Divide"))
    {
        fxCommand="divhistos(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=",";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", kTRUE);";
        SyntaxLabel->setText("Bool_t divhistos(const char* name1, const char* name2, Bool_t draw)");

        QToolTip::add(this,"Divide histograms: result=name1 by name2.\n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
     else if (selection.contains("Rebin"))
    {
        fxCommand="rebin(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", 2, kTRUE);";
        SyntaxLabel->setText("Bool_t rebin(const char* name1, int ngroup, Bool_t draw)");

        QToolTip::add(this,"Rebin histogram name1 by mergin ngroup neighboured channels together. \n If draw=true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Projection X"))
    {
        fxCommand="projectionX(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", -1, -1, kTRUE);";
        SyntaxLabel->setText("Bool_t projectionX(const char* name1, Int_t firstybin, Int_t lastybin, Bool_t draw)");
        QToolTip::add(this," Do projection to X axis for 2d histogram name1.\n Parameters firstybin, lastybin for y range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
    else if (selection.contains("Projection Y"))
    {
        fxCommand="projectionY(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", -1, -1, kTRUE);";
        SyntaxLabel->setText("Bool_t projectionY(const char* name1, Int_t firstxbin, Int_t lastxbin, Bool_t draw)");
        QToolTip::add(this," Do projection to Y axis for 2d histogram name1. \n Parameters firstxbin, lastxbin for x range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
    else if (selection.contains("Correlate"))
    {
        fxCommand="corrhistos(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=",";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", kTRUE);";
        SyntaxLabel->setText("Bool_t corrhistos(const char* name1, const char* name2, Bool_t draw)");
        QToolTip::add(this," Correlate bin contents of two histograms in a graph.\n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
    else if (selection.contains("Histogram of"))
    {
        fxCommand="hishisto(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", 1000, kTRUE);";
         SyntaxLabel->setText("Bool_t hishisto(const char* name1, Int_t bins, Bool_t draw)");
        QToolTip::add(this," Create histogram and fill with contents of histogram name1. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
    else if (selection.contains("Profile X"))
    {
        fxCommand="profileX(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", -1, -1, kTRUE);";
         SyntaxLabel->setText("Bool_t profileX(const char* name1, Int_t firstybin, Int_t lastybin, Bool_t draw)");
        QToolTip::add(this," Do profile to X axis for 2d histogram name1. \n Parameters firstybin, lastybin for y range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
    else if (selection.contains("Profile Y"))
    {
        fxCommand="profileY(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", -1, -1, kTRUE);";
        SyntaxLabel->setText("Bool_t profileY(const char* name1, Int_t firstxbin, Int_t lastxbin, Bool_t draw)");
        QToolTip::add(this," Do profile to Y axis for 2d histogram name1. \n Parameters firstxbin, lastxbin for x range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");

    }
     else if (selection.contains("Scale X"))
    {
        fxCommand="scalex(";
        fxCommand+='"';
        fxCommand+='"';
        fxCommand+=", 1, 0, kTRUE);";
        SyntaxLabel->setText("Bool_t scalex(const char* name1, Double_t a1, Double_t a0, Bool_t draw)");
        QToolTip::add(this," Scale x axis of histogram name1 by linear function. \n Parameters: x'= a1*x + a0. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else
    {
       fxCommand="unknown macro function" ;
    }


}


const QString& TGo4MacroDialog::getCommand()
{
 return fxCommand;
}


void TGo4MacroDialog::init()
{
    setCommand("Add");
}



