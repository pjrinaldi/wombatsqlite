#include "wombatsqlite.h"

// Copyright 2022-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

WombatSqlite::WombatSqlite(QWidget* parent) : QMainWindow(parent), ui(new Ui::WombatSqlite)
{
    ui->setupUi(this);
    this->menuBar()->hide();
    statuslabel = new QLabel(this);
    this->statusBar()->addPermanentWidget(statuslabel, 0);
    StatusUpdate("Open a SQLite DB,WAL, or journal to Begin");
    hexlabel = new QLabel(this);
    this->statusBar()->addWidget(hexlabel, 0);
    lengthlabel = new QLabel(this);
    this->statusBar()->addWidget(lengthlabel, 0);
    connect(ui->actionOpenDB, SIGNAL(triggered()), this, SLOT(OpenDB()), Qt::DirectConnection);
    ui->tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->propwidget->setHorizontalHeaderLabels({"Offset,Length", "Value", "Description"});
    ui->tablewidget->setHorizontalHeaderLabels({"Tag", "Is Live", "Row ID", "Offset,Length", "Type", "Value"});
    connect(ui->treewidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(FileSelected(QListWidgetItem*)), Qt::DirectConnection);
    connect(ui->pagespinbox, SIGNAL(valueChanged(int)), this, SLOT(PageChanged(int)), Qt::DirectConnection);
    connect(ui->propwidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectText()), Qt::DirectConnection);
    connect(ui->editscrollbar, SIGNAL(valueChanged(int)), this, SLOT(ScrollHex(int)), Qt::DirectConnection);
    connect(ui->hexedit, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()), Qt::DirectConnection);
    connect(ui->tablewidget, SIGNAL(itemSelectionChanged()), this, SLOT(ContentSelect()), Qt::DirectConnection);
    ui->offsetedit->setVerticalScrollBar(ui->editscrollbar);
    ui->hexedit->setVerticalScrollBar(ui->editscrollbar);
    ui->utf8edit->setVerticalScrollBar(ui->editscrollbar);
    ui->editscrollbar->setVisible(true);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(ShowAbout()), Qt::DirectConnection);
    connect(ui->actionManageTags, SIGNAL(triggered()), this, SLOT(ManageTags()), Qt::DirectConnection);
    connect(ui->actionPreviewReport, SIGNAL(triggered()), this, SLOT(PreviewReport()), Qt::DirectConnection);
    connect(ui->actionPublish, SIGNAL(triggered()), this, SLOT(PublishReport()), Qt::DirectConnection);
    QDir tmpdir;
    tmpdir.mkpath(QDir::tempPath() + "/wsf");
    // initialize Preview Report HTML code
    prehtml = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body style='" + ReturnCssString(0) + "'>\n";
    prehtml += "<div style='" + ReturnCssString(1) + "'><h1><span id='casename'>SQLite Report</span></h1></div>\n";
    psthtml = "</body></html>";
    tags.clear();
    tagmenu = new QMenu(ui->tablewidget);
    UpdateTagsMenu();
    ui->tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tablewidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(TagMenu(const QPoint &)), Qt::DirectConnection);
}

WombatSqlite::~WombatSqlite()
{
    delete ui;
    //QDir tmpdir(QDir::tempPath() + "/wsf");
    //tmpdir.removeRecursively();
}

void WombatSqlite::OpenDB()
{
    if(olddbpath.isEmpty())
        olddbpath = QDir::homePath();
    QFileDialog opendbdialog(this, tr("Open SQLite File"), olddbpath);
    opendbdialog.setLabelText(QFileDialog::Accept, "Open");
    if(opendbdialog.exec())
    {
        dbpath = opendbdialog.selectedFiles().first();
        olddbpath = dbpath;
        dbfiles.append(dbpath);
        dbfile.setFileName(dbpath);
        if(!dbfile.isOpen())
            dbfile.open(QIODevice::ReadOnly);
        if(dbfile.isOpen())
        {
            LoadSqliteFile();
            dbfile.close();
        }
    }
}

void WombatSqlite::ManageTags()
{
    TagManager* tagmanager = new TagManager(this);
    tagmanager->SetTagList(&tags);
    tagmanager->exec();
    UpdateTagsMenu();
}

void WombatSqlite::UpdatePreviewLinks()
{
    QString curcontent = "";
    curcontent += "<div id='toc'><h2>Contents</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<span" + QString::number(i) + "'><a href='#t" + QString::number(i) + "'>" + tags.at(i) + "</a></span><br/>\n";
    }
    curcontent += "<h2>Tagged Items</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<div id='t" + QString::number(i) + "'><h3>" + tags.at(i) + "</h3><br/><table>";
        for(int j=0; j < taggeditems.count(); j++)
        {
            if(taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(0) == tags.at(i))
            {
                QString filename = ui->treewidget->item(taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(1).split(",").at(0).toUInt())->text().split(" (").at(0);
                QString pagetxt = "Page: " + taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(1).split(",").at(1);
                QStringList contentlist = taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(2).split(";");
                curcontent += "<tr><td style='" + ReturnCssString(11) + "'>";
                curcontent += "Is Live: " + contentlist.at(0) + "<br/>";
                curcontent += "RowID: " + contentlist.at(1) + "<br/>";
                curcontent += "Offset, Length: " + contentlist.at(2) + "<br/>";
                curcontent += "Serial Type: " + contentlist.at(3) + "<br/>";
                curcontent += "Value: " + contentlist.at(4) + "<br/><pre>";
                curcontent += "Binary Content<br/>---------------<br/>";
                curcontent += contentlist.at(5) + " " + contentlist.at(6) + "</pre><br/>";
                curcontent += "On " + pagetxt + " From " + filename + " file.";
                curcontent += "</td></tr>";
            }
        }
        curcontent += "</table></div><br/>\n";
    }
    reportstring = prehtml + curcontent + psthtml;
    QFile indxfile(QDir::tempPath() + "/wsf/index.html");
    if(!indxfile.isOpen())
        indxfile.open(QIODevice::WriteOnly | QIODevice::Text);
    if(indxfile.isOpen())
    {
        indxfile.write(reportstring.toStdString().c_str());
        indxfile.close();
    }
}

void WombatSqlite::PreviewReport()
{
    UpdatePreviewLinks();
    HtmlViewer* htmlviewer = new HtmlViewer();
    htmlviewer->LoadHtml(QDir::tempPath() + "/wsf/index.html");
    htmlviewer->show();
}

