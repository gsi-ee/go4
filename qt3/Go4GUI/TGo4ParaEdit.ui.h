/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

enum ParEditColumns { fiColName=0, fiColType=1, fiColValue=2, fiColComment=3 };

void TGo4ParaEdit::init()
{
   fItems = 0;
   PleaseUpdateLabel->setShown(false);
   adjustSize();

   ParamNameLbl->setText("");
   ParamClassLbl->setText("");

   fItemName = "";
}

void TGo4ParaEdit::destroy()
{
   if (fItems!=0) {
      delete fItems;
      fItems = 0;
   }
}

bool TGo4ParaEdit::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   if (kind==TGo4Access::kndGo4Param) return true;

   return (cl==0) ? false : cl->InheritsFrom(TGo4Parameter::Class());
}

void TGo4ParaEdit::DropItem(const char* itemname, TClass* cl, int kind)
{
   if ((kind==TGo4Access::kndGo4Param) ||
       ((cl!=0) && cl->InheritsFrom(TGo4Parameter::Class())))
         WorkWithParameter(itemname, false);
}

void TGo4ParaEdit::linkedObjectUpdated(const char* linkname, TObject* obj)
{
//   cout << " TGo4ParaEdit::linkedObjectUpdated " << linkname << endl;

   if (strcmp(linkname,"Parameter")==0)
      RefreshWidget(dynamic_cast<TGo4Parameter*> (obj));
   else
   if (strcmp(linkname,"ParStatus")==0)
      RefreshWidget(dynamic_cast<TGo4ParameterStatus*> (obj));

//   cout << " TGo4ParaEdit::linkedObjectUpdated done " << linkname << endl;
}

void TGo4ParaEdit::linkedObjectRemoved( const char * linkname )
{
//   cout << " TGo4ParaEdit::linkedObjectRemoved " << linkname << endl;

   // if link removed while parameter is dissapear, just close parameter editor

   if (strcmp(linkname,"Fitter")!=0)
      ShootCloseWidget();

//   cout << " TGo4ParaEdit::linkedObjectRemoved done " << linkname << endl;
}

void TGo4ParaEdit::WorkWithParameter(const char* itemname, bool isrefresh)
{
//   cout << "WorkWithParameter  item = " << itemname << endl;

   if (PleaseUpdateLabel->isShown() && !isrefresh) {
       TGo4Parameter* par = dynamic_cast<TGo4Parameter*> (GetLinked("Parameter",0));
       const char* previtem = GetLinkedName("Parameter");
       if ((par!=0) && (previtem!=0)) {
          int res = QMessageBox::warning(this, "Parameter editor",
             QString("Current parameter ")+previtem+" is modified!\n"+
             "New parameter " + itemname + " is selected.",
             QString("Continue with current"),
             QString("Start with new"), QString::null, 0);
//            (BrowserItemRemote(previtem) ? QString("Update current in analysis and start with new") : QString::null), 0);
          if (res==0) return;
//          if (res==2) UpdateItemInAnalysis(previtem);
       }
   }

//   cout << "WorkWithParameter  ResetWidget " << endl;

   ResetWidget();

//   cout << "WorkWithParameter  ResetWidget done" << endl;


   ParamNameLbl->setText(itemname);

   fItemName = itemname;

   QString iconname = "right.png";
   QString tooltip = "Refresh parameter values from analysis";

   if (BrowserItemRemote(itemname)) {
      TGo4Slot* tgtslot = AddSlot("ParStatus");
      TGo4BrowserProxy* br = Browser();
      if (br!=0)
        br->RequestObjectStatus(itemname, tgtslot);
      // add dummy link to be informed when parameter is dissapear
      AddLink(itemname, "ParameterLock");
   } else {
      iconname = "refresh.png";
      tooltip = "Refresh parameter values from source";
      AddLink(itemname, "Parameter");
      TGo4Parameter* par = dynamic_cast<TGo4Parameter*> (GetLinked("Parameter",2));
      if (par!=0) RefreshWidget(par);
   }

   RefreshButton->setIconSet( QIconSet( QPixmap::fromMimeSource(iconname) ) );
   QToolTip::remove(RefreshButton);
   QToolTip::add(RefreshButton, tooltip);

   setFocus();

//   cout << "WorkWithParameter  finish item = " << itemname << endl;

}

void TGo4ParaEdit::ResetWidget()
{
   QGo4Widget::ResetWidget();

   ParamNameLbl->setText("");
   ParamClassLbl->setText("");

   fItemName="";

   RefreshButton->setEnabled(FALSE);
   ApplyButton->setEnabled(FALSE);

   delete fItems;
   fItems = 0;

   ShowVisibleItems();

   PleaseUpdateLabel->setShown(false);
}

void TGo4ParaEdit::RefreshWidget(TGo4Parameter* par)
{
   RefreshButton->setEnabled(par!=0);
   ApplyButton->setEnabled(par!=0);

   delete fItems;
   fItems = new TObjArray(100);
   fItems->SetOwner(kTRUE);

   if (par!=0) {
      par->GetMemberValues(fItems);
      ParamClassLbl->setText(QString(" - ")+par->ClassName());
   }

   ShowVisibleItems();

   PleaseUpdateLabel->setShown(false);
}


