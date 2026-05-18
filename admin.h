#ifndef ADMIN_H
#define ADMIN_H

#include "data_global.h"
#include "utils.h"
#include "database.h"

void lihatPersonaUtama();
void cariPersona(vector<persona>* daftarPersona, int* status);
void sortingPersona(vector<persona>* daftarPersona, int* status);

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

void selectionSortHarga(vector<persona>* daftarPersona, bool ascending) {
    int sizePersona = daftarPersona->size();
    for (int i = 0; i < sizePersona - 1; i++) {
        int targetIndex = i;
        for (int j = i + 1; j < sizePersona; j++) {
            if (ascending) {
                if ((*daftarPersona)[j].harga < (*daftarPersona)[targetIndex].harga) {
                    targetIndex = j;
                }
            } else {
                 if ((*daftarPersona)[j].harga > (*daftarPersona)[targetIndex].harga) {
                    targetIndex = j;
                 }
            }
        }
        if (targetIndex != i) {
            persona temp = (*daftarPersona)[i];
            (*daftarPersona)[i] = (*daftarPersona)[targetIndex];
            (*daftarPersona)[targetIndex] = temp;
        }
    }
}

void sortingPersona(vector<persona>* daftarPersona, int* status) {
    cout << "\n pilih jenis sorting : " << endl;
    cout << "1. berdasarkan nama(A-Z)" << endl;
    cout << "2. berdasarkan nama(Z-A)" << endl;
    cout << "3. berdasarkan level(terendah ke tertinggi)" << endl;
    cout << "4. berdasarkan level(tertinggi ke terendah)" << endl;
    if(*status == 1){
      cout << "5. berdasarkan harga(termurah ke termahal)" << endl;
      cout << "6. berdasarkan harga(termahal ke termurah)" << endl;  
    }

    int pilihan = cekInteger("pilihan : ");

    if(*status != 1 && (pilihan == 5 || pilihan == 6)){
        cout << "pilihan tidak valid" << endl;
        return;
    }
        switch (pilihan)
        {
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
        case 5:
            selectionSortHarga(daftarPersona, true);
            break;
        case 6:
            selectionSortHarga(daftarPersona, false);
            break;           
        default:
        cout << "pilihan sorting tidak valid" << endl;
            break;
        }
}

