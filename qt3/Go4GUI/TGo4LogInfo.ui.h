
void TGo4LogInfo::init()
{
   setAcceptDrops(FALSE);
   infoIcon = QPixmap::fromMimeSource("info.png");
   errorIcon = QPixmap::fromMimeSource("error.png");
   warnIcon = QPixmap::fromMimeSource("warn.png");

   LogText->setSorting(0, FALSE);
}

void TGo4LogInfo::WorkWithInfo(TGo4Slot* slot)
{
    ResetWidget();
    AddLink(slot, "Loginfo");
}

void TGo4LogInfo::linkedObjectUpdated( const char * linkname, TObject * obj )
{
   if (obj==0) return;

   QDateTime dt = QDateTime::currentDateTime();
   QString Name = obj->GetName();
   QString Date = dt.toString("dd.MM.yy  ");
   QString Time = dt.toString("hh.mm.ss  ");

   QListViewItem *Item = new QListViewItem(LogText, Date, Time);
   if(Name.contains("GO4-*")) {
      Item->setText(3, "Info");
      Item->setPixmap(0, infoIcon);
    } else if (Name.contains("GO4-!")) {
      Item->setText(3, "Error");
      Item->setPixmap(0, errorIcon);
    } else if(Name.contains("GO4-#")) {
      Item->setText(3, "Warning");
      Item->setPixmap(0, warnIcon);
      }
    Name.remove(0, 6);
    Item->setText(2, Name);
    LogText->setSelected(Item, true);
}


void TGo4LogInfo::ClearLogInfo()
{
   LogText->clear();
}

void TGo4LogInfo::SaveLogInfo()
{
    QString TextToSave;
    QFileDialog fd( this, "Save Log Information", TRUE );
    fd.setMode( QFileDialog::AnyFile );
    fd.setName( "Save Log Information ");
    fd.setCaption( "Save analysis log window");
    fd.setFilter( "Plain text (*.txt)" );
    if ( fd.exec() != QDialog::Accepted ) return;

    QString fileName = fd.selectedFile();
    if(!fileName.endsWith(".txt")) fileName.append(".txt");
    QFile NewFile(fileName);
    NewFile.open( IO_ReadWrite | IO_Append );
    QTextStream t( &NewFile );

    QListViewItemIterator it(LogText);
      // iterate through all items of the list view
    for ( ; it.current(); ++it ) {
       QListViewItem* itm = it.current();
       t << itm->text(0) << " " << itm->text(1) << " " << itm->text(2) << endl;
    }
    NewFile.close();
}
