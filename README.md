# PA APL Persona — Velvet Room

Merupakan refactor dari game persona 3 berbasis CLI dan C++ untuk mengelola data Persona, user, skill, pembelian Persona, pembelian skill card, fusion Persona, dan special fusion. Project ini menggunakan database MariaDB/MySQL sebagai penyimpanan utama.

Project ini dibuat sebagai aplikasi simulasi sistem Persona dengan konsep Velvet Room. User dapat login, membeli Persona, membeli skill card, menggabungkan Persona, memilih skill warisan, melakukan special fusion, dan mengelola koleksi Persona miliknya. Admin dapat mengelola data master Persona, melakukan pencarian, sorting, penambahan, update, dan penghapusan data Persona.
Data utama seperti user, Persona, arcana, skill, fusion matrix, inventory skill, dan recipe special fusion disimpan di database `persona`.

## Fitur 

### Fitur Umum

- Login dan register user
- Validasi input angka dan teks
- Penyimpanan data ke database MariaDB/MySQL
- Tampilan data berbentuk tabel di terminal
- Pemisahan role antara admin dan user

### Fitur Admin

- Melihat seluruh data Persona
- Mencari Persona berdasarkan nama atau level
- Sorting Persona berdasarkan:
  - Nama A-Z
  - Nama Z-A
  - Level terendah ke tertinggi
  - Level tertinggi ke terendah
- Menambah Persona baru
- Mengubah data Persona
- Menghapus Persona
- Otomatis membuat arcana atau skill baru jika belum tersedia di database

### Fitur User

- Melihat koleksi Persona milik user
- Membeli Persona
- Membeli skill card
- Mengubah skill Persona menggunakan skill card
- Menghapus Persona dari koleksi
- Fusion Persona biasa
- Special Fusion
- Pemilihan skill warisan maksimal 2 skill
- Persona special dengan harga 0 tidak dapat dibeli langsung dan hanya didapat dari special fusion

## Struktur File

| File | Fungsi |
|---|---|
| `main.cpp` | Entry point program, koneksi database, load data awal, dan menu login/register |
| `data_global.h` | Struct, variabel global, konstanta, dan include utama |
| `utils.h` | Helper validasi input, pencarian user, pengecekan duplikasi, dan penggabungan teks skill |
| `database.h` | Koneksi database dan query load/insert/update data utama |
| `auth.h` | Login dan registrasi user |
| `menu.h` | Menu admin dan menu user |
| `admin.h` | Fitur CRUD, sorting, dan searching Persona untuk admin |
| `user.h` | Fitur user seperti beli Persona, fusion, skill card, dan special fusion |
| `persona.sql` | Struktur dan data awal database |

## Struktur Database

Database yang digunakan bernama:

```sql
persona.sql
```

Tabel utama:

| Tabel | Fungsi |
|---|---|
| `users` | Menyimpan akun user dan admin |
| `persona` | Menyimpan data master Persona |
| `arcana_master` | Menyimpan daftar arcana |
| `skill_master` | Menyimpan daftar skill |
| `persona_skills` | Relasi Persona master dengan skill |
| `arcana_fusion_matrix` | Matriks hasil fusion berdasarkan arcana |
| `user_persona_collection` | Koleksi Persona milik user |
| `user_persona_equipped_skills` | Skill yang dimiliki Persona user |
| `skill_card_shop` | Daftar skill card yang dijual |
| `inventori_user` | Inventory skill card milik user |
| `special_fusion_recipe` | Resep hasil special fusion |
| `special_fusion_material` | Material Persona untuk special fusion |

## Akun Default

Berdasarkan data awal di `persona.sql`, tersedia akun berikut:

| Username | Password | Role |
|---|---|---|
| `igor` | `4444` | Admin |
| `ozora` | `038` | User |

## Persiapan Database

### 1. Masuk ke MariaDB sebagai root

```bash
sudo mariadb
```

### 2. Buat database dan user aplikasi

```sql
DROP DATABASE IF EXISTS persona;
CREATE DATABASE persona CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

CREATE USER IF NOT EXISTS 'apluser'@'localhost' IDENTIFIED BY '';
CREATE USER IF NOT EXISTS 'apluser'@'127.0.0.1' IDENTIFIED BY '';

GRANT ALL PRIVILEGES ON persona.* TO 'apluser'@'localhost';
GRANT ALL PRIVILEGES ON persona.* TO 'apluser'@'127.0.0.1';

FLUSH PRIVILEGES;
EXIT;
```

### 3. Import database

Jalankan dari folder project:

```bash
mariadb -u apluser -h 127.0.0.1 -P 3306 persona < persona.sql
```

### 4. Cek database

```bash
mariadb -u apluser -h 127.0.0.1 -P 3306 persona
```

## Konfigurasi default:

| Field | Nilai |
|---|---|
| Host | `127.0.0.1` |
| User | `apluser` |
| Password | kosong |
| Database | `persona` |
| Port | `3306` |

## Cara Compile

### Linux

Pastikan MariaDB/MySQL development library sudah tersedia.

Contoh compile:

```bash
g++ -std=c++17 main.cpp -o main -Itabulate-master/include -lmysqlclient -Wno-sfinae-incomplete
```

Jalankan program:

```bash
./main
```

### Windows

Jika menggunakan MinGW dan library MySQL/MariaDB sudah tersedia di folder project, sesuaikan path include dan library dengan environment masing-masing.

Contoh umum:

```bash
g++ -std=c++17 main.cpp -o main.exe -Itabulate-master/include -Iinclude -Llib -lmysql
```

Jalankan:

```bash
main.exe
```

## Alur Penggunaan

### Admin

1. Jalankan program
2. Login menggunakan akun admin:
   - Username: `igor`
   - Password: `4444`
3. Masuk ke menu admin
4. Kelola data Persona melalui menu:
   - Tampilkan data Persona
   - Cari Persona
   - Tambah Persona
   - Update Persona
   - Hapus Persona
   - Sorting Persona

### User

1. Jalankan program
2. Login menggunakan akun user atau register akun baru
3. Masuk ke menu user
4. Gunakan fitur:
   - Lihat Persona user
   - Beli Persona
   - Fusion Persona
   - Ubah skill Persona
   - Beli skill card
   - Special Fusion

## Catatan Penting

- Jangan menjalankan aplikasi menggunakan user database `root`.
- Gunakan `root` hanya untuk setup database, membuat database, dan memberi privilege.
- Aplikasi sebaiknya tetap memakai user database khusus, yaitu `apluser`.
- Jika import SQL gagal karena tabel sudah ada, reset database terlebih dahulu.
- Jika program menampilkan `Jumlah user: 0`, cek apakah struktur tabel `inventori_user` sudah memiliki kolom `jumlah_skill_card`.

## Tim Pengembang
* Diftya Azzahra
* Febrianno Ozora Alinanto
* Anah Wiyana
* Muhammad Adrian