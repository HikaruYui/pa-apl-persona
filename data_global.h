#ifndef DATA_GLOBAL_H
#define DATA_GLOBAL_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cctype>
#include <stdexcept>
#include <mysql/mysql.h>
#include <tabulate/table.hpp>

#define max_persona_user 10
#define max_skill_persona 6
#define max_skill_warisan 2

using namespace std;
using namespace tabulate;

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
    int original_id;
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


vector<LevelUser> users;
vector<persona> personaUtama;
vector<personaUser> profilUser;
vector<skillCard> skillCardShop;

#endif