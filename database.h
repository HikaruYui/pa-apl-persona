#ifndef DATABASE_H
#define DATABASE_H

#include "data_global.h"
#include "utils.h"

// helper escape string 

/*Note: 
c_str() itu fungsi bawaan dari std::string di C++. Gunanya: mengubah string C++ menjadi bentuk C-style string, yaitu const char*.
Di C++, kalau kamu bikin tempat penyimpanan sementara pakai new, kamu wajib menghapusnya sendiri pakai delete[].
Fungsi mysql_real_escape_string() adalah fungsi bawaan MySQL/MariaDB. Tugasnya: mengamankan teks dari karakter yang bisa merusak query SQL, seperti: ', ", \.
Isi parameternya: koneksi database
*/
string escapeSQL(MYSQL* conn, const string& input) {
    // buat dlu tempat penyimpanan sementara bernama buffer, ukurannya input.length() * 2 + 1 karena hasil escape bisa lebih panjang dari teks asli.
    char* buffer = new char[input.length()* 2 + 1];
    // input.c_str() isi input dalam bentuk C-style string, karena fungsi MySQL ini butuh const char*, bukan std::string.
    /*input.length() panjang teks input. Misalnya input: Jack O'Lantern hasil di buffer bisa jadi: Jack O\'Lantern*/
    mysql_real_escape_string(conn, buffer, input.c_str(), input.length());
    string hasil(buffer);
    /*Menghapus memori yang tadi dibuat dengan: new char[] Karena buffer dibuat manual, maka harus dihapus manual juga. Kalau tidak dihapus, bisa terjadi memory leak.*/
    delete[] buffer;
    return hasil;
}

/*Tambahkan helper ambil/buat arcana
Kalau arcana belum ada, fungsi ini akan otomatis masukin ke arcana_master.*/

int createArcana(MYSQL* conn, const string& arcanaName) {
    // Mengamankan teks arcanaName sebelum dimasukkan ke query SQL.
    string arcanaEsc = escapeSQL(conn, arcanaName);
    string selectQuery =  "SELECT id FROM arcana_master WHERE nama_arcana = '" + arcanaEsc + "' LIMIT 1";
    /*Contoh kalau arcanaEsc = "fool", maka query-nya jadi: SELECT id FROM arcana_master WHERE nama_arcana = 'fool' LIMIT 1;*/
    if (mysql_query(conn, selectQuery.c_str())) {
        cout << "Gagal mencari Arcana: " << mysql_error(conn) << endl;
        return -1;
    }
    /*Menjalankan query selectQuery ke database. selectQuery itu tipe C++ string. Tapi mysql_query() butuh const char*, jadi dipakai: selectQuery.c_str()
    Kalau query gagal, mysql_query() mengembalikan nilai bukan 0, sehingga masuk ke blok if.*/
    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cout << "Gagal mengambil hasil query arcana: " << mysql_error(conn) << endl;
        return -1;
    }

    /*Mengambil hasil query SELECT tadi dan menyimpannya ke variabel res. res ini berisi hasil tabel dari query.*/
    MYSQL_ROW row = mysql_fetch_row(res);
    /*Mengambil satu baris pertama dari hasil query. Kalau arcana ditemukan, row berisi data. Kalau tidak ditemukan, row bernilai NULL.*/
    if(row) {
        int id = atoi(row[0]);
        mysql_free_result(res);
        return(id);
        //atoi itu fungsi C/C++ buat mengubah teks angka menjadi integer.
    }

    // Menghapus/membersihkan hasil query dari memori. Setiap kali pakai mysql_store_result(), sebaiknya dibersihkan supaya tidak boros memori.
    mysql_free_result(res);
    string insertQuery = "INSERT INTO arcana_master (nama_arcana) VALUES ('" + arcanaEsc + "')";

    if (mysql_query(conn, insertQuery.c_str())) {
        cout << "Gagal menambah arcana: " << mysql_error(conn) << endl;
        return -1;
    }
    return(int)mysql_insert_id(conn);
}

