/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.map_range_node;

using namespace pragma::shadergraph;

MapRangeNode::MapRangeNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<Type>(CONST_TYPE, Type::Linear);

	AddInput(IN_VALUE, SocketType::Float, 1.f);
	AddInput(IN_FROM_MIN, SocketType::Float, 0.f);
	AddInput(IN_FROM_MAX, SocketType::Float, 1.f);
	AddInput(IN_TO_MIN, SocketType::Float, 0.f);
	AddInput(IN_TO_MAX, SocketType::Float, 1.f);
	AddInput(IN_STEPS, SocketType::Float, 4.f);
	AddInput(IN_CLAMP, SocketType::Boolean, false);

	AddOutput(OUT_RESULT, SocketType::Float);
}

void MapRangeNode::Expand(Graph &graph, GraphNode &gn) const
{
	auto clamp = graph.AddNode("clamp");
	gn.Relink(OUT_RESULT, *clamp, ClampNode::OUT_RESULT);
	gn.Link(OUT_RESULT, *clamp, ClampNode::IN_VALUE);
	gn.PropagateInputSocket(IN_TO_MIN, *clamp, ClampNode::IN_MIN);
	gn.PropagateInputSocket(IN_TO_MAX, *clamp, ClampNode::IN_MAX);
}

std::string MapRangeNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;

	auto value = gn.GetInputNameOrValue(IN_VALUE);
	auto fromMin = gn.GetInputNameOrValue(IN_FROM_MIN);
	auto fromMax = gn.GetInputNameOrValue(IN_FROM_MAX);
	auto toMin = gn.GetInputNameOrValue(IN_TO_MIN);
	auto toMax = gn.GetInputNameOrValue(IN_TO_MAX);
	auto steps = gn.GetInputNameOrValue(IN_STEPS);
	auto clamp = gn.GetInputNameOrValue(IN_CLAMP);

	auto type = *gn.GetConstantInputValue<Type>(CONST_TYPE);
	code << "if(compare(" << fromMax << ", " << fromMin << ")) {\n";
	auto factor = gn.GetVarName("factor");
	code << "\tfloat " << factor << ";\n";
	switch(type) {
	case Type::Linear:
		code << "\t" << factor << " = (" << value << " - " << fromMin << ") / (" << fromMax << " - " << fromMin << ");\n";
		break;
	case Type::Stepped:
		code << "\t" << factor << " = (" << value << " - " << fromMin << ") / (" << fromMax << " - " << fromMin << ");\n";
		code << "\t" << factor << " = (" << steps << " > 0.0) ? floor(" << factor << " * (" << steps << " + 1.0)) / " << steps << " : 0.0;\n";
		break;
	case Type::Smoothstep:
		code << "\t" << factor << " = (" << fromMin << " > " << fromMax << ") ? 1.0 - smoothstep(" << fromMax << ", " << fromMin << ", " << value << ") : smoothstep(" << fromMin << ", " << fromMax << ", " << value << ");\n";
		break;
	case Type::Smootherstep:
		code << "\t" << factor << " = (" << fromMin << " > " << fromMax << ") ? 1.0 - smootherstep(" << fromMax << ", " << fromMin << ", " << value << ") : smootherstep(" << fromMin << ", " << fromMax << ", " << value << ");\n";
		break;
	}

	code << "\t" << gn.GetGlslOutputDeclaration(OUT_RESULT) << " = " << toMin << " + " << factor << " * (" << toMax << " - " << toMin << ");\n";

	code << "} else {\n";
	code << gn.GetGlslOutputDeclaration(OUT_RESULT) << " = 0.0;\n";
	code << "}\n";
	return code.str();
}
