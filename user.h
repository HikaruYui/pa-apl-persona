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

        if (userPtr->uang < personaUtama[index].harga) {
            throw runtime_error("Uang tidak cukup!");
        }

        if (mysql_query(conn, "START TRANSACTION")) {
            cout << "Gagal memulai transaksi: " << mysql_error(conn) << endl;
            return;
        }

        string insertCollection =
            "INSERT INTO user_persona_collection (user_id, original_persona_id, level_saat_ini) VALUES (" +
            to_string(userPtr->id) + ", " +
            to_string(terbeli.original_id) + ", " +
            to_string(terbeli.level) + ")";

        if (mysql_query(conn, insertCollection.c_str())) {
            cout << "Gagal menyimpan persona ke collection: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        int personaId = (int)mysql_insert_id(conn);
        terbeli.id = personaId;

        for (const string& skill : terbeli.skills) {
        int skillId = getOrCreateSkill(conn, skill);

        if (skillId == -1) {
            mysql_query(conn, "ROLLBACK");
            cout << "Gagal Menambah Persona: " << endl;
            return;
        }

        string insertSkill = "INSERT INTO user_persona_equipped_skills (persona_instance_id, skill_id) VALUES (" +
                              to_string(personaId) + ", " +
                              to_string(skillId) + ")";

        if (mysql_query(conn, insertSkill.c_str())) {
            cout << "Gagal Menambah Skill Persona: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
    }

        int uangBaru = userPtr->uang - terbeli.harga;

        string updateUang =  "UPDATE users SET uang = " + to_string(uangBaru) +
                             " WHERE id = " + to_string(userPtr->id);

        if (mysql_query(conn, updateUang.c_str())) {
            cout << "Gagal update uang user: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

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

    
    int p1 = cekInteger("masukkan persona pertama");
    int p2 = cekInteger("masukkan persona kedua");
    
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

        string insertCollection =
            "INSERT INTO user_persona_collection (user_id, original_persona_id, level_saat_ini) VALUES (" +
            to_string(userPtr->id) + ", " +
            to_string(hasilFusion.original_id) + ", " +
            to_string(hasilFusion.level) + ")";

        if (mysql_query(conn, insertCollection.c_str())) {
            cout << "Gagal menyimpan persona ke collection: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        int id_persona = (int)mysql_insert_id(conn);
        hasilFusion.id = id_persona;

        for (int i = 0; i < (int)hasilFusion.skills.size(); i++) {
        int skillId = getOrCreateSkill(conn, hasilFusion.skills[i]);

        if (skillId == -1) {
            mysql_query(conn, "ROLLBACK");
            return;
        }
        
        string insertSkill = 
            "INSERT INTO user_persona_equipped_skills (persona_instance_id, skill_id) VALUES (" +
            to_string(id_persona) + ", " +
            to_string(skillId) + ")";

        if (mysql_query(conn, insertSkill.c_str())) {
            cout << "Gagal menyimpan skill special fusion: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
    }

        string queryHapusSkillUser = "DELETE FROM user_persona_equipped_skills WHERE "
        "persona_instance_id IN("+ to_string(parent1.id) +", "+ to_string(parent2.id) +" )";

        if (mysql_query(conn, queryHapusSkillUser.c_str())) {
            cout << "Gagal hapus skill persona user: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        string queryHapusPersonaUser = "DELETE FROM user_persona_collection WHERE "
        "user_id = "+ to_string(userPtr->id) +" "
        "AND persona_instance_id IN("+ to_string(parent1.id) +", "+ to_string(parent2.id) +" )";

        if (mysql_query(conn, queryHapusPersonaUser.c_str())) {
            cout << "Gagal hapus persona user: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

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

            int idxItem = cekInteger("pilih nomor skill card");
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

                        string queryDeleteInventory = 
                            "DELETE FROM inventori_user "
                            "WHERE user_id = " + to_string(userPtr->id) + " "
                            "AND skill_tersimpan = " + to_string(list_skill_card.id);

                        if (mysql_query(conn, queryDeleteInventory.c_str())){
                            cerr << "Query update inventory user gagal: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }
                        
                        hapusInventory = true;
                        } 
                        else {
                            string queryUpdateInventory = 
                                "UPDATE inventori_user SET "
                                "jumlah_skill_card = " + to_string(userPtr->inventorySkill[idxItem].jumlah - 1) + " "
                                "WHERE user_id = " + to_string(userPtr->id) + " "
                                "AND skill_tersimpan = " + to_string(userPtr->inventorySkill[idxItem].id);

                            if (mysql_query(conn, queryUpdateInventory.c_str())){
                                cerr << "Query update inventory gagal: " << mysql_error(conn) << endl;
                                mysql_query(conn, "ROLLBACK");
                                return;
                            }
                        }

                        string skillLamaEsc = escapeSQL(conn, editedPersona.skills[idxSkill]);

                        string queryUpdateSkill = 
                            "UPDATE user_persona_equipped_skills SET skill_id = " + to_string(list_skill_card.id) + " "
                            "WHERE persona_instance_id = " + to_string(editedPersona.id) + " "
                            "AND skill_id = (SELECT id FROM skill_master WHERE nama_skill = '" + skillLamaEsc + "')";

                        if (mysql_query(conn, queryUpdateSkill.c_str())){
                            cerr << "Query update skill persona user gagal: " << mysql_error(conn) << endl;
                            mysql_query(conn, "ROLLBACK");
                            return;
                        }

                        if (mysql_affected_rows(conn) == 0) {
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

    string queryHapusSkillUser =
        "DELETE FROM user_persona_equipped_skills WHERE persona_instance_id = " +
        to_string(deletedPersona.id);

    if (mysql_query(conn, queryHapusSkillUser.c_str())) {
        cout << "Gagal hapus skill persona user: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    string queryHapusPersonaUser =
        "DELETE FROM user_persona_collection WHERE persona_instance_id = " +
        to_string(deletedPersona.id);

    if (mysql_query(conn, queryHapusPersonaUser.c_str())) {
        cout << "Gagal hapus persona user: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

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
        string queryUpdate =
            "UPDATE inventori_user SET jumlah_skill_card = jumlah_skill_card + 1 "
            "WHERE user_id = " + to_string(userPtr->id) + " "
            "AND skill_tersimpan = " + to_string(skillTerbeli.original_id);

        if (mysql_query(conn, queryUpdate.c_str())) {
            cerr << "Query update inventory gagal: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
    } else {
        string queryInsert =
            "INSERT INTO inventori_user (user_id, skill_tersimpan, jumlah_skill_card) VALUES (" +
            to_string(userPtr->id) + ", " +
            to_string(skillTerbeli.original_id) + ", 1)";

        if (mysql_query(conn, queryInsert.c_str())) {
            cerr << "Query insert inventori gagal: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
        
        skillBaru.id = skillTerbeli.original_id;
        skillBaru.nama_skill = skillTerbeli.nama_skill;
        skillBaru.jumlah = 1;
    }

    int uangBaru = userPtr->uang - skillTerbeli.harga;

    string updateUang =
        "UPDATE users SET uang = " + to_string(uangBaru) +
        " WHERE id = " + to_string(userPtr->id);

    if (mysql_query(conn, updateUang.c_str())) {
        cout << "Query update uang gagal: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

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

    string insertCollection =
         "INSERT INTO user_persona_collection (user_id, original_persona_id, level_saat_ini) VALUES (" +
        to_string(userPtr->id) + ", " +
        to_string(hasilFusion.original_id) + ", " +
        to_string(hasilFusion.level) + ")";

    if (mysql_query(conn, insertCollection.c_str())) {
        cout << "Gagal menyimpan hasil special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    int hasilInstanceId = (int)mysql_insert_id(conn);
    hasilFusion.id = hasilInstanceId;

    for (int i = 0; i < (int)hasilFusion.skills.size(); i++) {
        int skillId = getOrCreateSkill(conn, hasilFusion.skills[i]);

        if (skillId == -1) {
            mysql_query(conn, "ROLLBACK");
            return;
        }

        string insertSkill = 
            "INSERT INTO user_persona_equipped_skills (persona_instance_id, skill_id) VALUES (" +
            to_string(hasilInstanceId) + ", " +
            to_string(skillId) + ")";

        if (mysql_query(conn, insertSkill.c_str())) {
            cout << "Gagal menyimpan skill special fusion: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }
    }

    string idHapus = "";
    for (int i = 0; i < (int)parents.size(); i++) {
        if (i > 0) {
            idHapus += ", ";
        }
        idHapus += to_string(parents[i].id);
    }

    string deleteSkill = 
        "DELETE FROM user_persona_equipped_skills "
        "WHERE persona_instance_id IN (" + idHapus + ")";

    if (mysql_query(conn, deleteSkill.c_str())) {
        cout << "Gagal hapus skill parent special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    string deleteParents = 
        "DELETE FROM user_persona_collection "
        "WHERE user_id = " + to_string(userPtr->id) + " "
        "AND persona_instance_id IN (" + idHapus + ")";

    if (mysql_query(conn, deleteParents.c_str())) {
        cout << "Gagal hapus parent special fusion: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

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