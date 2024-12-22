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

import :nodes.separate_xyz;

using namespace pragma::shadergraph;

SeparateXyzNode::SeparateXyzNode(const std::string_view &type) : Node {type}
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
