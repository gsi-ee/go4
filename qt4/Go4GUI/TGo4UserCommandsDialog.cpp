// $Id: TGo4UserCommandsDialog.cpp 842 2012-05-30 14:23:51Z adamczew $
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

#include "TGo4UserCommandsDialog.h"
#include "TGo4QSettings.h"

#include "TGo4UserCommands.h"
#include <QToolTip>
#include <QPushButton>
#include <iostream>

TGo4UserCommandsDialog::TGo4UserCommandsDialog( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4UserCommandsDialog");
   setupUi(this);

   // here we fill table directly from go4 settings:
   for(int id=0; id<GO4GUI_MAXMACRONUM; ++id)
   {
	   QString com= go4sett->getAnalysisMacroCommand(id);
	   QString tip=go4sett->getAnalysisMacroTip(id).split(":").first();
	   bool monitor=go4sett->getAnalysisMacroAutomode(id);
	   QTableWidgetItem* comitem= CommandsTable->item(id,0);
	   if(comitem) comitem->setText(com);
	   QTableWidgetItem* tipitem=  CommandsTable->item(id,1);
	   if(tipitem) tipitem->setText(tip);

	   QTableWidgetItem* autoitem=  CommandsTable->item(id,2);
	   if(autoitem)
	   	   {
		   	   autoitem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		   	   autoitem->setCheckState( monitor ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

	   	   }

   }

   QObject::connect (DialogButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ButtonPressed(QAbstractButton*)));

}

QString TGo4UserCommandsDialog::GetCommand(int id)
   {
		QString rev("");
		QTableWidgetItem* item=CommandsTable->item(id,0);
		if(item)
			rev=item->text();
		return rev;
   }

QString TGo4UserCommandsDialog::GetTooltip(int id)
   {
		QString rev("");
		QTableWidgetItem* item=CommandsTable->item(id,1);
		if(item)
			rev=item->text();
		return rev;
   }

bool TGo4UserCommandsDialog::GetAutoExecute(int id)
{
	bool rev(false);
	QTableWidgetItem* item=CommandsTable->item(id,2);
	if(item)
	rev=(item->checkState()==Qt::CheckState::Checked);
	return rev;

}

void TGo4UserCommandsDialog::ButtonPressed(QAbstractButton* but)
{
 //std::cout<<"TGo4UserCommandsDialog ButtonPressed "  << std::endl;
 QPushButton* pbut=dynamic_cast<QPushButton*>(but);
 if (pbut == DialogButtonBox->button (QDialogButtonBox::Reset))
   {
	 //std::cout<<"Found Reset button "  << std::endl;
	 for(int id=0; id<GO4GUI_MAXMACRONUM; ++id)
	   {
		   QTableWidgetItem* comitem= CommandsTable->item(id,0);
		   if(comitem) comitem->setText(QString(""));
		   QTableWidgetItem* tipitem=  CommandsTable->item(id,1);
		   if(tipitem) tipitem->setText(QString(""));
		   QTableWidgetItem* autoitem=  CommandsTable->item(id,2);
		   if(autoitem)
		  	   	   {
		  		   	   autoitem->setCheckState(Qt::CheckState::Unchecked);
		  	   	   }
	   }
   } // if Reset

}
