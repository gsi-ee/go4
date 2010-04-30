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
        fxCommand="addhistos(\"\",\"\",1,kTRUE);";
        SyntaxLabel->setText("Bool_t addhistos(const char* histo1, const char* histo2, Double_t factor, Bool_t draw)");
        QToolTip::add(this,"Add histograms: result=histo1 + factor * histo2. \n If draw==true, display in new viewpanel, otherwise just update existing displays");
    }
    else if (selection.contains("Divide"))
    {
        fxCommand="divhistos(\"\",\"\",kTRUE);";
        SyntaxLabel->setText("Bool_t divhistos(const char* histo1, const char* histo2, Bool_t draw)");
        QToolTip::add(this,"Divide histograms: result=histo1 by histo2.\n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
     else if (selection.contains("Rebin"))
    {
        fxCommand="rebin(\"\",2,0,kTRUE);";
        SyntaxLabel->setText("Bool_t rebin(const char* histo, int ngroupX, int ngroupY, Bool_t draw)");
        QToolTip::add(this,"Rebin histogram histo by merging ngroup neighboured channels together. \n If draw=true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Projection X"))
    {
        fxCommand="projectionX(\"\",\"\",-1,-1,kTRUE);";
        SyntaxLabel->setText("Bool_t projectionX(const char* histo, const char* polycon, Int_t firstybin, Int_t lastybin, Bool_t draw)");
        QToolTip::add(this," Do projection to X axis for 2d histogram histo.\n Optional polygon condition.\n Parameters firstybin, lastybin for y range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Projection Y"))
    {
        fxCommand="projectionY(\"\",\"\",-1,-1,kTRUE);";
        SyntaxLabel->setText("Bool_t projectionY(const char* histo, const char* polycon, Int_t firstxbin, Int_t lastxbin, Bool_t draw)");
        QToolTip::add(this," Do projection to Y axis for 2d histogram histo. \n Optional polygon condition.\n Parameters firstxbin, lastxbin for x range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Correlate"))
    {
        fxCommand="corrhistos(\"\",\"\",kTRUE);";
        SyntaxLabel->setText("Bool_t corrhistos(const char* histo1, const char* histo2, Bool_t draw)");
        QToolTip::add(this," Correlate bin contents of two histograms in a graph.\n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Histogram of"))
    {
        fxCommand="hishisto(\"\",1000,kTRUE);";
        SyntaxLabel->setText("Bool_t hishisto(const char* histo, Int_t bins, Bool_t draw)");
        QToolTip::add(this," Create histogram and fill with contents of histogram histo. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Profile X"))
    {
        fxCommand="profileX(\"\",-1,-1,kTRUE);";
        SyntaxLabel->setText("Bool_t profileX(const char* histo, Int_t firstybin, Int_t lastybin, Bool_t draw)");
        QToolTip::add(this," Do profile to X axis for 2d histogram histo. \n Parameters firstybin, lastybin for y range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
    else if (selection.contains("Profile Y"))
    {
        fxCommand="profileY(\"\",-1,-1,kTRUE);";
        SyntaxLabel->setText("Bool_t profileY(const char* histo, Int_t firstxbin, Int_t lastxbin, Bool_t draw)");
        QToolTip::add(this," Do profile to Y axis for 2d histogram histo. \n Parameters firstxbin, lastxbin for x range. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
    }
     else if (selection.contains("Scale X"))
    {
        fxCommand="scalex(\"\",1,0,kTRUE);";
        SyntaxLabel->setText("Bool_t scalex(const char* histo, Double_t a1, Double_t a0, Bool_t draw)");
        QToolTip::add(this," Scale x axis of histogram histo by linear function. \n Parameters: x'= a1*x + a0. \n If draw==true, display in new viewpanel, otherwise just update existing displays ");
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



