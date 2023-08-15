/******************************************************************************************
*                                                                                         *
*    Core Diagram                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef COREDIAGRAM_HPP
#define COREDIAGRAM_HPP

#include "CoreNode.hpp"
#include <set>
#include <cmath>

enum class State
{
    None = 0,
    Default,
    HoveringNode,
    HoveringInput,
    HoveringOutput,
    Draging,
    DragingInput,
    DragingOutput,
    Selecting
};

class CoreDiagram
{
public:
    CoreDiagram() = default;
    virtual ~CoreDiagram();
    void Update();
    void Save(pugi::xml_node& xmlNode) const;
    void Load(const pugi::xml_node& xmlNode);

private:
    CoreNodeLib coreNodeLib;
    State state = State::Default;
    float scale = 1.0f;
    ImVec2 position;
    ImVec2 size;
    ImVec2 scroll;
    ImVec2 mousePos;
    std::vector<CoreNode*> coreNodeVec;
    CoreNode* highlightedNode = nullptr;
    void HighlightNode();
    std::string CreateUniqueName(const std::string& libName) const;
    CoreNode* CreateCoreNode(const CoreNodeLib::Node* node, ImVec2 pos);

    // Update canvas
    ImRect rectCanvas;
    void UpdateCanvasRect();
    void UpdateCanvasScrollZoom();
    void UpdateCanvasGrid(ImDrawList* drawList) const;
    
    // Actions
    void Actions();
    void MouseMove();
    void MouseLeftButtonDoubleClick();
    void MouseRightButtonDoubleClick();
    void MouseLeftButtonSingleClick();
    void MouseRightButtonSingleClick();
    void MouseLeftButtonDrag();
    void MouseLeftButtonRelease();
    void MouseRightButtonRelease();
    void KeyboardPressDelete();

    // Draw canvas elements
    void DrawCanvasElements();
    void DrawNodes() const;
    ImRect rectSelecting;
    void DrawSelecting() const;
    void PrintInfo();

    // Interaction
    CoreNode* hovNode = nullptr;            // Hovered node
    CoreNode* iNode = nullptr;              // Node under interaction
    CoreNodeInput* iNodeInput = nullptr;    // Node input under interaction
    CoreNodeOutput* iNodeOutput = nullptr;  // Node output under interaction
    void UpdateNodeFlags();
    void UpdateInputFlags(CoreNode* node);
    void UpdateOutputFlags(CoreNode* node);
    bool ConnectionRules([[maybe_unused]] const CoreNode* inputNode, const CoreNode* outputNode, const CoreNodeInput* input, const CoreNodeOutput* output) const;
    void SortNodeOrder();
    void Popups();

    // Network
    enum class LinkType;
    struct Link
    {
        CoreNode* inputNode;
        CoreNode* outputNode;
        CoreNodeInput* inputPort;
        CoreNodeOutput* outputPort;
        LinkType type;
        ImColor color;
        float thickness;
        float xSepIn;
        float xSepOut;
        float ykSep = 0;
        void Save(pugi::xml_node& xmlNode) const
        {
            auto link = xmlNode.append_child("link");
            SaveString(link, "inputNode", inputNode->GetName());
            SaveString(link, "outputNode", outputNode->GetName());
            SaveInt(link, "inputPort", inputPort->GetOrder());
            SaveInt(link, "outputPort", outputPort->GetOrder());
            SaveInt(link, "type", (int)type);
            SaveImColor(link, "color", color);
            SaveFloat(link, "thickness", thickness);
            SaveFloat(link, "xSepIn", xSepIn);
            SaveFloat(link, "xSepOut", xSepOut);
            SaveFloat(link, "ykSep", ykSep);
        }
        void Load(const pugi::xml_node& xmlNode)
        {
            type = (LinkType)LoadInt(xmlNode, "type");
            color = LoadImColor(xmlNode, "color");
            thickness = LoadFloat(xmlNode, "thickness");
            xSepIn = LoadFloat(xmlNode, "xSepIn");
            xSepOut = LoadFloat(xmlNode, "xSepOut");
            ykSep = LoadFloat(xmlNode, "ykSep");
        }
    };
    std::vector<Link> linkVec;
    void EraseLink(const CoreNodeInput* input);
    bool IsLinkVisible(ImVec2 pInput, ImVec2 pOutput) const;
    void DrawLinks() const;
    ImVec2 inputFreeLink;
    ImVec2 outputFreeLink;
    void SetLinkProperties();
    void DrawFreeLink() const;
    void DrawLinkBezier(const Link& link, ImVec2 pInput, ImVec2 pOutput, float rounding, bool invert = false) const;
    void DrawLinkIOInv(const Link& link, ImVec2 pInput, ImVec2 pOutput, float dHandle) const;
    void DrawLinkBNInv(const Link& link, ImVec2 pInput, ImVec2 pOutput, float dHandle, bool invert = false) const;
    void SetInputSepUp(Link& link) const;
    void SetInputSepDown(Link& link) const;
    void SetOutputSepUp(Link& link) const;
    void SetOutputSepDown(Link& link) const;
    void SetNodeSep(Link& link) const;
    enum class LinkType
    {
        NONE,
        // BINV: both nodes inverted.
        BINV_LEFT,
        BINV_RIGHT_OVER,
        BINV_RIGHT_UNDER,
        BINV_RIGHT_MID,
        // IINV: only input node inverted.
        IINV_RIGHT_OVER,
        IINV_LEFT_OVER,
        IINV_RIGHT_UNDER,
        IINV_LEFT_UNDER,
        IINV_MID,
        // OINV: only output node inverted.
        OINV_RIGHT_OVER,
        OINV_LEFT_OVER,
        OINV_RIGHT_UNDER,
        OINV_LEFT_UNDER,
        OINV_MID,
        // NINV: No Inversion. Location of input node wrt output node.
        NINV_RIGHT,
        NINV_LEFT_OVER,
        NINV_LEFT_UNDER,
        NINV_LEFT_MID
    };
};

#endif /* COREDIAGRAM_HPP */