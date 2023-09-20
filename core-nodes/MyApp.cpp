/******************************************************************************************
*                                                                                         *
*    MyApp                                                                                *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "MyApp.hpp"

void MyApp::Update()
{
    //TestBasic();
    Dockspace();
    DrawFileDialog();
    ImGui::PushFont(fontLarge);
    Notifier::Draw();
    ImGui::PopFont();
    UndoRedoSave();
}

MyApp::MyApp() : GuiApp("MyApp")
{
    SetTitle("untitled");
    coreDiagram = std::make_unique<CoreDiagram>();
}

void MyApp::Dockspace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", &open, window_flags);
    ImGui::PopStyleVar(3);

    if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == nullptr || redock)
    {
        redock = false;
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_id_left_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
        ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left_top, ImGuiDir_Down, 0.60f, nullptr, &dock_id_left_top);

        ImGui::DockBuilderDockWindow("Simulation", dock_id_left_top);
        ImGui::DockBuilderDockWindow("Library", dock_id_left_top);
        ImGui::DockBuilderDockWindow("Diagram", dock_main_id);
        ImGui::DockBuilderDockWindow("Properties", dock_id_left_bottom);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    Menu();
    ImGui::End();

    ImGui::Begin("Simulation", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Text");
    ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Library", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Text");
    ImGui::End();

    ImGui::Begin("Diagram", nullptr, ImGuiWindowFlags_None);
    coreDiagram->Update();
    ImGui::End();

    ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Text");
    ImGui::End();

    if (initialSetup == false)
    {
        SelectTab("Simulation");
        initialSetup = true;
        ImGui::SetWindowFocus("Diagram");
        ResetDocDeque();
    }
}

void MyApp::Menu()
{
    if (ImGui::BeginMenuBar())
    {
        MenuFile();
        MenuView();
        MenuAbout();
        ImGui::EndMenuBar();
    }
}

void MyApp::MenuFile()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", nullptr, false, true))
        {
            // TODO want to save the current project?
            SetTitle("new");
            hasFile = false;
            SetAsterisk(false);
            coreDiagram = std::make_unique<CoreDiagram>();
            ResetDocDeque();
        }
        if (ImGui::MenuItem("Open", nullptr, false, true))
        {
            // TODO want to save the current project?
            fileDialogOpen = true;
            fileDialog.SetType(FileDialog::Type::OPEN);
            fileDialog.SetFileName(fileDialog.GetFileFormat());
            fileDialog.SetDirectory(hasFile ? filePath.parent_path() : std::filesystem::current_path());
        }
        if (ImGui::MenuItem("Save", nullptr, false, true))
        {
            SaveButtonClick();
        }
        if (ImGui::MenuItem("Save As...", nullptr, false, true))
        {
            fileDialogOpen = true;
            fileDialog.SetType(FileDialog::Type::SAVE);
            fileDialog.SetFileName("untitled");
            fileDialog.SetDirectory(filePath.parent_path());
        }
        ImGui::EndMenu();
    }
}

void MyApp::MenuView()
{
    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Redock", nullptr, false, true))
        {
            redock = true;
        }
        ImGui::EndMenu();
    }
}

void MyApp::MenuAbout()
{
    if (ImGui::BeginMenu("About"))
    {
        ImGui::MenuItem("https://github.com/onurae", nullptr, true, false);
        ImGui::EndMenu();
    }
}

void MyApp::DrawFileDialog()
{
    if (fileDialog.Draw(&fileDialogOpen))
    {
        if (fileDialog.GetType() == FileDialog::Type::OPEN)
        {
            LoadFromFile();
        }
        else if (fileDialog.GetType() == FileDialog::Type::SAVE)
        {
            SaveToFile(fileDialog.GetFileName().string(), fileDialog.GetResultPath().string());
        }
    }
}

pugi::xml_document MyApp::CreateDoc() const
{
    pugi::xml_document doc;
    auto declarationNode = doc.append_child(pugi::node_declaration);
    declarationNode.append_attribute("version") = "1.0";
    declarationNode.append_attribute("encoding") = "ISO-8859-1";
    declarationNode.append_attribute("standalone") = "yes";

    auto root = doc.append_child("core-nodes");
    root.append_attribute("version").set_value("v0.1.0");
    auto sim = root.append_child("simulation");
    sim.append_attribute("solver").set_value(1);
    sim.append_attribute("sampleTime").set_value(0.01);
    sim.append_attribute("stopTime").set_value(5.0);
    sim.append_attribute("speed").set_value("realTime");
    coreDiagram->Save(root);
    return doc;
}

void MyApp::SaveButtonClick()
{
    if (hasFile == false)
    {
        fileDialogOpen = true;
        fileDialog.SetType(FileDialog::Type::SAVE);
        fileDialog.SetFileName("untitled");
        fileDialog.SetDirectory(std::filesystem::current_path());
    }
    else
    {
        SaveToFile(GetTitle(), filePath.string());
    }
}

void MyApp::SaveToFile(const std::string& fName, const std::string& fPath)
{
    auto doc = CreateDoc();
    if (doc.save_file(fPath.c_str(), PUGIXML_TEXT("  ")))
    {
        filePath = fPath;
        hasFile = true;
        SetTitle(fName);
        SetAsterisk(false);
        iSavedDoc = iCurrentDoc;
        Notifier::Add(Notif(Notif::Type::SUCCESS, "Saved"));
    }
    else
    {
        Notifier::Add(Notif(Notif::Type::ERROR, "Save failed"));
    }
}

void MyApp::LoadDoc(const pugi::xml_document* doc)
{
    pugi::xml_node root = doc->document_element();
    // TODO read simulation related parameters here.
    coreDiagram = std::make_unique<CoreDiagram>();
    coreDiagram->Load(root);
}

void MyApp::LoadFromFile()
{
    auto fPath = fileDialog.GetResultPath().string();
    auto fNameWFormat = fileDialog.GetFileName().string();
    auto fNameWoFormat = fNameWFormat.substr(0, fNameWFormat.rfind("."));
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(fPath.c_str(), pugi::parse_default | pugi::parse_declaration);
    if (result)
    {
        // TODO check version.
        LoadDoc(&doc);
        ResetDocDeque();
        filePath = fileDialog.GetResultPath();
        hasFile = true;
        SetTitle(fNameWoFormat);
        SetAsterisk(false);
        Notifier::Add(Notif(Notif::Type::SUCCESS, "Loaded"));
    }
    else
    {
        Notifier::Add(Notif(Notif::Type::ERROR, "Load failed"));
    }
}

void MyApp::SelectTab(const char* windowName) const
{
    ImGuiWindow* window = ImGui::FindWindowByName(windowName);
    if (window == nullptr || window->DockNode == nullptr || window->DockNode->TabBar == nullptr) { return; }
    window->DockNode->TabBar->NextSelectedTabId = window->TabId;
}

void MyApp::UndoRedoSave()
{
    // If project modified, add doc. TODO: other modifications
    if (coreDiagram->GetModificationFlag() == true)
    {
        if (iCurrentDoc < docs.size() - 1)
        {
            docs.erase(docs.begin() + iCurrentDoc + 1, docs.end());
        }
        docs.emplace_back(CreateDoc());
        iCurrentDoc += 1;
        coreDiagram->ResetModificationFlag();
        SetAsterisk(true);
        if (docs.size() > maxSavedDoc)
        {
            docs.pop_front();
            iCurrentDoc -= 1;
            iSavedDoc -= 1;
        }
        return;
    }

    // Undo
    const auto& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        if (iCurrentDoc > 0)
        {
            auto doc = &(docs[iCurrentDoc - 1]);
            iCurrentDoc -= 1;
            LoadDoc(doc);
            if (iCurrentDoc == iSavedDoc)
            {
                SetAsterisk(false);
            }
            else
            {
                SetAsterisk(true);
            }
            Notifier::Add(Notif(Notif::Type::SUCCESS, "Undo"));
            return;
        }
        Notifier::Add(Notif(Notif::Type::INFO, "Cannot undo"));
    }

    // Redo
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
    {
        if (iCurrentDoc < docs.size() - 1)
        {
            auto doc = &(docs[iCurrentDoc + 1]);
            iCurrentDoc += 1;
            LoadDoc(doc);
            if (iCurrentDoc == iSavedDoc)
            {
                SetAsterisk(false);
            }
            else
            {
                SetAsterisk(true);
            }
            Notifier::Add(Notif(Notif::Type::SUCCESS, "Redo"));
            return;
        }
        Notifier::Add(Notif(Notif::Type::INFO, "Cannot redo"));
    }

    // Save
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S) && (iSavedDoc != iCurrentDoc || hasFile == false))
    {
        SaveButtonClick();
    }
}

void MyApp::ResetDocDeque()
{
    docs.clear();
    iCurrentDoc = 0;
    iSavedDoc = 0;
    docs.emplace_back(CreateDoc());
}

void MyApp::TestBasic() const
{
    ImGui::Begin("Basic", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Hello, world %d", 123);
    ImGui::Text("This is RobotoMedium %d", 123); // the first loaded font gets used by default
    ImGui::PushFont(fontRobotoRegular); // use ImGui::PushFont()/ImGui::PopFont() to change the font at runtime
    ImGui::Text("This is RobotoRegular %d", 123);
    ImGui::PopFont();
    ImGui::Text(u8"\uefe1 Hello % d", 123);
    ImGui::Text(u8"\uea16 Hello % d", 123);

    ImGui::PushFont(fontLarge);
    ImGui::Text(u8"\uf011 This is RobotoMedium24");
    ImGui::PopFont();

    ImGui::Spacing();
    static float f = 0;
    if (ImGui::Button(u8"\ue1c4 Run")) { f = 0.0f; }
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::End();
}
