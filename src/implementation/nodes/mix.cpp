/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <sstream>
#include <sharedutils/util_string.h>
#include <sharedutils/magic_enum.hpp>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.mix;

using namespace pragma::shadergraph;

MixNode::MixNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<Type>(IN_TYPE, Type::Mix);
	AddInput(IN_FAC, SocketType::Float, 0.5f);
	AddInput(IN_CLAMP, SocketType::Boolean, false);
	AddInput(IN_COLOR1, SocketType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_COLOR2, SocketType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, SocketType::Color);

	AddModuleDependency("mix");
}

std::string MixNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = ";
	auto c1 = gn.GetInputNameOrValue(IN_COLOR1);
	auto c2 = gn.GetInputNameOrValue(IN_COLOR2);
	auto fac = gn.GetInputNameOrValue(IN_FAC);
	auto type = *gn.GetConstantInputValue<Type>(IN_TYPE);
	std::string opName {magic_enum::enum_name(type)};
	opName = ustring::to_snake_case(opName);
	opName += "_color";
	code << opName << "(" << c1 << ", " << c2 << ", " << fac << ");\n";

	auto clamp = gn.GetInputNameOrValue(IN_CLAMP);
	auto constClamp = gn.GetConstantInputValue<bool>(IN_CLAMP);
	if(!constClamp.has_value() || *constClamp) {
		auto outVarName = gn.GetOutputVarName(OUT_COLOR);
		code << outVarName << " = " << clamp << " ? clamp(" << outVarName << ", 0.0, 1.0) : " << outVarName << ";\n";
	}
	return code.str();
}
