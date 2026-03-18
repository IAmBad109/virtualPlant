#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <cstdlib>

using namespace std;

// =====================================================
// Dokumente-Pfad des Users ermitteln
// =====================================================
string getDocumentsPath() {
    char* buffer = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&buffer, &len, "USERPROFILE");

    string path;
    if (err == 0 && buffer != nullptr) {
        path = string(buffer) + "\\Documents\\plant_status.txt";
        free(buffer);
    }
    else {
        path = "plant_status.txt";
    }

    return path;
}

// =====================================================
// Level berechnen basierend auf Wachstum
// =====================================================
int calculateLevel(int growth) {
    return growth / 5 + 1; // alle 5 Wachstumspunkte = 1 Level
}

// =====================================================
// Pflanze Datenstruktur
// =====================================================
struct Plant {
    int growth;       // Wachstumspunkte
    int water;        // Wasserlevel (0-30)
    int health;       // Lebenspunkte
    int level;        // Level
    int disease;      // 0 = gesund, 1 = trocken, 2 = Wurzelfäule
    int diseaseTimer; // verbleibende Runden Krankheit

    Plant() {
        // Standardwerte
        growth = 0;
        water = 10;
        health = 20;
        level = 1;
        disease = 0;
        diseaseTimer = 0;

        // Versuche Spielstand zu laden
        string path = getDocumentsPath();
        ifstream file(path);
        if (!file) return;

        string line;
        while (getline(file, line)) {
            size_t sep = line.find(':');
            if (sep == string::npos) continue;

            string key = line.substr(0, sep);
            string valueStr = line.substr(sep + 1);

            int value = stoi(valueStr);

            if (key == "Growth") growth = value;
            else if (key == "Water") water = value;
            else if (key == "Health") health = value;
            else if (key == "Level") level = value;
            else if (key == "Disease") disease = value;
            else if (key == "DiseaseTimer") diseaseTimer = value;
        }

        file.close();
    }
};

// =====================================================
// Spielstand speichern
// =====================================================
void saveStatusToFile(const Plant& p) {
    ofstream file(getDocumentsPath());
    if (!file) return;

    file << "Growth:" << p.growth << "\n";
    file << "Water:" << p.water << "\n";
    file << "Health:" << p.health << "\n";
    file << "Level:" << p.level << "\n";
    file << "Disease:" << p.disease << "\n";
    file << "DiseaseTimer:" << p.diseaseTimer << "\n";

    file.close();
}

// =====================================================
// ASCII-Pflanze anzeigen
// =====================================================
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

// =====================================================
// Status anzeigen und speichern
// =====================================================
void showStatus(const Plant& p) {
    int maxHealth = 20 + p.level * 5;

    cout << "\nStatus:\n";
    cout << "Level    : " << p.level << "\n";
    cout << "Growth   : " << p.growth << "\n";
    cout << "Water    : " << p.water << "\n";
    cout << "Health   : " << p.health << "/" << maxHealth << "\n";

    cout << "Disease  : ";
    if (p.disease == 0) cout << "Healthy\n";
    else if (p.disease == 1) cout << "Dryness\n";
    else if (p.disease == 2) cout << "Root Rot\n";

    saveStatusToFile(p);
}

// =====================================================
// Update-Funktion (jede Stunde)
// =====================================================
void update(Plant& p) {
    p.growth += 2;  // Wachstum pro Stunde
    p.water -= 1;   // Wasserverbrauch

    // Krankheiten entstehen
    if (p.water <= 2 && p.disease == 0) {
        p.disease = 1;
        p.diseaseTimer = 5;
        cout << "The plant is getting dry!\n";
    }

    if (p.water >= 28 && p.disease == 0) {
        p.disease = 2;
        p.diseaseTimer = 5;
        cout << "Too much water! Root rot started!\n";
    }

    // Krankheit aktiv
    if (p.disease != 0) {
        p.diseaseTimer--;

        if (p.disease == 1) {
            p.health -= 1; // geringer Schaden
            cout << "Dryness hurts the plant...\n";
        }
        else if (p.disease == 2) {
            p.health -= 1; // geringer Schaden
            cout << "Root rot damages the plant...\n";
        }

        // Wachstum während Krankheit
        p.growth += 1;

        // Krankheit endet automatisch
        if (p.diseaseTimer <= 0) {
            p.disease = 0;
            cout << "The plant recovered naturally.\n";
        }
    }

    // Regeneration
    int maxHealth = 20 + p.level * 5;
    if (p.water >= 8 && p.disease == 0 && p.health < maxHealth) {
        p.health += 2;
    }

    // Level-System
    int oldLevel = p.level;
    p.level = calculateLevel(p.growth);

    if (p.level > oldLevel) {
        cout << "LEVEL UP! New Level: " << p.level << "\n";
        p.health += 5;
        p.water += 3;
    }

    // Limits setzen
    if (p.water < 0) p.water = 0;
    if (p.water > 30) p.water = 30;
    if (p.health < 0) p.health = 0;
    if (p.health > maxHealth) p.health = maxHealth;

    cout << "\nUpdate:\n";
    showStatus(p);
}

// =====================================================
// Pflanze gießen
// =====================================================
void waterPlant(Plant& p) {
    p.water += 4;
    if (p.water > 30) p.water = 30;

    cout << "You watered the plant!\n";

    if (p.disease == 1) {
        p.disease = 0;
        cout << "The plant recovered from dryness!\n";
    }

    showStatus(p);
}

// =====================================================
// Hauptprogramm
// =====================================================
int main() {
    cout << "=== PLANT SIMULATOR ===\n";
    cout << "Welcome to Plant Simulator.\n";
    cout << "Your plant starts with:\n";
    cout << "  Water: 10 (max 30)\n";
    cout << "  Health: 20 (increases with level, max = 20 + level*5)\n";
    cout << "  Growth: 0\n";
    cout << "  Level: 1\n\n";

    cout << "Controls:\n";
    cout << "  g = water plant (+4 water)\n";
    cout << "  c = clear screen\n\n";

    cout << "Rules:\n";
    cout << "- Water < 3 -> Dryness (health -1 per hour)\n";
    cout << "- Water > 27 -> Root Rot (health -1 per hour)\n";
    cout << "- Sick plants gain extra growth (+1 per hour)\n";
    cout << "- Healthy plant regenerates health if water >= 8\n\n";

    Plant plant;

    while (plant.health > 0) {
        showPlantVisual();
        showStatus(plant);

        auto start = chrono::steady_clock::now();
        auto duration = chrono::seconds(20); // 20 Sekunden pro Update

        while (chrono::steady_clock::now() - start < duration) {
            if (_kbhit()) {
                char c = _getch();
                if (c == 'g' || c == 'G') waterPlant(plant);
                if (c == 'c' || c == 'C') system("cls");
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }

        update(plant);
    }

    cout << "The plant has died...\n";
    system("pause");
    return 0;
}