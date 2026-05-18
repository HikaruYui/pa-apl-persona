#ifndef AUTH_H
#define AUTH_H

#include "data_global.h"
#include "utils.h"
#include "database.h"

void registerUser(MYSQL* conn) {
    cout << "\n=== Registrasi user baru ===" << endl;
    cout << "masukkan nama user : ";
    
    string namaBaru = cekString("SIlakan masukkan nama anda");
    string passBaru = cekString("SIlakan masukkan password anda");

    if (cekUser(namaBaru)) {
        cout << "nama user sudah terdaftar!" << endl;
        return;
    }

    string namaEsc = escapeSQL(conn, namaBaru);
    string passEsc = escapeSQL(conn, passBaru);

    string query =
        "INSERT INTO users (nama_user, password, uang, status) VALUES ('" +
        namaEsc + "', '" +
        passEsc + "', 10000, 'user')";

    if (mysql_query(conn, query.c_str())) {
        cout << "Gagal registrasi ke database: " << mysql_error(conn) << endl;
        return;
    }

    int userId = (int)mysql_insert_id(conn);

    LevelUser newUser;
    newUser.id = userId;
    newUser.nama = namaBaru;
    newUser.password = passBaru;
    newUser.uang = 10000;
    newUser.status = 2;

    users.push_back(newUser);

    cout << "registrasi berhasil dan masuk ke database!" << endl;
}

bool login(int& userIndex) {
    int percobaan = 1;
    
    while (percobaan <= 3) {
        cout << "\n--- Menu Login ---" << endl;
        string inputNama = cekString("Masukkan nama (atau 0 untuk keluar): ");

        if (inputNama == "0") {
            return false;
        }

        string inputPass = cekString("Masukkan password: ");

        userIndex = cariIndexUser(inputNama);

        if (userIndex != -1 && users[userIndex].password == inputPass) {
            cout << "selamat datang " << users[userIndex].nama << endl;
            return true;
        } else {
            cout << "nama atau password salah, coba lagi " << endl;
            percobaan++;
        }
    }

    cout << "Terlalu banyak mencoba login." << endl;
    return false;
}

#endif