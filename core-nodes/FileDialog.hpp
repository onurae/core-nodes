/******************************************************************************************
*                                                                                         *
*    File Dialog                                                                          *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP

#include <filesystem>
#include "imgui.h"
#include "imgui_internal.h"
#include "pugixml.hpp"
#include "Notifier.hpp"

class FileDialog
{
public:
    enum class Type
    {
        SAVE,
        OPEN
    };
private:
    Type type = Type::OPEN;
    std::string fileFormat{ ".dxdt" };
    std::filesystem::path fileName;
    std::filesystem::path directoryPath;
    std::filesystem::path resultPath;
    bool refresh;
    size_t currentIndex;
    std::vector<std::filesystem::directory_entry> currentFiles;
    std::vector<std::filesystem::directory_entry> currentDirectories;
    std::string title{ "" };
    static const size_t bufferSize = 200;
    char buffer[bufferSize];

public:
    FileDialog() = default;
    virtual ~FileDialog() = default;

    void SetType(Type t) { type = t; }
    void SetFileName(std::string_view name) { fileName = name; }
    void SetDirectory(const std::filesystem::path& dir) { directoryPath = dir; }
    std::filesystem::path GetResultPath() const { return resultPath; }
    auto GetFileName() const { return fileName; }
    auto GetFileFormat() const { return fileFormat; }
    Type GetType() const { return type; }
    bool Draw(bool* open);
};

void SaveImRect(pugi::xml_node& xmlNode, const std::string& name, const ImRect& rect);
void SaveImVec2(pugi::xml_node& xmlNode, const std::string& name, const ImVec2& vec);
void SaveImColor(pugi::xml_node& xmlNode, const std::string& name, const ImColor& color);
void SaveFloat(pugi::xml_node& xmlNode, const std::string& name, float data);
void SaveString(pugi::xml_node& xmlNode, const std::string& name, const std::string& str);
void SaveInt(pugi::xml_node& xmlNode, const std::string& name, int i);
void SaveBool(pugi::xml_node& xmlNode, const std::string& name, bool b);

ImRect LoadImRect(const pugi::xml_node& xmlNode, const std::string& name);
ImVec2 LoadImVec2(const pugi::xml_node& xmlNode, const std::string& name);
ImColor LoadImColor(const pugi::xml_node& xmlNode, const std::string& name);
float LoadFloat(const pugi::xml_node& xmlNode, const std::string& name);
std::string LoadString(const pugi::xml_node& xmlNode, const std::string& name);
int LoadInt(const pugi::xml_node& xmlNode, const std::string& name);
bool LoadBool(const pugi::xml_node& xmlNode, const std::string& name);

#endif /* FILEDIALOG_HPP */