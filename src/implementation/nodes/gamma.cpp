// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.gamma;

using namespace pragma::shadergraph;

GammaNode::GammaNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_GAMMA, DataType::Float, 1.f, 0.f, 5.f);

	AddOutput(OUT_COLOR, DataType::Color);
}

std::string GammaNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);
	auto gamma = gn.GetInputNameOrValue(IN_GAMMA);

	auto outVar = gn.GetOutputVarName(OUT_COLOR);
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << ";\n";
	code << "if (" << gamma << " == 0.0f)\n";
	code << "\t" << outVar << " = vec3(1.0f, 1.0f, 1.0f);\n";
	code << "else\n";
	code << "{\n";
	code << "\tif (" << color << ".x > 0.0f)\n";
	code << "\t\t" << color << ".x = pow(" << color << ".x, " << gamma << ");\n";
	code << "\tif (" << color << ".y > 0.0f)\n";
	code << "\t\t" << color << ".y = pow(" << color << ".y, " << gamma << ");\n";
	code << "\tif (" << color << ".z > 0.0f)\n";
	code << "\t\t" << color << ".z = pow(" << color << ".z, " << gamma << ");\n";
	code << "\t" << outVar << " = " << color << ";\n";
	code << "}\n";
	return code.str();
}
