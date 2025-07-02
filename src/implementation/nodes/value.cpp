// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.value;

using namespace pragma::shadergraph;

ValueNode::ValueNode(const std::string_view &type) : Node {type, CATEGORY_MATH}
{
	AddSocket(CONST_VALUE, DataType::Float, 0.f);

	AddOutput(OUT_VALUE, DataType::Float);
}

std::string ValueNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto inValue = gn.GetConstantValue(CONST_VALUE);

	code << gn.GetGlslOutputDeclaration(OUT_VALUE) << " = ";
	code << inValue << ";\n";
	return code.str();
}
