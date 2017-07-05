#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/builder/EpisodeState.h"
#include "MCTS/Statistic.h"
#include "MCTS/Types.h"
#include "MCTS/builder/TreeUpdater.h"
#include "MCTS/board/Board.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	namespace builder
	{
		// Traverse and build a game tree in a monte-carlo fashion
		// To support share-node and eliminate random-nodes, we actually build
		// a flatten structure using hash table
		class TreeBuilder
		{
		public:
			typedef selection::TreeNode TreeNode;

			TreeBuilder(Statistic<> & statistic) : action_parameter_getter_(*this), random_generator_(*this),
				turn_start_node_(nullptr), statistic_(statistic)
			{
			}

			void TurnStart(TreeNode* node) { turn_start_node_ = node; }

			struct SelectResult
			{
				Result result; // Never returns kResultInvalid (automatically retry)
				bool new_node_created; // only valid if started in selection stage
				TreeNode * node; // only valid if started in selection stage

				SelectResult(Result new_result) :
					result(new_result),
					new_node_created(false),
					node(nullptr)
				{}
			};
			
			// Note: can only be called when current player is the viewer of 'board'
			SelectResult PerformSelect(TreeNode * node, board::Board & board, TreeUpdater * updater);

			// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
			// Note: can only be called when current player is the viewer of 'board'
			Result PerformSimulate(board::Board & board);

		private:
			template <typename StageHandler>
			Result ApplyAction(
				board::BoardActionAnalyzer & action_analyzer,
				StageHandler&& stage_handler);

		public: // for callbacks: action-parameter-getter and random-generator
			int ChooseAction(ActionType action_type, board::ActionChoicesGetter const& choices_getter);

		private:
			board::ActionParameterGetter action_parameter_getter_;
			board::RandomGenerator random_generator_;

			TreeNode* turn_start_node_;
			EpisodeState episode_state_;
			Statistic<> & statistic_;

			selection::Selection selection_stage_;
			simulation::Simulation simulation_stage_;
		};
	}
}