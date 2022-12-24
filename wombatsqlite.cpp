#include "wombatsqlite.h"

// Copyright 2013-2020 Pasquale J. Rinaldi, Jr.
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
    connect(ui->actionOpenDB, SIGNAL(triggered()), this, SLOT(OpenDB()), Qt::DirectConnection);
    ui->tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablewidget->setHorizontalHeaderLabels({"Tag", "Is Live", "Type"});
    connect(ui->treewidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(FileSelected(QListWidgetItem*)), Qt::DirectConnection);
    connect(ui->pagespinbox, SIGNAL(valueChanged(int)), this, SLOT(PageChanged(int)), Qt::DirectConnection);
    connect(ui->propwidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectText()), Qt::DirectConnection);
    connect(ui->editscrollbar, SIGNAL(valueChanged(int)), this, SLOT(ScrollHex(int)), Qt::DirectConnection);
    //connect(ui->hexedit, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()), Qt::DirectConnection);
    ui->offsetedit->setVerticalScrollBar(ui->editscrollbar);
    ui->hexedit->setVerticalScrollBar(ui->editscrollbar);
    ui->utf8edit->setVerticalScrollBar(ui->editscrollbar);
    ui->editscrollbar->setVisible(true);
    //connect(ui->offsetedit, ui->hexedit, ui->utf8edit, ui->editscrollbar
    /*
    connect(ui->treewidget, SIGNAL(itemSelectionChanged()), this, SLOT(KeySelected()), Qt::DirectConnection);
    connect(ui->tablewidget, SIGNAL(itemSelectionChanged()), this, SLOT(ValueSelected()), Qt::DirectConnection);
    connect(ui->actionOpenHive, SIGNAL(triggered()), this, SLOT(OpenHive()), Qt::DirectConnection);
    connect(ui->actionManageTags, SIGNAL(triggered()), this, SLOT(ManageTags()), Qt::DirectConnection);
    connect(ui->actionPreviewReport, SIGNAL(triggered()), this, SLOT(PreviewReport()), Qt::DirectConnection);
    connect(ui->actionPublish, SIGNAL(triggered()), this, SLOT(PublishReport()), Qt::DirectConnection);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(ShowAbout()), Qt::DirectConnection);
    // initialize temp directory for html code...
    QDir tmpdir;
    tmpdir.mkpath(QDir::tempPath() + "/wr");
    tmpdir.mkpath(QDir::tempPath() + "/wr/tagged");
    // initialize Preview Report HTML code
    prehtml = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body style='" + ReturnCssString(0) + "'>\n";
    prehtml += "<div style='" + ReturnCssString(1) + "'><h1><span id='casename'>Registry Report</span></h1></div>\n";
    psthtml = "</body></html>";

    tags.clear();
    tagmenu = new QMenu(ui->tablewidget);
    UpdateTagsMenu();

    hives.clear();

    ui->tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tablewidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(TagMenu(const QPoint &)), Qt::DirectConnection);
    */
}

WombatSqlite::~WombatSqlite()
{
    delete ui;
    //QDir tmpdir(QDir::tempPath() + "/wr");
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
    /*
    TagManager* tagmanager = new TagManager(this);
    tagmanager->SetTagList(&tags);
    tagmanager->exec();
    UpdateTagsMenu();
    */
}

