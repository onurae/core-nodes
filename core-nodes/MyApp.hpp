/******************************************************************************************
*                                                                                         *
*    MyApp                                                                                *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "gui-app-template/GuiApp.hpp"
#include "CoreDiagram.hpp"
#include <memory>
#include <deque>
#include <iostream>

class MyApp : public GuiApp
{
public:
    MyApp();
    ~MyApp() final = default;

    void Update() override;
    void TestBasic() const;

private:
    bool initialSetup = false;
    void SelectTab(const char* windowName) const;
    std::string version{ "v0.1.0" };
    std::unique_ptr<CoreDiagram> coreDiagram;

    bool openDockspace = true;
    bool redock = false;
    void Dockspace();

    void Menu();
    void MenuFile();
    void MenuView();
    void MenuHelp();

    FileDialog fileDialog;
    bool fileDialogOpen = false;
    void DrawFileDialog();

    bool hasFile = false;
    std::filesystem::path filePath;
    void NewProject();
    void OpenProject();
    bool openSaveModal = false;
    int stateSaveModal = 0; // 1: from new, 2: from open, 3: from exit.
    void DrawSaveModal();
    pugi::xml_document CreateDoc() const;
    void SaveProject(bool saveAs = false);
    void SaveToFile(const std::string& fName, const std::string& fPath);
    void LoadDoc(const pugi::xml_document* doc);
    void LoadFromFile();

    std::deque<pugi::xml_document> docs;
    int iCurrentDoc{ 0 };
    int iSavedDoc{ 0 };
    const int maxSavedDoc{ 20 };
    void UndoRedoSave();
    void ResetDocDeque();

    bool openAbout = false;
    void DrawAbout();
};