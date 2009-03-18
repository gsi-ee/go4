#include "TGo4QtDesignerPlugin.h"

#include "QGo4Widget.h"
#include "QGo4LineEdit.h"
#include "QGo4Table.h"
#include "QGo4BrowserListView.h"
//#include "QGo4RootCanvas.h"
#include "QFitWidget.h"
#include "QFitNamedWidget.h"
#include "QFitModelWidget.h"
#include "QGo4CommandsHistory.h"

static const char *go4logo2[] = {
/* width height num_colors chars_per_pixel */
"    38    37       35            1",
/* colors */
". c #ffffff",
"# c #006600",
"a c #009900",
"b c #3333cc",
"c c #339933",
"d c #33cc33",
"e c #6666cc",
"f c #66cc66",
"g c #66ff33",
"h c #9999cc",
"i c #99cc99",
"j c #99cccc",
"k c #99ff66",
"l c #99ff99",
"m c #cc0000",
"n c #cc3333",
"o c #cc6666",
"p c #cc9900",
"q c #cc9966",
"r c #cc9999",
"s c #cccccc",
"t c #ccccff",
"u c #ccff33",
"v c #ccff66",
"w c #ccff99",
"x c #ccffcc",
"y c #ff9999",
"z c #ffcc00",
"A c #ffcc66",
"B c #ffcc99",
"C c #ffcccc",
"D c #ffccff",
"E c #ffff99",
"F c #ffffcc",
"G c #ffffff",
/* pixels */
"......................................",
"......................................",
"......................................",
"..........................ymmmmmy.....",
"..........zzzzzzF........mmmmmmmmm....",
"........FzzzzzzzzB......mmmnC.Commo...",
".......GzzzF..Fzzz.....Cmmr.....rmm...",
".......zzz......zzE....mmm.......mm...",
"......FzzG......zzz...Gmm........mm...",
"......zzz.......zzz...rmm........mm...",
"......zzF.......zzz...mmm.......Cmm...",
"......zz........zzExglmmm.......mmm...",
"......zz.......Fzzvgggpmm......CmmC...",
"......zzB......zzzgggggmmm....Cmmm....",
"......AzzF....Azzugk...qmmmmmmmmm.....",
".......zzzzzzzzzzgg.....Cmmmmmmm......",
"........zzzzzzzAggG.......CnmnC.......",
".........GBzzE.ggg....................",
"...............ggx.wgggggg............",
"...............gg.kggggggk............",
"...............gg..G...ggjbbbbbh......",
"........aaaaa#xggx....gggbbbbbbbb.....",
"......s########dggggggggbbeG.Ghbbh....",
".....Gaaas..saaagggggggbbs.....hbb....",
".....aaa......aaixggwGbbb.......bb....",
"....xaaG......###....Dbb........bb....",
"....###.......###....hbb........bb....",
"....##x.......###....bbb.......tbb....",
"....aa........aai....bbb.......bbb....",
"....aa.......GaaG....tbb......tbbt....",
"....aai......###......bbb....Gbbb.....",
"....faas....c##D......tbbbbbbbbb......",
".....aa##aaaaai........tbbbbbbb.......",
"......aaaaaaai...........tebet........",
".......xiaai..........................",
"......................................",
"......................................"
};

TGo4QtDesignerPlugin::TGo4QtDesignerPlugin()
{
}

QStringList TGo4QtDesignerPlugin::keys() const
{
   QStringList list;
   list << "QGo4Widget";
   list << "QGo4LineEdit";
   list << "QGo4Table";
   list << "QGo4BrowserListView";
//   list << "QGo4RootCanvas";
   list << "QFitWidget";
   list << "QFitNamedWidget";
   list << "QFitModelWidget";
   list << "QGo4CommandsHistory";
   return list;
}

QWidget* TGo4QtDesignerPlugin::create( const QString &classname, QWidget* parent, const char* name)
{
   if ( classname == "QGo4Widget" )
      return new QGo4Widget( parent, name );
   if (classname == "QGo4LineEdit")
      return new QGo4LineEdit(parent, name);
   if (classname == "QGo4Table")
      return new QGo4Table(parent, name);
   if (classname == "QGo4BrowserListView")
      return new QGo4BrowserListView(parent, name);
//   if (classname == "QGo4RootCanvas")
//      return new QGo4RootCanvas(parent, name);
   if (classname == "QFitWidget")
      return new QFitWidget(parent, name);
   if (classname == "QFitNamedWidget")
      return new QFitNamedWidget(parent, name);
   if (classname == "QFitModelWidget")
      return new QFitModelWidget(parent, name);
    if (classname == "QGo4CommandsHistory")
      return new QGo4CommandsHistory(parent, name);
   return 0;
}

