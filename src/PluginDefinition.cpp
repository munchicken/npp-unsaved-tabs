//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "Notepad_plus_msgs.h"
#include "Scintilla.h"
#include <windows.h>
#include <tchar.h>
#include <unordered_set>

std::unordered_set<INT_PTR> g_dirty;   // Track all unsaved buffers

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Show Unsaved Tabs (Test)"), cmdShowUnsavedTabsTest, nullptr, false);
    setCommand(1, TEXT("Check Unsaved Tabs Count"), cmdShowUnsavedTabsCount, nullptr, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void cmdShowUnsavedTabsTest()
{
    ::MessageBox(nullptr,
        TEXT("Plugin loaded successfully!"),
        TEXT("Unsaved Tabs"),
        MB_OK | MB_ICONINFORMATION);
}

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

static INT_PTR getBufferId()
{
    return (INT_PTR)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
}

int getUnsavedTabsCount()
{
    return (int)g_dirty.size();
}

// ---------------------------------------------------------------------
// Notifications - Handle notifications sent from the exported beNotified()
// ---------------------------------------------------------------------

void handleUnsavedTabsNotifications(SCNotification* notify)
{
    switch (notify->nmhdr.code)
    {
    case NPPN_SNAPSHOTDIRTYFILELOADED:   // red-disk unsaved files restored
        g_dirty.insert(notify->nmhdr.idFrom);
        break;

    case SCN_SAVEPOINTLEFT:              // user typed -> dirty
        g_dirty.insert(getBufferId());
        break;

    case SCN_SAVEPOINTREACHED:           // user saved -> clean
    case NPPN_FILESAVED:
    case NPPN_FILECLOSED:
        g_dirty.erase(getBufferId());
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------
// Core command
// ---------------------------------------------------------------------

void cmdShowUnsavedTabsCount()
{
    int count = getUnsavedTabsCount();
    TCHAR msg[128];
    _stprintf_s(msg, TEXT("%d unsaved tab%s detected."),
        count, (count == 1 ? TEXT("") : TEXT("s")));

    ::MessageBox(nullptr, msg, TEXT("Unsaved Tabs"), MB_OK | MB_ICONINFORMATION);
}
