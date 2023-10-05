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

enum class NodeType
{
    None = 0,
    Generic, // TODO change to core module?
    CoreIn, // maybe
    CoreOut // maybe
};

class CoreNode
{
private:
    std::string name;
    std::string libName;
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
    float inputsWidth = 0.0f;
    float inputsHeight = 0.0f;
    float outputsWidth = 0.0f;
    float outputsHeight = 0.0f;
    std::string nameEdited;

protected:
    void AddInput(CoreNodeInput& input);
    void AddOutput(CoreNodeOutput& output);
    void BuildGeometry();
    virtual void SaveProperties(pugi::xml_node& xmlNode) = 0;
    virtual void LoadProperties(pugi::xml_node& xmlNode) = 0;
    bool modifFlag = false;

    static bool IsNameUnique(std::string_view str, const std::vector<CoreNode*>& coreNodeVec);
    void EditName(const std::vector<CoreNode*>& coreNodeVec);
    bool editingName = false;

public:
    CoreNode() = default;
    CoreNode(const std::string& name, const std::string& libName, NodeType type, ImColor colorNode);
    virtual ~CoreNode() = default;
    void Save(pugi::xml_node& xmlNode);
    void Load(const pugi::xml_node& xmlNode);
    bool GetModifFlag() const { return modifFlag; }
    void ResetModifFlag() { modifFlag = false; }

    std::string GetName() const { return name; }
    std::string GetLibName() const { return libName; }
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

    void Translate(ImVec2 delta, bool selectedOnly = false);
    void Draw(ImDrawList* drawList, ImVec2 offset, float scale) const;
    void InvertPort();
    bool IsPortInverted() const { return portInverted; }

    virtual void Build() = 0;
    virtual void DrawProperties(const std::vector<CoreNode*>& coreNodeVec) = 0;
};

class NodeParamDouble
{
private:
    std::string name;
    bool edit = false;
    double data;

public:
    explicit NodeParamDouble(const std::string& name, double v) : name(name), data(v) {}
    virtual ~NodeParamDouble() = default;
    double Get() const { return data; }
    void Set(double v) { data = v; }
    void Draw(bool& modifFlag, double step = 0.0, double stepFast = 0.0)
    {
        ImGui::SetNextItemWidth(140);
        ImGui::PushStyleColor(ImGuiCol_Text, edit ? ImVec4(0.992f, 0.914f, 0.169f, 1.0f) : ImGuiStyle().Colors[ImGuiCol_Text]);
        if (ImGui::InputDouble(name.c_str(), &data, step, stepFast, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            edit = false;
            modifFlag = true;
        }
        edit = ImGui::IsItemActive() ? true : false;
        ImGui::PopStyleColor(1);
    }
};

#endif /* CORENODE_HPP */