void WombatSqlite::PublishReport()
{
    UpdatePreviewLinks();
    QString savepath = QFileDialog::getExistingDirectory(this, tr("Select Report Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!savepath.isEmpty())
    {
        QFile::copy(QDir::tempPath() + "/wsf/index.html", savepath + "/index.html");
    }
}

void WombatSqlite::ShowAbout()
{
    AboutBox* aboutbox = new AboutBox();
    aboutbox->exec();
}

void WombatSqlite::CreateNewTag()
{
    QString tagname = "";
    QInputDialog* newtagdialog = new QInputDialog(this);
    newtagdialog->setCancelButtonText("Cancel");
    newtagdialog->setInputMode(QInputDialog::TextInput);
    newtagdialog->setLabelText("Enter Tag Name");
    newtagdialog->setOkButtonText("Create Tag");
    newtagdialog->setTextEchoMode(QLineEdit::Normal);
    newtagdialog->setWindowTitle("New Tag");
    if(newtagdialog->exec())
        tagname = newtagdialog->textValue();
    if(!tagname.isEmpty())
    {
	tags.append(tagname);
        UpdateTagsMenu();
    }
    ui->tablewidget->selectedItems().first()->setText(tagname);

    // NEED TO STORE TAGGED ITEM TO REPOPULATE IT WHEN SELECTED... I CAN STORE IT BY
    // TREEWIDGET->SELECTEDROW, PAGE NUMBER, CONTENT->SELECTEDROW

    QString idvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(ui->tablewidget->currentRow());
    QString idcontent = ui->tablewidget->selectedItems().at(1)->text() + ";" + ui->tablewidget->selectedItems().at(2)->text() + ";" + ui->tablewidget->selectedItems().at(3)->text() + ";" + ui->tablewidget->selectedItems().at(4)->text() + ";" + ui->tablewidget->selectedItems().at(5)->text() + ";" + ui->hexedit->textCursor().selectedText() + ";" + ui->utf8edit->textCursor().selectedText();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idvalue))
            taggeditems.removeAt(i);
    }
    // may want to store all the columns for the row here...
    taggeditems.append(tagname + "|" + idvalue + "|" + idcontent);
}

void WombatSqlite::UpdateTagsMenu()
{
    tagmenu->clear();
    newtagaction = new QAction("New Tag", tagmenu);
    newtagaction->setIcon(QIcon(":/bar/newtag"));
    connect(newtagaction, SIGNAL(triggered()), this, SLOT(CreateNewTag()));
    tagmenu->addAction(newtagaction);
    tagmenu->addSeparator();
    for(int i=0; i < tags.count(); i++)
    {
	QAction* tmpaction = new QAction(tags.at(i), tagmenu);
	tmpaction->setIcon(QIcon(":/bar/tag"));
	tmpaction->setData(QVariant("t" + QString::number(i)));
	connect(tmpaction, SIGNAL(triggered()), this, SLOT(SetTag()));
	tagmenu->addAction(tmpaction);
    }
    tagmenu->addSeparator();
    remtagaction = new QAction("Remove Tag", tagmenu);
    remtagaction->setIcon(QIcon(":/bar/removetag"));
    connect(remtagaction, SIGNAL(triggered()), this, SLOT(RemoveTag()));
    tagmenu->addAction(remtagaction);
}

void WombatSqlite::SetTag()
{
    QAction* tagaction = qobject_cast<QAction*>(sender());
    QString idvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(ui->tablewidget->currentRow());
    QString idcontent = ui->tablewidget->selectedItems().at(1)->text() + ";" + ui->tablewidget->selectedItems().at(2)->text() + ";" + ui->tablewidget->selectedItems().at(3)->text() + ";" + ui->tablewidget->selectedItems().at(4)->text() + ";" + ui->tablewidget->selectedItems().at(5)->text() + ";" + ui->hexedit->textCursor().selectedText() + ";" + ui->utf8edit->textCursor().selectedText();
    //qDebug() << "idvalue:" << idvalue;
    if(!ui->tablewidget->selectedItems().first()->text().isEmpty())
    {
        for(int i=0; i < taggeditems.count(); i++)
        {
            if(taggeditems.at(i).contains(idvalue))
                taggeditems.removeAt(i);
        }
    }
    // may want to store all the columns for the row here...
    taggeditems.append(tagaction->iconText() + "|" + idvalue + "|" + idcontent);
    ui->tablewidget->selectedItems().first()->setText(tagaction->iconText());
}

void WombatSqlite::RemoveTag()
{
    ui->tablewidget->selectedItems().first()->setText("");
    QString idvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(ui->tablewidget->currentRow());
    //QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idvalue))
            taggeditems.removeAt(i);
    }
}

void WombatSqlite::PageChanged(int cpage)
{
    if(ui->treewidget->currentItem() != NULL)
    {
        ui->propwidget->setCurrentItem(NULL);
        ui->treewidget->currentItem()->setData(258, cpage);
        ui->tablewidget->clearContents();
        curpage = cpage;
        LoadPage();
    }
}

void WombatSqlite::ScrollHex(int linecount)
{
    ui->offsetedit->verticalScrollBar()->setValue(linecount);
    ui->hexedit->verticalScrollBar()->setValue(linecount);
    ui->utf8edit->verticalScrollBar()->setValue(linecount);
}

void WombatSqlite::FileSelected(QListWidgetItem* curitem)
{
    curfilepath = curitem->toolTip();
    filetype = curitem->data(256).toUInt();
    pagesize = curitem->data(257).toUInt();
    curpage = curitem->data(258).toUInt();
    ui->pagespinbox->setValue(curpage);
    ui->propwidget->setCurrentItem(NULL);
    LoadPage();
}

void WombatSqlite::LoadPage()
{
    //qDebug() << "file type:" << filetype << "pagesize:" << pagesize << "curpage:" << curpage;
    QByteArray pagearray;
    if(dbfile.isOpen())
        dbfile.close();
    dbfile.setFileName(curfilepath);
    dbfile.open(QIODevice::ReadOnly);
    if(dbfile.isOpen())
    {
        dbfile.seek((curpage - 1) * pagesize);
        //qDebug() << "dbfile offset:" << dbfile.pos();
        pagearray = dbfile.read(pagesize);
        dbfile.close();
    }
    if(curpage == 1)
    {
        QByteArray pghdrarray = pagearray.left(100);
        ParseHeader(&pghdrarray);
        PopulateHeader();
    }
    ParsePage(&pagearray, filetype, curpage);
    QString offsetcontent = "";
    QString hexcontent = "";
    QString utf8content = "";
    int linecount = pagearray.size() / 16;

    ui->editscrollbar->setMaximum(linecount - 1);
    ui->editscrollbar->setMinimum(0);
    ui->editscrollbar->setSingleStep(1);

    for(int i=0; i < linecount; i++)
    {
        offsetcontent += QString::number(i*16, 16).rightJustified(5, '0') + "\n";
        for(int j=0; j < 16; j++)
        {
            hexcontent += QString("%1").arg((quint8)pagearray.at(j+i*16), 2, 16, QChar('0')).toUpper();
            if(j < 15)
            {
                hexcontent += " ";
            }
            QChar curchar = QChar(pagearray.at(j+i*16));
            if(!curchar.isPrint())
                utf8content += ".";
            else
                utf8content += curchar;
        }
        hexcontent += "\n";
        utf8content += "\n";
    }
    ui->offsetedit->setPlainText(offsetcontent);
    ui->hexedit->setPlainText(hexcontent);
    ui->utf8edit->setPlainText(utf8content);

    offsetcontent = "";
    hexcontent = "";
    utf8content = "";
}

