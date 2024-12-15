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
#include <sharedutils/util_case_insensitive_hash.hpp>
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

GraphNode::GraphNode(Graph &graph, const GraphNode &other) : graph {graph}, node {other.node}, m_name {other.m_name}, m_displayName {other.m_displayName}, nodeIndex {other.nodeIndex}, inputs {other.inputs}, outputs {other.outputs} {}
GraphNode::GraphNode(Graph &graph, Node &node) : graph {graph}, node {node}
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
void GraphNode::Relink(const std::string_view &outputName, GraphNode &newNode, const std::string_view &newNodeOutputName)
{
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		throw std::runtime_error("Invalid output index");
	auto newNodeOutputIdx = newNode.node.FindOutputIndex(newNodeOutputName);
	if(!newNodeOutputIdx)
		throw std::runtime_error("Invalid new node output index");
	Relink(*outputIdx, newNode, *newNodeOutputIdx);
}
void GraphNode::Relink(uint32_t outputIdx, GraphNode &newNode, uint32_t newNodeOutputIdx)
{
	if(outputIdx >= outputs.size())
		throw std::runtime_error("Invalid output index");
	auto &output = outputs[outputIdx];
	if(newNodeOutputIdx >= newNode.outputs.size())
		throw std::runtime_error("Invalid new node output index");
	auto &newOutput = newNode.outputs[newNodeOutputIdx];
	if(output.GetSocket().type != newOutput.GetSocket().type)
		throw std::runtime_error("Output types do not match");
	struct OutputInfo {
		GraphNode &linkTarget;
		uint32_t inputIndex;
	};
	std::vector<OutputInfo> outputInfos;
	outputInfos.reserve(output.links.size());
	for(auto link : output.links)
		outputInfos.push_back({*link->parent, link->inputIndex});
	DisconnectOutputs(outputIdx);
	for(auto &oi : outputInfos)
		newNode.Link(newNodeOutputIdx, oi.linkTarget, oi.inputIndex);
}
void GraphNode::PropagateInputSocket(const std::string_view &inputName, GraphNode &otherNode, const std::string_view &otherNodeInputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		throw std::runtime_error("Invalid input index");
	auto otherInputIdx = otherNode.node.FindInputIndex(otherNodeInputName);
	if(!otherInputIdx)
		throw std::runtime_error("Invalid other node input index");
	PropagateInputSocket(*inputIdx, otherNode, *otherInputIdx);
}
void GraphNode::PropagateInputSocket(uint32_t inputIdx, GraphNode &otherNode, uint32_t otherNodeInputIdx)
{
	if(inputIdx >= inputs.size())
		throw std::runtime_error("Invalid input index");
	auto &input = inputs[inputIdx];
	if(otherNodeInputIdx >= otherNode.inputs.size())
		throw std::runtime_error("Invalid other node input index");
	auto &otherInput = otherNode.inputs[otherNodeInputIdx];
	if(input.GetSocket().type != otherInput.GetSocket().type)
		throw std::runtime_error("Input types do not match");
	if(input.link) {
		input.link->parent->Link(input.link->outputIndex, otherNode, otherNodeInputIdx);
		return;
	}
	visit(input.GetSocket().type, [&input, &otherInput](auto tag) {
		using T = typename decltype(tag)::type;
		T val;
		if(input.GetValue(val))
			otherInput.SetValue(val);
	});
}
void GraphNode::DisconnectInputs()
{
	auto numInputs = inputs.size();
	for(size_t i = 0; i < numInputs; ++i)
		Disconnect(i);
}
void GraphNode::DisconnectOutputs()
{
	for(size_t i = 0; i < outputs.size(); ++i)
		DisconnectOutputs(i);
}
void GraphNode::DisconnectOutputs(uint32_t outputIdx)
{
	if(outputIdx >= outputs.size())
		return;
	auto &output = outputs.at(outputIdx);
	while(!output.links.empty()) {
		auto *link = output.links.front();
		auto n = output.links.size();
		auto &socketName = link->GetSocket().name;
		auto res = link->parent->Disconnect(socketName);
		if(!res || output.links.size() >= n)
			throw std::runtime_error {"Failed to disconnect output to input socket '" + socketName + "'!"};
	}
}
void GraphNode::DisconnectAll()
{
	DisconnectInputs();
	DisconnectOutputs();
}
bool GraphNode::Disconnect(uint32_t inputIdx)
{
	auto &input = inputs[inputIdx];
	if(!input.link)
		return false;
	auto it = std::find(input.link->links.begin(), input.link->links.end(), &input);
	assert(it != input.link->links.end());
	input.link->links.erase(it);
	input.link = nullptr;
	return true;
}
bool GraphNode::Disconnect(const std::string_view &inputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	return Disconnect(*inputIdx);
}
bool GraphNode::Disconnect(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx)
{
	if(outputIdx >= outputs.size())
		return false;
	auto &output = outputs.at(outputIdx);
	auto it = std::find_if(output.links.begin(), output.links.end(), [&linkTarget, &inputIdx](const InputSocket *input) { return input->parent == &linkTarget && input->inputIndex == inputIdx; });
	if(it == output.links.end())
		return false;
	auto &link = *it;
	return link->parent->Disconnect(inputIdx);
}
bool GraphNode::Disconnect(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName)
{
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		return false;
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	return Disconnect(*outputIdx, linkTarget, *inputIdx);
}
bool GraphNode::CanLink(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx) const
{
	// TODO: Check if types are compatible
	return true;
}
bool GraphNode::CanLink(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName) const
{
	if(&linkTarget == this)
		return false;
	// TODO: Check if types are compatible
	return true;
}
bool GraphNode::Link(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx, std::string *optOutErr)
{
	if(inputIdx >= linkTarget.inputs.size()) {
		if(optOutErr)
			*optOutErr = "No input at index " + std::to_string(inputIdx) + "!";
		return false;
	}
	if(outputIdx >= outputs.size()) {
		if(optOutErr)
			*optOutErr = "No output at index " + std::to_string(outputIdx) + "!";
		return false;
	}
	if(!CanLink(outputIdx, linkTarget, inputIdx)) {
		if(optOutErr)
			*optOutErr = "Incompatible socket types!";
		return false;
	}
	Disconnect(outputIdx, linkTarget, inputIdx);

	auto &input = linkTarget.inputs[inputIdx];
	auto &output = outputs[outputIdx];
	output.links.push_back(&input);

	input.link = &output;
	return true;
}
bool GraphNode::Link(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName, std::string *optOutErr)
{
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx) {
		if(optOutErr)
			*optOutErr = "No output named '" + std::string {outputName} + "' exists!";
		return false;
	}
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx) {
		if(optOutErr)
			*optOutErr = "No input named '" + std::string {inputName} + "' exists!";
		return false;
	}
	return Link(*outputIdx, linkTarget, *inputIdx, optOutErr);
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

