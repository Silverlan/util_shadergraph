/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <unordered_map>
#include <udm.hpp>
#include <udm_types.hpp>
#include <queue>
#include <stdexcept>
#include <iostream>

export module pragma.shadergraph:node_registry;

import :node;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	class NodeRegistry {
	  public:
		NodeRegistry();
		template<typename T>
		void RegisterNode(const std::string_view &name)
		{
			RegisterNode(std::string {name}, std::make_shared<T>(name));
		}
		const std::shared_ptr<Node> GetNode(const std::string &name) const;
	  private:
		void RegisterNode(const std::string &name, const std::shared_ptr<Node> &node);
		std::unordered_map<std::string, std::shared_ptr<Node>> m_nodes;
	};
};