void WombatSqlite::UpdatePreviewLinks()
{
    /*
    // POSSIBLY REBUILD THE MAIN PAGE EVERY TIME, RATHER THAN FIND AND REPLACE...
    QDir tmpdir(QDir::tempPath() + "/wr/tagged");
    tmpdir.removeRecursively();
    tmpdir.mkpath(QDir::tempPath() + "/wr/tagged");
    QString curcontent = "";
    curcontent += "<div id='toc'><h2>Contents</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<span" + QString::number(i) + "'><a href='#t" + QString::number(i) + "'>" + tags.at(i) + "</a></span><br/>\n";
    }
    curcontent += "<h2>Tagged Items</h2>";
    for(int i=0; i < tags.count(); i++)
    {
        curcontent += "<div id='t" + QString::number(i) + "'><h3>" + tags.at(i) + "</h3><br/><table><tr>";
        for(int j=0; j < taggeditems.count(); j++)
        {
            if(taggeditems.at(j).split("|", Qt::SkipEmptyParts).at(0) == tags.at(i))
            {
                curcontent += "<td style='" + ReturnCssString(11) + "'><a href='" + QDir::tempPath() + "/wr/tagged/" + QString::number(i) + "-" + QString::number(j) + ".html'>" + taggeditems.at(j).split("|").at(1) + "</a></td>";
                QString htmlvalue = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body style='" + ReturnCssString(0) + "'>";
                htmlvalue += "<div style='" + ReturnCssString(1) + "'>Registry Analysis</div><br/>";
                htmlvalue += "<pre>";
                htmlvalue += taggeditems.at(j).split("|").at(2).toUtf8();
                htmlvalue + "</pre><body></html>";
                QFile htmlfile(QDir::tempPath() + "/wr/tagged/" + QString::number(i) + "-" + QString::number(j) + ".html");
                if(!htmlfile.isOpen())
                    htmlfile.open(QIODevice::WriteOnly | QIODevice::Text);
                if(htmlfile.isOpen())
                {
                    htmlfile.write(htmlvalue.toStdString().c_str());
                    htmlfile.close();
                }
            }
        }
        curcontent += "</tr></table></div><br/>\n";
    }
    reportstring = prehtml + curcontent + psthtml;
    QFile indxfile(QDir::tempPath() + "/wr/index.html");
    if(!indxfile.isOpen())
        indxfile.open(QIODevice::WriteOnly | QIODevice::Text);
    if(indxfile.isOpen())
    {
        indxfile.write(reportstring.toStdString().c_str());
        indxfile.close();
    }
    */
}

void WombatSqlite::PreviewReport()
{
    /*
    UpdatePreviewLinks();
    HtmlViewer* htmlviewer = new HtmlViewer();
    htmlviewer->LoadHtml(QDir::tempPath() + "/wr/index.html");
    htmlviewer->show();
    */
}

void WombatSqlite::PublishReport()
{
    /*
    UpdatePreviewLinks();
    QString savepath = QFileDialog::getExistingDirectory(this, tr("Select Report Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!savepath.isEmpty())
    {
        // Make tagged path to store tagged registry files
        QDir tmppath;
        tmppath.mkpath(savepath + "/tagged/");
        QFile::copy(QDir::tempPath() + "/wr/index.html", savepath + "/index.html");
        QDirIterator it(QString(QDir::tempPath() + "/wr/tagged"), QDirIterator::NoIteratorFlags);
        while(it.hasNext())
        {
            QString curfile = it.next();
            if(curfile.endsWith("html"))
            {
                QFile::copy(curfile, savepath + "/tagged/" + curfile.split("/").last());
            }
        }
    }
    */
}

void WombatSqlite::ShowAbout()
{
    /*
    AboutBox* aboutbox = new AboutBox();
    aboutbox->exec();
    */
}

void WombatSqlite::CreateNewTag()
{
    /*
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
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idkeyvalue))
            taggeditems.removeAt(i);
    }
    taggeditems.append(tagname + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());
    */
}

void WombatSqlite::UpdateTagsMenu()
{
    /*
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
    */
}

void WombatSqlite::SetTag()
{
    /*
    QAction* tagaction = qobject_cast<QAction*>(sender());
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    if(!ui->tablewidget->selectedItems().first()->text().isEmpty())
    {
        for(int i=0; i < taggeditems.count(); i++)
        {
            if(taggeditems.at(i).contains(idkeyvalue))
                taggeditems.removeAt(i);
        }
    }
    taggeditems.append(tagaction->iconText() + "|" + statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text() + "|" + ui->plaintext->toPlainText());

    ui->tablewidget->selectedItems().first()->setText(tagaction->iconText());
    */
}

void WombatSqlite::RemoveTag()
{
    /*
    ui->tablewidget->selectedItems().first()->setText("");
    QString idkeyvalue = statuslabel->text() + "\\" + ui->tablewidget->selectedItems().at(1)->text();
    for(int i=0; i < taggeditems.count(); i++)
    {
        if(taggeditems.at(i).contains(idkeyvalue))
            taggeditems.removeAt(i);
    }
    */
}

