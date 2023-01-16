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
        uint32_t sqlheader = 0;
        ReadContent(filebufptr, &sqlheader, 0);
        //std::cout << "sql header:" << std::hex << sqlheader << std::endl;

        if(sqlheader == 0x377f0682 || sqlheader == 0x377f0683) // WAL
        {
            filetype = 1; // WAL
            ReadContent(filebufptr, &pagesize, 8);
            /*
            uint8_t* psize = new uint8_t[4];
            pagesize = 0;
            ReadContent(filebufptr, psize, 8, 4);
            ReturnUint32(&pagesize, psize);
            delete[] psize;
            */
            //std::cout << "WAL page size:" << pagesize << std::endl;
        }
        else
        {
            uint64_t journalheader = 0;
            ReadContent(filebufptr, &journalheader, 0);
            /*
            uint8_t* jh = new uint8_t[8];
            ReadContent(filebufptr, jh, 0, 8);
            uint64_t journalheader = 0;
            ReturnUint64(&journalheader, jh);
            delete[] jh;
            */
            //std::cout << "journalheader: " << std::hex << journalheader << std::endl;
            if(journalheader == 0xd9d505f920a163d7) // JOURNAL
            {
                filetype = 2; // JOURNAL
                ReadContent(filebufptr, &pagesize, 24);
                /*
                uint8_t* ps = new uint8_t[4];
                pagesize = 0;
                ReadContent(filebufptr, ps, 24, 4);
                ReturnUint32(&pagesize, ps);
                delete[] ps;
                */
                //std::cout << "page size:" << pagesize << std::endl;
            }
            else
            {
                char* sqliteheader = new char[15];
                filebufptr->seekg(0);
                filebufptr->read(sqliteheader, 15);
                if(FXString(sqliteheader) == "SQLite format 3") // SQLITE DB
                {
                    filetype = 3; // SQLITE DB
                    uint16_t ps = 0;
                    ReadContent(filebufptr, &ps, 16);
                    pagesize = ps;
                    /*
                    uint8_t* ps = new uint8_t[2];
                    pagesize = 0;
                    uint16_t ps16 = 0;
                    ReadContent(filebufptr, ps, 16, 2);
                    ReturnUint16(&ps16, ps);
                    delete[] ps;
                    pagesize = ps16;
                    */
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
    ParsePageHeader(pagebuf, filetype, curpage);


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
        ReadInteger(pageheader, 0, &walheader.header);
        ReadInteger(pageheader, 4, &walheader.fileversion);
        ReadInteger(pageheader, 8, &walheader.pagesize);
        ReadInteger(pageheader, 12, &walheader.checkptseqnum);
        ReadInteger(pageheader, 16, &walheader.salt1);
        ReadInteger(pageheader, 20, &walheader.salt2);
        ReadInteger(pageheader, 24, &walheader.checksum1);
        ReadInteger(pageheader, 28, &walheader.checksum2);
        /*
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
        */
    }
    else if(filetype == '2') // JOURNAL
    {
        ReadInteger(pageheader, 0, &journalheader.header);
        ReadInteger(pageheader, 8, &journalheader.pagecnt);
        ReadInteger(pageheader, 12, &journalheader.randomnonce);
        ReadInteger(pageheader, 16, &journalheader.initsize);
        ReadInteger(pageheader, 20, &journalheader.sectorsize);
        ReadInteger(pageheader, 24, &journalheader.pagesize);
        /*
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
        */
    }
    else if(filetype == '3') // SQLITE DB
    {
        sqliteheader.header = FXString((char*)substr(pageheader, 0, 16));
        ReadInteger(pageheader, 16, &sqliteheader.pagesize);
        //std::cout << "page header: " << substr(pageheader, 0, 16)[0] << std::endl;
        //std::cout << "sqlite header:" << sqliteheader.header.text() << std::endl;
        /*
        uint8_t* ps = new uint8_t[2];
        ps = substr(pageheader, 16, 2);
        ReturnUint16(&sqliteheader.pagesize, ps);
        delete[] ps;
        */
        //std::cout << "pagesize: " << sqliteheader.pagesize << std::endl;
        sqliteheader.writeversion = pageheader[18];
        sqliteheader.readversion = pageheader[19];
        sqliteheader.unusedpagespace = pageheader[20];
        ReadInteger(pageheader, 28, &sqliteheader.pagecount);
        ReadInteger(pageheader, 32, &sqliteheader.firstfreepagenum);
        ReadInteger(pageheader, 36, &sqliteheader.freepagescount);
        ReadInteger(pageheader, 40, &sqliteheader.schemacookie);
        ReadInteger(pageheader, 44, &sqliteheader.schemaformat);
        ReadInteger(pageheader, 48, &sqliteheader.pagecachesize);
        ReadInteger(pageheader, 52, &sqliteheader.largestrootbtreepagenumber);
        ReadInteger(pageheader, 56, &sqliteheader.textencoding);
        ReadInteger(pageheader, 60, &sqliteheader.userversion);
        ReadInteger(pageheader, 64, &sqliteheader.incrementalvacuummodebool);
        ReadInteger(pageheader, 68, &sqliteheader.appid);
        ReadInteger(pageheader, 92, &sqliteheader.versionvalidfornum);
        ReadInteger(pageheader, 96, &sqliteheader.version);
        /*
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
        */
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
    if(filetype == '1')
        proptable->setTableSize(8, 3);
    else if(filetype == '2')
        proptable->setTableSize(6, 3);
    else if(filetype == '3')
        proptable->setTableSize(18, 3);
    proptable->setColumnText(0, "Offset, Length");
    proptable->setColumnText(1, "Value");
    proptable->setColumnText(2, "Description");
    if(filetype == '1') // WAL
    {
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
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << journalheader.header;
        AddProperty(0, "0, 8", FXString("0x" + FXString(ss.str().c_str()).upper()), "JOURNAL HEADER");
        AddProperty(1, "8, 4", FXString::value(journalheader.pagecnt), "Journal Page Count");
        AddProperty(2, "12, 4", FXString::value(journalheader.randomnonce), "Journal Random Nonce");
        AddProperty(3, "16, 4", FXString::value(journalheader.initsize), "Journal Initial Size");
        AddProperty(4, "20, 4", FXString::value(journalheader.sectorsize), "Journal Sector Size");
        AddProperty(5, "24, 4", FXString::value(journalheader.pagesize), "Journal Page Size");
    }
    else if(filetype == '3') // DB
    {
        AddProperty(0, "0, 16", sqliteheader.header, "SQLite Header");
        AddProperty(1, "16, 2", FXString::value(sqliteheader.pagesize), "SQLite Page Size");
        AddProperty(2, "18, 1", FXString::value(sqliteheader.writeversion), "SQLite Write Version");
        AddProperty(3, "19, 1", FXString::value(sqliteheader.readversion), "SQLite Read Version");
        AddProperty(4, "20, 1", FXString::value(sqliteheader.unusedpagespace), "SQLite Unused Page Space");
        AddProperty(5, "28, 4", FXString::value(sqliteheader.pagecount), "SQLite Page Count");
        AddProperty(6, "32, 4", FXString::value(sqliteheader.firstfreepagenum), "SQLite First Free Page Number");
        AddProperty(7, "36, 4", FXString::value(sqliteheader.freepagescount), "SQLite Free Pages Count");
        AddProperty(8, "40, 4", FXString::value(sqliteheader.schemacookie), "SQLite Schema Cookie");
        AddProperty(9, "44, 4", FXString::value(sqliteheader.schemaformat), "SQLite Schema Format");
        AddProperty(10, "48, 4", FXString::value(sqliteheader.pagecachesize), "SQLite Page Cache Size");
        AddProperty(11, "52, 4", FXString::value(sqliteheader.largestrootbtreepagenumber), "SQLite Largest Root B-Tree Page Number");
        AddProperty(12, "56, 4", FXString::value(sqliteheader.textencoding), "SQLite Text Encoding");
        AddProperty(13, "60, 4", FXString::value(sqliteheader.userversion), "SQLite User Version");
        AddProperty(14, "64, 4", FXString::value(sqliteheader.incrementalvacuummodebool), "SQLite Incremental Vacuum Mode Boolean");
        AddProperty(15, "68, 4", FXString::value(sqliteheader.appid), "SQLite App ID");
        AddProperty(16, "92, 4", FXString::value(sqliteheader.versionvalidfornum), "SQLite Version Valid for Number");
        AddProperty(17, "96, 4", FXString::value(sqliteheader.version), "SQLite Version");
    }
    proptable->fitColumnsToContents(2);
    AlignColumn(proptable, 2, FXTableItem::LEFT);
}

void WombatSqlite::ParsePageHeader(uint8_t* pagearray, uint8_t fileheader, uint64_t curpage)
{
    uint64_t curpos = 0;
    uint64_t cellarrayoffset = 0;
    uint rowcnt = 0;
    if(curpage == 1)
    {
        if(filetype == '1') // WAL
            curpos = 32;
        else if(filetype == '2') // JOURNAL
            curpos = 28;
        else if(filetype == '3') // DB
            curpos = 100;
        rowcnt = proptable->getNumRows();
    }
    if(filetype == '1') // WAL
    {
        ReadInteger(pagearray, curpos, &frameheader.pagenumber);
        ReadInteger(pagearray, curpos + 4, &frameheader.pagecount);
        ReadInteger(pagearray, curpos + 8, &frameheader.salt1);
        ReadInteger(pagearray, curpos + 12, &frameheader.salt2);
        ReadInteger(pagearray, curpos + 16, &frameheader.checksum1);
        ReadInteger(pagearray, curpos + 20, &frameheader.checksum1);
    }
    else if(filetype == '2') // JOURNAL
    {
    }
    else if(filetype == '3') // DB
    {
    }
    /*
     * TOO MUCH IN THIS FUNCTION, NEED TO BREAK IT OUT FOR SIMPLICITY
     *
    if(filetype == 1) // WAL
    {
        frameheader.pagenumber = qFromBigEndian<quint32>(pagearray->mid(curpos, 4));
        frameheader.pagecount = qFromBigEndian<quint32>(pagearray->mid(curpos + 4, 4));
        frameheader.salt1 = qFromBigEndian<quint32>(pagearray->mid(curpos + 8, 4));
        frameheader.salt2 = qFromBigEndian<quint32>(pagearray->mid(curpos + 12, 4));
        frameheader.checksum1 = qFromBigEndian<quint32>(pagearray->mid(curpos + 16, 4));
        frameheader.checksum2 = qFromBigEndian<quint32>(pagearray->mid(curpos + 24, 4));
        qDebug() << "frameheader:" << frameheader.pagenumber << frameheader.pagecount << frameheader.salt1 << frameheader.salt2 << frameheader.checksum1 << frameheader.checksum2;
        // AFTER THE FRAMEHEADER IS A PAGE, WHICH IS THE PAGESIZE OF THE DB PAGESIZE FROM THE FRAMEHEADER, SO I NEED TO REDO THE
        // HEX DISPLAY AND PAGE DISPLAY FOR THE WAL FILES...
    }
    else if(filetype == 2) // JOURNAL
    {
    }
    else if(filetype == 3) // SQLITE DB
    {
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
                    /*
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
    }
    //qDebug() << "cell offset array count: " << celloffarray.count() << celloffarray;
     */
}

void WombatSqlite::AlignColumn(FXTable* curtable, int col, FXuint justify)
{
    for(int i=0; i < curtable->getNumRows(); i++)
        curtable->setItemJustify(i, col, justify);
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