std::string GraphNode::GetBaseVarName() const { return "var" + std::to_string(nodeIndex); }
std::string GraphNode::GetVarName(const std::string &var) const { return GetBaseVarName() + "_" + var; }
std::string GraphNode::GetOutputVarName(size_t outputIdx) const { return GetVarName(std::to_string(outputIdx)); }
std::string GraphNode::GetOutputVarName(const std::string_view &name) const
{
	auto it = std::find_if(outputs.begin(), outputs.end(), [&name](const OutputSocket &output) { return output.GetSocket().name == name; });
	if(it == outputs.end())
		throw std::invalid_argument {"No output named '" + std::string {name} + "' exists!"};
	return GetOutputVarName(it - outputs.begin());
}

bool GraphNode::IsInputLinked(uint32_t inputIdx) const
{
	auto &input = inputs.at(inputIdx);
	return input.link && input.link->parent;
}

bool GraphNode::IsInputLinked(const std::string_view &name) const
{
	auto &inputs = node.GetInputs();
	auto it = std::find_if(inputs.begin(), inputs.end(), [&name](const Socket &socket) { return socket.name == name; });
	if(it == inputs.end())
		throw std::invalid_argument {"No input named '" + std::string {name} + "' exists!"};
	return IsInputLinked(it - inputs.begin());
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
