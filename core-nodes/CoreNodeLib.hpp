/******************************************************************************************
*                                                                                         *
*    Core Node Library                                                                    *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#ifndef CORENODELIB_HPP
#define CORENODELIB_HPP

#include "FileDialog.hpp"

enum class NodeType
{
    None = 0,
    Generic, // TODO change to core module?
    CoreIn, // maybe
    CoreOut // maybe
};

const std::vector<std::string> portTypeNames
{
    "Generic",
    "Int",
    "Float",
    "Double",
    "Vector",
    "Image",
    "Text"
};

enum class PortDataType
{
    Generic,
    Int,
    Float,
    Double,
    Vector,
    Image,
    Text
    // Warning! If you update this, update port type names vector.
};

enum class PortType
{
    None,   // None.
    In,     // Input
    Ic,     // Initial condition
    Out     // Output
};

class CoreNodeLib
{
public:
    CoreNodeLib();
    virtual ~CoreNodeLib() = default;
    struct Node
    {
        struct Port
        {
            std::string name;
            PortType type;
            PortDataType dataType;
            std::string note;
        };
        int id = 0;
        std::string name;
        NodeType type;
        ImColor color;
        std::vector<Port> inputs;
        std::vector<Port> outputs;
    };

    void Draw();
    Node* GetSelectedLeaf() const { return selectedLeaf; }
    bool IsLeafClicked() const { return leafClicked; }
    void SetLeafClickedFalse() { leafClicked = false; }
    
private:
    int iSelectedLeaf = -1;
    int iSelectedBranch = -1;
    std::string nameSelectedLeaf;
    std::string nameSelectedBranch;
    Node* selectedLeaf = nullptr;
    bool leafClicked = false;
    void DrawBranch(const std::string& branchName, int branchId, std::vector<CoreNodeLib::Node>& branchVec);

    std::vector<Node> libMath;
    void MathGain();
    void MathAbs();

    std::vector<Node> libTest;
    void Test();
    void GreatNode();
    void Node1();
    void Node2();
    void NiceNode();
    void NicerNode();
    void LongNode();
    void Sink();
    void Source();
};

#endif /* CORENODELIB_HPP */