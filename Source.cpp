#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <cstdlib>  // für _dupenv_s

using namespace std;

struct Plant {
    int growth = 0;
    int water = 10;
    int health = 20;
};

// sichere Methode, um Dokumente-Ordner-Pfad zu bekommen
string getDocumentsPath() {
    char* buffer = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&buffer, &len, "USERPROFILE"); // Windows Home
    string path;
    if (err == 0 && buffer != nullptr) {
        path = string(buffer) + "\\Documents\\plant_status.txt";
        free(buffer);
    }
    else {
        path = "plant_status.txt"; // Fallback ins aktuelle Verzeichnis
    }
    return path;
}

// Status aus Datei laden
void loadStatusFromFile(Plant& p) {
    string path = getDocumentsPath();
    ifstream file(path);
    if (!file) return; // Datei existiert nicht ? Standardwerte bleiben

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string key;
        char sep;
        int value;
        if (iss >> key >> sep >> value) {
            if (key == "Growth") p.growth = value;
            else if (key == "Water") p.water = value;
            else if (key == "Health") p.health = value;
        }
    }
    file.close();
}

// Status in Datei speichern
void saveStatusToFile(const Plant& p) {
    string path = getDocumentsPath();
    ofstream file(path);
    if (!file) {
        cerr << "Fehler: Konnte Datei nicht speichern!\n";
        return;
    }

    file << "Growth:" << p.growth << "\n";
    file << "Water:" << p.water << "\n";
    file << "Health:" << p.health << "\n";

    file.close();
}

// ASCII Pflanze
void showPlantVisual() {
    cout << "      _-_      \n";
    cout << "   /~~   ~~\\   \n";
    cout << "  /~~     ~~\\ \n";
    cout << " {           } \n";
    cout << "  \\  _-  -_ / \n";
    cout << "    ~  | |  ~   \n";
    cout << " _- -  | | _- _\n";
    cout << "   _ - | |   -_\n";
    cout << "      // \\\\    \n";
}

// Status anzeigen + speichern
void showStatus(const Plant& p) {
    cout << "\nStatus:\n";
    cout << "Growth   : " << p.growth << "\n";
    cout << "Water    : " << p.water << "\n";
    cout << "Health   : " << p.health << "\n";

    saveStatusToFile(p);
}

// stündliches Update
void update(Plant& p) {
    p.growth += 1;
    p.water -= 1;

    if (p.water <= 5) {
        p.health -= 2;
    }

    if (p.water < 0) p.water = 0;
    if (p.health < 0) p.health = 0;

    cout << "\nHourly update:\n";
    showStatus(p);
}

// Pflanze gießen
void waterPlant(Plant& p) {
    if (p.water >= 30) {
        cout << "The plant already has maximum water!\n";
    }
    else {
        p.water += 3;
        if (p.water > 30) p.water = 30;
        cout << "You watered the plant!\n";
    }
    showStatus(p);
}

int main() {
    Plant plant;
    loadStatusFromFile(plant); // Werte aus Datei laden

    while (plant.health > 0) {
        cout << "Press 'g' to water the plant, otherwise wait 1 hour. Press 'c' to clear the screen.\n";

        showPlantVisual();
        showStatus(plant);

        auto start = chrono::steady_clock::now();
        auto duration = chrono::hours(1); // 1 hour real time

        while (chrono::steady_clock::now() - start < duration) {
            if (_kbhit()) {
                char c = _getch();
                if (c == 'g' || c == 'G') {
                    waterPlant(plant);
                }
                if (c == 'c' || c == 'C') {
                    system("cls");
                    showPlantVisual();
                    showStatus(plant);
                }
            }
            this_thread::sleep_for(chrono::milliseconds(1000));
        }

        update(plant);
    }

    cout << "The plant has died...\n";
    system("pause");
    return 0;
}