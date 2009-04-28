#ifndef TGO4LOADEDLIBRARIES_H
#define TGO4LOADEDLIBRARIES_H

#include <QDialog>
#include "ui_TGo4LoadedLibraries.h"

class TGo4LoadedLibraries : public QDialog, public Ui::TGo4LoadedLibraries
 {
     Q_OBJECT

 public:
     TGo4LoadedLibraries( QWidget* parent = 0 );

 public slots:

	virtual void LoadNewLibrary();

	virtual void UnloadLibrary();

	virtual void RefreshLibs();

 };

#endif
