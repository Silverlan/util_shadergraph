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

import :nodes.gamma;

using namespace pragma::shadergraph;

GammaNode::GammaNode(const std::string_view &type) : Node {type}
{
	AddInput(IN_COLOR, SocketType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_GAMMA, SocketType::Float, 1.f);

	AddOutput(OUT_COLOR, SocketType::Color);
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
