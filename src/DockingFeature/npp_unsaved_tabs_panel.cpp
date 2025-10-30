#include "npp_unsaved_tabs_panel.h"
#include <windows.h>
#include <tchar.h>
#include "resource.h"

void UnsavedTabsPanel::updateList(const std::vector<std::wstring>& files, int count)
{
    if (!isCreated()) return;

    // Update title
    TCHAR title[64];
    _stprintf_s(title, TEXT("Unsaved Tabs (%d)"), count);
    ::SendMessage(_hSelf, WM_SETTEXT, 0, (LPARAM)title);

    // Fill the listbox (assumes the dialog has a listbox IDC_LIST1)
    HWND list = ::GetDlgItem(_hSelf, IDC_UNSAVED_LIST);
    ::SendMessage(list, LB_RESETCONTENT, 0, 0);
    for (auto& f : files)
        ::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)f.c_str());
}

INT_PTR CALLBACK UnsavedTabsPanel::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    
    switch (message)
    {
    case WM_INITDIALOG:
        OutputDebugString(TEXT("[UnsavedTabsPanel] WM_INITDIALOG\n"));
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_UNSAVED_LIST:
            // handle listbox clicks, etc.
            break;
        }
        break;

    case WM_DESTROY:
        OutputDebugString(TEXT("[UnsavedTabsPanel] WM_DESTROY\n"));
        break;
    }

    return FALSE;
}
