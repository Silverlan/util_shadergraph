/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <udm.hpp>
#include <udm_enums.hpp>
#include <sstream>
#include <cassert>

export module pragma.shadergraph:nodes.math;

import :node;
#pragma optimize("", off)
export namespace pragma::shadergraph {

	class MathNode : public Node {
	  public:
		enum class Operation : uint32_t {
			Add = 0,
			Subtract,
			Multiply,
			Divide,
			MultiplyAdd,
			Sine,
			Cosine,
			Tangent,
			SinH,
			CosH,
			TanH,
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
			FlooredModulo,
			Absolute,
			ArcTan2,
			Floor,
			Ceil,
			Fraction,
			Trunc,
			Snap,
			Wrap,
			PingPong,
			Sqrt,
			InverseSqrt,
			Sign,
			Exponent,
			Radians,
			Degrees,
			SmoothMin,
			SmoothMax,
			Compare,
		};

		static constexpr const char *IN_OPERATION = "operation";
		static constexpr const char *IN_CLAMP = "clamp";
		static constexpr const char *IN_VALUE1 = "value1";
		static constexpr const char *IN_VALUE2 = "value2";
		static constexpr const char *IN_VALUE3 = "value3";

		static constexpr const char *OUT_VALUE = "value";

		MathNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const NodeInstance &instance) const override;
	};
};
