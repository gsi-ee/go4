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

#ifndef TGO4EDITTORPLUGIN_H
#define TGO4EDITTORPLUGIN_H

#include <qwidgetplugin.h>

class TGo4QtDesignerPlugin : public QWidgetPlugin {
   public:
      TGo4QtDesignerPlugin();
      QStringList keys() const;
      QWidget* create( const QString &classname, QWidget* parent = 0, const char* name = 0 );
      QString group( const QString& ) const;
      QIconSet iconSet( const QString& ) const;
      QString includeFile( const QString& ) const;
      QString toolTip( const QString& ) const;
      QString whatsThis( const QString& ) const;
      bool isContainer( const QString& ) const;
};

#endif

