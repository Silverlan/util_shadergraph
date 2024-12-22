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

import :nodes.combine_xyz;

using namespace pragma::shadergraph;

CombineXyzNode::CombineXyzNode(const std::string_view &type) : Node {type}
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