void WombatSqlite::ParseHeader(QByteArray* pageheader)
{
    if(filetype == 1) // WAL
    {
        walheader.header = qFromBigEndian<quint32>(pageheader->mid(0, 4));
        walheader.fileversion = qFromBigEndian<quint32>(pageheader->mid(4, 4));
        walheader.pagesize = qFromBigEndian<quint32>(pageheader->mid(8, 4));
        walheader.checkptseqnum = qFromBigEndian<quint32>(pageheader->mid(12, 4));
        walheader.salt1 = qFromBigEndian<quint32>(pageheader->mid(16, 4));
        walheader.salt2 = qFromBigEndian<quint32>(pageheader->mid(20, 4));
        walheader.checksum1 = qFromBigEndian<quint32>(pageheader->mid(24, 4));
        walheader.checksum2 = qFromBigEndian<quint32>(pageheader->mid(28, 4));
        //qDebug() << "walheader:" << QString::number(walheader.header, 16);
    }
    else if(filetype == 2) // JOURNAL
    {
	journalheader.header = qFromBigEndian<quint64>(pageheader->mid(0,8));
	journalheader.pagecnt = qFromBigEndian<quint32>(pageheader->mid(8, 4));
	journalheader.randomnonce = qFromBigEndian<quint32>(pageheader->mid(12, 4));
	journalheader.initsize = qFromBigEndian<quint32>(pageheader->mid(16, 4));
	journalheader.sectorsize = qFromBigEndian<quint32>(pageheader->mid(20, 4));
	journalheader.pagesize = qFromBigEndian<quint32>(pageheader->mid(24, 4));
	//qDebug() << "journal header:" << QString::number(journalheader.header, 16);
    }
    else if(filetype == 3) // SQLITE DB
    {
	sqliteheader.header = QString::fromStdString(pageheader->mid(0, 16).toStdString());
	sqliteheader.pagesize = qFromBigEndian<quint16>(pageheader->mid(16, 2));
	sqliteheader.writeversion = qFromBigEndian<quint8>(pageheader->mid(18, 1));
	sqliteheader.readversion = qFromBigEndian<quint8>(pageheader->mid(19, 1));
	sqliteheader.unusedpagespace = qFromBigEndian<quint8>(pageheader->mid(20, 1));
	sqliteheader.pagecount = qFromBigEndian<quint32>(pageheader->mid(28, 4));
	sqliteheader.firstfreepagenum = qFromBigEndian<quint32>(pageheader->mid(32, 4));
	sqliteheader.freepagescount = qFromBigEndian<quint32>(pageheader->mid(36, 4));
	sqliteheader.schemacookie = qFromBigEndian<quint32>(pageheader->mid(40, 4));
	sqliteheader.schemaformat = qFromBigEndian<quint32>(pageheader->mid(44, 4));
	sqliteheader.pagecachesize = qFromBigEndian<quint32>(pageheader->mid(48, 4));
	sqliteheader.largestrootbtreepagenumber = qFromBigEndian<quint32>(pageheader->mid(52, 4));
	sqliteheader.textencoding = qFromBigEndian<quint32>(pageheader->mid(56, 4));
	sqliteheader.userversion = qFromBigEndian<quint32>(pageheader->mid(60, 4));
	sqliteheader.incrementalvacuummodebool = qFromBigEndian<quint32>(pageheader->mid(64, 4));
	sqliteheader.appid = qFromBigEndian<quint32>(pageheader->mid(68, 4));
	sqliteheader.versionvalidfornum = qFromBigEndian<quint32>(pageheader->mid(92, 4));
	sqliteheader.version = qFromBigEndian<quint32>(pageheader->mid(96, 4));
	//qDebug() << "sqlite header:" << sqliteheader.header;
    }
}

void WombatSqlite::AddContent(int row, QString islive, QString rowid, QString offlen, QString type, QString val, QString tag)
{
    ui->tablewidget->setItem(row, 0, new QTableWidgetItem(tag));
    ui->tablewidget->setItem(row, 1, new QTableWidgetItem(islive));
    ui->tablewidget->setItem(row, 2, new QTableWidgetItem(rowid));
    ui->tablewidget->setItem(row, 3, new QTableWidgetItem(offlen));
    ui->tablewidget->setItem(row, 4, new QTableWidgetItem(type));
    ui->tablewidget->setItem(row, 5, new QTableWidgetItem(val));
}

void WombatSqlite::AddProperty(int row, QString offlen, QString val, QString desc)
{
    ui->propwidget->setItem(row, 0, new QTableWidgetItem(offlen));
    ui->propwidget->setItem(row, 1, new QTableWidgetItem(val));
    ui->propwidget->setItem(row, 2, new QTableWidgetItem(desc));
}

void WombatSqlite::PopulateHeader()
{
    ui->propwidget->setHorizontalHeaderLabels({"Offset,Length", "Value", "Description"});
    if(filetype == 1) // WAL
    {
        ui->propwidget->setRowCount(8);
        AddProperty(0, "0, 4", QString("0x" + QString("%1").arg(walheader.header, 8, 16, QChar('0')).toUpper()), "WAL HEADER, last byte is either 0x82 or 0x83 which means something i forget right now");
        AddProperty(1, "4, 4", QString::number(walheader.fileversion), "WAL File Version");
        AddProperty(2, "8, 4", QString::number(walheader.pagesize), "WAL Page Size");
        AddProperty(3, "12, 4", QString::number(walheader.checkptseqnum), "WAL Checkpoint Sequence Number");
        AddProperty(4, "16, 4", QString::number(walheader.salt1), "WAL Salt 1");
        AddProperty(5, "20, 4", QString::number(walheader.salt2), "WAL Salt 2");
        AddProperty(6, "24, 4", QString::number(walheader.checksum1), "WAL Checksum 1");
        AddProperty(7, "28, 4", QString::number(walheader.checksum2), "WAL Checksum 2");
    }
    else if(filetype == 2) // JOURNAL
    {
        ui->propwidget->setRowCount(6);
        AddProperty(0, "0, 8", QString("0x" + QString("%1").arg(journalheader.header, 8, 16, QChar('0')).toUpper()), "JOURNAL HEADER");
        AddProperty(1, "8, 4", QString::number(journalheader.pagecnt), "Journal Page Count");
        AddProperty(2, "12, 4", QString::number(journalheader.randomnonce), "Journal Random Nonce");
        AddProperty(3, "16, 4", QString::number(journalheader.initsize), "Journal Initial Size");
        AddProperty(4, "20, 4", QString::number(journalheader.sectorsize), "Journal Sector Size");
        AddProperty(5, "24, 4", QString::number(journalheader.pagesize), "Journal Page Size");
    }
    else if(filetype == 3) // SQLITE DB
    {
        ui->propwidget->setRowCount(18);
        AddProperty(0, "0, 16", sqliteheader.header, "SQLITE HEADER");
        AddProperty(1, "16, 2", QString::number(sqliteheader.pagesize), "SQLite Page Size");
        AddProperty(2, "18, 1", QString::number(sqliteheader.writeversion), "SQLite Write Version");
        AddProperty(3, "19, 1", QString::number(sqliteheader.readversion), "SQLite Read Version");
        AddProperty(4, "20, 1", QString::number(sqliteheader.unusedpagespace), "SQLite Unused Page Space");
        AddProperty(5, "28, 4", QString::number(sqliteheader.pagecount), "SQLite Page Count");
        AddProperty(6, "32, 4", QString::number(sqliteheader.firstfreepagenum), "SQLite First Free Page Number");
        AddProperty(7, "36, 4", QString::number(sqliteheader.freepagescount), "SQLite Free Pages Count");
        AddProperty(8, "40, 4", QString::number(sqliteheader.schemacookie), "SQLite Schema Cookie");
        AddProperty(9, "44, 4", QString::number(sqliteheader.schemaformat), "SQLite Schema Format");
        AddProperty(10, "48, 4", QString::number(sqliteheader.pagecachesize), "SQLite Page Cache Size");
        AddProperty(11, "52, 4", QString::number(sqliteheader.largestrootbtreepagenumber), "SQLite Largest Root B-Tree Page Number");
        AddProperty(12, "56, 4", QString::number(sqliteheader.textencoding), "SQLite Text Encoding");
        AddProperty(13, "60, 4", QString::number(sqliteheader.userversion), "SQLite User Version");
        AddProperty(14, "64, 4", QString::number(sqliteheader.incrementalvacuummodebool), "SQLite Incremental Vacuum Mode Boolean");
        AddProperty(15, "68, 4", QString::number(sqliteheader.appid), "SQLite App ID");
        AddProperty(16, "92, 4", QString::number(sqliteheader.versionvalidfornum), "SQLite Version Valid for Number");
        AddProperty(17, "96, 4", QString::number(sqliteheader.version), "SQLite Version");
    }
    ui->propwidget->resizeColumnToContents(2);
}

