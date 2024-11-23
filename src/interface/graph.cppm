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
#include <sstream>
#include <iostream>

export module pragma.shadergraph:graph;

import :node;
import :node_registry;
import :graph_node;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	class Graph {
	  public:
		static constexpr auto EXTENSION_BINARY = "psg_b";
		static constexpr auto EXTENSION_ASCII = "psg";

		static constexpr auto PSG_IDENTIFIER = "PSG";
		static constexpr udm::Version PSG_VERSION = 1;

		Graph(const std::shared_ptr<NodeRegistry> &nodeReg);
		static void Test();
		std::shared_ptr<GraphNode> AddNode(const std::string &type);
		std::shared_ptr<GraphNode> GetNode(const std::string &name);
		bool RemoveNode(const std::string &name);
		const std::vector<std::shared_ptr<GraphNode>> &GetNodes() const { return m_nodes; }
		void DebugPrint();
		void FindInvalidLinks();
		void GenerateGlsl(std::ostream &outHeader, std::ostream &outBody) const;
		bool Load(udm::LinkedPropertyWrapper &prop, std::string &outErr);
		bool Load(const std::string &filePath, std::string &outErr);
		bool Save(udm::AssetDataArg outData, std::string &outErr) const;
		bool Save(const std::string &filePath, std::string &outErr) const;
	  private:
		std::vector<GraphNode *> TopologicalSort(const std::vector<std::shared_ptr<GraphNode>> &nodes) const;
		std::shared_ptr<NodeRegistry> m_nodeRegistry;
		std::vector<std::shared_ptr<GraphNode>> m_nodes;
		std::unordered_map<std::string, size_t> m_nameToNodeIndex;
	};
};
