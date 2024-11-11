#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "json/json.h"
#include "jsoncpp.cpp"

using namespace std;

void clearScreen()
{
#ifdef _WIN32
  system("cls"); // Untuk Windows
#else
  system("clear"); // Untuk Linux/Unix/Mac
#endif
}

void validateNumber()
{
  if (cin.fail())
  {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Input harus berupa angka.\n";
  }
}

struct User
{
  string no_kartu;
  string pin;
  string no_pln;
  string no_bpjs;
  string no_asuransi;
  double saldo;
  bool isActive;
};

User loadUserFromJson(const Json::Value &userJson)
{
  User user;
  user.no_kartu = userJson["no_kartu"].asString();
  user.pin = userJson["pin"].asString();
  user.saldo = userJson["saldo"].asDouble();
  user.no_pln = userJson["no_pln"].asString();
  user.no_bpjs = userJson["no_bpjs"].asString();
  user.no_asuransi = userJson["no_asuransi"].asString();
  user.isActive = userJson["isActive"].asBool();
  return user;
}

void updateUserSaldo(Json::Value &usersJson, const User &user)
{
  for (auto &userJson : usersJson)
  {
    if (userJson["no_kartu"].asString() == user.no_kartu)
    {
      userJson["saldo"] = user.saldo; // Update saldo tujuan
      break;
    }
  }
}

void saveJsonToFile(const Json::Value &usersJson, const string &filePath)
{
  ofstream outFile(filePath);
  if (outFile.is_open())
  {
    outFile << usersJson;
    outFile.close();
  }
  else
  {
    cout << "Error: Tidak dapat membuka file untuk menyimpan data." << endl;
  }
}

void saveUserToJson(const User &user, const User &userTujuan = User())
{
  ifstream file("users.json");
  Json::Value usersJson;
  Json::Reader reader;

  if (file.is_open())
  {
    if (!reader.parse(file, usersJson))
    {
      cout << "Error: Gagal membaca data JSON." << endl;
    }
    file.close();
  }

  // Update saldo pengguna yang sedang login
  updateUserSaldo(usersJson, user);

  // Jika userTujuan valid, update saldo pengguna tujuan
  if (!user.no_kartu.empty())
  {
    updateUserSaldo(usersJson, userTujuan);
  }

  // Simpan kembali ke file JSON
  saveJsonToFile(usersJson, "users.json");
}

bool login(User &loggedInUser)
{
  string inputNoKartu, inputPin;
  int attemptCount = 0;
  const int maxAttempts = 3;
  cout << "=====================================\n";
  cout << "           SELAMAT DATANG           \n";
  cout << "           DI ATM CENTER           \n";
  cout << "=====================================\n";

  while (attemptCount < maxAttempts)
  {
    cout << "Masukkan nomor kartu: ";
    cin >> inputNoKartu;
    cout << "Masukkan PIN: ";
    cin >> inputPin;
    clearScreen();

    // Membaca file JSON
    ifstream file("users.json");
    if (!file.is_open())
    {
      cerr << "Error: Tidak dapat membuka file users.json!" << endl;
      return false;
    }

    Json::Value usersJson;
    Json::Reader reader;

    if (!reader.parse(file, usersJson))
    {
      cout << "Error: Gagal membaca data JSON." << endl;
      return false;
    }

    // Memeriksa kredensial
    bool userFound = false;
    for (const auto &userJson : usersJson)
    {
      User user = loadUserFromJson(userJson);
      if (user.no_kartu == inputNoKartu)
      {

        userFound = true;
        if (user.pin == inputPin)
        {
          if (user.isActive)
          {
            loggedInUser = user;
            return true;
          }
          else
          {
            cout << "Akun tidak aktif." << endl;
            return false;
          }
        }
      }
    }

    if (userFound)
    {
      cout << "Nomor kartu atau PIN salah." << endl;
      attemptCount++;
      cout << "PIN ANDA SALAH! Percobaan ke-" << attemptCount << endl; // Increment jumlah kesalahan
    }
    else
    {
      cout << "Nomor kartu tidak ditemukan" << endl;
    }

    // Jika sudah 3 kali percobaan salah, blokir akun
    if (attemptCount >= maxAttempts)
    {
      // Update status isActive menjadi false
      for (auto &userJson : usersJson)
      {
        User user = loadUserFromJson(userJson);
        if (user.no_kartu == inputNoKartu) // Pastikan kita mengupdate user yang tepat
        {
          userJson["isActive"] = false; // Set isActive menjadi false
          break;
        }
      }

      // Menyimpan kembali ke file JSON
      ofstream outFile("users.json");
      if (outFile.is_open())
      {
        outFile << usersJson;
        outFile.close();
      }
      else
      {
        cout << "Error: Tidak dapat membuka file untuk menyimpan data." << endl;
      }

      cout << "Akun Anda telah diblokir setelah 3 kali percobaan yang salah." << endl;
      return false;
    }
  }

  return false;
}

