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

export module pragma.shadergraph:graph;

import :node;
import :node_registry;
import :graph_node;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	class Graph {
	  public:
		static constexpr auto PSG_IDENTIFIER = "PSG";
		static constexpr udm::Version PSG_VERSION = 1;

		Graph(const std::shared_ptr<NodeRegistry> &nodeReg);
		static void Test();
		std::shared_ptr<GraphNode> AddNode(const std::string &type);
		std::shared_ptr<GraphNode> GetNode(const std::string &name);
		const std::vector<std::shared_ptr<GraphNode>> &GetNodes() const { return m_nodes; }
		std::vector<GraphNode *> topologicalSort(const std::vector<std::shared_ptr<GraphNode>> &nodes);
		void DebugPrint();
		void FindInvalidLinks();
		std::string GenerateGLSL();
		bool Load(udm::LinkedPropertyWrapper &prop, std::string &outErr);
		bool Save(udm::AssetDataArg outData, std::string &outErr) const;
	  private:
		std::shared_ptr<NodeRegistry> m_nodeRegistry;
		std::vector<std::shared_ptr<GraphNode>> m_nodes;
		std::unordered_map<std::string, size_t> m_nameToNodeIndex;
	};
};