void TGo4ParaEdit::RefreshWidget(TGo4ParameterStatus* status)
{
//   cout << "RefreshWidget " << endl;

   RefreshButton->setEnabled(status!=0);
   ApplyButton->setEnabled(status!=0);

//   cout << "RefreshWidget 1" << endl;

   delete fItems;
   fItems = 0;

//   cout << "RefreshWidget 2" << endl;

   if (status!=0) {
      fItems = status->GetMemberValues(kTRUE);
      ParamClassLbl->setText(QString(" - ") + status->GetObjectClass());
   }

   ShowVisibleItems();

   PleaseUpdateLabel->setShown(false);

//   cout << "RefreshWidget done" << endl;
}

void TGo4ParaEdit::ShowVisibleItems()
{
   if (fItems==0) {
      MemberTable->setNumRows(0);
      return;
   }

   MemberTable->hide();

   MemberTable->horizontalHeader()->setLabel(fiColType,"Type");
   MemberTable->horizontalHeader()->setLabel(fiColName,"Name");
   MemberTable->horizontalHeader()->setLabel(fiColValue,"Value");
   MemberTable->horizontalHeader()->setLabel(fiColComment,"Comments");

   int memnum = fItems->GetLast()+1;
   MemberTable->setNumRows(memnum);
   MemberTable->setColumnReadOnly(fiColType,true);
   MemberTable->setColumnReadOnly(fiColName,true);
   MemberTable->setColumnReadOnly(fiColComment,true);
   MemberTable->setLeftMargin(0);
   MemberTable->verticalHeader()->hide();
   TString buf;
   for(int row=0; row<memnum; row++) {
       TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);
       MemberTable->setText(row, fiColType, info->GetTypeName());
       MemberTable->setText(row, fiColName, info->GetFullName(buf));

       MemberTable->setText(row, fiColValue, info->GetStrValue());
       MemberTable->setText(row,fiColComment, info->GetTitle());

       QTableItem* item=MemberTable->item(row, fiColValue);
       if(item!=0) item->setEnabled(info->CanStrEdit());

       if(info->IsVisible())
          MemberTable->showRow(row);
       else
          MemberTable->hideRow(row);
   }

   MemberTable->show();
   polish();
   update();
   show();
   raise();
   MemberTable->show();
}

void TGo4ParaEdit::clearTextFields()
{
   if (fItems==0) return;

   for(int n=0;n<=fItems->GetLast();n++) {
      TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(n);
      info->SetStrValue("");
   }

   PleaseUpdateLabel->setShown(true);

   ShowVisibleItems();
}

void TGo4ParaEdit::ChangedTable( int row, int col )
{
   if (col==fiColValue) {
      QString txt = MemberTable->text(row, col);
      TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);
      info->SetStrValue(txt.latin1());
      PleaseUpdateLabel->setShown(true);
   }
}

void TGo4ParaEdit::TableDoubleClick( int row, int col )
{
   if(col!=fiColName) return;

   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);

   if (info->IsArrayItem()) {
      fiCurrentRow = row;
      ExpandShrinkArray();
   } else
   if (info->IsFitterItem()) {
      fiCurrentRow = row;
      EditFitter();
   }
}

void TGo4ParaEdit::ContextMenu( int row, int col )
{
   if(col!=fiColName) return;

   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);

   if (info->IsArrayItem()) {
       fiCurrentRow = row;
       QPopupMenu contextMenu(this );
       QLabel* caption = new QLabel( "<font color=darkblue><u><b>"
                                    "Array View</b></u></font>", this );
       caption->setAlignment( Qt::AlignCenter );
       contextMenu.insertItem( caption );

       if (row<fItems->GetLast()) {
         TGo4ParameterMember* next = (TGo4ParameterMember*) fItems->At(row+1);
         if ((next->GetMemberId() == info->GetMemberId()) && !next->IsVisible())
            contextMenu.insertItem( "++ \t(next index)",  this, SLOT(IncArrayIndex()));
       }

       if (row>0) {
          TGo4ParameterMember* prev = (TGo4ParameterMember*) fItems->At(row-1);
          if ((prev->GetMemberId() == info->GetMemberId()) && !prev->IsVisible())
             contextMenu.insertItem( "-- \t(prev index)", this, SLOT(DecArrayIndex()));
       }

       contextMenu.insertItem( "Expand/Shrink", this, SLOT(ExpandShrinkArray()));
       contextMenu.exec(QCursor::pos());
    } else

    if (info->IsFitterItem()) {
        fiCurrentRow = row;
        QPopupMenu contextMenu(this );
        QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
            "Modify Fitter</b></u></font>", this);
        caption->setAlignment( Qt::AlignCenter );
        contextMenu.insertItem( caption );
        contextMenu.insertItem( "Edit...",  this, SLOT(EditFitter()));

        TGo4Fitter* fitter = 0;
        ServiceCall("GetFitterFromFitPanel", &fitter);
        if ((fitter!=0) && (info->GetObject()!=fitter))
           contextMenu.insertItem( "Get from FitPanel",  this, SLOT(GetFitterFromEditor()));
        contextMenu.exec(QCursor::pos());
     }
}