string formatCurrency(double amount)
{
  // Jika nominal adalah puluhan juta
  if (amount >= 10000000 && amount < 100000000)
  {
    // Format untuk puluhan juta
    ostringstream out;
    out << fixed << setprecision(0) << amount; // Menghilangkan desimal
    string str = out.str();

    // Menambahkan pemisah ribuan
    string formatted;
    int count = 0;
    for (int i = str.length() - 1; i >= 0; --i)
    {
      if (count > 0 && count % 3 == 0)
      {
        formatted.insert(0, "."); // Menambahkan pemisah ribuan
      }
      formatted.insert(0, 1, str[i]); // Menambahkan karakter dari belakang
      count++;
    }

    return "Rp" + formatted; // Mengembalikan format yang diinginkan
  }
  // Jika nominal adalah puluhan ribu
  else if (amount >= 10000 && amount < 100000)
  {
    // Format untuk puluhan ribu
    ostringstream out;
    out << fixed << setprecision(0) << amount; // Menghilangkan desimal
    string str = out.str();

    // Menambahkan pemisah ribuan
    string formatted;
    int count = 0;
    for (int i = str.length() - 1; i >= 0; --i)
    {
      if (count > 0 && count % 3 == 0)
      {
        formatted.insert(0, "."); // Menambahkan pemisah ribuan
      }
      formatted.insert(0, 1, str[i]); // Menambahkan karakter dari belakang
      count++;
    }

    return "Rp" + formatted; // Mengembalikan format yang diinginkan
  }

  // Untuk nilai lainnya, kita bisa menggunakan format biasa
  ostringstream out;
  out << fixed << setprecision(0) << amount; // Menghilangkan desimal
  string str = out.str();

  // Menambahkan pemisah ribuan
  string formatted;
  int count = 0;
  for (int i = str.length() - 1; i >= 0; --i)
  {
    if (count > 0 && count % 3 == 0)
    {
      formatted.insert(0, "."); // Menambahkan pemisah ribuan
    }
    formatted.insert(0, 1, str[i]); // Menambahkan karakter dari belakang
    count++;
  }

  return "Rp" + formatted; // Mengembalikan format yang diinginkan
}

void cekSaldo(User &loggedInUser)
{
  cout << "\n========= CEK SALOD =========\n";
  cout << "\nSALDO ANDA SEKARANG : " << formatCurrency(loggedInUser.saldo) << endl;

  char is_continue;
  cout << "\nCek saldo lagi (y/n)? ";
  cin >> is_continue;
  clearScreen();
  if (is_continue == 'y' || is_continue == 'Y')
  {
    cekSaldo(loggedInUser);
  }
}

void setorTunai(User &loggedInUser)
{
  cout << "\n========= SETOR TUNAI =========\n";
  double jumlah;
  cout << "MASUKKAN NOMINAL Rp";
  cin >> jumlah;
  clearScreen();

  validateNumber();
  // Update saldo

  if (jumlah > 0)
  {
    loggedInUser.saldo += jumlah;
    cout << "\nTRANSAKSI SETOR TUNAI SEBESAR " << formatCurrency(jumlah) << " BERHASIL! \n SALDO ANDA SEKARANG : " << formatCurrency(loggedInUser.saldo) << endl;
    saveUserToJson(loggedInUser);
  }
  else
  {
    cout << "\nNilai harus lebih dari 0.\n";
  }

  char is_continue;
  cout << "\nIngin melanjutkan setor tunai lagi (y/n)? ";
  cin >> is_continue;
  clearScreen();
  if (is_continue == 'y' || is_continue == 'Y')
  {
    setorTunai(loggedInUser);
  }
}