void WombatSqlite::ValueSelected(void)
{
    /*
    if(ui->tablewidget->selectedItems().count() > 0)
    {
	QTreeWidgetItem* curitem = ui->treewidget->selectedItems().first();
	int rootindex = GetRootIndex(curitem);
	hivefilepath = hives.at(rootindex);
	int valueindex = ui->tablewidget->selectedItems().at(1)->row();
	QString keypath = statuslabel->text();
	libregf_file_t* regfile = NULL;
	libregf_error_t* regerr = NULL;
	libregf_file_initialize(&regfile, &regerr);
	libregf_file_open(regfile, hivefilepath.toStdString().c_str(), LIBREGF_OPEN_READ, &regerr);
	libregf_key_t* curkey = NULL;
	libregf_file_get_key_by_utf8_path(regfile, (uint8_t*)(keypath.toUtf8().data()), keypath.toUtf8().size(), &curkey, &regerr);
	libregf_value_t* curval = NULL;
	libregf_key_get_value(curkey, valueindex, &curval, &regerr);
        uint64_t lastwritetime = 0;
        libregf_key_get_last_written_time(curkey, &lastwritetime, &regerr);
        QString valuedata = "Last Written Time:\t" + ConvertWindowsTimeToUnixTimeUTC(lastwritetime) + " UTC\n\n";
	valuedata += "Name:\t" + ui->tablewidget->selectedItems().at(1)->text() + "\n\n";
	if(ui->tablewidget->selectedItems().at(1)->text().contains("(unnamed)"))
	{
	    valuedata += "Content\n-------\n\n";
	    valuedata += "Hex:\t0x" + ui->tablewidget->selectedItems().at(1)->text() + "\n";
	    valuedata += "Integer:\t" + QString::number(ui->tablewidget->selectedItems().at(1)->text().toInt(nullptr, 16)) + "\n";
	}
	else
	{
            QString valuetype = ui->tablewidget->selectedItems().at(2)->text();
            if(valuetype.contains("REG_SZ") || valuetype.contains("REG_EXPAND_SZ"))
            {
                valuedata += "Content:\t";
                size_t strsize = 0;
                libregf_value_get_value_utf8_string_size(curval, &strsize, &regerr);
                uint8_t valstr[strsize];
                libregf_value_get_value_utf8_string(curval, valstr, strsize, &regerr);
                valuedata += QString::fromUtf8(reinterpret_cast<char*>(valstr));
            }
            else if(valuetype.contains("REG_BINARY"))
            {
                valuedata += "Content\n-------\n\n";
                if(keypath.contains("UserAssist") && (keypath.contains("{750") || keypath.contains("{F4E") || keypath.contains("{5E6")))
                {
                    valuedata += "ROT13 Decrypted Content:\t";
                    valuedata += DecryptRot13(ui->tablewidget->selectedItems().at(1)->text()) + "\n";
                }
                else if(keypath.contains("SAM") && ui->tablewidget->selectedItems().at(1)->text().count() == 1 && ui->tablewidget->selectedItems().at(1)->text().startsWith("F"))
                {
                    size_t datasize = 0;
                    libregf_value_get_value_data_size(curval, &datasize, &regerr);
                    uint8_t data[datasize];
                    libregf_value_get_value_data(curval, data, datasize, &regerr);
                    QByteArray farray = QByteArray::fromRawData((char*)data, datasize);
                    valuedata += "Account Expiration:\t\t";
                    if(farray.mid(32,1).toHex() == "ff")
                    {
                        valuedata += "No Expiration is Set\n";
                    }
                    else
                        valuedata += ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(32, 8))) + " UTC\n";
                    valuedata += "Last Logon Time:\t\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(8, 8))) + " UTC\n";
                    valuedata += "Last Failed Login:\t\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(40, 8))) + " UTC\n";
                    valuedata += "Last Time Password Changed:\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(farray.mid(24, 8))) + " UTC";
                }
                else if(ui->tablewidget->selectedItems().at(1)->text().startsWith("ShutdownTime"))
                {
                    size_t datasize = 0;
                    libregf_value_get_value_data_size(curval, &datasize, &regerr);
                    uint8_t data[datasize];
                    libregf_value_get_value_data(curval, data, datasize, &regerr);
                    QByteArray valarray = QByteArray::fromRawData((char*)data, datasize);
                    valuedata += "Shutdown Time:\t" + ConvertWindowsTimeToUnixTimeUTC(qFromLittleEndian<uint64_t>(valarray)) + " UTC";
                }
            }
            else if(valuetype.contains("REG_DWORD"))
            {
                valuedata += "Content:\t";
                uint32_t dwordvalue = 0;
                libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                if(ui->tablewidget->selectedItems().at(1)->text().toLower().contains("date"))
                    valuedata += ConvertUnixTimeToString(dwordvalue);
                else
                    valuedata += QString::number(dwordvalue);
            }
            else if(valuetype.contains("REG_DWORD_BIG_ENDIAN"))
            {
                valuedata += "Content:\t";
                uint32_t dwordvalue = 0;
                libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                valuedata += QString::number(qFromBigEndian<uint32_t>(dwordvalue));
            }
            else if(valuetype.contains("REG_MULTI_SZ"))
            {
                valuedata += "Content\n";
                valuedata += "-------\n";
                libregf_multi_string_t* multistring = NULL;
                libregf_value_get_value_multi_string(curval, &multistring, &regerr);
                int strcnt = 0;
                libregf_multi_string_get_number_of_strings(multistring, &strcnt, &regerr);
                for(int i=0; i < strcnt; i++)
                {
                    size_t strsize = 0;
                    libregf_multi_string_get_utf8_string_size(multistring, i, &strsize, &regerr);
                    uint8_t valstr[strsize];
                    libregf_multi_string_get_utf8_string(multistring, i, valstr, strsize, &regerr);
                    valuedata += QString::fromUtf8(reinterpret_cast<char*>(valstr)) + "\n";
                }
                libregf_multi_string_free(&multistring, &regerr);
            }
            else if(valuetype.contains("REG_QWORD"))
            {
                valuedata += "Content:\t";
                uint64_t qwordvalue = 0;
                libregf_value_get_value_64bit(curval, &qwordvalue, &regerr);
                valuedata += QString::number(qwordvalue);
            }
	}
        size_t datasize = 0;
        libregf_value_get_value_data_size(curval, &datasize, &regerr);
        uint8_t data[datasize];
        libregf_value_get_value_data(curval, data, datasize, &regerr);
        QByteArray dataarray = QByteArray::fromRawData((char*)data, datasize);
        valuedata += "\n\nBinary Content\n--------------\n\n";
        int linecount = datasize / 16;
        //int remainder = datasize % 16;
        for(int i=0; i < linecount; i++)
        {
            valuedata += QString::number(i * 16, 16).rightJustified(8, '0') + "\t";
            for(int j=0; j < 16; j++)
            {
                valuedata += QString("%1").arg(data[j+i*16], 2, 16, QChar('0')).toUpper() + " ";
            }
            for(int j=0; j < 16; j++)
            {
                if(!QChar(dataarray.at(j+i*16)).isPrint())
                {
                    valuedata += ".";
                }
                else
                    valuedata += QString("%1").arg(dataarray.at(j+i*16));
            }
            valuedata += "\n";
        }
	ui->plaintext->setPlainText(valuedata);

        libregf_value_free(&curval, &regerr);
        libregf_key_free(&curkey, &regerr);
        libregf_file_close(regfile, &regerr);
        libregf_file_free(&regfile, &regerr);
        libregf_error_free(&regerr);
    }
    */
}

