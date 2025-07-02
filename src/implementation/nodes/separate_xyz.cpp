// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.separate_xyz;

using namespace pragma::shadergraph;

SeparateXyzNode::SeparateXyzNode(const std::string_view &type) : Node {type, CATEGORY_VECTOR_MATH}
{
	AddInput(IN_VECTOR, DataType::Vector, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_X, DataType::Float);
	AddOutput(OUT_Y, DataType::Float);
	AddOutput(OUT_Z, DataType::Float);
}

std::string SeparateXyzNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto inVector = gn.GetInputNameOrValue(IN_VECTOR);

	code << gn.GetGlslOutputDeclaration(OUT_X) << " = ";
	code << inVector << ".x;\n";

	code << gn.GetGlslOutputDeclaration(OUT_Y) << " = ";
	code << inVector << ".y;\n";

	code << gn.GetGlslOutputDeclaration(OUT_Z) << " = ";
	code << inVector << ".z;\n";
	return code.str();
}
