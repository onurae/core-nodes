/******************************************************************************************
*                                                                                         *
*    Core Diagram                                                                         *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreDiagram.hpp"

CoreDiagram::~CoreDiagram()
{
    for (const auto& node : coreNodeVec)
    {
        delete node;
    }
}

void CoreDiagram::Update()
{
    UpdateCanvasRect();
    UpdateCanvasScrollZoom();
    UpdateCanvasGrid(ImGui::GetWindowDrawList());
    UpdateNodeFlags(); // Sets hovNode.
    Actions();
    DrawCanvasElements();
    PopupMenu();
}

void CoreDiagram::Save(pugi::xml_node& xmlNode) const
{
    auto node = xmlNode.append_child("diagram");
    SaveFloat(node, "scale", scale);
    SaveImVec2(node, "scroll", scroll);
    highlightedNode != nullptr ? SaveString(node, "hNode", highlightedNode->GetName()) : SaveString(node, "hNode", "");
    auto nodeList = node.append_child("nodeList");
    for (const auto& element : coreNodeVec)
    {
        element->Save(nodeList);
    }
    auto linkList = node.append_child("linkList");
    for (const auto& element : linkVec)
    {
        element.Save(linkList);
    }
}

void CoreDiagram::Load(const pugi::xml_node& xmlNode)
{
    auto node = xmlNode.child("diagram");
    state = State::Default;
    scale = LoadFloat(node, "scale");
    scroll = LoadImVec2(node, "scroll");
    highlightedNode = nullptr;
    hovNode = nullptr;
    iNode = nullptr;
    iNodeInput = nullptr;
    iNodeOutput = nullptr;
    rectCanvas = ImRect();
    rectSelecting = ImRect();
    inputFreeLink = ImVec2();
    outputFreeLink = ImVec2();
    for (const auto& element : node.child("nodeList").children("node"))
    {
        // Add node without building.
        coreNodeVec.push_back(coreLib.GetNode(LoadString(element, "libName"), LoadString(element, "name")));
        coreNodeVec.back()->Load(element);
    }
    auto hName = LoadString(node, "hNode");
    for (const auto element : coreNodeVec)
    {
        if (element->GetName() == hName)
        {
            highlightedNode = element;
        }
    }
    for (const auto& element : node.child("linkList").children("link"))
    {
        linkVec.emplace_back();
        linkVec.back().Load(element);

        // Match pointers
        auto inputNodeName = LoadString(element, "inputNode");
        auto outputNodeName = LoadString(element, "outputNode");
        auto inputPortOrder = LoadInt(element, "inputPort");
        auto outputPortOrder = LoadInt(element, "outputPort");
        for (auto coreNode : coreNodeVec)
        {
            auto coreName = coreNode->GetName();
            if (coreName == inputNodeName)
            {
                linkVec.back().inputNode = coreNode;
                linkVec.back().inputPort = &(coreNode->GetInputVec().at(inputPortOrder));
            }
            if (coreName == outputNodeName)
            {
                linkVec.back().outputNode = coreNode;
                linkVec.back().outputPort = &(coreNode->GetOutputVec().at(outputPortOrder));
            }
        }
        for (auto& link : linkVec)
        {
            link.inputPort->SetTargetNode(link.outputNode);
            link.inputPort->SetTargetNodeOutput(link.outputPort);
        }
    }
}

void CoreDiagram::DrawProperties()
{
    if (highlightedNode != nullptr)
    {
        highlightedNode->DrawProperties();
    }
}

void CoreDiagram::Actions()
{
    MouseMove();
    if (ImGui::IsMouseDoubleClicked(0))
    {
        MouseLeftButtonDoubleClick();
        return;
    }
    if (ImGui::IsMouseDoubleClicked(1))
    {
        MouseRightButtonDoubleClick();
        return;
    }
    if (ImGui::IsMouseClicked(0))
    {
        MouseLeftButtonSingleClick();   // Set dragging states.
        return;
    }
    if (ImGui::IsMouseDragging(0))
    {
        MouseLeftButtonDrag();          // Set selecting state.
        return;
    }
    if (ImGui::IsMouseReleased(0))
    {
        MouseLeftButtonRelease();
        return;
    }
    if (ImGui::IsMouseReleased(1))
    {
        MouseRightButtonRelease();
        return;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        KeyboardPressDelete();
        return;
    }
}

void CoreDiagram::MouseMove()
{
    bool condition = (state == State::None) || (state == State::Default) || (state == State::HoveringNode) ||
                     (state == State::HoveringInput) || (state == State::HoveringOutput);
    if (condition == true)
    {
        if (ImGui::IsWindowHovered() == false)
        {
            state = State::None;
            iNode = nullptr;
            iNodeInput = nullptr;
            iNodeOutput = nullptr;
        }
        else if (hovNode == nullptr)
        {
            state = State::Default;
            iNode = nullptr;
            iNodeInput = nullptr;
            iNodeOutput = nullptr;
        }
        else
        {
            iNode = hovNode;
            iNodeInput = nullptr;
            iNodeOutput = nullptr;

            for (auto& input : hovNode->GetInputVec())
            {
                if (input.GetFlagSet().HasFlag(PortFlag::Hovered))
                {
                    iNodeInput = &input;
                    state = State::HoveringInput; // SetState:HoveringInput
                    break;
                }
            }

            for (auto& output : hovNode->GetOutputVec())
            {
                if (output.GetFlagSet().HasFlag(PortFlag::Hovered))
                {
                    iNodeOutput = &output;
                    state = State::HoveringOutput; // SetState:HoveringOutput
                    break;
                }
            }

            if (iNodeInput == nullptr && iNodeOutput == nullptr)
            {
                state = State::HoveringNode; // SetState:HoveringNode
            }
        }
    }
}

void CoreDiagram::MouseLeftButtonDoubleClick()
{
    if (state == State::HoveringNode)
    {
        ImVec2 offset = position + scroll;
        ImRect titleArea = hovNode->GetRectNodeTitle();
        titleArea.Min *= scale;
        titleArea.Max *= scale;
        titleArea.Translate(offset);
        if (titleArea.Contains(mousePos))
        {
            ImRect rNode = hovNode->GetRectNode();
            if (hovNode->GetFlagSet().HasFlag(NodeFlag::Collapsed))
            {
                hovNode->GetFlagSet().UnsetFlag(NodeFlag::Collapsed);
                rNode.Max.y += hovNode->GetBodyHeight();
                hovNode->SetRectNode(rNode);
                //iNode->Translate(ImVec2(0.0f, iNode->GetBodyHeight() * -0.5f));
            }
            else
            {
                hovNode->GetFlagSet().SetFlag(NodeFlag::Collapsed);
                rNode.Max.y -= hovNode->GetBodyHeight();
                hovNode->SetRectNode(rNode);
                //iNode->Translate(ImVec2(0.0f, iNode->GetBodyHeight() * 0.5f));
            }
            modifFlag = true;
        }
    }
    else if (state == State::HoveringInput) // Break connection
    {
        if (iNodeInput->GetTargetNode())
        {
            iNodeInput->BreakLink();
            EraseLink(iNodeInput);
            modifFlag = true;
            state = State::DragingInput; // To be able to drag input after breaking.
        }
    }
    else if (state == State::HoveringOutput) // Break all connections
    {
        if (iNodeOutput->GetLinkNum() != 0)
        {
            for (const auto& node : coreNodeVec)
            {
                for (auto& input : node->GetInputVec())
                {
                    if (input.GetTargetNodeOutput() == iNodeOutput)
                    {
                        input.BreakLink();
                        EraseLink(&input);
                        modifFlag = true;
                    }
                }
            }
        }
    }
}

void CoreDiagram::MouseRightButtonDoubleClick()
{
    if (state == State::HoveringNode)
    {
        ImVec2 offset = position + scroll;
        ImRect titleArea = hovNode->GetRectNodeTitle();
        titleArea.Min *= scale;
        titleArea.Max *= scale;
        titleArea.Translate(offset);
        if (titleArea.Contains(mousePos) && hovNode->GetFlagSet().HasFlag(NodeFlag::Collapsed) == false)
        {
            hovNode->InvertPort();
            modifFlag = true;
        }
    }
    else if (state == State::HoveringNode)
    {
        //IM_ASSERT(hovered_node);
        //
        //if (hovered_node->GetFlagSet().HasFlag(NodeFlag::Disabled))
        //    hovered_node->GetFlagSet().UnsetFlag(NodeFlag::Disabled);
        //else
        //    hovered_node->GetFlagSet().SetFlag(NodeFlag::Disabled);
    }
    else if (state == State::HoveringInput)
    {
        // show connected port? one click is ok TODO.
    }
}

void CoreDiagram::MouseLeftButtonSingleClick()
{
    if (state == State::Default)
    {
        bool selected = false;
        for (const auto& node : coreNodeVec)
        {
            if (node->GetFlagSet().HasFlag(NodeFlag::Selected))
            {
                selected = true;
            }
            node->GetFlagSet().UnsetFlag(NodeFlag::Selected | NodeFlag::Hovered);
        }
        if (highlightedNode && selected == false)
        {
            highlightedNode->GetFlagSet().UnsetFlag(NodeFlag::Highlighted);
            highlightedNode = nullptr;
        }
    }
    else if (state == State::HoveringNode)
    {
        const ImGuiIO& io = ImGui::GetIO();
        if (io.KeyCtrl)
        {
            iNode->GetFlagSet().FlipFlag(NodeFlag::Selected);
        }
        if (io.KeyShift)
        {
            iNode->GetFlagSet().SetFlag(NodeFlag::Selected);
        }

        if (iNode->GetFlagSet().HasFlag(NodeFlag::Selected) == false)
        {
            HighlightNode();
        }
        else
        {
            SortNodeOrder();
        }
        state = State::Draging; // SetState:Draging
    }
    else if (state == State::HoveringInput)
    {
        if (iNodeInput->GetTargetNode() == nullptr)
        {
            state = State::DragingInput; // SetState:DragingInput
        }
        else
        {
            state = State::Draging;

            const ImGuiIO& io = ImGui::GetIO();
            if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
            {
                mNodeDrag = true;
            }
        }
    }
    else if (state == State::HoveringOutput)
    {
        state = State::DragingOutput; // SetState:DragingOutput
    }
}

void CoreDiagram::MouseRightButtonSingleClick()
{
}

void CoreDiagram::MouseLeftButtonDrag()
{
    const ImGuiIO& io = ImGui::GetIO();
    if (state == State::Default)
    {
        if (io.KeyShift == false)
        {
            for (const auto& node : coreNodeVec)
            {
                node->GetFlagSet().UnsetFlag(NodeFlag::Selected);
            }
        }
        state = State::Selecting; // SetState:Selecting
    }
    else if (state == State::Selecting)
    {
        const ImVec2 pos = mousePos - ImGui::GetMouseDragDelta(0);
        rectSelecting.Min = ImMin(pos, mousePos);
        rectSelecting.Max = ImMax(pos, mousePos);
    }
    else if (state == State::Draging)
    {
        if (iNode->GetFlagSet().HasFlag(NodeFlag::Selected) == false) // If draging node is an unselected node, drag only that node.
        {
            iNode->Translate(io.MouseDelta / scale, false);
        }
        else // If draging node is selected, drag all selected nodes.
        {
            for (const auto& node : coreNodeVec)
            {
                node->Translate(io.MouseDelta / scale, true);
            }
        }
        if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
        {
            mNodeDrag = true;
        }
    }
    else if (state == State::DragingInput)
    {
        iNodeOutput = nullptr;
        if (hovNode)
        {
            for (auto& output : hovNode->GetOutputVec())
            {
                const FlagSet& fSet = output.GetFlagSet();
                if (fSet.HasFlag(PortFlag::Hovered | PortFlag::Connectible))
                {
                    iNodeOutput = &output;
                }
            }
        }
        ImVec2 offset = position + scroll;
        ImVec2 p1 = offset + (iNodeInput->GetPosition() * scale);
        ImVec2 p4 = iNodeOutput ? (offset + (iNodeOutput->GetPosition() * scale)) : mousePos;
        inputFreeLink = ImVec2(p1.x, p1.y);
        outputFreeLink = ImVec2(p4.x, p4.y);
    }
    else if (state == State::DragingOutput)
    {
        iNodeInput = nullptr;
        if (hovNode)
        {
            for (auto& input : hovNode->GetInputVec())
            {
                const FlagSet& fSet = input.GetFlagSet();
                if (fSet.HasFlag(PortFlag::Hovered | PortFlag::Connectible))
                {
                    iNodeInput = &input;
                }
            }
        }
        ImVec2 offset = position + scroll;
        ImVec2 p1 = offset + (iNodeOutput->GetPosition() * scale);
        ImVec2 p4 = iNodeInput ? (offset + (iNodeInput->GetPosition() * scale)) : mousePos;
        inputFreeLink = ImVec2(p4.x, p4.y);
        outputFreeLink = ImVec2(p1.x, p1.y);
    }
}

void CoreDiagram::MouseLeftButtonRelease()
{
    if (state == State::None)
    {
        if (coreLib.IsLeafClicked() == true) // If a leaf clicked then released on the outside of canvas.
        {
            coreLib.SetLeafClickedFalse();
        }
    }
    else if (state == State::Default)
    {
        if (coreLib.IsLeafClicked() == true) // If a leaf clicked then released on the canvas, create the leaf.
        {
            auto leafName = coreLib.GetSelectedLeaf();
            auto newNode = coreLib.GetNode(leafName, CreateUniqueName(leafName));
            if (newNode != nullptr)
            {
                newNode->Build();
                ImVec2 pos = (mousePos - scroll - position) / scale;
                newNode->Translate(pos - newNode->GetRectNode().GetCenter());
                newNode->GetFlagSet().SetFlag(NodeFlag::Visible | NodeFlag::Hovered | NodeFlag::Highlighted);
                coreNodeVec.push_back(newNode);
                if (highlightedNode != nullptr)
                {
                    highlightedNode->GetFlagSet().UnsetFlag(NodeFlag::Highlighted);
                }
                highlightedNode = newNode;
                modifFlag = true;
                coreLib.SetLeafClickedFalse();
            }
            else
            {
                Notifier::Add(Notif(Notif::Type::ERROR, "[" + leafName + "]" + " not found in the library!"));
                coreLib.SetLeafClickedFalse();
            }
        }
    }
    else if (state == State::Selecting)
    {
        rectSelecting = ImRect();
        SortNodeOrder();
        state = State::Default;
    }
    else if (state == State::Draging)
    {
        state = State::HoveringNode; //SetState:HoveringNode
        if (mNodeDrag == true)
        {
            modifFlag = true;
            mNodeDrag = false;
        }
    }
    else if (state == State::DragingInput || state == State::DragingOutput)
    {
        if (iNodeInput && iNodeOutput) // About to connect.
        {
            if (iNodeInput->GetTargetNodeOutput()) // If input is already connected to another output.
            {
                iNodeInput->BreakLink();
                EraseLink(iNodeInput);
            }

            Link link;
            if (state == State::DragingInput)
            {
                link.inputNode = iNode;
                link.outputNode = hovNode;
                iNodeInput->SetTargetNode(hovNode);
            }
            if (state == State::DragingOutput)
            {
                link.inputNode = hovNode;
                link.outputNode = iNode;
                iNodeInput->SetTargetNode(iNode);
            }
            iNodeInput->SetTargetNodeOutput(iNodeOutput);
            iNodeOutput->IncreaseLinkNum();

            link.inputPort = iNodeInput;
            link.outputPort = iNodeOutput;
            linkVec.push_back(link);
            modifFlag = true;
        }

        inputFreeLink = ImVec2();
        outputFreeLink = ImVec2();
        state = State::Default;
    }
}

void CoreDiagram::MouseRightButtonRelease()
{
    // Open diagram popup menu.
    const ImGuiIO& io = ImGui::GetIO();
    if (state != State::None && rectCanvas.Contains(mousePos) && ImGui::IsMouseDown(0) == false && ImGui::IsMouseReleased(1) && iNode == nullptr)
    {
        if (io.MouseDragMaxDistanceSqr[1] < (io.MouseDragThreshold * io.MouseDragThreshold))
        {
            bool selected = false;
            for (int node_idx = 0; node_idx < coreNodeVec.size(); ++node_idx)
            {
                if (coreNodeVec[node_idx]->GetFlagSet().HasFlag(NodeFlag::Selected))
                {
                    selected = true;
                    break;
                }
            }
            if (false == selected)
            {
                ImGui::OpenPopup("DiagramPopupMenu");
            }
        }
    }
}

void CoreDiagram::KeyboardPressDelete()
{
    std::vector<CoreNode*> unselectedNodes;
    std::vector<CoreNode*> selectedNodes;
    unselectedNodes.reserve(coreNodeVec.size());
    selectedNodes.reserve(coreNodeVec.size());
    for (const auto& node : coreNodeVec)
    {
        if (node->GetFlagSet().HasFlag(NodeFlag::Selected))
        {
            // Delete all input connections.
            for (auto& input : node->GetInputVec())
            {
                if (input.GetTargetNode() != nullptr)
                {
                    input.BreakLink();
                    EraseLink(&input);
                }
            }
            // Delete all connections that are connected with outputs.
            for (const auto& otherNode : coreNodeVec)
            {
                for (auto& input : otherNode->GetInputVec())
                {
                    if (input.GetTargetNode() != nullptr && input.GetTargetNode() == node)
                    {
                        input.BreakLink();
                        EraseLink(&input);
                    }
                }
            }
            // Check all output connections.
            for (const auto& output : node->GetOutputVec())
            {
                IM_ASSERT(output.GetLinkNum() == 0);
            }

            if (node == highlightedNode)
            {
                highlightedNode = nullptr;
            }
            if (node == hovNode)
            {
                hovNode = nullptr;
            }
            if (node == iNode)
            {
                iNode = nullptr;
            }
            selectedNodes.push_back(node);
            modifFlag = true;
        }
        else
        {
            unselectedNodes.push_back(node);
        }
    }

    // Delete selected nodes
    for (const auto& node : selectedNodes)
    {
        delete node;
    }
    coreNodeVec = unselectedNodes;
}

void CoreDiagram::DrawCanvasElements()
{
    SetLinkProperties();
    DrawLinks();
    DrawNodes();
    DrawFreeLink();
    DrawSelecting();
    PrintInfo();
}

void CoreDiagram::DrawNodes() const
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGui::SetWindowFontScale(scale);
    const ImVec2 offset = position + scroll;
    for (const auto& node : coreNodeVec)
    {
        node->Draw(drawList, offset, scale);
    }
    ImGui::SetWindowFontScale(1.0f);
}

void CoreDiagram::DrawSelecting() const
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (state == State::Selecting)
    {
        drawList->AddRectFilled(rectSelecting.Min, rectSelecting.Max, ImColor(1.0f, 1.0f, 0.0f, 0.05f));
        drawList->AddRect(rectSelecting.Min, rectSelecting.Max, ImColor(1.0f, 1.0f, 1.0f, 0.5f));
    }
}

void CoreDiagram::PrintInfo()
{
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    ImGui::NewLine();

    ImGui::NewLine();
    ImGui::Text("Position: %.2f, %.2f", position.x, position.y);
    ImGui::Text("Size: %.2f, %.2f", size.x, size.y);
    (state != State::None) ? ImGui::Text("Mouse: %.2f, %.2f", mousePos.x, mousePos.y) : ImGui::Text("Mouse: Outside of Canvas");
    ImGui::Text("CanvasTL: %.2f, %.2f", rectCanvas.GetTL().x, rectCanvas.GetTL().y);
    ImGui::Text("CanvasBR: %.2f, %.2f", rectCanvas.GetBR().x, rectCanvas.GetBR().y);
    ImGui::Text("Scroll: %.2f, %.2f", scroll.x, scroll.y);
    ImGui::Text("Scale: %.2f", scale);

    ImGui::NewLine();
    int numberOfVisibleNodes = 0;
    for (const auto& node : coreNodeVec)
    {
        if (node->GetFlagSet().HasFlag(NodeFlag::Visible))
        {
            numberOfVisibleNodes += 1;
        }
    }
    ImGui::Text("Visible Nodes #: %d", numberOfVisibleNodes);

    // Only 8 bits.
    ImGui::NewLine();
    ImGui::Text("FlagSet Node: %s", iNode ? iNode->GetFlagSet().Get().substr(8 * sizeof(unsigned int) - 8, 8).c_str() : nullptr);
    ImGui::Text("FlagSet Input: %s", iNodeInput ? iNodeInput->GetFlagSet().Get().substr(8 * sizeof(unsigned int) - 8, 8).c_str() : nullptr);
    ImGui::Text("FlagSet Output: %s", iNodeOutput ? iNodeOutput->GetFlagSet().Get().substr(8 * sizeof(unsigned int) - 8, 8).c_str() : nullptr);

    ImGui::NewLine();
    switch (state)
    {
        case State::None: ImGui::Text("None"); break;
        case State::Default: ImGui::Text("Default"); break;
        case State::HoveringNode: ImGui::Text("HoveringNode"); break;
        case State::HoveringInput: ImGui::Text("HoveringInput"); break;
        case State::HoveringOutput: ImGui::Text("HoveringOutput"); break;
        case State::Draging: ImGui::Text("Draging"); break;
        case State::DragingInput: ImGui::Text("DragingInput"); break;
        case State::DragingOutput: ImGui::Text("DragingOutput"); break;
        case State::Selecting: ImGui::Text("Selecting"); break;
        default: ImGui::Text("UNKNOWN"); break;
    }
    ImGui::Text("iNode: %s", iNode ? iNode->GetName().c_str() : nullptr);
    ImGui::Text("iNodeInput: %s", iNodeInput ? iNodeInput->GetName().c_str() : nullptr);
    ImGui::Text("iNodeOutput: %s", iNodeOutput ? iNodeOutput->GetName().c_str() : nullptr);

    ImGui::NewLine();
    ImGui::Text("Hovered: %s", hovNode ? hovNode->GetName().c_str() : nullptr);
    ImGui::Text("Highlighted: %s", highlightedNode ? highlightedNode->GetName().c_str() : nullptr);

    ImGui::NewLine();
    ImGui::Text("iNodeOutput_Link#: %d", iNodeOutput ? iNodeOutput->GetLinkNum() : 0);

    ImGui::NewLine();
    int linkNum = 0;
    for (const auto& node : coreNodeVec)
    {
        for (const auto& input : node->GetInputVec())
        {
            if (input.GetTargetNode() != nullptr)
            {
                linkNum += 1;
            }
        }
    }
    if (linkVec.size() != linkNum)
    {
        ImGui::Text("Error LinkNum-Links");
    }

    ImGui::Text("Total Links #: %d", linkVec.size());
    int numberOfVisibleLinks = 0;
    const ImVec2 offset = position + scroll;
    for (const auto& link : linkVec)
    {
        ImVec2 pInput = link.inputPort->GetPosition() * scale + offset;
        ImVec2 pOutput = link.outputPort->GetPosition() * scale + offset;
        if (IsLinkVisible(pInput, pOutput) == true)
        {
            numberOfVisibleLinks += 1;
        }
    }
    ImGui::Text("Visible Links #: %d", numberOfVisibleLinks);

    if (iNode != nullptr)
    {
        ImGui::NewLine();
        ImGui::Text("[wrtOrigin]");
        auto hTL = ImVec2(iNode->GetRectNode().GetTL() + scroll);
        auto hBR = ImVec2(iNode->GetRectNode().GetBR() + scroll);
        ImGui::Text("iNodeTL: %.2f, %.2f", hTL.x, hTL.y);
        ImGui::Text("iNodeBR: %.2f, %.2f", hBR.x, hBR.y);
    }
}

void CoreDiagram::UpdateCanvasRect()
{
    mousePos = ImGui::GetMousePos();
    position = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
    size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
    rectCanvas = ImRect(position, position + size);
}

void CoreDiagram::UpdateCanvasScrollZoom()
{
    const ImGuiIO& io = ImGui::GetIO();
    bool dragCond = state == State::DragingInput || state == State::DragingOutput;
    if (state != State::None && (ImGui::IsMouseDown(0) == false || dragCond) && rectCanvas.Contains(mousePos))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Space))
        {
            scroll = {};
            scale = 1.0f;
        }
        if (ImGui::IsMouseDragging(1))
        {
            scroll += io.MouseDelta;
        }
        ImVec2 focus = (mousePos - scroll - position) / scale;
        auto zoom = static_cast<int>(io.MouseWheel);
        float biasScale = 0.10f;
        if (zoom < 0)
        {
            while (zoom < 0)
            {
                scale = ImMax(scaleMin, scale - biasScale);
                zoom += 1;
            }
        }
        if (zoom > 0)
        {
            while (zoom > 0)
            {
                scale = ImMin(scaleMax, scale + biasScale);
                zoom -= 1;
            }
        }
        if (ImGui::IsMouseClicked(2))
        {
            scale = 1.0f;
        }
        ImVec2 shift = scroll + (focus * scale);
        scroll += mousePos - shift - position;
    }
}

void CoreDiagram::UpdateCanvasGrid(ImDrawList* drawList) const
{
    const float grid = 32.0f * scale;
    float x = std::fmodf(scroll.x, grid);
    float y = std::fmodf(scroll.y, grid);
    auto markX = static_cast<int>(scroll.x / grid);
    auto markY = static_cast<int>(scroll.y / grid);
    while (x < size.x)
    {
        ImColor color = markX % 5 ? ImColor(0.5f, 0.5f, 0.5f, 0.1f) : ImColor(1.0f, 1.0f, 1.0f, 0.1f);
        drawList->AddLine(ImVec2(x, 0.0f) + position, ImVec2(x, size.y) + position, color, 0.1f);
        x += grid;
        markX -= 1;
    }
    while (y < size.y)
    {
        ImColor color = markY % 5 ? ImColor(0.5f, 0.5f, 0.5f, 0.1f) : ImColor(1.0f, 1.0f, 1.0f, 0.1f);
        drawList->AddLine(ImVec2(0.0f, y) + position, ImVec2(size.x, y) + position, color, 0.1f);
        y += grid;
        markY -= 1;
    }
}

void CoreDiagram::UpdateNodeFlags()
{
    hovNode = nullptr; // Important.
    if (coreNodeVec.empty())
    {
        return;
    }

    ImVec2 offset = position + scroll;
    ImRect canvas(position, position + size);
    for (auto it = coreNodeVec.rbegin(); it != coreNodeVec.rend(); ++it)
    {
        CoreNode* node = *it;

        // Unset hovered flag
        node->GetFlagSet().UnsetFlag(NodeFlag::Hovered);

        // Node and io areas
        ImRect nodeArea = node->GetRectNode();
        nodeArea.Min *= scale;
        nodeArea.Max *= scale;
        nodeArea.Translate(offset);
        nodeArea.ClipWith(canvas);
        auto inputArea = ImRect(node->GetInputVec().front().GetRectPin().Min, node->GetInputVec().back().GetRectPin().Max);
        inputArea.Min *= scale;
        inputArea.Max *= scale;
        inputArea.Translate(offset);
        inputArea.ClipWith(canvas);
        auto outputArea = ImRect(node->GetOutputVec().front().GetRectPin().Min, node->GetOutputVec().back().GetRectPin().Max);
        outputArea.Min *= scale;
        outputArea.Max *= scale;
        outputArea.Translate(offset);
        outputArea.ClipWith(canvas);

        // Visibility
        if (canvas.Overlaps(nodeArea) || canvas.Overlaps(inputArea) || canvas.Overlaps(outputArea))
        {
            node->GetFlagSet().SetFlag(NodeFlag::Visible);
        }
        else
        {
            node->GetFlagSet().UnsetFlag(NodeFlag::Visible);
            continue;
        }

        // Hovered Node Condition
        if (state != State::None && hovNode == nullptr &&
            (nodeArea.Contains(mousePos) || inputArea.Contains(mousePos) || outputArea.Contains(mousePos)))
        {
            hovNode = node;
            hovNode->GetFlagSet().SetFlag(NodeFlag::Hovered);
        }

        // Selecting
        if (state == State::Selecting)
        {
            if (rectSelecting.Overlaps(nodeArea))
            {
                node->GetFlagSet().SetFlag(NodeFlag::Selected);
                continue;
            }
            else if (ImGui::GetIO().KeyShift == false)
            {
                node->GetFlagSet().UnsetFlag(NodeFlag::Selected);
            }
        }
        UpdateInputFlags(node);
        UpdateOutputFlags(node);
    }
}

void CoreDiagram::UpdateInputFlags(CoreNode* node)
{
    for (auto& input : node->GetInputVec())
    {
        if (input.GetType() == PortType::None)
        {
            continue;
        }
        input.GetFlagSet().UnsetFlag(PortFlag::Hovered | PortFlag::Connectible | PortFlag::Draging);

        if (state == State::DragingInput)
        {
            if (iNodeInput == &input)
            {
                input.GetFlagSet().SetFlag(PortFlag::Draging);
            }
            continue;
        }

        if (state == State::DragingOutput)
        {
            if (iNode == node) // Not allowed to connect itself.
            {
                continue;
            }
            if (ConnectionRules(node, iNode, &input, iNodeOutput))
            {
                input.GetFlagSet().SetFlag(PortFlag::Connectible);
            }
        }

        // If node is not hovNode.
        if (hovNode != node)
        {
            continue;
        }
        // If the state is selecting.
        if (state == State::Selecting)
        {
            continue;
        }
        // If input belongs to a selected node. However, it is connectible from an output.
        if (state != State::DragingOutput && node->GetFlagSet().HasFlag(NodeFlag::Selected))
        {
            continue;
        }

        // Hovered input condition.
        ImRect inputRect = input.GetRectPort();
        inputRect.Min *= scale;
        inputRect.Max *= scale;
        ImVec2 offset = position + scroll;
        inputRect.Translate(offset);
        if (state != State::None && inputRect.Contains(mousePos))
        {
            if (state != State::DragingOutput)
            {
                input.GetFlagSet().SetFlag(PortFlag::Hovered);
                continue;
            }
            if (input.GetFlagSet().HasFlag(PortFlag::Connectible))
            {
                input.GetFlagSet().SetFlag(PortFlag::Hovered);
            }
        }
    }
}

void CoreDiagram::UpdateOutputFlags(CoreNode* node)
{
    for (auto& output : node->GetOutputVec())
    {
        if (output.GetType() == PortType::None)
        {
            continue;
        }
        output.GetFlagSet().UnsetFlag(PortFlag::Hovered | PortFlag::Connectible | PortFlag::Draging);

        if (state == State::DragingOutput)
        {
            if (iNodeOutput == &output)
            {
                output.GetFlagSet().SetFlag(PortFlag::Draging);
            }
            continue;
        }

        if (state == State::DragingInput)
        {
            if (iNode == node) // Not allowed to connect itself.
            {
                continue;
            }
            if (ConnectionRules(iNode, node, iNodeInput, &output))
            {
                output.GetFlagSet().SetFlag(PortFlag::Connectible);
            }
        }

        // If node is not hovNode.
        if (hovNode != node)
        {
            continue;
        }
        // If the state is selecting.
        if (state == State::Selecting)
        {
            continue;
        }
        // If output belongs to a selected node. However, it is connectible from an input.
        if (state != State::DragingInput && node->GetFlagSet().HasFlag(NodeFlag::Selected))
        {
            continue;
        }

        // Hovered output condition.
        ImRect outputRect = output.GetRectPort();
        outputRect.Min *= scale;
        outputRect.Max *= scale;
        ImVec2 offset = position + scroll;
        outputRect.Translate(offset);
        if (state != State::None && outputRect.Contains(mousePos))
        {
            if (state != State::DragingInput)
            {
                output.GetFlagSet().SetFlag(PortFlag::Hovered);
                continue;
            }
            if (output.GetFlagSet().HasFlag(PortFlag::Connectible))
            {
                output.GetFlagSet().SetFlag(PortFlag::Hovered);
            }
        }
    }
}

void CoreDiagram::HighlightNode()
{
    if (highlightedNode != nullptr) // Unset highlighted node.
    {
        highlightedNode->GetFlagSet().UnsetFlag(NodeFlag::Highlighted);
    }
    iNode->GetFlagSet().SetFlag(NodeFlag::Highlighted);
    highlightedNode = iNode; // Set highlighted node.
    if (coreNodeVec.back() != iNode)
    {
        coreNodeVec.erase(std::find(coreNodeVec.begin(), coreNodeVec.end(), iNode));
        coreNodeVec.push_back(iNode); // Bring iNode to front.
    }
}

std::string CoreDiagram::CreateUniqueName(const std::string& libName) const
{
    std::string name = libName;
    unsigned int i = 0;
    bool nameFlag = true;
    while (nameFlag == true)
    {
        nameFlag = false;
        for (auto n : coreNodeVec)
        {
            if (n->GetName() == name)
            {
                nameFlag = true;
                if (i != 0)
                {
                    for (int k = 0; k < std::to_string(i).length(); k++)
                    {
                        name.pop_back();
                    }
                }
                i += 1;
                name += std::to_string(i);
            }
        }
    }
    return name;
}

bool CoreDiagram::ConnectionRules([[maybe_unused]] const CoreNode* inputNode, const CoreNode* outputNode, const CoreNodeInput* input, const CoreNodeOutput* output) const
{
    if (input->GetTargetNode() != nullptr && input->GetTargetNode() == outputNode) // If there is already a connection.
    {
        return false;
    }
    if (input->GetDataType() == output->GetDataType())
    {
        return true;
    }
    if (input->GetDataType() == PortDataType::Generic)
    {
        return true;
    }
    if (output->GetDataType() == PortDataType::Generic)
    {
        return true;
    }
    return false;
}

void CoreDiagram::SortNodeOrder()
{
    std::vector<CoreNode*> unselectedNodes;
    std::vector<CoreNode*> selectedNodes;
    unselectedNodes.reserve(coreNodeVec.size());
    selectedNodes.reserve(coreNodeVec.size());
    for (const auto& node : coreNodeVec)
    {
        if (node->GetFlagSet().HasFlag(NodeFlag::Selected))
        {
            selectedNodes.push_back(node);
        }
        else
        {
            unselectedNodes.push_back(node);
        }
    }
    int i = 0;
    for (const auto& node : unselectedNodes)
    {
        coreNodeVec[i] = node;
        i++;
    }
    for (const auto& node : selectedNodes)
    {
        coreNodeVec[i] = node;
        i++;
    }
}

void CoreDiagram::PopupMenu()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("DiagramPopupMenu"))
    {
        if (ImGui::MenuItem("Fit to window"))
        {
            FitToWindow();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void CoreDiagram::FitToWindow()
{
    if (coreNodeVec.empty() == true)
    {
        return;
    }

    // Rectangle of all nodes.
    ImVec2 min = coreNodeVec.at(0)->GetRectNode().GetTL();
    ImVec2 max = coreNodeVec.at(0)->GetRectNode().GetBR();
    for (const auto& element : coreNodeVec)
    {
        if (auto xTL = element->GetRectNode().GetTL().x; min.x > xTL) { min.x = xTL; }
        if (auto yTL = element->GetRectNode().GetTL().y; min.y > yTL) { min.y = yTL; }
        if (auto xBR = element->GetRectNode().GetBR().x; max.x < xBR) { max.x = xBR; }
        if (auto yBR = element->GetRectNode().GetBR().y; max.y < yBR) { max.y = yBR; }
    }
    auto rectNodes = ImRect(min, max);

    // Scaling to fit.
    auto rX = rectCanvas.GetWidth() / rectNodes.GetWidth();
    auto rY = rectCanvas.GetHeight() / rectNodes.GetHeight();
    auto r = rY < rX ? rY : rX;
    scale = r * 0.80f; // Canvas is 1.25x of the rectNodes.
    scale = std::floor(scale * 10.0f) * 0.1f;
    if (scale >= scaleMax)
    {
        scale = scaleMax;
    }
    if (scale <= scaleMin)
    {
        scale = scaleMin;
        Notifier::Add(Notif(Notif::Type::WARNING, "No more zoom out!", "There might be nodes outside of the visible canvas!", 5));
    }

    scroll -= rectNodes.GetCenter() * scale + scroll;
    scroll = scroll + ImVec2(rectCanvas.GetWidth(), rectCanvas.GetHeight()) * 0.5f;
}

void CoreDiagram::EraseLink(const CoreNodeInput* input)
{
    auto it = linkVec.begin();
    while (it != linkVec.end())
    {
        if (it->inputPort == input)
        {
            it = linkVec.erase(it);
            return;
        }
        else
        {
            ++it;
        }
    }
}

bool CoreDiagram::IsLinkVisible(ImVec2 pInput, ImVec2 pOutput) const
{
    if (rectCanvas.Contains(pInput) || rectCanvas.Contains(pOutput))
    {
        return true;
    }
    return false;
}

void CoreDiagram::DrawLinks() const
{
    for (const auto& link : linkVec)
    {
        const ImVec2 offset = position + scroll;
        ImVec2 pInput = link.inputPort->GetPosition() * scale + offset;
        ImVec2 pOutput = link.outputPort->GetPosition() * scale + offset;

        if (link.inputNode->GetFlagSet().HasFlag(NodeFlag::Collapsed))
        {
            float midPointY = (link.inputNode->GetRectNode().Max.y - link.inputNode->GetRectNode().Min.y) * 0.5f;
            ImVec2 collapsedPoint;
            if (link.inputNode->IsPortInverted() == false)
            {
                collapsedPoint = link.inputNode->GetRectNode().Min + ImVec2(0, midPointY);
            }
            else
            {
                collapsedPoint = link.inputNode->GetRectNode().Max - ImVec2(0, midPointY);
            }
            pInput = collapsedPoint * scale + offset;
        }
        if (link.outputNode->GetFlagSet().HasFlag(NodeFlag::Collapsed))
        {
            float midPointY = (link.outputNode->GetRectNode().Max.y - link.outputNode->GetRectNode().Min.y) * 0.5f;
            ImVec2 collapsedPoint;
            if (link.outputNode->IsPortInverted() == false)
            {
                collapsedPoint = link.outputNode->GetRectNode().Max - ImVec2(0, midPointY);
            }
            else
            {
                collapsedPoint = link.outputNode->GetRectNode().Min + ImVec2(0, midPointY);
            }
            pOutput = collapsedPoint * scale + offset;
        }
        if (IsLinkVisible(pInput, pOutput) == false)
        {
            continue;
        }

        float linkDistance = std::sqrt((pInput.x - pOutput.x) * (pInput.x - pOutput.x) + (pInput.y - pOutput.y) * (pInput.y - pOutput.y)) / 150.0f;
        float rounding = 25.0f * linkDistance / scale;
        float dHandle = 15.0f * scale;
        
        if (link.type == LinkType::BINV_LEFT)
        {
            DrawLinkBezier(link, pInput, pOutput, rounding, true);
        }
        else if (link.type == LinkType::BINV_RIGHT_OVER || link.type == LinkType::BINV_RIGHT_UNDER || link.type == LinkType::BINV_RIGHT_MID)
        {
            DrawLinkBNInv(link, pInput, pOutput, dHandle, true);
        }
        else if (link.type == LinkType::IINV_RIGHT_OVER || link.type == LinkType::IINV_LEFT_OVER ||
            link.type == LinkType::IINV_RIGHT_UNDER || link.type == LinkType::IINV_LEFT_UNDER ||
            link.type == LinkType::OINV_RIGHT_OVER || link.type == LinkType::OINV_LEFT_OVER ||
            link.type == LinkType::OINV_RIGHT_UNDER || link.type == LinkType::OINV_LEFT_UNDER)
        {
            DrawLinkIOInv(link, pInput, pOutput, dHandle);
        }
        else if (link.type == LinkType::IINV_MID || link.type == LinkType::OINV_MID)
        {
            DrawLinkBezier(link, pInput, pOutput, 0.0f);
        }
        else if (link.type == LinkType::NINV_RIGHT)
        {
            DrawLinkBezier(link, pInput, pOutput, rounding);
        }
        else if (link.type == LinkType::NINV_LEFT_OVER || link.type == LinkType::NINV_LEFT_UNDER || link.type == LinkType::NINV_LEFT_MID)
        {
            DrawLinkBNInv(link, pInput, pOutput, dHandle);
        }
    }
}

void CoreDiagram::SetLinkProperties()
{
    for (auto& link : linkVec)
    {
        link.inputPort->SetLinkDir(0);
        link.inputPort->SetLinkSepX(0);
        link.inputPort->SetLinkSepY(0);
        link.inputPort->SetTargetLinkDir(0);
        link.inputPort->SetTargetLinkSep(0);
    }
    for (auto& link : linkVec)
    {
        link.type = LinkType::NONE;
        link.color = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
        link.thickness = 2.0f * scale;
        link.xSepIn = link.inputPort->GetRectPort().GetHeight() * scale;
        link.xSepOut = link.inputPort->GetRectPort().GetHeight() * scale;
        link.ykSep = 0;
        const ImVec2 offset = position + scroll;
        auto rInput = ImRect(link.inputNode->GetRectNode().Min * scale + offset, link.inputNode->GetRectNode().Max * scale + offset);
        auto rOutput = ImRect(link.outputNode->GetRectNode().Min * scale + offset, link.outputNode->GetRectNode().Max * scale + offset);
        float yInput = (link.inputPort->GetRectPin().GetCenter() * scale + offset).y;
        float yOutput = (link.inputPort->GetTargetNodeOutput()->GetRectPin().GetCenter() * scale + offset).y;
        float yMargin = 30.0f * scale;
        bool inputNodeInverted = link.inputNode->IsPortInverted();
        bool outputNodeInverted = link.outputNode->IsPortInverted();
        if (inputNodeInverted && outputNodeInverted)
        {
            if (rInput.Max.x <= rOutput.Min.x)
            {
                link.type = LinkType::BINV_LEFT;
            }
            if (rInput.Max.x > rOutput.Min.x)
            {
                float nodeMargin = 24.0f * scale;
                if (rInput.Max.y + nodeMargin < rOutput.Min.y)
                {
                    link.type = LinkType::BINV_RIGHT_OVER;
                    SetOutputSepUp(link);
                    SetInputSepDown(link);
                    SetNodeSep(link);
                }
                else if (rInput.Min.y > rOutput.Max.y + nodeMargin)
                {
                    link.type = LinkType::BINV_RIGHT_UNDER;
                    SetOutputSepDown(link);
                    SetInputSepUp(link);
                    SetNodeSep(link);
                }
                else
                {
                    link.type = LinkType::BINV_RIGHT_MID;
                    SetOutputSepDown(link);
                    SetInputSepDown(link);
                    SetNodeSep(link);
                }
            }
        }
        else if (inputNodeInverted)
        {
            if (yInput + yMargin < yOutput)
            {
                if (ImMax(rInput.Max.x, rOutput.Max.x) == rInput.Max.x)
                {
                    link.type = LinkType::IINV_RIGHT_OVER;
                    SetInputSepDown(link);
                }
                else if (ImMax(rInput.Max.x, rOutput.Max.x) == rOutput.Max.x)
                {
                    link.type = LinkType::IINV_LEFT_OVER;
                    SetOutputSepUp(link);
                }
            }
            else if (yInput > yOutput + yMargin)
            {
                if (ImMax(rInput.Max.x, rOutput.Max.x) == rInput.Max.x)
                {
                    link.type = LinkType::IINV_RIGHT_UNDER;
                    SetInputSepUp(link);
                }
                else if (ImMax(rInput.Max.x, rOutput.Max.x) == rOutput.Max.x)
                {
                    link.type = LinkType::IINV_LEFT_UNDER;
                    SetOutputSepDown(link);
                }
            }
            else
            {
                link.type = LinkType::IINV_MID;
            }
        }
        else if (outputNodeInverted)
        {
            if (yInput + yMargin < yOutput)
            {
                if (ImMax(rInput.Min.x, rOutput.Min.x) == rInput.Min.x)
                {
                    link.type = LinkType::OINV_RIGHT_OVER;
                    SetOutputSepUp(link);
                }
                else if (ImMax(rInput.Min.x, rOutput.Min.x) == rOutput.Min.x)
                {
                    link.type = LinkType::OINV_LEFT_OVER;
                    SetInputSepDown(link);
                }
            }
            else if (yInput > yOutput + yMargin)
            {
                if (ImMax(rInput.Min.x, rOutput.Min.x) == rInput.Min.x)
                {
                    link.type = LinkType::OINV_RIGHT_UNDER;
                    SetOutputSepDown(link);
                }
                else if (ImMax(rInput.Min.x, rOutput.Min.x) == rOutput.Min.x)
                {
                    link.type = LinkType::OINV_LEFT_UNDER;
                    SetInputSepUp(link);
                }
            }
            else
            {
                link.type = LinkType::OINV_MID;
            }
        }
        else if (rInput.Min.x >= rOutput.Max.x)
        {
            link.type = LinkType::NINV_RIGHT;
        }
        else if (rInput.Min.x < rOutput.Max.x)
        {
            float nodeMargin = 24.0f * scale;
            if (rInput.Max.y + nodeMargin < rOutput.Min.y)
            {
                link.type = LinkType::NINV_LEFT_OVER;
                SetInputSepDown(link);
                SetOutputSepUp(link);
                SetNodeSep(link);
            }
            else if (rInput.Min.y > rOutput.Max.y + nodeMargin)
            {
                link.type = LinkType::NINV_LEFT_UNDER;
                SetInputSepUp(link);
                SetOutputSepDown(link);
                SetNodeSep(link);
            }
            else
            {
                link.type = LinkType::NINV_LEFT_MID;
                SetOutputSepDown(link);
                SetInputSepDown(link);
                SetNodeSep(link);
            }
        }
    }
}

void CoreDiagram::DrawFreeLink() const
{
    if (inputFreeLink.x != outputFreeLink.x || inputFreeLink.y != outputFreeLink.y)
    {
        Link link;
        link.color = ImColor(0.996f, 0.431f, 0.000f, 1.0f);
        link.thickness = 2.0f * scale;
        DrawLinkBezier(link, inputFreeLink, outputFreeLink, 0.0f);
    }
}

void CoreDiagram::DrawLinkBezier(const Link& link, ImVec2 pInput, ImVec2 pOutput, float rounding, bool invert) const
{
    if (IsLinkVisible(pInput, pOutput) == true)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 handle = invert ? ImVec2(rounding, 0.0f) * scale : ImVec2(-rounding, 0.0f) * scale;
        ImVec2 p1 = pInput;
        ImVec2 p2 = pInput + handle;
        ImVec2 p3 = pOutput - handle;
        ImVec2 p4 = pOutput;
        drawList->AddBezierCurve(p1, p2, p3, p4, link.color, link.thickness);
    }
}

void CoreDiagram::DrawLinkIOInv(const Link& link, ImVec2 pInput, ImVec2 pOutput, float dHandle) const
{
    float xMax = ImMax(pInput.x, pOutput.x);
    float xMin = ImMin(pInput.x, pOutput.x);
    float xMargin = dHandle;

    float x1 = 0.0f;
    float x2 = 0.0f;
    // Input inverted.
    if (link.type == LinkType::IINV_LEFT_OVER || link.type == LinkType::IINV_LEFT_UNDER)
    {
        x1 = xMax - xMargin + link.xSepOut;
        x2 = x1 + dHandle;
    }
    if (link.type == LinkType::IINV_RIGHT_OVER || link.type == LinkType::IINV_RIGHT_UNDER)
    {
        x1 = xMax - xMargin + link.xSepIn;
        x2 = x1 + dHandle;
    }
    // Output inverted.
    if (link.type == LinkType::OINV_LEFT_OVER || link.type == LinkType::OINV_LEFT_UNDER)
    {
        x1 = xMin + xMargin - link.xSepIn;
        x2 = x1 - dHandle;
    }
    if (link.type == LinkType::OINV_RIGHT_OVER || link.type == LinkType::OINV_RIGHT_UNDER)
    {
        x1 = xMin + xMargin - link.xSepOut;
        x2 = x1 - dHandle;
    }
    // Input is over the output.
    if (link.type == LinkType::IINV_LEFT_OVER || link.type == LinkType::IINV_RIGHT_OVER ||
        link.type == LinkType::OINV_LEFT_OVER || link.type == LinkType::OINV_RIGHT_OVER)
    {
        dHandle *= +1;
    }
    // Input is under the output.
    if (link.type == LinkType::IINV_LEFT_UNDER || link.type == LinkType::IINV_RIGHT_UNDER ||
        link.type == LinkType::OINV_LEFT_UNDER || link.type == LinkType::OINV_RIGHT_UNDER)
    {
        dHandle *= -1;
    }

    float y1 = pInput.y + dHandle;
    float y2 = pOutput.y - dHandle;

    std::vector<ImVec2> points;
    points.reserve(8);
    points.push_back(pInput);
    points.push_back(ImVec2(x1, pInput.y));
    points.push_back(ImVec2(x2, pInput.y));
    points.push_back(ImVec2(x2, y1));
    points.push_back(ImVec2(x2, y2));
    points.push_back(ImVec2(x2, pOutput.y));
    points.push_back(ImVec2(x1, pOutput.y));
    points.push_back(pOutput);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddBezierQuadratic(points[0], (points[0] + points[1]) * 0.5f, points[1], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[1], points[2], points[3], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[3], (points[3] + points[4]) * 0.5f, points[4], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[4], points[5], points[6], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[6], (points[6] + points[7]) * 0.5f, points[7], link.color, link.thickness);
}

void CoreDiagram::DrawLinkBNInv(const Link& link, ImVec2 pInput, ImVec2 pOutput, float dHandle, bool invert) const
{
    const ImVec2 offset = position + scroll;
    auto rInputNode = ImRect(link.inputNode->GetRectNode().Min * scale + offset, link.inputNode->GetRectNode().Max * scale + offset);
    auto rOutputNode = ImRect(link.outputNode->GetRectNode().Min * scale + offset, link.outputNode->GetRectNode().Max * scale + offset);

    // Unique lines between two nodes.
    std::vector<CoreNodeOutput*> vec;
    for (const auto& input : link.inputNode->GetInputVec())
    {
        if (input.GetTargetNode() == link.outputNode)
        {
            vec.push_back(input.GetTargetNodeOutput());
        }
    }
    auto numberOfUniqueLines = (int)std::set<CoreNodeOutput*>(vec.begin(), vec.end()).size();

    float x1 = 0;
    float x2 = 0;
    float x3 = 0;
    float x4 = 0;
    float xMargin = dHandle;
    if (invert == false)
    {
        x1 = pInput.x + xMargin - link.xSepIn;
        x2 = x1 - dHandle;
        x3 = pOutput.x - xMargin + link.xSepOut;
        x4 = x3 + dHandle;
    }
    else
    {
        x1 = pInput.x - xMargin + link.xSepIn;
        x2 = x1 + dHandle;
        x3 = pOutput.x + xMargin - link.xSepOut;
        x4 = x3 - dHandle;
    }

    float yM = 0;
    float y1 = 0;
    float y2 = 0;
    float y3 = 0;
    float y4 = 0;
    float y5 = 0;
    if (link.type == LinkType::NINV_LEFT_OVER || link.type == LinkType::BINV_RIGHT_OVER)
    {
        dHandle *= +1;
        yM = rInputNode.Max.y + link.ykSep * (rOutputNode.Min.y - rInputNode.Max.y) / float(numberOfUniqueLines + 1);
        y1 = pInput.y + dHandle;
        y2 = yM - dHandle;
        y3 = y2 + dHandle;
        y4 = y3 + dHandle;
        y5 = pOutput.y - dHandle;
    }
    else if (link.type == LinkType::NINV_LEFT_UNDER || link.type == LinkType::BINV_RIGHT_UNDER)
    {
        dHandle *= -1;
        yM = rOutputNode.Max.y + link.ykSep * (rInputNode.Min.y - rOutputNode.Max.y) / float(numberOfUniqueLines + 1);
        y1 = pInput.y + dHandle;
        y2 = yM - dHandle;
        y3 = y2 + dHandle;
        y4 = y3 + dHandle;
        y5 = pOutput.y - dHandle;
    }
    else if (link.type == LinkType::NINV_LEFT_MID || link.type == LinkType::BINV_RIGHT_MID)
    {
        float yMax = ImMax(rInputNode.Max.y, rOutputNode.Max.y);
        float yMargin = 4.0f * scale;
        yM = yMax + (yMargin + dHandle) * link.ykSep;

        y1 = pInput.y + dHandle;
        y2 = yM - dHandle;
        y3 = y2 + dHandle;
        y4 = y3 - dHandle;
        y5 = pOutput.y + dHandle;
    }

    std::vector<ImVec2> points;
    points.reserve(14);
    points.push_back(pInput);
    points.push_back(ImVec2(x1, pInput.y));
    points.push_back(ImVec2(x2, pInput.y));
    points.push_back(ImVec2(x2, y1));
    points.push_back(ImVec2(x2, y2));
    points.push_back(ImVec2(x2, y3));
    points.push_back(ImVec2(x1, y3));
    points.push_back(ImVec2(x3, y3));
    points.push_back(ImVec2(x4, y3));
    points.push_back(ImVec2(x4, y4));
    points.push_back(ImVec2(x4, y5));
    points.push_back(ImVec2(x4, pOutput.y));
    points.push_back(ImVec2(x3, pOutput.y));
    points.push_back(pOutput);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddBezierQuadratic(points[0], (points[0] + points[1]) * 0.5f, points[1], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[1], points[2], points[3], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[3], (points[3] + points[4]) * 0.5f, points[4], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[4], points[5], points[6], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[6], (points[6] + points[7]) * 0.5f, points[7], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[7], points[8], points[9], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[9], (points[9] + points[10]) * 0.5f, points[10], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[10], points[11], points[12], link.color, link.thickness);
    drawList->AddBezierQuadratic(points[12], (points[12] + points[13]) * 0.5f, points[13], link.color, link.thickness);
}

void CoreDiagram::SetInputSepDown(Link& link) const
{
    int kSep = 1;
    while (true)
    {
        bool full = false;
        for (const auto& input : link.inputNode->GetInputVec())
        {
            if (input.GetLinkSepX() == kSep &&
                (input.GetOrder() < link.inputPort->GetOrder() && input.GetLinkDir() == -1) == false &&
                input.GetTargetNodeOutput() != link.outputPort) // If it is the same output, no need to seperate.
            {
                full = true;
                kSep += 1;
                break;
            }
        }
        if (full == false)
        {
            link.inputPort->SetLinkDir(1);
            link.inputPort->SetLinkSepX(kSep);
            link.xSepIn *= (float)kSep;
            break;
        }
    }
}

void CoreDiagram::SetInputSepUp(Link& link) const
{
    int kSep = 1;
    while (true)
    {
        bool full = false;
        for (const auto& input : link.inputNode->GetInputVec())
        {
            if (input.GetLinkSepX() == kSep &&
                (input.GetOrder() > link.inputPort->GetOrder() && input.GetLinkDir() == 1) == false &&
                input.GetTargetNodeOutput() != link.outputPort) // If it is the same output, no need to seperate.
            {
                full = true;
                kSep += 1;
                break;
            }
        }
        if (full == false)
        {
            link.inputPort->SetLinkDir(-1);
            link.inputPort->SetLinkSepX(kSep);
            link.xSepIn *= (float)kSep;
            break;
        }
    }
}

void CoreDiagram::SetOutputSepUp(Link& link) const
{
    // Collect all inputs related with the output node.
    std::vector<CoreNodeInput*> inputVec;
    for (const auto& node : coreNodeVec)
    {
        for (auto& input : node->GetInputVec())
        {
            if (input.GetTargetNode() == link.outputNode)
            {
                inputVec.push_back(&input);
            }
        }
    }

    int kSep = 1;
    while (true)
    {
        bool full = false;
        for (const auto& input : inputVec)
        {
            if (input->GetTargetLinkSep() == kSep &&
                (input->GetTargetNodeOutput()->GetOrder() >= link.outputPort->GetOrder() && input->GetTargetLinkDir() == 1) == false &&
                input->GetTargetNodeOutput() != link.outputPort) // If it is the same output, no need to seperate.
            {
                full = true;
                kSep += 1;
                break;
            }
        }
        if (full == false)
        {
            link.inputPort->SetTargetLinkDir(-1);
            link.inputPort->SetTargetLinkSep(kSep);
            link.xSepOut *= (float)kSep;
            break;
        }
    }
}

void CoreDiagram::SetOutputSepDown(Link& link) const
{
    // Collect all inputs related with the output node.
    std::vector<CoreNodeInput*> inputVec;
    for (const auto& node : coreNodeVec)
    {
        for (auto& input : node->GetInputVec())
        {
            if (input.GetTargetNode() == link.outputNode)
            {
                inputVec.push_back(&input);
            }
        }
    }

    int kSep = 1;
    while (true)
    {
        bool full = false;
        for (const auto& input : inputVec)
        {
            if (input->GetTargetNode() == link.outputNode && input->GetTargetLinkSep() == kSep &&
                (input->GetTargetNodeOutput()->GetOrder() <= link.outputPort->GetOrder() && input->GetTargetLinkDir() == -1) == false &&
                input->GetTargetNodeOutput() != link.outputPort) // If it is the same output, no need to seperate.
            {
                full = true;
                kSep += 1;
                break;
            }
        }
        if (full == false)
        {
            link.inputPort->SetTargetLinkDir(1);
            link.inputPort->SetTargetLinkSep(kSep);
            link.xSepOut *= (float)kSep;
            break;
        }
    }
}

void CoreDiagram::SetNodeSep(Link& link) const
{
    int kSep = 1;
    while (true)
    {
        bool full = false;
        for (const auto& input : link.inputNode->GetInputVec())
        {
            if (input.GetLinkSepY() == kSep &&
                input.GetTargetNodeOutput() != link.outputPort) // If it is the same output, no need to seperate.
            {
                full = true;
                kSep += 1;
                break;
            }
        }
        if (full == false)
        {
            link.inputPort->SetLinkSepY(kSep);
            link.ykSep = float(kSep);
            break;
        }
    }
}