#include "npp_unsaved_tabs_panel.h"
#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "../PluginDefinition.h"  // for nppData

void UnsavedTabsPanel::updateList(const std::vector<std::pair<INT_PTR, std::wstring>>& files)
{
    if (!isCreated()) return;

    _entries = files;

    // Update title
    TCHAR title[64];
    _stprintf_s(title, TEXT("Unsaved Tabs (%d)"), (int)files.size());
    ::SendMessage(_hSelf, WM_SETTEXT, 0, (LPARAM)title);

    // Fill the listbox (assumes the dialog has a listbox IDC_LIST1)
    HWND list = ::GetDlgItem(_hSelf, IDC_UNSAVED_LIST);
    ::SendMessage(list, LB_RESETCONTENT, 0, 0);
    for (auto& f : files)
        ::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)f.second.c_str());

    INT_PTR currentBid = (INT_PTR)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
    highlightByBufferId(currentBid);

}

INT_PTR CALLBACK UnsavedTabsPanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    
    switch (message)
    {
    case WM_INITDIALOG:
        OutputDebugString(TEXT("[UnsavedTabsPanel] WM_INITDIALOG\n"));
        return TRUE;

    case WM_COMMAND:
        // Detect double-click in the list box
        if (HIWORD(wParam) == LBN_DBLCLK && LOWORD(wParam) == IDC_UNSAVED_LIST)
        {
            HWND list = ::GetDlgItem(_hSelf, IDC_UNSAVED_LIST);
            int sel = (int)::SendMessage(list, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR && sel < (int)_entries.size())
            {
                INT_PTR bufferId = _entries[sel].first;
                if (bufferId)
                {
                    // Debug
                    TCHAR msg[128];
                    _stprintf_s(msg, TEXT("Trying to activate bufferId = %p"), (void*)bufferId);
                    OutputDebugString(msg);

                    // 1 Find its position in main view
                    int index = (int)::SendMessage(nppData._nppHandle, NPPM_GETPOSFROMBUFFERID, (WPARAM)bufferId, 0);

                    // 2 If not in main view, check secondary
                    int view = 0;
                    if (index == -1)
                    {
                        index = (int)::SendMessage(nppData._nppHandle, NPPM_GETPOSFROMBUFFERID, (WPARAM)bufferId, 1);
                        view = 1;
                    }

                    // 3 Activate if found
                    if (index != -1)
                    {
                        TCHAR buf[128];
                        _stprintf_s(buf, TEXT("Activating bufferId %p (view=%d, index=%d)"), (void*)bufferId, view, index);
                        OutputDebugString(buf);

                        ::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, view, index);
                    }
                    else
                    {
                        OutputDebugString(TEXT("Buffer not found in either view\n"));
                    }
                }
            }
        }

    case WM_DESTROY:
        OutputDebugString(TEXT("[UnsavedTabsPanel] WM_DESTROY\n"));
        break;
    }

    return FALSE;
}

void UnsavedTabsPanel::highlightByBufferId(INT_PTR bufferId)
{
    if (!isCreated())
        return;

    HWND list = ::GetDlgItem(_hSelf, IDC_UNSAVED_LIST);
    if (!list)
        return;

    // Find the index of the buffer in our current entries
    int sel = -1;
    for (int i = 0; i < (int)_entries.size(); ++i)
    {
        if (_entries[i].first == bufferId)
        {
            sel = i;
            break;
        }
    }

    // Apply selection (or clear if not present)
    ::SendMessage(list, LB_SETCURSEL, (WPARAM)sel, 0);

    // Optional: force a repaint if you don’t immediately see the focus cue
    ::InvalidateRect(list, nullptr, TRUE);
}
