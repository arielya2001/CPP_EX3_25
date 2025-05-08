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

    };
}
