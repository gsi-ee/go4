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

#ifndef TQCANVASMENU_H
#define TQCANVASMENU_H

#include "qobject.h"
#include "TList.h"

class TCanvas;
class QPopupMenu;
class TMethodArg;

/**
*
* TQCanvasMenu
*
* This class provides an interface to  Qt based context sensitive popup menus.
* These menus pop up when the user hits the right mouse button, and
* are destroyed when the menu pops downs.
*
* @short interface to context sensitive menu
*
* @see QObject, TQCanvasMenu, TQPopupMenu, TQRootdialog
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*/


class TQCanvasMenu : public QObject {
   Q_OBJECT

   public:
      TQCanvasMenu(QWidget* parent = 0, TCanvas *canvas = 0);
      TQCanvasMenu(QWidget* parent, QWidget *tabWin, TCanvas *canvas);
      virtual ~TQCanvasMenu();

      /** Perform the corresponding selected TObject  popup
        * in the position defined
        * by x, y coordinates (in user coordinate system).
        * @param obj (TObject*)
        * @param p (QPoint&) */
      void popup(TObject *obj, double x, double y, QMouseEvent *e);

      /** Create a dialog object with OK and Cancel buttons.
        * This dialog prompts for the arguments of "method"
        * @param obj (TObject*)
        * @param method (TMethod*) */
      void dialog(TObject *obj, TMethod* method);

      /** Create a dialog title */
      char* createDialogTitle(TObject *object, TMethod *method);

      /** Create string describing argument (for use in dialog box). */
      char* createArgumentTitle(TMethodArg *argument);

      void ChangeHisLineColor();
      void ChangeHisFillColor();

   signals:
      void MenuCommandExecuted(TObject*, const char*);

   public slots:

      /**  Slot defined to execute a method from a selected TObject
        * using TObject::Execute() function. */
      void execute(int id);
      void EmitMenuCommandExecuted(TObject*, const char*);

   protected:
      TObject*         fCurrObj;
      QPopupMenu*      fPopup;
      TList            fMethods;
      TCanvas*         c;
      QWidget*         fParent;
      QWidget*         fTabWin;
      double           MousePosX;    // mouse position in user coordinate
      double           MousePosY;    // mouse position in user coordinate
};

#endif
