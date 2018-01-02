#pragma once

#include <queue>
#include "MCTS/selection/TraversedNodeInfo.h"
#include "MCTS/selection/TreeUpdater.h"
#include "MCTS/Config.h"

namespace mcts
{
	namespace selection
	{
		class TreeUpdater
		{
		public:
			TreeUpdater() : bfs_()
#ifndef NDEBUG
				, should_visits_()
#endif
			{}

			TreeUpdater(TreeUpdater const&) = delete;
			TreeUpdater & operator=(TreeUpdater const&) = delete;

			void Update(std::vector<selection::TraversedNodeInfo> const& nodes, selection::TreeNode * last_node, double credit)
			{
				for (size_t i = 0; i < nodes.size(); ++i) {
					auto const& item = nodes[i];

					if (item.edge_addon_) {
						if constexpr (StaticConfigs::kVirtualLoss != 0) {
							static_assert(StaticConfigs::kVirtualLoss > 0);
							item.edge_addon_->AddTotal(-StaticConfigs::kVirtualLoss); // remove virtual loss
							assert(item.edge_addon_->GetTotal() >= 0);
						}
					}
				}

				TreeLikeUpdateWinRate(nodes, credit);
				//LinearlyUpdateWinRate(nodes, credit);
			}

		private:
			void LinearlyUpdateWinRate(std::vector<selection::TraversedNodeInfo> const& nodes, double credit) {
				for (auto const& item : nodes) {
					auto * edge_addon = item.edge_addon_;
					if (!edge_addon) continue;

					edge_addon->AddTotal(100);
					edge_addon->AddCredit((int)(credit * 100.0));
				}
			}

			void TreeLikeUpdateWinRate(std::vector<selection::TraversedNodeInfo> const& nodes, double credit) {
				if (nodes.empty()) return;

				assert([&](){
					should_visits_.clear();
					for (auto const& item : nodes) {
						if (item.edge_addon_) {
							should_visits_.insert(item.edge_addon_);
						}
					}
					return true;
				}());

				auto it = nodes.rbegin();
				while (it != nodes.rend()) {
					TreeLikeUpdateWinRate(it->node_, it->edge_addon_, credit);

					// skip to next redirect node
					// all intermediate nodes are already updated
					++it;
					while (it != nodes.rend()) {
						if (it->choice_ < 0) break;
						++it;
					}
					if (it == nodes.rend()) break;

					assert(it->edge_addon_ == nullptr);
					++it;
				}

				assert(should_visits_.empty());
			}

			void TreeLikeUpdateWinRate(selection::TreeNode * start_node, EdgeAddon * start_edge, double credit)
			{
				assert(start_node);
				
				assert(bfs_.empty());
				bfs_.push({ start_node, start_edge });

				while (!bfs_.empty()) {
					auto node = bfs_.front().node;
					auto * edge_addon = bfs_.front().edge_addon;
					bfs_.pop();

					assert(edge_addon);
					assert([&]() {
						should_visits_.erase(edge_addon);
						return true;
					}());
					edge_addon->AddTotal(100);
					edge_addon->AddCredit((int)(credit*100.0));

					// use BFS to reduce the lock time
					node->GetAddon().leading_nodes.ForEachLeadingNode(
						[&](selection::TreeNode * leading_node, EdgeAddon *leading_edge)
					{
						// TODO: search for identitical nodes. if found, just update it multiple times. don't need to traverse multiple times
						bfs_.push({ leading_node, leading_edge });
						return true;
					});
				}
			}

		private:
			struct Item {
				TreeNode * node;
				EdgeAddon * edge_addon;
			};
			std::queue<Item> bfs_;

#ifndef NDEBUG
			std::unordered_set<EdgeAddon*> should_visits_;
#endif
		};
	}
}
