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
