#ifndef DATABASE_H
#define DATABASE_H

#include "data_global.h"
#include "utils.h"
#include <cstring>

// helper escape string 

/*Note: 
c_str() itu fungsi bawaan dari std::string di C++. Gunanya: mengubah string C++ menjadi bentuk C-style string, yaitu const char*.
Di C++, kalau kamu bikin tempat penyimpanan sementara pakai new, kamu wajib menghapusnya sendiri pakai delete[].
Fungsi mysql_real_escape_string() adalah fungsi bawaan MySQL/MariaDB. Tugasnya: mengamankan teks dari karakter yang bisa merusak query SQL, seperti: ', ", \.
Isi parameternya: koneksi database
*/
string escapeSQL(MYSQL* conn, const string& input) {
    // Buffer otomatis dikelola oleh vector, jadi tidak perlu new/delete manual.
    vector<char> buffer(input.length() * 2 + 1);

    // Hasil escape bisa lebih panjang dari input asli.
    // mysql_real_escape_string mengembalikan panjang hasil escape.
    unsigned long panjangHasil = mysql_real_escape_string(
        conn,
        buffer.data(),
        input.c_str(),
        input.length()
    );

    return string(buffer.data(), panjangHasil);
}

/*Tambahkan helper ambil/buat arcana
Kalau arcana belum ada, fungsi ini akan otomatis masukin ke arcana_master.*/

