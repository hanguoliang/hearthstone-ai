#pragma once

#include "game-engine/board.h"
#include "minions.h"

namespace GameEngine {
	inline MinionIterator Minions::InsertBefore(MinionIterator const& it, MinionData && minion)
	{
		auto new_minion = Minion(*this, std::move(minion));

		auto new_it = this->minions.insert(it.GetIterator(), std::move(new_minion));
#ifdef DEBUG
		// this->change_id++; // change id is not changed since we're using a std::list
#endif

		this->GetBoard().hook_manager.HookAfterMinionAdded(*new_it);

		return MinionIterator(this->GetBoard(), *this, new_it);
	}

	inline void Minions::MarkPendingRemoval(MinionIterator const & it)
	{
		if (it.it->minion.pending_removal) return;
		it.it->minion.pending_removal = true;
		this->pending_removal_count++;
	}

	inline void Minions::MarkPendingRemoval(Minion & minion)
	{
		if (minion.minion.pending_removal) return;
		minion.minion.pending_removal = true;
		this->pending_removal_count++;
	}

	inline void Minions::EraseAndGoToNext(MinionIterator & it)
	{
		if (it.it->minion.pending_removal) this->pending_removal_count--;

#ifdef DEBUG
		// check minion can be destructed
		if (!it->enchantments.Empty()) throw std::runtime_error("enchantments should be cleared first");
		if (!it->auras.Empty()) throw std::runtime_error("auras should be cleared first");
#endif

		it.it = this->minions.erase(it.it);
#ifdef DEBUG
		this->change_id++; // TODO: Iterators point to exist minions are still valid; but I'd like to know when this happens
		it.container_change_id = this->change_id;
#endif
	}

	inline void Minions::DestroyBoard()
	{
		for (auto & minion : this->minions) {
			minion.DestroyBoard();
		}
	}

	inline Board & Minions::GetBoard() const
	{
		return this->player.board;
	}

	inline Player & Minions::GetPlayer() const
	{
		return this->player;
	}
} // namespace GameEngine