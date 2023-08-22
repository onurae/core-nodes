﻿/******************************************************************************************
*                                                                                         *
*    File Dialog                                                                          *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "FileDialog.hpp"

bool FileDialog::Draw(bool* open)
{
    if (*open == false)
    {
        return false;
    }

    bool done = false;
    title = (type == Type::OPEN) ? "Open File" : "Save File";
    ImGui::SetNextWindowSize(ImVec2(660.0f, 410.0f), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(ImVec2(410, 410), ImVec2(1280, 410));

    if (ImGui::Begin(title.c_str(), open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        if (currentFiles.empty() && currentDirectories.empty() || refresh)
        {
            refresh = false;
            currentIndex = 0;
            currentFiles.clear();
            currentDirectories.clear();
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath))
            {
                entry.is_directory() ? currentDirectories.push_back(entry) : currentFiles.push_back(entry);
            }
        }

        // Path
        ImGui::Text("%s", directoryPath.string().c_str());
        ImGui::BeginChild("##browser", ImVec2(ImGui::GetWindowContentRegionWidth(), 300.0f), true, ImGuiWindowFlags_None);
        size_t index = 0;

        // Parent
        if (directoryPath.has_parent_path())
        {
            if (ImGui::Selectable("..", currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0)))
            {
                currentIndex = index;
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    directoryPath = directoryPath.parent_path();
                    refresh = true;
                }
            }
            index++;
        }

        // Directories
        for (const auto& element : currentDirectories)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 210, 0, 255));
            if (ImGui::Selectable(element.path().filename().string().c_str(), currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0)))
            {
                currentIndex = index;
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    directoryPath = element.path();
                    refresh = true;
                }
            }
            ImGui::PopStyleColor();
            index++;
        }

        // Files
        for (const auto& element : currentFiles)
        {
            if (ImGui::Selectable(element.path().filename().string().c_str(), currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0)))
            {
                currentIndex = index;
                fileName = element.path().filename();
            }
            index++;
        }
        ImGui::EndChild();

        // Draw filename
        size_t fileNameSize = fileName.string().size();
        if (fileNameSize >= bufferSize)
        {
            fileNameSize = bufferSize - 1;
        }
        std::memcpy(buffer, fileName.string().c_str(), fileNameSize);
        buffer[fileNameSize] = 0;

        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 200, 0, 255));
        if (ImGui::InputText("File Name", buffer, bufferSize))
        {
            fileName = std::string(buffer);
            currentIndex = 0;
        }
        ImGui::PopStyleColor();
        if (ImGui::Button("Cancel"))
        {
            refresh = false;
            currentIndex = 0;
            currentFiles.clear();
            currentDirectories.clear();
            *open = false;
        }

        ImGui::SameLine();
        if (std::string buttonName = (type == Type::OPEN) ? "Open" : "Save"; ImGui::Button(buttonName.c_str()))
        {
            resultPath = directoryPath / fileName;
            if ((type == Type::OPEN) ? std::filesystem::exists(resultPath) : !std::filesystem::exists(resultPath))
            {
                refresh = false;
                currentIndex = 0;
                currentFiles.clear();
                currentDirectories.clear();
                done = true;
                *open = false;
            }
        }
    }
    ImGui::End();
    return done;
}

void SaveImRect(pugi::xml_node& xmlNode, const std::string& name, const ImRect& rect)
{
    auto child = xmlNode.append_child(name.c_str());
    child.append_attribute("xMin") = rect.Min.x;
    child.append_attribute("xMax") = rect.Max.x;
    child.append_attribute("yMin") = rect.Min.y;
    child.append_attribute("yMax") = rect.Max.y;
}
void SaveImVec2(pugi::xml_node& xmlNode, const std::string& name, const ImVec2& vec)
{
    auto child = xmlNode.append_child(name.c_str());
    child.append_attribute("x") = vec.x;
    child.append_attribute("y") = vec.y;
}
void SaveImColor(pugi::xml_node& xmlNode, const std::string& name, const ImColor& color)
{
    auto child = xmlNode.append_child(name.c_str());
    child.append_attribute("x") = color.Value.x;
    child.append_attribute("y") = color.Value.y;
    child.append_attribute("z") = color.Value.z;
    child.append_attribute("w") = color.Value.w;
}
void SaveFloat(pugi::xml_node& xmlNode, const std::string& name, float data)
{
    xmlNode.append_child(name.c_str()).append_attribute("data").set_value(data);
}
void SaveString(pugi::xml_node& xmlNode, const std::string& name, const std::string& str)
{
    xmlNode.append_child(name.c_str()).append_attribute("data").set_value(str.c_str());
}
void SaveInt(pugi::xml_node& xmlNode, const std::string& name, int i)
{
    xmlNode.append_child(name.c_str()).append_attribute("data").set_value(i);
}
void SaveBool(pugi::xml_node& xmlNode, const std::string& name, bool b)
{
    xmlNode.append_child(name.c_str()).append_attribute("data").set_value(b);
}

ImRect LoadImRect(const pugi::xml_node & xmlNode, const std::string & name)
{
    auto min = ImVec2(xmlNode.child(name.c_str()).attribute("xMin").as_float(), xmlNode.child(name.c_str()).attribute("yMin").as_float());
    auto max = ImVec2(xmlNode.child(name.c_str()).attribute("xMax").as_float(), xmlNode.child(name.c_str()).attribute("yMax").as_float());
    return ImRect(min, max);
}

ImVec2 LoadImVec2(const pugi::xml_node & xmlNode, const std::string & name)
{
    float x = xmlNode.child(name.c_str()).attribute("x").as_float();
    float y = xmlNode.child(name.c_str()).attribute("y").as_float();
    return ImVec2(x, y);
}

ImColor LoadImColor(const pugi::xml_node & xmlNode, const std::string & name)
{
    float r = xmlNode.child(name.c_str()).attribute("x").as_float();
    float g = xmlNode.child(name.c_str()).attribute("y").as_float();
    float b = xmlNode.child(name.c_str()).attribute("z").as_float();
    float a = xmlNode.child(name.c_str()).attribute("w").as_float();
    return ImColor(r, g, b, a);
}

float LoadFloat(const pugi::xml_node & xmlNode, const std::string & name)
{
    return xmlNode.child(name.c_str()).attribute("data").as_float();
}

std::string LoadString(const pugi::xml_node & xmlNode, const std::string & name)
{
    return xmlNode.child(name.c_str()).attribute("data").as_string();
}

int LoadInt(const pugi::xml_node & xmlNode, const std::string & name)
{
    return xmlNode.child(name.c_str()).attribute("data").as_int();
}

bool LoadBool(const pugi::xml_node & xmlNode, const std::string & name)
{
    return xmlNode.child(name.c_str()).attribute("data").as_bool();
}