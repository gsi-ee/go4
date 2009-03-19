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

#include "Riostream.h"
#include <stdlib.h>

#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qprinter.h>
#include <QtGui/qtoolbutton.h>
#include <QtGui/qspinbox.h>
#include <QtGui/qtooltip.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qcursor.h>
#include <QtGui/qimage.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qaction.h>
#include <QtGui/qdial.h>
#include <QtGui/qapplication.h>
#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qtoolbutton.h>
#include <QtGui/qmenubar.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qfile.h>
#include <QtGui/qstatusbar.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qdialog.h>
#include <QtGui/qlabel.h>
#include <QtGui/qlineedit.h>
#include <QtGui/QCloseEvent>

#include <Qt3Support/q3filedialog.h>
#include <Qt3Support/q3strlist.h>
#include <Qt3Support/q3popupmenu.h>
#include <Qt3Support/q3intdict.h>
#include <Qt3Support/q3toolbar.h>
#include <Qt3Support/q3filedialog.h>
#include <Qt3Support/q3whatsthis.h>
#include <Qt3Support/q3toolbar.h>

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

#include "lockguard.h"
#include "tqcanvasmenu.h"

TQRootDialog::TQRootDialog(QWidget *parent, const char *name, Qt::WFlags f,
                          TObject* obj, TMethod *method ) :
   Q3VBox(parent,name, f | (Qt::WType_Dialog | Qt::WShowModal) | Qt::WType_Dialog)
{
   Qtrootlockguard threadlock;
   fCurObj=obj;
   fCurMethod=method;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::Expanding));

  fArgBox = new Q3VBox(this, "args");


  fArgBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                 QSizePolicy::Expanding));

  Q3HBox *hbox = new Q3HBox(this,"buttons");
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
   Qtrootlockguard threadlock;
   Bool_t deletion = kFALSE;

   qDebug("DIAL executeMethod:  simple version\n");
   TVirtualPad *psave =  gROOT->GetSelectedPad();

   qDebug("DIAL saved pad: %s gPad:%s \n",psave->GetName(),gPad->GetName());

   qDebug(" fCurObj %p \n", fCurObj);
   if (fCurObj)
      qDebug("DIAL obj:%s meth:%s \n",fCurObj->GetName(), fCurMethod->GetName());

   //cout<< "executeMethod" << fCurMethod->GetName() << endl;

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
}

void TQRootDialog::add(const char* argname, const char* value, const char* type)
{
   Qtrootlockguard threadlock;

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
   Qtrootlockguard threadlock;
   show();
}

void TQRootDialog::closeEvent( QCloseEvent* ce )
{
   Qtrootlockguard threadlock;
   ce->accept();
   delete this;
}
