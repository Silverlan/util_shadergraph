/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <sstream>

module pragma.shadergraph;

import :nodes.math;

using namespace pragma::shadergraph;

MathNode::MathNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<Operation>(IN_OPERATION, Operation::Add);
	AddInput(IN_CLAMP, SocketType::Boolean, false);
	AddInput(IN_VALUE1, SocketType::Float, 0.5f);
	AddInput(IN_VALUE2, SocketType::Float, 0.5f);
	AddInput(IN_VALUE3, SocketType::Float, 0.f);
	AddOutput(OUT_VALUE, SocketType::Float);
}

std::string MathNode::DoEvaluate(const Graph &graph, const GraphNode &instance) const
{
	std::ostringstream code;
	// TODO
	code << "float " << instance.GetOutputVarName(OUT_VALUE) << " = ";

	//instance.inputs[0].link
	//instance.inputs[0].inputIndex;
	code << "(" << GetInputNameOrValue(instance, IN_VALUE1) << " ";
	switch(Operation::Add) {
	case Operation::Add:
		code << "+";
		break;
	case Operation::Subtract:
		code << "-";
		break;
	case Operation::Multiply:
		code << "*";
		break;
	case Operation::Divide:
		code << "/";
		break;
	}
	code << " " << GetInputNameOrValue(instance, IN_VALUE2) << ");\n";
	return code.str();
}
