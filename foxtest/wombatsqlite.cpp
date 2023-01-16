#include "wombatsqlite.h"

FXIMPLEMENT(WombatSqlite,FXMainWindow,WombatSqliteMap,ARRAYNUMBER(WombatSqliteMap))

WombatSqlite::WombatSqlite(FXApp* a):FXMainWindow(a, "Wombat SQLite Forensics", new FXICOIcon(a, wombat_32), new FXICOIcon(a, wombat_32), DECOR_ALL, 0, 0, 1024, 768)
{
    mainframe = new FXVerticalFrame(this, LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    toolbar = new FXToolBar(mainframe, this, LAYOUT_TOP|LAYOUT_LEFT);
    topframe = new FXHorizontalFrame(mainframe, LAYOUT_TOP|LAYOUT_FILL_X);
    pagelabel = new FXLabel(topframe, "Page: ");
    pagespinner = new FXSpinner(topframe, 3, this, ID_PAGESPIN);
    ofpagelabel = new FXLabel(topframe, " of ");
    vsplitter = new FXSplitter(mainframe, SPLITTER_NORMAL|LAYOUT_FILL);
    sqlfilelist = new FXList(vsplitter, this, ID_SQLLIST);
    hsplitter = new FXSplitter(vsplitter, SPLITTER_VERTICAL);
    proptable = new FXTable(vsplitter, this, ID_PROPTABLE, TABLE_COL_SIZABLE);
    statusbar = new FXStatusBar(mainframe, LAYOUT_BOTTOM|LAYOUT_LEFT|LAYOUT_FILL_X);
    vsplitter2 = new FXSplitter(hsplitter, SPLITTER_NORMAL, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    tablelist = new FXTable(hsplitter, this, ID_TABLESELECT, TABLE_COL_SIZABLE|LAYOUT_FILL_X);
    offsettext = new FXText(vsplitter2, this, ID_OFFTEXT, LAYOUT_FILL_Y);
    hextext = new FXText(vsplitter2, this, ID_HEXTEXT, LAYOUT_FILL_Y);
    asciitext = new FXText(vsplitter2, this, ID_ASCTEXT, LAYOUT_FILL_Y);
    offsettext->setScrollStyle(VSCROLLER_NEVER|HSCROLLER_NEVER);
    hextext->setScrollStyle(VSCROLLER_NEVER|HSCROLLER_NEVER);
    asciitext->setScrollStyle(VSCROLLER_ALWAYS|HSCROLLER_NEVER);
    plainfont = new FXFont(a, "monospace");
    sqlfilelist->setWidth(this->getWidth() / 4);
    vsplitter2->setHeight(this->getHeight() / 2);
    offsettext->setWidth(55);
    offsettext->setFont(plainfont);
    offsettext->setText("000000");
    hextext->setFont(plainfont);
    hextext->setWidth(390);
    hextext->setText("00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff");
    asciitext->setFont(plainfont);
    asciitext->setWidth(150);
    asciitext->setText("1234567890abcdef");
    hsplitter->setWidth(offsettext->getWidth() + hextext->getWidth() + asciitext->getWidth() + 10);
    proptable->setRowHeaderWidth(0);
    tablelist->setRowHeaderWidth(0);
    proptable->setEditable(false);
    proptable->setTableSize(4, 3);
    tablelist->setEditable(false);
    tablelist->setTableSize(5, 6);
    proptable->setColumnText(0, "Offset, Length");
    proptable->setColumnText(1, "Value");
    proptable->setColumnText(2, "Description");
    proptable->setColumnHeaderHeight(proptable->getColumnHeaderHeight() + 5);
    tablelist->setColumnText(0, "Tag");
    tablelist->setColumnText(1, "Is Live");
    tablelist->setColumnText(2, "Row ID");
    tablelist->setColumnText(3, "Offset, Length");
    tablelist->setColumnText(4, "Type");
    tablelist->setColumnText(5, "Value");
    tablelist->setColumnHeaderHeight(tablelist->getColumnHeaderHeight() + 5);
    /*
    treelist->setWidth(this->getWidth() / 4);
    hsplitter = new FXSplitter(vsplitter, SPLITTER_VERTICAL);
    tablelist = new FXTable(hsplitter, this, ID_TABLESELECT, TABLE_COL_SIZABLE|LAYOUT_FILL_X, LAYOUT_FILL_Y);
    plainfont = new FXFont(a, "monospace");
    plaintext = new FXText(hsplitter);
    plaintext->setFont(plainfont);
    plaintext->setEditable(false);
    tablelist->setHeight(this->getHeight() / 3);
    tablelist->setEditable(false);
    tablelist->setTableSize(4, 3);
    tablelist->setColumnText(0, "Tag");
    tablelist->setColumnText(1, "Value Name");
    tablelist->setColumnText(2, "Value Type");
    tablelist->setColumnHeaderHeight(tablelist->getColumnHeaderHeight() + 5);
    */
    openicon = new FXPNGIcon(this->getApp(), folderopen);
    openbutton = new FXButton(toolbar, "", openicon, this, ID_OPEN, BUTTON_TOOLBAR);
    managetagsicon = new FXPNGIcon(this->getApp(), managetags);
    managetagsbutton = new FXButton(toolbar, "", managetagsicon, this, ID_MANAGETAGS, BUTTON_TOOLBAR);
    previewicon = new FXPNGIcon(this->getApp(), reportpreview1);
    previewbutton = new FXButton(toolbar, "", previewicon, this, ID_PREVIEW, BUTTON_TOOLBAR);
    publishicon = new FXPNGIcon(this->getApp(), paperairplane2);
    publishbutton = new FXButton(toolbar, "", publishicon, this, ID_PUBLISH, BUTTON_TOOLBAR);
    abouticon = new FXPNGIcon(this->getApp(), helpcontents);
    aboutbutton = new FXButton(toolbar, "", abouticon, this, ID_ABOUT, BUTTON_TOOLBAR);
    statusbar->getStatusLine()->setNormalText("Open a SQLite File to Begin");
    fileuserdata.clear();
    sqlitefiles.clear();
    tags.clear();
    taggedlist.clear();
}

void WombatSqlite::create()
{
    FXMainWindow::create();
    show(PLACEMENT_SCREEN);
}

long WombatSqlite::TagMenu(FXObject*, FXSelector, void* ptr)
{
    /*
    FXEvent* event = (FXEvent*)ptr;
    if(tablelist->getCurrentRow() > -1 && !tablelist->getItemText(tablelist->getCurrentRow(), 1).empty())
    {
        if(!event->moved)
        {
            FXMenuPane tagmenu(this, POPUP_SHRINKWRAP);
            new FXMenuCommand(&tagmenu, "Create New Tag", new FXPNGIcon(this->getApp(), bookmarknew), this, ID_NEWTAG);
            new FXMenuSeparator(&tagmenu);
            for(int i=0; i < tags.size(); i++)
            {
                new FXMenuCommand(&tagmenu, FXString(tags.at(i).c_str()), new FXPNGIcon(this->getApp(), bookmark), this, ID_SETTAG);
            }
            new FXMenuSeparator(&tagmenu);
            new FXMenuCommand(&tagmenu, "Remove Tag", new FXPNGIcon(this->getApp(), bookmarkrem), this, ID_REMTAG);
            tagmenu.forceRefresh();
            tagmenu.create();
            tagmenu.popup(nullptr, event->root_x, event->root_y);
            getApp()->runModalWhileShown(&tagmenu);
        }
    }
    */
    return 1;
}

long WombatSqlite::CreateNewTag(FXObject*, FXSelector, void*)
{
    /*
    FXString tagstr = "";
    bool isset = FXInputDialog::getString(tagstr, this, "Enter Tag Name", "New Tag");
    if(isset)
    {
        tags.push_back(tagstr.text());
        tablelist->setItemText(tablelist->getCurrentRow(), 0, tagstr);
    }
    FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    taggedlist.append(tagstr + "|" + idkeyvalue + "|" + plaintext->getText());
    */
    return 1;
}

long WombatSqlite::RemoveTag(FXObject*, FXSelector, void*)
{
    /*
    tablelist->setItemText(tablelist->getCurrentRow(), 0, "");
    FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    */
    return 1;
}

long WombatSqlite::KeySelected(FXObject* sender, FXSelector, void*)
{
    /*
    FXTreeItem* curitem = treelist->getCurrentItem();
    bool toplevel = false;
    std::vector<FXString> pathitems;
    pathitems.clear();
    pathitems.push_back(curitem->getText());
    FXTreeItem* parent;
    FXTreeItem* child;
    child = curitem;
    while(toplevel == false)
    {
	parent = child->getParent();
	if(parent == NULL)
	    toplevel = true;
	else
	{
	    pathitems.push_back(parent->getText());
	    child = parent;
	}
    }
    FXString keypath = "";
    for(int i=pathitems.size() - 2; i > -1; i--)
    {
	keypath += "\\" + pathitems.at(i);
    }
    StatusUpdate(keypath);
    libregf_file_t* regfile = NULL;
    libregf_error_t* regerr = NULL;
    libregf_file_initialize(&regfile, &regerr);
    libregf_file_open(regfile, hivefilepath.c_str(), LIBREGF_OPEN_READ, &regerr);
    libregf_key_t* curkey = NULL;
    libregf_file_get_key_by_utf8_path(regfile, (uint8_t*)(keypath.text()), keypath.count(), &curkey, &regerr);
    // valid key, get values...
    int valuecount = 0;
    libregf_key_get_number_of_values(curkey, &valuecount, &regerr);
    tablelist->clearItems();
    plaintext->setText("");
    tablelist->setTableSize(valuecount, 3);
    tablelist->setColumnText(0, "Tag");
    tablelist->setColumnText(1, "Value Name");
    tablelist->setColumnText(2, "Value Type");
    FXString tagstr = "";
    if(valuecount == 0) // no values, so create empty key
    {
	tablelist->setTableSize(1, 3);
	tablelist->setColumnText(0, "Tag");
	tablelist->setColumnText(1, "Value Name");
	tablelist->setColumnText(2, "Value Type");
	FXString curtagvalue = keypath + "\\" + "(empty)";
	tablelist->setItemText(0, 1, "(empty)");
	tablelist->setItemText(0, 2, "0x00");
        for(int j=0; j < taggedlist.no(); j++)
        {
            if(taggedlist.at(j).contains(curtagvalue))
            {
                std::size_t found = taggedlist.at(j).find("|");
                tagstr = taggedlist.at(j).left(found);
            }
        }
        tablelist->setItemText(0, 0, tagstr);
    }
    for(int i=0; i < valuecount; i++)
    {
	libregf_value_t* curval = NULL;
	libregf_key_get_value(curkey, i, &curval, &regerr);
	size_t namesize = 0;
	libregf_value_get_utf8_name_size(curval, &namesize, &regerr);
	uint8_t name[namesize];
	libregf_value_get_utf8_name(curval, name, namesize, &regerr);
	uint32_t type = 0;
	libregf_value_get_value_type(curval, &type, &regerr);
	FXString curtagvalue = keypath + "\\";
	if(namesize == 0)
	{
	    curtagvalue += "(unnamed)";
	    tablelist->setItemText(i, 1, "(unnamed)");
	    FXString typestr = FXString::value(type, 16);
	    tablelist->setItemText(i, 2, typestr);
	}
	else
	{
	    curtagvalue += reinterpret_cast<char*>(name);
	    FXString valuetypestr = "";
	    tablelist->setItemText(i, 1, reinterpret_cast<char*>(name));
            if(type == 0x00) // none
            {
            }
            else if(type == 0x01) // reg_sz
            {
                valuetypestr = "REG_SZ";
            }
            else if(type == 0x02) // reg_expand_sz
            {
                valuetypestr = "REG_EXPAND_SZ";
            }
            else if(type == 0x03) // reg_binary
            {
                valuetypestr = "REG_BINARY";
            }
            else if(type == 0x04) // reg_dword reg_dword_little_endian (4 bytes)
            {
                valuetypestr = "REG_DWORD";
            }
            else if(type == 0x05) // reg_dword_big_endian (4 bytes)
            {
                valuetypestr = "REG_DWORD_BIG_ENDIAN";
            }
            else if(type == 0x06) // reg_link
            {
                valuetypestr = "REG_LINK";
            }
            else if(type == 0x07) // reg_multi_sz
            {
                valuetypestr = "REG_MULTI_SZ";
            }
            else if(type == 0x08) // reg_resource_list
            {
                valuetypestr = "REG_RESOURCE_LIST";
            }
            else if(type == 0x09) // reg_full_resource_descriptor
            {
                valuetypestr = "REG_FULL_RESOURCE_DESCRIPTOR";
            }
            else if(type == 0x0a) // reg_resource_requirements_list
            {
                valuetypestr = "REG_RESOURCE_REQUIREMENTS_LIST";
            }
            else if(type == 0x0b) // reg_qword_little_endian (8 bytes)
            {
                valuetypestr = "REG_QWORD";
            }
            else
            {
            }
	    tablelist->setItemText(i, 2, valuetypestr);
	}
        for(int j=0; j < taggedlist.no(); j++)
        {
            if(taggedlist.at(j).contains(curtagvalue))
            {
                std::size_t found = taggedlist.at(j).find("|");
                tagstr = taggedlist.at(j).left(found);
                tablelist->setItemText(i, 0, tagstr); 
            }
        }
	libregf_value_free(&curval, &regerr);
    }
    libregf_key_free(&curkey, &regerr);
    libregf_file_close(regfile, &regerr);
    libregf_file_free(&regfile, &regerr);
    libregf_error_free(&regerr);
    //tablelist->selectRow(0);
    //tablelist->setCurrentItem(0, 0);
    */
    return 1;
}

void WombatSqlite::GetRootString(FXTreeItem* curitem, FXString* rootstring)
{
    if(curitem->getParent() == NULL)
	*rootstring = curitem->getText();
    else
        GetRootString(curitem->getParent(), rootstring);
}

FXString WombatSqlite::ConvertUnixTimeToString(uint32_t input)
{
    time_t crtimet = (time_t)input;
    struct tm* dt;
    dt = gmtime(&crtimet);
    char timestr[30];
    strftime(timestr, sizeof(timestr), "%m/%d/%Y %I:%M:%S %p", dt);

    return timestr;
}

FXString WombatSqlite::ConvertWindowsTimeToUnixTimeUTC(uint64_t input)
{
    uint64_t temp;
    temp = input / TICKS_PER_SECOND; //convert from 100ns intervals to seconds;
    temp = temp - EPOCH_DIFFERENCE;  //subtract number of seconds between epochs
    time_t crtimet = (time_t)temp;
    struct tm* dt;
    dt = gmtime(&crtimet);
    char timestr[30];
    strftime(timestr, sizeof(timestr), "%m/%d/%Y %I:%M:%S %p", dt);

    return timestr;
}


long WombatSqlite::ValueSelected(FXObject*, FXSelector, void*)
{
    /*
    if(tablelist->getCurrentRow() > -1)
    {
	tablelist->selectRow(tablelist->getCurrentRow());
	int valueindex = tablelist->getCurrentRow();
        if(!tablelist->getItemText(tablelist->getCurrentRow(), 1).empty())
        {
            FXString valuename = tablelist->getItemText(tablelist->getCurrentRow(), 1);
            FXString valuetype = tablelist->getItemText(tablelist->getCurrentRow(), 2);
            FXTreeItem* curitem = treelist->getCurrentItem();
            FXString rootstring = "";
            FXString hivefilepath = "";
            GetRootString(curitem, &rootstring);
            for(int i=0; i < hives.size(); i++)
            {
                if(rootstring.contains(FXString(hives.at(i).string().c_str())))
                    hivefilepath = FXString(hives.at(i).string().c_str());
            }
            FXString keypath = statusbar->getStatusLine()->getNormalText();
            libregf_file_t* regfile = NULL;
            libregf_error_t* regerr = NULL;
            libregf_file_initialize(&regfile, &regerr);
            libregf_file_open(regfile, hivefilepath.text(), LIBREGF_OPEN_READ, &regerr);
            libregf_key_t* curkey = NULL;
            libregf_file_get_key_by_utf8_path(regfile, (uint8_t*)(keypath.text()), keypath.count(), &curkey, &regerr);
            libregf_value_t* curval = NULL;
            libregf_key_get_value(curkey, valueindex, &curval, &regerr);
            uint64_t lastwritetime = 0;
            libregf_key_get_last_written_time(curkey, &lastwritetime, &regerr);
            FXString valuedata = "Last Written Time:\t" + ConvertWindowsTimeToUnixTimeUTC(lastwritetime) + " UTC\n\n";
            valuedata += "Name:\t" + valuename + "\n\n";
            if(valuename.contains("(unnamed)"))
            {
                valuedata += "Content\n-------\n\n";
                valuedata += "Hex:\t0x" + FXString::value(valuetype.toInt(16), 16) + "\n";
                valuedata += "Integer:\t" + FXString::value(valuetype.toInt()) + "\n";
            }
            else
            {
                if(valuetype.contains("REG_SZ") || valuetype.contains("REG_EXPAND_SZ"))
                {
                    valuedata += "Content:\t";
                    size_t strsize = 0;
                    libregf_value_get_value_utf8_string_size(curval, &strsize, &regerr);
                    uint8_t valstr[strsize];
                    libregf_value_get_value_utf8_string(curval, valstr, strsize, &regerr);
                    valuedata += FXString(reinterpret_cast<char*>(valstr));
                }
                else if(valuetype.contains("REG_BINARY"))
                {
                    valuedata += "Content\n-------\n\n";
                    if(keypath.contains("UserAssist") && (keypath.contains("{750") || keypath.contains("{F4E") || keypath.contains("{5E6")))
                    {
                        valuedata += "ROT13 Decrypted Content:\t";
                        valuedata += DecryptRot13(valuename) + "\n";
                    }
                    else if(keypath.contains("SAM") && valuename.count() == 1 && valuename.contains("F"))
                    {
                        uint64_t tmp64 = 0;
                        size_t datasize = 0;
                        libregf_value_get_value_data_size(curval, &datasize, &regerr);
                        uint8_t data[datasize];
                        libregf_value_get_value_data(curval, data, datasize, &regerr);
                        valuedata += "Account Expiration:\t\t";
                        if(data[32] == 0xff)
                        {
                            valuedata += "No Expiration is Set\n";
                        }
                        else
                        {
                            tmp64 = (uint64_t)data[32] | (uint64_t)data[33] << 8 | (uint64_t)data[34] << 16 | (uint64_t)data[35] << 24 | (uint64_t)data[36] << 32 | (uint64_t)data[37] << 40 | (uint64_t)data[38] << 48 | (uint64_t)data[39] << 56;
                            valuedata += ConvertWindowsTimeToUnixTimeUTC(tmp64) + " UTC\n";
                        }
                        tmp64 = (uint64_t)data[8] | (uint64_t)data[9] << 8 | (uint64_t)data[10] << 16 | (uint64_t)data[11] << 24 | (uint64_t)data[12] << 32 | (uint64_t)data[13] << 40 | (uint64_t)data[14] << 48 | (uint64_t)data[15] << 56;
                        valuedata += "Last Logon Time:\t\t" + ConvertWindowsTimeToUnixTimeUTC(tmp64) + " UTC\n";
                        tmp64 = (uint64_t)data[40] | (uint64_t)data[41] << 8 | (uint64_t)data[42] << 16 | (uint64_t)data[43] << 24 | (uint64_t)data[44] << 32 | (uint64_t)data[45] << 40 | (uint64_t)data[46] << 48 | (uint64_t)data[47] << 56;
                        valuedata += "Last Failed Login:\t\t" + ConvertWindowsTimeToUnixTimeUTC(tmp64) + " UTC\n";
                        tmp64 = (uint64_t)data[24] | (uint64_t)data[25] << 8 | (uint64_t)data[26] << 16 | (uint64_t)data[27] << 24 | (uint64_t)data[28] << 32 | (uint64_t)data[29] << 40 | (uint64_t)data[30] << 48 | (uint64_t)data[31] << 56;
                        valuedata += "Last Time Password Changed:\t" + ConvertWindowsTimeToUnixTimeUTC(tmp64) + " UTC\n";
                    }
                    else if(valuename.contains("ShutdownTime"))
                    {
                        size_t datasize = 0;
                        libregf_value_get_value_data_size(curval, &datasize, &regerr);
                        uint8_t data[datasize];
                        libregf_value_get_value_data(curval, data, datasize, &regerr);
                        uint64_t tmp64 = (uint64_t)data[0] | (uint64_t)data[1] << 8 | (uint64_t)data[2] << 16 | (uint64_t)data[3] << 24 | (uint64_t)data[4] << 32 | (uint64_t)data[5] << 40 | (uint64_t)data[6] << 48 | (uint64_t)data[7] << 56;
                        valuedata += "Shutdown Time:\t" + ConvertWindowsTimeToUnixTimeUTC(tmp64) + " UTC\n";

                    }
                    else if(valuename.contains("MRUListEx"))
                    {
                        size_t datasize = 0;
                        libregf_value_get_value_data_size(curval, &datasize, &regerr);
                        uint8_t data[datasize];
                        libregf_value_get_value_data(curval, data, datasize, &regerr);
                        valuedata += "Order:\t[";
                        for(int i=0; i < sizeof(data) / 4; i++)
                        {
                            uint32_t tmp32 = (uint32_t)data[i*4] | (uint32_t)data[i*4 + 1] << 8 | (uint32_t)data[i*4 + 2] << 16 | (uint32_t)data[i*4 + 3] << 24;
                            if(tmp32 < 0xFFFFFFFF)
                                valuedata += FXString::value(tmp32);
                            if(i < ((sizeof(data) / 4) - 2))
                                valuedata += ", ";
                        }
                        valuedata += "]\n";
                    }
                    else if(keypath.contains("RecentDocs"))
                    {
                        if(!valuename.contains("MRUListEx"))
                        {
                            size_t datasize = 0;
                            libregf_value_get_value_data_size(curval, &datasize, &regerr);
                            uint8_t data[datasize];
                            libregf_value_get_value_data(curval, data, datasize, &regerr);
                            valuedata += "Name:\t";
                            for(int i=0; i < sizeof(data) / 2; i++)
                            {
                                uint16_t tmp16 = (uint16_t)data[i*2] | (uint16_t)data[i*2 + 1] << 8;
                                FXwchar tmpwc = FX::wc(&tmp16);
                                if(tmp16 == 0x0000)
                                    break;
                                valuedata += tmpwc;
                            }
                        }
                    }
                }
                else if(valuetype.contains("REG_DWORD"))
                {
                    valuedata += "Content:\t";
                    uint32_t dwordvalue = 0;
                    libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                    if(valuename.lower().contains("date"))
                        valuedata += ConvertUnixTimeToString(dwordvalue);
                    else
                        valuedata += FXString::value(dwordvalue);
                }
                else if(valuetype.contains("REG_DWORD_BIG_ENDIAN"))
                {
                    valuedata += "Content:\t";
                    uint32_t dwordvalue = 0;
                    libregf_value_get_value_32bit(curval, &dwordvalue, &regerr);
                    valuedata += FXString::value(dwordvalue);
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
                        valuedata += FXString(reinterpret_cast<char*>(valstr));
                    }
                    libregf_multi_string_free(&multistring, &regerr);
                }
                else if(valuetype.contains("REG_QWORD"))
                {
                    valuedata += "Content:\t";
                    uint64_t qwordvalue = 0;
                    libregf_value_get_value_64bit(curval, &qwordvalue, &regerr);
                    valuedata += FXString::value(qwordvalue);
                }
            }
            size_t datasize = 0;
            libregf_value_get_value_data_size(curval, &datasize, &regerr);
            uint8_t data[datasize];
            libregf_value_get_value_data(curval, data, datasize, &regerr);
            valuedata += "\n\nBinary Content\n--------------\n\n";
            if(datasize < 16)
            {
                valuedata += "0000\t";
                std::stringstream ss;
                ss << std::hex <<  std::setfill('0');
                for(int i=0; i < datasize; i++)
                    ss << std::setw(2) << ((uint)data[i]) << " ";
                valuedata += FXString(ss.str().c_str()).upper();
                for(int i=0; i < datasize; i++)
                {
                    if(isprint(data[i]))
                        valuedata += FXchar(reinterpret_cast<unsigned char>(data[i]));
                    else
                        valuedata += ".";
                }
                valuedata += "\n";
            }
            else
            {
                int linecount = datasize / 16;
                for(int i=0; i < linecount; i++)
                {
                    std::stringstream ss;
                    ss << std::hex << std::setfill('0') << std::setw(8) << i * 16 << "\t";
                    for(int j=0; j < 16; j++)
                    {
                        ss << std::setw(2) << ((uint)data[j+i*16]) << " ";
                    }
                    valuedata += FXString(ss.str().c_str()).upper();
                    for(int j=0; j < 16; j++)
                    {
                        if(isprint(data[j+i*16]))
                            valuedata += FXchar(reinterpret_cast<unsigned char>(data[j+i*16]));
                        else
                            valuedata += ".";
                    }
                    valuedata += "\n";
                }
            }
            plaintext->setText(valuedata);
            libregf_value_free(&curval, &regerr);
            libregf_key_free(&curkey, &regerr);
            libregf_file_close(regfile, &regerr);
            libregf_file_free(&regfile, &regerr);
            libregf_error_free(&regerr);
        }
    }
    */
    return 1;
}

FXString WombatSqlite::DecryptRot13(FXString encstr)
{
    FXString decstr = "";
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

FXchar WombatSqlite::Rot13Char(FXchar curchar)
{
    FXchar rot13char;
    if('0' <= curchar && curchar <= '4')
        rot13char = FXchar(curchar + 5);
    else if('5' <= curchar && curchar <= '9')
        rot13char = FXchar(curchar - 5);
    else if('A' <= curchar && curchar <= 'M')
        rot13char = FXchar(curchar + 13);
    else if('N' <= curchar && curchar <= 'Z')
        rot13char = FXchar(curchar - 13);
    else if('a' <= curchar && curchar <= 'm')
        rot13char = FXchar(curchar + 13);
    else if('n' <= curchar && curchar <= 'z')
        rot13char = FXchar(curchar - 13);
    else
        rot13char = curchar;
    return rot13char;
}

long WombatSqlite::OpenTagManager(FXObject*, FXSelector, void*)
{
    ManageTags tagmanager(this, "Manage Tags");
    tagmanager.SetTagList(&tags);
    tagmanager.execute(PLACEMENT_OWNER);
    return 1;
}

long  WombatSqlite::OpenAboutBox(FXObject*, FXSelector, void*)
{
    AboutBox aboutbox(this, "About Wombat Registry Forensics");
    aboutbox.execute(PLACEMENT_OWNER);
    return 1;
}

long WombatSqlite::PreviewReport(FXObject*, FXSelector, void*)
{
    viewer = new Viewer(this, "Report Preview");
    //viewer->GenerateReport(taggedlist, tags);
    viewer->execute(PLACEMENT_OWNER);

    return 1;
}

long WombatSqlite::PublishReport(FXObject*, FXSelector, void*)
{
    /*
    FXString startpath = FXString(getenv("HOME")) + "/";
    FXString filename = FXFileDialog::getSaveFilename(this, "Publish Report", startpath, "Text Files (*.txt)\nHTML Files (*.html,*.htm)");
    if(!filename.empty())
    {
        FXFile* outfile = new FXFile(filename, FXIO::Writing, FXIO::OwnerReadWrite);
        FXString buf;
        if(filename.contains(".htm"))
        {
            buf = "<html><head><title>Wombat Registry Report</title></head>\n";
            buf += "<body style='font-color: #3a291a; background-color: #d6ceb5;'>\n";
            buf += "<h2>Wombat Registry Report</h2>\n";
            buf += "<div id='toc'><h3>Contents</h3>\n";
            for(int j=0; j < tags.size(); j++)
            {
                int tagcnt = 0;
                for(int i=0; i < taggedlist.no(); i++)
                {
                    if(taggedlist.at(i).contains(tags.at(j).c_str()))
                        tagcnt++;
                }
                buf += "<div><a href='#t" + FXString::value(j) + "'>" + FXString(tags.at(j).c_str()) + " (" + FXString::value(tagcnt) + ")</a></div>\n";
            }
            buf += "<h3>Tagged Items</h3>";
            for(int i=0; i < tags.size(); i++)
            {
                buf += "<div id='t" + FXString::value(i) + "'><h4>" + tags.at(i).c_str() + "<span style='font-size: 10px;'>&nbsp;&nbsp;<a href='#toc'>TOP</a></span></h4>\n";
                for(int j=0; j < taggedlist.no(); j++)
                {
                    std::size_t found = taggedlist.at(j).find("|");
                    std::size_t rfound = taggedlist.at(j).rfind("|");
                    FXString itemtag = taggedlist.at(j).mid(0, found);
                    FXString itemhdr = taggedlist.at(j).mid(found+1, rfound - found - 1);
                    FXString itemcon = taggedlist.at(j).mid(rfound+1, taggedlist.at(j).length() - rfound);
                    if(itemtag == tags.at(i).c_str())
                    {
                        buf += "<div style='border-bottom: 1px solid black;'>\n";
                        buf += "<div>Key:&nbsp;&nbsp;&nbsp;&nbsp;" + itemhdr + "</div>\n";
                        buf += "<div><pre>" + itemcon + "</pre></div>\n";
                        buf += "</div>\n";
                    }
                }
            }
            buf += "</body></html>";
        }
        else
        {
            viewer->GetText(&buf);
        }
        outfile->writeBlock(buf.text(), buf.length());
        outfile->close();
    }
    */
    return 1;
}

long WombatSqlite::OpenSqliteFile(FXObject*, FXSelector, void*)
{
    if(prevsqlitepath.empty())
        prevsqlitepath = FXString(getenv("HOME")) + "/";
    sqlitefilepath = FXFileDialog::getOpenFilename(this, "Open SQLite File", prevsqlitepath);
    //std::cout << "sqlitefilepath: " << sqlitefilepath.text() << std::endl;
    if(!sqlitefilepath.empty())
    {
        prevsqlitepath = sqlitefilepath;
        filetype = 0;
        filebuffer.open(sqlitefilepath.text(), std::ios::in|std::ios::binary);
        filebufptr = &filebuffer;
        filebufptr->seekg(0, filebuffer.end);
        filesize = filebufptr->tellg();
        filebufptr->seekg(0, filebuffer.beg);
        uint8_t* shdr = new uint8_t[4];
        uint32_t sqlheader = 0;
        ReadContent(filebufptr, shdr, 0, 4);
        ReturnUint32(&sqlheader, shdr);
        delete[] shdr;
        //std::cout << "sql header:" << std::hex << sqlheader << std::endl;
        if(sqlheader == 0x377f0682 || sqlheader == 0x377f0683) // WAL
        {
            filetype = 1; // WAL
            uint8_t* psize = new uint8_t[4];
            pagesize = 0;
            ReadContent(filebufptr, psize, 8, 4);
            ReturnUint32(&pagesize, psize);
            delete[] psize;
            //std::cout << "WAL page size:" << pagesize << std::endl;
        }
        else
        {
            uint8_t* jh = new uint8_t[8];
            ReadContent(filebufptr, jh, 0, 8);
            uint64_t journalheader = 0;
            ReturnUint64(&journalheader, jh);
            delete[] jh;
            //std::cout << "journalheader: " << std::hex << journalheader << std::endl;
            if(journalheader == 0xd9d505f920a163d7) // JOURNAL
            {
                filetype = 2; // JOURNAL
                uint8_t* ps = new uint8_t[4];
                pagesize = 0;
                ReadContent(filebufptr, ps, 24, 4);
                ReturnUint32(&pagesize, ps);
                delete[] ps;
                //std::cout << "page size:" << pagesize << std::endl;
            }
            else
            {
                char* sqliteheader = new char[15];
                filebufptr->seekg(0);
                filebufptr->read(sqliteheader, 15);
                //ReadContent(filebufptr, sqliteheader, 0, 15);
                if(FXString(sqliteheader) == "SQLite format 3") // SQLITE DB
                {
                    filetype = 3; // SQLITE DB
                    uint8_t* ps = new uint8_t[2];
                    pagesize = 0;
                    uint16_t ps16 = 0;
                    ReadContent(filebufptr, ps, 16, 2);
                    ReturnUint16(&ps16, ps);
                    delete[] ps;
                    pagesize = ps16;
                    //std::cout << "pagesize: " << pagesize << std::endl;
                }
            }
        }
        if(filetype > 0)
        {
            sqlitefiles.append(sqlitefilepath);
            pagecount = filesize / pagesize;
            //std::cout << "page count:" << pagecount << std::endl;
            int found = sqlitefilepath.find_last_of("/");
            FXString itemstring = sqlitefilepath.right(sqlitefilepath.length() - found - 1) + " (" + FXString::value(pagecount) + ") [" + sqlitefilepath.left(found+1) + "]";
            FXListItem* rootitem = new FXListItem(itemstring);
            sqlfilelist->appendItem(rootitem);
            pagespinner->setRange(1, pagecount);
            pagespinner->setValue(1);
            ofpagelabel->setText(" of " + FXString::value(pagecount) + " pages");
            StatusUpdate("SQLite File: " + sqlitefilepath + " successfully opened.");
            // CURRENT ITEM INDEX(FILETYPE|PAGESIZE|CURPAGE)
            fileuserdata.append(FXString::value(filetype) + "|" + FXString::value(pagesize) + "|" + FXString::value(1));
            //std::cout << "filetype: " << FXString::value(filetype).text() << std::endl;
            // MAY NOT WANT TO SET SELECTED, SINCE IT DOESN'T SEEM TO TRIGGER THE FILESELECTED FUNCTION
            //sqlfilelist->selectItem(sqlfilelist->getNumItems() - 1);
        }
        else
            StatusUpdate("Not a SQLite file, file not opened.");
    }
    return 1;
}

long WombatSqlite::FileSelected(FXObject*, FXSelector, void*)
{
    FXString tmpstr = sqlfilelist->getItemText(sqlfilelist->getCurrentItem());
    curfileuserdata = fileuserdata.at(sqlfilelist->getCurrentItem());
    //std::cout << "cur file user data: " << curfileuserdata.text() << std::endl;
    int found = tmpstr.find(" (");
    int lfound = tmpstr.find("[");
    int rfound = tmpstr.find("]");
    curfilepath = tmpstr.mid(lfound+1, rfound-1) + tmpstr.mid(0, found);
    //std::cout << tmpstr.mid(0, found).text() << std::endl;
    //std::cout << tmpstr.mid(lfound+1, rfound-1).text() << std::endl;
    //std::cout << curfilepath.text() << std::endl;
    //std::cout << curfileuserdata.text() << std::endl;
    lfound = curfileuserdata.find_first_of("|");
    rfound = curfileuserdata.find_last_of("|");
    filetype = (uint8_t)curfileuserdata.mid(0, lfound).at(0);
    pagesize = curfileuserdata.mid(lfound+1, rfound-1).toULong();
    curpage = curfileuserdata.mid(rfound+1, curfileuserdata.length() - rfound - 1).toULong();
    pagespinner->setValue(curpage);
    proptable->setCurrentItem(-1, -1);
    LoadPage();
    //std::cout << curfileuserdata.mid(0, lfound).text() << std::endl;
    //std::cout << filetype << " " << pagesize << " " << curpage << std::endl;
    /*
    LoadPage();
     */ 
    return 1;
}

void WombatSqlite::LoadPage()
{
    //std::cout << "curpage: " << curpage << " filetype: " << filetype << std::endl;
    uint8_t* pagebuf = new uint8_t[pagesize];
    ReadContent(filebufptr, pagebuf, (curpage - 1) * pagesize, pagesize);
    if(curpage == 1)
    {
        uint8_t* pageheader = substr(pagebuf, 0, 100);
        ParseFileHeader(pageheader);
        PopulateFileHeader();
    }
    //std::cout << "content: " << pagebuf[0] << std::endl;
    /*
    //qDebug() << "file type:" << filetype << "pagesize:" << pagesize << "curpage:" << curpage;
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
     */ 
}

void WombatSqlite::ParseFileHeader(uint8_t* pageheader)
{
    //std::cout << "filetype: " << filetype << std::endl;
    //std::cout << "page hedaer: " << pageheader[0] << pageheader[1] << std::endl;
    if(filetype == '1') // WAL
    {
        uint8_t* wh = new uint8_t[4];
        wh = substr(pageheader, 0, 4);
        ReturnUint32(&walheader.header, wh);
        delete[] wh;
        uint8_t* fv = new uint8_t[4];
        fv = substr(pageheader, 4, 4);
        ReturnUint32(&walheader.fileversion, fv);
        delete[] fv;
        uint8_t* ps = new uint8_t[4];
        ps = substr(pageheader, 8, 4);
        ReturnUint32(&walheader.pagesize, ps);
        delete[] ps;
        uint8_t* csn = new uint8_t[4];
        csn = substr(pageheader, 12, 4);
        ReturnUint32(&walheader.checkptseqnum, csn);
        delete[] csn;
        uint8_t* s1 = new uint8_t[4];
        s1 = substr(pageheader, 16, 4);
        ReturnUint32(&walheader.salt1, s1);
        delete[] s1;
        uint8_t* s2 = new uint8_t[4];
        s2 = substr(pageheader, 20, 4);
        ReturnUint32(&walheader.salt2, s2);
        delete[] s2;
        uint8_t* cs1 = new uint8_t[4];
        cs1 = substr(pageheader, 24, 4);
        ReturnUint32(&walheader.checksum1, cs1);
        delete[] cs1;
        uint8_t* cs2 = new uint8_t[4];
        cs2 = substr(pageheader, 28, 4);
        ReturnUint32(&walheader.checksum2, cs2);
        delete[] cs2;
    }
    else if(filetype == '2') // JOURNAL
    {
        uint8_t* hd = new uint8_t[8];
        hd = substr(pageheader, 0, 8);
        ReturnUint64(&journalheader.header, hd);
        delete[] hd;
        uint8_t* pc = new uint8_t[4];
        pc = substr(pageheader, 8, 4);
        ReturnUint32(&journalheader.pagecnt, pc);
        delete[] pc;
        uint8_t* rn = new uint8_t[4];
        rn = substr(pageheader, 12, 4);
        ReturnUint32(&journalheader.randomnonce, rn);
        delete[] rn;
        uint8_t* is = new uint8_t[4];
        is = substr(pageheader, 16, 4);
        ReturnUint32(&journalheader.initsize, is);
        delete[] is;
        uint8_t* ss = new uint8_t[4];
        ss = substr(pageheader, 20, 4);
        ReturnUint32(&journalheader.sectorsize, ss);
        delete[] ss;
        uint8_t* ps = new uint8_t[4];
        ps = substr(pageheader, 24, 4);
        ReturnUint32(&journalheader.pagesize, ps);
        delete[] ps;
    }
    else if(filetype == '3') // SQLITE DB
    {
        sqliteheader.header = FXString((char*)substr(pageheader, 0, 16));
        //std::cout << "page header: " << substr(pageheader, 0, 16)[0] << std::endl;
        //std::cout << "sqlite header:" << sqliteheader.header.text() << std::endl;
        uint8_t* ps = new uint8_t[2];
        ps = substr(pageheader, 16, 2);
        ReturnUint16(&sqliteheader.pagesize, ps);
        delete[] ps;
        //std::cout << "pagesize: " << sqliteheader.pagesize << std::endl;
        sqliteheader.writeversion = pageheader[18];
        sqliteheader.readversion = pageheader[19];
        sqliteheader.unusedpagespace = pageheader[20];
        uint8_t* pc = new uint8_t[4];
        pc = substr(pageheader, 28, 4);
        ReturnUint32(&sqliteheader.pagecount, pc);
        delete[] pc;
        //std::cout << "page count: " << sqliteheader.pagecount << std::endl;
        uint8_t* ffpn = new uint8_t[4];
        ffpn = substr(pageheader, 32, 4);
        ReturnUint32(&sqliteheader.firstfreepagenum, ffpn);
        delete[] ffpn;
        uint8_t* fpc = new uint8_t[4];
        fpc = substr(pageheader, 36, 4);
        ReturnUint32(&sqliteheader.freepagescount, fpc);
        delete[] fpc;
        uint8_t* sc = new uint8_t[4];
        sc = substr(pageheader, 40, 4);
        ReturnUint32(&sqliteheader.schemacookie, sc);
        delete[] sc;
        uint8_t* sf = new uint8_t[4];
        sf = substr(pageheader, 44, 4);
        ReturnUint32(&sqliteheader.schemaformat, sf);
        delete[] sf;
        uint8_t* pcs = new uint8_t[4];
        pcs = substr(pageheader, 48, 4);
        ReturnUint32(&sqliteheader.pagecachesize, pcs);
        delete[] pcs;
        uint8_t* lrbpn = new uint8_t[4];
        lrbpn = substr(pageheader, 52, 4);
        ReturnUint32(&sqliteheader.largestrootbtreepagenumber, lrbpn);
        delete[] lrbpn;
        uint8_t* te = new uint8_t[4];
        te = substr(pageheader, 56, 4);
        ReturnUint32(&sqliteheader.textencoding, te);
        delete[] te;
        uint8_t* uv = new uint8_t[4];
        uv = substr(pageheader, 60, 4);
        ReturnUint32(&sqliteheader.userversion, uv);
        delete[] uv;
        uint8_t* ivm = new uint8_t[4];
        ivm = substr(pageheader, 64, 4);
        ReturnUint32(&sqliteheader.incrementalvacuummodebool, ivm);
        delete[] ivm;
        uint8_t* ai = new uint8_t[4];
        ai = substr(pageheader, 68, 4);
        ReturnUint32(&sqliteheader.appid, ai);
        delete[] ai;
        uint8_t* vvn = new uint8_t[4];
        vvn = substr(pageheader, 92, 4);
        ReturnUint32(&sqliteheader.versionvalidfornum, vvn);
        delete[] vvn;
        uint8_t* vn = new uint8_t[4];
        vn = substr(pageheader, 96, 4);
        ReturnUint32(&sqliteheader.version, vn);
        delete[] vn;
    }
}

void WombatSqlite::AddContent(int row, FXString islive, FXString rowid, FXString offlen, FXString type, FXString val, FXString tag)
{
    tablelist->setItemText(row, 0, tag);
    tablelist->setItemText(row, 1, islive);
    tablelist->setItemText(row, 2, rowid);
    tablelist->setItemText(row, 3, offlen);
    tablelist->setItemText(row, 4, type);
    tablelist->setItemText(row, 5, val);
}

void WombatSqlite::AddProperty(int row, FXString offlen, FXString val, FXString desc)
{
    proptable->setItemText(row, 0, offlen);
    proptable->setItemText(row, 1, val);
    proptable->setItemText(row, 2, desc);
}

void WombatSqlite::PopulateFileHeader()
{
    if(filetype == '1') // WAL
    {
        proptable->setTableSize(8, 3);
        proptable->setColumnText(0, "Offset, Length");
        proptable->setColumnText(1, "Value");
        proptable->setColumnText(2, "Description");
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << walheader.header;
        AddProperty(0, "0, 4", FXString("0x" + FXString(ss.str().c_str()).upper()), "WAL HEADER, last byte is either 0x82 or 0x83 which means something i forget right now.");
        AddProperty(1, "4, 4", FXString::value(walheader.fileversion), "WAL File Version");
        AddProperty(2, "8, 4", FXString::value(walheader.pagesize), "WAL Page Size");
        AddProperty(3, "12, 4", FXString::value(walheader.checkptseqnum), "WAL Checkpoint Sequence Number");
        AddProperty(4, "16, 4", FXString::value(walheader.salt1), "WAL Salt 1");
        AddProperty(5, "20, 4", FXString::value(walheader.salt2), "WAL Salt 2");
        AddProperty(6, "24, 4", FXString::value(walheader.checksum1), "WAL Checksum 1");
        AddProperty(7, "28, 4", FXString::value(walheader.checksum2), "WAL Checksum 2");
    }
    else if(filetype == '2') // JOURNAL
    {
    }
    else if(filetype == '3') // DB
    {
    }
    /*
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

     */ 
}

long WombatSqlite::PropertySelected(FXObject*, FXSelector, void*)
{
    proptable->selectRow(proptable->getCurrentRow());

    return 1;
    /*
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
     */ 
}

    /*
void WombatSqlite::PopulateChildKeys(libregf_key_t* curkey, FXTreeItem* curitem, libregf_error_t* regerr)
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
            FXString itemstring((reinterpret_cast<char*>(name)));
	    FXTreeItem* subitem = new FXTreeItem(itemstring);
	    treelist->appendItem(curitem, subitem);
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


long WombatSqlite::SetTag(FXObject* sender, FXSelector, void*)
{
    /*
    FXString tagstr = ((FXMenuCommand*)sender)->getText();
    tablelist->setItemText(tablelist->getCurrentRow(), 0, tagstr);
    FXString idkeyvalue = statusbar->getStatusLine()->getText() + "\\" + tablelist->getItemText(tablelist->getCurrentRow(), 1);
    for(int i=0; i < taggedlist.no(); i++)
    {
        if(taggedlist.at(i).contains(idkeyvalue))
            taggedlist.erase(i);
    }
    taggedlist.append(tagstr + "|" + idkeyvalue + "|" + plaintext->getText());
    */
    return 1;
}

int main(int argc, char* argv[])
{
    FXApp* wr = new FXApp("SQLite Forensics", "Wombat");

    wr->init(argc, argv);

    new WombatSqlite(wr);

    wr->create();
    wr->run();

    return 0;
}
