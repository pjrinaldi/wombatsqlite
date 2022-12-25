#ifndef WOMBATSQLITE_H
#define WOMBATSQLITE_H

// Copyright 2022-2022 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QtEndian>
#include <QDateTime>
#include <QTimeZone>
#include <QDirIterator>
#include "ui_wombatsqlite.h"

#include <bitset>
/*
#include "tagmanager.h"
#include "htmlviewer.h"
#include "aboutbox.h"
#include "libregf.h"
#include "cssstrings.h"
*/

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

/*
class HexEdit : public QPlainTextEdit
{
    Q_OBJECT

    public:

    protected:
        void mouseDoubleClickEvent(QMouseEvent* e)
        {
            if(e)
            {
                qDebug() << "figure out how to implement this and the problem will be solved, or pick another signal.";
                UpdateOffsetLabel(QString::number(this->textCursor().selectionStart() / 3, 16));
                //ui->utf8edit->textCursor().clearSelection();
                //ui->propwidget->setCurrentItem(NULL);
                //OffsetUpdate(QString::number(ui->hexedit->textCursor().selectionStart() / 3, 16));
            }
        };

    signals:
        void UpdateOffsetLabel(QString tmptext);
        //void LaunchFile(const QModelIndex &index);
};
*/

struct WalHeader
{
        quint32 header; // = qFromBigEndian<quint32>(pageheader->mid(0, 4));
        quint32 fileversion; // = qFromBigEndian<quint32>(pageheader->mid(4, 4));
        quint32 pagesize; // = qFromBigEndian<quint32>(pageheader->mid(8, 4));
        quint32 checkptseqnum; // = qFromBigEndian<quint32>(pageheader->mid(12, 4));
        quint32 salt1; // = qFromBigEndian<quint32>(pageheader->mid(16, 4));
        quint32 salt2; // = qFromBigEndian<quint32>(pageheader->mid(20, 4));
        quint32 checksum1; // = qFromBigEndian<quint32>(pageheader->mid(24, 4));
        quint32 checksum2; // = qFromBigEndian<quint32>(pageheader->mid(28, 4));
};

struct JournalHeader
{
    quint64 header; //= qFromBigEndian<quint64>(pageheader->mid(0, 8));
    quint32 pagecnt; // = qFromBigEndian<quint32>(pageheader->mid(8, 4));
    quint32 randomnonce; // = qFromBigEndian<quint32>(pageheader->mid(12, 4));
    quint32 initsize; // = qFromBigEndian<quint32>(pageheader->mid(16, 4));
    quint32 sectorsize; // = qFromBigEndian<quint32>(pageheader->mid(20, 4));
    quint32 pagesize; // = qFromBigEndian<quint32>(pageheader->mid(24, 4));
};

struct SqliteHeader
{
    //char header[16];
    QString header; // = QString::fromStdString(pageheader->mid(0, 16).toStdString());
    quint16 pagesize; // = qFromBigEndian<quint16>(pageheader->mid(16, 2));
    quint8 writeversion; // = qFromBigEndian<quint8>(pageheader->mid(18, 1));
    quint8 readversion; // = qFromBigEndian<quint8>(pageheader->mid(19, 1));
    quint8 unusedpagespace; // = qFromBigEndian<quint8>(pageheader->mid(20, 1));
    quint32 pagecount; // = qFromBigEndian<quint32>(pageheader->mid(28, 4));
    quint32 firstfreepagenum; // = qFromBigEndian<quint32>(pageheader->mid(32, 4));
    quint32 freepagescount; // 36, 4
    quint32 schemacookie; // 40, 4
    quint32 schemaformat; // 44, 4 - 1,2,3,4
    quint32 pagecachesize; // 48, 4
    quint32 largestrootbtreepagenumber; // 52, 4 - or zero
    quint32 textencoding; // 56, 4 - 1 utf-8, 2 utf-16le, 3 utf-16be
    quint32 userversion; // 60, 4
    quint32 incrementalvacuummodebool; // 64, 4 - 0 = false, otherwise true
    quint32 appid; // 68, 4
    char reserved[20]; // 72, 20
    quint32 versionvalidfornum; // 92, 4
    quint32 version; // 96, 4
};

