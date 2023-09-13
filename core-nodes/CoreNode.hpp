/******************************************************************************************
*                                                                                         *
*    Core Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef CORENODE_HPP
#define CORENODE_HPP

#include "CoreNodePort.hpp"

struct NodeFlag
{
    NodeFlag() = delete;
    static const unsigned int Default = 0;
    static const unsigned int Visible = 1 << 0;
    static const unsigned int Hovered = 1 << 1;
    static const unsigned int Selected = 1 << 2;
    static const unsigned int Collapsed = 1 << 3;
    static const unsigned int Disabled = 1 << 4;
    static const unsigned int Highlighted = 1 << 5;
};

class CoreNode
{
private:
    int id;
    std::string name;
    std::string libName; // TODO canvasta node kopyalarken ise yarar.
    NodeType type;
    ImColor colorNode;
    ImColor colorHead;
    ImColor colorLine;
    ImColor colorBody;
    FlagSet flagSet;
    ImRect rectNode;
    ImRect rectNodeTitle;
    ImRect rectName;
    float titleHeight{ 0.0f };
    float bodyHeight{ 0.0f };
    const float kTitleHeight{ 1.8f };
    const float kHorizontal{ 1.7f };
    const float kVerticalTop{ 0.50f };
    const float kVerticalBottom{ 0.50f };
    std::vector<CoreNodeInput> inputVec;
    std::vector<CoreNodeOutput> outputVec;
    ImVec2 leftPortPos;
    ImVec2 rightPortPos;
    bool portInverted = false;
public:
    CoreNode() = default;
    CoreNode(int id, const std::string& name, const std::string& libName, NodeType type, ImColor colorNode);
    virtual ~CoreNode() = default;
    void Save(pugi::xml_node& xmlNode) const;
    void Load(const pugi::xml_node& xmlNode);

    std::string GetName() const { return name; }
    NodeType GetType() const { return type; };
    FlagSet& GetFlagSet() { return flagSet; }
    const FlagSet& GetFlagSet() const { return flagSet; }
    ImRect GetRectNode() const { return rectNode; }
    ImRect GetRectNodeTitle() const { return rectNodeTitle; }
    void SetRectNode(const ImRect& rect) { rectNode = rect; }
    float GetBodyHeight() const { return bodyHeight; }
    std::vector<CoreNodeInput>& GetInputVec() { return inputVec; }
    const std::vector<CoreNodeInput>& GetInputVec() const { return inputVec; }
    std::vector<CoreNodeOutput>& GetOutputVec() { return outputVec; }
    void BuildGeometry(float inputsWidth, float inputsHeight, float outputsWidth, float outputsHeight);
    void Translate(ImVec2 delta, bool selectedOnly = false);
    void Draw(ImDrawList* drawList, ImVec2 offset, float scale) const;
    void InvertPort();
    bool IsPortInverted() const { return portInverted; }
};

#endif /* CORENODE_HPP */