void WombatSqlite::ParsePage(QByteArray* pagearray, quint8 filetype, quint64 curpage)
{
    quint64 curpos = 0;
    quint64 cellarrayoff = 0;
    uint rowcnt = 0;
    if(curpage == 1) // get page header after the file header, so set new curpos here
    {
        if(filetype == 1) // WAL
        {
            curpos = 32;
        }
        else if(filetype == 2) // JOURNAL
        {
            curpos = 28;
        }
        else if(filetype == 3) // SQLITE DB
        {
            curpos = 100;
        }
        rowcnt = ui->propwidget->rowCount();
    }
    pageheader.type = qFromBigEndian<quint8>(pagearray->mid(curpos, 1));
    pageheader.firstfreeblock = qFromBigEndian<quint16>(pagearray->mid(curpos + 1, 2));
    pageheader.cellcount = qFromBigEndian<quint16>(pagearray->mid(curpos + 3, 2));
    pageheader.cellcontentstart = qFromBigEndian<quint16>(pagearray->mid(curpos + 5, 2));
    pageheader.fragmentedfreebytescount = qFromBigEndian<quint8>(pagearray->mid(curpos + 7, 1));
    ui->propwidget->setRowCount(rowcnt + 5);
    AddProperty(rowcnt, QString::number(curpos) + ", 1", "0x" + QString("%1").arg(pageheader.type, 2, 16, QChar('0')).toUpper(), "Page Type: 0x02 | 0x05 - Index | Table Interior, 0x0a | 0x0d - Index | Table Leaf, any other value is an error.");
    AddProperty(rowcnt + 1, QString::number(curpos + 1) + ", 2", QString::number(pageheader.firstfreeblock), "Start of the first free block on the page or zero for no free blocks.");
    AddProperty(rowcnt + 2, QString::number(curpos + 3) + ", 2", QString::number(pageheader.cellcount), "Number of cells on the page.");
    AddProperty(rowcnt + 3, QString::number(curpos + 5) + ", 2", QString::number(pageheader.cellcontentstart), "Start of the cell content area, zero represents 65536.");
    AddProperty(rowcnt + 4, QString::number(curpos + 7) + ", 1", QString::number(pageheader.fragmentedfreebytescount), "Number of fragmented free bytes within cell content area.");
    cellarrayoff = curpos + 8;
    if(pageheader.type == 0x02 || pageheader.type == 0x05)
    {
        pageheader.rightmostpagenumber = qFromBigEndian<quint32>(pagearray->mid(curpos + 8, 4));
        ui->propwidget->setRowCount(rowcnt + 6);
        AddProperty(rowcnt + 5, QString::number(curpos + 8) + ", 4", QString::number(pageheader.rightmostpagenumber), "Largest page number, right most pointer.");
        cellarrayoff = curpos + 12;
    }
    ui->propwidget->resizeColumnToContents(2);
    //qDebug() << "cell pointer array offset:" << cellarrayoff;
    // Parse Cell Pointers and rows here...
    rowcnt = ui->propwidget->rowCount();
    QList<quint16> celloffarray;
    celloffarray.clear();
    if(pageheader.cellcount > 0)
    {
        ui->propwidget->setRowCount(rowcnt + pageheader.cellcount);
        //ui->tablewidget->setRowCount(pageheader.cellcount);
        for(int i=0; i < pageheader.cellcount; i++)
        {
            celloffarray.append(qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2)));
            AddProperty(rowcnt + i, QString::number(cellarrayoff + 2*i) + ", 2", QString::number(qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2))), "Cell Array Offset " + QString::number(i+1) + ".");
            //qDebug() << "cell array " + QString::number(i) + " offset:" << qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2));
        }
        uint curtmprow = 0;
        QString tmprowid = "";
        ui->tablewidget->setRowCount(celloffarray.count() * 2);
        for(int i=0; i < celloffarray.count(); i++)
        {
            //qDebug() << "cell off array " + QString::number(i+1) + ":" << celloffarray.at(i);
            if(pageheader.type == 0x02) // index interior
            {
                quint32 pagenum = qFromBigEndian<quint32>(pagearray->mid(celloffarray.at(i), 4));
                //qDebug() << "pagenum:" << pagenum;
		QString curidvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(curtmprow);
		QString tagstr = "";
		for(int j=0; j < taggeditems.count(); j++)
		{
		    if(taggeditems.at(j).contains(curidvalue))
			tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
		}
                AddContent(curtmprow, "True", "", QString::number(celloffarray.at(i)) + ", 4", "Page Number", QString::number(pagenum), tagstr);
                curtmprow++;
                uint payloadlength = GetVarIntLength(pagearray, celloffarray.at(i) + 4);
                uint payloadsize = GetVarInt(pagearray, celloffarray.at(i) + 4, payloadlength);
		tagstr = "";
		curidvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(curtmprow);
		for(int j=0; j < taggeditems.count(); j++)
		{
		    if(taggeditems.at(j).contains(curidvalue))
			tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
		}
                AddContent(curtmprow, "True", "", QString::number(celloffarray.at(i) + 4) + ", " + QString::number(payloadsize), "Payload", QString::fromStdString(pagearray->mid(celloffarray.at(i) + 4 + payloadlength, payloadsize).toStdString()), tagstr);
                curtmprow++;
                /*
                 * payload = 33, byte array 4, initial payload not in overflow - 27, 
                uint recordlengthlength = GetVarIntLength(pagearray, celloffarray.at(i) + payloadlength);
                uint recordlength = GetVarInt(pagearray, celloffarray.at(i) + payloadlength, recordlengthlength);
                quint64 contentoffset = celloffarray.at(i) + payloadlength + recordlength;
                QList<int> serialtypes;
                serialtypes.clear();
                QByteArray serialarray = pagearray->mid(celloffarray.at(i) + payloadlength + recordlengthlength, recordlength - recordlengthlength);
                //qDebug() << "serialarray:" << serialarray.toHex() << "serialarray.count:" << serialarray.count();
                uint curserialtypelength = 0;
                while(curserialtypelength < serialarray.count())
                {
                    uint curstlen = GetVarIntLength(&serialarray, curserialtypelength);
                    uint curst = GetVarInt(&serialarray, curserialtypelength, curstlen);
                    curserialtypelength += curstlen;
                    serialtypes.append(GetSerialType(curst));
                }
                if(i == 0)
                {
                    uint tmprowcnt = serialtypes.count() * celloffarray.count();
                    ui->tablewidget->setRowCount(tmprowcnt);
                }
                for(int j=0; j < serialtypes.count(); j++)
                {
                    uint curst = serialtypes.at(j);
                    QString tmpofflen = "";
                    QString tmptype = "";
                    QString tmpval = "";
                    if(curst == 0) // col length is zero, so content length doesn't change
                    {
                        tmptype = "0 - NULL";
                        tmpval = "NULL";
                        //qDebug() << "NULL";
                    }
                    else if(curst == 1) // quint8 (1)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 1";
                        tmptype = "1 - 8-bit int";
                        tmpval = QString::number(qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1)));
                        //qDebug() << "1 byte" << qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1));
                        contentoffset++;
                    }
                    else if(curst == 2) // quint16 (2)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 2";
                        tmptype = "2 - 16-bit int";
                        tmpval = QString::number(qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2)));
                        //qDebug() << "2 byte:" << qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2));
                        contentoffset = contentoffset + 2;
                    }
                    else if(curst == 3)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 3";
                        tmptype = "3 - 24-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3)));
                        //qDebug() << "3 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3));
                        contentoffset = contentoffset + 3;
                    }
                    else if(curst == 4)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 4";
                        tmptype = "4 - 32-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4)));
                        //qDebug() << "4 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4));
                        contentoffset = contentoffset + 4;
                    }
                    else if(curst == 5)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 6";
                        tmptype = "5 - 38-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6)));
                        //qDebug() << "6 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6));
                        contentoffset = contentoffset + 6;
                    }
                    else if(curst == 6)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "6 - 64-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 7)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "7 - 64-bit double";
                        tmpval = QString::number(qFromBigEndian<double>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<double>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 8) // col length is zero, so content length doesn't change
                    {
                        tmptype = "8 - Integer value 0";
                        tmpval = "0";
                        //qDebug() << "0";
                    }
                    else if(curst == 9) // col length is zero, so content length doesn't change)
                    {
                        tmptype = "9 - Integer value 1";
                        tmpval = "1";
                        //qDebug() << "1";
                    }
                    else if(curst >= 12) // BLOB OR TEXT
                    {
                        if(curst % 2 == 0) // EVEN AND BLOB
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 12) / 2);
                            tmptype = ">=12 && even - BLOB";
                            tmpval = pagearray->mid(contentoffset, (curst - 12) / 2).toHex();
                            //qDebug() << "blob size:" << (curst - 12) / 2 << pagearray->mid(contentoffset, (curst-12) / 2).toHex();
                            contentoffset = contentoffset + ((curst - 12) / 2);
                        }
                        else // ODD AND TEXT
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 13) / 2);
                            tmptype = ">=13 & odd - TEXT";
                            tmpval = QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            //qDebug() << "Text Size:" << (curst - 13) / 2 << QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            contentoffset = contentoffset + ((curst - 13) / 2);
                        }
                    }
                    //qDebug() << "curtmprow:" << curtmprow;
                    AddContent(curtmprow, "True", "", tmpofflen, tmptype, tmpval);
                    curtmprow++;
                }
                */
            }
            else if(pageheader.type == 0x05) // table interior
            {
                quint32 pagenum = qFromBigEndian<quint32>(pagearray->mid(celloffarray.at(i), 4));
                uint rowidlength = GetVarIntLength(pagearray, celloffarray.at(i) + 4);
                uint rowid = GetVarInt(pagearray, celloffarray.at(i) + 4, rowidlength);
		QString curidvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(curtmprow);
		QString tagstr = "";
		for(int j=0; j < taggeditems.count(); j++)
		{
		    if(taggeditems.at(j).contains(curidvalue))
			tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
		}
                AddContent(curtmprow, "True", QString::number(rowid), QString::number(celloffarray.at(i)) + ", 4", "Page Number", QString::number(pagenum), tagstr);
                curtmprow++;
                //qDebug() << "pagenum:" << pagenum;
            }
            else if(pageheader.type == 0x0a) // index leaf
            {
                //qDebug() << "index leaf, need to parse similar to table leaf...";
                uint payloadlength = GetVarIntLength(pagearray, celloffarray.at(i));
                uint payloadsize = GetVarInt(pagearray, celloffarray.at(i), payloadlength);
                uint recordlengthlength = GetVarIntLength(pagearray, celloffarray.at(i) + payloadlength);
                uint recordlength = GetVarInt(pagearray, celloffarray.at(i) + payloadlength, recordlengthlength);
                quint64 contentoffset = celloffarray.at(i) + payloadlength + recordlength;
                QList<int> serialtypes;
                serialtypes.clear();
                QByteArray serialarray = pagearray->mid(celloffarray.at(i) + payloadlength + recordlengthlength, recordlength - recordlengthlength);
                uint curserialtypelength = 0;
                while(curserialtypelength < serialarray.count())
                {
                    uint curstlen = GetVarIntLength(&serialarray, curserialtypelength);
                    uint curst = GetVarInt(&serialarray, curserialtypelength, curstlen);
                    curserialtypelength += curstlen;
                    serialtypes.append(curst);
                    //serialtypes.append(GetSerialType(curst));
                }
                if(i == 0)
                {
                    uint tmprowcnt = serialtypes.count() * celloffarray.count();
                    ui->tablewidget->setRowCount(tmprowcnt);
                }
                for(int j=0; j < serialtypes.count(); j++)
                {
                    uint curst = serialtypes.at(j);
                    QString tmpofflen = "";
                    QString tmptype = "";
                    QString tmpval = "";
                    if(curst == 0) // col length is zero, so content length doesn't change
                    {
                        tmptype = "0 - NULL";
                        tmpval = "NULL";
                        //qDebug() << "NULL";
                    }
                    else if(curst == 1) // quint8 (1)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 1";
                        tmptype = "1 - 8-bit int";
                        tmpval = QString::number(qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1)));
                        //qDebug() << "1 byte" << qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1));
                        contentoffset++;
                    }
                    else if(curst == 2) // quint16 (2)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 2";
                        tmptype = "2 - 16-bit int";
                        tmpval = QString::number(qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2)));
                        //qDebug() << "2 byte:" << qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2));
                        contentoffset = contentoffset + 2;
                    }
                    else if(curst == 3)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 3";
                        tmptype = "3 - 24-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3)));
                        //qDebug() << "3 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3));
                        contentoffset = contentoffset + 3;
                    }
                    else if(curst == 4)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 4";
                        tmptype = "4 - 32-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4)));
                        //qDebug() << "4 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4));
                        contentoffset = contentoffset + 4;
                    }
                    else if(curst == 5)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 6";
                        tmptype = "5 - 38-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6)));
                        //qDebug() << "6 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6));
                        contentoffset = contentoffset + 6;
                    }
                    else if(curst == 6)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "6 - 64-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 7)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "7 - 64-bit double";
                        tmpval = QString::number(qFromBigEndian<double>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<double>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 8) // col length is zero, so content length doesn't change
                    {
                        tmptype = "8 - Integer value 0";
                        tmpval = "0";
                        //qDebug() << "0";
                    }
                    else if(curst == 9) // col length is zero, so content length doesn't change)
                    {
                        tmptype = "9 - Integer value 1";
                        tmpval = "1";
                        //qDebug() << "1";
                    }
                    else if(curst >= 12) // BLOB OR TEXT
                    {
                        if(curst % 2 == 0) // EVEN AND BLOB
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 12) / 2);
                            tmptype = ">=12 && even - BLOB";
                            tmpval = pagearray->mid(contentoffset, (curst - 12) / 2).toHex();
                            //qDebug() << "blob size:" << (curst - 12) / 2 << pagearray->mid(contentoffset, (curst-12) / 2).toHex();
                            contentoffset = contentoffset + ((curst - 12) / 2);
                        }
                        else // ODD AND TEXT
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 13) / 2);
                            tmptype = ">=13 & odd - TEXT";
                            tmpval = QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            //qDebug() << "Text Size:" << (curst - 13) / 2 << QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            contentoffset = contentoffset + ((curst - 13) / 2);
                        }
                    }
                    //qDebug() << "curtmprow:" << curtmprow;
		    QString curidvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(curtmprow);
		    QString tagstr = "";
		    for(int j=0; j < taggeditems.count(); j++)
		    {
			if(taggeditems.at(j).contains(curidvalue))
			    tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
		    }
                    AddContent(curtmprow, "True", "", tmpofflen, tmptype, tmpval, tagstr);
                    curtmprow++;
                }
            }
            else if(pageheader.type == 0x0d) // table leaf
            {
                uint payloadlength = GetVarIntLength(pagearray, celloffarray.at(i));
                uint payloadsize = GetVarInt(pagearray, celloffarray.at(i), payloadlength);
                //qDebug() << "payload length:" << payloadlength << "payload size:" << payloadsize;
                uint rowidlength = GetVarIntLength(pagearray, celloffarray.at(i) + payloadlength);
                uint rowid = GetVarInt(pagearray, celloffarray.at(i) + payloadlength, rowidlength);
                tmprowid = QString::number(rowid);
                //qDebug() << "rowid length:" << rowidlength << "row id:" << rowid;
                uint recordlengthlength = GetVarIntLength(pagearray, celloffarray.at(i) + payloadlength + rowidlength);
                uint recordlength = GetVarInt(pagearray, celloffarray.at(i) + payloadlength + rowidlength, recordlengthlength);
                //qDebug() << "record length length:" << recordlengthlength << "record length:" << recordlength;
                // so i need to get the number of bytes after recordlength byte... these are the serial types...
                quint64 contentoffset = celloffarray.at(i) + payloadlength + rowidlength + recordlength;
                QList<int> serialtypes;
                serialtypes.clear();
                QByteArray serialarray = pagearray->mid(celloffarray.at(i) + payloadlength + rowidlength + recordlengthlength, recordlength - recordlengthlength);
                //qDebug() << "Serial Array:" << serialarray.toHex();
                uint curserialtypelength = 0;
                while(curserialtypelength < serialarray.count())
                {
                    uint curstlen = GetVarIntLength(&serialarray, curserialtypelength);
                    uint curst = GetVarInt(&serialarray, curserialtypelength, curstlen);
                    curserialtypelength += curstlen;
                    serialtypes.append(curst);
                    //serialtypes.append(GetSerialType(curst));
                }
                //qDebug() << "serialtypes:" << serialtypes;
                if(i == 0)
                {
                    uint tmprowcnt = serialtypes.count() * celloffarray.count();
                    //qDebug() << "tmprowcnt:" << tmprowcnt;
                    ui->tablewidget->setRowCount(tmprowcnt);
                }
                for(int j=0; j < serialtypes.count(); j++)
                {
                    uint curst = serialtypes.at(j);
                    QString tmpofflen = "";
                    QString tmptype = "";
                    QString tmpval = "";
                    // attempt to spit out content for serial type here... 
                    //qDebug() << "current serial type:" << curst;
                    if(curst == 0) // col length is zero, so content length doesn't change
                    {
                        tmptype = "0 - NULL";
                        tmpval = "NULL";
                        //qDebug() << "NULL";
                    }
                    else if(curst == 1) // quint8 (1)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 1";
                        tmptype = "1 - 8-bit int";
                        tmpval = QString::number(qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1)));
                        //qDebug() << "1 byte" << qFromBigEndian<quint8>(pagearray->mid(contentoffset, 1));
                        contentoffset++;
                    }
                    else if(curst == 2) // quint16 (2)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 2";
                        tmptype = "2 - 16-bit int";
                        tmpval = QString::number(qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2)));
                        //qDebug() << "2 byte:" << qFromBigEndian<quint16>(pagearray->mid(contentoffset, 2));
                        contentoffset = contentoffset + 2;
                    }
                    else if(curst == 3)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 3";
                        tmptype = "3 - 24-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3)));
                        //qDebug() << "3 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 3));
                        contentoffset = contentoffset + 3;
                    }
                    else if(curst == 4)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 4";
                        tmptype = "4 - 32-bit int";
                        tmpval = QString::number(qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4)));
                        //qDebug() << "4 bytes:" << qFromBigEndian<quint32>(pagearray->mid(contentoffset, 4));
                        contentoffset = contentoffset + 4;
                    }
                    else if(curst == 5)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 6";
                        tmptype = "5 - 38-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6)));
                        //qDebug() << "6 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 6));
                        contentoffset = contentoffset + 6;
                    }
                    else if(curst == 6)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "6 - 64-bit int";
                        tmpval = QString::number(qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<quint64>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 7)
                    {
                        tmpofflen = QString::number(contentoffset) + ", 8";
                        tmptype = "7 - 64-bit double";
                        tmpval = QString::number(qFromBigEndian<double>(pagearray->mid(contentoffset, 8)));
                        //qDebug() << "8 bytes:" << qFromBigEndian<double>(pagearray->mid(contentoffset, 8));
                        contentoffset = contentoffset + 8;
                    }
                    else if(curst == 8) // col length is zero, so content length doesn't change
                    {
                        tmptype = "8 - Integer value 0";
                        tmpval = "0";
                        //qDebug() << "0";
                    }
                    else if(curst == 9) // col length is zero, so content length doesn't change)
                    {
                        tmptype = "9 - Integer value 1";
                        tmpval = "1";
                        //qDebug() << "1";
                    }
                    else if(curst >= 12) // BLOB OR TEXT
                    {
                        if(curst % 2 == 0) // EVEN AND BLOB
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 12) / 2);
                            tmptype = ">=12 && even - BLOB";
                            tmpval = pagearray->mid(contentoffset, (curst - 12) / 2).toHex();
                            //qDebug() << "blob size:" << (curst - 12) / 2 << pagearray->mid(contentoffset, (curst-12) / 2).toHex();
                            contentoffset = contentoffset + ((curst - 12) / 2);
                        }
                        else // ODD AND TEXT
                        {
                            tmpofflen = QString::number(contentoffset) + ", " + QString::number((curst - 13) / 2);
                            tmptype = ">=13 & odd - TEXT";
                            tmpval = QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            //qDebug() << "Text Size:" << (curst - 13) / 2 << QString::fromStdString(pagearray->mid(contentoffset, (curst - 13) / 2).toStdString());
                            contentoffset = contentoffset + ((curst - 13) / 2);
                        }
                    }
                    //qDebug() << "curtmprow:" << curtmprow;
		    QString curidvalue = QString::number(ui->treewidget->currentRow()) + "," + ui->treewidget->currentItem()->data(258).toString() + "," + QString::number(curtmprow);
		    QString tagstr = "";
		    for(int j=0; j < taggeditems.count(); j++)
		    {
			if(taggeditems.at(j).contains(curidvalue))
			    tagstr = taggeditems.at(j).split("|", Qt::SkipEmptyParts).first();
		    }
                    AddContent(curtmprow, "True", tmprowid, tmpofflen, tmptype, tmpval, tagstr);
                    curtmprow++;
                    //serialtypes.append(GetSerialType(curst));
                    //qDebug() << "curstlen:" << curstlen << "curst:" << curst << "curserialtypelength:" << curserialtypelength;
                }
                //qDebug() << "serialtypes:" << serialtypes;
                // that new offset is correct for where to start with the content... so once i return serial types, i can pull the values i need...
                //qDebug() << "new offset for column content:" << celloffarray.at(i) + payloadlength + rowidlength + recordlengthlength + recordlength - recordlengthlength;
            }
        }
	// UPDATE TAGS HERE....
        ui->tablewidget->resizeColumnsToContents();
    }
    else // root page of a table with no rows
    {
        qDebug() << "cell content area offset is equal to pagesize - reserved space bytes.";
    }
    //qDebug() << "cell offset array count: " << celloffarray.count() << celloffarray;
}

