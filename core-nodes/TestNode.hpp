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
    void DrawProperties() override;

    void SaveProperties(pugi::xml_node& xmlNode) override;
    void LoadProperties(pugi::xml_node& xmlNode) override;
private:
    double parameter1 = 1.0;
    double parameter2 = 1.0;
};

#endif /* TESTNODE_HPP */