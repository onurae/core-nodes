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

void GainNode::DrawProperties()
{
    ImGui::Text(GetName().c_str());
    ImGui::Separator();
    ImGui::Text("Outputs input times parameter.");
    ImGui::NewLine();
    ImGui::Text("Parameters");
    ImGui::Separator();
    // TODO size of input text and gain value.
    std::string gainStr = "";
    gainStr.resize(16);
    gainStr = std::to_string(gain);
    ImGui::InputText("gain", gainStr.data(), 15, ImGuiInputTextFlags_CharsDecimal);
    // TODO detect change, modifFlag.
    try
    {
        gain = std::stod(gainStr);
    }
    catch (const std::exception&)
    {
        //
    }
}

void GainNode::SaveProperties(pugi::xml_node& xmlNode)
{
    SaveDouble(xmlNode, "gain", gain);
}

void GainNode::LoadProperties(pugi::xml_node& xmlNode)
{
    gain = LoadDouble(xmlNode, "gain");
}