int getOrCreateSkill(MYSQL* conn, const string& skillName) {
    string skillEsc = escapeSQL(conn, skillName);

    string selectQuery =  "SELECT id FROM skill_master WHERE nama_skill = '" + skillEsc + "' LIMIT 1";
    
    if(mysql_query(conn, selectQuery.c_str())) {
        cout << "Gagal mencari Skill Arcana: " << mysql_error(conn) << endl;
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (res == nullptr) {
        cout << "Gagal mengambil hasil query skill arcana: " << endl;
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        int id = atoi(row[0]);
        // Menghapus/membersihkan hasil query dari memori. Setiap kali pakai mysql_store_result(), sebaiknya dibersihkan dengan:
        mysql_free_result(res);
        return(id);
    }

    mysql_free_result(res);
    string insertQuery = "INSERT INTO skill_master (nama_skill) VALUES ('" + skillEsc + "')";

    if (mysql_query(conn, insertQuery.c_str())) {
        cout << "Gagal Menambah Skill: " << mysql_error(conn) << endl;
        return -1;
    }

    // Kalau insert berhasil, ambil id terakhir yang baru dibuat oleh database.
    return(int)mysql_insert_id(conn);

}

string cariHasilArcanaDB(MYSQL* conn, string arcana1, string arcana2) {
    string a1 = escapeSQL(conn, arcana1);
    string a2 = escapeSQL(conn, arcana2);

    string query = 
        "SELECT hasil.nama_arcana "
        "FROM arcana_fusion_matrix afm "
        "JOIN arcana_master ar1 ON afm.arcana_1 = ar1.id "
        "JOIN arcana_master ar2 ON afm.arcana_2 = ar2.id "
        "JOIN arcana_master hasil ON afm.result_arcana = hasil.id "
        "WHERE (LOWER(ar1.nama_arcana) = LOWER('" + a1 + "') "
        "AND LOWER(ar2.nama_arcana) = LOWER('" + a2 + "')) "
        "OR (LOWER(ar1.nama_arcana) = LOWER('" + a2 + "') "
        "AND LOWER(ar2.nama_arcana) = LOWER('" + a1 + "')) "
        "LIMIT 1";
    
    if (mysql_query(conn, query.c_str())) {
        cout << "Gagal cari hasil arcana: " << mysql_error(conn) << endl;
        return "";
    }
    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        return "";
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    string hasilArcana = "";

    if (row != NULL) {
        hasilArcana = row[0];
    }

    mysql_free_result(res);

    return hasilArcana;
}

int cariHasilSpecialPersonaDB(MYSQL* conn, const vector<persona>& parents) {
    int jumlahParent = parents.size();

    // Variabel ini dipakai untuk menyimpan daftar ID persona parent.
    string idList = "";
    
    for (int i = 0; i < jumlahParent; i++) {
        if (i > 0) {
            idList += ", ";
        }

        idList += to_string(parents[i].original_id);
    }
        
    string query =
        "SELECT sfr.result_persona_id "
        "FROM special_fusion_recipe sfr "
        "JOIN special_fusion_material sfm ON sfr.id = sfm.recipe_id "
        "WHERE sfm.material_persona_id IN (" + idList + ") "
        "GROUP BY sfr.id, sfr.result_persona_id "
        "HAVING COUNT(DISTINCT sfm.material_persona_id) = " + to_string(jumlahParent) + " "
        "AND COUNT(*) = " + to_string(jumlahParent) + " "
        "LIMIT 1";
    
    if (mysql_query(conn, query.c_str())) {
        cout << "Gagal cari special fusion persona: " << mysql_error(conn) << endl;
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == nullptr) {
        return -1;
    }
    
    // Ambil 1 baris hasil query dari database.
    MYSQL_ROW row = mysql_fetch_row(res);
    int hasilPersonaId;

    // Kalau query menemukan data, row akan berisi data. Kalau query tidak menemukan data, row akan bernilai NULL.
    if (row != NULL) {
        hasilPersonaId = atoi(row[0]);
    }
    else {
        hasilPersonaId = -1;
    }

    mysql_free_result(res);
    return hasilPersonaId;
}

MYSQL* connectDB() {
    MYSQL* conn = mysql_init(nullptr);

    if (conn == nullptr) {
        cerr << "mysql_init gagal" << endl;
        exit(1);
    }

    MYSQL* result = mysql_real_connect(
        conn,
        "127.0.0.1",
        "apluser",
        "",
        "persona",
        3306,
        NULL,
        0
    );

    if (result == nullptr) {
        cerr << "Koneksi database gagal: " << mysql_error(conn) << endl;
        mysql_close(conn);
        exit(1);
    }

    return conn;
}

vector<persona> loadPersonaFromDB(MYSQL* conn) {
    vector<persona> daftar;

    string query =
        "SELECT p.id, p.nama, p.level, a.nama_arcana, p.harga "
        "FROM persona p "
        "JOIN arcana_master a ON p.arcana_id = a.id "
        "ORDER BY p.id";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Query persona gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cerr << "Ambil data persona gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        int personaId = stoi(row[0]);

        persona p;
        p.id = stoi(row[0]);
        p.original_id = stoi(row[0]);
        p.nama = row[1];
        p.level = stoi(row[2]);
        p.arcana = row[3];
        p.harga = stoi(row[4]);

       string skillQuery =
            "SELECT sm.nama_skill "
            "FROM persona_skills ps "
            "JOIN skill_master sm ON ps.skill_id = sm.id "
            "WHERE ps.persona_id = " + to_string(personaId) + " "
            "ORDER BY sm.id";
            
        if (mysql_query(conn, skillQuery.c_str())) {
            cerr << "Query skill gagal: " << mysql_error(conn) << endl;
        } else {
            MYSQL_RES* skillRes = mysql_store_result(conn);
            MYSQL_ROW skillRow;

            while ((skillRow = mysql_fetch_row(skillRes))) {
                p.skills.push_back(skillRow[0]);
            }

            mysql_free_result(skillRes);
        }

        daftar.push_back(p);
    }

    mysql_free_result(res);

    return daftar;
}


