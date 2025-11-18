// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.shadergraph;

import :nodes.clamp;

using namespace pragma::shadergraph;

ClampNode::ClampNode(const std::string_view &type) : Node {type, CATEGORY_MATH}
{
	AddSocketEnum<ClampType>(CONST_CLAMP_TYPE, ClampType::MinMax);

	AddInput(IN_VALUE, DataType::Float, 1.f);
	AddInput(IN_MIN, DataType::Float, 0.f);
	AddInput(IN_MAX, DataType::Float, 1.f);

	AddOutput(OUT_RESULT, DataType::Float);
}

std::string ClampNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto value = gn.GetInputNameOrValue(IN_VALUE);
	auto min = gn.GetInputNameOrValue(IN_MIN);
	auto max = gn.GetInputNameOrValue(IN_MAX);
	auto clampType = *gn.GetConstantInputValue<ClampType>(CONST_CLAMP_TYPE);

	code << gn.GetGlslOutputDeclaration(OUT_RESULT) << ";\n";
	auto outVar = gn.GetOutputVarName(OUT_RESULT);

	switch(clampType) {
	case ClampType::MinMax:
		code << outVar << " = clamp(" << value << ", " << min << ", " << max << ");\n";
		break;
	case ClampType::Range:
		code << "if(" << min << " > " << max << ")\n";
		code << "\t" << outVar << " = clamp(" << value << ", " << max << ", " << min << ");\n";
		code << "else\n";
		code << "\t" << outVar << " = clamp(" << value << ", " << min << ", " << max << ");\n";
		break;
	}
	return code.str();
}
