#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=64&sort=-cost&display=1
// Done

namespace Cards
{
	struct Card_EX1_621 : SpellCardBase<Card_EX1_621> {
		Card_EX1_621() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				auto op = [&](state::CardRef target) {
					context.manipulate_.OnBoardMinion(target).Heal(context.card_ref_, 4);
					return true;
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_EX1_332 : SpellCardBase<Card_EX1_332> {
		Card_EX1_332() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Silence();
			});
		}
	};

	template <int v> struct Card_CS1_129e : Enchantment<Card_CS1_129e<v>, SetAttack<v>> {};
	struct Card_CS1_129 : SpellCardBase<Card_CS1_129> {
		Card_CS1_129() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				int hp = context.manipulate_.GetCard(context.GetTarget()).GetHP();
				context.manipulate_.OnBoardMinion(context.GetTarget())
					.Enchant().Add<Card_CS1_129e>(hp);
			});
		}
	};

	struct Card_EX1_341 : MinionCardBase<Card_EX1_341> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnStart::Context const& context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			auto targets = TargetsGenerator(owner).Ally().Damaged();
			auto target_ref = context.manipulate_.GetRandomTarget(targets.GetInfo());
			if (!target_ref.IsValid()) return true;
			context.manipulate_.OnBoardCharacter(target_ref).Heal(self, 3);
			return true;
		}
		Card_EX1_341() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnStart>();
		}
	};

	struct Card_EX1_625t : public HeroPowerCardBase<Card_EX1_625t> {
		Card_EX1_625t() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget())
					.Damage(context.card_ref_, 2);
			});
		}
	};
	struct Card_EX1_625t2 : public HeroPowerCardBase<Card_EX1_625t2> {
		Card_EX1_625t2() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget())
					.Damage(context.card_ref_, 3);
			});
		}
	};
	struct Card_EX1_625 : SpellCardBase<Card_EX1_625> {
		Card_EX1_625() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::PlayerIdentifier owner = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
				
				state::CardRef exist_heropower_ref = context.manipulate_.Board().Player(owner).GetHeroPowerRef();
				Cards::CardId exist_heropower_id = context.manipulate_.GetCard(exist_heropower_ref).GetCardId();
				if (exist_heropower_id == Cards::ID_EX1_625t) {
					context.manipulate_.Player(owner).ReplaceHeroPower(Cards::ID_EX1_625t2);
				}
				else {
					context.manipulate_.Player(owner).ReplaceHeroPower(Cards::ID_EX1_625t);
				}
			});
		}
	};

	struct Card_EX1_339 : SpellCardBase<Card_EX1_339> {
		Card_EX1_339() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				int max_steal_cards = (int)context.manipulate_.Board().Player(context.player_).hand_.LeftSpaces();

				if (max_steal_cards == 0) return;
				else if (max_steal_cards == 1) {
					context.manipulate_.Player(context.player_).AddHandCard(
						context.manipulate_.Player(context.player_).GetOneRandomDeckCard()
					);
				}
				else {
					std::pair<Cards::CardId, Cards::CardId> steal_cards_id = 
						context.manipulate_.Player(context.player_).GetTwoRandomDeckCards();
					context.manipulate_.Player(context.player_).AddHandCard(steal_cards_id.first);
					context.manipulate_.Player(context.player_).AddHandCard(steal_cards_id.second);
				}
			});
		}
	};

	struct Card_EX1_626 : SpellCardBase<Card_EX1_626> {
		Card_EX1_626() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Silence();
					return true;
				});
				context.manipulate_.Player(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_345 : SpellCardBase<Card_EX1_345> {
		Card_EX1_345() {
			onplay_handler.SetCheckPlayableCallback([](Contexts::CheckPlayable & context) {
				if (context.state_.GetBoard().Get(context.player_).minions_.Full()) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				std::vector<Cards::CardId> possibles;
				context.manipulate_.Board().Player(context.player_).deck_.ForEach([&](Cards::CardId in_card_id) {
					if (Cards::CardDispatcher::CreateInstance(in_card_id).card_type == state::kCardTypeMinion) {
						possibles.push_back(in_card_id);
					}
					return true;
				});

				Cards::CardId summon_id;
				if (possibles.empty()) summon_id = Cards::ID_EX1_345t;
				else {
					summon_id = possibles[context.manipulate_.GetRandom().Get(possibles.size())];
				}
				SummonToRightmost(context.manipulate_, context.player_, summon_id);
			});
		}
	};

	template <state::PlayerSide player>
	struct Card_EX1_334e : EnchantmentForThisTurn<Card_EX1_334e<player>, SetPlayer<player>, Charge> {};
	// Note: Temporary grant charge this turn
	// The temporary enchantment from this card allows stolen minions to attack immediately, by granting them Charge.
	// Example: You cast Shadow Madness on an enemy Warsong Commander.
	// Its ongoing effect grants itself + 1 Attack for the rest of the turn.
	// Reference: http://hearthstone.gamepedia.com/Shadow_Madness

	struct Card_EX1_334 : SpellCardBase<Card_EX1_334> {
		Card_EX1_334() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Enemy().Minion();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target_ref = context.GetTarget();
				if (!target_ref.IsValid()) return;
				context.manipulate_.OnBoardMinion(target_ref).Enchant().Add<Card_EX1_334e>(context.player_);
			});
		}
	};

	struct Card_EX1_591 : MinionCardBase<Card_EX1_591> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CalculateHealDamageAmount::Context const& context) {
			if (*context.amount_ >= 0) return true;

			state::PlayerIdentifier owner = context.manipulate_.Board().GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& source_card = context.manipulate_.GetCard(context.source_ref_);
			if (source_card.GetPlayerIdentifier() != owner) return true; // for friendly only
			assert(*context.amount_ < 0);
			*context.amount_ = -*context.amount_; // change healing to damaging
			return true;
		};

		Card_EX1_591() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::CalculateHealDamageAmount>();
		}
	};

	struct Card_EX1_335o : EnchantmentCardBase {
		static constexpr EnchantmentTiers aura_tier = EnchantmentTiers::kEnchantmentTier3; // always apply at the last stage
		Card_EX1_335o() {
			apply_functor = [](engine::FlowControl::enchantment::Enchantments::ApplyFunctorContext const& context) {
				context.stats_->attack = context.state_.GetCard(context.card_ref_).GetHP();
			};
			force_update_every_time = true;
		}
	};
	struct Card_EX1_335 : MinionCardBase<Card_EX1_335> {
		static auto GetAuraTargets(engine::FlowControl::aura::contexts::AuraGetTargets const& context) {
			context.new_targets.push_back(context.card_ref_);
		}
		Card_EX1_335() {
			Aura<Card_EX1_335o, EmitWhenAlive, engine::FlowControl::aura::kUpdateOnlyFirstTime>();
		}
	};

	struct Card_EX1_624 : SpellCardBase<Card_EX1_624> {
		Card_EX1_624() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 5);
				context.manipulate_.OnBoardCharacter(
					context.manipulate_.Board().Player(context.player_).GetHeroRef()
				).Heal(context.card_ref_, 5);
			});
		}
	};

	struct Card_EX1_091 : MinionCardBase<Card_EX1_091> {
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.manipulate_.GetRandomTarget(
				TargetsGenerator(context.player_).Enemy().Minion().AttackLessOrEqualTo(2).GetInfo());
			if (!target.IsValid()) return;

			context.manipulate_.OnBoardMinion(target).ChangeOwner(context.player_);
		}
	};

	struct Card_EX1_623e : Enchantment<Card_EX1_623e, MaxHP<3>> {};
	struct Card_EX1_623 : MinionCardBase<Card_EX1_623> {
		static void GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_).Ally().Minion().Exclude(context.card_ref_);
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			if (!context.GetTarget().IsValid()) return;
			Manipulate(context).OnBoardMinion(context.GetTarget()).Enchant().Add<Card_EX1_623e>();
		}
	};

	struct Card_EX1_350 : MinionCardBase<Card_EX1_350> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CalculateHealDamageAmount::Context const& context) {
			state::PlayerIdentifier owner = context.manipulate_.Board().GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& source_card = context.manipulate_.GetCard(context.source_ref_);
			if (source_card.GetPlayerIdentifier() != owner) return true; // for friendly only
			if (source_card.GetCardType() == state::kCardTypeSpell ||
				source_card.GetCardType() == state::kCardTypeHeroPower)
			{
				*context.amount_ *= 2; // both healing and damaging. Positive for damaging; nagitive for healing
			}
			return true;
		};

		Card_EX1_350() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::CalculateHealDamageAmount>();
		}
	};
}

REGISTER_CARD(EX1_350)
REGISTER_CARD(EX1_623)
REGISTER_CARD(EX1_091)
REGISTER_CARD(EX1_624)
REGISTER_CARD(EX1_335)
REGISTER_CARD(EX1_591)
REGISTER_CARD(EX1_334)
REGISTER_CARD(EX1_345)
REGISTER_CARD(EX1_626)
REGISTER_CARD(EX1_339)
REGISTER_CARD(EX1_625)
REGISTER_CARD(EX1_625t)
REGISTER_CARD(EX1_625t2)
REGISTER_CARD(EX1_341)
REGISTER_CARD(CS1_129)
REGISTER_CARD(EX1_332)
REGISTER_CARD(EX1_621)