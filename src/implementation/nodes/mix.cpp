// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>
#include <sharedutils/util_string.h>
#include <sharedutils/magic_enum.hpp>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.mix;

using namespace pragma::shadergraph;

MixNode::MixNode(const std::string_view &type) : Node {type, CATEGORY_MATH}
{
	AddSocketEnum<Type>(IN_TYPE, Type::Mix);
	AddInput(IN_FAC, DataType::Float, 0.5f);
	AddInput(IN_CLAMP, DataType::Boolean, false);
	AddInput(IN_COLOR1, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_COLOR2, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, DataType::Color);

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
