#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cctype>
#include <stdexcept>
#include <mysql/mysql.h>

#define max_persona_user 10
#define max_skill_persona 6
#define max_skill_warisan 2
using namespace std;

struct skill_card_user {
    int id;
    string nama_skill;
    int jumlah;
};

struct LevelUser {
    int id;
    string nama;
    string password;
    int uang;
    int status; 
    vector<skill_card_user> inventorySkill;
};

struct persona {
    int id;
    int original_id; // ini id dari tabel persona ya
    string nama;
    int level;
    string arcana;
    vector<string> skills; 
    int harga;
    
};

struct personaUser {
    string user;
    vector<persona> listPersona; 
    
};

struct skillCard {
    int id;
    string nama_skill;
    int original_id;
    int harga;
};

string escapeSQL(MYSQL* conn, const string& input);
void loadUserPersonaCollectionFromDB(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);
int cariHasilSpecialPersonaDB(MYSQL* conn, const vector<persona>& parents);
string cariHasilArcanaDB(MYSQL* conn, string arcana1, string arcana2);
void printPersona(persona p, bool tampilkanHarga);
void lihatPersonaUtama();
void fusePersonaSpecial(MYSQL* conn, LevelUser* userPtr, personaUser* profilePtr);

vector<LevelUser> users;
vector<persona> personaUtama;
vector<personaUser> profilUser;
vector<skillCard> skillCardShop;

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

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool skillSudahAda(const vector<string>& skills, string skillName) {
    for(int i = 0; i < skills.size(); i++) {
        if (skills[i] == skillName) {
            return true;
        }
    }
    return false;
}

bool konfirmasiFuse() {
    char jawaban;

    cout << "Apakah anda yakin ingin melakukan penggabungan persona? y/n";
    cin >> jawaban;

    if (jawaban == 'y' || jawaban == 'Y' ) {
        return true;
    }
    return false;
}

bool spasi(const string& teks) {
    for (int i = 0; i < teks.length(); i++) {
        if (!isspace(teks[i])) {
            return false;
        }
    }
    return true;
}

int cekInteger(const string& prompt){
    string input;

    while (true)
    {
        try
        {
            cout << prompt;
            getline(cin, input);

            if (input.empty() || spasi(input)) {
                throw invalid_argument("Input tidak boleh kosong!");
            }

            for (int i = 0; i < input.length(); i++) {
                if (!isdigit(input[i])) {
                    throw invalid_argument("Input harus angka");
                }
            }

            int input_angka = stoi(input);

            if (input_angka < 0) {
                throw runtime_error("Input tidak boleh angka negatif!");
            }

            return input_angka;
        }
        catch (const invalid_argument& e) {
           cout << "error : " << e.what() << endl;
        }
        
        catch (const runtime_error& e) {
           cout << "error : " << e.what() << endl;
        }

        catch (const out_of_range& e) {
            cout << "error : Angka terlalu besar" << e.what() << endl;
        }
    }
}

string cekString(const string& prompt) {
    string input;
    
    while(true) {
        try {
            cout << prompt;
            getline(cin, input);

            if (input.empty() || spasi(input)) {
                throw invalid_argument("Error: Input tidak boleh kosong");
            } 
            return input;
            }
            catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
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

bool cekNamaPersona(const string& name, int excludeIndex = -1) {
    for (size_t i = 0; i < personaUtama.size(); ++i) {
        if (i == excludeIndex) continue;
        if (personaUtama[i].nama == name) {
            return true;
        }
    }
    return false;
}

bool cekSkillPersona(const vector<string>& skills, const string& skillName, int excludeIndex = -1) {
    for (size_t i = 0; i < skills.size(); ++i) {
        if (i == excludeIndex) continue;
        if (skills[i] == skillName) {
            return true;
        }
    }
    return false;
}

bool cekUser(const string& name) {
    for (const auto& u : users) {
        if (u.nama == name) {
            return true;
        }
    }
    return false;
}

int cariIndexUser(const string& name) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].nama == name) {
            return i;
        }
    }
    return -1;
}

