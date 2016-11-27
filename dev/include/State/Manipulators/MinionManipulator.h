#pragma once

#include "State/Cards/Card.h"
#include "State/Manipulators/Helpers/BasicHelper.h"
#include "State/Manipulators/Helpers/EnchantmentHelper.h"
#include "State/Manipulators/Helpers/AuraHelper.h"
#include "State/Manipulators/Helpers/ZonePositionSetter.h"
#include "State/Manipulators/Helpers/ZoneChanger.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class MinionManipulator
		{
		public:
			MinionManipulator(state::State & state, CardRef card_ref, state::Cards::Card &card) :
				state_(state), card_ref_(card_ref), card_(card)
			{
			}

			void SetCost(int new_cost) { Helpers::BasicHelper::SetCost(card_, new_cost); }

		public:
			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(card_); }
			Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, card_); }
			Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }
			Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion> Zone() {
				return Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, card_ref_, card_);
			}

		private:
			state::State & state_;
			CardRef card_ref_;
			state::Cards::Card & card_;
		};
	}
}