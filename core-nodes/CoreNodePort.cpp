/******************************************************************************************
*                                                                                         *
*    Core Node Port                                                                       *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreNodePort.hpp"

CoreNodeInput::CoreNodeInput(const std::string& name, PortType type, PortDataType dataType, int order) :
    name(name), type(type), dataType(dataType), order(order)
{
    flagSet.SetFlag(PortFlag::Default);

    rectName.Min = ImVec2(0.0f, 0.0f);
    rectName.Max = ImGui::CalcTextSize(name.c_str());
    ref = rectName.GetHeight();
    rectPin = ImRect(ImVec2(0.0f, 0.0f), ImVec2((kPadding + kPin + kPadding) * ref, kHeight * ref));
    rectName.Translate(ImVec2(rectPin.GetWidth(), (rectPin.GetHeight() - rectName.GetHeight()) * 0.5f));
    rectPort.Min = ImVec2(0.0f, 0.0f);
    rectPort.Max = ImVec2(rectPin.GetWidth() + rectName.GetWidth() + kPadding * ref, rectPin.GetHeight());
    ImVec2 offset = ImVec2(0.0f, 0.0f) - rectPin.GetCenter();
    rectPin.Translate(offset);
    rectName.Translate(offset);
    rectPort.Translate(offset);
}

void CoreNodeInput::Save(pugi::xml_node& xmlNode) const
{
    auto node = xmlNode.append_child("input");
    node.append_attribute("name").set_value(name.c_str());
    SaveImVec2(node, "position", position);
    SaveFloat(node, "ref", ref);
    SaveImRect(node, "rectName", rectName);
    SaveImRect(node, "rectPin", rectPin);
    SaveImRect(node, "rectPort", rectPort);
    SaveFloat(node, "kPin", kPin);
    SaveFloat(node, "kPadding", kPadding);
    SaveFloat(node, "kHeight", kHeight);
    SaveInt(node, "type", (int)type);
    SaveInt(node, "dataType", (int)dataType);
    SaveInt(node, "order", order);
    //SaveInt(node, "flagSet", flagSet.GetInt());

    /*
    ImVec2 position;
    float ref;
    ImRect rectName;
    ImRect rectPin;
    ImRect rectPort;
    float kPin{ 0.6f };
    float kPadding{ 0.25f };
    float kHeight{ 1.25f };
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
    */
}

void CoreNodeInput::BreakLink()
{
    if (targetNodeOutput != nullptr)
    {
        targetNodeOutput->DecreaseLinkNum();
    }
    targetNode = nullptr;
    targetNodeOutput = nullptr;
    linkDir = 0;
    linkSepX = 0;
    linkSepY = 0;
    targetLinkDir = 0;
    targetLinkSep = 0;
}

void CoreNodeInput::Translate(ImVec2 delta)
{
    position += delta;
    rectPort.Translate(delta);
    rectName.Translate(delta);
    rectPin.Translate(delta);
}

void CoreNodeInput::Invert()
{
    float deltaPort = rectName.GetWidth() + kPadding * ref;
    float deltaName = rectName.GetWidth() + rectPin.GetWidth();
    if (inverted == false)
    {
        rectPort.Translate(ImVec2(-deltaPort, 0.0f));
        rectName.Translate(ImVec2(-deltaName, 0.0f));
        inverted = true;
    }
    else
    {
        rectPort.Translate(ImVec2(deltaPort, 0.0f));
        rectName.Translate(ImVec2(deltaName, 0.0f));
        inverted = false;
    }
}