void cariPersona(vector<persona>* daftarPersona, int* status) { 
    cout << "Pilih jenis pencarian : " << endl;
    cout << "1. berdasarkan nama" << endl;
    cout << "2. berdasarkan level" << endl;
    int pilihan = cekInteger("pilihan : ");

    switch (pilihan) {
        case 1: { 
            bubbleSortNama(daftarPersona, true);
            string pilihanNama = cekString("Masukkan nama persona: ");

            std::transform(pilihanNama.begin(), pilihanNama.end(), pilihanNama.begin(),
                           [](unsigned char c){return std::tolower(c); });

            int low = 0;
            int high = daftarPersona->size() - 1;
            bool ketemu = false;

            Table table;

            if (*status == 1) {
                table.add_row({"No", "Nama", "Level", "Arcana", "Harga", "Skill"});
            } else {
                table.add_row({"No", "Nama", "Level", "Arcana", "Skill"});
            }
            
            while (low <= high) {
                int mid = low + (high - low) / 2;

                string midNameLower = (*daftarPersona)[mid].nama;
                std::transform(midNameLower.begin(), midNameLower.end(), midNameLower.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                if (midNameLower == pilihanNama) {
                    ketemu = true;

                    if (*status == 1) {
                        table.add_row({
                            to_string(mid + 1),
                            (*daftarPersona)[mid].nama,
                            to_string((*daftarPersona)[mid].level),
                            (*daftarPersona)[mid].arcana,
                            to_string((*daftarPersona)[mid].harga),
                            gabungSkill((*daftarPersona)[mid].skills)
                        });
                    } else {
                        table.add_row({
                            to_string(mid + 1),
                            (*daftarPersona)[mid].nama,
                            to_string((*daftarPersona)[mid].level),
                            (*daftarPersona)[mid].arcana,
                            gabungSkill((*daftarPersona)[mid].skills)
                        });
                    }

                    break;
                } else if (midNameLower < pilihanNama) {
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }

            if (ketemu) {
                cout << "\nData ditemukan!" << endl;
                cout << table << endl;
            } else {
                cout << "tidak ada nama persona yang sama " << endl;
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
                cout << "tidak ada level persona yang sama" << endl;
            }

            break; 
        } 

        default: {
            cout << "pilihan searching tidak valid" << endl;
            break;
        }
    }
}

void lihatPersonaUtama() {
    if (personaUtama.empty()) {
        cout << "Belum ada Persona" << endl;
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

void tambahPersona(MYSQL* conn) {
    persona newP;

    newP.nama = cekString("Masukkan nama persona: ");

    if (cekNamaPersona(newP.nama)) {
        cout << "Gagal: Nama persona sudah ada!" << endl;
        return;
    }
    newP.level = cekInteger("masukkan level persona : ");
    newP.arcana = cekStringTanpaAngka("Silakan masukkan arcana persona: ");

    while (newP.skills.size() < 4) {
        string inputSkill = cekStringTanpaAngka("Skill " + to_string(newP.skills.size() + 1) + ": ");

        if (inputSkill == "stop") {
            if (newP.skills.size() >= 2) {
                break;
            }
            else {
                cout << "Minimal skill harus 2!" << endl;
                continue;
            }
        }
        if (cekSkillPersona(newP.skills, inputSkill)) {
                cout << "Skill tersebut sudah ada di persona ini, abaikan." << endl;
        }
        else {
            newP.skills.push_back(inputSkill);
        }
    }

    newP.harga = cekInteger("masukkan harga persona : ");
    
    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }
    
    int arcanaId = createArcana(conn, newP.arcana);
    if (arcanaId == -1) {
        mysql_query(conn, "ROLLBACK");
        cout << "Gagal Menambah Persona" << endl;
        return; 
    }

    string namaEsc = escapeSQL(conn, newP.nama);
    string insertPersona = "INSERT INTO persona (nama, level, harga, isSpecial, arcana_id) VALUES ('" + namaEsc + "', " +
                            to_string(newP.level) + ", " +
                            to_string(newP.harga) + ", 0, " +
                            to_string(arcanaId) + ")";
    if (mysql_query(conn, insertPersona.c_str())) {
        cout << "Gagal Menambah Persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    int personaId = (int)mysql_insert_id(conn);
    newP.id = personaId;
    newP.original_id = personaId;

    for (const string& skill : newP.skills) {
        int skillId = getOrCreateSkill(conn, skill);

        if (skillId == -1) {
            mysql_query(conn, "ROLLBACK");
            cout << "Gagal Menambah Persona: " << endl;
            return;
        }

        string insertSkill = "INSERT INTO persona_skills (persona_id, skill_id) VALUES (" +
                              to_string(personaId) + ", " +
                              to_string(skillId) + ")";

        if (mysql_query(conn, insertSkill.c_str())) {
            cout << "Gagal Menambah Skill Persona: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
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

            string namaEsc = escapeSQL(conn, namaBaru);

            string query =
                "UPDATE persona SET nama = '" + namaEsc +
                "' WHERE id = " + to_string(personaId);

            if (mysql_query(conn, query.c_str())) {
                cout << "Gagal update nama di database: " << mysql_error(conn) << endl;
                break;
            }

            personaUtama[idx].nama = namaBaru;
            cout << "Nama berhasil diubah!" << endl;
            break;
        }

        case 2: {
            int levelBaru = cekInteger("Silakan masukkan level persona: ");

            string query =
                "UPDATE persona SET level = " + to_string(levelBaru) +
                " WHERE id = " + to_string(personaId);

            if (mysql_query(conn, query.c_str())) {
                cout << "Gagal update level: " << mysql_error(conn) << endl;
                break;
            }

            personaUtama[idx].level = levelBaru;
            cout << "Level berhasil diubah" << endl;
            break;
        }

        case 3: {
            string arcanaBaru = cekStringTanpaAngka("Silakan masukkan arcana baru: ");
            int arcanaId = createArcana(conn, arcanaBaru);

            if (arcanaId == -1) {
                cout << "Gagal mendapatkan ID arcana" << endl;
                break;
            }

            string query =
                "UPDATE persona SET arcana_id = " + to_string(arcanaId) +
                " WHERE id = " + to_string(personaId);

            if (mysql_query(conn, query.c_str())) {
                cout << "Gagal mengupdate arcana: " << mysql_error(conn) << endl;
                break;
            }

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

            string skillBaru = cekStringTanpaAngka("Silakan masukkan nama skill baru: ");

            if (cekSkillPersona(personaUtama[idx].skills, skillBaru, idxSkill)) {
                cout << "Gagal: Persona ini sudah punya skill tersebut!" << endl;
                break;
            }

            if (mysql_query(conn, "START TRANSACTION")) {
                cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
                return;
            }
            
            int skillBaruId = getOrCreateSkill(conn, skillBaru);

            if (skillBaruId == -1) {
                mysql_query(conn, "ROLLBACK");
                cout <<  "Gagal mendapatkan ID skill baru!" << endl;
                break;
            }
            string skillLamaEsc = escapeSQL(conn, personaUtama[idx].skills[idxSkill]);

            string queryUpdateSkill = 
                "UPDATE persona_skills SET skill_id = " + to_string(skillBaruId) + " "
                "WHERE persona_id = " + to_string(personaId) + " "
                "AND skill_id = (SELECT id FROM skill_master WHERE nama_skill = '" + skillLamaEsc + "') "
                "LIMIT 1";

            if (mysql_query(conn, queryUpdateSkill.c_str())) {
                cout << "Gagal update skill persona: " << mysql_error(conn) << endl;
                mysql_query(conn, "ROLLBACK");
                break;
            }

            if (mysql_affected_rows(conn) == 0) {
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

            string query =
                "UPDATE persona SET harga = " + to_string(hargaBaru) + " WHERE id = " + to_string(personaId);

            if (mysql_query(conn, query.c_str())) {
                cout << "Gagal mengupdate harga: " << mysql_error(conn) << endl;
                break;
            }

            personaUtama[idx].harga = hargaBaru;
            cout << "Harga berhasil diubah!" << endl;
            break;
        }

        case 0:
            cout << "Batal update" << endl;
            break;

        default:
            cout << "pilihan tidak valid" << endl;
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

    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    string deleteSkill =
        "DELETE FROM persona_skills WHERE persona_id = " + to_string(personaId);

    if (mysql_query(conn, deleteSkill.c_str())) {
        cout << "Gagal menghapus skill persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    string deletePersona =
        "DELETE FROM persona WHERE id = " + to_string(personaId);

    if (mysql_query(conn, deletePersona.c_str())) {
        cout << "Gagal menghapus persona: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    personaUtama.erase(personaUtama.begin() + idx);

    cout << "Persona berhasil dihapus dari database!" << endl;
}

#endif


