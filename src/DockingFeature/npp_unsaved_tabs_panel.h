#pragma once
#include "DockingDlgInterface.h"
#include <vector>
#include <string>
#include "resource.h"

class UnsavedTabsPanel : public DockingDlgInterface
{
public:
    UnsavedTabsPanel() : DockingDlgInterface(IDD_UNSAVEDTABS_PANEL) {}
    void updateList(const std::vector<std::wstring>& files, int count);

protected:
    virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
