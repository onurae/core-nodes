/******************************************************************************************
*                                                                                         *
*    Core Library                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreLibrary.hpp"

CoreNode* CoreLibrary::GetNode(std::string_view libName, const std::string& uniqueName)
{
    if (libName == "Gain")
    {
        return new GainNode(uniqueName);
    }
    if (libName == "Test")
    {
        return new TestNode(uniqueName);
    }
    return nullptr;
}

void CoreLibrary::Draw()
{
    DrawTooltip();

    int id = 0; // Branch id.
    if (ImGui::TreeNode("Nested"))
    {
        if (ImGui::TreeNode("Other"))
        {
            DrawBranch("Test", id, libMath);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    DrawBranch("Math", id, libMath);
}

void CoreLibrary::DrawTooltip() const
{
    if (leafClicked == true)
    {
        ImGui::BeginTooltip();
        ImGui::Text(std::string(selectedBranch + " / " + selectedLeaf).c_str());
        ImGui::EndTooltip();
    }
}

void CoreLibrary::DrawBranch(const std::string& name, int& id, const std::vector<std::string>& vec)
{
    id++;
    if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
    {
        for (int i = 0; i < vec.size(); i++)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
            if (iSelectedLeaf == i && iSelectedBranch == id)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, vec.at(i).c_str());
            if (ImGui::IsItemClicked())
            {
                iSelectedLeaf = i;
                iSelectedBranch = id;
                selectedLeaf = vec.at(i);
                selectedBranch = name;
                leafClicked = true;
            }
        }
        ImGui::TreePop();
    }
}