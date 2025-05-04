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

        // ייתכן שתוסיף מתודות נוספות כמו remove_player, get_index וכו'
    };
}
