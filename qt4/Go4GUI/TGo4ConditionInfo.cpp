// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ConditionInfo.h"

#include "TClass.h"
#include "TDatime.h"
#include "TH1.h"
#include "TGo4Log.h"
#include "TGo4Condition.h"
#include "TGo4WinCond.h"



TGo4ConditionInfo::TGo4ConditionInfo(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
   setupUi(this);
   setWindowTitle("Condition Info:");
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
   WorkWithCondition(CondnameLbl->text().toStdString().c_str());
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

    TGo4Log::Message(1, textbuffer.toStdString().c_str());
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
   PropertyBox->item(CONTITLE)->setText("Title");
   PropertyBox->item(CONCLASS)->setText("Class:");
   PropertyBox->item(CONX)->setText("X Testrange");
   PropertyBox->item(CONY)->setText("Y Testrange");
   PropertyBox->item(CONDIM)->setText("Dimension");
   PropertyBox->item(CONCOUNTS)->setText("Counts");
   PropertyBox->item(CONRESULT)->setText("Result");
   PropertyBox->item(CONMODE)->setText("Test mode");
   PropertyBox->item(CONSIZE)->setText("Size");
   TDatime now;
   PropertyBox->item(CONTIME)->setText(now.AsSQLString());
   ensurePolished();
   update();
   show();
   raise();
}

void TGo4ConditionInfo::RefreshWidget(TGo4Condition* cond)
{
   if(cond==0) return;

   TString str;
   PropertyBox->item(CONTITLE)->setText(cond->GetTitle());
   PropertyBox->item(CONCLASS)->setText(cond->ClassName());
   int dimension = 0;
   if(cond->InheritsFrom(TGo4WinCond::Class())) {
       TGo4WinCond* wcon = (TGo4WinCond*) cond;
       double xmin, xmax, ymin, ymax;
       wcon->GetValues(dimension, xmin, xmax, ymin, ymax);
       str.Form("X: [%.1f,%.1f]",xmin, xmax);
       PropertyBox->item(CONX)->setText(str.Data());
       str.Form("Y: [%.1f,%.1f]",ymin, ymax);
       PropertyBox->item(CONY)->setText(str.Data());
   } else {
       PropertyBox->item(CONX)->setText("X:-");
       PropertyBox->item(CONY)->setText("Y:-");
       dimension = 2;
   }

   str.Form("Dim:%d", dimension);
   PropertyBox->item(CONDIM)->setText(str.Data());
   str.Form("Counts:%d", cond->Counts());
   PropertyBox->item(CONCOUNTS)->setText(str.Data());
   str.Form("True:%d",cond->TrueCounts());
   PropertyBox->item(CONRESULT)->setText(str.Data());

   if(cond->IsEnabled()) str = "test enabled"; else
     if(cond->FixedResult()) str = "always true";
                        else  str = "always false";
   if(cond->IsTrue()) str += " - regular";
                 else str += " - inverse";
   PropertyBox->item(CONMODE)->setText(str.Data());

   str.Form("size:%d b",cond->GetMemorySize());
   PropertyBox->item(CONSIZE)->setText(str.Data());

   TDatime now;
   PropertyBox->item(CONTIME)->setText(now.AsSQLString());
   ensurePolished();
   update();
   show();
   raise();
   if(TGo4Log::IsAutoEnabled()) PrintLog_clicked();
}
