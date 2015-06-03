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
** Copyright ( C ) 2000 denis Bertini.  All rights reserved.
*****************************************************************************/

#include "tqcanvasmenu.h"

#include "qpopupmenu.h"
#include "qinputdialog.h"
#include "qcolordialog.h"

#include "Riostream.h"
#include "TH1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TClass.h"
#include "TDataType.h"
#include "TDataMember.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TMethodArg.h"
#include "TColor.h"
#include "TLatex.h"

#include "TGo4LockGuard.h"
#include "tqrootdialog.h"

TQCanvasMenu::TQCanvasMenu(QWidget* parent, TCanvas *canvas)
{
    c = canvas;
    fPopup = new QPopupMenu;
    fCurrObj = 0;
    fParent= parent;
    fTabWin = 0;
}

TQCanvasMenu::TQCanvasMenu(QWidget* parent, QWidget *tabWin, TCanvas *canvas)
{
   c=canvas;
   fPopup = new QPopupMenu;
   fParent = parent;
   fTabWin = tabWin;
   fCurrObj=0;
}

TQCanvasMenu::~TQCanvasMenu()
{
   if(fPopup) delete fPopup;
}

char* TQCanvasMenu::createDialogTitle( TObject *object, TMethod *method )
{
 // Create title for dialog box retrieving argument values.
   TGo4LockGuard threadlock;
   static char methodTitle[128];

  if (object && method)
    snprintf(methodTitle, 127, "%s::%s", object->ClassName(), method->GetName());
  else
    *methodTitle = 0;

  return methodTitle;

}

char* TQCanvasMenu::createArgumentTitle(TMethodArg *argument)
{
  // Create string describing argument (for use in dialog box).
   TGo4LockGuard threadlock;
   static Char_t argTitle[128];
  if (argument) {
    snprintf(argTitle, 127, "(%s)  %s", argument->GetTitle(), argument->GetName());
    if (argument->GetDefault() && *(argument->GetDefault())) {
      strcat(argTitle, "  [default: ");
      strcat(argTitle, argument->GetDefault());
      strcat(argTitle, "]");
    }
  } else
    *argTitle = 0;

  return argTitle;
}


void TQCanvasMenu::popup(TObject *obj, double x, double y, QMouseEvent *e)
{
   TGo4LockGuard threadlock;
   TClass *klass=obj->IsA();
   int curId=-1;

   fCurrObj=obj;
   fPopup->clear();
   fMethods.Clear();

   QString buffer=klass->GetName();
   buffer+="::";
   buffer+=obj->GetName();
   fPopup->insertItem(buffer, this, SLOT( execute(int) ), 0,curId); curId++;
   klass->GetMenuItems(&fMethods);
   fPopup->insertSeparator();

   if(!klass->InheritsFrom(TLatex::Class())) {
       fPopup->insertItem("Insert Latex", this, SLOT(execute(int)), 0, 100 );
       fPopup->insertSeparator();
   }

   if(klass->InheritsFrom(TH1::Class())) {
      fPopup->insertItem("Qt Hist Line Color ", this, SLOT(execute(int)), 0, 101 );
      fPopup->insertItem("Qt Hist Fill Color ", this, SLOT(execute(int)), 0, 102 );
      fPopup->insertSeparator();
   }

   TIter iter(&fMethods);
   TMethod *method=0;
   while ( (method = dynamic_cast<TMethod*>(iter())) != 0) {
      buffer=method->GetName();
      fPopup->insertItem(buffer, this, SLOT( execute(int) ), 0,curId);
       curId++;
   }

   MousePosX= x;  // hold the position where the mouse was clicked
   MousePosY= y;

   fPopup->popup(e->globalPos(), 0); /* let Qt decide how to draw the popup Menu otherwise we have a problem that
                    *   the visible rectangle can get outside the screen (M. Al-Turany 03.06.02)
                    */
}

void TQCanvasMenu::execute(int id)
{
   TGo4LockGuard threadlock;
   QString text("");
   bool ok = FALSE;
   if (id >=100) {
      switch (id){
         case 100: {
            TLatex *fxLatex = new TLatex();
            text = QInputDialog::getText(tr( "Qt Root" ),
                                         tr( "Please enter your text" ),
                                         QLineEdit::Normal, QString::null, &ok);
            //if (ok && !text.isEmpty())
            fxLatex->DrawLatex(MousePosX, MousePosY, text.latin1());
            emit MenuCommandExecuted(fxLatex, "DrawLatex");

            break;
        }
        case 101:
           ChangeHisLineColor();
           break;
        case 102 :
           ChangeHisFillColor();
           break;
      }
      gROOT->GetSelectedPad()->Update();
      gROOT->GetSelectedPad()->Modified();
      c->Modified();
      c->ForceUpdate();
      gROOT->SetFromPopUp( kFALSE );
   } else
   if (!(id < 0)) {

      // save global to Pad before calling TObject::Execute()

      TVirtualPad  *psave = gROOT->GetSelectedPad();
      TMethod *method= (TMethod *)fMethods.At(id);

      /// test: do this in any case!
      c->HandleInput(kButton3Up,gPad->XtoAbsPixel(MousePosX), gPad->YtoAbsPixel(MousePosY) );

      // change current dir that all new histograms appear here
      gROOT->cd();

      if (method->GetListOfMethodArgs()->First())
         dialog(fCurrObj, method);
      else {
         gROOT->SetFromPopUp(kTRUE);
         fCurrObj->Execute(method->GetName(), "");

         if (fCurrObj->TestBit(TObject::kNotDeleted)) {
            emit MenuCommandExecuted(fCurrObj, method->GetName());
         } else
            fCurrObj = 0;
      }
      c->GetPadSave()->Update();
      c->GetPadSave()->Modified();
      gROOT->SetSelectedPad(psave);
      ////qDebug("MENU:  gPad:%s gROOT:%s cur:%s \n",
      //gPad->GetName(), gROOT->GetSelectedPad()->GetName(), c->GetName());

      gROOT->GetSelectedPad()->Update();
      gROOT->GetSelectedPad()->Modified();
      c->Modified();
      c->ForceUpdate();
      gROOT->SetFromPopUp(kFALSE);
    }
}

