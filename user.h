#ifndef USER_H
#define USER_H

#include "data_global.h"
#include "utils.h"
#include "database.h"
#include "admin.h"

void lihatPersonaUser(personaUser* profilePtr);
void fusePersonaSpecial(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
void beliPersona(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
bool login(int& userIndex);
void registerUser(MYSQL* conn);

int cariPersonaFusionLevel(string hasilArcana, int targetLevel) {
    int indeksTerbaik = -1;
    int selisihTerkecil = 999999;

    for (int i = 0; i < (int)personaUtama.size(); i++) {
        if (personaUtama[i].arcana == hasilArcana) {
            int selisih = abs(personaUtama[i].level - targetLevel);
        
        if (selisih < selisihTerkecil) {
            selisihTerkecil = selisih;
            indeksTerbaik = i;
        }
    }
}
return indeksTerbaik;
}

vector<string> skillWarisan(persona parent1, persona parent2) {
    vector<string> semuaSkill;
    vector<string> skillDipilih;

     
    for (int i = 0; i < parent1.skills.size(); i++) {
        if (!skillSudahAda(semuaSkill, parent1.skills[i])) {
            semuaSkill.push_back(parent1.skills[i]);
        }
    }
    
    for (int i = 0; i < parent2.skills.size(); i++) {
        if (!skillSudahAda(semuaSkill, parent2.skills[i])) {
            semuaSkill.push_back(parent2.skills[i]);
        }
    }

    int jumlahWajib = max_skill_warisan;

    if (semuaSkill.size() < max_skill_warisan) {
        jumlahWajib = semuaSkill.size();
    }

    cout << "\n=== Pilih Skill Warisan ===" << endl;

    
    
    for (int i = 0; i < semuaSkill.size(); i++) {
        cout << i + 1 << ". " << semuaSkill[i] << endl;
    }
    while ((int)skillDipilih.size() < jumlahWajib) {

        try {
        int pilihan = cekInteger("Silakan Pilih skill warisan: ");

        if (pilihan == 0) {
            break;
        }

        if (pilihan < 1 || pilihan > semuaSkill.size()) {
            throw out_of_range("Nomor skill tidak ada di daftar");
        }

        string skill = semuaSkill[pilihan - 1];
        
        if (skillSudahAda(skillDipilih, skill)) {
            throw invalid_argument("Skill Sudah Ada Pada persona!");
        }
        
        
        skillDipilih.push_back(skill);
        cout << "Skill berhasil ditambah!" << endl;
    }
    catch (const out_of_range& e) {
        cout << "Error: " << e.what() << endl;
        }

    catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
        }   
    }
    return skillDipilih;
}

void beliPersona(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) {
    try {
        if (profilePtr->listPersona.size() >= max_persona_user) {
            throw out_of_range("Batas maksimum persona sudah tercapai");
        }

        lihatPersonaUtama();

        int index = cekInteger("Pilih nomor persona yang ingin dibeli (0 untuk batal) : ");

        if (index == 0) {
            cout << "batal membeli" << endl;
            return;
        }

        index--;

        if (index < 0 || index >= (int)personaUtama.size()) {
            throw out_of_range("Nomor persona tidak valid");
        }

        persona terbeli = personaUtama[index];
        terbeli.original_id = personaUtama[index].original_id;

        // Kalau persona special, tidak boleh dibeli langsung.
        if (terbeli.isSpecial) {
            cout << "Persona special tidak bisa dibeli langsung!" << endl;
            return;
        }

        if (userPtr->uang < terbeli.harga) {
            throw runtime_error("Uang tidak cukup!");
        }

        // Mulai transaksi karena proses beli persona mengubah beberapa tabel:
        // 1. user_persona_collection
        // 2. user_persona_equipped_skills
        // 3. users
        if (mysql_query(conn, "START TRANSACTION")) {
            cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
            return;
        }

        // INSERT ke user_persona_collection
        const char* insertCollectionQuery =
            "INSERT INTO user_persona_collection "
            "(user_id, original_persona_id, level_saat_ini) "
            "VALUES (?, ?, ?)";

        MYSQL_STMT* insertCollectionStmt = mysql_stmt_init(conn);

        if (insertCollectionStmt == NULL) {
            cout << "Gagal init insert collection: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(insertCollectionStmt, insertCollectionQuery, strlen(insertCollectionQuery))) {
            cout << "Gagal prepare insert collection: " << mysql_stmt_error(insertCollectionStmt) << endl;
            mysql_stmt_close(insertCollectionStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND collectionParam[3];
        memset(collectionParam, 0, sizeof(collectionParam));

        // Parameter 1: user_id
        collectionParam[0].buffer_type = MYSQL_TYPE_LONG;
        collectionParam[0].buffer = &userPtr->id;

        // Parameter 2: original_persona_id
        collectionParam[1].buffer_type = MYSQL_TYPE_LONG;
        collectionParam[1].buffer = &terbeli.original_id;

        // Parameter 3: level_saat_ini
        collectionParam[2].buffer_type = MYSQL_TYPE_LONG;
        collectionParam[2].buffer = &terbeli.level;

        if (mysql_stmt_bind_param(insertCollectionStmt, collectionParam)) {
            cout << "Gagal bind insert collection: " << mysql_stmt_error(insertCollectionStmt) << endl;
            mysql_stmt_close(insertCollectionStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(insertCollectionStmt)) {
            cout << "Gagal menyimpan persona ke collection: " << mysql_stmt_error(insertCollectionStmt) << endl;
            mysql_stmt_close(insertCollectionStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        int personaId = (int)mysql_stmt_insert_id(insertCollectionStmt);

        mysql_stmt_close(insertCollectionStmt);

        terbeli.id = personaId;

        // INSERT skill persona user
        for (int i = 0; i < (int)terbeli.skills.size(); i++) {
            string skill = terbeli.skills[i];

            int skillId = getOrCreateSkill(conn, skill);

            if (skillId == -1) {
                mysql_query(conn, "ROLLBACK");
                cout << "Gagal Menambah Skill Persona" << endl;
                return;
            }

            const char* insertSkillQuery =
                "INSERT INTO user_persona_equipped_skills "
                "(persona_instance_id, skill_id) "
                "VALUES (?, ?)";

            MYSQL_STMT* insertSkillStmt = mysql_stmt_init(conn);

            if (insertSkillStmt == NULL) {
                cout << "Gagal init insert skill user: " << mysql_error(conn) << endl;
                mysql_query(conn, "ROLLBACK");
                return;
            }

            if (mysql_stmt_prepare(insertSkillStmt, insertSkillQuery, strlen(insertSkillQuery))) {
                cout << "Gagal prepare insert skill user: " << mysql_stmt_error(insertSkillStmt) << endl;
                mysql_stmt_close(insertSkillStmt);
                mysql_query(conn, "ROLLBACK");
                return;
            }

            MYSQL_BIND skillParam[2];
            memset(skillParam, 0, sizeof(skillParam));

            // Parameter 1: persona_instance_id
            skillParam[0].buffer_type = MYSQL_TYPE_LONG;
            skillParam[0].buffer = &personaId;

            // Parameter 2: skill_id
            skillParam[1].buffer_type = MYSQL_TYPE_LONG;
            skillParam[1].buffer = &skillId;

            if (mysql_stmt_bind_param(insertSkillStmt, skillParam)) {
                cout << "Gagal bind insert skill user: " << mysql_stmt_error(insertSkillStmt) << endl;
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

        // UPDATE uang user
        int uangBaru = userPtr->uang - terbeli.harga;

        const char* updateUangQuery =
            "UPDATE users SET uang = ? WHERE id = ?";

        MYSQL_STMT* updateUangStmt = mysql_stmt_init(conn);

        if (updateUangStmt == NULL) {
            cout << "Gagal init update uang: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(updateUangStmt, updateUangQuery, strlen(updateUangQuery))) {
            cout << "Gagal prepare update uang: " << mysql_stmt_error(updateUangStmt) << endl;
            mysql_stmt_close(updateUangStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND uangParam[2];
        memset(uangParam, 0, sizeof(uangParam));

        // Parameter 1: uang baru
        uangParam[0].buffer_type = MYSQL_TYPE_LONG;
        uangParam[0].buffer = &uangBaru;

        // Parameter 2: user_id
        uangParam[1].buffer_type = MYSQL_TYPE_LONG;
        uangParam[1].buffer = &userPtr->id;

        if (mysql_stmt_bind_param(updateUangStmt, uangParam)) {
            cout << "Gagal bind update uang: " << mysql_stmt_error(updateUangStmt) << endl;
            mysql_stmt_close(updateUangStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(updateUangStmt)) {
            cout << "Gagal update uang user: " << mysql_stmt_error(updateUangStmt) << endl;
            mysql_stmt_close(updateUangStmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(updateUangStmt);

        if (mysql_query(conn, "COMMIT")) {
            cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        profilePtr->listPersona.push_back(terbeli);
        userPtr->uang = uangBaru;

        cout << "persona berhasil dibeli! Sisa uang: " << userPtr->uang << endl;
    }
    catch (const out_of_range& e) {
        cout << "Error: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void fusePersona(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) { 
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa dibaca" << endl;
        return;
    }

    if (profilePtr->listPersona.size() < 2) { 
        cout << "Tidak ada persona yang bisa digabung (Minimal 2)" << endl;
        return;
    }
    
    lihatPersonaUser(profilePtr); 

    
    int p1 = cekInteger("masukkan persona pertama: ");
    int p2 = cekInteger("masukkan persona kedua: ");
    
    p1--;
    p2--;

    if (p1 < 0 || p1 >= profilePtr->listPersona.size() || 
        p2 < 0 || p2 >= profilePtr->listPersona.size() || 
        p1 == p2) { 
            cout << "Nomor persona tidak valid!" << endl;
            return;
        }
        
        persona parent1 = profilePtr->listPersona[p1];
        persona parent2 = profilePtr->listPersona[p2];
        
        int targetLevel = (parent1.level + parent2.level) / 2 + 1;

        string hasilArcana = cariHasilArcanaDB(conn, parent1.arcana, parent2.arcana);

        if (hasilArcana == "") {
            cout << "Kombinasi Arcana: " << parent1.arcana
                 << " + " << parent2.arcana << "Belum Tersedia. " << endl;
            return;
        }
        
        int indexHasil = cariPersonaFusionLevel(hasilArcana, targetLevel);

        if (indexHasil == -1) {
            cout << "Belum ada persona dengan arcana hasil: " << hasilArcana << endl;
            return;
        }

        persona hasilFusion = personaUtama[indexHasil];

        vector<string> warisan = skillWarisan( parent1, parent2);

        for (int i = 0; i < warisan.size(); i++) {
            if (!skillSudahAda(hasilFusion.skills, warisan[i]) &&
            hasilFusion.skills.size() < max_skill_persona) {
                hasilFusion.skills.push_back(warisan[i]);
            }
        }

        cout << "\n========== PREVIEW FUSION ==========" << endl;

        cout << "Parent 1      : " << parent1.nama
             << " | Level " << parent1.level
             << " | Arcana " << parent1.arcana << endl;

        cout << "Parent 2      : " << parent2.nama
             << " | Level " << parent2.level
             << " | Arcana " << parent2.arcana << endl;

        cout << "\nTarget Level  : " << targetLevel << endl;
        cout << "Arcana Hasil    : " << hasilArcana << endl;

        cout << "\nPersona Hasil : " << hasilFusion.nama << endl;
        cout << "Level           : " << hasilFusion.level << endl;
        cout << "Arcana          : " << hasilFusion.arcana << endl;

        cout << "\nSkill Hasil Fusion:" << endl;
        for (int i = 0; i < hasilFusion.skills.size(); i++) {
            cout << "- " << hasilFusion.skills[i] << endl;
        }

        cout << "====================================" << endl;

        if (!konfirmasiString("Lanjutkan Fusion? y/n : ")) {
            cout << "Penggabungan Arcana Dibatalkan." << endl;
            return;
        }


        if (mysql_query(conn, "START TRANSACTION")) {
            cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
            return;
        }

        const char* insertCollectionQuery =
    "INSERT INTO user_persona_collection "
    "(user_id, original_persona_id, level_saat_ini) "
    "VALUES (?, ?, ?)";

MYSQL_STMT* stmtCollection = mysql_stmt_init(conn);

if (stmtCollection == NULL) {
    cout << "Gagal init simpan persona fusion: " << mysql_error(conn) << endl;
    mysql_query(conn, "ROLLBACK");
    return;
}

if (mysql_stmt_prepare(stmtCollection, insertCollectionQuery, strlen(insertCollectionQuery))) {
    cout << "Gagal prepare simpan persona fusion: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

MYSQL_BIND paramCollection[3];
memset(paramCollection, 0, sizeof(paramCollection));

paramCollection[0].buffer_type = MYSQL_TYPE_LONG;
paramCollection[0].buffer = &userPtr->id;

paramCollection[1].buffer_type = MYSQL_TYPE_LONG;
paramCollection[1].buffer = &hasilFusion.original_id;

paramCollection[2].buffer_type = MYSQL_TYPE_LONG;
paramCollection[2].buffer = &hasilFusion.level;

if (mysql_stmt_bind_param(stmtCollection, paramCollection)) {
    cout << "Gagal bind simpan persona fusion: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

if (mysql_stmt_execute(stmtCollection)) {
    cout << "Gagal menyimpan persona ke collection: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

int id_persona = (int)mysql_stmt_insert_id(stmtCollection);
mysql_stmt_close(stmtCollection);

hasilFusion.id = id_persona;

for (int i = 0; i < (int)hasilFusion.skills.size(); i++) {
    int skillId = getOrCreateSkill(conn, hasilFusion.skills[i]);

    if (skillId == -1) {
        mysql_query(conn, "ROLLBACK");
        return;
    }

    const char* insertSkillQuery =
        "INSERT INTO user_persona_equipped_skills "
        "(persona_instance_id, skill_id) "
        "VALUES (?, ?)";

    MYSQL_STMT* stmtSkill = mysql_stmt_init(conn);

    if (stmtSkill == NULL) {
        cout << "Gagal init simpan skill fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtSkill, insertSkillQuery, strlen(insertSkillQuery))) {
        cout << "Gagal prepare simpan skill fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND paramSkill[2];
    memset(paramSkill, 0, sizeof(paramSkill));

    paramSkill[0].buffer_type = MYSQL_TYPE_LONG;
    paramSkill[0].buffer = &id_persona;

    paramSkill[1].buffer_type = MYSQL_TYPE_LONG;
    paramSkill[1].buffer = &skillId;

    if (mysql_stmt_bind_param(stmtSkill, paramSkill)) {
        cout << "Gagal bind simpan skill fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtSkill)) {
        cout << "Gagal menyimpan skill fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtSkill);
}

        const char* deleteSkillQuery =
            "DELETE FROM user_persona_equipped_skills "
            "WHERE persona_instance_id IN (?, ?)";

        MYSQL_STMT* stmtDeleteSkill = mysql_stmt_init(conn);

        if (stmtDeleteSkill == NULL) {
            cout << "Gagal init hapus skill parent: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(stmtDeleteSkill, deleteSkillQuery, strlen(deleteSkillQuery))) {
            cout << "Gagal prepare hapus skill parent: " << mysql_stmt_error(stmtDeleteSkill) << endl;
            mysql_stmt_close(stmtDeleteSkill);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND paramDeleteSkill[2];
        memset(paramDeleteSkill, 0, sizeof(paramDeleteSkill));

        paramDeleteSkill[0].buffer_type = MYSQL_TYPE_LONG;
        paramDeleteSkill[0].buffer = &parent1.id;

        paramDeleteSkill[1].buffer_type = MYSQL_TYPE_LONG;
        paramDeleteSkill[1].buffer = &parent2.id;

        if (mysql_stmt_bind_param(stmtDeleteSkill, paramDeleteSkill)) {
            cout << "Gagal bind hapus skill parent: " << mysql_stmt_error(stmtDeleteSkill) << endl;
            mysql_stmt_close(stmtDeleteSkill);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(stmtDeleteSkill)) {
            cout << "Gagal hapus skill persona user: " << mysql_stmt_error(stmtDeleteSkill) << endl;
            mysql_stmt_close(stmtDeleteSkill);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmtDeleteSkill);

        const char* deletePersonaQuery =
            "DELETE FROM user_persona_collection "
            "WHERE user_id = ? "
            "AND persona_instance_id IN (?, ?)";

        MYSQL_STMT* stmtDeletePersona = mysql_stmt_init(conn);

        if (stmtDeletePersona == NULL) {
            cout << "Gagal init hapus persona parent: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(stmtDeletePersona, deletePersonaQuery, strlen(deletePersonaQuery))) {
            cout << "Gagal prepare hapus persona parent: " << mysql_stmt_error(stmtDeletePersona) << endl;
            mysql_stmt_close(stmtDeletePersona);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND paramDeletePersona[3];
        memset(paramDeletePersona, 0, sizeof(paramDeletePersona));

        paramDeletePersona[0].buffer_type = MYSQL_TYPE_LONG;
        paramDeletePersona[0].buffer = &userPtr->id;

        paramDeletePersona[1].buffer_type = MYSQL_TYPE_LONG;
        paramDeletePersona[1].buffer = &parent1.id;

        paramDeletePersona[2].buffer_type = MYSQL_TYPE_LONG;
        paramDeletePersona[2].buffer = &parent2.id;

        if (mysql_stmt_bind_param(stmtDeletePersona, paramDeletePersona)) {
            cout << "Gagal bind hapus persona parent: " << mysql_stmt_error(stmtDeletePersona) << endl;
            mysql_stmt_close(stmtDeletePersona);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(stmtDeletePersona)) {
            cout << "Gagal hapus persona user: " << mysql_stmt_error(stmtDeletePersona) << endl;
            mysql_stmt_close(stmtDeletePersona);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmtDeletePersona);

        if (mysql_query(conn, "COMMIT")) {
            cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        profilePtr->listPersona.push_back(hasilFusion);

        int maxIdx = max(p1, p2);
        int minIdx = min(p1, p2);

        profilePtr->listPersona.erase(profilePtr->listPersona.begin() + maxIdx);
        profilePtr->listPersona.erase(profilePtr->listPersona.begin() + minIdx);

        cout << "\nFusion berhasil!" << endl;
        cout << "Mendapatkan persona: " << hasilFusion.nama << endl;
}

void updateSkillUser(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) { 
    skill_card_user list_skill_card;

    if ((*profilePtr).listPersona.empty()) { 
        cout << "Kamu belum punya persona apa-apa" << endl;
        return;
    }

    Table personaTable;
    personaTable.add_row({"No", "Nama", "Level", "Arcana", "Skill"});

    for (int i = 0; i < (int)(*profilePtr).listPersona.size(); ++i) { 
        personaTable.add_row({
            to_string(i + 1),
            (*profilePtr).listPersona[i].nama,
            to_string((*profilePtr).listPersona[i].level),
            (*profilePtr).listPersona[i].arcana,
            gabungSkill((*profilePtr).listPersona[i].skills)
        });
    }

    cout << personaTable << endl;

    int index = cekInteger("pilih nomor persona : ");
    index -= 1;

    if (index >= 0 && index < (int)(*profilePtr).listPersona.size()) { 
        auto& p = (*profilePtr).listPersona[index]; 

        if (p.skills.empty()) {
            cout << "persona ini tidak memiliki skill" << endl;
            return;
        }

        Table skillPersonaTable;
        skillPersonaTable.add_row({"No", "Skill Persona"});

        for (int j = 0; j < (int)p.skills.size(); ++j) {
            skillPersonaTable.add_row({
                to_string(j + 1),
                p.skills[j]
            });
        }
        cout << skillPersonaTable << endl;

        int idxSkill = cekInteger("pilih nomor skill yang ingin diubah : ");
        idxSkill -= 1;

        if (idxSkill >= 0 && idxSkill < (int)p.skills.size()) {

            if (userPtr->inventorySkill.empty()) {
                cout << "Kamu belum punya skill card" << endl;
                return;
            }

            cout << "Daftar Skill card:" << endl;
            Table skillCardTable;

            skillCardTable.add_row({"No", "Nama Skill", "Jumlah"});
            
            for (int i = 0; i < (int)userPtr->inventorySkill.size(); ++i) {
                list_skill_card = userPtr->inventorySkill[i];

                skillCardTable.add_row({
                    to_string(i + 1),
                    list_skill_card.nama_skill,
                    to_string(list_skill_card.jumlah)
                });
            }

            cout << skillCardTable << endl;

            int idxItem = cekInteger("pilih nomor skill card: ");
            idxItem -= 1;

           if (idxItem >= 0 && idxItem < (int)userPtr->inventorySkill.size()) {
                list_skill_card = userPtr->inventorySkill[idxItem];

                if (cekSkillPersona(p.skills, list_skill_card.nama_skill, idxSkill)) {
                    cout << "Sudah punya skill yang sama, batalkan pembaruan" << endl;

                } else {
                    persona editedPersona = (*profilePtr).listPersona[index];

                    if (mysql_query(conn, "START TRANSACTION")) {
                        cout << "Gagal memulai transaksi" << mysql_error(conn) << endl;
                        return;
                    }

                    bool hapusInventory = false;

                    if (list_skill_card.jumlah == 1) {
                        const char* queryDeleteInventory =
                            "DELETE FROM inventori_user "
                            "WHERE user_id = ? AND skill_tersimpan = ?";

                        MYSQL_STMT* stmtDeleteInventory = mysql_stmt_init(conn);

                        if (stmtDeleteInventory == NULL) {
                            cout << "Gagal init delete inventory: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_prepare(stmtDeleteInventory, queryDeleteInventory, strlen(queryDeleteInventory))) {
                            cout << "Gagal prepare delete inventory: " << mysql_stmt_error(stmtDeleteInventory) << endl;
                            mysql_stmt_close(stmtDeleteInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        MYSQL_BIND paramDeleteInventory[2];
                        memset(paramDeleteInventory, 0, sizeof(paramDeleteInventory));

                        paramDeleteInventory[0].buffer_type = MYSQL_TYPE_LONG;
                        paramDeleteInventory[0].buffer = &userPtr->id;

                        paramDeleteInventory[1].buffer_type = MYSQL_TYPE_LONG;
                        paramDeleteInventory[1].buffer = &list_skill_card.id;

                        if (mysql_stmt_bind_param(stmtDeleteInventory, paramDeleteInventory)) {
                            cout << "Gagal bind delete inventory: " << mysql_stmt_error(stmtDeleteInventory) << endl;
                            mysql_stmt_close(stmtDeleteInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_execute(stmtDeleteInventory)) {
                            cerr << "Query delete inventory user gagal: " << mysql_stmt_error(stmtDeleteInventory) << endl;
                            mysql_stmt_close(stmtDeleteInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        mysql_stmt_close(stmtDeleteInventory);
                        hapusInventory = true;
                    }
                    else {
                        int jumlahBaru = userPtr->inventorySkill[idxItem].jumlah - 1;

                        const char* queryUpdateInventory =
                            "UPDATE inventori_user SET jumlah_skill_card = ? "
                            "WHERE user_id = ? AND skill_tersimpan = ?";

                        MYSQL_STMT* stmtUpdateInventory = mysql_stmt_init(conn);

                        if (stmtUpdateInventory == NULL) {
                            cout << "Gagal init update inventory: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_prepare(stmtUpdateInventory, queryUpdateInventory, strlen(queryUpdateInventory))) {
                            cout << "Gagal prepare update inventory: " << mysql_stmt_error(stmtUpdateInventory) << endl;
                            mysql_stmt_close(stmtUpdateInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        MYSQL_BIND paramUpdateInventory[3];
                        memset(paramUpdateInventory, 0, sizeof(paramUpdateInventory));

                        paramUpdateInventory[0].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateInventory[0].buffer = &jumlahBaru;

                        paramUpdateInventory[1].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateInventory[1].buffer = &userPtr->id;

                        paramUpdateInventory[2].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateInventory[2].buffer = &userPtr->inventorySkill[idxItem].id;

                        if (mysql_stmt_bind_param(stmtUpdateInventory, paramUpdateInventory)) {
                            cout << "Gagal bind update inventory: " << mysql_stmt_error(stmtUpdateInventory) << endl;
                            mysql_stmt_close(stmtUpdateInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_execute(stmtUpdateInventory)) {
                            cerr << "Query update inventory gagal: " << mysql_stmt_error(stmtUpdateInventory) << endl;
                            mysql_stmt_close(stmtUpdateInventory);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        mysql_stmt_close(stmtUpdateInventory);
                    }

                        int skillLamaId = getOrCreateSkill(conn, editedPersona.skills[idxSkill]);

                        if (skillLamaId == -1) {
                            cout << "Gagal mendapatkan ID skill lama" << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        const char* queryUpdateSkill =
                            "UPDATE user_persona_equipped_skills "
                            "SET skill_id = ? "
                            "WHERE persona_instance_id = ? AND skill_id = ?";

                        MYSQL_STMT* stmtUpdateSkill = mysql_stmt_init(conn);

                        if (stmtUpdateSkill == NULL) {
                            cout << "Gagal init update skill user: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_prepare(stmtUpdateSkill, queryUpdateSkill, strlen(queryUpdateSkill))) {
                            cout << "Gagal prepare update skill user: " << mysql_stmt_error(stmtUpdateSkill) << endl;
                            mysql_stmt_close(stmtUpdateSkill);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        MYSQL_BIND paramUpdateSkill[3];
                        memset(paramUpdateSkill, 0, sizeof(paramUpdateSkill));

                        paramUpdateSkill[0].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateSkill[0].buffer = &list_skill_card.id;

                        paramUpdateSkill[1].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateSkill[1].buffer = &editedPersona.id;

                        paramUpdateSkill[2].buffer_type = MYSQL_TYPE_LONG;
                        paramUpdateSkill[2].buffer = &skillLamaId;

                        if (mysql_stmt_bind_param(stmtUpdateSkill, paramUpdateSkill)) {
                            cout << "Gagal bind update skill user: " << mysql_stmt_error(stmtUpdateSkill) << endl;
                            mysql_stmt_close(stmtUpdateSkill);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_stmt_execute(stmtUpdateSkill)) {
                            cerr << "Query update skill persona user gagal: " << mysql_stmt_error(stmtUpdateSkill) << endl;
                            mysql_stmt_close(stmtUpdateSkill);
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        int affectedRows = mysql_stmt_affected_rows(stmtUpdateSkill);

                        mysql_stmt_close(stmtUpdateSkill);

                        if (affectedRows == 0) {
                            cout << "Skill lama tidak ditemukan di database." << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_query(conn, "COMMIT")) {
                            cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        p.skills[idxSkill] = list_skill_card.nama_skill;
                        if (hapusInventory) {
                            userPtr->inventorySkill.erase(userPtr->inventorySkill.begin() + idxItem); 
                        }
                        else {
                            userPtr->inventorySkill[idxItem].jumlah -= 1;
                        }
                        cout << "Pembaruan berhasil" << endl; 
                }             
            } 
            else {
                cout << "Nomor skill item tidak ada" << endl;
            }
        } 
        else {
            cout << "Nomor skill persona tidak ada" << endl;
        }
    } 
    else {
        cout << "Nomor persona tidak ada" << endl;
    }
}

void hapusPersonaUser(MYSQL* conn, personaUser* profilePtr) { 
    if (profilePtr->listPersona.empty()) { 
        cout << "Kamu belum punya persona apa-apa" << endl;
        return;
    }

    lihatPersonaUser(profilePtr);

    int index = cekInteger("pilih persona yang ingin dihapus : ");

    if (index < 1 || index > (int)profilePtr->listPersona.size()) {
        cout << "Nomor persona tidak ada" << endl;
        return;
    }

    int idx = index - 1;
    persona deletedPersona = profilePtr->listPersona[idx];

    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    const char* queryHapusSkillUser =
        "DELETE FROM user_persona_equipped_skills "
        "WHERE persona_instance_id = ?";

    MYSQL_STMT* stmtHapusSkill = mysql_stmt_init(conn);

    if (stmtHapusSkill == NULL) {
        cout << "Gagal init hapus skill persona user: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtHapusSkill, queryHapusSkillUser, strlen(queryHapusSkillUser))) {
        cout << "Gagal prepare hapus skill persona user: " << mysql_stmt_error(stmtHapusSkill) << endl;
        mysql_stmt_close(stmtHapusSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND paramHapusSkill[1];
    memset(paramHapusSkill, 0, sizeof(paramHapusSkill));

    paramHapusSkill[0].buffer_type = MYSQL_TYPE_LONG;
    paramHapusSkill[0].buffer = &deletedPersona.id;

    if (mysql_stmt_bind_param(stmtHapusSkill, paramHapusSkill)) {
        cout << "Gagal bind hapus skill persona user: " << mysql_stmt_error(stmtHapusSkill) << endl;
        mysql_stmt_close(stmtHapusSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtHapusSkill)) {
        cout << "Gagal hapus skill persona user: " << mysql_stmt_error(stmtHapusSkill) << endl;
        mysql_stmt_close(stmtHapusSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtHapusSkill);

    const char* queryHapusPersonaUser =
        "DELETE FROM user_persona_collection "
        "WHERE persona_instance_id = ?";

    MYSQL_STMT* stmtHapusPersona = mysql_stmt_init(conn);

    if (stmtHapusPersona == NULL) {
        cout << "Gagal init hapus persona user: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtHapusPersona, queryHapusPersonaUser, strlen(queryHapusPersonaUser))) {
        cout << "Gagal prepare hapus persona user: " << mysql_stmt_error(stmtHapusPersona) << endl;
        mysql_stmt_close(stmtHapusPersona);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND paramHapusPersona[1];
    memset(paramHapusPersona, 0, sizeof(paramHapusPersona));

    paramHapusPersona[0].buffer_type = MYSQL_TYPE_LONG;
    paramHapusPersona[0].buffer = &deletedPersona.id;

    if (mysql_stmt_bind_param(stmtHapusPersona, paramHapusPersona)) {
        cout << "Gagal bind hapus persona user: " << mysql_stmt_error(stmtHapusPersona) << endl;
        mysql_stmt_close(stmtHapusPersona);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtHapusPersona)) {
        cout << "Gagal hapus persona user: " << mysql_stmt_error(stmtHapusPersona) << endl;
        mysql_stmt_close(stmtHapusPersona);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtHapusPersona);

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    profilePtr->listPersona.erase(profilePtr->listPersona.begin() + idx);

    cout << "persona berhasil dihapus" << endl;
}

void beliSkill(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) {
    if (skillCardShop.empty()) {
        cout << "Belum ada skill yang dijual" << endl;
        return;
    }

    cout << "\n=== Daftar Skill Item ===" << endl;

    Table table;
    table.add_row({"No", "Nama Skill", "Harga"});

    for (int i = 0; i < (int)skillCardShop.size(); i++) {
        skillCard skillForSale = skillCardShop[i];

        table.add_row({
            to_string(i + 1),
            skillForSale.nama_skill,
            to_string(skillForSale.harga)
        });
    }

    cout << table << endl;

    int pilih = cekInteger("Pilih skill yang ingin dibeli (0 untuk batal): ");

    if (pilih == 0) {
        cout << "Batal membeli skill" << endl;
        return;
    }

    pilih--;

    if (pilih < 0 || pilih >= (int)skillCardShop.size()) {
        cout << "Pilihan tidak valid" << endl;
        return;
    }

    skillCard skillTerbeli = skillCardShop[pilih];

    if (userPtr->uang < skillTerbeli.harga) {
        cout << "Uang anda tidak cukup!" << endl;
        return;
    }

    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    bool sudahAda = false;
    int indexInventory = -1;

    for (int i = 0; i < (int)userPtr->inventorySkill.size(); i++) {
        if (userPtr->inventorySkill[i].id == skillTerbeli.original_id) {
            sudahAda = true;
            indexInventory = i;
            break;
        }
    }

    skill_card_user skillBaru;

    if (sudahAda) {
        const char* queryUpdate =
            "UPDATE inventori_user "
            "SET jumlah_skill_card = jumlah_skill_card + 1 "
            "WHERE user_id = ? AND skill_tersimpan = ?";

        MYSQL_STMT* stmtUpdate = mysql_stmt_init(conn);

        if (stmtUpdate == NULL) {
            cout << "Gagal init update inventory: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(stmtUpdate, queryUpdate, strlen(queryUpdate))) {
            cout << "Gagal prepare update inventory: " << mysql_stmt_error(stmtUpdate) << endl;
            mysql_stmt_close(stmtUpdate);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND paramUpdate[2];
        memset(paramUpdate, 0, sizeof(paramUpdate));

        paramUpdate[0].buffer_type = MYSQL_TYPE_LONG;
        paramUpdate[0].buffer = &userPtr->id;

        paramUpdate[1].buffer_type = MYSQL_TYPE_LONG;
        paramUpdate[1].buffer = &skillTerbeli.original_id;

        if (mysql_stmt_bind_param(stmtUpdate, paramUpdate)) {
            cout << "Gagal bind update inventory: " << mysql_stmt_error(stmtUpdate) << endl;
            mysql_stmt_close(stmtUpdate);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(stmtUpdate)) {
            cerr << "Query update inventory gagal: " << mysql_stmt_error(stmtUpdate) << endl;
            mysql_stmt_close(stmtUpdate);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmtUpdate);
    } else {
        const char* queryInsert =
            "INSERT INTO inventori_user "
            "(user_id, skill_tersimpan, jumlah_skill_card) "
            "VALUES (?, ?, ?)";

        MYSQL_STMT* stmtInsert = mysql_stmt_init(conn);

        if (stmtInsert == NULL) {
            cout << "Gagal init insert inventory: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_prepare(stmtInsert, queryInsert, strlen(queryInsert))) {
            cout << "Gagal prepare insert inventory: " << mysql_stmt_error(stmtInsert) << endl;
            mysql_stmt_close(stmtInsert);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        int jumlahAwal = 1;

        MYSQL_BIND paramInsert[3];
        memset(paramInsert, 0, sizeof(paramInsert));

        paramInsert[0].buffer_type = MYSQL_TYPE_LONG;
        paramInsert[0].buffer = &userPtr->id;

        paramInsert[1].buffer_type = MYSQL_TYPE_LONG;
        paramInsert[1].buffer = &skillTerbeli.original_id;

        paramInsert[2].buffer_type = MYSQL_TYPE_LONG;
        paramInsert[2].buffer = &jumlahAwal;

        if (mysql_stmt_bind_param(stmtInsert, paramInsert)) {
            cout << "Gagal bind insert inventory: " << mysql_stmt_error(stmtInsert) << endl;
            mysql_stmt_close(stmtInsert);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        if (mysql_stmt_execute(stmtInsert)) {
            cerr << "Query insert inventori gagal: " << mysql_stmt_error(stmtInsert) << endl;
            mysql_stmt_close(stmtInsert);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmtInsert);

        skillBaru.id = skillTerbeli.original_id;
        skillBaru.nama_skill = skillTerbeli.nama_skill;
        skillBaru.jumlah = 1;
    }

    int uangBaru = userPtr->uang - skillTerbeli.harga;

    const char* queryUpdateUang =
        "UPDATE users SET uang = ? WHERE id = ?";

    MYSQL_STMT* stmtUang = mysql_stmt_init(conn);

    if (stmtUang == NULL) {
        cout << "Gagal init update uang: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtUang, queryUpdateUang, strlen(queryUpdateUang))) {
        cout << "Gagal prepare update uang: " << mysql_stmt_error(stmtUang) << endl;
        mysql_stmt_close(stmtUang);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND paramUang[2];
    memset(paramUang, 0, sizeof(paramUang));

    paramUang[0].buffer_type = MYSQL_TYPE_LONG;
    paramUang[0].buffer = &uangBaru;

    paramUang[1].buffer_type = MYSQL_TYPE_LONG;
    paramUang[1].buffer = &userPtr->id;

    if (mysql_stmt_bind_param(stmtUang, paramUang)) {
        cout << "Gagal bind update uang: " << mysql_stmt_error(stmtUang) << endl;
        mysql_stmt_close(stmtUang);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtUang)) {
        cout << "Query update uang gagal: " << mysql_stmt_error(stmtUang) << endl;
        mysql_stmt_close(stmtUang);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtUang);

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (sudahAda) {
        userPtr->inventorySkill[indexInventory].jumlah += 1;
    } else {
        userPtr->inventorySkill.push_back(skillBaru);
    }

    userPtr->uang = uangBaru;

    cout << "Skill berhasil dibeli!" << endl;
    cout << "Sisa uang: " << uangBaru << endl;
}

void fusePersonaSpecial(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) { 

    int jumlahParent = cekInteger("Mau Special Fusion berapa persona? (3 atau 4): ");

    if (jumlahParent < 3 || jumlahParent > 4) {
        cout << "Special Fusion hanya bisa 3 sampai 4 persona!" << endl;
        return;
    }

    if ((int)profilePtr->listPersona.size() < jumlahParent) {
        cout << "Persona kamu tidak cukup untuk Special Fusion!" << endl;
        return;
    }

    lihatPersonaUser(profilePtr);
    vector<int> pilihanIndex;
    vector<persona> parents;

    for (int i = 0; i < jumlahParent; i++) {
        int pilih =  cekInteger("Pilih persona ke-" + to_string(i + 1) + " : ") - 1;

        if (pilih < 0 || pilih >= (int)profilePtr->listPersona.size()) {
            cout << "Nomor persona tidak valid!" << endl;
            return;
        }

        for (int j = 0; j < pilihanIndex.size(); j++) {
            if (pilihanIndex[j] == pilih) {
                cout << "Persona tidak boleh dipilih dua kali!" << endl;
                return;
            }
        }

        pilihanIndex.push_back(pilih);
        parents.push_back(profilePtr->listPersona[pilih]);
    }

    int hasilPersonaId = cariHasilSpecialPersonaDB(conn, parents);

    if (hasilPersonaId == -1) {
        cout << "Kombinasi persona tidak tersedia untuk Special Fusion." << endl;
        return;
    } 

    int indexHasil = -1;

    for (int i = 0; i < (int)personaUtama.size(); i++) {
        if (personaUtama[i].original_id == hasilPersonaId) {
            indexHasil = i;
            break;
        }
    }

    if (indexHasil == -1) {
        cout << "Persona hasil Special Fusion tidak ditemukan." << endl;
        return;
    }
    
    persona hasilFusion = personaUtama[indexHasil];

    vector<string> semuaSkill;
   
    for (int i = 0; i < (int)parents.size(); i++) {
        for (int j = 0; j < (int)parents[i].skills.size(); j++) {
            if (!skillSudahAda(semuaSkill, parents[i].skills[j])) {
                semuaSkill.push_back(parents[i].skills[j]);
            }
        }
    }

    cout << "\n=== Pilih Skill Warisan (maks 2) ===" << endl;
    for (int i = 0; i < (int)semuaSkill.size(); i++) {
        cout << i + 1 << ". " << semuaSkill[i] << endl;
    }

    vector<string> skillWarisanDipilih;
    int jumlahMaks = min(2, (int)semuaSkill.size());

    while ((int)skillWarisanDipilih.size() < jumlahMaks) {
        int pilih = cekInteger("Pilih skill (0 jika selesai) : ");
        if (pilih == 0) break;
        pilih--;

        if (pilih < 0 || pilih >= (int)semuaSkill.size()) {
            cout << "Nomor tidak valid!" << endl;
            continue;
        }

        if (skillSudahAda(skillWarisanDipilih, semuaSkill[pilih])) {
            cout << "Skill sudah dipilih!" << endl;
            continue;
        }

        skillWarisanDipilih.push_back(semuaSkill[pilih]);
        cout << semuaSkill[pilih] << " ditambahkan." << endl;
    }

    for (int i = 0; i < (int)skillWarisanDipilih.size(); i++) {
        if (!skillSudahAda(hasilFusion.skills, skillWarisanDipilih[i]) &&
            (int)hasilFusion.skills.size() < max_skill_persona)
            hasilFusion.skills.push_back(skillWarisanDipilih[i]);
    }

    cout << "\n===== PREVIEW SPECIAL FUSION =====" << endl;
    for (int i = 0; i < (int)parents.size(); i++) {
        cout << "Parent " << i + 1 << " : "
         << parents[i].nama << " (" << parents[i].arcana << ")" << endl;
    }
    cout << "Hasil    : " << hasilFusion.nama << " | Level " << hasilFusion.level << " | Arcana " << hasilFusion.arcana << endl;
    cout << "Skill    : ";
    for (int i = 0; i < (int)hasilFusion.skills.size(); i++)
        cout << hasilFusion.skills[i] << " ";
    cout << "\n==================================" << endl;

    if (!konfirmasiString("Lanjutkan Fusion? y/n : ")) {
        cout << "Penggabungan Arcana Dibatalkan." << endl;
        return;
    }

    if (mysql_query(conn, "START TRANSACTION")) {
        cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
        return;
    }

    const char* insertCollectionQuery =
    "INSERT INTO user_persona_collection "
    "(user_id, original_persona_id, level_saat_ini) "
    "VALUES (?, ?, ?)";

MYSQL_STMT* stmtCollection = mysql_stmt_init(conn);

if (stmtCollection == NULL) {
    cout << "Gagal init hasil special fusion: " << mysql_error(conn) << endl;
    mysql_query(conn, "ROLLBACK");
    return;
}

if (mysql_stmt_prepare(stmtCollection, insertCollectionQuery, strlen(insertCollectionQuery))) {
    cout << "Gagal prepare hasil special fusion: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

MYSQL_BIND paramCollection[3];
memset(paramCollection, 0, sizeof(paramCollection));

paramCollection[0].buffer_type = MYSQL_TYPE_LONG;
paramCollection[0].buffer = &userPtr->id;

paramCollection[1].buffer_type = MYSQL_TYPE_LONG;
paramCollection[1].buffer = &hasilFusion.original_id;

paramCollection[2].buffer_type = MYSQL_TYPE_LONG;
paramCollection[2].buffer = &hasilFusion.level;

if (mysql_stmt_bind_param(stmtCollection, paramCollection)) {
    cout << "Gagal bind hasil special fusion: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

if (mysql_stmt_execute(stmtCollection)) {
    cout << "Gagal menyimpan hasil special fusion: " << mysql_stmt_error(stmtCollection) << endl;
    mysql_stmt_close(stmtCollection);
    mysql_query(conn, "ROLLBACK");
    return;
}

int hasilInstanceId = (int)mysql_stmt_insert_id(stmtCollection);
mysql_stmt_close(stmtCollection);

hasilFusion.id = hasilInstanceId;

for (int i = 0; i < (int)hasilFusion.skills.size(); i++) {
    int skillId = getOrCreateSkill(conn, hasilFusion.skills[i]);

    if (skillId == -1) {
        mysql_query(conn, "ROLLBACK");
        return;
    }

    const char* insertSkillQuery =
        "INSERT INTO user_persona_equipped_skills "
        "(persona_instance_id, skill_id) "
        "VALUES (?, ?)";

    MYSQL_STMT* stmtSkill = mysql_stmt_init(conn);

    if (stmtSkill == NULL) {
        cout << "Gagal init skill special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtSkill, insertSkillQuery, strlen(insertSkillQuery))) {
        cout << "Gagal prepare skill special fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_BIND paramSkill[2];
    memset(paramSkill, 0, sizeof(paramSkill));

    paramSkill[0].buffer_type = MYSQL_TYPE_LONG;
    paramSkill[0].buffer = &hasilInstanceId;

    paramSkill[1].buffer_type = MYSQL_TYPE_LONG;
    paramSkill[1].buffer = &skillId;

    if (mysql_stmt_bind_param(stmtSkill, paramSkill)) {
        cout << "Gagal bind skill special fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtSkill)) {
        cout << "Gagal menyimpan skill special fusion: " << mysql_stmt_error(stmtSkill) << endl;
        mysql_stmt_close(stmtSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtSkill);
}

string placeholder = "";

for (int i = 0; i < (int)parents.size(); i++) {
    if (i > 0) {
        placeholder += ", ";
    }

    placeholder += "?";
}

    string deleteSkillQuery =
        "DELETE FROM user_persona_equipped_skills "
        "WHERE persona_instance_id IN (" + placeholder + ")";

    MYSQL_STMT* stmtDeleteSkill = mysql_stmt_init(conn);

    if (stmtDeleteSkill == NULL) {
        cout << "Gagal init hapus skill parent special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtDeleteSkill, deleteSkillQuery.c_str(), deleteSkillQuery.length())) {
        cout << "Gagal prepare hapus skill parent special fusion: " << mysql_stmt_error(stmtDeleteSkill) << endl;
        mysql_stmt_close(stmtDeleteSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    vector<MYSQL_BIND> paramDeleteSkill(parents.size());
    memset(paramDeleteSkill.data(), 0, sizeof(MYSQL_BIND) * parents.size());

    vector<int> parentIds;

    for (int i = 0; i < (int)parents.size(); i++) {
        parentIds.push_back(parents[i].id);
    }

    for (int i = 0; i < (int)parents.size(); i++) {
        paramDeleteSkill[i].buffer_type = MYSQL_TYPE_LONG;
        paramDeleteSkill[i].buffer = &parentIds[i];
    }

    if (mysql_stmt_bind_param(stmtDeleteSkill, paramDeleteSkill.data())) {
        cout << "Gagal bind hapus skill parent special fusion: " << mysql_stmt_error(stmtDeleteSkill) << endl;
        mysql_stmt_close(stmtDeleteSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtDeleteSkill)) {
        cout << "Gagal hapus skill parent special fusion: " << mysql_stmt_error(stmtDeleteSkill) << endl;
        mysql_stmt_close(stmtDeleteSkill);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtDeleteSkill);

    string deleteParentsQuery =
        "DELETE FROM user_persona_collection "
        "WHERE user_id = ? "
        "AND persona_instance_id IN (" + placeholder + ")";

    MYSQL_STMT* stmtDeleteParents = mysql_stmt_init(conn);

    if (stmtDeleteParents == NULL) {
        cout << "Gagal init hapus parent special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_prepare(stmtDeleteParents, deleteParentsQuery.c_str(), deleteParentsQuery.length())) {
        cout << "Gagal prepare hapus parent special fusion: " << mysql_stmt_error(stmtDeleteParents) << endl;
        mysql_stmt_close(stmtDeleteParents);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    vector<MYSQL_BIND> paramDeleteParents(parents.size() + 1);
    memset(paramDeleteParents.data(), 0, sizeof(MYSQL_BIND) * (parents.size() + 1));

    paramDeleteParents[0].buffer_type = MYSQL_TYPE_LONG;
    paramDeleteParents[0].buffer = &userPtr->id;

    for (int i = 0; i < (int)parents.size(); i++) {
        paramDeleteParents[i + 1].buffer_type = MYSQL_TYPE_LONG;
        paramDeleteParents[i + 1].buffer = &parentIds[i];
    }

    if (mysql_stmt_bind_param(stmtDeleteParents, paramDeleteParents.data())) {
        cout << "Gagal bind hapus parent special fusion: " << mysql_stmt_error(stmtDeleteParents) << endl;
        mysql_stmt_close(stmtDeleteParents);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_stmt_execute(stmtDeleteParents)) {
        cout << "Gagal hapus parent special fusion: " << mysql_stmt_error(stmtDeleteParents) << endl;
        mysql_stmt_close(stmtDeleteParents);
        mysql_query(conn, "ROLLBACK");
        return;
    }

    mysql_stmt_close(stmtDeleteParents);

    if (mysql_query(conn, "COMMIT")) {
        cout << "Gagal commit transaksi: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    profilePtr->listPersona.push_back(hasilFusion);
    sort(pilihanIndex.begin(), pilihanIndex.end(), greater<int>());

    for (int i = 0; i < (int)pilihanIndex.size(); i++) {
        int idx = pilihanIndex[i];
        profilePtr->listPersona.erase(profilePtr->listPersona.begin() + idx);
    }
    cout << "\nSpecial Fusion berhasil! Mendapatkan: " << hasilFusion.nama << endl;
}

void lihatPersonaUser(personaUser* profilePtr) { 
    if ((*profilePtr).listPersona.empty()) { 
        cout << "Tidak ada persona yang dilihat" << endl;
        return;
    }

    Table table;
    table.add_row({"No", "Nama", "Level", "Arcana", "Skill"});

    for (int i = 0; i < (int)(*profilePtr).listPersona.size(); ++i) { 
        table.add_row({
            to_string(i + 1),
            (*profilePtr).listPersona[i].nama,
            to_string((*profilePtr).listPersona[i].level),
            (*profilePtr).listPersona[i].arcana,
            gabungSkill((*profilePtr).listPersona[i].skills)
        });
    }

    cout << table << endl;
}


#endif