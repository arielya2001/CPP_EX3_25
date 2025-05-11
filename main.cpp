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

int main() {
    Game game;

    // יצירת שחקנים עם כל התפקידים
    Governor governor(game, "Gili");
    Spy spy(game, "Shai");
    Baron baron(game, "Tamar");
    General general(game, "Noam");
    Judge judge(game, "Dana");
    Merchant merchant(game, "Lior");

    // הדגמת פעולות בסיסיות
    governor.tax(); // לוקח 3 מטבעות
    spy.spy_on(baron); // רואה את כמות המטבעות
    baron.invest(); // משקיע 3 ומקבל 6
    general.gather(); // איסוף רגיל
    judge.gather(); // איסוף רגיל
    merchant.gather(); // איסוף רגיל

    // הפעלת סיבוב נוסף להדגמת שוחד
    governor.gather();
    spy.block_arrest(general); // חוסם את גנרל
    baron.gather();
    general.gather();
    judge.gather();
    merchant.gather();

    // merchant מתחיל תור עם 3 מטבעות => מקבל אחד נוסף
    merchant.gather(); // אמור להוסיף עוד אחד חינם

    // הדגמת arrest + block של general
    governor.gather();
    spy.gather();
    baron.gather();
    general.arrest(judge); // גנרל נעצר => מקבל בחזרה את המטבע

    // הדגמת sanction עם Judge ו-Baron
    judge.sanction(baron); // Judge => מטיל סנקציה + גובה עוד מטבע מהשחקן

    // הדגמת שוחד שנחסם ע"י שופט
    merchant.gather();
    governor.gather();
    spy.gather();
    baron.bribe(general); // אמור להעביר תור לשופט
    judge.block_bribe(baron); // שופט חוסם => הברון מפסיד 4 מטבעות

    // הדגמת הפיכה
    general.gather(); // צובר מטבעות
    judge.gather();
    merchant.gather();
    governor.gather();
    spy.gather();
    baron.gather();
    general.gather(); // צובר ל-7
    general.coup(spy); // גנרל מבצע הפיכה על מרגל

    // ניסיון הפיכה על ברון => גנרל חוסם
    judge.gather();
    merchant.gather();
    governor.gather();
    baron.gather();
    general.gather();
    judge.gather();
    merchant.gather();
    general.coup(baron); // ברון עדיין במשחק => גנרל יכול לחסום אם יש לו 5+

    // סיום המשחק עד שנשאר מנצח
    while (game.players().size() > 1) {
        string curr = game.turn();
        for (Player* p : game.get_all_players()) {
            if (p->name() == curr && p->is_active()) {
                try {
                    p->gather();
                } catch (...) {}
            }
        }
    }

    cout << "\nWinner: " << game.winner() << endl;
    return 0;
}