/*
int WombatSqlite::GetRootIndex(QTreeWidgetItem* curitem)
{
    if(ui->treewidget->indexOfTopLevelItem(curitem) == -1)
	GetRootIndex(curitem->parent());
    else
	return ui->treewidget->indexOfTopLevelItem(curitem);
}
*/

void WombatSqlite::PageChanged(int cpage)
{
    if(ui->treewidget->currentItem() != NULL)
    {
        ui->propwidget->setCurrentItem(NULL);
        ui->treewidget->currentItem()->setData(258, cpage);
        curpage = cpage;
        LoadPage();
        //LoadRecords();
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
    //qDebug() << "curitem text:" << curitem->text();
    //qDebug() << "curitem tag:" << curitem->toolTip();
    //qDebug() << "file type:" << curitem->data(256).toUInt();
    //qDebug() << "page size:" << curitem->data(257).toUInt();
    //qDebug() << "current page:" << curitem->data(258).toUInt();
    ui->pagespinbox->setValue(curpage);
    ui->propwidget->setCurrentItem(NULL);
    LoadPage();
    // need to implement this to parse the records within the page...
    //LoadRecords();
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
    ParsePageHeader(&pagearray, filetype, curpage);
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

void WombatSqlite::ParsePageHeader(QByteArray* pagearray, quint8 filetype, quint64 curpage)
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
        for(int i=0; i < pageheader.cellcount; i++)
        {
            celloffarray.append(qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2)));
            AddProperty(rowcnt + i, QString::number(cellarrayoff + 2*i) + ", 2", QString::number(qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2))), "Cell Array Offset " + QString::number(i+1) + ".");
            //qDebug() << "cell array " + QString::number(i) + " offset:" << qFromBigEndian<quint16>(pagearray->mid(cellarrayoff + 2*i, 2));
        }
    }
    else // root page of a table with no rows
    {
        qDebug() << "cell content area offset is equal to pagesize - reserved space bytes.";
    }
    //qDebug() << "cell offset array count: " << celloffarray.count() << celloffarray;
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
            utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber);
            utf8cursor.setPosition(vallist.at(0).toUInt() + linenumber + vallist.at(1).toUInt(), QTextCursor::KeepAnchor);
        }
        else
        {
            utf8cursor.setPosition(vallist.at(0).toUInt());
            utf8cursor.setPosition(vallist.at(0).toUInt() + vallist.at(1).toUInt(), QTextCursor::KeepAnchor);
        }
        ui->utf8edit->setTextCursor(utf8cursor);

        OffsetUpdate(QString::number(vallist.at(0).toUInt(), 16));
    }
}