void tarikTunai(User &loggedInUser)
{
  cout << "\n========= TARIK TUNAI =========\n";
  double jumlah;
  cout << "MASUKKAN NOMINAL TARIK TUNAI : Rp";
  cin >> jumlah;
  clearScreen();
  validateNumber();

  if (jumlah > 0)
  {
    if (loggedInUser.saldo >= jumlah)
    {
      loggedInUser.saldo -= jumlah;
      cout << "TARIK TUNAI SEBESAR " << formatCurrency(jumlah) << " BERHASIL! \n SALDO ANDA SEKARANG : " << formatCurrency(loggedInUser.saldo) << endl;
      saveUserToJson(loggedInUser);
    }
    else
    {
      cout << "SALDO TIDAK CUKUP!" << endl;
    }
  }
  else
  {
    cout << "\nNilai harus lebih dari 0.\n";
  }

  char is_continue;
  cout << "\nIngin melanjutkan tarik tunai lagi (y/n)? ";
  cin >> is_continue;
  clearScreen();
  if (is_continue == 'y' || is_continue == 'Y')
  {
    tarikTunai(loggedInUser);
  }
}

void transfer(User &loggedInUser)
{
  cout << "\n========= TRANSFER =========\n";
  string no_kartu_tujuan;
  double jumlah;

  cout << "MASUKKAN NO KARTU TUJUAN : ";
  cin >> no_kartu_tujuan;
  // Pengecekan jika nomor kartu tujuan sama dengan nomor kartu pengguna
  if (no_kartu_tujuan == loggedInUser.no_kartu)
  {
    cout << "Error: Anda tidak dapat mentransfer ke nomor kartu sendiri." << endl;
    return; // Keluar dari fungsi jika nomor kartu tujuan sama
  }
  cout << "MASUKKAN NOMINAL TRANSFER : Rp";
  cin >> jumlah;
  clearScreen();
  validateNumber();

  ifstream file("users.json");
  Json::Value usersJson;
  Json::Reader reader;

  if (!file.is_open())
  {
    cerr << "Error: Tidak dapat membuka file users.json!" << endl;
    return;
  }

  if (!reader.parse(file, usersJson))
  {
    cout << "Error: Gagal membaca data JSON." << endl;
    return;
  }
  file.close();

  // Mencari pengguna tujuan
  User userTujuan;
  bool userFound = false;
  for (const auto &userJson : usersJson)
  {
    User user = loadUserFromJson(userJson);
    if (user.no_kartu == no_kartu_tujuan)
    {
      userTujuan = user;
      userFound = true;
      break;
    }
  }

  if (!userFound)
  {
    cout << "Nomor kartu tujuan tidak valid." << endl;
    return;
  }

  if (jumlah > 0)
  {
    if (loggedInUser.saldo >= jumlah)
    {
      loggedInUser.saldo -= jumlah;
      userTujuan.saldo += jumlah;
      cout << "TRANSFER SEBESAR " << formatCurrency(jumlah) << " BERHASIL! \n SALDO ANDA SEKARANG : " << formatCurrency(loggedInUser.saldo) << endl;
      saveUserToJson(loggedInUser, userTujuan);
    }
    else
    {
      cout << "SALDO TIDAK CUKUP!" << endl;
    }
  }
  else
  {
    cout << "\nNilai harus lebih dari 0.\n";
  }

  char is_continue;
  cout << "\nIngin melanjutkan transfer lagi (y/n)? ";
  cin >> is_continue;
  clearScreen();
  if (is_continue == 'y' || is_continue == 'Y')
  {
    transfer(loggedInUser);
  }
}