uint WombatSqlite::GetVarIntLength(QByteArray* pagearray, quint64 pageoffset)
{
    bool getnextbyte = true;
    uint length = 1;
    while(getnextbyte == true)
    {
        quint8 curbyte = qFromBigEndian<quint8>(pagearray->mid(pageoffset + length - 1, 1));
        if(curbyte >= 0x80)
            length++;
        else
            getnextbyte = false;
    }
    return length;
}

uint WombatSqlite::GetVarInt(QByteArray* pagearray, quint64 pageoffset, uint varintlength)
{
    QByteArray varbytes;
    varbytes.clear();
    for(uint i=0; i < varintlength; i++)
        varbytes.append(pagearray->mid(pageoffset + i, 1));
    //qDebug() << "pageoffset:" << pageoffset << "length:" << varintlength;
    //qDebug() << "varbytes:" << varbytes.toHex() << "varbytes count:" << varbytes.count();
    if(varintlength > 1)
    {
        uint i;
        quint64 x;
        quint64 uX = 0;
        for(i=0; i < varintlength && i < 9; i++)
        {
            x = (quint8)varbytes.at(i);
            uX = (uX<<7) + (x&0x7f);
            if((x&0x80)==0)
                break;
        }
        if(i == 9 && i < varintlength)
            uX = (uX<<8) + x;
        i++;
        //qDebug() << "varint:" << uX;
        return uX;
    }
    else
        return qFromBigEndian<quint8>(varbytes);
}

