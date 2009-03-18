#include "QGo4Table.h" 
#include "QGo4TablePlugin.h"
#include <QtPlugin>


QGo4TablePlugin::QGo4TablePlugin(QObject *parent)
  : QObject(parent)
{
  initialized = false;
}

void QGo4TablePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized)
      return;

  initialized = true;
}

bool QGo4TablePlugin::isInitialized() const
{
  return initialized;
}

QWidget *QGo4TablePlugin::createWidget(QWidget *parent)
{
  return new QGo4Table(parent);
}

QString QGo4TablePlugin::name() const
{
  return "QGo4Table";
}

QString QGo4TablePlugin::group() const
{
  return "Go4 Custom Widgets";
}

QIcon QGo4TablePlugin::icon() const
{
  return QIcon(QPixmap(":/go4logo2.png"));
}

QString QGo4TablePlugin::toolTip() const
{
  return "Go4 QTable (sort whole table after one column)";
}

QString QGo4TablePlugin::whatsThis() const
{
  return "Standard QTable with complete sorting feature";
}

bool QGo4TablePlugin::isContainer() const
{
  return false;
}

QString QGo4TablePlugin::domXml() const
{
  return "<widget class=\"QGo4Table\" name=\"QGo4Table\">\n"
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

QString QGo4TablePlugin::includeFile() const
{
  return "QGo4Table.h";
}


