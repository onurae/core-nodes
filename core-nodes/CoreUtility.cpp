/******************************************************************************************
*                                                                                         *
*    Core Utility                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreUtility.hpp"

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
