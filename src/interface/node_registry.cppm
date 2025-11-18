// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.shadergraph:node_registry;

import :node;

export namespace pragma::shadergraph {
	class NodeRegistry {
	  public:
		NodeRegistry();
		NodeRegistry(const NodeRegistry &) = delete;
		NodeRegistry &operator=(const NodeRegistry &) = delete;
		template<typename T>
		void RegisterNode(const std::string_view &name)
		{
			RegisterNode(std::make_shared<T>(name));
		}
		template<typename T>
		void RegisterNode(const std::shared_ptr<T> &node)
		{
			RegisterNode(std::string {node->GetType()}, node);
		}
		const std::shared_ptr<Node> GetNode(const std::string &name) const;
		void GetNodeTypes(std::vector<std::string> &outNames) const;
		void AddChildRegistry(const std::shared_ptr<NodeRegistry> &registry) { m_childRegistries.push_back(registry); }
		const std::vector<std::shared_ptr<NodeRegistry>> &GetChildRegistries() const { return m_childRegistries; }
	  private:
		void RegisterNode(const std::string &name, const std::shared_ptr<Node> &node);
		std::unordered_map<std::string, std::shared_ptr<Node>> m_nodes;
		std::vector<std::shared_ptr<NodeRegistry>> m_childRegistries;
	};
};
