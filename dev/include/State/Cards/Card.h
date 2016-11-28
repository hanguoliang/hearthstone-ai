#pragma once

#include <string>
#include "State/Cards/RawCard.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class EnchantmentHelper;
			class AuraHelper;
			template <state::CardZone T1, state::CardType T2> class ZoneChanger;
			class ZonePositionSetter;
		}
	}
}

namespace state
{
	namespace Cards
	{
		class Card
		{
		public:
			class MutableEnchantmentAuxDataGetter
			{
				friend class FlowControl::Manipulators::Helpers::EnchantmentHelper;
			public:
				MutableEnchantmentAuxDataGetter(RawCard & data) : data_(data) {}
			private:
				EnchantmentAuxData & Get() { return data_.enchantment_aux_data; }
			private:
				RawCard & data_;
			};

			class MutableAuraAuxDataGetter
			{
				friend class FlowControl::Manipulators::Helpers::AuraHelper;
			public:
				MutableAuraAuxDataGetter(RawCard & data) : data_(data) {}
			private:
				AuraAuxData & Get() { return data_.aura_aux_data; }
			private:
				RawCard & data_;
			};

			class LocationSetter
			{
				template <state::CardZone T1, state::CardType T2> friend class FlowControl::Manipulators::Helpers::ZoneChanger;
				friend class FlowControl::Manipulators::Helpers::ZonePositionSetter;

			public:
				LocationSetter(RawCard & data) : data_(data) {}
			private:
				LocationSetter & Player(PlayerIdentifier player)
				{
					data_.enchanted_states.player = player;
					return *this;
				}

				LocationSetter & Zone(CardZone new_zone)
				{
					data_.enchanted_states.zone = new_zone;
					return *this;
				}

				LocationSetter & Position(int pos)
				{
					data_.zone_position = pos;
					return *this;
				}

			private:
				RawCard & data_;
			};

		public:
			explicit Card(const RawCard & data) : data_(data) {}

		public: // getters and setters
			int GetCardId() const { return data_.card_id; }
			CardType GetCardType() const { return data_.card_type; }

			const PlayerIdentifier GetPlayerIdentifier() const { return data_.enchanted_states.player; }
			const CardZone GetZone() const { return data_.enchanted_states.zone; }
			int GetZonePosition() const { return data_.zone_position; }

			int GetCost() const { return data_.enchanted_states.cost; }
			void SetCost(int new_cost) { data_.enchanted_states.cost = new_cost; }

			int GetDamage() const { return data_.damaged; }
			void SetDamage(int new_damage) { data_.damaged = new_damage; }

			int GetHP() const { return data_.enchanted_states.max_hp - data_.damaged; }
			int GetAttack() const { return data_.enchanted_states.attack; }

			MutableEnchantmentAuxDataGetter GetMutableEnchantmentAuxDataGetter()
			{
				return MutableEnchantmentAuxDataGetter(data_);
			}

			MutableAuraAuxDataGetter GetMutableAuraAuxDataGetter()
			{
				return MutableAuraAuxDataGetter(data_);
			}

			LocationSetter SetLocation() { return LocationSetter(data_); }

		public:
			const RawCard & GetRawData() const { return data_; }

		private:
			RawCard data_;
		};
	}
}