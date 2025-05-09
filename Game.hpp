#pragma once
#include <vector>
#include <string>

namespace coup {
    class Player;  // קדימה להכרזה – כי Game משתמש במחלקה לפני שהיא מוגדרת

    class Game {
    private:
        std::vector<Player*> players_list;
        size_t turn_index = 0;
        bool started = false;
        bool awaiting_bribe_block = false;
        Player* bribing_player = nullptr;

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


    };
}
