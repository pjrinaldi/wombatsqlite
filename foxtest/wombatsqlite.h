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

#define TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
#define NSEC_BTWN_1904_1970	(uint32_t) 2082844800U

class WombatSqlite : public FXMainWindow
{
    FXDECLARE(WombatSqlite)

    private:
        FXVerticalFrame* mainframe;
        FXHorizontalFrame* topframe;
        FXSplitter* hsplitter;
        FXSplitter* vsplitter;
        FXHorizontalFrame* editorframe;
        FXList* sqlfilelist;
        FXToolBar* toolbar;
        FXText* offsettext;
        FXText* hextext;
        FXText* asciitext;
        FXScrollBar* editscrollbar;
        FXLabel* pagelabel;
        FXSpinner* pagespinner;
        FXLabel* ofpagelabel;
        FXTable* proplist;
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
        std::string prevhivepath;
        std::string hivefilepath;
        std::vector<std::filesystem::path> hives;
        std::vector<std::string> tags;
        FXArray<FXString> taggedlist;
        std::ifstream* filebufptr;
        Viewer* viewer;

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
            ID_LAST
        };
        WombatSqlite(FXApp* a);
        long OpenHive(FXObject*, FXSelector, void*);
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
	//void PopulateChildKeys(libregf_key_t* curkey, FXTreeItem* curitem, libregf_error_t* regerr);
	void GetRootString(FXTreeItem* curitem, FXString* rootstring);
	FXString ConvertWindowsTimeToUnixTimeUTC(uint64_t input);
        FXString ConvertUnixTimeToString(uint32_t input);
        FXString DecryptRot13(FXString encstr);
        FXchar Rot13Char(FXchar curchar);
	void StatusUpdate(FXString tmptext)
	{
	    statusbar->getStatusLine()->setNormalText(tmptext);
	};
        virtual void create();

};

FXDEFMAP(WombatSqlite) WombatSqliteMap[]={
    FXMAPFUNC(SEL_CLICKED, WombatSqlite::ID_TREESELECT, WombatSqlite::KeySelected),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_OPEN, WombatSqlite::OpenHive),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_MANAGETAGS, WombatSqlite::OpenTagManager),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_ABOUT, WombatSqlite::OpenAboutBox),
    FXMAPFUNC(SEL_SELECTED, WombatSqlite::ID_TABLESELECT, WombatSqlite::ValueSelected),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, WombatSqlite::ID_TABLESELECT, WombatSqlite::TagMenu),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_NEWTAG, WombatSqlite::CreateNewTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_SETTAG, WombatSqlite::SetTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_REMTAG, WombatSqlite::RemoveTag),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_PREVIEW, WombatSqlite::PreviewReport),
    FXMAPFUNC(SEL_COMMAND, WombatSqlite::ID_PUBLISH, WombatSqlite::PublishReport),
};

#endif // WOMBATSQLITE_H
