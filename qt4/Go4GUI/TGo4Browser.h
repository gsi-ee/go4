#ifndef TGO4BROWSER_H
#define TGO4BROWSER_H

#include "QGo4Widget.h"
#include "QGo4BrowserListView.h"
#include "ui_TGo4Browser.h"

class Q3ListViewItem;
class Q3DragObject;
class TGo4BrowserProxy;


class TGo4Browser : public QGo4Widget, public Ui::TGo4Browser
 {
     Q_OBJECT

 public:
     TGo4Browser(QWidget *parent = 0, const char* name=0);



	void StartWorking();


	void linkedObjectUpdated(const char* linkname, TObject* obj);



	void ResetWidget();

	TGo4BrowserProxy* BrowserProxy();

	QString FullItemName(Q3ListViewItem* item);


	Q3ListViewItem* FindItemFor(TGo4Slot* slot);

	void SetViewItemProperties(TGo4Slot* itemslot, Q3ListViewItem* item);


	void checkVisisbilityFlags(bool showall);


	void DisplaySelectedItems();



	void SuperImposeSelectedItems();


	bool canDrawItem(Q3ListViewItem* item);

	void ShootUpdateTimer();

	void SaveSelectedItems();

	void ExportSelectedItems(const char* filtername);

	void ExportSelectedItems(const char* filename, const char* filedir, const char* format, const char* description);

public slots:

	void RequestDragObjectSlot(Q3DragObject** res);

	void ItemDropAcceptSlot(void* item, void* mime, bool* res);

	void ItemDropProcessSlot(void* item, void* e);

	void updateListViewItems();

	void ListView_doubleClicked(Q3ListViewItem* item);


	void ListView_contextMenuRequested(Q3ListViewItem* item, const QPoint& pos, int col);

	void ColumnToggled(int indx);

	void HeaderSizeChanged(int, int, int);

	void ContextMenuActivated(int id);

protected:

	bool fbUpdateTimerActive;
	bool fVisibleColumns[7];
	int fIndexes[7];
};


#endif

