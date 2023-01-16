#ifndef WOMBATSQLITE_H
#define WOMBATSQLITE_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <filesystem>
#include <byteswap.h>
#include <time.h>
#include "/usr/local/include/fox-1.7/fx.h"
#include "icons.h"
#include "managetags.h"
#include "aboutbox.h"
#include "viewer.h"
#include "common.h"

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

struct WalHeader
{
    uint32_t header; // = qFromBigEndian<uint32_t>(pageheader->mid(0, 4));
    uint32_t fileversion; // = qFromBigEndian<uint32_t>(pageheader->mid(4, 4));
    uint32_t pagesize; // = qFromBigEndian<uint32_t>(pageheader->mid(8, 4));
    uint32_t checkptseqnum; // = qFromBigEndian<uint32_t>(pageheader->mid(12, 4));
    uint32_t salt1; // = qFromBigEndian<uint32_t>(pageheader->mid(16, 4));
    uint32_t salt2; // = qFromBigEndian<uint32_t>(pageheader->mid(20, 4));
    uint32_t checksum1; // = qFromBigEndian<uint32_t>(pageheader->mid(24, 4));
    uint32_t checksum2; // = qFromBigEndian<uint32_t>(pageheader->mid(28, 4));
};

struct JournalHeader
{
    uint64_t header; //= qFromBigEndian<uint64_t>(pageheader->mid(0, 8));
    uint32_t pagecnt; // = qFromBigEndian<uint32_t>(pageheader->mid(8, 4));
    uint32_t randomnonce; // = qFromBigEndian<uint32_t>(pageheader->mid(12, 4));
    uint32_t initsize; // = qFromBigEndian<uint32_t>(pageheader->mid(16, 4));
    uint32_t sectorsize; // = qFromBigEndian<uint32_t>(pageheader->mid(20, 4));
    uint32_t pagesize; // = qFromBigEndian<uint32_t>(pageheader->mid(24, 4));
};

struct SqliteHeader
{
    FXString header; // = FXString::fromStdString(pageheader->mid(0, 16).toStdString());
    uint16_t pagesize; // = qFromBigEndian<uint16_t>(pageheader->mid(16, 2));
    uint8_t writeversion; // = qFromBigEndian<uint8_t>(pageheader->mid(18, 1));
    uint8_t readversion; // = qFromBigEndian<uint8_t>(pageheader->mid(19, 1));
    uint8_t unusedpagespace; // = qFromBigEndian<uint8_t>(pageheader->mid(20, 1));
    uint32_t pagecount; // = qFromBigEndian<uint32_t>(pageheader->mid(28, 4));
    uint32_t firstfreepagenum; // = qFromBigEndian<uint32_t>(pageheader->mid(32, 4));
    uint32_t freepagescount; // 36, 4
    uint32_t schemacookie; // 40, 4
    uint32_t schemaformat; // 44, 4 - 1,2,3,4
    uint32_t pagecachesize; // 48, 4
    uint32_t largestrootbtreepagenumber; // 52, 4 - or zero
    uint32_t textencoding; // 56, 4 - 1 utf-8, 2 utf-16le, 3 utf-16be
    uint32_t userversion; // 60, 4
    uint32_t incrementalvacuummodebool; // 64, 4 - 0 = false, otherwise true
    uint32_t appid; // 68, 4
    char reserved[20]; // 72, 20
    uint32_t versionvalidfornum; // 92, 4
    uint32_t version; // 96, 4
};

struct PageHeader
{
    uint8_t type; // page type 0x02 - index interior (12) | 0x05 - table interior (12) | 0x0a - index leaf (8) | 0x0d - table leaf (8) [0,1]
    uint16_t firstfreeblock; // start of first free block on hte page or zero for no free blocks [1, 2]
    uint16_t cellcount; // number of cells on the page [3, 2]
    uint16_t cellcontentstart; // start of cell content area, zero represents 65536 [5, 2]
    uint8_t fragmentedfreebytescount; // number of fragmented free bytes within cell content area [7, 1]
    uint32_t rightmostpagenumber; // largest page number, right most pointer, interior page only [8, 4]
};

struct FrameHeader
{
    uint32_t pagenumber; // pagenumber
    uint32_t pagecount; // size of db file in pages for commits or zero
    uint32_t salt1; // salt-1 from the wal header
    uint32_t salt2; // salt-2 from the wal header
    uint32_t checksum1; // checksum-1 cumulative checksum up through and including this page
    uint32_t checksum2; // checksum-2 second half of cumulative checksum
};

class WombatSqlite : public FXMainWindow
{
    FXDECLARE(WombatSqlite)

