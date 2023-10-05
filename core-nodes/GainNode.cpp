/******************************************************************************************
*                                                                                         *
*    Gain Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "GainNode.hpp"

void GainNode::Build()
{
    AddInput(CoreNodeInput("Input", PortType::In, PortDataType::Double));
    AddOutput(CoreNodeOutput("Output", PortType::Out, PortDataType::Double));
    BuildGeometry();
}

void GainNode::DrawProperties(const std::vector<CoreNode*>& coreNodeVec)
{
    ImGui::Text(GetLibName().c_str());
    ImGui::Separator();
    ImGui::Text("Outputs input times parameter.");
    ImGui::NewLine();
    ImGui::Text("Parameters");
    ImGui::Separator();
    EditName(coreNodeVec);
    gain.Draw(modifFlag);
}

void GainNode::SaveProperties(pugi::xml_node& xmlNode)
{
    SaveDouble(xmlNode, "gain", gain.Get());
}

void GainNode::LoadProperties(pugi::xml_node& xmlNode)
{
    gain.Set(LoadDouble(xmlNode, "gain"));
}