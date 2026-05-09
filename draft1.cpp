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


struct LevelUser {
    string nama;
    string password;
    int uang;
    int status; 
};

struct persona {
    string nama;
    int level;
    string arcana;
    vector<string> skills; 
    int harga;
    
};

struct personaUser {
    string user;
    vector<persona> listPersona; 
    vector<string> inventorySkill;
};

struct FusionRule {
    string arcana1;
    string arcana2;
    string hasilArcana;
};

string escapeSQL(MYSQL* conn, const string& input);

vector<LevelUser> users;
vector<persona> personaUtama;

vector<personaUser> profilUser;

vector<string> skillItems;

vector<int> hargaSkill;

vector<FusionRule> fusionRules;

string cariHasilArcana(string arcana1, string arcana2) {
    for (int i = 0; i < fusionRules.size(); i++) {
        if (fusionRules[i].arcana1 == arcana1 && fusionRules[i].arcana2 == arcana2) {
            return fusionRules[i].hasilArcana;
        }
        if (fusionRules[i].arcana2 == arcana1 && fusionRules[i].arcana1 == arcana2) {
            return fusionRules[i].hasilArcana;
        }
    }
    return "";
}

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

int cekInteger(const string& prompt){
    int input_angka;
    while (true)
    {
        try
        {
            cout << prompt;
            cin >> input_angka;

            if (cin.fail()){
                cin.clear();
                clearInputBuffer();
                throw invalid_argument("input harus angka");
            }
            if (input_angka < 0){
                cin.clear();
                clearInputBuffer();
                throw runtime_error("input tidak boleh kurang dari satu");
            }
            return input_angka;
        }

        catch (const invalid_argument& e)
        {
           cout << "error : " << e.what() << endl;
        }
        
        catch (const runtime_error& e)
        {
           cout << "error : " << e.what() << endl;
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
        int pilihan = cekInteger("Silakan Pilih skill warisan (Tekan 0 jika selesai)");

        if (pilihan < 1 || pilihan > semuaSkill.size()) {
            throw out_of_range("nomor skill tidak ada di daftar");
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
    string namaBaru, passBaru;

    cout << "\n=== Registrasi user baru ===" << endl;
    cout << "masukkan nama user : ";
    cin >> namaBaru;
    
    if (cekUser(namaBaru)) {
        cout << "nama user sudah terdaftar!" << endl;
        return;
    }

    cout << "masukkan password : ";
    cin >> passBaru;

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

    LevelUser newUser;
    newUser.nama = namaBaru;
    newUser.password = passBaru;
    newUser.uang = 10000;
    newUser.status = 2;

    users.push_back(newUser);

    cout << "registrasi berhasil dan masuk ke database!" << endl;
}

bool login(int& userIndex) {
    string inputNama, inputPass;
    int percobaan = 1;
    
    while (percobaan <= 3) {
        cout << "\n--- Menu Login ---" << endl;
        cout << "masukkan nama (atau 0 untuk keluar): ";
        cin >> inputNama;

        if (inputNama == "0") {
            return false;
        }

        cout << "masukkan Password : ";
        cin >> inputPass;

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
    cout << "pilihan : ";
    int pilihan = cekInteger("pilihan : ");

    switch (pilihan) {
        case 1: { 
            string pilihanNama; 
            bubbleSortNama(daftarPersona, true);
            cout << "masukkan nama persona : ";
            clearInputBuffer(); 
            getline(cin, pilihanNama); 

            std::transform(pilihanNama.begin(), pilihanNama.end(), pilihanNama.begin(),
                           [](unsigned char c){ return std::tolower(c); });

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
                    cout << "\n====== " << (*daftarPersona)[mid].nama << " ======" << endl;
                    cout << "Level    : " << (*daftarPersona)[mid].level << endl;
                    cout << "Arcana   : " << (*daftarPersona)[mid].arcana << endl;
                    cout << "Skill    : ";
                    for (const auto& skill : (*daftarPersona)[mid].skills) {
                        cout << skill << ", ";
                    }
                    cout << endl;
                    if (*status == 1){
                        cout << "harga    : " << (*daftarPersona)[mid].harga << endl;
                    }
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
                    cout << "\n====== " << (*daftarPersona)[i].nama << " ======" << endl;
                    cout << "Level    : " << (*daftarPersona)[i].level << endl;
                    cout << "Arcana   : " << (*daftarPersona)[i].arcana << endl;
                    cout << "Skill    : ";
                    for (const auto& skill : (*daftarPersona)[i].skills) {
                        cout << skill << ", ";
                    }
                    cout << endl;
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

void lihatPersonaUtama() {
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa dibaca" << endl;
        return;
    }
    for (size_t i = 0; i < personaUtama.size(); ++i) {
        cout << "\n====== " << i + 1 << personaUtama[i].nama << " ======" << endl;
        cout << "Level    : " << personaUtama[i].level << endl;
        cout << "Arcana   : " << personaUtama[i].arcana << endl;
        cout << "Skill    : ";
        for (const auto& skill : personaUtama[i].skills) {
            cout << skill << ", ";
        }
        cout << endl;
        cout << "Harga    : " << personaUtama[i].harga << endl;
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
    cout << "masukkan nama persona : ";
    clearInputBuffer();
    getline(cin, newP.nama);

    if (cekNamaPersona(newP.nama)) {
        cout << "Gagal: Nama persona sudah ada!" << endl;
        return;
    }

    newP.level = cekInteger("masukkan level persona : ");

    cout << "masukkan skill persona, minimal 2 maksimal 4:" << endl;
    cin >> newP.arcana;

    clearInputBuffer();

    while (newP.skills.size() < 4) {
        string inputSkill;
        cout << "Skill " << (newP.skills.size() + 1) << ": ";
        getline(cin, inputSkill);

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
    
    mysql_query(conn, "Start Transaction");
    
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
    
    mysql_query(conn, "COMMIT");
    personaUtama.push_back(newP);

    cout << "Data persona berhasil dimasukkan ke database!" << endl;
}


void updatePersona() {
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa diubah" << endl;
        return;
    }

    lihatPersonaUtama();

    int index;
    cout << "masukkan nomor persona yang ingin diubah: ";
    cin >> index;

    if (index - 1 >= 0 && index - 1 < (int)personaUtama.size()) {
        int idx = index - 1;
        int pilihan;
        do {
            cout << "\n--- Update persona: " << personaUtama[idx].nama << " ---" << endl;
            cout << "1. Ubah nama persona" << endl;
            cout << "2. Ubah level persona" << endl;
            cout << "3. Ubah arcana persona" << endl;
            cout << "4. Ubah skill persona" << endl;
            cout << "5. Ubah harga persona" << endl;
            cout << "0. Batal update" << endl;
            cout << "pilihan: ";
            cin >> pilihan;
    
            switch (pilihan) {
            case 1: {
                string namaBaru;
                cout << "masukkan nama baru : ";
                clearInputBuffer();
                getline(cin, namaBaru);
                if (cekNamaPersona(namaBaru, idx)) {
                    cout << "Gagal: Nama persona sudah ada!" << endl;
                } else {
                    personaUtama[idx].nama = namaBaru;
                    cout << "Berhasil diubah" << endl;
                }
                break;
            }
            case 2:
                personaUtama[idx].level = cekInteger("masukkan level persona : ");
                cout << "Berhasil diubah" << endl;
                break;
            case 3:
                cout << "masukkan arcana baru : ";
                cin >> personaUtama[idx].arcana;
                cout << "Berhasil diubah" << endl;
                break;
            case 4: {
                if (personaUtama[idx].skills.empty()) {
                    cout << "Tidak ada skill untuk diubah" << endl;
                    break;
                }
                for (size_t j = 0; j < personaUtama[idx].skills.size(); ++j) {
                    cout << "Skill ke " << (j + 1) << " : " << personaUtama[idx].skills[j] << endl;
                }
                int idxSkill = cekInteger("nomor skill yang ingin diubah : ");
                if (idxSkill >= 1 && idxSkill <= (int)personaUtama[idx].skills.size()) {
                    clearInputBuffer();
                    string skillBaru;
                    cout << "masukkan skill baru : ";
                    getline(cin, skillBaru);
                    
                    if (cekSkillPersona(personaUtama[idx].skills, skillBaru, idxSkill - 1)) {
                        cout << "Gagal: Skill sudah ada di persona ini!" << endl;
                    } else {
                        personaUtama[idx].skills[idxSkill - 1] = skillBaru;
                        cout << "Skill berhasil diubah" << endl;
                    }
                } else {
                    cout << "Nomor skill tidak valid" << endl;
                }
                break;
            }
            case 5:
                personaUtama[idx].harga = cekInteger("masukkan harga persona baru : ");
                cout << "Berhasil diubah" << endl;
                break;
            case 0:
                cout << "Batal update" << endl;
                break;
            default:
                cout << "pilihan tidak valid" << endl;
            }
        } while (pilihan != 0);

    }else{
        cout << "Nomor tidak valid" << endl;
        return; 
    }

}

void hapusPersona() {
    if (personaUtama.empty()) {
        cout << "Tidak ada persona yang bisa dihapus" << endl;
        return;
    }

    lihatPersonaUtama();

    int index = cekInteger("masukkan nomor persona yang ingin dihapus : ");

    if (index >= 1 && index <= (int)personaUtama.size()) {
        personaUtama.erase(personaUtama.begin() + (index - 1));
        cout << "persona berhasil dihapus" << endl;
    } else {
        cout << "Tidak ada persona di nomor itu" << endl;
    }
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
            case 4: updatePersona(); 
                break;
            case 5: hapusPersona(); 
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
    for (size_t i = 0; i < (*profilePtr).listPersona.size(); ++i) { 
        const auto& p = (*profilePtr).listPersona[i]; 
        cout << "\n====== "<< i + 1 << p.nama << " ======" << endl;
        cout << "Level    : " << p.level << endl;
        cout << "Arcana   : " << p.arcana << endl;
        cout << "Skill    : ";
        for (const auto& skill : p.skills) {
            cout << skill << ", ";
        }
        cout << endl;
    }
}

void beliPersona(LevelUser* userPtr, personaUser* profilePtr) {
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

        
        if (userPtr->uang < personaUtama[index].harga) {
            throw runtime_error("Uang tidak cukup!");
        }

        
        persona terbeli = personaUtama[index];

        
        profilePtr->listPersona.push_back(terbeli);

        
        userPtr->uang -= personaUtama[index].harga;

        cout << "persona berhasil dibeli! Sisa uang: " << userPtr->uang << endl;
    }
    catch (const out_of_range& e) {
        cout << "Error: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void fusePersona(personaUser* profilePtr) { 

    
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

        
        string hasilArcana = cariHasilArcana(parent1.arcana, parent2.arcana);

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

        
        vector<string> warisan = skillWarisan(parent1, parent2);

        
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

        profilePtr->listPersona.push_back(hasilFusion);

        int maxIdx = max(p1, p2);
        int minIdx = min(p1, p2);

        profilePtr->listPersona.erase(profilePtr->listPersona.begin() + maxIdx);

        profilePtr->listPersona.erase(profilePtr->listPersona.begin() + minIdx);

        cout << "\nFusion berhasil!" << endl;
        cout << "Mendapatkan persona: " << hasilFusion.nama << endl;
}

void updateSkillUser(personaUser* profilePtr) { 
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
            cout << "Daftar Skill Item:" << endl;
            for (size_t i = 0; i < skillItems.size(); ++i) {
                cout << (i + 1) << ". " << skillItems[i] << endl;
            }
            int idxItem;
            cout << "Pilih nomor skill baru : "; cin >> idxItem;
            idxItem -= 1;
            if (idxItem >= 0 && idxItem < (int)skillItems.size()) {
                string skillBaru = skillItems[idxItem];
                if (cekSkillPersona(p.skills, skillBaru, idxSkill)) {
                    cout << "Sudah punya skill yang sama, batalkan pembaruan" << endl;
                } else {
                    p.skills[idxSkill] = skillBaru;
                    cout << "Pembaruan berhasil" << endl;
                }
            } else {
                cout << "Nomor skill item tidak ada" << endl;
            }
        } else {
            cout << "Nomor skill persona tidak ada" << endl;
        }
    } else {
        cout << "Nomor persona tidak ada" << endl;
    }
}

void hapusPersonaUser(personaUser* profilePtr) { 
    if ((*profilePtr).listPersona.empty()) { 
        cout << "Kamu belum punya persona apa-apa" << endl;
        return;
    }
    lihatPersonaUser(profilePtr);
    int index = cekInteger("pilih persona yang ingin dihapus : ");
    if (index >= 1 && index <= (int)(*profilePtr).listPersona.size()) { 
        (*profilePtr).listPersona.erase((*profilePtr).listPersona.begin() + (index - 1));
        cout << "persona berhasil dihapus" << endl;
    } else {
        cout << "Nomor persona tidak ada" << endl;
    }
}

void beliSkill(LevelUser* userPtr, personaUser* profilePtr) {
    cout << "\n=== Daftar Skill Item ===" << endl;

    for (int i = 0; i < skillItems.size(); i++) {
        cout << i + 1 << ". " << skillItems[i] 
             << " | Harga: " << hargaSkill[i] << endl;
    }

    int pilih = cekInteger("Pilih skill yang ingin dibeli (0 untuk batal): ");

    if (pilih == 0) {
        cout << "Batal membeli skill" << endl;
        return;
    }

    pilih--;

    if (pilih < 0 || pilih >= skillItems.size()) {
        cout << "Pilihan tidak valid" << endl;
        return;
    }

    if (userPtr->uang < hargaSkill[pilih]) {
        cout << "Uang tidak cukup!" << endl;
        return;
    }

    string skillDibeli = skillItems[pilih];

    profilePtr->inventorySkill.push_back(skillDibeli);
    userPtr->uang -= hargaSkill[pilih];

    cout << "Skill berhasil dibeli!" << endl;
    cout << "Sisa uang: " << userPtr->uang << endl;
}

void userMenu(int userIndex) { 
    int profilIndex = cariAtauBuatProfil(users[userIndex].nama);

    LevelUser* currentUserPtr = &users[userIndex]; 
    personaUser* currentUserProfilePtr = &profilUser[profilIndex]; 

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
        cout << "0. Keluar" << endl;
        cout << "pilihan : ";
        pilihan = cekInteger("masukkan pilihan : ");
        switch (pilihan) {

            case 1: lihatPersonaUser(currentUserProfilePtr); break; 
            case 2: cariPersona(&(currentUserProfilePtr->listPersona), &(currentUserPtr->status)); break; 
            case 3: beliPersona(currentUserPtr, currentUserProfilePtr); break; 
            case 4: fusePersona(currentUserProfilePtr); break; 
            case 5: updateSkillUser(currentUserProfilePtr); break; 
            case 6: sortingPersona(&(currentUserProfilePtr->listPersona), &(currentUserPtr->status)); break; 
            case 7: hapusPersonaUser(currentUserProfilePtr); break;
            case 8: beliSkill(currentUserPtr, currentUserProfilePtr); break; 
            case 0: cout << "Log out" << endl; break;
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
        "root",
        "",
        "persona",
        3306,
        nullptr,
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
        p.nama = row[1];
        p.level = stoi(row[2]);
        p.arcana = row[3];
        p.harga = stoi(row[4]);

        string skillQuery =
            "SELECT s.nama_skill "
            "FROM persona_skills ps "
            "JOIN skill_master s ON ps.skill_id = s.id "
            "WHERE ps.persona_id = " + to_string(personaId) + " "
            "ORDER BY ps.skill_id";

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

vector<LevelUser> loadUsersFromDB(MYSQL* conn) {
    vector<LevelUser> daftar;

    string query =
        "SELECT nama_user, password, uang, status "
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
        u.nama = row[0];
        u.password = row[1];
        u.uang = stoi(row[2]);

        string statusDB = row[3];

        if (statusDB == "admin") {
            u.status = 1;
        } else {
            u.status = 2;
        }

        daftar.push_back(u);
    }

    mysql_free_result(res);

    return daftar;
}

void loadSkillShopFromDB(MYSQL* conn) {
    skillItems.clear();
    hargaSkill.clear();

    string query =
        "SELECT sm.nama_skill, scs.harga "
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
        skillItems.push_back(row[0]);
        hargaSkill.push_back(stoi(row[1]));
    }

    mysql_free_result(res);
}

vector<FusionRule> loadFusionRulesFromDB(MYSQL* conn) {
    vector<FusionRule> daftar;

    string query =
        "SELECT a1.nama_arcana, a2.nama_arcana, ah.nama_arcana "
        "FROM arcana_fusion_matrix afm "
        "JOIN arcana_master a1 ON afm.arcana_1 = a1.id "
        "JOIN arcana_master a2 ON afm.arcana_2 = a2.id "
        "JOIN arcana_master ah ON afm.result_arcana = ah.id";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Query fusion rules gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    if (res == nullptr) {
        cerr << "Ambil data fusion rules gagal: " << mysql_error(conn) << endl;
        return daftar;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        FusionRule f;
        f.arcana1 = row[0];
        f.arcana2 = row[1];
        f.hasilArcana = row[2];

        daftar.push_back(f);
    }

    mysql_free_result(res);

    return daftar;
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
    fusionRules = loadFusionRulesFromDB(conn);

    cout << "Data berhasil diload dari database." << endl;
    cout << "Jumlah user: " << users.size() << endl;
    cout << "Jumlah persona: " << personaUtama.size() << endl;
    cout << "Jumlah skill: " << skillItems.size() << endl;
    cout << "Jumlah fusion rule: " << fusionRules.size() << endl;

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
                    userMenu(currentUserIndex);
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

    panggilPersona(conn);

    mysql_close(conn);
    return 0;
}
