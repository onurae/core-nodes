/******************************************************************************************
*                                                                                         *
*    Core Utility                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef COREUTILITY_HPP
#define COREUTILITY_HPP

#include "pugixml.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>

void SaveImRect(pugi::xml_node& xmlNode, const std::string& name, const ImRect& rect);
void SaveImVec2(pugi::xml_node& xmlNode, const std::string& name, const ImVec2& vec);
void SaveImColor(pugi::xml_node& xmlNode, const std::string& name, const ImColor& color);
void SaveFloat(pugi::xml_node& xmlNode, const std::string& name, float data);
void SaveString(pugi::xml_node& xmlNode, const std::string& name, const std::string& str);
void SaveInt(pugi::xml_node& xmlNode, const std::string& name, int i);
void SaveBool(pugi::xml_node& xmlNode, const std::string& name, bool b);

ImVec2 LoadImVec2(const pugi::xml_node& xmlNode, const std::string& name);
float LoadFloat(const pugi::xml_node& xmlNode, const std::string& name);

#endif /* COREUTILITY_HPP */