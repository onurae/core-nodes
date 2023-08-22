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
#include <iostream>

class MyApp : public GuiApp
{
public:
    MyApp();
    ~MyApp() final = default;

    void Update() override;
    void TestBasic() const;

private:
    bool newFile = true;
    std::string fileFormat{ ".dxdt" };
    std::string version{ "v0.1.0 " };
    bool open = true;
    bool redock = false;
    bool fileDialogOpen = false;

    std::unique_ptr<CoreDiagram> coreDiagram;
    FileDialog fileDialog;

    void Dockspace();
    void Menu();
    void FileNewOpenSave();
};