int cariAtauBuatProfil(const string& username) {
    for (size_t i = 0; i < profilUser.size(); ++i) {
        if (profilUser[i].user == username) {
            return i;
        }
    }

    personaUser newProfile;
    newProfile.user = username;
    profilUser.push_back(newProfile);
    return profilUser.size() - 1;
}

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
    cout << "\npilih jenis pencarian : " << endl;
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
            while (low <= high) {
                int mid = low + (high - low) / 2;

                string midNameLower = (*daftarPersona)[mid].nama;
                std::transform(midNameLower.begin(), midNameLower.end(), midNameLower.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                if (midNameLower == pilihanNama) {
                    cout << "data ditemuin " << endl;
                    printPersona((*daftarPersona)[mid], *status == 1);
                    ketemu = true;
                    break;

                } else if (midNameLower < pilihanNama) {
                    low = mid + 1;

                } else {
                    high = mid - 1;
                }
            }
            if (!ketemu) {
                cout << "tidak ada nama persona yang sama " << endl;
            }
            break;
        }

        case 2: { 
            int pilihanLevel = cekInteger("masukkan level Persona yang dicari : ");
            bool levelKetemu = false;
            for (int i = 0; i < daftarPersona->size(); i++) {
                if (pilihanLevel == (*daftarPersona)[i].level) {
                    cout << "data ditemuin " << endl;
                    printPersona((*daftarPersona)[i], *status == 1);
                    levelKetemu = true;
                }
            }
            if (!levelKetemu) {
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

// Helper tampilkan Persona
void printPersona(persona p, bool tampilkanHarga) {
    cout << "======" << p.nama << "======" << endl;
    cout << "Level   : " << p.level << endl;
    cout << "Arcana  : " << p.arcana << endl;

    cout << "Skill   : ";
    for (int i = 0; i < p.skills.size(); i++) {
        cout << p.skills[i];

        // biar koma tidak muncul di skill terakhir.
        if (i < p.skills.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;

    if (tampilkanHarga) {
        cout << "Harga   : " << p.harga << endl; 
    }
}

// Helper tampilkan Persona
void lihatPersonaUtama() {
     if (personaUtama.empty()) {
        cout << "Belum ada Persona" << endl;
        return;
    }

    for (int i = 0; i < personaUtama.size(); i++) {
        cout << "\nNo. " << i + 1 << endl; 
        printPersona(personaUtama[i], true);
    }
}

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

void tambahPersona(MYSQL* conn) {
    persona newP;

    newP.nama = cekString("Masukkan nama persona: ");

    if (cekNamaPersona(newP.nama)) {
        cout << "Gagal: Nama persona sudah ada!" << endl;
        return;
    }
    newP.level = cekInteger("masukkan level persona : ");
    newP.arcana = cekString("Silakan masukkan arcana persona: ");

    clearInputBuffer();

    while (newP.skills.size() < 4) {
        string inputSkill = cekString("Skill " + to_string(newP.skills.size() + 1) + ": ");

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

    // Untuk setiap skill yang ada di newP.skills, ambil satu per satu, lalu cari/buat ID skill-nya di database.
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
            string arcanaBaru = cekString("Silakan masukkan arcana baru: ");
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

            string skillBaru = cekString("Silakan masukkan nama skill baru: ");

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

    char konfirmasi;
    cout << "Apakah kamu yakin ingin menghapus persona " 
         << personaUtama[idx].nama << "? y/n: ";
    cin >> konfirmasi;

    if (konfirmasi != 'y' && konfirmasi != 'Y') {
        cout << "Hapus persona dibatalkan" << endl;
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

void menuAdmin(LevelUser* userPtr, MYSQL* conn) {
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
            case 6: sortingPersona(&personaUtama, &(userPtr->status)); 
                break;
            case 0:
                cout << "Logout " << (*userPtr).nama << endl; 
                break;
            default: cout << "pilihan tidak valid." << endl;
        }
    } while (pilihan != 0);
}

void lihatPersonaUser(personaUser* profilePtr) { 
    if ((*profilePtr).listPersona.empty()) { 
        cout << "Tidak ada persona yang dilihat" << endl;
        return;
    }
    for (int i = 0; i < (*profilePtr).listPersona.size(); ++i) { 
        cout << "\nNo. " << i + 1 << endl;
        printPersona(profilePtr->listPersona[i], false);
    }
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

        

        if (!konfirmasiFuse()) {
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

    for (size_t i = 0; i < (*profilePtr).listPersona.size(); ++i) { 
        cout << (i + 1) << ". " << (*profilePtr).listPersona[i].nama << endl; 
    }

    int index = cekInteger("pilih nomor persona : ");
    index -= 1;

    if (index >= 0 && index < (int)(*profilePtr).listPersona.size()) { 
        auto& p = (*profilePtr).listPersona[index]; 

        if (p.skills.empty()) {
            cout << "persona ini tidak memiliki skill" << endl;
            return;
        }

        for (size_t j = 0; j < p.skills.size(); ++j) {
            cout << (j + 1) << ". " << p.skills[j] << endl;
        }

        int idxSkill = cekInteger("pilih nomor skill yang ingin diubah : ");
        idxSkill -= 1;

        if (idxSkill >= 0 && idxSkill < (int)p.skills.size()) {

            if (userPtr->inventorySkill.empty()) {
                cout << "Kamu belum punya skill card" << endl;
                return;
            }
            cout << "Daftar Skill card:" << endl;

            for (size_t i = 0; i < userPtr->inventorySkill.size(); ++i) {
                list_skill_card = userPtr->inventorySkill[i];
                cout << (i + 1) << ". " << list_skill_card.nama_skill << endl;
                cout << "jumlah : " << list_skill_card.jumlah << endl;
            }

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
    
    for (int i = 0; i < (int)skillCardShop.size(); i++) {
        skillCard skillForSale = skillCardShop[i];
        cout << i + 1 << ". " << skillForSale.nama_skill 
             << " | Harga: " << skillForSale.harga << endl;
    }

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

    cout << "\n=== Pilih Skill Warisan (maks 4) ===" << endl;
    for (int i = 0; i < (int)semuaSkill.size(); i++)
        cout << i + 1 << ". " << semuaSkill[i] << endl;

    vector<string> skillWarisanDipilih;
    int jumlahMaks = min(4, (int)semuaSkill.size());

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

    char jawaban;
    cout << "Lanjutkan Special Fusion? y/n : ";
    cin >> jawaban;
    if (jawaban != 'y' && jawaban != 'Y') {
        cout << "Special Fusion dibatalkan." << endl;
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
        pilihan = cekInteger("masukkan pilihan : ");
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
            case 6: sortingPersona(&(currentUserProfilePtr->listPersona), &(currentUserPtr->status)); 
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
            break;
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
            cout << "keluar dari program...." << endl;
            break;
        } else if (pilihanUtama == 1) {
            if (login(currentUserIndex)) {
                LevelUser* loggedUserPtr = &users[currentUserIndex];

                if (loggedUserPtr->status == 1) { 
                    menuAdmin(loggedUserPtr, conn); 
                } else if (loggedUserPtr->status == 2) { 
                    userMenu(conn, currentUserIndex);
                } else {
                    cout << "status user tidak dikenali." << endl;
                }
            }

        } else if (pilihanUtama == 2) {
            registerUser(conn);
        } else {
            cout << "pilihan tidak valid." << endl;
        }
    }
    mysql_close(conn);
    return 0;
}
