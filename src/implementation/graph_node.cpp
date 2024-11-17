/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <cassert>
#include <stdexcept>
#include <udm.hpp>

module pragma.shadergraph;

import :graph_node;

using namespace pragma::shadergraph;
#pragma optimize("", off)
OutputSocket::OutputSocket(GraphNode &node, uint32_t index) : parent {&node}, outputIndex {index} {}
const Socket &OutputSocket::GetSocket() const { return *parent->node.GetOutput(outputIndex); }

InputSocket::InputSocket(GraphNode &node, uint32_t index) : parent {&node}, inputIndex {index}, value {GetSocket().type} {}
InputSocket::~InputSocket() { ClearValue(); }
void InputSocket::ClearValue() { value.Clear(); }
bool InputSocket::HasValue() const { return value; }
const Socket &InputSocket::GetSocket() const { return *parent->node.GetInput(inputIndex); }

GraphNode::GraphNode(Graph &graph, Node &node, const std::string &name) : graph {graph}, node {node}, m_name {name}
{
	auto &nodeInputs = node.GetInputs();
	inputs.reserve(nodeInputs.size());
	for(uint32_t i = 0; i < nodeInputs.size(); ++i)
		inputs.emplace_back(*this, i);

	auto &nodeOutputs = node.GetOutputs();
	outputs.reserve(nodeOutputs.size());
	for(uint32_t i = 0; i < nodeOutputs.size(); ++i)
		outputs.emplace_back(*this, i);
}
std::string GraphNode::GetName() const { return m_name; }
void GraphNode::ClearInputValue(const std::string_view &inputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		return;
	auto &input = inputs[*inputIdx];
	input.ClearValue();
}
bool GraphNode::Disconnect(const std::string_view &inputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	auto &input = inputs[*inputIdx];
	if(!input.link)
		return false;
	auto it = std::find(input.link->links.begin(), input.link->links.end(), &input);
	assert(it != input.link->links.end());
	input.link->links.erase(it);
	input.link = nullptr;
	return true;
}
bool GraphNode::Disconnect(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName)
{
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		return false;
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	auto &output = outputs.at(*outputIdx);
	auto it = std::find_if(output.links.begin(), output.links.end(), [&linkTarget, &inputIdx](const InputSocket *input) { return input->parent == &linkTarget && input->inputIndex == *inputIdx; });
	if(it == output.links.end())
		return false;
	auto &link = *it;
	link->parent->Disconnect(inputName);
	return true;
}
bool GraphNode::CanLink(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName) const
{
	if(&linkTarget == this)
		return false;
	// TODO: Check if types are compatible
	return true;
}
bool GraphNode::Link(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName)
{
	if(!CanLink(outputName, linkTarget, inputName))
		return false;
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		return false;
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	Disconnect(outputName, linkTarget, inputName);

	auto &input = linkTarget.inputs[*inputIdx];
	auto &output = outputs[*outputIdx];
	output.links.push_back(&input);

	input.link = &output;
	return true;
}

std::optional<size_t> GraphNode::FindOutputIndex(const std::string_view &name) const
{
	auto it = std::find_if(outputs.begin(), outputs.end(), [&name](const OutputSocket &output) { return output.GetSocket().name == name; });
	return (it != outputs.end()) ? (it - outputs.begin()) : std::optional<size_t> {};
}
std::optional<size_t> GraphNode::FindInputIndex(const std::string_view &name) const
{
	auto it = std::find_if(inputs.begin(), inputs.end(), [&name](const InputSocket &input) { return input.GetSocket().name == name; });
	return (it != inputs.end()) ? (it - inputs.begin()) : std::optional<size_t> {};
}
InputSocket *GraphNode::FindInput(const std::string_view &name)
{
	auto idx = FindInputIndex(name);
	if(!idx)
		return nullptr;
	return GetInput(*idx);
}
OutputSocket *GraphNode::FindOutput(const std::string_view &name)
{
	auto idx = FindOutputIndex(name);
	if(!idx)
		return nullptr;
	return GetOutput(*idx);
}
InputSocket *GraphNode::GetInput(size_t index) { return (index < inputs.size()) ? &inputs[index] : nullptr; }
OutputSocket *GraphNode::GetOutput(size_t index) { return (index < outputs.size()) ? &outputs[index] : nullptr; }
bool GraphNode::IsOutputLinked(const std::string_view &name) const
{
	auto *output = FindOutput(name);
	if(!output)
		return false;
	return !output->links.empty();
}

std::string GraphNode::GetOutputVarName(size_t outputIdx) const { return "var" + std::to_string(nodeIndex) + "_" + std::to_string(outputIdx); }
std::string GraphNode::GetOutputVarName(const std::string_view &name) const
{
	auto it = std::find_if(outputs.begin(), outputs.end(), [&name](const OutputSocket &output) { return output.GetSocket().name == name; });
	if(it == outputs.end())
		throw std::invalid_argument {"No output named '" + std::string {name} + "' exists!"};
	return GetOutputVarName(it - outputs.begin());
}

bool GraphNode::LoadFromAssetData(udm::LinkedPropertyWrapper &prop, std::vector<SocketLink> &outLinks, std::string &outErr)
{
	prop["displayName"] >> m_displayName;

	auto udmInputs = prop["inputs"];
	auto numInputs = udmInputs.GetSize();
	for(size_t idx = 0; idx < numInputs; ++idx) {
		auto udmInput = udmInputs[idx];
		std::string inputName;
		udmInput["input"] >> inputName;

		auto inputIdx = FindInputIndex(inputName);
		if(!inputIdx)
			return false;
		auto *input = GetInput(*inputIdx);

		auto udmValue = udmInput["value"];
		if(udmValue) {
			auto res = visit(input->GetSocket().type, [input, &udmValue](auto tag) {
				using T = typename decltype(tag)::type;
				T val;
				if(!udmValue(val))
					return false;
				input->SetValue(val);
				return true;
			});
			if(!res)
				return false;
		}

		auto udmLink = udmInput["link"];
		if(udmLink) {
			if(outLinks.size() == outLinks.capacity())
				outLinks.reserve(outLinks.size() * 1.5 + 100);
			outLinks.push_back({});
			auto &link = outLinks.back();
			link.inputSocket = input;
			udmLink["node"] >> link.outputNode;
			udmLink["output"] >> link.outputName;
		}
	}
	return true;
}

bool GraphNode::Save(udm::LinkedPropertyWrapper &prop) const
{
	prop["name"] << m_name;
	prop["type"] << node.GetType();
	prop["displayName"] << m_displayName;

	std::vector<const InputSocket *> assignedInputs;
	assignedInputs.reserve(inputs.size());
	for(auto &input : inputs) {
		if(!input.link && !input.HasValue())
			continue;
		assignedInputs.push_back(&input);
	}
	auto udmInputs = prop.AddArray("inputs", assignedInputs.size());
	for(size_t idx = 0; idx < assignedInputs.size(); ++idx) {
		auto *input = assignedInputs[idx];
		auto udmInput = udmInputs[idx];
		udmInput["input"] = input->GetSocket().name;
		if(input->HasValue()) {
			visit(input->GetSocket().type, [input, &udmInput](auto tag) {
				using T = typename decltype(tag)::type;
				T val;
				if(input->GetValue(val))
					udmInput["value"] = val;
			});
		}

		if(input->link) {
			auto *output = input->link;
			auto udmLink = udmInput["link"];
			udmLink["node"] = output->parent->GetName();
			udmLink["output"] = output->GetSocket().name;
		}
	}
	return true;
}