struct PageHeader
{
    quint8 type; // page type 0x02 - index interior (12) | 0x05 - table interior (12) | 0x0a - index leaf (8) | 0x0d - table leaf (8) [0,1]
    quint16 firstfreeblock; // start of first free block on hte page or zero for no free blocks [1, 2]
    quint16 cellcount; // number of cells on the page [3, 2]
    quint16 cellcontentstart; // start of cell content area, zero represents 65536 [5, 2]
    quint8 fragmentedfreebytescount; // number of fragmented free bytes within cell content area [7, 1]
    quint32 rightmostpagenumber; // largest page number, right most pointer, interior page only [8, 4]
};

namespace Ui
{
    class WombatSqlite;
}

class WombatSqlite : public QMainWindow
{
    Q_OBJECT

public:
    explicit WombatSqlite(QWidget* parent = 0);
    ~WombatSqlite();
    void LoadSqliteFile(void);

private slots:
    void OpenDB(void);
    void ManageTags(void);
    void PreviewReport(void);
    void PublishReport(void);
    void ShowAbout(void);
    void FileSelected(QListWidgetItem* curitem);
    void PageChanged(int curpage);
    void SelectText(void);
    void ScrollHex(int linecount);
    void SelectionChanged(void);

    void ValueSelected(void);
    void TagMenu(const QPoint &point);
    void SetTag(void);
    void CreateNewTag(void);
    void RemoveTag(void);
    void UpdateTagsMenu(void);
    void UpdatePreviewLinks(void);
    void StatusUpdate(QString tmptext)
    {
        statuslabel->setText(tmptext);
    };
    void OffsetUpdate(QString tmptext)
    {
        hexlabel->setText("Hex Offset: 0x" + tmptext);
    };
    void LengthUpdate(QString tmptext)
    {
        lengthlabel->setText("Selection Length: " + tmptext);
    };

protected:
    //void closeEvent(QCloseEvent* event);
    //void mouseDoubleClickEvent(QMouseEvent* event);

private:
    Ui::WombatSqlite* ui;
    QLabel* statuslabel;
    QLabel* hexlabel;
    QLabel* lengthlabel;
    QString dbpath;
    QString olddbpath;
    QString curfilepath = "";
    QFile dbfile;
    quint8 filetype = 0;
    quint64 pagecount = 0;
    quint32 pagesize = 0;
    quint64 curpage = 0;
    void AddProperty(int row, QString offlen, QString val, QString desc);
    void LoadPage(void);
    void ParseHeader(QByteArray* pageheader);
    void PopulateHeader(void);
    void ParsePageHeader(QByteArray* pagearray, quint8 filetype, quint64 curpage);
    uint GetVarIntLength(QByteArray* pagearray, quint64 pageoffset);
    uint GetVarInt(QByteArray* pagearray, quint64 pageoffset, uint varintlength);
    uint GetSerialType(uint serialtype);
    // change ParsePageHeader to ParsePage
    //void PopulateChildKeys(libregf_key_t* curkey, QTreeWidgetItem* curitem, libregf_error_t* curerr);
    QString DecryptRot13(QString encstr);
    QChar Rot13Char(QChar curchar);
    QString ConvertUnixTimeToString(uint32_t input);
    QString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
    //int GetRootIndex(QTreeWidgetItem* curitem);
    QMenu* tagmenu;
    QAction* newtagaction;
    QAction* remtagaction;
    QString htmlentry;
    QString htmlvalue;
    QString reportstring;
    QString prehtml;
    QString psthtml;
    QByteArray reporttimezone;
    QStringList tags;
    QStringList taggeditems;
    QStringList hives;
    QStringList dbfiles;
    JournalHeader journalheader;
    WalHeader walheader;
    SqliteHeader sqliteheader;
    PageHeader pageheader;
};

#endif // WOMBATSQLITE_H
