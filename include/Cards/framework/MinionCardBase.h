#pragma once

#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"
#include "Cards/FlagAuraHelper.h"
#include "Cards/EventRegister.h"
#include "Cards/BattlecryHelper.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	template <typename T,
		typename SpecifiedCardAttributes1 = NoAttribute,
		typename SpecifiedCardAttributes2 = NoAttribute,
		typename SpecifiedCardAttributes3 = NoAttribute,
		typename SpecifiedCardAttributes4 = NoAttribute,
		typename SpecifiedCardAttributes5 = NoAttribute>
		class MinionCardBase : public state::Cards::CardData, protected MinionCardUtils
	{
	public:
		MinionCardBase()
		{
			this->card_id = CardClassIdMap<T>::id;

			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			assert(data.card_type == state::kCardTypeMinion);

			this->card_type = data.card_type;
			this->card_race = data.card_race;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
			this->enchanted_states.attack = data.attack;
			this->enchanted_states.max_hp = data.max_hp;

			SpecifiedCardAttributes1::Apply(*this);
			SpecifiedCardAttributes2::Apply(*this);
			SpecifiedCardAttributes3::Apply(*this);
			SpecifiedCardAttributes4::Apply(*this);
			SpecifiedCardAttributes5::Apply(*this);

			BattlecryProcessor<T>(*this);
		}

		template <typename... Types>
		auto Aura() { return AuraHelper<T, Types...>(*this); }

		template <typename... Types>
		auto PlayerFlagAura() { return PlayerFlagAuraHelper<T, Types...>(*this); }

		template <typename... Types>
		auto Enrage() { return EnrageHelper<T, Types...>(*this); }

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler = T>
		using RegisteredEventType = OneEventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler>;

		template <typename EventType, typename EventHandler = T>
		using RegisteredManagedEventType = ManagedOneEventRegisterHelper<EventType, EventHandler>;

		template <typename... RegisteredEvents>
		auto RegisterEvents() {
			return EventsRegisterHelper<RegisteredEvents...>::Process((state::Cards::CardData&)*this);
		}

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler = T>
		auto RegisterEvent() {
			return RegisterEvents<RegisteredEventType<LifeTime, SelfPolicy, EventType, EventHandler>>();
		}

		template <typename EventType, typename EventHandler = T>
		auto RegisterEvent() {
			return RegisterEvents<RegisteredManagedEventType<EventType, EventHandler>>();
		}
	};
}