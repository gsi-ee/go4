#ifndef QROOTWINDOW_H
#define QROOTWINDOW_H

#include <QtGui/qwidget.h>
#include <QtGui/QPaintEvent>
#include <QtCore/QEvent>
#include <QtGui/QCloseEvent>

#include "TVirtualX.h"

class QMouseEvent;
class QPaintEvent;
class TGLayoutHints;
class Event_t;
class TQRootFrame;
class TGFrame;
class TGCompositeFrame;

/** Qt widget wrapper for ROOT TGWindow class.
  * @author J.Adamczewski */

class QRootWindow : public QWidget {
   Q_OBJECT

   public:
      /** real ctor*/
      QRootWindow( QWidget *parent = 0, const char *name = 0, bool designermode=false);

      virtual ~QRootWindow();

      TGCompositeFrame* GetRootFrame();
      int GetRootWid() { return fiWinid;}

      /** Add root subframe into this widget */
      void AddSubFrame(TGFrame* f, TGLayoutHints* l=0);

      /** make this frame to current root master frame.*/
      void SetEditable(bool on=true);

      /** switch root frame resize mode on paint event:
       true=explicit resize, false=no resize of TQRootFrame*/
      void SetResizeOnPaint(bool on=true){fbResizeOnPaint=on;}

   protected:

      /** Event filter is used to pass Qt mouse events  root, after
        * translatein them in MapQMouseEvent. Note that root grabs gui buttons
        * via x11 directly, thus this mechanism is not used for the regular
        *  root widgets! */
      virtual bool eventFilter( QObject *, QEvent * );
      virtual void paintEvent( QPaintEvent *e );
      virtual void resizeEvent( QResizeEvent *e );
      virtual void closeEvent( QCloseEvent * e);

      //virtual QPaintEngine * paintEngine () const {return 0;}

      /** translate Qt mouse event into root event structure. */
      Event_t* MapQMouseEvent(QMouseEvent *e);

      /** this is the pseudo root window embedded into this widget.
        * is used as parent for subwindows */
      TQRootFrame *fxRootwindow;

      /** ROOT internal window index*/
      int fiWinid;
      /** X window id as received from Qt (may change during runtime?)*/
      Window_t fiXid;

      /** switch if paint event does explicit resize of ROOT frame (default)
       * for embedded TGedEditor, we better disable it and resize editor from
       * parent widget*/
      Bool_t fbResizeOnPaint;
};

#endif
