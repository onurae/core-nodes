/******************************************************************************************
*                                                                                         *
*    Gain Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef GAINNODE_HPP
#define GAINNODE_HPP

#include "CoreNode.hpp"

class GainNode : public CoreNode
{
public:
    explicit GainNode(const std::string& uniqueName);
    ~GainNode() final = default;

    void DrawProperties() override
    {
        ImGui::Text(GetName().c_str());
        ImGui::Separator();
        ImGui::Text("Outputs input times parameter.");
        ImGui::NewLine();
        ImGui::Text("Parameters");
        ImGui::Separator();
        gainStr.resize(13);
        gainStr = std::to_string(gain);
        ImGui::InputText("gain", gainStr.data(), 12, ImGuiInputTextFlags_CharsDecimal);
        gain = std::stod(gainStr); // exception atiyo
    };
private:
    double gain = 1.0;
    std::string gainStr = "1.0";
};

#endif /* GAINNODE_HPP */