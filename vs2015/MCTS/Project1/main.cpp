#include <time.h>
#include <map>
#include <iostream>

#include "game-engine/deck-database.h"
#include "task.h"
#include "decider.h"

void InitializeDeck1(const GameEngine::DeckDatabase &deck_database, GameEngine::Deck &deck)
{
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(233));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
	deck.AddCard(deck_database.GetCard(111));
	deck.AddCard(deck_database.GetCard(211));
	deck.AddCard(deck_database.GetCard(213));
	deck.AddCard(deck_database.GetCard(222));
	deck.AddCard(deck_database.GetCard(231));
}

void InitializeHand1(const GameEngine::DeckDatabase &deck_database, GameEngine::Hand &hand)
{
	hand.AddCard(deck_database.GetCard(111));
	hand.AddCard(deck_database.GetCard(211));
	hand.AddCard(deck_database.GetCard(213));
	hand.AddCard(deck_database.GetCard(222));
	hand.AddCard(deck_database.GetCard(231));
}

void InitializeBoard(GameEngine::Board &board)
{
	GameEngine::DeckDatabase deck_database;

	board.player_stat.hp = 30;
	board.player_stat.armor = 0;
	board.player_stat.crystal.Set(2, 2, 0, 0);

	board.opponent_stat.hp = 30;
	board.opponent_stat.armor = 0;
	board.opponent_stat.crystal.Set(2, 2, 0, 0);

	board.opponent_cards.Set(30);

	InitializeDeck1(deck_database, board.player_deck);
	InitializeHand1(deck_database, board.player_hand);

	GameEngine::Minion minion;
	minion.Set(111, 1, 7, 1);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(213, 3, 2, 3);
	minion.TurnStart();
	board.player_minions.AddMinion(minion);

	minion.Set(333, 2, 3, 3);
	minion.TurnStart();
	board.opponent_minions.AddMinion(minion);

	board.SetStateToPlayerChooseBoardMove();
	//board.SetStateToPlayerTurnStart();
}

static void Run()
{
	constexpr int threads = 4;
	constexpr int sec_each_run = 1;

	MCTS mcts[threads];
	std::vector<Task*> tasks;
	std::map<Task*, Task::PauseNotifier*> pause_notifiers;

	GameEngine::Board board;
	InitializeBoard(board);
	board.DebugPrint();

	for (int i = 0; i < threads; ++i) {
		//mcts[i].Initialize((unsigned int)time(nullptr), board);
		mcts[i].Initialize(i, board);
		Task *new_task = new Task(&mcts[i]);
		new_task->Initialize(std::thread(Task::ThreadCreatedCallback, new_task));
		tasks.push_back(new_task);

		pause_notifiers[new_task] = new Task::PauseNotifier;

		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		new_task->Start(run_until, pause_notifiers[new_task]);
	}

	auto start = std::chrono::steady_clock::now();
	for (int times = 0;; times++)
	{
		MCTS mcts_copy[threads];

		int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

		std::cout << "Waiting... (Elasped " << elapsed_ms << "ms)" << std::endl;
		std::cout.flush();
		for (const auto &task : tasks)
		{
			pause_notifiers[task]->WaitUntilPaused();
		}

		std::cout << "Copying..." << std::endl;
		std::cout.flush();
		for (int i = 0; i < threads; ++i) mcts_copy[i] = mcts[0];

		// start all threads for 10 second
		std::cout << "Resuming..." << std::endl;
		std::cout.flush();
		auto now = std::chrono::steady_clock::now();
		auto run_until = now + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(sec_each_run));
		for (const auto &task : tasks)
		{
			task->Start(run_until, pause_notifiers[task]);
		}

		std::cout << "Merging..." << std::endl;
		std::cout.flush();
		Decider decider(std::move(mcts_copy[0]));
		for (int i = 1; i < threads; ++i) {
			decider.Merge(std::move(mcts_copy[i]));
		}
		decider.DebugPrint();
	}

	for (const auto &task : tasks)
	{
		task->Stop();
	}
	for (const auto &task : tasks)
	{
		task->Join();
	}

	std::cout << "Threads stopped" << std::endl;
}

int main(void)
{
	Run();

	return 0;
}
