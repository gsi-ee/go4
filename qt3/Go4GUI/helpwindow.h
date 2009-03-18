#ifndef TGO4HELPWINDOW_H
#define TGO4HELPWINDOW_H

#include "qmainwindow.h"
#include "qstringlist.h"
#include "qmap.h"

class QComboBox;
class QPopupMenu;
class QTextBrowser;

class TGo4HelpWindow : public QMainWindow {
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

    QTextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    QPopupMenu *hist, *bookm;
    QString _path;
};

#endif




