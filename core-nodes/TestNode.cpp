/******************************************************************************************
*                                                                                         *
*    Test Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "TestNode.hpp"

void TestNode::Build()
{
    AddInput(CoreNodeInput("Input1", PortType::In, PortDataType::Float));
    AddInput(CoreNodeInput("Input2", PortType::In, PortDataType::Double));
    AddInput(CoreNodeInput("Input3", PortType::In, PortDataType::Int));
    AddInput(CoreNodeInput("Input4", PortType::In, PortDataType::Double));
    AddOutput(CoreNodeOutput("Output1", PortType::Out, PortDataType::Int));
    AddOutput(CoreNodeOutput("Output2", PortType::Out, PortDataType::Double));
    AddOutput(CoreNodeOutput("Output3", PortType::Out, PortDataType::Generic));
    BuildGeometry();
}

void TestNode::DrawProperties()
{
    ImGui::Text("Library Name: Test");
    ImGui::Text(GetName().c_str());
    ImGui::Separator();
    ImGui::Text("This is a test module explanation.");
    ImGui::NewLine();
    ImGui::Text("Parameters");
    ImGui::Separator();
    // TODO size of input text and gain value.
    std::string parameter1Str = "";
    std::string parameter2Str = "";
    parameter1Str.resize(16);
    parameter1Str = std::to_string(parameter1);
    parameter2Str.resize(16);
    parameter2Str = std::to_string(parameter2);
    ImGui::InputText("parameter1", parameter1Str.data(), 15, ImGuiInputTextFlags_CharsDecimal);
    ImGui::InputText("parameter2", parameter2Str.data(), 15, ImGuiInputTextFlags_CharsDecimal);
    // TODO detect change, modifFlag.
    try
    {
        parameter1 = std::stod(parameter1Str);
        parameter2 = std::stod(parameter2Str);
    }
    catch (const std::exception&)
    {
        //
    }
}

void TestNode::SaveProperties(pugi::xml_node& xmlNode)
{
    SaveDouble(xmlNode, "parameter1", parameter1);
    SaveDouble(xmlNode, "parameter2", parameter2);
}

void TestNode::LoadProperties(pugi::xml_node& xmlNode)
{
    parameter1 = LoadDouble(xmlNode, "parameter1");
    parameter2 = LoadDouble(xmlNode, "parameter2");
}


