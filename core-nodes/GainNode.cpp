/******************************************************************************************
*                                                                                         *
*    Gain Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "GainNode.hpp"
#include <iostream>

void GainNode::Build()
{
    AddInput(CoreNodeInput("Input", PortType::In, PortDataType::Double));
    AddOutput(CoreNodeOutput("Output", PortType::Out, PortDataType::Double));
    BuildGeometry();
}

void GainNode::DrawProperties()
{
    ImGui::Text(GetName().c_str());
    ImGui::Separator();
    ImGui::Text("Outputs input times parameter.");
    ImGui::NewLine();
    ImGui::Text("Parameters");
    ImGui::Separator();
    ImGui::SetNextItemWidth(100);
    ImGui::InputDouble("gain", &iGain, 0.0, 0.0, "%.15g");
    if (ImGui::IsItemActive() == false && gain != iGain)
    {
        modifFlag = true;
        gain = iGain;
    }
}

void GainNode::SaveProperties(pugi::xml_node& xmlNode)
{
    SaveDouble(xmlNode, "gain", gain);
}

void GainNode::LoadProperties(pugi::xml_node& xmlNode)
{
    gain = LoadDouble(xmlNode, "gain");
    iGain = gain;
}


