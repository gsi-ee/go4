#include "QGo4LineEdit.h"
#include "QGo4LineEditPlugin.h"
#include <QtPlugin>


QGo4LineEditPlugin::QGo4LineEditPlugin(QObject *parent)
  : QObject(parent)
{
  initialized = false;
}

void QGo4LineEditPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool QGo4LineEditPlugin::isInitialized() const
{
  return initialized;
}

QWidget *QGo4LineEditPlugin::createWidget(QWidget *parent)
{
  return new QGo4LineEdit(parent);
}

QString QGo4LineEditPlugin::name() const
{
  return "QGo4LineEdit";
}

QString QGo4LineEditPlugin::group() const
{
  return "Go4 Custom Widgets";
}

QIcon QGo4LineEditPlugin::icon() const
{
  return QIcon(":/go4logo2.png");
}

QString QGo4LineEditPlugin::toolTip() const
{
  return "Go4 QLineEdit";
}

QString QGo4LineEditPlugin::whatsThis() const
{
  return "Standard QLineEdit with additional drag & drop capabilities";
}

bool QGo4LineEditPlugin::isContainer() const
{
  return false;
}

QString QGo4LineEditPlugin::domXml() const
{
  return "<widget class=\"QGo4LineEdit\" name=\"QGo4LineEdit\">\n"
         " <property name=\"geometry\">\n"
         "  <rect>\n"
         "   <x>0</x>\n"
         "   <y>0</y>\n"
         "   <width>100</width>\n"
         "   <height>100</height>\n"
         "  </rect>\n"
         " </property>\n"
         "</widget>\n";
}

QString QGo4LineEditPlugin::includeFile() const
{
  return "QGo4LineEdit.h";
}