void loadUserPersonaCollectionFromDB(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr) {
    profilePtr->listPersona.clear();


    // dia ambil dua id gt
    string query =
        "SELECT upc.persona_instance_id, upc.original_persona_id, p.nama, "
        "upc.level_saat_ini, a.nama_arcana, p.harga "
        "FROM user_persona_collection upc "
        "JOIN persona p ON upc.original_persona_id = p.id "
        "JOIN arcana_master a ON p.arcana_id = a.id "
        "WHERE upc.user_id = " + to_string(userPtr->id) + " "
        "ORDER BY upc.persona_instance_id";

    if (mysql_query(conn, query.c_str())) {
        cout << "Gagal load persona user: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cout << "Gagal ambil data persona user: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        int originalPersonaId = atoi(row[1]);

        persona p;
        p.id = atoi(row[0]);
        p.original_id = atoi(row[1]);
        p.nama = row[2];
        p.level = atoi(row[3]);
        p.arcana = row[4];
        p.harga = atoi(row[5]);

        string skillQuery =
            "SELECT sm.nama_skill "
            "FROM user_persona_equipped_skills upes "
            "JOIN skill_master sm ON upes.skill_id = sm.id "
            "WHERE upes.persona_instance_id = " + to_string(p.id) + " "
            "ORDER BY sm.id";

        if (mysql_query(conn, skillQuery.c_str())) {
            cout << "Gagal load skill persona user: " << mysql_error(conn) << endl;
        } else {
            MYSQL_RES* skillRes = mysql_store_result(conn);
            MYSQL_ROW skillRow;

            while ((skillRow = mysql_fetch_row(skillRes))) {
                p.skills.push_back(skillRow[0]);
            }

            mysql_free_result(skillRes);
        }

        profilePtr->listPersona.push_back(p);
    }

    mysql_free_result(res);
}