void WombatSqlite::SelectionChanged()
{
    // might be able to do an if propwidget-currentitem current vallist != selectionstart then do the below
    ui->utf8edit->textCursor().clearSelection();
    ui->propwidget->setCurrentItem(NULL);
    OffsetUpdate(QString::number(ui->hexedit->textCursor().selectionStart() / 3, 16));
}

/*
void WombatSqlite::PopulateChildKeys(libregf_key_t* curkey, QTreeWidgetItem* curitem, libregf_error_t* regerr)
{
    int subkeycount = 0;
    libregf_key_get_number_of_sub_keys(curkey, &subkeycount, &regerr);
    if(subkeycount > 0)
    {
	for(int i=0; i < subkeycount; i++)
	{
	    libregf_key_t* cursubkey = NULL;
	    libregf_key_get_sub_key(curkey, i, &cursubkey, &regerr);
	    size_t namesize = 0;
	    libregf_key_get_utf8_name_size(cursubkey, &namesize, &regerr);
	    uint8_t name[namesize];
	    libregf_key_get_utf8_name(cursubkey, name, namesize, &regerr);
	    QTreeWidgetItem* subitem = new QTreeWidgetItem(curitem);
	    subitem->setText(0, QString::fromUtf8(reinterpret_cast<char*>(name)));
	    curitem->addChild(subitem);
	    int subsubkeycount = 0;
	    libregf_key_get_number_of_sub_keys(cursubkey, &subsubkeycount, &regerr);
	    if(subsubkeycount > 0)
	    {
		PopulateChildKeys(cursubkey, subitem, regerr);
	    }
	    libregf_key_free(&cursubkey, &regerr);
	}
    }
}
*/

QString WombatSqlite::DecryptRot13(QString encstr)
{
    QString decstr = "";
    int i = 0;
    int strlength = 0;
    strlength = encstr.count();
    decstr = encstr;
    for(i = 0; i < strlength; i++)
    {
        decstr[i] = Rot13Char(decstr.at(i));
    }
    return decstr;
}

QChar WombatSqlite::Rot13Char(QChar curchar)
{
    QChar rot13char;
    if('0' <= curchar && curchar <= '4')
        rot13char = QChar(curchar.unicode() + 5);
    else if('5' <= curchar && curchar <= '9')
        rot13char = QChar(curchar.unicode() - 5);
    else if('A' <= curchar && curchar <= 'M')
        rot13char = QChar(curchar.unicode() + 13);
    else if('N' <= curchar && curchar <= 'Z')
        rot13char = QChar(curchar.unicode() - 13);
    else if('a' <= curchar && curchar <= 'm')
        rot13char = QChar(curchar.unicode() + 13);
    else if('n' <= curchar && curchar <= 'z')
        rot13char = QChar(curchar.unicode() - 13);
    else
        rot13char = curchar;
    return rot13char;
}

QString WombatSqlite::ConvertUnixTimeToString(uint32_t input)
{
    time_t crtimet = (time_t)input;
    QString timestr = QDateTime::fromSecsSinceEpoch(crtimet, QTimeZone::utc()).toString("MM/dd/yyyy hh:mm:ss AP");

    return timestr;
}

QString WombatSqlite::ConvertWindowsTimeToUnixTimeUTC(uint64_t input)
{
    uint64_t temp;
    temp = input / TICKS_PER_SECOND; //convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;  //subtract number of seconds between epochs
    time_t crtimet = (time_t)temp;
    QString timestr = "";
    timestr = QDateTime::fromSecsSinceEpoch(crtimet, QTimeZone::utc()).toString("MM/dd/yyyy hh:mm:ss AP");

    return timestr;
}

void WombatSqlite::TagMenu(const QPoint &pt)
{
    /*
    QTableWidgetItem* currow = ui->tablewidget->itemAt(pt);
    if(ui->tablewidget->item(currow->row(), 0)->text().isEmpty())
	remtagaction->setEnabled(false);
    else
	remtagaction->setEnabled(true);
    tagmenu->exec(ui->tablewidget->mapToGlobal(pt));
    */
}
