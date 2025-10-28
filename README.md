# Unsaved Tabs for Notepad++

**Unsaved Tabs** is a lightweight Notepad++ plugin that shows you every document (tab)
with unsaved changes — no more losing track of quick notes or half-written scripts.

It adds a small command under **Plugins → Unsaved Tabs** that:

- Displays the number of unsaved documents in real time  
- Lets you open a dialog listing each unsaved file  
- (Upcoming) Adds **Save All Unsaved Tabs** and session-safety tools

---

## 🛠 Installation

1. Download the latest release from  
   **[Releases](https://github.com/Munchicken/npp-unsaved-tabs/releases)**  
2. Extract the ZIP so you have:
%AppData%\Notepad++\plugins\UnsavedTabs\UnsavedTabs.dll

3. Restart Notepad++.  
The plugin will appear under **Plugins → Unsaved Tabs**.

---

## 💻 Building from source

Prerequisites  
- Visual Studio 2019 or 2022 (with “Desktop development with C++”)  
- C++17 or later  
- The official Notepad++ plugin template → <https://github.com/npp-plugins/plugintemplate>

Steps  
```
git clone https://github.com/Munchicken/npp-unsaved-tabs.git
cd npp-unsaved-tabs
# open npp-unsaved-tabs\vs.proj\npp-unsaved-tabs.vcxproj in Visual Studio
# Build → Release | x64
The compiled DLL will be in npp-unsaved-tabs\vs.proj\x64\Release\UnsavedTabs.dll.
```
---
## 🗺 Roadmap
| Version | Features |
|---------|----------|
| v1.0.0 | Core tracking + unsaved-tabs list |
| v1.1.0 | Save-All command, timestamp filter |
| v1.2.0 | Dockable panel and settings |
---
Created by Sarah Pierce for Hacktoberfest 2025

![Hacktoberfest](https://img.shields.io/badge/Hacktoberfest-2025-blueviolet)
![Notepad++ Plugin](https://img.shields.io/badge/Notepad++-Plugin-green)

