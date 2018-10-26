/// \file rootwebview.cpp
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

#include "rootwebview.h"
#include "rootwebpage.h"

#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

RootWebView::RootWebView(QWidget *parent, unsigned width, unsigned height) :
   QWebEngineView(parent),
   fWidth(width),
   fHeight(height)
{
   setPage(new RootWebPage());

   connect(page(), &QWebEnginePage::windowCloseRequested, this, &RootWebView::onWindowCloseRequested);

   connect(page(), &QWebEnginePage::loadFinished /*   loadStarted */, this, &RootWebView::onLoadStarted);

   setAcceptDrops(true);
}

RootWebView::~RootWebView()
{
}

QSize RootWebView::sizeHint() const
{
   if (fWidth && fHeight) return QSize(fWidth, fHeight);
   return QWebEngineView::sizeHint();
}

void RootWebView::dragEnterEvent( QDragEnterEvent *e )
{
   if (e->mimeData()->hasText())
      e->acceptProposedAction();
}


void RootWebView::dropEvent(QDropEvent* event)
{
   emit drop(event);
}

void RootWebView::closeEvent(QCloseEvent *)
{
   page()->runJavaScript("if (window && window.onqt5unload) window.onqt5unload();");
}

void RootWebView::onLoadStarted()
{
   page()->runJavaScript("var jsroot_qt5_identifier = true;");
   page()->runJavaScript("window.jsroot_qt5_identifier = true;");
   page()->runJavaScript("console.log('window type = ' + typeof window + '  1: ' + typeof jsroot_qt5_identifier + '   2: ' +  typeof window.jsroot_qt5_identifier);");

   printf("RootWebView::onLoadStarted\n");
}

void RootWebView::onWindowCloseRequested()
{
   close();
}
