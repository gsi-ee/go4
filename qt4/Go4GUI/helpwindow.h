#ifndef TGO4HELPWINDOW_H
#define TGO4HELPWINDOW_H

#include "q3mainwindow.h"
#include "qstringlist.h"
#include "qmap.h"
//Added by qt3to4:
#include <Q3PopupMenu>

class QComboBox;
class Q3PopupMenu;
class Q3TextBrowser;

class TGo4HelpWindow : public Q3MainWindow {
  Q_OBJECT

  public:
    TGo4HelpWindow( const QString& home_, QWidget* parent, const QString& subdir="");
    virtual ~TGo4HelpWindow();

  private slots:

    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void textChanged();
    void about();
    void aboutQt();
    void print();

    void pathSelected( const QString & );
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();

  private:
    void readBookmarks();
    void readHistory();

    Q3TextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    Q3PopupMenu *hist, *bookm;
    QString _path;
};

#endif




