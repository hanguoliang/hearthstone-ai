#pragma once

#include "mcts/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace builder
	{
		class TreeUpdater
		{
		public:
			TreeUpdater() : last_node_(nullptr), nodes_() {}

			TreeUpdater(TreeUpdater const&) = delete;
			TreeUpdater & operator=(TreeUpdater const&) = delete;

			void Update(bool credit)
			{
				UpdateChosenTimes();
				UpdateWinRate(credit);
			}

			// Note: should never touch the last_node, since it might be a sink node (win or lose)
			// sink node is at address 0x1 or 0x2
			void PushBackNodes(std::vector<selection::TraversedNodeInfo> & nodes, selection::TreeNode * last_node)
			{
				assert([&]() {
					for (size_t i = 0; i < nodes.size(); ++i) {
						auto const& item = nodes[i];
						
						if (!item.GetNode()) return false;

						// every node should have an edge to the next node,
						if (!item.GetEdgeAddon()) return false;

						if (i > 0) {
							if (nodes[i - 1].GetNode()->GetChildNode(nodes[i - 1].GetChoice())
								!= nodes[i].GetNode()) return false;
						}
					}
					return true;
				}());

				last_node_ = last_node;
				if (HasLastNode()) {
					if (nodes.front().GetNode() != last_node_) {
						nodes_.emplace_back(last_node_);
					}
				}

				std::move(nodes.begin(), nodes.end(), std::back_inserter(nodes_));
				nodes.clear();
			}

			void Clear()
			{
				last_node_ = nullptr;
				nodes_.clear();
			}

		private:
			bool HasLastNode() const {
				if (!last_node_) return false;
				if (last_node_->IsWinNode()) return false;
				return true;
			}

			void UpdateChosenTimes() {
				for (size_t i = 0; i < nodes_.size(); ++i) {
					auto const& item = nodes_[i];

					if (item.GetChoice() >= 0) {
						selection::TreeNodeAddon * next_node_addon = nullptr;
						if ((i + 1) < nodes_.size()) {
							next_node_addon = &nodes_[i + 1].GetNode()->GetAddon();
						}
						else {
							if (HasLastNode()) {
								next_node_addon = &last_node_->GetAddon();
							}
						}

						if (next_node_addon) {
							next_node_addon->leading_nodes.AddLeadingNodes(
								item.GetNode(), item.GetChoice());
						}
					}

					if (item.GetEdgeAddon()) {
						auto & edge_addon = *item.GetEdgeAddon();
						edge_addon.chosen_times++;
					}
				}
			}

			void UpdateWinRate(bool credit) {
				for (auto const& item : nodes_) {
					if (item.GetEdgeAddon()) {
						auto & edge_addon = *item.GetEdgeAddon();
						if (credit) ++edge_addon.credit;
						++edge_addon.total;
					}
				}
			}
		private:
			selection::TreeNode * last_node_;
			std::vector<selection::TraversedNodeInfo> nodes_;
		};
	}
}