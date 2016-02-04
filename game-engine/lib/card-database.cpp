#include <fstream>
#include "json/reader.h"
#include "game-engine/card-database.h"

namespace GameEngine {

	static Card::MinionRace GetMinionRace(Json::Value const& json_card)
	{
		if (!json_card.isMember("race")) return Card::RACE_NORMAL;

		std::string race = json_card["race"].asString();
		if (race == "BEAST") return Card::RACE_BEAST;
		if (race == "DEMON") return Card::RACE_DEMON;
		if (race == "DRAGON") return Card::RACE_DRAGON;
		if (race == "MECHANICAL") return Card::RACE_MECH;
		if (race == "MURLOC") return Card::RACE_MURLOC;
		if (race == "PIRATE") return Card::RACE_PIRATE;
		if (race == "TOTEM") return Card::RACE_TOTEM;

		throw std::runtime_error("unknown race");
	}

	static void ProcessMinionCardMechanics(Json::Value const& json_card, Card & new_card)
	{
		if (!json_card.isMember("mechanics")) return;

		Json::Value json_mechanics = json_card["mechanics"];

		if (json_mechanics.isArray() == false) return;

		for (auto const& json_mechanic: json_mechanics)
		{
			std::string mechanic = json_mechanic.asString();
			if (mechanic == "TAUNT") {
				new_card.data.minion.taunt = true;
			}
		}
	}

	CardDatabase::CardDatabase()
	{
		this->Clear();
	}

	bool CardDatabase::ReadFromJsonFile(std::string const & filepath)
	{
		Json::Reader reader;
		Json::Value cards_json;

		std::ifstream cards_file(filepath);

		if (reader.parse(cards_file, cards_json, false) == false) return false;

		return this->ReadFromJson(cards_json);
	}

	bool CardDatabase::ReadFromJson(Json::Value const & cards_json)
	{
		this->Clear();

		if (cards_json.isArray() == false) return false;

		for (auto const& card_json : cards_json)
		{
			try {
				this->AddCard(card_json);
			}
			catch (std::exception const&) {
				continue; // ignore error
			}
		}

		return true;
	}

	bool CardDatabase::AddCard(Json::Value const & card)
	{
		std::string type = card["type"].asString();

		if (type == "MINION") {
			return this->AddMinionCard(card);
		}
		else if (type == "SPELL") {
			return this->AddSpellCard(card);
		}
		else {
			// ignored
			return true;
		}
	}

	bool CardDatabase::AddMinionCard(Json::Value const & json_card)
	{
		std::string origin_id = json_card["id"].asString();

		Card new_card;
		new_card.type = Card::TYPE_MINION;
		new_card.cost = json_card["cost"].asInt();
		new_card.data.minion.attack = json_card["attack"].asInt();
		new_card.data.minion.hp = json_card["health"].asInt();
		new_card.data.minion.race = GetMinionRace(json_card);

		ProcessMinionCardMechanics(json_card, new_card);

		new_card.id = this->GetAvailableCardId();

		this->AddCard(new_card, origin_id);
		return true;
	}

	bool CardDatabase::AddSpellCard(Json::Value const & json_card)
	{
		std::string origin_id = json_card["id"].asString();

		Card new_card;
		new_card.type = Card::TYPE_SPELL;
		new_card.cost = json_card["cost"].asInt();
		new_card.id = this->GetAvailableCardId();

		this->AddCard(new_card, origin_id);
		return true;
	}

	void GameEngine::CardDatabase::AddCard(Card const & card, std::string const& origin_id)
	{
		this->cards[card.id] = card;
		this->origin_id_map[origin_id] = card.id;
	}

	void CardDatabase::Clear()
	{
		this->cards.clear();
		this->next_card_id = 1;
	}

	int CardDatabase::GetAvailableCardId()
	{
		int id = this->next_card_id;
		++this->next_card_id;
		return id;
	}

	Card GameEngine::CardDatabase::GetCard(int card_id) const
	{
		auto it = this->cards.find(card_id);
		if (it == this->cards.end())
		{
			Card ret;
			ret.MarkInvalid();
			return ret;
		}

		return it->second;
	}

	std::unordered_map<std::string, int> const & CardDatabase::GetOriginalIdMap() const
	{
		return this->origin_id_map;
	}

	int CardDatabase::GetCardIdFromOriginalId(std::string const & origin_id) const
	{
		auto it = this->origin_id_map.find(origin_id);
		if (it == this->origin_id_map.end()) return -1;
		return it->second;
	}

} // namespace