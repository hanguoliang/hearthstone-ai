#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardType T> class ZoneChanger;
	}
}

namespace State
{
	class Deck
	{
	public:
		class LocationManipulator
		{
			template <Entity::CardType T> friend class Manipulators::Helpers::ZoneChanger;

		public:
			explicit LocationManipulator(Deck & deck) : deck_(deck) {}

		private:
			template <typename T1, typename T2>
			void Insert(T1&& mgr, T2&& card_ref)
			{
				++deck_.change_id;
				auto instance = Utils::OrderedCardsManager(deck_.cards_);
				return instance.Insert(std::forward<T1>(mgr), std::forward<T2>(card_ref));
			}
			template <typename T1, typename T2>
			void Remove(T1&& mgr, T2&& pos)
			{
				++deck_.change_id;
				auto instance = Utils::OrderedCardsManager(deck_.cards_);
				return instance.Remove(std::forward<T1>(mgr), std::forward<T2>(pos));
			}

		private:
			Deck & deck_;
		};
		friend class LocationManipulator;

	public:
		Deck() : change_id(0)
		{
			cards_.reserve(40);
		}

		LocationManipulator GetLocationManipulator()
		{
			return LocationManipulator(*this);
		}

		size_t Size() const { return cards_.size(); }

	private:
		int change_id;
		std::vector<CardRef> cards_;
	};
}