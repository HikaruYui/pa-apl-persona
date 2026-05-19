#ifndef ADMIN_H
#define ADMIN_H

#include "data_global.h"
#include "utils.h"
#include "database.h"

void lihatPersonaUtama();
void cariPersona(vector<persona>* daftarPersona, int* status);
void sortingPersona(vector<persona>* daftarPersona);

void bubbleSortNama(vector<persona>* daftarPersona, bool ascending) {
    int sizePersona = daftarPersona->size();
    bool swapped;
    for (int i = 0; i < sizePersona - 1; i++) {
        swapped = false;
        for (int j = 0; j < sizePersona - i - 1; j++) {
            bool shouldSwap = false;
            if (ascending) {
                shouldSwap = (*daftarPersona)[j].nama > (*daftarPersona)[j + 1].nama;
            } else { 
                shouldSwap = (*daftarPersona)[j].nama < (*daftarPersona)[j + 1].nama;
            }
            if (shouldSwap) {
                persona temp = (*daftarPersona)[j];
                (*daftarPersona)[j] = (*daftarPersona)[j + 1];
                (*daftarPersona)[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void insertionSortLevel(vector<persona>* daftarPersona, bool ascending) {
    int sizePersona = daftarPersona->size();
    for (int i = 1; i < sizePersona; i++) {
        persona key = (*daftarPersona)[i];
        int j = i - 1;

        if (ascending) {
            while (j >= 0 && (*daftarPersona)[j].level > key.level) {
                (*daftarPersona)[j + 1] = (*daftarPersona)[j];
                j--;
            }
        } else {
            while (j >= 0 && (*daftarPersona)[j].level < key.level) { 
                (*daftarPersona)[j + 1] = (*daftarPersona)[j];
                j--;
            }
        }
        (*daftarPersona)[j + 1] = key;
    }
}

void sortingPersona(vector<persona>* daftarPersona) {
    cout << "\npilih jenis sorting : " << endl;
    cout << "1. berdasarkan nama(A-Z)" << endl;
    cout << "2. berdasarkan nama(Z-A)" << endl;
    cout << "3. berdasarkan level(terendah ke tertinggi)" << endl;
    cout << "4. berdasarkan level(tertinggi ke terendah)" << endl;

    int pilihan = cekInteger("pilihan : ");

    switch (pilihan) {
        case 1:
            bubbleSortNama(daftarPersona, true);
            break;
        case 2:
            bubbleSortNama(daftarPersona, false);
            break;
        case 3:
            insertionSortLevel(daftarPersona, true);
            break;
        case 4:
            insertionSortLevel(daftarPersona, false);
            break;
        default:
            cout << "pilihan sorting tidak valid" << endl;
            break;
    }
}

void lihatPersonaUtama() {
    if (personaUtama.empty()) {
        cout << "Tidak ada data persona" << endl;
        return;
    }

    Table table;
    table.add_row({"No", "Nama", "Level", "Arcana", "Harga", "Skill"});

    for (int i = 0; i < (int)personaUtama.size(); i++) {
        table.add_row({
            to_string(i + 1),
            personaUtama[i].nama,
            to_string(personaUtama[i].level),
            personaUtama[i].arcana,
            to_string(personaUtama[i].harga),
            gabungSkill(personaUtama[i].skills)
        });
    }

    cout << table << endl;
}

void cariPersona(vector<persona>* daftarPersona, int* status) { 
    cout << "Pilih jenis pencarian : " << endl;
    cout << "1. berdasarkan nama" << endl;
    cout << "2. berdasarkan level" << endl;

    int pilihan = cekInteger("pilihan : ");

    switch (pilihan) {
        case 1: {
            string pilihanNama = cekString("Masukkan nama persona: ");

            transform(pilihanNama.begin(), pilihanNama.end(), pilihanNama.begin(),
                      [](unsigned char c) {
                          return tolower(c);
                      });

            bool ketemu = false;

            Table table;

            if (*status == 1) {
                table.add_row({"No", "Nama", "Level", "Arcana", "Harga", "Skill"});
            } else {
                table.add_row({"No", "Nama", "Level", "Arcana", "Skill"});
            }

            for (int i = 0; i < (int)daftarPersona->size(); i++) {
                string namaLower = (*daftarPersona)[i].nama;

                transform(namaLower.begin(), namaLower.end(), namaLower.begin(),
                          [](unsigned char c) {
                              return tolower(c);
                          });

                if (namaLower.find(pilihanNama) != string::npos) {
                    ketemu = true;

                    if (*status == 1) {
                        table.add_row({
                            to_string(i + 1),
                            (*daftarPersona)[i].nama,
                            to_string((*daftarPersona)[i].level),
                            (*daftarPersona)[i].arcana,
                            to_string((*daftarPersona)[i].harga),
                            gabungSkill((*daftarPersona)[i].skills)
                        });
                    } else {
                        table.add_row({
                            to_string(i + 1),
                            (*daftarPersona)[i].nama,
                            to_string((*daftarPersona)[i].level),
                            (*daftarPersona)[i].arcana,
                            gabungSkill((*daftarPersona)[i].skills)
                        });
                    }
                }
            }

            if (ketemu) {
                cout << "\nData ditemukan!" << endl;
                cout << table << endl;
            } else {
                cout << "Tidak ada nama persona yang cocok" << endl;
            }

            break;
        }

        case 2: { 
            int pilihanLevel = cekInteger("masukkan level Persona yang dicari : ");
            bool levelKetemu = false;

            Table table;

            if (*status == 1) {
                table.add_row({"No", "Nama", "Level", "Arcana", "Harga", "Skill"});
            } else {
                table.add_row({"No", "Nama", "Level", "Arcana", "Skill"});
            }

            for (int i = 0; i < (int)daftarPersona->size(); i++) {
                if (pilihanLevel == (*daftarPersona)[i].level) {
                    levelKetemu = true;

                    if (*status == 1) {
                        table.add_row({
                            to_string(i + 1),
                            (*daftarPersona)[i].nama,
                            to_string((*daftarPersona)[i].level),
                            (*daftarPersona)[i].arcana,
                            to_string((*daftarPersona)[i].harga),
                            gabungSkill((*daftarPersona)[i].skills)
                        });
                    } else {
                        table.add_row({
                            to_string(i + 1),
                            (*daftarPersona)[i].nama,
                            to_string((*daftarPersona)[i].level),
                            (*daftarPersona)[i].arcana,
                            gabungSkill((*daftarPersona)[i].skills)
                        });
                    }
                }
            }

            if (levelKetemu) {
                cout << "\nData ditemukan!" << endl;
                cout << table << endl;
            } else {
                cout << "Tidak ada level persona yang sama" << endl;
            }

            break; 
        } 

        default: {
            cout << "pilihan searching tidak valid" << endl;
            break;
        }
    }
}

void tambahPersona(MYSQL* conn) {
    persona newP;

    // Default untuk persona baru.
    // Persona yang ditambah admin dianggap bukan special dan bukan special material.
    newP.isSpecial = false;
    newP.isSpecialMaterial = false;

    newP.nama = cekString("Masukkan nama persona: ");

    if (cekNamaPersona(newP.nama)) {
        cout << "Gagal: Nama persona sudah ada!" << endl;
        return;
    }

    newP.level = cekInteger("masukkan level persona : ");
    newP.arcana = cekString("Silakan masukkan arcana persona: ", true);

    while (newP.skills.size() < 4) {
        string inputSkill = cekString("Skill " + to_string(newP.skills.size() + 1) + ": ");

        if (inputSkill == "stop") {
            if (newP.skills.size() >= 2) {
                break;
            } else {
                cout << "Minimal skill harus 2!" << endl;
                continue;
            }
        }

        if (cekSkillPersona(newP.skills, inputSkill)) {
            cout << "Skill tersebut sudah ada di persona ini, abaikan." << endl;
        } else {
            newP.skills.push_back(inputSkill);
        }
    }

    newP.harga = cekInteger("masukkan harga persona : ");

    // Mulai transaksi karena tambah persona mengubah tabel persona dan persona_skills.
    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    // Ambil id arcana. Kalau arcana belum ada, createArcana() akan membuatnya.
    int arcanaId = createArcana(conn, newP.arcana);
    if (arcanaId == -1) {
        mysql_query(conn, "ROLLBACK");
        cout << "Gagal Menambah Persona" << endl;
        return;
    }

    const char* insertPersonaQuery =
        "INSERT INTO persona (nama, level, harga, isSpecial, arcana_id, isSpecialMaterial) "
        "VALUES (?, ?, ?, ?, ?, ?)";

    MYSQL_STMT* insertPersonaStmt = mysql_stmt_init(conn);

    if (insertPersonaStmt == NULL) {
        cout << "Gagal init insert persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(insertPersonaStmt, insertPersonaQuery, strlen(insertPersonaQuery))) {
        cout << "Gagal prepare insert persona: " << mysql_stmt_error(insertPersonaStmt) << endl;
        mysql_stmt_close(insertPersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND personaParam[6];
    memset(personaParam, 0, sizeof(personaParam));

    unsigned long namaLength = newP.nama.length();

    int isSpecial = 0;
    int isSpecialMaterial = 0;

    // Parameter 1: nama
    personaParam[0].buffer_type = MYSQL_TYPE_STRING;
    personaParam[0].buffer = (char*)newP.nama.c_str();
    personaParam[0].buffer_length = newP.nama.length();
    personaParam[0].length = &namaLength;

    // Parameter 2: level
    personaParam[1].buffer_type = MYSQL_TYPE_LONG;
    personaParam[1].buffer = &newP.level;

    // Parameter 3: harga
    personaParam[2].buffer_type = MYSQL_TYPE_LONG;
    personaParam[2].buffer = &newP.harga;

    // Parameter 4: isSpecial
    personaParam[3].buffer_type = MYSQL_TYPE_LONG;
    personaParam[3].buffer = &isSpecial;

    // Parameter 5: arcana_id
    personaParam[4].buffer_type = MYSQL_TYPE_LONG;
    personaParam[4].buffer = &arcanaId;

    // Parameter 6: isSpecialMaterial
    personaParam[5].buffer_type = MYSQL_TYPE_LONG;
    personaParam[5].buffer = &isSpecialMaterial;

    if (mysql_stmt_bind_param(insertPersonaStmt, personaParam)) {
        cout << "Gagal bind insert persona: " << mysql_stmt_error(insertPersonaStmt) << endl;
        mysql_stmt_close(insertPersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(insertPersonaStmt)) {
        cout << "Gagal Menambah Persona: " << mysql_stmt_error(insertPersonaStmt) << endl;
        mysql_stmt_close(insertPersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    int personaId = (int)mysql_stmt_insert_id(insertPersonaStmt);
    mysql_stmt_close(insertPersonaStmt);

    newP.id = personaId;
    newP.original_id = personaId;

    // Menyimpan skill persona ke tabel persona_skills.
    for (int i = 0; i < (int)newP.skills.size(); i++) {
        string skill = newP.skills[i];

        int skillId = getOrCreateSkill(conn, skill);

        if (skillId == -1) {
            mysql_query(conn, "ROLLBACK");
            cout << "Gagal Menambah Persona" << endl;
            return;
        }

        const char* insertSkillQuery =
            "INSERT INTO persona_skills (persona_id, skill_id) VALUES (?, ?)";

        MYSQL_STMT* insertSkillStmt = mysql_stmt_init(conn);

        if (insertSkillStmt == NULL) {
            cout << "Gagal init insert skill persona: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(insertSkillStmt, insertSkillQuery, strlen(insertSkillQuery))) {
            cout << "Gagal prepare insert skill persona: " << mysql_stmt_error(insertSkillStmt) << endl;
            mysql_stmt_close(insertSkillStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND skillParam[2];
        memset(skillParam, 0, sizeof(skillParam));

        // Parameter 1: persona_id
        skillParam[0].buffer_type = MYSQL_TYPE_LONG;
        skillParam[0].buffer = &personaId;

        // Parameter 2: skill_id
        skillParam[1].buffer_type = MYSQL_TYPE_LONG;
        skillParam[1].buffer = &skillId;

        if (mysql_stmt_bind_param(insertSkillStmt, skillParam)) {
            cout << "Gagal bind insert skill persona: " << mysql_stmt_error(insertSkillStmt) << endl;
            mysql_stmt_close(insertSkillStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(insertSkillStmt)) {
            cout << "Gagal Menambah Skill Persona: " << mysql_stmt_error(insertSkillStmt) << endl;
            mysql_stmt_close(insertSkillStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(insertSkillStmt);
    }

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    personaUtama.push_back(newP);

    cout << "Data persona berhasil dimasukkan ke database!" << endl;
}

void updatePersona(MYSQL* conn) {
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa diubah" << endl;
        return;
    }

    lihatPersonaUtama();

    int index = cekInteger("Silakan masukkan nomor persona yang ingin diubah: ");

    if (index < 1 || index > (int)personaUtama.size()) {
        cout << "Nomor Tidak Valid" << endl;
        return;
    }

    int idx = index - 1;
    int personaId = personaUtama[idx].id;
    int pilihan;

    do {
        cout << "\n--- Update persona: " << personaUtama[idx].nama << " ---" << endl;
        cout << "1. Ubah nama persona" << endl;
        cout << "2. Ubah level persona" << endl;
        cout << "3. Ubah arcana persona" << endl;
        cout << "4. Ubah skill persona" << endl;
        cout << "5. Ubah harga persona" << endl;
        cout << "0. Batal update" << endl;

        pilihan = cekInteger("Pilihan: ");

        switch (pilihan) {
            case 1: {
                string namaBaru = cekString("Silakan masukkan nama baru: ");

                if (cekNamaPersona(namaBaru, idx)) {
                    cout << "Gagal: Nama persona sudah ada!" << endl;
                    break;
                }

                const char* query =
                    "UPDATE persona SET nama = ? WHERE id = ?";

                MYSQL_STMT* stmt = mysql_stmt_init(conn);

                if (stmt == NULL) {
                    cout << "Gagal init update nama: " << mysql_error(conn) << endl;
                    break;
                }

                if (mysql_stmt_prepare(stmt, query, strlen(query))) {
                    cout << "Gagal prepare update nama: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                MYSQL_BIND param[2];
                memset(param, 0, sizeof(param));

                unsigned long namaLength = namaBaru.length();

                // Parameter 1: nama baru
                param[0].buffer_type = MYSQL_TYPE_STRING;
                param[0].buffer = (char*)namaBaru.c_str();
                param[0].buffer_length = namaBaru.length();
                param[0].length = &namaLength;

                // Parameter 2: id persona
                param[1].buffer_type = MYSQL_TYPE_LONG;
                param[1].buffer = &personaId;

                if (mysql_stmt_bind_param(stmt, param)) {
                    cout << "Gagal bind update nama: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                if (mysql_stmt_execute(stmt)) {
                    cout << "Gagal update nama di database: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                mysql_stmt_close(stmt);

                personaUtama[idx].nama = namaBaru;
                cout << "Nama berhasil diubah!" << endl;
                break;
            }

            case 2: {
                int levelBaru = cekInteger("Silakan masukkan level persona: ");

                const char* query =
                    "UPDATE persona SET level = ? WHERE id = ?";

                MYSQL_STMT* stmt = mysql_stmt_init(conn);

                if (stmt == NULL) {
                    cout << "Gagal init update level: " << mysql_error(conn) << endl;
                    break;
                }

                if (mysql_stmt_prepare(stmt, query, strlen(query))) {
                    cout << "Gagal prepare update level: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                MYSQL_BIND param[2];
                memset(param, 0, sizeof(param));

                // Parameter 1: level baru
                param[0].buffer_type = MYSQL_TYPE_LONG;
                param[0].buffer = &levelBaru;

                // Parameter 2: id persona
                param[1].buffer_type = MYSQL_TYPE_LONG;
                param[1].buffer = &personaId;

                if (mysql_stmt_bind_param(stmt, param)) {
                    cout << "Gagal bind update level: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                if (mysql_stmt_execute(stmt)) {
                    cout << "Gagal update level: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                mysql_stmt_close(stmt);

                personaUtama[idx].level = levelBaru;
                cout << "Level berhasil diubah" << endl;
                break;
            }

            case 3: {
                string arcanaBaru = cekString("Silakan masukkan arcana baru: ", true);
                int arcanaId = createArcana(conn, arcanaBaru);

                if (arcanaId == -1) {
                    cout << "Gagal mendapatkan ID arcana" << endl;
                    break;
                }

                const char* query =
                    "UPDATE persona SET arcana_id = ? WHERE id = ?";

                MYSQL_STMT* stmt = mysql_stmt_init(conn);

                if (stmt == NULL) {
                    cout << "Gagal init update arcana: " << mysql_error(conn) << endl;
                    break;
                }

                if (mysql_stmt_prepare(stmt, query, strlen(query))) {
                    cout << "Gagal prepare update arcana: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                MYSQL_BIND param[2];
                memset(param, 0, sizeof(param));

                // Parameter 1: arcana_id baru
                param[0].buffer_type = MYSQL_TYPE_LONG;
                param[0].buffer = &arcanaId;

                // Parameter 2: id persona
                param[1].buffer_type = MYSQL_TYPE_LONG;
                param[1].buffer = &personaId;

                if (mysql_stmt_bind_param(stmt, param)) {
                    cout << "Gagal bind update arcana: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                if (mysql_stmt_execute(stmt)) {
                    cout << "Gagal update arcana: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                mysql_stmt_close(stmt);

                personaUtama[idx].arcana = arcanaBaru;
                cout << "Arcana berhasil diubah!" << endl;
                break;
            }

            case 4: {
                if (personaUtama[idx].skills.empty()) {
                    cout << "Persona ini belum punya skill" << endl;
                    break;
                }

                cout << "\nDaftar skill persona " << personaUtama[idx].nama << ":" << endl;
                for (int i = 0; i < (int)personaUtama[idx].skills.size(); i++) {
                    cout << i + 1 << ". " << personaUtama[idx].skills[i] << endl;
                }

                int nomorSkill = cekInteger("Pilih nomor skill yang ingin diubah: ");

                if (nomorSkill < 1 || nomorSkill > (int)personaUtama[idx].skills.size()) {
                    cout << "Nomor skill tidak valid!" << endl;
                    break;
                }

                int idxSkill = nomorSkill - 1;

                string skillBaru = cekString("Silakan masukkan nama skill baru: ", true);

                if (cekSkillPersona(personaUtama[idx].skills, skillBaru, idxSkill)) {
                    cout << "Gagal: Persona ini sudah punya skill tersebut!" << endl;
                    break;
                }

                int skillBaruId = getOrCreateSkill(conn, skillBaru);

                if (skillBaruId == -1) {
                    cout << "Gagal mendapatkan ID skill baru!" << endl;
                    break;
                }

                string skillLama = personaUtama[idx].skills[idxSkill];
                int skillLamaId = getOrCreateSkill(conn, skillLama);

                if (skillLamaId == -1) {
                    cout << "Gagal mendapatkan ID skill lama!" << endl;
                    break;
                }

                if (mysql_query(conn, "START TRANSACTION")) {
                    cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
                    return;
                }

                const char* query =
                    "UPDATE persona_skills "
                    "SET skill_id = ? "
                    "WHERE persona_id = ? AND skill_id = ? "
                    "LIMIT 1";

                MYSQL_STMT* stmt = mysql_stmt_init(conn);

                if (stmt == NULL) {
                    cout << "Gagal init update skill persona: " << mysql_error(conn) << endl;
                    mysql_query(conn, "ROLLBACK");
                    break;
                }

                if (mysql_stmt_prepare(stmt, query, strlen(query))) {
                    cout << "Gagal prepare update skill persona: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    mysql_query(conn, "ROLLBACK");
                    break;
                }

                MYSQL_BIND param[3];
                memset(param, 0, sizeof(param));

                // Parameter 1: skill baru
                param[0].buffer_type = MYSQL_TYPE_LONG;
                param[0].buffer = &skillBaruId;

                // Parameter 2: id persona
                param[1].buffer_type = MYSQL_TYPE_LONG;
                param[1].buffer = &personaId;

                // Parameter 3: skill lama
                param[2].buffer_type = MYSQL_TYPE_LONG;
                param[2].buffer = &skillLamaId;

                if (mysql_stmt_bind_param(stmt, param)) {
                    cout << "Gagal bind update skill persona: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    mysql_query(conn, "ROLLBACK");
                    break;
                }

                if (mysql_stmt_execute(stmt)) {
                    cout << "Gagal update skill persona: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    mysql_query(conn, "ROLLBACK");
                    break;
                }

                int affectedRows = mysql_stmt_affected_rows(stmt);
                mysql_stmt_close(stmt);

                if (affectedRows == 0) {
                    cout << "Skill lama tidak ditemukan di database." << endl;
                    mysql_query(conn, "ROLLBACK");
                    break;
                }

                if (mysql_query(conn, "COMMIT")) {
                    cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
                    mysql_query(conn, "ROLLBACK");
                    return;
                }

                personaUtama[idx].skills[idxSkill] = skillBaru;

                cout << "Skill persona berhasil diubah!" << endl;
                break;
            }

            case 5: {
                int hargaBaru = cekInteger("Masukkan harga persona baru: ");

                const char* query =
                    "UPDATE persona SET harga = ? WHERE id = ?";

                MYSQL_STMT* stmt = mysql_stmt_init(conn);

                if (stmt == NULL) {
                    cout << "Gagal init update harga: " << mysql_error(conn) << endl;
                    break;
                }

                if (mysql_stmt_prepare(stmt, query, strlen(query))) {
                    cout << "Gagal prepare update harga: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                MYSQL_BIND param[2];
                memset(param, 0, sizeof(param));

                // Parameter 1: harga baru
                param[0].buffer_type = MYSQL_TYPE_LONG;
                param[0].buffer = &hargaBaru;

                // Parameter 2: id persona
                param[1].buffer_type = MYSQL_TYPE_LONG;
                param[1].buffer = &personaId;

                if (mysql_stmt_bind_param(stmt, param)) {
                    cout << "Gagal bind update harga: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                if (mysql_stmt_execute(stmt)) {
                    cout << "Gagal update harga: " << mysql_stmt_error(stmt) << endl;
                    mysql_stmt_close(stmt);
                    break;
                }

                mysql_stmt_close(stmt);

                personaUtama[idx].harga = hargaBaru;
                cout << "Harga berhasil diubah!" << endl;
                break;
            }

            case 0:
                cout << "Batal update" << endl;
                break;

            default:
                cout << "pilihan tidak valid" << endl;
                break;
        }

    } while (pilihan != 0);
}

void hapusPersona(MYSQL* conn) {
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa dihapus" << endl;
        return;
    }

    lihatPersonaUtama();

    int index = cekInteger("Silakan masukkan nomor persona yang ingin dihapus: ");

    if (index < 1 || index > (int)personaUtama.size()) {
        cout << "Tidak ada persona di nomor itu" << endl;
        return;
    }

    int idx = index - 1;
    int personaId = personaUtama[idx].id;

    if (!konfirmasiString("Apakah kamu yakin ingin menghapus persona " + personaUtama[idx].nama + "? y/n: ")) {
        cout << "Penghapusan dibatalkan." << endl;
        return;
    }

    // Mulai transaksi karena hapus persona mengubah lebih dari satu tabel.
    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    // 1. Hapus dulu skill persona dari tabel persona_skills.
    // Karena persona_skills punya persona_id yang mengarah ke tabel persona.
    const char* deleteSkillQuery =
        "DELETE FROM persona_skills WHERE persona_id = ?";

    MYSQL_STMT* deleteSkillStmt = mysql_stmt_init(conn);

    if (deleteSkillStmt == NULL) {
        cout << "Gagal init delete skill persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(deleteSkillStmt, deleteSkillQuery, strlen(deleteSkillQuery))) {
        cout << "Gagal prepare delete skill persona: " << mysql_stmt_error(deleteSkillStmt) << endl;
        mysql_stmt_close(deleteSkillStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND skillParam[1];
    memset(skillParam, 0, sizeof(skillParam));

    // Parameter 1: persona_id
    skillParam[0].buffer_type = MYSQL_TYPE_LONG;
    skillParam[0].buffer = &personaId;

    if (mysql_stmt_bind_param(deleteSkillStmt, skillParam)) {
        cout << "Gagal bind delete skill persona: " << mysql_stmt_error(deleteSkillStmt) << endl;
        mysql_stmt_close(deleteSkillStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(deleteSkillStmt)) {
        cout << "Gagal menghapus skill persona: " << mysql_stmt_error(deleteSkillStmt) << endl;
        mysql_stmt_close(deleteSkillStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(deleteSkillStmt);

    // 2. Setelah skill-nya dihapus, baru hapus data persona utamanya.
    const char* deletePersonaQuery =
        "DELETE FROM persona WHERE id = ?";

    MYSQL_STMT* deletePersonaStmt = mysql_stmt_init(conn);

    if (deletePersonaStmt == NULL) {
        cout << "Gagal init delete persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(deletePersonaStmt, deletePersonaQuery, strlen(deletePersonaQuery))) {
        cout << "Gagal prepare delete persona: " << mysql_stmt_error(deletePersonaStmt) << endl;
        mysql_stmt_close(deletePersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND personaParam[1];
    memset(personaParam, 0, sizeof(personaParam));

    // Parameter 1: id persona
    personaParam[0].buffer_type = MYSQL_TYPE_LONG;
    personaParam[0].buffer = &personaId;

    if (mysql_stmt_bind_param(deletePersonaStmt, personaParam)) {
        cout << "Gagal bind delete persona: " << mysql_stmt_error(deletePersonaStmt) << endl;
        mysql_stmt_close(deletePersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(deletePersonaStmt)) {
        cout << "Gagal menghapus persona: " << mysql_stmt_error(deletePersonaStmt) << endl;
        mysql_stmt_close(deletePersonaStmt);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(deletePersonaStmt);

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    personaUtama.erase(personaUtama.begin() + idx);

    cout << "Persona berhasil dihapus dari database!" << endl;
}

#endif