uint WombatSqlite::GetSerialType(uint serialtype)
{
    /*
    if(serialtype == 0) // NULL
        return -1;
    else if(serialtype == 1)
    */
    // 0 = NULL (0)
    // 1 = quint8 (1)
    // 2 = quint16 (2)
    // 3 = quint24 (3)
    // 4 = quint32 (4)
    // 5 = quint48 (6)
    // 6 = quint64 (8)
    // 7 = double (8)
    // 8 = 0 (0)
    // 9 = 1 (0)
    // 10, 11 = reserved
    // N >= 12 (EVEN) TEXT/BLOB (N - 12) / 2 for size
    // N >= 13 (ODD) TEXT/BLOB (N - 13) / 2 for size

    return serialtype;
}

/*
void WombatSqlite::closeEvent(QCloseEvent* e)
{
    e->accept();
}
*/

/*
void WombatSqlite::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event)
    {
        ui->utf8edit->textCursor().clearSelection();
        ui->propwidget->setCurrentItem(NULL);
        OffsetUpdate(QString::number(ui->hexedit->textCursor().selectionStart() / 3, 16));
    }
}
*/

void WombatSqlite::LoadSqliteFile(void)
{
    filetype = 0;
    dbfile.seek(0);
    uint32_t walheader = qFromBigEndian<uint32_t>(dbfile.read(4));
    if(walheader == 0x377f0682 || walheader == 0x377f0683) // WAL
    {
        filetype = 1; // WAL
        dbfile.seek(8);
        pagesize = qFromBigEndian<quint32>(dbfile.read(4));
    }
    else
    {
        dbfile.seek(0);
        quint64 journalheader = qFromBigEndian<quint64>(dbfile.read(8));
        if(journalheader == 0xd9d505f920a163d7) // JOURNAL
        {
            filetype = 2; // JOURNAL
            dbfile.seek(24);
            pagesize = qFromBigEndian<quint32>(dbfile.read(4));
        }
        else
        {
            dbfile.seek(0);
            QString sqliteheader = QString::fromStdString(dbfile.read(15).toStdString());
            if(sqliteheader == "SQLite format 3") // SQLITE DB
            {
                filetype = 3; // SQLITE DB
                dbfile.seek(16);
                pagesize = qFromBigEndian<quint16>(dbfile.read(2));
            }
        }
    }
    if(filetype > 0)
    {
        // to get the file where it needs to go, i need the file type, file path to load, page size, current page, page count
        // file path is in tooltip, page count is in text, so i need to put filetype, page size and current page in userrole
        pagecount = dbfile.size() / pagesize;
        //qDebug() << "pagesize: " << pagesize << "File size: " << dbfile.size() << "page count:" << pagecount;
        QListWidgetItem* rootitem = new QListWidgetItem(ui->treewidget);
        rootitem->setText(dbpath.split("/").last() + " (" + QString::number(pagecount) + ")");
        rootitem->setToolTip(dbpath);
        rootitem->setData(256, QVariant(filetype)); // file type
        rootitem->setData(257, QVariant(pagesize)); // page size
        rootitem->setData(258, QVariant(1)); // current page
        //rootitem->setToolTip(dbpath + "," + QString::number(filetype));
        ui->treewidget->addItem(rootitem);
        //ui->treewidget->addTopLevelItem(rootitem);
        ui->pagespinbox->setMaximum(pagecount);
        ui->countlabel->setText("of " + QString::number(pagecount) + " pages");
        StatusUpdate("SQLite File: " + dbpath + " successfully opened.");
        ui->treewidget->setCurrentRow(ui->treewidget->count() - 1);
        emit(ui->treewidget->itemClicked(ui->treewidget->item(ui->treewidget->count() -1)));
    }
    else
        StatusUpdate("Not a SQLite file, file not opened.");
}

