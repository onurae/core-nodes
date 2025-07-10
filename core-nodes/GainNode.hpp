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
    explicit GainNode(const std::string& uniqueName) : CoreNode(uniqueName, "Gain", NodeType::Generic, ImColor(0.2f, 0.3f, 0.6f, 0.0f)) {};
    ~GainNode() override = default;

    void Build() override;
    void DrawProperties(const std::vector<CoreNode*>& coreNodeVec) override;

    void SaveProperties(pugi::xml_node& xmlNode) override;
    void LoadProperties(const pugi::xml_node& xmlNode) override;
private:
    NodeParamDouble gain{"gain", 1.0};
};

#endif /* GAINNODE_HPP */