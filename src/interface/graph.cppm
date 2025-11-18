// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.shadergraph:graph;

import :node;
import :node_registry;
import :graph_node;

export namespace pragma::shadergraph {
	class Graph {
	  public:
		static constexpr auto EXTENSION_BINARY = "psg_b";
		static constexpr auto EXTENSION_ASCII = "psg";

		static constexpr auto PSG_IDENTIFIER = "PSG";
		static constexpr udm::Version PSG_VERSION = 1;

		Graph(const std::shared_ptr<NodeRegistry> &nodeReg);
		Graph(const Graph &other);
		static void Test();
		std::shared_ptr<GraphNode> AddNode(const std::string &type);
		std::shared_ptr<GraphNode> GetNode(const std::string &name);
		std::shared_ptr<GraphNode> FindNodeByType(const std::string_view &type) const;
		bool RemoveNode(const std::string &name);
		const std::vector<std::shared_ptr<GraphNode>> &GetNodes() const { return m_nodes; }
		const std::shared_ptr<NodeRegistry> &GetNodeRegistry() const { return m_nodeRegistry; }
		void Clear();
		void Merge(const Graph &other);
		void DebugPrint();
		void FindInvalidLinks();
		void GenerateGlsl(std::ostream &outHeader, std::ostream &outBody, const std::optional<std::string> &namePrefix = {}) const;
		void Resolve();
		bool Load(udm::LinkedPropertyWrapper &prop, std::string &outErr);
		bool Load(const std::string &filePath, std::string &outErr);
		bool Save(udm::AssetDataArg outData, std::string &outErr) const;
		bool Save(const std::string &filePath, std::string &outErr) const;
	  private:
		void AddNode(const std::shared_ptr<GraphNode> &node);
		void DoGenerateGlsl(std::ostream &outHeader, std::ostream &outBody, const std::optional<std::string> &namePrefix);
		std::vector<GraphNode *> TopologicalSort(const std::vector<std::shared_ptr<GraphNode>> &nodes) const;
		std::shared_ptr<NodeRegistry> m_nodeRegistry;
		std::vector<std::shared_ptr<GraphNode>> m_nodes;
		std::unordered_map<std::string, size_t> m_nameToNodeIndex;
	};
};