QString TGo4QtDesignerPlugin::group(const QString& classname) const
{
   return "Go4 widgets";
   if ( classname == "QGo4Widget" )
      return "Containers";
   if ( classname == "QGo4LineEdit" )
      return "Input";
   if (classname == "QGo4BrowserListView")
      return "Views";
   if (classname == "QGo4RootCanvas")
      return "Views";
   if ( classname == "QFitWidget" )
      return "Containers";
   if ( classname == "QFitNamedWidget" )
      return "Containers";
   if ( classname == "QFitModelWidget" )
      return "Containers";
    if ( classname == "QGo4CommandsHistory" )
      return "Input";

   return QString::null;
}

QIconSet TGo4QtDesignerPlugin::iconSet( const QString& ) const
{
//   if ( feature == "QGo4Widget" )
//      return QIconSet( QPixmap("parameter.png"));
   return QIconSet(QPixmap(go4logo2));
}

QString TGo4QtDesignerPlugin::includeFile(const QString& classname) const
{
   if ( classname == "QGo4Widget" )
      return "QGo4Widget.h";
   if ( classname == "QGo4LineEdit" )
      return "QGo4LineEdit.h";
   if ( classname == "QGo4Table" )
      return "QGo4Table.h";      
   if (classname == "QGo4BrowserListView")
      return "QGo4BrowserListView.h";
   if (classname == "QGo4RootCanvas")
      return "QGo4RootCanvas.h";
   if (classname == "QFitWidget")
      return "QFitWidget.h";
   if (classname == "QFitNamedWidget")
      return "QFitNamedWidget.h";
   if (classname == "QFitModelWidget")
      return "QFitModelWidget.h";
   if (classname == "QGo4CommandsHistory")
      return "QGo4CommandsHistory.h";
   return QString::null;
}

QString TGo4QtDesignerPlugin::toolTip(const QString& classname) const
{
   if ( classname == "QGo4Widget" )
      return "Go4 Editor Widget";
   if ( classname == "QGo4LineEdit" )
      return "Go4 QLineEdit";
   if ( classname == "QGo4Table" )
      return "Go4 QTable";      
   if (classname == "QGo4BrowserListView")
      return "Go4browser list view";
   if (classname == "QGo4RootCanvas")
      return "Go4 QtROOT canvas";
   if (classname == "QFitWidget")
      return "Basic widget in Go4 Fit gui";
   if (classname == "QFitNamedWidget")
      return "Widget for named items in Go4 Fit gui";
   if (classname == "QFitModelWidget")
      return "Widget for model items in Go4 Fit gui";
   if (classname == "QGo4CommandsHistory")
      return "Command line combo box with history";
   return QString::null;
}

QString TGo4QtDesignerPlugin::whatsThis(const QString& classname) const
{
   if ( classname == "QGo4Widget" )
      return "Basic widget to communicate with Go4 GUI enviroment";
   if ( classname == "QGo4LineEdit" )
      return "Standard QLineEdit with additional drag & drop capabilities";
   if ( classname == "QGo4Table" )
      return "Standard QTable with modified sorting behaviour";
   if (classname == "QGo4BrowserListView")
      return "List view, used in browser to handle start drag event of its items";
   if (classname == "QGo4RootCanvas")
      return "Go4 extension of standard QtROOT canvas";
   if (classname == "QFitWidget")
      return "Fit Widget, connected with items in expert mode";
   if (classname == "QFitNamedWidget")
      return "Widget, connected with named items in expert mode";
   if (classname == "QFitModelWidget")
      return "Widget, connected with model items in expert mode";
    if (classname == "QGo4CommandsHistory")
      return "ComboBox with command line functionality: enter key will execute command, history of commands is available";
   return QString::null;
}

bool TGo4QtDesignerPlugin::isContainer(const QString& classname) const
{
   if ( classname == "QGo4Widget" )
      return TRUE;
   if ( classname == "QGo4LineEdit" )
      return FALSE;
   if ( classname == "QGo4Table" )
      return FALSE;
   if (classname == "QGo4BrowserListView")
      return FALSE;
   if (classname == "QGo4RootCanvas")
      return FALSE;
   if (classname == "QFitWidget")
      return TRUE;
   if (classname == "QFitNamedWidget")
      return TRUE;
   if (classname == "QFitModelWidget")
      return TRUE;
   if ( classname == "QGo4CommandsHistory" )
      return FALSE;
   return FALSE;
}

Q_EXPORT_PLUGIN( TGo4QtDesignerPlugin )
