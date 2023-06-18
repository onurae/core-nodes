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

#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>

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
    const std::vector<Node>& Get() const { return nodeVec; }
private:
    std::vector<Node> nodeVec;
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