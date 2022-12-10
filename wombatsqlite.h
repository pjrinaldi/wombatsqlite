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
    QFile dbfile;
    quint8 filetype = 0;
    quint64 pagecount = 0;
    quint32 pagesize = 0;
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
};

#endif // WOMBATSQLITE_H
