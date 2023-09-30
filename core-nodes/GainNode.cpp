/******************************************************************************************
*                                                                                         *
*    Gain Node                                                                            *
*                                                                                         *
*    Copyright (c) 2023 Onur AKIN <https://github.com/onurae>                             *
*    Licensed under the MIT License.                                                      *
*                                                                                         *
******************************************************************************************/

#include "GainNode.hpp"

GainNode::GainNode(const std::string& uniqueName) : CoreNode(uniqueName, "Gain", NodeType::Generic, ImColor(0.2f, 0.3f, 0.6f, 0.0f))
{
    AddInput(CoreNodeInput("Input", PortType::In, PortDataType::Double));
    AddOutput(CoreNodeOutput("Output", PortType::Out, PortDataType::Double));
    BuildGeometry();
}
