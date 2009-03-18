#ifndef TQROOTWINDOW_H
#define TQROOTWINDOW_H

#include "qwidget.h"

class QMouseEvent;
class QPaintEvent;
class TGLayoutHints;
class Event_t;
class TQRootFrame;
class TGFrame;
class TGCompositeFrame;

/** Qt widget wrapper for ROOT TGWindow class.
  * @author J.Adamczewski */

class TQRootWindow : public QWidget {
   Q_OBJECT

   public:
      TQRootWindow( QWidget *parent = 0, const char *name = 0);
      virtual ~TQRootWindow();

      TGCompositeFrame* GetRootFrame();
      int GetRootWid() { return fiWinid;}

      /** Add root subframe into this widget */
      void AddSubFrame(TGFrame* f, TGLayoutHints* l=0);

      /** make this frame to current root master frame.*/
      void SetEditable(bool on=true);

   protected:

      /** Event filter is used to pass Qt mouse events  root, after
        * translatein them in MapQMouseEvent. Note that root grabs gui buttons
        * via x11 directly, thus this mechanism is not used for the regular
        *  root widgets! */
      virtual bool eventFilter( QObject *, QEvent * );
      virtual void paintEvent( QPaintEvent *e );
      virtual void closeEvent( QCloseEvent * e);

      /** translate Qt mouse event into root event structure. */
      Event_t* MapQMouseEvent(QMouseEvent *e);

      /** this is the pseudo root window embedded into this widget.
        * is used as parent for subwindows */
      TQRootFrame *fxRootwindow;
      int fiWinid;
};

#endif
