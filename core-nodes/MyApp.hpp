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
    bool hasFile = false;
    std::filesystem::path filePath;
    std::string version{ "v0.1.0" };
    bool open = true;
    bool redock = false;
    bool fileDialogOpen = false;

    std::unique_ptr<CoreDiagram> coreDiagram;
    FileDialog fileDialog;

    void Dockspace();
    void Menu();
    void MenuFile();
    void MenuView();
    void MenuAbout();

    void DrawFileDialog();

    pugi::xml_document CreateDoc() const;
    void SaveButtonClick();
    void SaveToFile(const std::string& fName, const std::string& fPath);
    void LoadDoc(const pugi::xml_document* doc);
    void LoadFromFile();

    void SelectTab(const char* windowName) const;

    std::deque<pugi::xml_document> docs;
    int iCurrentDoc{ 0 };
    int iSavedDoc{ 0 };
    const int maxSavedDoc{ 20 };
    void UndoRedoSave();
    void ResetDocDeque();
};