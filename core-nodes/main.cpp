/******************************************************************************************
*                                                                                         *
*    Core Nodes                                                                           *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "gui-app-template/GuiApp.hpp"
#include "CoreDiagram.hpp"
#include <memory>

class CoreNodes : public GuiApp
{
public:
    CoreNodes() : GuiApp("CoreNodes")
    {
        SetTitle("Untitled", false);
        coreDiagram = std::make_unique<CoreDiagram>();
    };
    ~CoreNodes() final = default;

    void Update() override
    {
        //TestBasic();
        TestDock();
    }

    void TestBasic() const
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

    void TestDock()
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

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", nullptr, false, true))
                {
                    SetTitle("Untitled", true);
                    // TODO want to save the current project?
                    coreDiagram = std::make_unique<CoreDiagram>();
                }
                if (ImGui::MenuItem("Save", nullptr, false, true))
                {
                    pugi::xml_document doc; auto declarationNode = doc.append_child(pugi::node_declaration);
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

                    std::string filePath = "myModel.cono";
                    bool saveSucceeded = doc.save_file(filePath.c_str(), PUGIXML_TEXT("  "));
                    if (saveSucceeded)
                    {
                        SetTitle("Untitled", false);
                    }
                }
                if (ImGui::MenuItem("Load", nullptr, false, true))
                {
                    bool load = true;
                    pugi::xml_document doc;
                    std::string filePath = "myModel.cono";
                    pugi::xml_parse_result result = doc.load_file(filePath.c_str(), pugi::parse_default | pugi::parse_declaration);
                    if (result)
                    {
                        pugi::xml_node root = doc.document_element();
                        // TODO check core-nodes version.
                        // TODO want to save the current project?
                        coreDiagram = std::make_unique<CoreDiagram>();
                        coreDiagram->Load(root);
                    }
                    else
                    {
                        //Load Error.
                    }
                }
                if (ImGui::MenuItem("SaveAs", nullptr, false, true))
                {
                    // name
                    SetTitle("asdasd", false);
                    // call save with a new name.
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Redock", nullptr, false, true))
                {
                    redock = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("About"))
            {
                ImGui::MenuItem("https://github.com/onurae", nullptr, true, false);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
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
    }

private:
    bool open = true;
    bool redock = false;

    std::unique_ptr<CoreDiagram> coreDiagram;
};

int main()
{
    CoreNodes coreNodes;
    coreNodes.Run();
    return 0;
}