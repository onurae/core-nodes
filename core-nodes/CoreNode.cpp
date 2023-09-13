/******************************************************************************************
*                                                                                         *
*    Core Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreNode.hpp"

CoreNode::CoreNode(int id, const std::string& name, const std::string& libName, NodeType type, ImColor colorNode) : id(id), name(name), libName(libName), type(type), colorNode(colorNode)
{
    flagSet.SetFlag(NodeFlag::Default);
    rectName.Min = ImVec2(0.0f, 0.0f);
    rectName.Max = ImGui::CalcTextSize(name.c_str());
    titleHeight = kTitleHeight * rectName.GetHeight();

    colorHead.Value.x = colorNode.Value.x * 0.5f;
    colorHead.Value.y = colorNode.Value.y * 0.5f;
    colorHead.Value.z = colorNode.Value.z * 0.5f;
    colorHead.Value.w = 1.0f;
    colorLine = ImColor(0.416f, 0.016f, 0.059f, 0.75f);
    colorBody = ImColor(0.0f, 0.0f, 0.0f, 0.75f);
}

void CoreNode::Save(pugi::xml_node& xmlNode) const
{
    auto node = xmlNode.append_child("node");
    node.append_attribute("name") = name.c_str();
    SaveInt(node, "id", id);
    SaveString(node, "libName", libName);
    SaveInt(node, "type", (int)type);
    SaveImColor(node, "colorNode", colorNode);
    SaveImColor(node, "colorHead", colorHead);
    SaveImColor(node, "colorLine", colorLine);
    SaveImColor(node, "colorBody", colorBody);
    SaveInt(node, "flagSet", flagSet.GetInt());
    SaveImRect(node, "rectNode", rectNode);
    SaveImRect(node, "rectNodeTitle", rectNodeTitle);
    SaveImRect(node, "rectName", rectName);
    SaveFloat(node, "titleHeight", titleHeight);
    SaveFloat(node, "bodyHeight", bodyHeight);
    auto inputList = node.append_child("inputList");
    for (const auto& element : inputVec)
    {
        element.Save(inputList);
    }
    auto outputList = node.append_child("outputList");
    for (const auto& element : outputVec)
    {
        element.Save(outputList);
    }
    SaveImVec2(node, "leftPortPos", leftPortPos);
    SaveImVec2(node, "rightPortPos", rightPortPos);
    SaveBool(node, "portInverted", portInverted);
}

void CoreNode::Load(const pugi::xml_node& xmlNode)
{
    id = LoadInt(xmlNode, "id");
    name = xmlNode.attribute("name").as_string();
    libName = LoadString(xmlNode, "libName");
    type = (NodeType)LoadInt(xmlNode, "type");
    colorNode = LoadImColor(xmlNode, "colorNode");
    colorHead = LoadImColor(xmlNode, "colorHead");
    colorLine = LoadImColor(xmlNode, "colorLine");
    colorBody = LoadImColor(xmlNode, "colorBody");
    flagSet.SetInt(LoadInt(xmlNode, "flagSet"));
    flagSet.UnsetFlag(NodeFlag::Highlighted);   // Unset highlighted flag.
    flagSet.UnsetFlag(NodeFlag::Hovered);       // Unset hovered flag.
    rectNode = LoadImRect(xmlNode, "rectNode");
    rectNodeTitle = LoadImRect(xmlNode, "rectNodeTitle");
    rectName = LoadImRect(xmlNode, "rectName");
    titleHeight = LoadFloat(xmlNode, "titleHeight");
    bodyHeight = LoadFloat(xmlNode, "bodyHeight");
    for (const auto& element : xmlNode.child("inputList").children("input"))
    {
        inputVec.emplace_back();
        inputVec.back().Load(element);
    }
    for (const auto& element : xmlNode.child("outputList").children("output"))
    {
        outputVec.emplace_back();
        outputVec.back().Load(element);
    }
    leftPortPos = LoadImVec2(xmlNode, "leftPortPos");
    rightPortPos = LoadImVec2(xmlNode, "rightPortPos");
    portInverted = LoadBool(xmlNode, "portInverted");
}

void CoreNode::Translate(ImVec2 delta, bool selectedOnly)
{
    if (selectedOnly && (flagSet.HasAnyFlag(NodeFlag::Selected) == false))
    {
        return; // Do not translate unselected node in the node vector.
    }
    rectNode.Translate(delta);
    rectNodeTitle.Translate(delta);
    rectName.Translate(delta);
    for (auto& input : inputVec)
    {
        input.Translate(delta);
    }
    for (auto& output : outputVec)
    {
        output.Translate(delta);
    }
}

void CoreNode::InvertPort()
{
    float delta;
    if (portInverted == false)
    {
        delta = rightPortPos.x - leftPortPos.x;
        portInverted = true;
    }
    else
    {
        delta = leftPortPos.x - rightPortPos.x;
        portInverted = false;
    }

    for (auto& input : inputVec)
    {
        input.Invert();
        input.Translate(ImVec2(delta, 0.0f));
    }
    for (auto& output : outputVec)
    {
        output.Invert();
        output.Translate(ImVec2(-delta, 0.0f));
    }
}

void CoreNode::BuildGeometry(float inputsWidth, float inputsHeight, float outputsWidth, float outputsHeight)
{
    bodyHeight = ImMax(inputsHeight, outputsHeight) + kVerticalTop * rectName.GetHeight() + kVerticalBottom * rectName.GetHeight();
    rectNode.Min = ImVec2(0.0f, 0.0f);
    rectNode.Max.x = ImMax(inputsWidth + outputsWidth + kHorizontal * rectName.GetHeight(), rectName.GetWidth() + kHorizontal * rectName.GetHeight());
    rectNode.Max.y = titleHeight + bodyHeight;
    rectName.Translate(ImVec2((rectNode.GetWidth() - rectName.GetWidth()) * 0.5f, ((titleHeight - rectName.GetHeight()) * 0.5f)));

    leftPortPos = ImVec2(rectNode.GetTL().x, rectNode.GetTL().y + titleHeight + kVerticalTop * rectName.GetHeight());
    rightPortPos = ImVec2(rectNode.GetTR().x, rectNode.GetTR().y + titleHeight + kVerticalTop * rectName.GetHeight());
    for (auto& input : inputVec)
    {
        auto pinTopCenter = ImVec2(input.GetRectPin().GetCenter().x, input.GetRectPin().GetCenter().y - input.GetRectPin().GetHeight() * 0.5f);
        input.Translate(leftPortPos - pinTopCenter);
        leftPortPos.y += input.GetRectPort().GetHeight();
    }
    for (auto& output : outputVec)
    {
        auto pinTopCenter = ImVec2(output.GetRectPin().GetCenter().x, output.GetRectPin().GetCenter().y - output.GetRectPin().GetHeight() * 0.5f);
        output.Translate(rightPortPos - pinTopCenter);
        rightPortPos.y += output.GetRectPort().GetHeight();
    }

    rectNodeTitle.Min = ImVec2(0.0f, 0.0f);
    rectNodeTitle.Max.x = rectNode.Max.x;
    rectNodeTitle.Max.y = titleHeight;
}

void CoreNode::Draw(ImDrawList* drawList, ImVec2 offset, float scale) const
{
    if (flagSet.HasAnyFlag(NodeFlag::Visible) == false)
    {
        return;
    }

    ImRect rect = rectNode;
    rect.Min *= scale;
    rect.Max *= scale;
    rect.Translate(offset);
    const float rounding = titleHeight * scale * 0.3f;
    const ImDrawFlags roundCornersFlags = ImDrawFlags_RoundCornersAll;

    if (flagSet.HasAnyFlag(NodeFlag::Hovered)) { /* TODO */ }
    if (flagSet.HasAnyFlag(NodeFlag::Disabled)) { /* TODO */ }

    // Body
    drawList->AddRectFilled(rect.Min, rect.Max, colorBody, rounding, roundCornersFlags);

    // Head
    const ImVec2 headBottomRight = rect.GetTR() + ImVec2(0.0f, titleHeight * scale);
    const ImDrawFlags headRoundCornersFlags = flagSet.HasAnyFlag(NodeFlag::Collapsed) ? roundCornersFlags : (ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight);
    drawList->AddRectFilled(rect.Min, headBottomRight, colorHead, rounding, headRoundCornersFlags);

    // Line
    if (flagSet.HasAnyFlag(NodeFlag::Collapsed) == false)
    {
        drawList->AddLine(ImVec2(rect.Min.x, headBottomRight.y), ImVec2(headBottomRight.x - 1.0f, headBottomRight.y), colorLine, 2.0f);
    }

    // Name and Shadow
    ImGui::SetCursorScreenPos(((rectName.Min + ImVec2(2, 2)) * scale) + offset);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    ImGui::Text(name.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorScreenPos((rectName.Min * scale) + offset);
    ImGui::Text(name.c_str());

    // Selection
    if (flagSet.HasAnyFlag(NodeFlag::Selected))
    {
        drawList->AddRectFilled(rect.Min, rect.Max, ImColor(1.0f, 1.0f, 1.0f, 0.25f), rounding, roundCornersFlags);
    }

    // Outline
    ImColor outlineColor = colorNode;
    float outlineThickness = 2.0f * scale;
    if (flagSet.HasAnyFlag(NodeFlag::Highlighted))
    {
        outlineColor.Value.x *= 1.5f;
        outlineColor.Value.y *= 1.5f;
        outlineColor.Value.z *= 1.5f;
        outlineColor.Value.w = 1.0f;
        outlineThickness *= 1.2f;
    }
    else
    {
        outlineColor.Value.x *= 0.8f;
        outlineColor.Value.y *= 0.8f;
        outlineColor.Value.z *= 0.8f;
        outlineColor.Value.w = 1.0f;
    }
    drawList->AddRect(rect.Min, rect.Max, outlineColor, rounding, roundCornersFlags, outlineThickness);

    // Ports
    if (flagSet.HasAnyFlag(NodeFlag::Collapsed) == false)
    {
        for (const auto& input : inputVec)
            input.Draw(drawList, offset, scale);

        for (const auto& output : outputVec)
            output.Draw(drawList, offset, scale);
    }
    else
    {
        // No pin when collapsed.
    }
}