// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <cinttypes>
#include <vector>
#include <optional>
#include <unordered_map>
#include <sstream>
#include <cassert>

export module pragma.shadergraph:nodes.math;

import :node;

export namespace pragma::shadergraph {

	class MathNode : public Node {
	  public:
		enum class Operation : uint32_t {
			Add = 0,
			Subtract,
			Multiply,
			Divide,
			Sine,
			Cosine,
			Tangent,
			ArcSine,
			ArcCosine,
			ArcTangent,
			Power,
			Logarithm,
			Minimum,
			Maximum,
			Round,
			LessThan,
			GreaterThan,
			Modulo,
			Absolute,
			ArcTan2,
			Floor,
			Ceil,
			Fraction,
			Sqrt,
			InverseSqrt,
			Sign,
			Exponent,
			Radians,
			Degrees,
			SinH,
			CosH,
			TanH,
			Trunc,
			Snap,
			Wrap,
			Compare,
			MultiplyAdd,
			PingPong,
			SmoothMin,
			SmoothMax,
			FlooredModulo,
		};

		static constexpr const char *IN_OPERATION = "operation";
		static constexpr const char *IN_CLAMP = "clamp";
		static constexpr const char *IN_VALUE1 = "value1";
		static constexpr const char *IN_VALUE2 = "value2";
		static constexpr const char *IN_VALUE3 = "value3";

		static constexpr const char *OUT_VALUE = "value";

		MathNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
