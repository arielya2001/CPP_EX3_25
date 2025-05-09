#pragma once
#include <vector>
#include <string>

namespace coup {
    class Player;  // קדימה להכרזה – כי Game משתמש במחלקה לפני שהיא מוגדרת

    class Game {
    private:
        std::vector<Player*> players_list;
        size_t turn_index = 0;
        int total_turns = 0;
        bool started = false;
        bool awaiting_bribe_block = false;
        Player* bribing_player = nullptr;
        Player* coup_attacker = nullptr;
        Player* coup_target = nullptr;
        bool awaiting_coup_block = false;

    public:
        Game();

        void add_player(Player* player);
        std::string turn() const;
        std::vector<std::string> players() const;
        std::string winner() const;
        void next_turn();
        const std::vector<Player*>& get_all_players() const;
        int get_player_index(Player* p) const;
        int get_turn_index() const;
        int num_players() const;

        void set_awaiting_bribe_block(bool val) { awaiting_bribe_block = val; }
        bool is_awaiting_bribe_block() const { return awaiting_bribe_block; }

        void set_bribing_player(Player* p) { bribing_player = p; }
        Player* get_bribing_player() const { return bribing_player; }
        void set_turn_to(Player* player);
        int get_total_turns() const { return total_turns; }
        void set_awaiting_coup_block(bool val) { awaiting_coup_block = val; }
        bool is_awaiting_coup_block() const { return awaiting_coup_block; }

        void set_coup_attacker(Player* p) { coup_attacker = p; }
        Player* get_coup_attacker() const { return coup_attacker; }

        void set_coup_target(Player* p) { coup_target = p; }
        Player* get_coup_target() const { return coup_target; }
        void clear_coup_target();




    };
}