int createArcana(MYSQL* conn, const string& arcanaName) {
    /*
        Tujuan fungsi ini:
        1. Cek dulu apakah arcana sudah ada di tabel arcana_master.
        2. Kalau sudah ada, return id arcana tersebut.
        3. Kalau belum ada, insert arcana baru.
        4. Return id arcana yang baru dibuat.
    */

    // Membuat object statement untuk query SELECT.
    MYSQL_STMT* stmt = mysql_stmt_init(conn);

    if (stmt == NULL) {
        cout << "Gagal init statement arcana: " << mysql_error(conn) << endl;
        return -1;
    }

    /*
        Query pakai tanda ? sebagai placeholder.
        Placeholder ini nanti diisi oleh mysql_stmt_bind_param().
        
        LOWER(nama_arcana) = LOWER(?)
        artinya pencarian tidak peduli huruf besar/kecil.
        Jadi "Death", "death", dan "DEATH" dianggap sama.
    */
    const char* selectQuery =
        "SELECT id FROM arcana_master "
        "WHERE LOWER(nama_arcana) = LOWER(?) "
        "LIMIT 1";

    // Menyiapkan query SELECT agar bisa dipakai sebagai prepared statement.
    if (mysql_stmt_prepare(stmt, selectQuery, strlen(selectQuery))) {
        cout << "Gagal prepare select arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    /*
        MYSQL_BIND dipakai untuk menghubungkan variabel C++ ke placeholder ?.
        Karena query SELECT punya 1 tanda ?, maka param-nya juga 1.
    */
    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));

    // Panjang string perlu disimpan karena MySQL butuh tahu ukuran data string.
    unsigned long arcanaLength = arcanaName.length();

    param[0].buffer_type = MYSQL_TYPE_STRING;       // tipe datanya string
    param[0].buffer = (char*)arcanaName.c_str();    // isi parameter
    param[0].buffer_length = arcanaName.length();   // panjang buffer
    param[0].length = &arcanaLength;                // pointer ke panjang string

    // Mengisi placeholder ? dengan nilai arcanaName.
    if (mysql_stmt_bind_param(stmt, param)) {
        cout << "Gagal bind param arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Menjalankan query SELECT.
    if (mysql_stmt_execute(stmt)) {
        cout << "Gagal execute select arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    /*
        Menyiapkan tempat untuk hasil SELECT.
        Query SELECT id akan menghasilkan 1 kolom, yaitu id arcana.
    */
    int idArcana = 0;

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONG; // tipe hasilnya integer
    result[0].buffer = &idArcana;            // hasil id disimpan ke idArcana

    // Menghubungkan hasil query ke variabel idArcana.
    if (mysql_stmt_bind_result(stmt, result)) {
        cout << "Gagal bind result arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    /*
        mysql_stmt_fetch() mengambil hasil SELECT.
        Kalau return 0, berarti data ditemukan.
    */
    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == 0) {
        // Arcana sudah ada, jadi statement ditutup lalu id-nya dikembalikan.
        mysql_stmt_close(stmt);
        return idArcana;
    }

    // Kalau tidak ditemukan, tutup statement SELECT dulu.
    mysql_stmt_close(stmt);

    /*
        Sampai sini berarti arcana belum ada di database.
        Maka kita buat prepared statement baru untuk INSERT.
    */
    MYSQL_STMT* insertStmt = mysql_stmt_init(conn);

    if (insertStmt == NULL) {
        cout << "Gagal init insert arcana: " << mysql_error(conn) << endl;
        return -1;
    }

    const char* insertQuery =
        "INSERT INTO arcana_master (nama_arcana) VALUES (?)";

    // Menyiapkan query INSERT.
    if (mysql_stmt_prepare(insertStmt, insertQuery, strlen(insertQuery))) {
        cout << "Gagal prepare insert arcana: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    /*
        Bind parameter untuk INSERT.
        Query INSERT juga punya 1 placeholder ?, yaitu nama_arcana.
    */
    MYSQL_BIND insertParam[1];
    memset(insertParam, 0, sizeof(insertParam));

    insertParam[0].buffer_type = MYSQL_TYPE_STRING;
    insertParam[0].buffer = (char*)arcanaName.c_str();
    insertParam[0].buffer_length = arcanaName.length();
    insertParam[0].length = &arcanaLength;

    if (mysql_stmt_bind_param(insertStmt, insertParam)) {
        cout << "Gagal bind insert arcana: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    // Menjalankan query INSERT.
    if (mysql_stmt_execute(insertStmt)) {
        cout << "Gagal menambah arcana: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    // Mengambil id dari data arcana yang baru dimasukkan.
    int newId = (int)mysql_stmt_insert_id(insertStmt);

    // Menutup statement INSERT agar resource database tidak bocor.
    mysql_stmt_close(insertStmt);

    return newId;
}

int getOrCreateSkill(MYSQL* conn, const string& skillName) {
    MYSQL_STMT* stmt = mysql_stmt_init(conn);

    // Membuat statement untuk query SELECT.
    // MYSQL_STMT dipakai untuk prepared statement.
    if (stmt == NULL) {
        cout << "Gagal init statement skill: " << mysql_error(conn) << endl;
        return -1;
    }

    const char* selectQuery = 
        "SELECT id FROM skill_master "
        "WHERE LOWER(nama_skill) = LOWER(?) "
        "LIMIT 1";
    
    // Mempersiapkan query SELECT agar bisa menerima parameter.
    if (mysql_stmt_prepare(stmt, selectQuery, strlen(selectQuery))) {
        cout << "Gagal prepare select skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // MYSQL_BIND dipakai untuk menghubungkan variabel C++ dengan placeholder (?) di query.
    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));

    // Menyimpan panjang string skillName.
    // Dibutuhkan oleh MySQL karena tipe datanya string.
    unsigned long skillLength = skillName.length();

    // Parameter pertama (?) diisi dengan skillName.
    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (char*)skillName.c_str();
    param[0].buffer_length = skillName.length();
    param[0].length = &skillLength;

    // Menghubungkan parameter dengan statement.
    if (mysql_stmt_bind_param(stmt, param)) {
        cout << "Gagal bind param skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Menjalankan query SELECT.
    if (mysql_stmt_execute(stmt)) {
        cout << "Gagal execute select skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Menyimpan hasil SELECT ke statement.
    // Ini membuat proses fetch hasil query lebih aman dan jelas.
    if (mysql_stmt_store_result(stmt)) {
        cout << "Gagal store result skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Variabel untuk menampung hasil select id.
    int idSkill = 0;

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    // Hasil kolom id akan dimasukkan ke idSkill.
    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = &idSkill;

    // Menghubungkan hasil query ke variabel idSkill.
    if (mysql_stmt_bind_result(stmt, result)) {
        cout << "Gagal bind result skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Mengambil hasil query.
    // Kalau fetchStatus == 0, berarti skill sudah ditemukan.
    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == 0) {
        mysql_stmt_close(stmt);
        return idSkill;
    }

    if (fetchStatus == 1) {
        cout << "Gagal fetch skill: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    // Kalau skill belum ditemukan, statement select ditutup dulu.
    mysql_stmt_close(stmt);

    // Membuat statement baru untuk insert skill.
    MYSQL_STMT* insertStmt = mysql_stmt_init(conn);

    if (insertStmt == NULL) {
        cout << "Gagal init insert skill: " << mysql_error(conn) << endl;
        return -1;
    }

    // Query INSERT juga memakai placeholder (?).
    const char* insertQuery =
        "INSERT INTO skill_master (nama_skill) VALUES (?)";
    
    // Mempersiapkan query INSERT.
    if (mysql_stmt_prepare(insertStmt, insertQuery, strlen(insertQuery))) {
        cout << "Gagal prepare insert skill: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    MYSQL_BIND insertParam[1];
    memset(insertParam, 0, sizeof(insertParam));

    // Parameter (?) pada INSERT diisi dengan skillName.
    insertParam[0].buffer_type = MYSQL_TYPE_STRING;
    insertParam[0].buffer = (char*)skillName.c_str();
    insertParam[0].buffer_length = skillName.length();
    insertParam[0].length = &skillLength;

    // Menghubungkan parameter INSERT ke statement.
    if (mysql_stmt_bind_param(insertStmt, insertParam)) {
        cout << "Gagal bind insert skill: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    // Menjalankan INSERT.
    if (mysql_stmt_execute(insertStmt)) {
        cout << "Gagal menambah skill: " << mysql_stmt_error(insertStmt) << endl;
        mysql_stmt_close(insertStmt);
        return -1;
    }

    // Mengambil id skill baru yang baru saja dibuat.
    int newId = (int)mysql_stmt_insert_id(insertStmt);

    // Menutup statement agar tidak boros memori.
    mysql_stmt_close(insertStmt);

    return newId;
}


string cariHasilArcanaDB(MYSQL* conn, string arcana1, string arcana2) {
    string hasilArcana = "";

    const char* query =
        "SELECT hasil.nama_arcana "
        "FROM arcana_fusion_matrix afm "
        "JOIN arcana_master ar1 ON afm.arcana_1 = ar1.id "
        "JOIN arcana_master ar2 ON afm.arcana_2 = ar2.id "
        "JOIN arcana_master hasil ON afm.result_arcana = hasil.id "
        "WHERE (LOWER(ar1.nama_arcana) = LOWER(?) "
        "AND LOWER(ar2.nama_arcana) = LOWER(?)) "
        "OR (LOWER(ar1.nama_arcana) = LOWER(?) "
        "AND LOWER(ar2.nama_arcana) = LOWER(?)) "
        "LIMIT 1";

    MYSQL_STMT* stmt = mysql_stmt_init(conn);

    if (stmt == NULL) {
        cout << "Gagal init cari hasil arcana: " << mysql_error(conn) << endl;
        return "";
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        cout << "Gagal prepare cari hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    MYSQL_BIND param[4];
    memset(param, 0, sizeof(param));

    unsigned long arcana1Length = arcana1.length();
    unsigned long arcana2Length = arcana2.length();

    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (char*)arcana1.c_str();
    param[0].buffer_length = arcana1.length();
    param[0].length = &arcana1Length;

    param[1].buffer_type = MYSQL_TYPE_STRING;
    param[1].buffer = (char*)arcana2.c_str();
    param[1].buffer_length = arcana2.length();
    param[1].length = &arcana2Length;

    param[2].buffer_type = MYSQL_TYPE_STRING;
    param[2].buffer = (char*)arcana2.c_str();
    param[2].buffer_length = arcana2.length();
    param[2].length = &arcana2Length;

    param[3].buffer_type = MYSQL_TYPE_STRING;
    param[3].buffer = (char*)arcana1.c_str();
    param[3].buffer_length = arcana1.length();
    param[3].length = &arcana1Length;

    if (mysql_stmt_bind_param(stmt, param)) {
        cout << "Gagal bind cari hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    if (mysql_stmt_execute(stmt)) {
        cout << "Gagal execute cari hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    if (mysql_stmt_store_result(stmt)) {
        cout << "Gagal store result cari hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    char hasilBuffer[100];
    unsigned long hasilLength = 0;
    my_bool isNull = 0;

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = hasilBuffer;
    result[0].buffer_length = sizeof(hasilBuffer);
    result[0].length = &hasilLength;
    result[0].is_null = &isNull;

    if (mysql_stmt_bind_result(stmt, result)) {
        cout << "Gagal bind result cari hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

    if (fetchStatus == 0) {
        if (!isNull) {
            hasilArcana = string(hasilBuffer, hasilLength);
        }
    }
    else if (fetchStatus == 1) {
        cout << "Gagal fetch hasil arcana: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return "";
    }

    mysql_stmt_close(stmt);

    return hasilArcana;
}

int cariHasilSpecialPersonaDB(MYSQL* conn, const vector<persona>& parents) {
    int jumlahParent = parents.size();

    if (jumlahParent == 0) {
        return -1;
    }

    /*
        Membuat placeholder sesuai jumlah parent.

        Contoh:
        jumlahParent = 2  -> ?, ?
        jumlahParent = 3  -> ?, ?, ?
        jumlahParent = 4  -> ?, ?, ?, ?
    */
    string placeholder = "";

    for (int i = 0; i < jumlahParent; i++) {
        if (i > 0) {
            placeholder += ", ";
        }

        placeholder += "?";
    }
        
    /*
        Bagian IN (...) tetap dibuat dinamis,
        tapi isinya bukan angka langsung, melainkan placeholder ?.
    */
    string query =
        "SELECT sfr.result_persona_id "
        "FROM special_fusion_recipe sfr "
        "JOIN special_fusion_material sfm ON sfr.id = sfm.recipe_id "
        "WHERE sfm.material_persona_id IN (" + placeholder + ") "
        "GROUP BY sfr.id, sfr.result_persona_id "
        "HAVING COUNT(DISTINCT sfm.material_persona_id) = ? "
        "AND COUNT(*) = ? "
        "LIMIT 1";
    
    MYSQL_STMT* stmt = mysql_stmt_init(conn);

    if (stmt == NULL) {
        cout << "Gagal init special fusion: " << mysql_error(conn) << endl;
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        cout << "Gagal prepare special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    /*
        Total parameter:
        - jumlahParent parameter untuk IN (...)
        - 1 parameter untuk COUNT(DISTINCT ...)
        - 1 parameter untuk COUNT(*)

        Jadi total = jumlahParent + 2
    */
    int jumlahParam = jumlahParent + 2;

    vector<MYSQL_BIND> param(jumlahParam);
    memset(param.data(), 0, sizeof(MYSQL_BIND) * jumlahParam);

    /*
        Simpan id parent di vector int.
        Ini penting supaya alamat memorinya tetap aman saat di-bind.
    */
    vector<int> parentIds;

    for (int i = 0; i < jumlahParent; i++) {
        parentIds.push_back(parents[i].original_id);
    }

    // Parameter untuk IN (?, ?, ...)
    for (int i = 0; i < jumlahParent; i++) {
        param[i].buffer_type = MYSQL_TYPE_LONG;
        param[i].buffer = &parentIds[i];
    }

    // Parameter untuk HAVING COUNT(DISTINCT ...) = ?
    param[jumlahParent].buffer_type = MYSQL_TYPE_LONG;
    param[jumlahParent].buffer = &jumlahParent;

    // Parameter untuk AND COUNT(*) = ?
    param[jumlahParent + 1].buffer_type = MYSQL_TYPE_LONG;
    param[jumlahParent + 1].buffer = &jumlahParent;

    if (mysql_stmt_bind_param(stmt, param.data())) {
        cout << "Gagal bind special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_execute(stmt)) {
        cout << "Gagal execute special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    if (mysql_stmt_store_result(stmt)) {
        cout << "Gagal store result special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    int hasilPersonaId = -1;

    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = &hasilPersonaId;

    if (mysql_stmt_bind_result(stmt, result)) {
        cout << "Gagal bind result special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    int fetchStatus = mysql_stmt_fetch(stmt);

     if (fetchStatus == 0) {
        mysql_stmt_close(stmt);
        return hasilPersonaId;
    }

    if (fetchStatus == 1) {
        cout << "Gagal fetch special fusion: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return -1;
    }

    mysql_stmt_close(stmt);
    return -1;
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

    // Memakai COALESCE, supaya kalau datanya NULL, otomatis dianggap 0.
    string query =
    "SELECT p.id, p.nama, p.level, a.nama_arcana, p.harga, "
    "COALESCE(p.isSpecial, 0), COALESCE(p.isSpecialMaterial, 0) "
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

        if (row[1] == NULL) {
            cout << "Data persona rusak: nama kosong, dilewati." << endl;
            continue;
        }
        p.nama = row[1];

        if (row[2] == NULL) {
            cout << "Data persona rusak: level kosong, dilewati." << endl;
            continue;
        }
        p.level = stoi(row[2]);

        if (row[3] == NULL) {
            cout << "Data persona rusak: arcana kosong, dilewati." << endl;
            continue;
        }
        p.arcana = row[3];

        if (row[4] == NULL) {
            cout << "Data persona rusak: harga kosong, dilewati." << endl;
            continue;
        }
        p.harga = stoi(row[4]);

        if (row[5] != NULL) {
            p.isSpecial = atoi(row[5]) == 1;
        } else {
            p.isSpecial = false;
        }

        if (row[6] != NULL) {
            p.isSpecialMaterial = atoi(row[6]) == 1;
        } else {
            p.isSpecialMaterial = false;
        }

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