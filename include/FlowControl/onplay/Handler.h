#pragma once

#include "FlowControl/onplay/Contexts.h"

namespace FlowControl
{
	namespace onplay
	{
		class Handler
		{
		public:
			typedef state::targetor::Targets SpecifiedTargetGetter(context::GetSpecifiedTarget const&);
			typedef void OnPlayCallback(context::OnPlay const&);

			Handler() : specified_target_getter(nullptr), onplay(nullptr) {}

			void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
			void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

		public:
			bool PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const;
			void OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const;

		private:
			SpecifiedTargetGetter *specified_target_getter;
			OnPlayCallback *onplay;
		};
	}
}