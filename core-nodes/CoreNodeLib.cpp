/******************************************************************************************
*                                                                                         *
*    Core Node Library                                                                    *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "CoreNodeLib.hpp"

CoreNodeLib::CoreNodeLib()
{
    Test();
    GreatNode();
    Node1();
    Node2();
    NiceNode();
    NicerNode();
    LongNode();
    Sink();
    Source();
}

void CoreNodeLib::Test()
{
    CoreNodeLib::Node node;
    node.id = 1;
    node.name = "Test";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);
    
    node.inputs.push_back({ "Input", PortType::In , PortDataType::Float});
    node.inputs.push_back({ "Force", PortType::In, PortDataType::Int });
    node.inputs.push_back({ "Initial", PortType::Ic, PortDataType::Text });

    node.outputs.push_back({ "Awesome", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Signal", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}

void CoreNodeLib::GreatNode()
{
    CoreNodeLib::Node node;
    node.id = 2;
    node.name = "Great node and it has a very long name";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);
    
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });

    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}

void CoreNodeLib::Node1()
{
    CoreNodeLib::Node node;
    node.id = 3;
    node.name = "Node1";
    node.type = NodeType::Generic;
    node.color = ImColor(0.3f, 0.5f, 0.5f, 0.0f);

    node.inputs.push_back({ "Vector", PortType::In, PortDataType::Vector });
    node.inputs.push_back({ "Image", PortType::In, PortDataType::Image });
    node.inputs.push_back({ "Text", PortType::In, PortDataType::Text });
    node.inputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.inputs.push_back({ "Int", PortType::In, PortDataType::Int });
    node.inputs.push_back({ "Int", PortType::In, PortDataType::Int });
    node.inputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.inputs.push_back({ "Generic", PortType::In, PortDataType::Generic });
    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });
    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });

    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Text", PortType::Out, PortDataType::Text });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Int", PortType::Out, PortDataType::Int });
    nodeVec.push_back(node);
}

void CoreNodeLib::Node2()
{
    CoreNodeLib::Node node;
    node.id = 4;
    node.name = "Node2";
    node.type = NodeType::Generic;
    node.color = ImColor(0.3f, 0.5f, 0.5f, 0.0f);

    node.inputs.push_back({ "Int", PortType::In, PortDataType::Int });
    node.inputs.push_back({ "Text", PortType::In, PortDataType::Text });
    node.inputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });
    node.inputs.push_back({ "Generic", PortType::In, PortDataType::Generic });
    node.inputs.push_back({ "Generic", PortType::In, PortDataType::Generic });

    node.outputs.push_back({ "Vector", PortType::Out, PortDataType::Vector });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Int", PortType::Out, PortDataType::Int });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Image", PortType::Out, PortDataType::Image });
    node.outputs.push_back({ "Text", PortType::Out, PortDataType::Text });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Generic", PortType::Out, PortDataType::Generic });
    nodeVec.push_back(node);
}

void CoreNodeLib::NiceNode()
{
    CoreNodeLib::Node node;
    node.id = 5;
    node.name = "Nice Node";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);
    
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });

    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}

void CoreNodeLib::NicerNode()
{
    CoreNodeLib::Node node;
    node.id = 6;
    node.name = "Nicer Node";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);

    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });

    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}

void CoreNodeLib::LongNode()
{
    CoreNodeLib::Node node;
    node.id = 7;
    node.name = "Long Node";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);

    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Float", PortType::In, PortDataType::Float });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });
    node.inputs.push_back({ "Double", PortType::In, PortDataType::Double });

    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Float", PortType::Out, PortDataType::Float });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    node.outputs.push_back({ "Double", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}

void CoreNodeLib::Sink()
{
    CoreNodeLib::Node node;
    node.id = 8;
    node.name = "Sink";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);

    node.inputs.push_back({ "In", PortType::In, PortDataType::Double });
    node.outputs.push_back({ "", PortType::None, PortDataType::Generic });
    nodeVec.push_back(node);
}

void CoreNodeLib::Source()
{
    CoreNodeLib::Node node;
    node.id = 9;
    node.name = "Source";
    node.type = NodeType::Generic;
    node.color = ImColor(0.2f, 0.3f, 0.6f, 0.0f);

    node.inputs.push_back({ "", PortType::None, PortDataType::Generic });
    node.outputs.push_back({ "Out", PortType::Out, PortDataType::Double });
    nodeVec.push_back(node);
}