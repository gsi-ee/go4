/// \file rootwebview.h
/// \ingroup CanvasPainter ROOT7
/// \author Sergey Linev <S.Linev@gsi.de>
/// \date 2017-06-29
/// \warning This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback
/// is welcome!

/*************************************************************************
 * Copyright (C) 1995-2017, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RootWebView
#define ROOT_RootWebView

#include <QWebEngineView>

class RootWebView : public QWebEngineView {
   Q_OBJECT
protected:
   unsigned fWidth;
   unsigned fHeight;

   virtual void closeEvent(QCloseEvent *);

public slots:
   void onWindowCloseRequested();

public:
   RootWebView(QWidget *parent = 0, unsigned width = 0, unsigned height = 0);
   virtual ~RootWebView();

   virtual QSize  sizeHint() const;
};

#endif
