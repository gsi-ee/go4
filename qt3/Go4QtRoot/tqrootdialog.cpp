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

//Author : Denis Bertini 01.11.2000

/**************************************************************************
* Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI           *
*                     Planckstr. 1, 64291 Darmstadt, Germany              *
*                     All rights reserved.                                *
* Contact:            http://go4.gsi.de                                   *
*                                                                         *
* This software can be used under the license agreements as stated in     *
* Go4License.txt file which is part of the distribution.                  *
***************************************************************************/

#include "tqrootdialog.h"

#include <stdlib.h>

#include "Riostream.h"

#include "qevent.h"
#include "qpainter.h"
#include "qprinter.h"
#include "qtoolbar.h"
#include "qtoolbutton.h"
#include "qspinbox.h"
#include "qtooltip.h"
#include "qrect.h"
#include "qpoint.h"
#include "qcolordialog.h"
#include "qfiledialog.h"
#include "qcursor.h"
#include "qimage.h"
#include "qstrlist.h"
#include "qpopupmenu.h"
#include "qintdict.h"
#include "qpushbutton.h"
#include "qaction.h"
#include "qdial.h"
#include "qapplication.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qtoolbar.h"
#include "qtoolbutton.h"
#include "qmenubar.h"
#include "qkeycode.h"
#include "qfile.h"
#include "qfiledialog.h"
#include "qstatusbar.h"
#include "qmessagebox.h"
#include "qwhatsthis.h"
#include "qdialog.h"
#include "qlabel.h"
#include "qlineedit.h"

#include "TPad.h"
#include "TList.h"
#include "TObject.h"
#include "TROOT.h"
#include "TString.h"
#include "TH1.h"
#include "TList.h"
#include "TMethod.h"
#include "TCanvas.h"
#include "TDataType.h"
#include "TMethodCall.h"
#include "TPad.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TGo4LockGuard.h"
#include "tqcanvasmenu.h"

TQRootDialog::TQRootDialog(QWidget *parent, const char *name, WFlags f,
                          TObject* obj, TMethod *method ) :
   QVBox(parent,name, f | WType_Modal | WStyle_Dialog)
{
   TGo4LockGuard threadlock;
   fCurObj=obj;
   fCurMethod=method;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::Expanding));

  fArgBox = new QVBox(this, "args");


  fArgBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                 QSizePolicy::Expanding));

  QHBox *hbox = new QHBox(this,"buttons");
  QPushButton *bOk = new QPushButton("Apply",hbox,"apply");
  QPushButton *bCancel = new QPushButton("Cancel",hbox,"close");
  connect(bCancel,SIGNAL (clicked()), this, SLOT(close()));
  connect(bOk,SIGNAL( clicked() ), this, SLOT( executeMethod() ));

//  QPushButton *bOk = new QPushButton("Ok",hbox,"ok");
//   QPushButton *bCancel = new QPushButton("Cancel",hbox,"cancel");
//   connect(bCancel,SIGNAL (clicked()), this, SLOT(close()));
//   connect(bOk,SIGNAL( clicked() ), this, SLOT( executeMethod() ));


  fPrevMethod=0;
  fPrevObj=0;
  fParent = parent;
  fMenu = 0;
}

TQRootDialog::~TQRootDialog()
{
   if(fArgBox) delete fArgBox;
   aList.remove();
}

void TQRootDialog::receive()
{
   executeMethod();
}

void TQRootDialog::executeMethod()
{
   TGo4LockGuard threadlock;
   Bool_t deletion = kFALSE;

   qDebug("DIAL executeMethod:  simple version\n");
   TVirtualPad *psave =  gROOT->GetSelectedPad();

   qDebug("DIAL saved pad: %s gPad:%s \n",psave->GetName(),gPad->GetName());

   qDebug(" fCurObj %p \n", fCurObj);
   if (fCurObj)
      qDebug("DIAL obj:%s meth:%s \n",fCurObj->GetName(), fCurMethod->GetName());

   //std::cout<< "executeMethod" << fCurMethod->GetName() << std::endl;

   TObjArray tobjlist(fCurMethod->GetListOfMethodArgs()->LastIndex()+1);
   for ( QLineEdit* st = aList.first(); st; st = aList.next()) {
      QString s = st->text();
      qDebug( "** QString values (first ) :%s \n", (const char*) s );
      TObjString *t = new TObjString( (const char*) s );
      tobjlist.AddLast((TObject*) t) ;
   }
     // handle command if existing object
   if (fCurObj != 0) {
     if(strcmp(fCurMethod->GetName(),"Delete") == 0) {
         // here call explicitely the dtor
         qDebug(" DIAL obj name deleted :%s \n", fCurObj->GetName());
         if (fCurObj) {
            if (fMenu!=0)
               fMenu->EmitMenuCommandExecuted(fCurObj,"Delete");
            delete fCurObj;
            fCurObj = 0;
            deletion = kTRUE;
         }

       qDebug(" DIAL deletion done closing ... \n");

     } else
     if (strcmp(fCurMethod->GetName(), "SetCanvasSize") == 0) {
        int value[2];
        int l=0;
        for ( QLineEdit* st = aList.first(); st; st = aList.next()) {
           QString s = st->text();
           qDebug( "** QString values (first ) :%s \n", (const char*) s );
           value[l++] = atoi ( s );
        }
        qDebug( " do resize with %i %i \n", value[0], value[1]);
        fParent->resize(value[0],value[1]);
        if (fMenu!=0)
          fMenu->EmitMenuCommandExecuted(fCurCanvas, "SetCanvasSize");
     } else {
       // here call cint call
        qDebug("TCint::Execute called !\n");

        fCurObj->Execute(fCurMethod, &tobjlist);

        if (fCurObj->TestBit(TObject::kNotDeleted)) {
           if (fMenu!=0)
              fMenu->EmitMenuCommandExecuted(fCurObj, fCurMethod->GetName());
        } else {
           deletion = TRUE;
           fCurObj = 0;
        }
     }

   } // ! fCurrent Obj

   if(!deletion ) {
      qDebug("DIAL set saved pad: %s herit:%s gPad:%s\n",
             psave->GetName(), psave->ClassName(), gPad->GetName());
      gROOT->SetSelectedPad(psave);
      gROOT->GetSelectedPad()->Modified();
      gROOT->GetSelectedPad()->Update();
      qDebug("DIAL update done on %s \n", gROOT->GetSelectedPad()->GetName());
   } else {
      gROOT->SetSelectedPad( gPad );
      gROOT->GetSelectedPad()->Update();
   }

   close();
}

void TQRootDialog::add(const char* argname, const char* value, const char* type)
{
   TGo4LockGuard threadlock;

   new QLabel(argname,fArgBox);
   QLineEdit* le = new  QLineEdit(fArgBox);
   le->setGeometry(10,10, 130, 30);
   le->setFocus();
   le->setText(value);
   aList.append( le );

#if DEBUG_LEVEL>2
  qDebug("TQRootDialog::add %s %s %s \n", argname, value, type);
#endif

}

void TQRootDialog::popup()
{
   TGo4LockGuard threadlock;
   show();
}

void TQRootDialog::closeEvent( QCloseEvent* ce )
{
   TGo4LockGuard threadlock;
   ce->accept();
   delete this;
}
