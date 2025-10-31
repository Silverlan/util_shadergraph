// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


module pragma.shadergraph;

import :nodes.combine_xyz;

using namespace pragma::shadergraph;

CombineXyzNode::CombineXyzNode(const std::string_view &type) : Node {type, CATEGORY_VECTOR_MATH}
{
	AddInput(IN_X, DataType::Float, 0.f);
	AddInput(IN_Y, DataType::Float, 0.f);
	AddInput(IN_Z, DataType::Float, 0.f);

	AddOutput(OUT_VECTOR, DataType::Vector);
}

std::string CombineXyzNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	code << gn.GetGlslOutputDeclaration(OUT_VECTOR) << " = ";
	auto x = gn.GetInputNameOrValue(IN_X);
	auto y = gn.GetInputNameOrValue(IN_Y);
	auto z = gn.GetInputNameOrValue(IN_Z);
	code << "vec3(" << x << ", " << y << ", " << z << ");\n";
	return code.str();
}