void TGo4ParaEdit::ExpandShrinkArray()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);

   Int_t memberid = info->GetMemberId();
   for(int n=0;n<=fItems->GetLast();n++) {
      TGo4ParameterMember* item = (TGo4ParameterMember*) fItems->At(n);
      if (item->GetMemberId()==memberid)
        item->SetVisible(!item->IsVisible());
   }
   info->SetVisible(kTRUE);
   ShowVisibleItems();
}

void TGo4ParaEdit::IncArrayIndex()
{
   if (fiCurrentRow>fItems->GetLast()) return;
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4ParameterMember* next = (TGo4ParameterMember*) fItems->At(fiCurrentRow+1);
   if (next->GetMemberId() == info->GetMemberId()) {
      info->SetVisible(kFALSE);
      next->SetVisible(kTRUE);
      ShowVisibleItems();
   }
}

void TGo4ParaEdit::DecArrayIndex()
{
   if (fiCurrentRow==0) return;
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4ParameterMember* prev = (TGo4ParameterMember*) fItems->At(fiCurrentRow-1);
   if (prev->GetMemberId() == info->GetMemberId()) {
      info->SetVisible(kFALSE);
      prev->SetVisible(kTRUE);
      ShowVisibleItems();
   }
}

void TGo4ParaEdit::EditFitter()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (info->GetObject());

   if(fitter==0) {
      fitter= new TGo4Fitter("Fitter","Fitter from parameter editor");
      info->SetObject(fitter, kTRUE);
   }

   TGo4Slot* fitterslot = AddSlot("Fitter");
   fitterslot->AssignObject(fitter, kFALSE);
   EditObjectInSlot(fitterslot);
   PleaseUpdateLabel->setShown(true);
}

void TGo4ParaEdit::GetFitterFromEditor()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   if ((info==0) || !info->IsFitterItem()) return;

   TGo4Fitter* fitter = 0;
   ServiceCall("CloneFitterFromFitPanel", &fitter);

   if (fitter!=0) info->SetObject(fitter, kTRUE);
}

void TGo4ParaEdit::saveFile()
{
   TGo4LockGuard lock;

   if (fItems==0) return;

   if (GetLinkedName("Parameter")!=0) {
      TGo4Parameter* par = dynamic_cast<TGo4Parameter*> (GetLinked("Parameter",0));
      if (par==0) return;
      if (par->SetMemberValues(fItems))
         if (SaveItemToFile(GetLinkedName("Parameter"), "Parameters"))
            PleaseUpdateLabel->setShown(false);
   } else
   if (fItemName.length()>0) {
      const char* parclass = Browser()->ItemClassName(fItemName.latin1());
      if (parclass==0) return;
      if (gROOT->GetClass(parclass)==0) {
         QMessageBox::warning(this, "Parameter editor",
            QString("Parameter class ") + parclass + " not known to gui");
         return;
      }
      TString foldname, parname;
      TGo4Slot::ProduceFolderAndName(fItemName.latin1(), foldname, parname);

      TGo4ParameterStatus status(parname, parclass, (TObjArray*)fItems->Clone());

      TGo4Parameter* par = status.CreateParameter();

      if (par==0) {
         QMessageBox::warning(this, "Parameter editor",
            QString("Can not crate parameter object of class ") + parclass);
         return;
      }

      QFileDialog fd(this, "Save parameter", kTRUE );
      fd.setMode( QFileDialog::AnyFile);
      fd.setCaption(QString("Save ") + fItemName + " in root file");
      fd.setFilters(QString("ROOT (*.root);;ROOT XML (*.xml)"));
      fd.setSelection("file.root");
      if (fd.exec() != QDialog::Accepted) return;

      TFile* f = TFile::Open(fd.selectedFile().latin1(),"UPDATE");
      if (f!=0) {
         f->WriteTObject(par, par->GetName(), "WriteDelete") > 0;
         delete f;
      }
   }
}

void TGo4ParaEdit::RefreshClicked()
{
//   cout << " RefreshClicked " << endl;

   WorkWithParameter(ParamNameLbl->text(), true);

//   cout << " RefreshClicked done" << endl;
}

void TGo4ParaEdit::ApplyClicked()
{
   TGo4LockGuard lock;

   if ((fItemName.length()==0) || (fItems==0)) return;

   TString foldname, parname;
   TGo4Slot::ProduceFolderAndName(fItemName.latin1(), foldname, parname);

   const char* parclass = Browser()->ItemClassName(fItemName.latin1());
   if (parclass==0) return;

   TGo4ParameterStatus status(parname, parclass, (TObjArray*)fItems->Clone());

   if (UpdateItemInAnalysis(fItemName, &status))
     if (BrowserItemRemote(fItemName))
        RefreshClicked();
     else
        PleaseUpdateLabel->setShown(false);
}
