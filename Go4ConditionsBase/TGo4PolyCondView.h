#ifndef TGO4POLYCONDVIEW_H
#define TGO4POLYCONDVIEW_H

#include "TCutG.h"

class TGo4PolyCond;

class TGo4PolyCondView : public TCutG {

public:

    TGo4PolyCondView(TCutG* source);
    TGo4PolyCondView();
    virtual ~TGo4PolyCondView();

    virtual void Paint(Option_t* opt);
    virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

    /** Check if cut on pad is changed compared with condition */
    Bool_t IsCutChanged();
    /**Copy modified view values back to condition */
    virtual void UpdateCondition();


    void SetCondition(TGo4PolyCond* condition)
        {
             fxPolyCondition=condition;
        }

    TGo4PolyCond* GetCondition(){return fxPolyCondition;}

    /** Set this view to values of cut source*/
    void SetCut(TCutG* source);

    /** create plain cutg from this view. */
    TCutG* CreateCut();

    virtual const char* GetName() const;
    void SetName(const char* nam); // *MENU*
     /** Delete condition instead of view: */
    void DeleteRegion(); // *MENU*
    void SetLabelDraw(Bool_t on); // *MENU*
    void SetLimitsDraw(Bool_t on); // *MENU*
    void SetIntDraw(Bool_t on); // *MENU*
    void SetXMeanDraw(Bool_t on); // *MENU*
    void SetXRMSDraw(Bool_t on); // *MENU*
    void SetYMeanDraw(Bool_t on); // *MENU*
    void SetYRMSDraw(Bool_t on); // *MENU*
    void SetXMaxDraw(Bool_t on); // *MENU*
    void SetYMaxDraw(Bool_t on); // *MENU*
    void SetCMaxDraw(Bool_t on); // *MENU*
    /** This will save draw flags into  static default setup */
    void SaveLabelStyle();// *MENU*
    /** Will reset label position to defaults */
    void ResetLabel();// *MENU*

    virtual Int_t InsertPoint();// *MENU*
    virtual Int_t RemovePoint();// *MENU*

private:

    /** pointer to viewed condition for update after graphical action */
    TGo4PolyCond* fxPolyCondition; //!

    ClassDef(TGo4PolyCondView,6)
};

#endif //TGO4POLYCONDVIEW_H
