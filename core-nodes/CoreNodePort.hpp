/******************************************************************************************
*                                                                                         *
*    Core Node Port                                                                       *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef CORENODEPORT_HPP
#define CORENODEPORT_HPP

#define IMGUI_DEFINE_MATH_OPERATORS
#include <bitset>
#include "CoreNodeLib.hpp"

class FlagSet
{
private:
    unsigned int flags = 0;
public:
    void SetFlag(unsigned int flag) { flags |= flag; }
    void UnsetFlag(unsigned int flag) { flags &= ~flag; }
    void FlipFlag(unsigned int flag) { flags ^= flag; }
    bool HasFlag(unsigned int flag) const { return (flags & flag) == flag; }
    bool HasAnyFlag(unsigned int multiFlag) const { return (flags & multiFlag) != 0; }
    bool Equal(unsigned int multiFlag) const { return flags == multiFlag; }
    std::string Get() const { return std::bitset<8 * sizeof(flags)>(flags).to_string(); }
    int GetInt() const { return (int)flags; }
    void SetInt(int argFlags) { flags = argFlags; }
};

struct PortFlag
{
    PortFlag() = delete;
    static const unsigned int Default = 0;
    //static const unsigned int Visible = 1 << 0; // not used.
    static const unsigned int Hovered = 1 << 1;
    static const unsigned int Connectible = 1 << 2;
    static const unsigned int Draging = 1 << 3;
};

class CoreNode;
class CoreNodeOutput;
class CoreNodeInput
{
private:
    ImVec2 position;
    float ref;
    ImRect rectName;
    ImRect rectPin;
    ImRect rectPort;
    const float kPin{ 0.6f };
    const float kPadding{ 0.25f };
    const float kHeight{ 1.25f };
    std::string name;
    PortType type;
    PortDataType dataType;
    int order;
    FlagSet flagSet;
    CoreNode* targetNode{ nullptr };
    CoreNodeOutput* targetNodeOutput{ nullptr };
    bool inverted = false;
    int linkDir = 0;        // direction of the input on the y axis {-1, 0, 1}. +1:downwards.
    int linkSepX = 0;       // seperation coeff. for the horizontal axis.
    int linkSepY = 0;       // seperation coeff. for the vertical axis (between nodes).
    int targetLinkDir = 0;  // direction of the linked output.
    int targetLinkSep = 0;  // seperation coeff. of the linked output.
public:
    CoreNodeInput() = default;
    CoreNodeInput(const std::string& name, PortType type, PortDataType dataType, int order);
    virtual ~CoreNodeInput() = default;
    void Save(pugi::xml_node& xmlNode) const;
    void Load(const pugi::xml_node& xmlNode);

    std::string GetName() const { return name; }
    PortType GetType() const { return type; };
    PortDataType GetDataType() const { return dataType; };
    int GetOrder() const { return order; }
    ImVec2 GetPosition() const { return position; }
    ImRect GetRectPin() const { return rectPin; }
    ImRect GetRectPort() const { return rectPort; }
    FlagSet& GetFlagSet() { return flagSet; }
    CoreNode* GetTargetNode() const { return targetNode; }
    void SetTargetNode(CoreNode* node) { targetNode = node; }
    CoreNodeOutput* GetTargetNodeOutput() const { return targetNodeOutput; }
    void SetTargetNodeOutput(CoreNodeOutput* nodeOutput) { targetNodeOutput = nodeOutput; }
    void BreakLink();
    void SetLinkDir(int dir) { linkDir = dir; }
    int GetLinkDir() const { return linkDir; }
    void SetLinkSepX(int sep) { linkSepX = sep; }
    int GetLinkSepX() const { return linkSepX; }
    void SetLinkSepY(int sep) { linkSepY = sep; }
    int GetLinkSepY() const { return linkSepY; }
    void SetTargetLinkDir(int dir) { targetLinkDir = dir; }
    int GetTargetLinkDir() const { return targetLinkDir; }
    void SetTargetLinkSep(int sep) { targetLinkSep = sep; }
    int GetTargetLinkSep() const { return targetLinkSep; }
    void Translate(ImVec2 delta);
    void Draw(ImDrawList* drawList, ImVec2 offset, float scale) const;
    void Invert();
};

class CoreNodeOutput
{
private:
    ImVec2 position;
    float ref;
    ImRect rectName;
    ImRect rectPin;
    ImRect rectPort;
    const float kPin{ 0.6f };
    const float kPadding{ 0.25f };
    const float kHeight{ 1.25f };
    std::string name;
    PortType type;
    PortDataType dataType;
    int order;
    FlagSet flagSet;
    int linkNum{ 0 };
    bool inverted = false;
public:
    CoreNodeOutput() = default;
    CoreNodeOutput(const std::string& name, PortType type, PortDataType dataType, int order);
    virtual ~CoreNodeOutput() = default;
    void Save(pugi::xml_node& xmlNode) const;
    void Load(const pugi::xml_node& xmlNode);

    std::string GetName() const { return name; }
    PortType GetType() const { return type; };
    PortDataType GetDataType() const { return dataType; };
    int GetOrder() const { return order; }
    ImVec2 GetPosition() const { return position; }
    ImRect GetRectPin() const { return rectPin; }
    ImRect GetRectPort() const { return rectPort; }
    FlagSet& GetFlagSet() { return flagSet; }
    int GetLinkNum() const { return linkNum; }
    void IncreaseLinkNum() { linkNum += 1; }
    void DecreaseLinkNum() { linkNum > 0 ? linkNum -= 1 : linkNum = 0; }
    void Translate(ImVec2 delta);
    void Draw(ImDrawList* drawList, ImVec2 offset, float scale) const;
    void Invert();
};

#endif /* CORENODEPORT_HPP */