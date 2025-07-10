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

void TestNode::DrawProperties(const std::vector<CoreNode*>& coreNodeVec)
{
    ImGui::Text(GetLibName().c_str());
    ImGui::Separator();
    ImGui::Text("This is a test module explanation.");
    ImGui::NewLine();
    ImGui::Text("Parameters");
    ImGui::Separator();
    EditName(coreNodeVec);
    parameter1.Draw(modifFlag);
    parameter2.Draw(modifFlag);
}

void TestNode::SaveProperties(pugi::xml_node& xmlNode)
{
    SaveDouble(xmlNode, "parameter1", parameter1.Get());
    SaveDouble(xmlNode, "parameter2", parameter2.Get());
}

void TestNode::LoadProperties(const pugi::xml_node& xmlNode)
{
    parameter1.Set(LoadDouble(xmlNode, "parameter1"));
    parameter2.Set(LoadDouble(xmlNode, "parameter2"));
}


