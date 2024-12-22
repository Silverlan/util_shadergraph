/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <sstream>

module pragma.shadergraph;

import :nodes.math;

using namespace pragma::shadergraph;

MathNode::MathNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<Operation>(IN_OPERATION, Operation::Add);
	AddInput(IN_CLAMP, DataType::Boolean, false);
	AddInput(IN_VALUE1, DataType::Float, 0.5f);
	AddInput(IN_VALUE2, DataType::Float, 0.5f);
	AddInput(IN_VALUE3, DataType::Float, 0.f);

	AddOutput(OUT_VALUE, DataType::Float);

	AddModuleDependency("math");
}

std::string MathNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	code << gn.GetGlslOutputDeclaration(OUT_VALUE) << " = ";
	auto v1 = gn.GetInputNameOrValue(IN_VALUE1);
	auto v2 = gn.GetInputNameOrValue(IN_VALUE2);
	auto v3 = gn.GetInputNameOrValue(IN_VALUE3);
	auto op = *gn.GetConstantInputValue<Operation>(IN_OPERATION);
	switch(op) {
	case Operation::Add:
		code << v1 << " + " << v2;
		break;
	case Operation::Subtract:
		code << v1 << " - " << v2;
		break;
	case Operation::Multiply:
		code << v1 << " * " << v2;
		break;
	case Operation::Divide:
		code << v1 << " / " << v2;
		break;
	case Operation::MultiplyAdd:
		code << v1 << " * " << v2 << " + " << v3;
		break;
	case Operation::Sine:
		code << "sin(" << v1 << ")";
		break;
	case Operation::Cosine:
		code << "cos(" << v1 << ")";
		break;
	case Operation::Tangent:
		code << "tan(" << v1 << ")";
		break;
	case Operation::SinH:
		code << "sinh(" << v1 << ")";
		break;
	case Operation::CosH:
		code << "cosh(" << v1 << ")";
		break;
	case Operation::TanH:
		code << "tanh(" << v1 << ")";
		break;
	case Operation::ArcSine:
		code << "asin(" << v1 << ")";
		break;
	case Operation::ArcCosine:
		code << "acos(" << v1 << ")";
		break;
	case Operation::ArcTangent:
		code << "atan(" << v1 << ")";
		break;
	case Operation::Power:
		code << "pow(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Logarithm:
		code << "log(" << v1 << ")";
		break;
	case Operation::Minimum:
		code << "min(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Maximum:
		code << "max(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Round:
		code << "round(" << v1 << ")";
		break;
	case Operation::LessThan:
		code << "max(sign(" << v2 << " -" << v1 << "), 0.0)";
		break;
	case Operation::GreaterThan:
		code << "max(sign(" << v1 << " -" << v2 << "), 0.0)";
		break;
	case Operation::Modulo:
		code << "mod(" << v1 << ", " << v2 << ")";
		break;
	case Operation::FlooredModulo:
		code << "floored_modulo(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Absolute:
		code << "abs(" << v1 << ")";
		break;
	case Operation::ArcTan2:
		code << "atan(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Floor:
		code << "floor(" << v1 << ")";
		break;
	case Operation::Ceil:
		code << "ceil(" << v1 << ")";
		break;
	case Operation::Fraction:
		code << "fract(" << v1 << ")";
		break;
	case Operation::Trunc:
		code << "trunc(" << v1 << ")";
		break;
	case Operation::Snap:
		code << "floor(" << v1 << " /" << v2 << ") *" << v2;
		break;
	case Operation::Wrap:
		code << "wrap(" << v1 << ", " << v2 << ", " << v3 << ")";
		break;
	case Operation::PingPong:
		code << "pingpong(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Sqrt:
		code << "sqrt(" << v1 << ")";
		break;
	case Operation::InverseSqrt:
		code << "inversesqrt(" << v1 << ")";
		break;
	case Operation::Sign:
		code << "sign(" << v1 << ")";
		break;
	case Operation::Exponent:
		code << "exp(" << v1 << ")";
		break;
	case Operation::Radians:
		code << "radians(" << v1 << ")";
		break;
	case Operation::Degrees:
		code << "degrees(" << v1 << ")";
		break;
	case Operation::SmoothMin:
		code << "smoothmin(" << v1 << ", " << v2 << ", " << v3 << ")";
		break;
	case Operation::SmoothMax:
		code << "-smoothmin(-" << v1 << ", -" << v2 << ", " << v3 << ")";
		break;
	case Operation::Compare:
		code << "((abs(" << v1 << " -" << v2 << ") <= max(" << v3 << ", FLT_EPSILON))) ? 1.0 : 0.0";
		break;
	}
	code << ";\n";

	auto clamp = gn.GetInputNameOrValue(IN_CLAMP);
	auto constClamp = gn.GetConstantInputValue<bool>(IN_CLAMP);
	if(!constClamp.has_value() || *constClamp) {
		auto outVarName = gn.GetOutputVarName(OUT_VALUE);
		code << outVarName << " = " << clamp << " ? clamp(" << outVarName << ", 0.0, 1.0) : " << outVarName << ";\n";
	}
	return code.str();
}
