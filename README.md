# Unsaved Tabs for Notepad++

**Unsaved Tabs** is a lightweight Notepad++ plugin that shows you every document (tab)
with unsaved changes — no more losing track of quick notes or half-written scripts.

It adds a small command under **Plugins -> Unsaved Tabs** that:

- Displays the number of unsaved documents  
- Lets you open a docked panel listing each unsaved file  
- More features planned

---

## 🛠 Installation

1. Download the latest release from  
   **[Releases](https://github.com/Munchicken/npp-unsaved-tabs/releases)**  
2. Extract the ZIP so you have:
C:\Program Files\Notepad++\plugins\npp_unsaved_tabs\npp_unsaved_tabs.dll (Notepadd++ --> Plugins menu --> Open plugins folder)

3. Restart Notepad++.  
The plugin will appear under **Plugins -> Unsaved Tabs**.

---

## 💻 Building from source

Prerequisites  
- Visual Studio 2019 or 2022 (with “Desktop development with C++”)

Steps  
```
git clone https://github.com/Munchicken/npp-unsaved-tabs.git
cd npp-unsaved-tabs
# open npp-unsaved-tabs\vs.proj\npp_unsaved_tabs.vcxproj in Visual Studio
# Build -> Release | x64
The compiled DLL will be in npp-unsaved-tabs\vs.proj\x64\Release\npp_unsaved_tabs.dll.
```
---
## 🗺 Roadmap
| Version | Features |
|---------|----------|
| v1.0.0 | Core tracking + fetch count + real-time unsaved-tabs list + Dockable panel |
| v1.1.0 | Real-time display |
| v1.2+ | Save, sort/filter, and settings |
---
Created by Sarah Pierce for Hacktoberfest 2025

![Hacktoberfest](https://img.shields.io/badge/Hacktoberfest-2025-blueviolet)
![Notepad++ Plugin](https://img.shields.io/badge/Notepad++-Plugin-green)

