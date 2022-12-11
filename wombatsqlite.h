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

struct WalHeader
{
        quint32 walheader; // = qFromBigEndian<quint32>(pageheader->mid(0, 4));
        quint32 fileversion; // = qFromBigEndian<quint32>(pageheader->mid(4, 4));
        quint32 checkptseqnum; // = qFromBigEndian<quint32>(pageheader->mid(12, 4));
        quint32 salt1; // = qFromBigEndian<quint32>(pageheader->mid(16, 4));
        quint32 salt2; // = qFromBigEndian<quint32>(pageheader->mid(20, 4));
        quint32 checksum1; // = qFromBigEndian<quint32>(pageheader->mid(24, 4));
        quint32 checksum2; // = qFromBigEndian<quint32>(pageheader->mid(28, 4));
};

struct JournalHeader
{
    quint64 jnlheader; //= qFromBigEndian<quint64>(pageheader->mid(0, 8));
    quint32 jnlpagecnt; // = qFromBigEndian<quint32>(pageheader->mid(8, 4));
    quint32 randomnonce; // = qFromBigEndian<quint32>(pageheader->mid(12, 4));
    quint32 initsize; // = qFromBigEndian<quint32>(pageheader->mid(16, 4));
    quint32 sectorsize; // = qFromBigEndian<quint32>(pageheader->mid(20, 4));
    quint32 jnlpagesize; // = qFromBigEndian<quint32>(pageheader->mid(24, 4));
};

struct SqliteHeader
{
        char sqlheader[16];
        //QString sqliteheader; // = QString::fromStdString(pageheader->mid(0, 16).toStdString());
        quint16 dbpagesize; // = qFromBigEndian<quint16>(pageheader->mid(16, 2));
        quint8 writever; // = qFromBigEndian<quint8>(pageheader->mid(18, 1));
        quint8 readver; // = qFromBigEndian<quint8>(pageheader->mid(19, 1));
        quint8 unusedpagespace; // = qFromBigEndian<quint8>(pageheader->mid(20, 1));
        quint32 dbpagecount; // = qFromBigEndian<quint32>(pageheader->mid(28, 4));
        quint32 firstfreepagenum; // = qFromBigEndian<quint32>(pageheader->mid(32, 4));
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
protected:
    //void closeEvent(QCloseEvent* event);

private:
    Ui::WombatSqlite* ui;
    QLabel* statuslabel;
    QString dbpath;
    QString olddbpath;
    QString curfilepath = "";
    QFile dbfile;
    quint8 filetype = 0;
    quint64 pagecount = 0;
    quint32 pagesize = 0;
    quint64 curpage = 0;
    void LoadPage(void);
    void ParseHeader(QByteArray* pageheader);
    //void LoadPage(QString filepath, quint8 filepath, curitem->data(257).toUInt(), curitem->data(258).toUInt());
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
};

#endif // WOMBATSQLITE_H
