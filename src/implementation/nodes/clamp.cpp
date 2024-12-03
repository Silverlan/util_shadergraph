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

import :nodes.clamp;

using namespace pragma::shadergraph;

ClampNode::ClampNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<ClampType>(CONST_CLAMP_TYPE, ClampType::MinMax);

	AddInput(IN_VALUE, SocketType::Float, 1.f);
	AddInput(IN_MIN, SocketType::Float, 0.f);
	AddInput(IN_MAX, SocketType::Float, 1.f);

	AddOutput(OUT_RESULT, SocketType::Float);
}

std::string ClampNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto value = gn.GetInputNameOrValue(IN_VALUE);
	auto min = gn.GetInputNameOrValue(IN_MIN);
	auto max = gn.GetInputNameOrValue(IN_MAX);
	auto clampType = *gn.GetConstantInputValue<ClampType>(CONST_CLAMP_TYPE);

	code << gn.GetGlslOutputDeclaration(OUT_RESULT) << ";\n";
	auto outVar = gn.GetOutputVarName(OUT_RESULT);

	switch(clampType) {
	case ClampType::MinMax:
		code << outVar << " = clamp(" << value << ", " << min << ", " << max << ");\n";
		break;
	case ClampType::Range:
		code << "if(" << min << " > " << max << ")\n";
		code << "\t" << outVar << " = clamp(" << value << ", " << max << ", " << min << ");\n";
		code << "else\n";
		code << "\t" << outVar << " = clamp(" << value << ", " << min << ", " << max << ");\n";
		break;
	}
	return code.str();
}