void CoreNodeInput::Draw(ImDrawList* drawList, ImVec2 offset, float scale) const
{
    if (type == PortType::None)
    {
        return;
    }

    ImGui::SetCursorScreenPos((rectName.Min * scale) + offset);
    ImGui::Text(name.c_str());

    // Show type if hovered.
    if (flagSet.Equal(PortFlag::Hovered) && targetNode == nullptr)
    {
        std::string typeName = portTypeNames.at(static_cast<int>(dataType));
        ImVec2 typeNameSize = ImGui::CalcTextSize(typeName.c_str());
        auto distText = ImVec2(10.0f, 0.0f);
        if (inverted == true)
        {
            auto point = ImVec2(rectName.Min.x + rectName.GetWidth() + rectPin.GetWidth(), rectName.Min.y) + distText;
            ImGui::SetCursorScreenPos((point * scale) + offset);
        }
        else
        {
            auto point = ImVec2(rectName.Min.x - rectPin.GetWidth(), rectName.Min.y) - distText;
            ImGui::SetCursorScreenPos((point * scale - ImVec2(typeNameSize.x, 0.0f)) + offset);
        }
        auto typeColor = ImColor(0.996f, 0.431f, 0.000f, 1.0f); // data type text color
        ImGui::TextColored(typeColor, typeName.c_str());
    }

    auto center = ImVec2((position * scale) + offset);
    auto radius1 = kPin * rectName.GetHeight() * 0.5f * scale;
    auto radius2 = kPin * rectName.GetHeight() * 0.5f * scale * 0.5f;
    auto radius3 = kPin * rectName.GetHeight() * 0.5f * scale * 0.8f;
    auto thickness1 = 0.1f * rectName.GetHeight() * scale;

    auto color1 = ImColor(0.000f, 0.459f, 0.388f, 1.0f); // background
    auto color2 = ImColor(0.341f, 0.659f, 0.769f, 1.0f); // teal ring
    auto color3 = ImColor(1.000f, 1.000f, 1.000f, 1.0f); // white
    auto color4 = ImColor(0.996f, 0.431f, 0.000f, 1.0f); // orange
    auto color5 = ImColor(1.000f, 0.824f, 0.000f, 1.0f); // orange for ic

    drawList->AddCircleFilled(center, radius1, color1, 0); // Background
    drawList->AddCircle(center, radius1, color2, 0, thickness1); // Outer ring

    float distTri = radius1 * 1.6f;
    float triLength = radius1;
    if (flagSet.Equal(PortFlag::Hovered) && type == PortType::In && targetNode == nullptr)
    {
        if (inverted == false)
        {
            auto loc = ImVec2(center.x - distTri, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(-triLength, triLength), loc + ImVec2(-triLength, -triLength), loc, color4);
        }
        else
        {
            auto loc = ImVec2(center.x + distTri, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(triLength, triLength), loc + ImVec2(triLength, -triLength), loc, color4);
        }
    }
    else if (flagSet.Equal(PortFlag::Hovered) && type == PortType::Ic && targetNode == nullptr)
    {
        if (inverted == false)
        {
            auto loc = ImVec2(center.x - distTri, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(-triLength, triLength), loc + ImVec2(-triLength, -triLength), loc, color5);
        }
        else
        {
            auto loc = ImVec2(center.x + distTri, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(triLength, triLength), loc + ImVec2(triLength, -triLength), loc, color5);
        }
    }

    if (targetNode == nullptr)
    {
        if (flagSet.Equal(PortFlag::Hovered))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
        if (flagSet.Equal(PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
        if (flagSet.Equal(PortFlag::Draging))
        {
            drawList->AddCircleFilled(center, radius3, color4);
        }
        if (flagSet.Equal(PortFlag::Hovered | PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius3, color4);
        }
    }
    else
    {
        drawList->AddCircleFilled(center, radius2, color3); // Inner circle
        if (flagSet.Equal(PortFlag::Hovered))
        {
            // drawList->AddRect(rectPort.Min * scale + offset, rectPort.Max * scale + offset, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
        }
        if (flagSet.Equal(PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
    }
}

CoreNodeOutput::CoreNodeOutput(const std::string& name, PortType type, PortDataType dataType, int order) :
    name(name), type(type), dataType(dataType), order(order)
{
    flagSet.SetFlag(PortFlag::Default);

    rectName.Min = ImVec2(0.0f, 0.0f) - ImGui::CalcTextSize(name.c_str());
    rectName.Max = ImVec2(0.0f, 0.0f);
    ref = rectName.GetHeight();
    rectPin = ImRect(ImVec2((kPadding + kPin + kPadding) * ref * -1.0f, kHeight * ref * -1.0f), ImVec2(0.0f, 0.0f));
    rectName.Translate(ImVec2(-rectPin.GetWidth(), (rectPin.GetHeight() - rectName.GetHeight()) * 0.5f * -1.0f));
    rectPort.Min = ImVec2(-rectPin.GetWidth() + -rectName.GetWidth() - kPadding * ref, -rectPin.GetHeight());
    rectPort.Max = ImVec2(0.0f, 0.0f);
    ImVec2 offset = ImVec2(0.0f, 0.0f) - rectPin.GetCenter();
    rectPin.Translate(offset);
    rectPort.Translate(offset);
    rectName.Translate(offset);
}

void CoreNodeOutput::Save(pugi::xml_node & xmlNode) const
{
    /*    ImVec2 position;
    float ref;
    ImRect rectName;
    ImRect rectPin;
    ImRect rectPort;
    float kPin{ 0.6f };
    float kPadding{ 0.25f };
    float kHeight{ 1.25f };
    std::string name;
    PortType type;
    PortDataType dataType;
    int order;
    FlagSet flagSet;
    int linkNum{ 0 };
    bool inverted = false;*/
}

void CoreNodeOutput::Translate(ImVec2 delta)
{
    position += delta;
    rectPort.Translate(delta);
    rectName.Translate(delta);
    rectPin.Translate(delta);
}

void CoreNodeOutput::Invert()
{
    float deltaPort = rectName.GetWidth() + kPadding * ref;
    float deltaName = rectName.GetWidth() + rectPin.GetWidth();
    if (inverted == false)
    {
        rectPort.Translate(ImVec2(deltaPort, 0.0f));
        rectName.Translate(ImVec2(deltaName, 0.0f));
        inverted = true;
    }
    else
    {
        rectPort.Translate(ImVec2(-deltaPort, 0.0f));
        rectName.Translate(ImVec2(-deltaName, 0.0f));
        inverted = false;
    }
}

void CoreNodeOutput::Draw(ImDrawList* drawList, ImVec2 offset, float scale) const
{
    if (type == PortType::None)
    {
        return;
    }

    ImGui::SetCursorScreenPos((rectName.Min * scale) + offset);
    ImGui::Text(name.c_str());

    // Show type if hovered.
    if (flagSet.Equal(PortFlag::Hovered) && linkNum == 0)
    {
        std::string typeName = portTypeNames.at(static_cast<int>(dataType));
        ImVec2 typeNameSize = ImGui::CalcTextSize(typeName.c_str());
        auto distText = ImVec2(10.0f, 0.0f);
        if (inverted == false)
        {
            auto point = ImVec2(rectName.Min.x + rectName.GetWidth() + rectPin.GetWidth(), rectName.Min.y) + distText;
            ImGui::SetCursorScreenPos((point * scale) + offset);
        }
        else
        {
            auto point = ImVec2(rectName.Min.x - rectPin.GetWidth(), rectName.Min.y) - distText;
            ImGui::SetCursorScreenPos((point * scale - ImVec2(typeNameSize.x, 0.0f)) + offset);
        }
        auto typeColor = ImColor(0.996f, 0.431f, 0.000f, 1.0f); // data type text color
        ImGui::TextColored(typeColor, typeName.c_str());
    }

    auto center = ImVec2((position * scale) + offset);
    auto radius1 = kPin * rectName.GetHeight() * 0.5f * scale;
    auto radius2 = kPin * rectName.GetHeight() * 0.5f * scale * 0.5f;
    auto radius3 = kPin * rectName.GetHeight() * 0.5f * scale * 0.8f;

    auto thickness1 = 0.1f * rectName.GetHeight() * scale;

    auto color1 = ImColor(0.000f, 0.459f, 0.388f, 1.0f); // background
    auto color2 = ImColor(0.341f, 0.659f, 0.769f, 1.0f); // teal ring
    auto color3 = ImColor(1.000f, 1.000f, 1.000f, 1.0f); // white
    auto color4 = ImColor(0.996f, 0.431f, 0.000f, 1.0f); // orange

    drawList->AddCircleFilled(center, radius1, color1, 0); // Background
    drawList->AddCircle(center, radius1, color2, 0, thickness1); // Outer ring

    float distTri = radius1 * 1.6f;
    float triLength = radius1;
    if (flagSet.Equal(PortFlag::Hovered) && type == PortType::Out && linkNum == 0)
    {
        if (inverted == false)
        {
            auto loc = ImVec2(center.x + distTri + radius1, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(-triLength, triLength), loc + ImVec2(-triLength, -triLength), loc, color4);
        }
        else
        {
            auto loc = ImVec2(center.x - distTri - radius1, center.y);
            drawList->AddTriangleFilled(loc + ImVec2(triLength, triLength), loc + ImVec2(triLength, -triLength), loc, color4);
        }
    }

    if (linkNum == 0)
    {
        if (flagSet.Equal(PortFlag::Hovered))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
        if (flagSet.Equal(PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
        if (flagSet.Equal(PortFlag::Draging))
        {
            drawList->AddCircleFilled(center, radius3, color4);
        }
        if (flagSet.Equal(PortFlag::Hovered | PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius3, color4);
        }
    }
    else
    {
        drawList->AddCircleFilled(center, radius2, color3); // Inner circle
        if (flagSet.Equal(PortFlag::Hovered))
        {
            // drawList->AddRect(rectPort.Min * scale + offset, rectPort.Max * scale + offset, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
            drawList->AddCircleFilled(center, radius2, color4);
        }
        if (flagSet.Equal(PortFlag::Draging))
        {
            drawList->AddCircleFilled(center, radius3, color4);
        }
        if (flagSet.Equal(PortFlag::Connectible))
        {
            drawList->AddCircleFilled(center, radius2, color4);
        }
    }
}