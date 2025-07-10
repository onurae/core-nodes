/******************************************************************************************
*                                                                                         *
*    Test Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef TESTNODE_HPP
#define TESTNODE_HPP

#include "CoreNode.hpp"

class TestNode : public CoreNode
{
public:
    explicit TestNode(const std::string& uniqueName) : CoreNode(uniqueName, "Test", NodeType::Generic, ImColor(0.2f, 0.3f, 0.6f, 0.0f)) {};
    ~TestNode() override = default;

    void Build() override;
    void DrawProperties(const std::vector<CoreNode*>& coreNodeVec) override;

    void SaveProperties(pugi::xml_node& xmlNode) override;
    void LoadProperties(const pugi::xml_node& xmlNode) override;
private:
    NodeParamDouble parameter1{ "parameter1", 1.234 };
    NodeParamDouble parameter2{ "parameter2", 1.234 };
};

#endif /* TESTNODE_HPP */