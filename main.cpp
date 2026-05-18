#include "data_global.h"
#include "utils.h"
#include "database.h"
#include "admin.h"
#include "auth.h"
#include "user.h"
#include "menu.h"

bool login(int& userIndex);
void registerUser(MYSQL* conn);

int main() {
    MYSQL* conn = connectDB();

    users = loadUsersFromDB(conn);
    personaUtama = loadPersonaFromDB(conn);
    loadSkillShopFromDB(conn);

    cout << "Data berhasil diload dari database." << endl;
    cout << "Jumlah user: " << users.size() << endl;
    cout << "Jumlah persona: " << personaUtama.size() << endl;
    cout << "Jumlah skill: " << skillCardShop.size() << endl;

    int currentUserIndex = -1;

    while (true) {
        cout << "\n=== SELAMAT DATANG DI VELVET ROOM ===" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "0. keluar" << endl;

        int pilihanUtama = cekInteger("pilihan : ");

        if (pilihanUtama == 0) {
            cout << "Terima Kasih Telah Bermain Game Ini" << endl;
            break;
        } 
        else if (pilihanUtama == 1) {
            if (login(currentUserIndex)) {
                LevelUser* loggedUserPtr = &users[currentUserIndex];

                if (loggedUserPtr->status == 1) { 
                    menuAdmin(loggedUserPtr, conn); 
                } 
                else if (loggedUserPtr->status == 2) { 
                    userMenu(conn, currentUserIndex);
                } 
                else {
                    cout << "status user tidak dikenali." << endl;
                }
            }
        } 
        else if (pilihanUtama == 2) {
            registerUser(conn);
        } 
        else {
            cout << "pilihan tidak valid." << endl;
        }
    }

    mysql_close(conn);
    return 0;
}
