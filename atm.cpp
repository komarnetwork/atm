#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <locale>
#include <limits>
// #include "json/json.h"
// #include "jsoncpp.cpp"
#include <fstream>

using namespace std;

void clearScreen()
{
#ifdef _WIN32
	system("cls"); // Untuk Windows
#else
	system("clear"); // Untuk Linux/Unix/Mac
#endif
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

int getOption(int &PIN);

int main(){
	// ifstream file("data.json");
	// Json::Value actualJson;
	// Json::Reader reader;

	// reader.parse(file, actualJson);

	// cout << "Total json data:\n"
	// 		 << actualJson << endl;

	// cout << "Name: " << actualJson["Name"] << endl;

	int pilihan;
	char is_continue;
	double n = 0.0, i;
	enum option
	{
		CEK_SALDO = 1,
		SETOR_TUNAI,
		TARIK_TUNAI,
		PEMBAYARAN,
		FINISH
	};

	int PIN = 0;							// Inisialisai PIN
	pilihan = getOption(PIN); // meminta PIN dan mendapatkan pilihan menu

	if (pilihan == 0)
	{
		cout << "Akses diblokir. Program akan segera dihentikan." << endl;
		return 0;
	}

	// jika PIN benar tampilkan menunya
	while (pilihan != FINISH)
	{
		switch(pilihan){
		case CEK_SALDO:
			cout << "\n========= CEK SALOD =========\n";
			cout << "\nSALDO ANDA SEKARANG : " << formatCurrency(n) << endl;
			break;
		case SETOR_TUNAI:
			cout << "\n========= SETOR TUNAI =========\n";
			while (true)
			{
				cout << "MASUKAN TUNAI YANG INGIN ANDA SETOR : ";
				cin >> i;
				clearScreen();

				if (cin.fail())
				{
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Input harus berupa angka.\n";
					continue;
				}

				if (i > 0)
				{
					cout << "\nTRANSAKSI SETOR TUNAI SUKSES\n";
					n += i;
					cout << "\nTOTAL SALDO ANDA SEKARANG ADALAH : " << formatCurrency(n) << endl;
				}
				else
				{
					cout << "\nNilai harus lebih dari 0.\n";
				}

				char depositLagi;
				cout << "\nIngin melakukan setor tunai lagi? (y/n) : ";
				cin >> depositLagi;
				clearScreen();
				if (depositLagi == 'n' || depositLagi == 'N')
				{
					break;
				}
			}
			break;
		case TARIK_TUNAI:
			cout << "\n========= TARIK TUNAI =========\n";

			while (true)
			{
				cout << "MASUKAN NOMINAL YANG INGIN ANDA TARIK : ";
				cin >> i;
				clearScreen();

				if (cin.fail())
				{
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Input harus berupa angka.\n";
					continue;
				}

				if (i > 0)
				{
					if (n >= i)
					{
						n -= i;
						cout << "\nBERHASIL TARIK TUNAI\n";
						cout << "\nTOTAL SALDO ANDA SEKARANG ADALAH : " << formatCurrency(n) << endl;
					}
					else
					{
						cout << "\nSALDO ANDA TIDAK CUKUP.\n";
					}
				}
				else
				{
					cout << "\nNilai harus lebih dari 0.\n";
				}

				char tarikTunaiLagi;
				cout << "\nIngin melakukan tarik tunai lagi? (y/n) : ";
				cin >> tarikTunaiLagi;
				if (tarikTunaiLagi == 'n' || tarikTunaiLagi == 'N')
				{
					break;
				}
			}
			break;
		case PEMBAYARAN:
			cout << "PEMBAYARAN" << endl;
			break;
		default:
			cout << "Pilihan tidak ditemukan" << endl;
			break;
		}

	label_continue:
		cout << "\nIngin melakukan transaksi lain? (y/n) : ";
		cin >> is_continue;
		clearScreen();
		if ((is_continue == 'y') | (is_continue == 'Y'))
		{
			pilihan = getOption(PIN);
			while (pilihan < 1 || pilihan > 5)
			{
				cout << "Pilihan tidak valid. Silahkan pilih menu [1-5]: " << endl;
				pilihan = getOption(PIN);
			}
		}
		else if ((is_continue == 'n') | (is_continue == 'N'))
		{
			break;
		}
		else
		{
			goto label_continue;
		}
		}

	cout << "Terimakasih telah menggunakan ATM CENTER" << endl;

	cin.get();
	return 0;
}

int getOption(int &PIN)
{
	const int myPIN = 223311;
	int input;
	int attemptCount = 0;

	// jika PIN belum dimasukkan
	if (PIN == 0)
	{
		cout << "========= SELAMAT DATANG DI ATM CENTER =========\n";
		while (attemptCount < 3)
		{
			cout << "SILAHKAN MASUKKAN PIN ANDA : ";
			string pinInput;
			cin >> pinInput;

			clearScreen();

			bool isValidPin = true;
			for (size_t i = 0; i < pinInput.length(); ++i)
			{
				if (!isdigit(pinInput[i]))
				{
					isValidPin = false;
					break;
				}
			}

			if (isValidPin)
			{
				PIN = stoi(pinInput);
			}
			else
			{
				cout << "Input PIN tidak valid." << endl;
				continue;
			}

			// cek PIN apakah benar?
			if (PIN != myPIN)
			{
				attemptCount++;
				cout << "\nPIN ANDA SALAH! Percobaan ke- " << attemptCount << endl;
				if (attemptCount >= 3)
				{
					cout << "\nPIN ANDA DI BLOKIR\n";
					return 0;
				}
			}
			else
			{
				break;
			}
		}
	}

	cout << "\n========= MENU ATM =========" << endl;
	cout << "=============================" << endl;
	cout << "1. Cek Saldo" << endl;
	cout << "2. Setor Tunai" << endl;
	cout << "3. Tarik Tunai" << endl;
	cout << "4. Pembayaran" << endl;
	cout << "5. Keluar" << endl;
	cout << "=============================" << endl;
	cout << "Pilih menu [1-5]? : ";

	cin >> input;
	clearScreen();
	return input;
}