void WombatSqlite::SelectText()
{
    if(ui->propwidget->currentRow() > -1 && ui->propwidget->currentItem() != NULL)
    {
        ui->tablewidget->setCurrentItem(NULL);
        QStringList vallist = ui->propwidget->item(ui->propwidget->currentRow(), 0)->text().split(", ");
        QTextCursor hexcursor = ui->hexedit->textCursor();
        hexcursor.setPosition(vallist.at(0).toUInt() * 3);
        hexcursor.setPosition((vallist.at(0).toUInt() + vallist.at(1).toUInt()) * 3 - 1, QTextCursor::KeepAnchor);
        ui->hexedit->setTextCursor(hexcursor);
        QTextCursor utf8cursor = ui->utf8edit->textCursor();
        //qDebug() << "utf8 offset:" << vallist.at(0).toUInt();
        if(vallist.at(0).toUInt() > 15)
        {
            uint linenumber = vallist.at(0).toUInt() / 16;
            ui->editscrollbar->setValue(linenumber - 1);
            utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber);
            utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber + vallist.at(1).toUInt(), QTextCursor::KeepAnchor);
        }
        else
        {
            ui->editscrollbar->setValue(0);
            utf8cursor.setPosition(vallist.at(0).toUInt());
            utf8cursor.setPosition(vallist.at(0).toUInt() + vallist.at(1).toUInt(), QTextCursor::KeepAnchor);
        }
        ui->utf8edit->setTextCursor(utf8cursor);

        OffsetUpdate(QString::number(vallist.at(0).toUInt(), 16));
        LengthUpdate(vallist.at(1));
    }
}