void pembayaran(User &loggedInUser, const string &jenis)
{
  cout << "\n========= PEMBAYARAN =========\n";
  double jumlah;
  string nomor;

  cout << "MASUKKAN NOMOR " << jenis << " ";
  cin >> nomor;
  cout << "MASUKKAN NOMINAL PEMBAYARAN " << jenis << ": Rp";
  cin >> jumlah;
  clearScreen();
  validateNumber();

  if (nomor.empty())
  {
    cout << "Nomor pembayaran tidak boleh kosong" << endl;
    return;
  }

  bool nomorValid = false;
  if ((jenis == "PLN" && nomor == loggedInUser.no_pln) || (jenis == "BPJS" && nomor == loggedInUser.no_bpjs) || (jenis == "BPJS" && nomor == loggedInUser.no_bpjs) || (jenis == "ASURANSI" && nomor == loggedInUser.no_asuransi))
  {
    nomorValid = true;
    if (jumlah > 0)
    {
      if (loggedInUser.saldo >= jumlah)
      {
        loggedInUser.saldo -= jumlah;
        cout << "PEMBAYARAN " << jenis << " SEBESAR " << formatCurrency(jumlah) << " BERHASIL! \n SALDO ANDA SEKARANG : " << formatCurrency(loggedInUser.saldo) << endl;
        saveUserToJson(loggedInUser);
      }
      else
      {
        cout << "SALDO TIDAK CUKUP!" << endl;
      }
    }
    else
    {
      cout << "\nNilai harus lebih dari 0.\n";
    }
  }

  if (!nomorValid)
  {
    cout << "Nomor pembayaran tidak valid" << endl;
    return; // Keluar dari fungsi jika nomor tidak valid
  }

  char is_continue;
  cout << "\nIngin melanjutkan pembayaran lagi (y/n)? ";
  cin >> is_continue;
  clearScreen();
  if (is_continue == 'y' || is_continue == 'Y')
  {
    pembayaran(loggedInUser, jenis);
  }
}

void menu(User &loggedInUser)
{
  int choice;
  do
  {
    cout << "\n========= MENU ATM =========" << endl;
    cout << "=============================" << endl;
    cout << "1. Cek Saldo" << endl;
    cout << "2. Setor Tunai" << endl;
    cout << "3. Tarik Tunai" << endl;
    cout << "4. Transfer" << endl;
    cout << "5. Pembayaran" << endl;
    cout << "6. Keluar" << endl;
    cout << "=============================" << endl;
    cout << "Pilih menu [1-6]? : ";
    cin >> choice;
    clearScreen();
    validateNumber();

    switch (choice)
    {
    case 1:
      cekSaldo(loggedInUser);
      break;
    case 2:
      setorTunai(loggedInUser);
      break;
    case 3:
      tarikTunai(loggedInUser);
      break;
    case 4:
      transfer(loggedInUser);
      break;
    case 5:
      int paymentChoice;
      cout << "Pilih jenis pembayaran :\n";
      cout << "1. PLN\n";
      cout << "2. BPJS\n";
      cout << "3. ASURANSI\n";
      cout << "4. KEMBALI\n";
      cout << "Pilih menu [1-4]? :";
      cin >> paymentChoice;
      clearScreen();

      switch (paymentChoice)
      {
      case 1:
        pembayaran(loggedInUser, "PLN");
        break;
      case 2:
        pembayaran(loggedInUser, "BPJS");
        break;
      case 3:
        pembayaran(loggedInUser, "ASURANSI");
        break;
      case 4:
        cout << "Kembali ke menu utama..." << endl;
        clearScreen();
        break;
      default:
        cout << "Pilihan tidak valid!" << endl;
      }
      break;
    case 6:
      cout << "Terima kasih telah menggunakan ATM Center!" << endl;
      break;
    default:
      cout << "Pilihan tidak valid!" << endl;
    }
  } while (choice != 6);
}

int main()
{
  User loggedInUser;

  if (login(loggedInUser))
  {
    menu(loggedInUser);
  }

  return 0;
}
