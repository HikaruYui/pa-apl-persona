#ifndef AUTH_H
#define AUTH_H

#include "data_global.h"
#include "utils.h"
#include "database.h"
#include <cstring>

void registerUser(MYSQL* conn) {
    cout << "\n=== Registrasi user baru ===" << endl;
    
    string namaBaru = cekString("Silakan masukkan nama anda: ");
    string passBaru = cekString("Silakan masukkan password anda: ");

    if (cekUser(namaBaru)) {
        cout << "nama user sudah terdaftar!" << endl;
        return;
    }

    // Query memakai placeholder (?) agar input user tidak ditempel langsung ke query.
    const char* query =
        "INSERT INTO users (nama_user, password, uang, status) "
        "VALUES (?, ?, ?, ?)";

    // Membuat prepared statement.
    MYSQL_STMT* stmt = mysql_stmt_init(conn);

    if (stmt == NULL) {
        cout << "Gagal init statement register: " << mysql_error(conn) << endl;
        return;
    }

    // Menyiapkan query agar bisa menerima parameter.
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        cout << "Gagal prepare register: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    // Data default user baru.
    int uangAwal = 50000;
    string statusUser = "user";

    // Panjang string dibutuhkan oleh MySQL untuk tipe MYSQL_TYPE_STRING.
    unsigned long namaLength = namaBaru.length();
    unsigned long passLength = passBaru.length();
    unsigned long statusLength = statusUser.length();

    MYSQL_BIND param[4];
    memset(param, 0, sizeof(param));

    // Parameter 1: nama_user
    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (char*)namaBaru.c_str();
    param[0].buffer_length = namaBaru.length();
    param[0].length = &namaLength;

    // Parameter 2: password
    param[1].buffer_type = MYSQL_TYPE_STRING;
    param[1].buffer = (char*)passBaru.c_str();
    param[1].buffer_length = passBaru.length();
    param[1].length = &passLength;

    // Parameter 3: uang
    param[2].buffer_type = MYSQL_TYPE_LONG;
    param[2].buffer = &uangAwal;

    // Parameter 4: status
    param[3].buffer_type = MYSQL_TYPE_STRING;
    param[3].buffer = (char*)statusUser.c_str();
    param[3].buffer_length = statusUser.length();
    param[3].length = &statusLength;

    // Menghubungkan semua parameter dengan placeholder (?) di query.
    if (mysql_stmt_bind_param(stmt, param)) {
        cout << "Gagal bind register: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    // Menjalankan INSERT.
    if (mysql_stmt_execute(stmt)) {
        cout << "Gagal registrasi ke database: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    // Ambil id user yang baru dibuat.
    int userId = (int)mysql_stmt_insert_id(stmt);

    // Statement ditutup setelah query selesai.
    mysql_stmt_close(stmt);

    LevelUser newUser;
    newUser.id = userId;
    newUser.nama = namaBaru;
    newUser.password = passBaru;
    newUser.uang = uangAwal;
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