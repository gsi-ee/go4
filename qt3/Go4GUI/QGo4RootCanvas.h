#ifndef QGO4ROOTCANVAS_H
#define QGO4ROOTCANVAS_H

#include "tqrootcanvas.h"

class TPad;
class TCanvas;

class QGo4RootCanvas : public TQRootCanvas {
     Q_OBJECT

  public:

    QGo4RootCanvas(QWidget *parent = 0, const char *name = 0 ,TCanvas *c=0);
    virtual ~QGo4RootCanvas();

    bool showEventStatus() const;

  signals:
    void CanvasDropEvent(QDropEvent*, TPad*);
    void CanvasStatusEvent(const char*);
    void DoCanvasResize();
    void CanvasLeaveEvent();

  public slots:
    void setShowEventStatus(bool s);

  protected:

    virtual void performResize();

    virtual void dropEvent(QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

    virtual void leaveEvent(QEvent *);

  private:
    bool    fxShowEventStatus;
};

#endif //TGO4QROOTCANVAS_H