    private:
        FXVerticalFrame* mainframe;
        FXHorizontalFrame* topframe;
        FXSplitter* hsplitter;
        FXSplitter* vsplitter;
        FXSplitter* vsplitter2;
        FXHorizontalFrame* editorframe;
        FXList* sqlfilelist;
        FXToolBar* toolbar;
        FXText* offsettext;
        FXText* hextext;
        FXText* asciitext;
        //FXScrollBar* textscrollbar;
        FXLabel* pagelabel;
        FXSpinner* pagespinner;
        FXLabel* ofpagelabel;
        FXTable* proptable;
        FXTable* tablelist;
	FXIcon* openicon;
        FXButton* openbutton;
	FXIcon* managetagsicon;
	FXButton* managetagsbutton;
	FXIcon* previewicon;
	FXButton* previewbutton;
	FXIcon* publishicon;
	FXButton* publishbutton;
	FXIcon* abouticon;
	FXButton* aboutbutton;
        FXStatusBar* statusbar;
        FXFont* plainfont;
        FXString prevsqlitepath;
        FXString sqlitefilepath;
        FXString curfilepath;
        FXArray<FXString> sqlitefiles;
        std::vector<std::string> tags;
        FXArray<FXString> taggedlist;
        std::ifstream filebuffer;
        std::ifstream* filebufptr;
        FXArray<FXString> fileuserdata;
        FXString curfileuserdata;
        Viewer* viewer;
        uint8_t filetype = 0;
        uint64_t filesize = 0;
        uint64_t pagecount = 0;
        uint32_t pagesize = 0;
        uint64_t curpage = 0;
        JournalHeader journalheader;
        WalHeader walheader;
        SqliteHeader sqliteheader;
        PageHeader pageheader;
        FrameHeader frameheader;

    protected:
        WombatSqlite() {}

    public:
        enum
        {
            ID_TREELIST = 1,
            ID_OPEN = 100,
            ID_TREESELECT = 101,
	    ID_MANAGETAGS = 102,
	    ID_PREVIEW = 103,
	    ID_PUBLISH = 104,
	    ID_ABOUT = 105,
	    ID_TABLESELECT = 106,
            ID_TAGMENU = 107,
            ID_NEWTAG = 108,
            ID_SETTAG = 109,
            ID_REMTAG = 110,
            ID_PAGESPIN = 111,
            ID_SQLLIST = 112,
            ID_PROPTABLE = 113,
            ID_OFFTEXT = 114,
            ID_HEXTEXT = 115,
            ID_ASCTEXT = 116,
            ID_SCROLLBAR = 117,
            ID_LAST
        };
        WombatSqlite(FXApp* a);
        long OpenSqliteFile(FXObject*, FXSelector, void*);
        long OpenTagManager(FXObject*, FXSelector, void*);
	long OpenAboutBox(FXObject*, FXSelector, void*);
        long KeySelected(FXObject*, FXSelector, void*);
	long ValueSelected(FXObject*, FXSelector, void*);
        long TagMenu(FXObject*, FXSelector, void*);
        long SetTag(FXObject* sender, FXSelector, void*);
        long CreateNewTag(FXObject*, FXSelector, void*);
        long RemoveTag(FXObject*, FXSelector, void*);
        long PreviewReport(FXObject*, FXSelector, void*);
        long PublishReport(FXObject*, FXSelector, void*);
        long FileSelected(FXObject*, FXSelector, void*);
        long PropertySelected(FXObject*, FXSelector, void*);
	//void PopulateChildKeys(libregf_key_t* curkey, FXTreeItem* curitem, libregf_error_t* regerr);
	void GetRootString(FXTreeItem* curitem, FXString* rootstring);
	FXString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
        FXString ConvertUnixTimeToString(uint32_t input);
        FXString DecryptRot13(FXString encstr);
        FXchar Rot13Char(FXchar curchar);
        void LoadPage(void);
        void ParseFileHeader(uint8_t* pageheader);
        void PopulateFileHeader(void);
        void AddProperty(int row, FXString offlen, FXString val, FXString desc);
        void AddContent(int row, FXString islive, FXString rowid, FXString offlen, FXString type, FXString val, FXString tag);
	void StatusUpdate(FXString tmptext)
	{
	    statusbar->getStatusLine()->setNormalText(tmptext);
	};
        virtual void create();

};

FXDEFMAP(WombatSqlite) WombatSqliteMap[]={
    //FXMAPFUNC(SEL_CLICKED, WombatSqlite::ID_TREESELECT, WombatSqlite::KeySelected),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_OPEN, WombatSqlite::OpenSqliteFile),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_MANAGETAGS, WombatSqlite::OpenTagManager),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_ABOUT, WombatSqlite::OpenAboutBox),
    //FXMAPFUNC(SEL_SELECTED, WombatSqlite::ID_TABLESELECT, WombatSqlite::ValueSelected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, WombatSqlite::ID_TABLESELECT, WombatSqlite::TagMenu),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_NEWTAG, WombatSqlite::CreateNewTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_SETTAG, WombatSqlite::SetTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_REMTAG, WombatSqlite::RemoveTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_PREVIEW, WombatSqlite::PreviewReport),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_PUBLISH, WombatSqlite::PublishReport),
    FXMAPFUNC(SEL_SELECTED, WombatSqlite::ID_SQLLIST, WombatSqlite::FileSelected),
    FXMAPFUNC(SEL_SELECTED, WombatSqlite::ID_PROPTABLE, WombatSqlite::PropertySelected),
};

#endif // WOMBATSQLITE_H
