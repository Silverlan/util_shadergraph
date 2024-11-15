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
		Graph(const NodeRegistry &nodeReg);
		static void Test();
		std::shared_ptr<NodeInstance> AddNode(const std::string &type);
		std::vector<NodeInstance *> topologicalSort(const std::vector<std::shared_ptr<NodeInstance>> &nodes);
		void DebugPrint();
		void FindInvalidLinks();
		//void ConnectNodes(Node::Ptr outputNode, int outputIndex, Node::Ptr inputNode, int inputIndex) { connections.emplace_back(outputNode, outputIndex, inputNode, inputIndex); }
		std::string GenerateGLSL();
	  private:
		const NodeRegistry &m_nodeRegistry;
		std::vector<std::shared_ptr<NodeInstance>> m_nodes;
		std::unordered_map<std::string, size_t> m_nameToNodeIndex;
	};
};