vector<LevelUser> loadUsersFromDB(MYSQL* conn) {
    vector<LevelUser> daftar;

    string query =
    "SELECT id, nama_user, password, uang, status "
    "FROM users "
    "ORDER BY id";

    
    if (mysql_query(conn, query.c_str())) {
        cerr << "Query users gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cerr << "Ambil data users gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        LevelUser u;
        u.id = stoi(row[0]);
        u.nama = row[1];
        u.password = row[2];
        u.uang = stoi(row[3]);

        string statusDB = row[4];

        if (statusDB == "admin") {
            u.status = 1;
        } else {
            u.status = 2;
        }

       string querySkill =
            "SELECT iu.skill_tersimpan, sm.nama_skill, iu.jumlah_skill_card "
            "FROM inventori_user iu "
            "JOIN skill_master sm ON iu.skill_tersimpan = sm.id "
            "WHERE iu.user_id = " + to_string(u.id) + " "
            "ORDER BY sm.nama_skill";

        if (mysql_query(conn, querySkill.c_str())) {
            cerr << "Query inventory gagal: " << mysql_error(conn) << endl;
            daftar.push_back(u);
            continue;
        }

        MYSQL_RES* resSkill = mysql_store_result(conn);

        if (resSkill == nullptr) {
            cerr << "Ambil data inventory gagal: " << mysql_error(conn) << endl;
            break;
        }

        MYSQL_ROW rowSkill;

        while ((rowSkill = mysql_fetch_row(resSkill))) {
            skill_card_user su;
            su.id = atoi(rowSkill[0]);
            su.nama_skill = rowSkill[1];
            su.jumlah = atoi(rowSkill[2]);
            u.inventorySkill.push_back(su);
        }

        mysql_free_result(resSkill);
        // u itu variabel sementara bertipe LevelUser di fungsi loadUsersFromDB()
        daftar.push_back(u);
    }

    mysql_free_result(res);
    return daftar;
}

void loadSkillShopFromDB(MYSQL* conn) {
    skillCard forSale;
    
    string query =
        "SELECT sm.nama_skill, scs.harga, scs.id, scs.skill_id "
        "FROM skill_card_shop scs "
        "JOIN skill_master sm ON scs.skill_id = sm.id "
        "ORDER BY scs.id";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Query skill shop gagal: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cerr << "Ambil data skill shop gagal: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        skillCard forSale;
        forSale.nama_skill = row[0];
        forSale.harga = stoi(row[1]);
        forSale.id = stoi(row[2]);
        forSale.original_id = stoi(row[3]);
        skillCardShop.push_back(forSale);
    }
    mysql_free_result(res);
}


bool panggilPersona(MYSQL* conn) {
    const char* query = "SELECT p.id, p.nama, p.level, a.nama_arcana, p.harga, sm.nama_skill "
                        "FROM persona AS p "
                        "JOIN arcana_master AS a ON p.arcana_id = a.id "
                        "LEFT JOIN persona_skills AS ps ON p.id = ps.persona_id "
                        "LEFT JOIN skill_master AS sm ON ps.skill_id = sm.id "
                        "ORDER BY p.id ASC";

    if (mysql_query(conn, query)) {
        cout << "Gagal Select: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cout << "Gagal Store Result: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_ROW row;
    int current_persona_id = -1; 

    while ((row = mysql_fetch_row(res))) {
        int persona_id = atoi(row[0]);

        if (persona_id != current_persona_id) {
            if (current_persona_id != -1) cout << endl;
            
            cout << "\n====== " << row[1] << " ======" << endl; 
            cout << "Level    : " << row[2] << endl;           
            cout << "Arcana   : " << row[3] << endl;          
            cout << "Harga    : " << row[4] << endl;           
            cout << "Skills   : ";
            
            current_persona_id = persona_id;
        }

        if (row[5] != NULL) {
            cout << row[5] << ", ";
        }
    }

    cout << endl; 
    mysql_free_result(res);
    return true;
}

#endif