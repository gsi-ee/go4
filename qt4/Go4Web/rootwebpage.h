/// \file rootwebpage.h
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

#ifndef ROOT_RootWebPage
#define ROOT_RootWebPage

#include <QWebEnginePage>

class RootWebPage : public QWebEnginePage {
   Q_OBJECT
protected:
   virtual void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
                                         int lineNumber, const QString &sourceID);

public:
   RootWebPage(QObject *parent = 0) : QWebEnginePage(parent) {}
   virtual ~RootWebPage() {}
};

#endif