void WombatSqlite::ContentSelect()
{
    if(ui->tablewidget->currentRow() > -1 && ui->tablewidget->currentItem() != NULL)
    {
        ui->propwidget->setCurrentItem(NULL);
        QStringList vallist = ui->tablewidget->item(ui->tablewidget->currentRow(), 3)->text().split(", ");
        if(vallist.count() == 2)
        {
            QTextCursor hexcursor = ui->hexedit->textCursor();
            hexcursor.setPosition(vallist.at(0).toUInt() * 3);
            hexcursor.setPosition((vallist.at(0).toUInt() + vallist.at(1).toUInt()) * 3 - 1, QTextCursor::KeepAnchor);
            ui->hexedit->setTextCursor(hexcursor);

            QTextCursor utf8cursor = ui->utf8edit->textCursor();
            if(vallist.at(0).toUInt() > 15)
            {
                uint linenumber = vallist.at(0).toUInt() / 16;
                ui->editscrollbar->setValue(linenumber - 1);
                uint length = vallist.at(0).toUInt() + linenumber + vallist.at(1).toUInt() + vallist.at(1).toUInt() / 16;
                if(16 < ((vallist.at(0).toUInt() % 16) + vallist.at(1).toUInt()) && ((vallist.at(0).toUInt() % 16) + vallist.at(1).toUInt()) < 48)
                    length++;
                //qDebug() << "linenumber:" << linenumber;
                utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber);
                utf8cursor.setPosition(length, QTextCursor::KeepAnchor);
                //utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber + vallist.at(1).toUInt() + vallist.at(1).toUInt() / 16, QTextCursor::KeepAnchor);
            }
            else
            {
                // JUST NEED TO FIGURE OUT IF THIS SMALL ONE WRAPS TO A NEW LINE, AND THEN I NEED TO ADD 1 TO IT...
		ui->editscrollbar->setValue(0);
                utf8cursor.setPosition(vallist.at(0).toUInt());
                utf8cursor.setPosition(vallist.at(0).toUInt() + vallist.at(1).toUInt(), QTextCursor::KeepAnchor);
            }
            ui->utf8edit->setTextCursor(utf8cursor);

            OffsetUpdate(QString::number(vallist.at(0).toUInt(), 16));
            LengthUpdate(vallist.at(1));
        }
    }
}

void WombatSqlite::SelectionChanged()
{
    OffsetUpdate(QString::number(ui->hexedit->textCursor().selectionStart() / 3, 16));
    uint diff = (ui->hexedit->textCursor().selectionEnd() - ui->hexedit->textCursor().selectionStart()) / 3;
    uint rem = (ui->hexedit->textCursor().selectionEnd() - ui->hexedit->textCursor().selectionStart()) % 3;
    if(rem != 0)
        diff++;
    LengthUpdate(QString::number(diff));

    ui->utf8edit->textCursor().clearSelection();
    QTextCursor utf8cursor = ui->utf8edit->textCursor();
    if(ui->hexedit->textCursor().selectionStart() / 3 > 15)
    {
        uint linenumber = (ui->hexedit->textCursor().selectionStart() / 3) / 16;
        utf8cursor.setPosition((ui->hexedit->textCursor().selectionStart() / 3) + linenumber);
        utf8cursor.setPosition((ui->hexedit->textCursor().selectionStart() / 3) + linenumber + diff, QTextCursor::KeepAnchor);
    }
    else
    {
        utf8cursor.setPosition(ui->hexedit->textCursor().selectionStart() / 3);
        utf8cursor.setPosition(ui->hexedit->textCursor().selectionStart() / 3 + diff, QTextCursor::KeepAnchor);
    }
    ui->utf8edit->setTextCursor(utf8cursor);
}

void WombatSqlite::TagMenu(const QPoint &pt)
{
    QTableWidgetItem* currow = ui->tablewidget->itemAt(pt);
    if(ui->tablewidget->item(currow->row(), 0)->text().isEmpty())
	remtagaction->setEnabled(false);
    else
	remtagaction->setEnabled(true);
    tagmenu->exec(ui->tablewidget->mapToGlobal(pt));
}
