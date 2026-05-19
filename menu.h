#ifndef MENU_H
#define MENU_H

#include "data_global.h"
#include "utils.h"
#include "database.h"
#include "admin.h"
#include "user.h"

void lihatPersonaUser(personaUser* profilePtr);
void beliPersona(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
void fusePersona(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
void updateSkillUser(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
void hapusPersonaUser(MYSQL* conn, personaUser* profilePtr);
void beliSkill(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
void fusePersonaSpecial(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);

void menuAdmin(LevelUser* userPtr, MYSQL* conn) {
    int status = 1;
    int pilihan;
    do {
        cout << "\n= Menu Velvet Room Admin =" << endl;
        cout << "1. Tampilkan data persona" << endl;
        cout << "2. Cari persona" << endl;
        cout << "3. Tambah persona" << endl;
        cout << "4. Update persona" << endl;
        cout << "5. Hapus persona" << endl;
        cout << "6. sorting persona" << endl;
        cout << "0. Keluar" << endl;
        pilihan = cekInteger("pilihan : ");
        switch (pilihan) {
            case 1: lihatPersonaUtama(); 
                break;
            case 2: cariPersona(&personaUtama, &(userPtr->status)); 
                break;
            case 3: tambahPersona(conn); 
                break;
            case 4: updatePersona(conn); 
                break;
            case 5: hapusPersona(conn); 
                break;
            case 6:
                sortingPersona(&personaUtama);
                break;
            case 0:
                cout << "Logout " << (*userPtr).nama << endl; 
                break;
            default: cout << "pilihan tidak valid." << endl;
        }
    } while (pilihan != 0);
}

void userMenu(MYSQL* conn, int userIndex) { 
    int profilIndex = cariAtauBuatProfil(users[userIndex].nama);

    LevelUser* currentUserPtr = &users[userIndex]; 
    personaUser* currentUserProfilePtr = &profilUser[profilIndex]; 

    loadUserPersonaCollectionFromDB(conn, currentUserPtr, currentUserProfilePtr);

    int pilihan;
    do {
        cout << "\n= Menu Velvet Room User =" << endl;
        cout << "User: " << currentUserPtr->nama << " | Uang: " << currentUserPtr->uang << " yen" << endl; 
        cout << "1. Lihat data persona user" << endl;
        cout << "2. cari persona" << endl;
        cout << "3. Beli persona" << endl;
        cout << "4. Fuse/Gabung persona" << endl;
        cout << "5. Ubah skill persona" << endl;
        cout << "6. sorting persona" << endl;
        cout << "7. Hapus persona" << endl;
        cout << "8. Beli skill item" << endl;
        cout << "9. Special Fusion" << endl;
        cout << "0. Keluar" << endl;
        pilihan = cekInteger("Masukkan pilihan: ");
        switch (pilihan) {

            case 1: lihatPersonaUser(currentUserProfilePtr); 
                break; 
            case 2: cariPersona(&(currentUserProfilePtr->listPersona), &(currentUserPtr->status)); 
                break; 
            case 3: beliPersona(conn, currentUserPtr, currentUserProfilePtr); 
                break; 
            case 4: fusePersona(conn, currentUserPtr, currentUserProfilePtr); 
                break; 
            case 5: updateSkillUser(conn, currentUserPtr, currentUserProfilePtr); 
                break; 
           case 6:
                sortingPersona(&(currentUserProfilePtr->listPersona));
                break;
            case 7: hapusPersonaUser(conn, currentUserProfilePtr); 
                break;
            case 8: beliSkill(conn, currentUserPtr, currentUserProfilePtr); 
                break; 
            case 9: fusePersonaSpecial(conn, currentUserPtr, currentUserProfilePtr); 
                break;
            case 0: cout << "Log out" << endl; 
                break;
            default: cout << "pilihan tidak valid." << endl;
        }
    } while (pilihan != 0);
}

#endif