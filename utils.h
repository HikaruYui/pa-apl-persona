#ifndef UTILS_H
#define UTILS_H

#include "data_global.h"

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

string cekString(const string& prompt) {
    string input;
    
    while(true) {
        try {
            cout << prompt;
            getline(cin, input);

            if (input.empty()) {
                throw invalid_argument("Error: Input tidak boleh kosong");
            } 

            for (int i = 0; i < (int)input.length(); i++) {
                if (!isalnum((unsigned char)input[i]) && !isspace((unsigned char) input[i])) {
                    throw invalid_argument("Input tidak boleh mengandung simbol");
                }
            }

            return input;
            }
            catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
    }

bool konfirmasiString(const string& prompt) {
    while (true) {
        string jawaban = cekString(prompt);

        if (jawaban == "y" || jawaban == "Y") {
            return true;
        }

        if (jawaban == "n" || jawaban == "N") {
            return false;
        }

        cout << "Error: Pilihan hanya boleh y atau n" << endl;
    }
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

bool hanyaSpasi(const string& teks) {
    for (int i = 0; i < (int)teks.length(); i++) {
        if (!isspace((unsigned char)teks[i])) {
            return false;
        }
    }
    return true;
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

string gabungSkill (const vector<string>& skills) {
    string hasil = "";

    for (int i = 0; i < (int)skills.size(); i++) {
        hasil += skills[i];

    if (i < (int)skills.size() - 1) {
        hasil += ", ";
    }
    }
    return hasil;
    
}

#endif