void TQCanvasMenu::EmitMenuCommandExecuted(TObject* obj, const char* cmdname)
{
   emit MenuCommandExecuted(obj, cmdname);
}

void TQCanvasMenu::dialog(TObject* object, TMethod* method)
{
   TGo4LockGuard threadlock;
  // Create dialog object with OK and Cancel buttons. This dialog
   // prompts for the arguments of "method".

   if (!(object && method)) return;

   TQRootDialog* fDialog = new TQRootDialog(fParent, createDialogTitle(object, method),0,object,method);

   fDialog->setTCanvas(c);
  // iterate through all arguments and create apropriate input-data objects:
  // inputlines, option menus...
   TMethodArg *argument = 0;
   TIter next(method->GetListOfMethodArgs());

   while ((argument = (TMethodArg *) next())) {
     Text_t       *argname    = createArgumentTitle(argument);
     const char *type       = argument->GetTypeName();
     TDataType    *datatype   = gROOT->GetType(type);
     const char *charstar   = "char*";
     char          basictype [32];

     if (datatype) {
       strncpy(basictype, datatype->GetTypeName(),31);
     } else {
       if (strncmp(type, "enum", 4) != 0)
    std::cout << "*** Warning in Dialog(): data type is not basic type, assuming (int)\n";
       strcpy(basictype, "int");
      }

     if (strchr(argname, '*')) {
       strcat(basictype, "*");
       type = charstar;
     }

      TDataMember *m = argument->GetDataMember();
      if (m && m->GetterMethod()) {

   char gettername[256];
   strncpy(gettername, m->GetterMethod()->GetMethodName(),255);
   m->GetterMethod()->Init(object->IsA(), gettername, "");

   // Get the current value and form it as a text:

   char val[256];

   if (!strncmp(basictype, "char*", 5)) {
      char *tdefval;
      m->GetterMethod()->Execute(object, "", &tdefval);
      strncpy(val, tdefval, 255);
   } else if (!strncmp(basictype, "float", 5) ||
         !strncmp(basictype, "double", 6)) {
      Double_t ddefval;
      m->GetterMethod()->Execute(object, "", ddefval);
      snprintf(val, 255, "%g", ddefval);
   } else if (!strncmp(basictype, "char", 4) ||
                    !strncmp(basictype, "int", 3)  ||
         !strncmp(basictype, "long", 4) ||
         !strncmp(basictype, "short", 5)) {
      Long_t ldefval;
      m->GetterMethod()->Execute(object, "", ldefval);
            snprintf(val, 255, "%li", ldefval);
   }

   // Find out whether we have options ...

   TList *opt;
   if ((opt = m->GetOptions())) {
     //std::cout << "*** Warning in Dialog(): option menu not yet implemented " << opt << std::endl;
     // should stop dialog
     // workaround JAM: do not stop dialog, use textfield (for time display toggle)
                fDialog->add(argname, val, type);
     //return;
#if 0
     TMotifOptionMenu *o= new TMotifOptionMenu(argname);
     TIter nextopt(opt);
     TOptionListItem *it = 0;
     while ((it = (TOptionListItem*) nextopt())) {
       char *name  = it->fOptName;
       char *label = it->fOptLabel;
               Long_t value  = it->fValue;
               if (value != -9999) {
       char val[256];
       snprintf(val, 255, "%li", value);
       o->AddItem(name, val);
               }else
       o->AddItem(name, label);
     }
     o->SetData(val);
     fDialog->Add(o);
#endif
   } else {
     // we haven't got options - textfield ...
            fDialog->add(argname, val, type);
   }

   } else {    // if m not found ...
      TString argDflt = argument->GetDefault() ? argument->GetDefault() : "";

      if ((argDflt.Length() > 1) &&
          (argDflt[0]=='\"') && (argDflt[argDflt.Length()-1]=='\"')) {
         // cut "" from the string argument
         argDflt.Remove(0,1);
         argDflt.Remove(argDflt.Length()-1,1);
      }

      fDialog->add(argname, argDflt.Data(), type);

   }
   }

   fDialog->SetCanMenu(this);

   fDialog->popup();
}


void TQCanvasMenu::ChangeHisLineColor()
{
   TH1 *fxTH1 = (TH1*) fCurrObj;
   QColor col = QColorDialog::getColor();
   if ( col.isValid() ) {
      short int C_new =  TColor::GetColor(col.red(), col.green(), col.blue());
      if (fxTH1!=0) {
         fxTH1->SetLineColor(C_new);
         emit MenuCommandExecuted(fxTH1,"SetLineColor");
      }
   }
}

void TQCanvasMenu::ChangeHisFillColor()
{
   TH1 *fxTH1 = (TH1*) fCurrObj;
   QColor col = QColorDialog::getColor();
   if ( col.isValid() ) {
     short int C_new =  TColor::GetColor(col.red(), col.green(), col.blue());
     if (fxTH1!=0) {
         fxTH1->SetFillColor(C_new);
         emit MenuCommandExecuted(fxTH1,"SetFillColor");
     }
  }
}










