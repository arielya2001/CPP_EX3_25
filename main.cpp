#include "Game.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace coup;

void forceTurn(Game& game, Player* p) {
    game.set_turn_to(p);
}

int main() {
    Game game;

    // יצירת שחקנים עם כל התפקידים
    Governor* governor = new Governor(game, "Gili");
    Spy* spy = new Spy(game, "Shai");
    Baron* baron = new Baron(game, "Tamar");
    General* general = new General(game, "Noam");
    Judge* judge = new Judge(game, "Dana");
    Merchant* merchant = new Merchant(game, "Lior");

    // הוספת השחקנים למשחק
    game.add_player(governor);
    game.add_player(spy);
    game.add_player(baron);
    game.add_player(general);
    game.add_player(judge);
    game.add_player(merchant);

    // הדגמה: כופים תור לפי צורך, מבצעים פעולה, והתור ימשיך לבד
    forceTurn(game, governor);
    governor->tax();

    forceTurn(game, spy);
    cout << "Spy sees " << baron->name() << " has " << spy->spy_on(*baron) << " coins\n";

    forceTurn(game, baron);
    baron->invest(); // צריך 3 מטבעות, נניח יש לו

    forceTurn(game, general);
    general->gather();

    forceTurn(game, judge);
    judge->gather();

    forceTurn(game, merchant);
    merchant->gather(); // תור ראשון

    forceTurn(game, merchant);
    merchant->gather(); // תור שני

    forceTurn(game, merchant);
    merchant->gather(); // תור שלישי, אמור לקבל בונוס

    forceTurn(game, general);
    general->arrest(*judge); // אמור לעבוד – שופט מפסיד מטבע או merchant מפעיל תכונה

    forceTurn(game, judge);
    judge->sanction(*baron);

    forceTurn(game, spy);
    spy->block_arrest(*general);

    forceTurn(game, baron);
    baron->bribe(*general); // נותן תור בונוס אלא אם השופט ייחסום

    forceTurn(game, judge);
    judge->block_bribe(*baron);

    // קידום לקראת coup
    forceTurn(game, general);
    general->add_coins(10); // בכוונה – להכין ל־coup
    general->coup(*spy);

    // סיום המשחק – תור רגיל
    while (game.players().size() > 1) {
        string curr = game.turn();
        for (Player* p : game.get_all_players()) {
            if (p->name() == curr && p->is_active()) {
                try {
                    p->gather();
                } catch (...) {}
                break;
            }
        }
    }

    cout << "\n🏆 Winner: " << game.winner() << endl;

    delete governor;
    delete spy;
    delete baron;
    delete general;
    delete judge;
    delete merchant;

    return 0;
}
