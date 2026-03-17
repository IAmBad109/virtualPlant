#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>

using namespace std;

struct Plant {
    int growth = 0;
    int water = 10;
    int health = 20;
};

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

void showStatus(const Plant& p) {
    cout << "\nStatus:\n";
    cout << "Growth   : " << p.growth << "\n";
    cout << "Water    : " << p.water << "\n";
    cout << "Health   : " << p.health << "\n";
}

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

    while (plant.health > 0) {
        cout << "Press 'g' to water the plant, otherwise wait 1 hour. Press c for clearing the screen.\n";
        showPlantVisual();

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