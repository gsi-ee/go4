
enum PropertyIndex {
   CONTITLE,
   CONCLASS,
   CONDIM,
   CONX,
   CONY,
   CONCOUNTS,
   CONRESULT,
   CONMODE,
   CONSIZE,
   CONTIME
};

void TGo4ConditionInfo::init()
{
   setCaption("Condition Info:");
}


bool TGo4ConditionInfo::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   return cl==0 ? false : cl->InheritsFrom(TGo4Condition::Class());
}

void TGo4ConditionInfo::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return;
   if (cl->InheritsFrom(TGo4Condition::Class()))
      WorkWithCondition(itemname);
}

void TGo4ConditionInfo::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname,"Condition")==0)
     RefreshWidget(dynamic_cast<TGo4Condition*>(obj));
}


void TGo4ConditionInfo::ConInfoButton_clicked()
{
   WorkWithCondition(CondnameLbl->text());
}

void TGo4ConditionInfo::ConPrintButton_clicked()
{
   ServiceCall("PrintAnalysisConditions");
}

void TGo4ConditionInfo::ConEditButton_clicked()
{
   EditItem(CondnameLbl->text());
}

void TGo4ConditionInfo::PrintLog_clicked()
{
    QString textbuffer;
    textbuffer="Condition ";
    textbuffer+=CondnameLbl->text();
    textbuffer+=" Status: \n ";
    textbuffer+="Title: ";
    textbuffer+=PropertyBox->item(CONTITLE)->text();
    textbuffer+=" Class: ";
    textbuffer+=PropertyBox->item(CONCLASS)->text();
    textbuffer+=" ";
    textbuffer+=PropertyBox->item(CONDIM)->text();
    textbuffer+=" ";
    textbuffer+=PropertyBox->item(CONMODE)->text();
    textbuffer+="\n ";
    textbuffer+=PropertyBox->item(CONX)->text();
    textbuffer+=" ";
    textbuffer+=PropertyBox->item(CONY)->text();
    textbuffer+="\n ";
    textbuffer+=PropertyBox->item(CONCOUNTS)->text();
    textbuffer+=" ";
    textbuffer+=PropertyBox->item(CONRESULT)->text();
    textbuffer+="\n ";
    textbuffer+=PropertyBox->item(CONSIZE)->text();
    textbuffer+=" Status received at: ";
    textbuffer+=PropertyBox->item(CONTIME)->text();

    TGo4Log::Message(1, textbuffer.latin1());
}

void TGo4ConditionInfo::WorkWithCondition(const char* itemname)
{
   ResetWidget();

   CondnameLbl->setText(itemname);

   AddLink(itemname, "Condition");

   TObject* obj = GetLinked("Condition", 2);

   RefreshWidget(dynamic_cast<TGo4Condition*>(obj));

   setFocus();
}

void TGo4ConditionInfo::ResetWidget()
{
   QGo4Widget::ResetWidget();
   PropertyBox->changeItem("Title",CONTITLE);
   PropertyBox->changeItem("Class:",CONCLASS);
   PropertyBox->changeItem("X Testrange",CONX);
   PropertyBox->changeItem("Y Testrange",CONY);
   PropertyBox->changeItem("Dimension",CONDIM);
   PropertyBox->changeItem("Counts",CONCOUNTS);
   PropertyBox->changeItem("Result",CONRESULT);
   PropertyBox->changeItem("Test mode",CONMODE);
   PropertyBox->changeItem("Size",CONSIZE);
   TDatime now;
   PropertyBox->changeItem(now.AsSQLString(),CONTIME);
   polish();
   update();
   show();
   raise();
}

void TGo4ConditionInfo::RefreshWidget(TGo4Condition* cond)
{
   if(cond==0) return;

   TString str;
   PropertyBox->changeItem(cond->GetTitle(), CONTITLE);
   PropertyBox->changeItem(cond->ClassName(), CONCLASS);
   int dimension = 0;
   if(cond->InheritsFrom(TGo4WinCond::Class())) {
       TGo4WinCond* wcon = (TGo4WinCond*) cond;
       double xmin, xmax, ymin, ymax;
       wcon->GetValues(dimension, xmin, xmax, ymin, ymax);
       str.Form("X: [%.1f,%.1f]",xmin, xmax);
       PropertyBox->changeItem(str.Data(), CONX);
       str.Form("Y: [%.1f,%.1f]",ymin, ymax);
       PropertyBox->changeItem(str.Data(), CONY);
   } else {
       PropertyBox->changeItem("X:-", CONX);
       PropertyBox->changeItem("Y:-",CONY);
       dimension = 2;
   }

   str.Form("Dim:%d", dimension);
   PropertyBox->changeItem(str.Data(), CONDIM);
   str.Form("Counts:%d", cond->Counts());
   PropertyBox->changeItem(str.Data(), CONCOUNTS);
   str.Form("True:%d",cond->TrueCounts());
   PropertyBox->changeItem(str.Data(), CONRESULT);

   if(cond->IsEnabled()) str = "test enabled"; else
     if(cond->FixedResult()) str = "always true";
                        else  str = "always false";
   if(cond->IsTrue()) str += " - regular";
                 else str += " - inverse";
   PropertyBox->changeItem(str.Data(), CONMODE);

   str.Form("size:%d b",cond->GetMemorySize());
   PropertyBox->changeItem(str.Data(),CONSIZE);

   TDatime now;
   PropertyBox->changeItem(now.AsSQLString(), CONTIME);
   polish();
   update();
   show();
   raise();
   if(TGo4Log::IsAutoEnabled()) PrintLog_clicked();
}
