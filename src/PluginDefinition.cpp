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
#include "DockingFeature/npp_unsaved_tabs_panel.h"
#include "DockingFeature/resource.h"
#include "DockingFeature/DockingDlgInterface.h"

std::unordered_set<INT_PTR> g_dirty;   // Track all unsaved buffers
static UnsavedTabsPanel g_unsavedPanel;
extern HINSTANCE g_hModule;

static HWND g_hStatusBar = nullptr;
static HWND g_hUnsavedLabel = nullptr;
static bool g_showStatusBarCount = true;

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
void pluginInit(HANDLE hModule)
{
    g_hModule = (HINSTANCE)hModule;
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
    setCommand(0, TEXT("Check Unsaved Tabs Count"), cmdShowUnsavedTabsCount, nullptr, false);
    setCommand(1, TEXT("Show Unsaved Tabs Panel"), showUnsavedPanel, nullptr, false);
    setCommand(3, TEXT("Show Unsaved Count in Status Bar"), toggleStatusBarCount, nullptr, true);

    // Find the real Windows status-bar control inside Notepad++
    g_hStatusBar = ::FindWindowEx(nppData._nppHandle, nullptr,
        TEXT("msctls_statusbar32"), nullptr);

    if (g_hStatusBar && !g_hUnsavedLabel)
    {
        RECT rc{};
        ::GetClientRect(g_hStatusBar, &rc);

        // Create a child static control on the right side
        g_hUnsavedLabel = ::CreateWindowEx(
            0, TEXT("STATIC"), TEXT("Unsaved: 0"),
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            rc.right - 150, 2, 140, rc.bottom - 4,
            g_hStatusBar, nullptr, g_hModule, nullptr);
    }
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

void showUnsavedPanel()
{
    if (!g_unsavedPanel.isCreated())
    {
        g_unsavedPanel.init(g_hModule, nppData._nppHandle);
        
        // Prepare docking data first (zero-init)
        tTbData data{};
        // Pass address of data to create() so it can fill fields safely
        g_unsavedPanel.create(&data, false);

        // Prepare docking data
        data.dlgID = IDD_UNSAVEDTABS_PANEL;
        data.uMask = DWS_DF_CONT_RIGHT;
        data.hIconTab = nullptr;      // optional: load icon later
        data.pszAddInfo = nullptr;      // optional
        data.pszModuleName = g_unsavedPanel.getPluginFileName();

        // Register with Notepad++
        ::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
    }

    // Show/bring to front and update
    g_unsavedPanel.display();   // show or bring to front
    updateUnsavedUI();          // populate content
}

// ------------------------------------------------------------------
// Real-time unsaved count display helpers
// ------------------------------------------------------------------
static void updateStatusBarCount(int count)
{
    if (g_hUnsavedLabel && g_showStatusBarCount)
    {
        TCHAR msg[64];
        _stprintf_s(msg, TEXT("Unsaved: %d"), count);
        ::SetWindowText(g_hUnsavedLabel, msg);
        ::ShowWindow(g_hUnsavedLabel, SW_SHOW);
    }
    else if (g_hUnsavedLabel)
    {
        ::ShowWindow(g_hUnsavedLabel, SW_HIDE);
    }
}

void toggleStatusBarCount()
{
    g_showStatusBarCount = !g_showStatusBarCount;

    // Update the menu checkmark
    ::CheckMenuItem(::GetMenu(nppData._nppHandle),
        funcItem[4]._cmdID,
        MF_BYCOMMAND | (g_showStatusBarCount ? MF_CHECKED : MF_UNCHECKED));

    // Show or hide the label window if it exists
    if (g_hUnsavedLabel)
    {
        ::ShowWindow(g_hUnsavedLabel,
            g_showStatusBarCount ? SW_SHOW : SW_HIDE);
    }
}

void updateUnsavedUI()
{
    std::vector<std::pair<INT_PTR, std::wstring>> orderedUnsaved;

    // Iterate views in order: main (0) then secondary (1)
    for (int view = 0; view < 2; ++view)
    {
        int nbFiles = (int)::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, view);

        for (int i = 0; i < nbFiles; ++i)
        {
            // Get buffer ID for each tab position
            INT_PTR bid = (INT_PTR)::SendMessage(nppData._nppHandle, NPPM_GETBUFFERIDFROMPOS, i, view);

            // Skip if not unsaved
            if (g_dirty.find(bid) == g_dirty.end())
                continue;

            // Get file path or display name
            TCHAR path[MAX_PATH]{};
            ::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, bid, (LPARAM)path);

            if (path[0] == TEXT('\0'))
            {
                // Handle new/unsaved docs
                TCHAR name[32];
                _stprintf_s(name, TEXT("(new %d)"), i + 1);
                orderedUnsaved.emplace_back(bid, name);
            }
            else
            {
                orderedUnsaved.emplace_back(bid, path);
            }
        }
    }

    g_unsavedPanel.updateList(orderedUnsaved);

    // Count unsaved tabs
    int count = (int)orderedUnsaved.size();

    // Update displays
    updateStatusBarCount(count);
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
        updateUnsavedUI();
        break;

    case SCN_SAVEPOINTLEFT:              // user typed -> dirty
        g_dirty.insert(getBufferId());
        updateUnsavedUI();
        break;

    case SCN_SAVEPOINTREACHED:           // user saved -> clean
    case NPPN_FILESAVED:
    case NPPN_FILECLOSED:
        g_dirty.erase(getBufferId());
        updateUnsavedUI();
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
