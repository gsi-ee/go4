#ifndef QGO4WIDGET_H
#define QGO4WIDGET_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>

#include <QtGui/QDragMoveEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>

class TObject;
class TClass;
class TPad;
class TGo4Slot;
class TGo4ViewPanel;
class TGo4BrowserProxy;
class TGo4AnalysisProxy;

class QDESIGNER_WIDGET_EXPORT  QGo4Widget : public QWidget {
   Q_OBJECT

   friend class TGo4MainWindow;

   public:
      enum { service_DragEnter        = 1,
             service_DropEvent        = 2,
             service_CreateItem       = 3,
             service_DrawItem         = 4,
             service_EditItem         = 5,
             service_ShowInfo         = 6,
             service_EditInSlot       = 7,
             service_SaveItem         = 8,
             service_UpdateAnalysisItem = 9,
             service_StatusMessage    = 15,
             service_CreateViewPanel  = 17,
             service_BrowserItemRemote = 18,
             service_Browser          = 19,
             service_SaveToMemory     = 20,
             service_GetAnalysis      = 21,
             service_AddEditorLink    = 22,
             service_AddDirectLink    = 23,
             service_AddLinkInSlot    = 24,
             service_GetTopSlot       = 25,
             service_AddEditorSlot    = 26,
             service_SetLinkedName    = 27,
             service_GetLinkedName    = 28,
             service_GetLinkedName2   = 29,
             service_GetLinked0       = 30,
             service_GetLinked1       = 31,
             service_GetLinked2       = 32,
             service_RemoveLink       = 33,
             service_RemoveAllLinks   = 34,
             service_ObjectCreated    = 35,
             service_ConnectPad       = 36,
             service_LastActivePanel  = 37,
             service_WhereItemDrawn   = 38,
             service_UndrawItem       = 39,
             service_General          = 40,
             service_PanelTimer       = 41,
             service_HelpWindow       = 42,
             panel_Activated         = 101,
             panel_Modified          = 102,
             panel_Updated           = 103,
             panel_ActiveUpdated     = 104,
             panel_PadDeleted        = 105,
             panel_Deleted           = 106 };



      QGo4Widget(QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );
      virtual ~QGo4Widget();

      virtual bool IsAcceptDrag(const char* itemname, TClass* cl, int kind);
      virtual void DropItem(const char* itemname, TClass* cl, int kind);
      virtual void ResetWidget();

      void ObjectCreatedByWidget(const char* itemname, TClass* cl);

      void ProcessSignal(const char* linkname, bool assigned, TObject* obj, TGo4Slot* slot);

   signals:
      void widgetService(QGo4Widget* editor, int serviceid, const char* str, void* par);

   public slots:
      void ResetWidgetSlot();

   protected:
      virtual void closeEvent(QCloseEvent* e);
      virtual void dragEnterEvent(QDragEnterEvent *e);
      virtual void dragMoveEvent(QDragMoveEvent* e);
      virtual void dropEvent(QDropEvent* e);

      /** create link for item with name linkname*/
      void AddLink(const char* itemname, const char* linkname);

      /** create link for object in slot with name linkname*/
      void AddLink(TGo4Slot* slot, const char* linkname);

      /** create link for item in dedicated slot parent */
      TGo4Slot* AddLink(const char* itemname, TGo4Slot* parent);

      /** return top slot of structure, corresponding to this widget */
      TGo4Slot* GetTopSlot(bool force = false);

      /** add slot with specific name to widget top folder */
      TGo4Slot* AddSlot(const char* slotname);

      /** set name of item, which was linked by this slot */
      void SetLinkedName(TGo4Slot* slot, const char* itemname);

      /** returns name of item, which was linked with linkname */
      const char* GetLinkedName(const char* linkname);

      /** returns name of item, which was linked in that slot */
      const char* GetLinkedName(TGo4Slot* slot);

      TObject* GetLinked(const char* linkname, int updatelevel);
      void RemoveLink(const char* linkname, bool blockreset = true);
      void RemoveAllLinks(bool blockreset = true);
      void RemoveLinksMasked(const char* startedwith = 0, bool blockreset = true);

      bool BrowserItemRemote(const char* itemname);
      TGo4BrowserProxy* Browser();

      void StatusMessage(const char* message);

      /** Function call dialog to create object of appropriate class
        *  isremote==0 - object should be created localy and widget will be informed
        *  isremote==1 - object should be created on analysis side and widget should be informaed
        *  isremote==-1 - activate creation dialog without restriction (remote or local), widget will not be informed */
      void AskToCreateObject(TClass* cl, int isremote);
      void ShowItemInfo(const char* itemname);
      TGo4ViewPanel* CreateViewPanel(int ndiv = 0);
      TGo4ViewPanel* DrawItem(const char* itemname, TGo4ViewPanel* panel = 0, TPad* pad = 0, bool activate = true, int updatelevel = -1);
      TGo4ViewPanel* WhereItemDrawn(const char* itemname);
      void HelpWindow(const char* filename, const char* msg = 0);
      void UndrawItem(const char* itemname);
      TGo4ViewPanel* LastActivePanel();
      void EditItem(const char* itemname);
      void EditObjectInSlot(TGo4Slot* slot);
      QString SaveObjectInMemory(const char* foldername, TObject* obj);
      bool SaveItemToFile(const char* itemname, const char* subfolder = 0);
      bool UpdateItemInAnalysis(const char* itemname, TObject* obj = 0);
      void InformThatObjectCreated(const char* itemname, TClass* cl);
      TGo4AnalysisProxy* GetAnalysis(const char* itemname = 0);

      /** connect pad axis change signals to react on such signals */
      void ConnectPad(TPad* pad);

      void CallServiceFunc(int id, const char* str = 0, void* par = 0);
      void ServiceCall(const char* name, void* par = 0);

      virtual void requestedObjectCreated(const char* itemname, TClass* cl);
      virtual void linkedObjectUpdated(const char* linkname, TObject* obj);
      virtual void linkedObjectRemoved(const char* linkname);
      virtual void linkedUpdated(TGo4Slot* slot, TObject* obj);
      virtual void linkedRemoved(TGo4Slot* slot, TObject* obj);

      void setCanDestroyWidget(bool on = true) { fCanDestroyWidget = on; }

      void CallPanelFunc(int id, TPad* pad = 0);

      void ShootResetWidget();
      void ShootCloseWidget();

      bool IsUpdateBlocked() const { return fBlockUpdate; }

   private:
      bool   fWaitsForObjectCreation;
      bool   fCanDestroyWidget;
      bool   fResetWidgetShooted;  // indicates that reset widget timer is shooted
      bool   fBlockUpdate;      // set when automatic reset must be blocked

      TGo4BrowserProxy* fBrowserProxy;
};

#endif
