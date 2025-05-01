#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// Fonction pour simuler l'algorithme de remplacement de page FIFO
int fifo(const std::vector<int>& pages, int capacity, std::vector<int>& frameContents, int& pageFaults) {
    pageFaults = 0;
    std::vector<int> frame(capacity, -1); // Initialise le frame comme étant vide
    int frameIndex = 0;

    for (int page : pages) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == page) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            pageFaults++;
            frame[frameIndex] = page;
            frameIndex = (frameIndex + 1) % capacity; // Buffer circulaire
        }
        //pour le débogage
        /*std::cout << "Frame: ";
        for(int j=0; j<capacity; j++){
           std::cout << frame[j] << " ";
        }
        std::cout << std::endl;*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page Optimal
int optimal(const std::vector<int>& pages, int capacity, std::vector<int>& frameContents, int& pageFaults) {
    pageFaults = 0;
    std::vector<int> frame(capacity, -1); // Initialise le frame comme étant vide

    for (int i = 0; i < pages.size(); i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            pageFaults++;
            int farthest = -1, replaceIndex = -1;
            for (int j = 0; j < capacity; j++) {
                int k;
                for (k = i + 1; k < pages.size(); k++) {
                    if (frame[j] == pages[k]) {
                        if (k > farthest) {
                            farthest = k;
                            replaceIndex = j;
                        }
                        break;
                    }
                }
                if (k == pages.size()) { // Non trouvé dans le futur
                    replaceIndex = j;
                    break;
                }
            }
            if(replaceIndex == -1){
                 replaceIndex = 0;
            }
            frame[replaceIndex] = pages[i];
        }
        //pour le débogage
        /*std::cout << "Frame: ";
        for(int j=0; j<capacity; j++){
           std::cout << frame[j] << " ";
        }
        std::cout << std::endl;*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page LRU
int lru(const std::vector<int>& pages, int capacity, std::vector<int>& frameContents, int& pageFaults) {
    pageFaults = 0;
    std::vector<int> frame(capacity, -1); // Initialise le frame comme étant vide
    std::vector<int> time(capacity, 0); // Garde la trace du moment où chaque page a été utilisée pour la dernière fois
    int currentTime = 0;

    for (int i = 0; i < pages.size(); i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                time[j] = currentTime++; // Met à jour le temps de la dernière utilisation
                break;
            }
        }
        if (!pageHit) {
            pageFaults++;
            int lruIndex = 0, minTime = time[0];
            for (int j = 1; j < capacity; j++) {
                if (time[j] < minTime) {
                    minTime = time[j];
                    lruIndex = j;
                }
            }
            frame[lruIndex] = pages[i];
            time[lruIndex] = currentTime++;
        }
        //pour le débogage
        /*std::cout << "Frame: ";
        for(int j=0; j<capacity; j++){
           std::cout << frame[j] << " ";
        }
        std::cout << std::endl;*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page Second Chance
int secondChance(const std::vector<int>& pages, int capacity, std::vector<int>& frameContents, int& pageFaults) {
    pageFaults = 0;
    std::vector<int> frame(capacity, -1); // Initialise le frame comme étant vide
    std::vector<bool> referenced(capacity, false); // Garde la trace du bit de référence
    int frameIndex = 0;

    for (int page : pages) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == page) {
                pageHit = true;
                referenced[j] = true; // Met le bit de référence
                break;
            }
        }
        if (!pageHit) {
            pageFaults++;
            while (true) {
                if (!referenced[frameIndex]) {
                    frame[frameIndex] = page;
                    referenced[frameIndex] = true; // Met le bit de référence pour la nouvelle page
                    frameIndex = (frameIndex + 1) % capacity;
                    break;
                } else {
                    referenced[frameIndex] = false; // Efface le bit de référence
                    frameIndex = (frameIndex + 1) % capacity;
                }
            }
        }
        //pour le débogage
        /*std::cout << "Frame: ";
        for(int j=0; j<capacity; j++){
           std::cout << frame[j] << " ";
        }
        std::cout << std::endl;*/
    }
    return 0;
}

// Fonction pour obtenir l'entrée de l'utilisateur pour les références de page et la taille du frame
int getUserInput(std::vector<int>& pages, int& n, int& capacity) {
    int choice;
    std::cout << "1. Pour saisir manuellement les données\n";
    std::cout << "2. Pour charger les données à partir d'un fichier texte\n";
    std::cout << "3. Pour revenir au menu principal\n";
    std::cout << "Entrez votre choix : ";
    std::cin >> choice;

    switch (choice) {
        case 1: {
            std::cout << "Entrez le nombre de références de pages : ";
            std::cin >> n;
            pages.resize(n);
            std::cout << "Entrez les références de pages (séparées par des espaces) : ";
            for (int i = 0; i < n; i++) {
                std::cin >> pages[i];
            }
            std::cout << "Entrez le nombre de pages de la mémoire tampon : ";
            std::cin >> capacity;
            break;
        }
        case 2: {
            std::string filename;
            std::cout << "Entrez le nom du fichier texte : ";
            std::cin >> filename;
            std::ifstream file(filename);
            if (!file.is_open()) {
                perror("Erreur lors de l'ouverture du fichier");
                return -1;
            }
            // D'abord, lire le nombre de pages
            if (!(file >> n)) {
                std::cerr << "Erreur lors de la lecture du nombre de pages à partir du fichier.\n";
                file.close();
                return -1;
            }

            pages.resize(n);
            // Ensuite, lire les références de page
            for (int i = 0; i < n; i++) {
                if (!(file >> pages[i])) {
                    std::cerr << "Erreur lors de la lecture de la référence de page " << i + 1 << " à partir du fichier.\n";
                    file.close();
                    pages.clear();
                    return -1;
                }
            }
            // Enfin, lire la capacité
            if (!(file >> capacity)) {
                std::cerr << "Erreur lors de la lecture de la capacité à partir du fichier.\n";
                file.close();
                pages.clear();
                return -1;
            }
            file.close();
            break;
        }
        case 3:
            return 1; // Retour au menu principal
        default:
            std::cout << "Choix invalide.\n";
            return -1;
    }
    return 0;
}

// Fonction pour afficher les résultats de la simulation
void displayResults(const std::string& algorithmName, int pageFaults) {
    std::cout << "Algorithme " << algorithmName << ": " << pageFaults << " fautes de page\n";
}

// Fonction pour sauvegarder les résultats dans un fichier
void saveResults(const std::string& algorithmName, int pageFaults, const std::string& filename) {
    std::ofstream file(filename, std::ios::app); // Ajouter au fichier
    if (!file.is_open()) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    file << "Algorithme " << algorithmName << " | " << pageFaults << "\n";
    file.close();
}

// Fonction pour afficher les résultats sauvegardés à partir du fichier
void displaySavedResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    std::string line;
    std::cout << "\n--- Résultats des Exécutions Précédentes ---\n";
    while (std::getline(file, line)) {
        std::cout << line << "\n"; // Affiche chaque ligne du fichier
    }
    file.close();
}

// Fonction pour afficher la simulation graphiquement.
void displaySimulation(const std::vector<int>& pages, int capacity, const std::vector<int>& frameContents, const std::string& algorithmName) {
    int pageFaults = 0;
    std::vector<int> frame(capacity, -1);
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
    }

    if (algorithmName == "FIFO") {
        fifo(pages, capacity, frameContents, pageFaults);
    } else if (algorithmName == "Optimal") {
        optimal(pages, capacity, frameContents, pageFaults);
    } else if (algorithmName == "LRU") {
        lru(pages, capacity, frameContents, pageFaults);
    } else if (algorithmName == "Second Chance") {
        secondChance(pages, capacity, frameContents, pageFaults);
    }

    std::cout << "\nRéférences de pages logiques: ";
    for (int page : pages) {
        std::cout << page << " ";
    }
    std::cout << "\n\n";

    std::cout << "Pages physiques -->\n";
    for (int i = 0; i < pages.size(); i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            std::cout << "F ";
        } else {
            std::cout << "  ";
        }
    }
    std::cout << "\n";
    std::cout << "Pages logiques -->\n";
    for (int i = 0; i < pages.size(); i++) {
        std::cout << "  ";
    }
    std::cout << "\n";

    if (algorithmName == "Second Chance") {
        std::vector<int> frame2(capacity, -1);
        std::vector<bool> referenced(capacity, false);
        int frameIndex = 0;
        for (int i = 0; i < pages.size(); i++) {
            bool pageHit = false;
            for (int j = 0; j < capacity; j++) {
                if (frame2[j] == pages[i]) {
                    pageHit = true;
                    referenced[j] = true;
                    break;
                }
            }
            if (!pageHit) {
                while (true) {
                    if (!referenced[frameIndex]) {
                        frame2[frameIndex] = pages[i];
                        referenced[frameIndex] = true;
                        frameIndex = (frameIndex + 1) % capacity;
                        break;
                    } else {
                        referenced[frameIndex] = false;
                        frameIndex = (frameIndex + 1) % capacity;
                    }
                }
            }
            for (int j = 0; j < capacity; j++) {
                if (frame2[j] == -1) {
                    std::cout << "  ";
                } else {
                    std::cout << frame2[j] << (referenced[j] ? '+' : '-') << " ";
                }
            }
            std::cout << "\n";
        }
    } else {
        for (int i = 0; i < pages.size(); i++) {
            bool pageHit = false;
            for (int j = 0; j < capacity; j++) {
                if (frame[j] == pages[i]) {
                    pageHit = true;
                    break;
                }
            }
            if (!pageHit) {
                std::cout << "  ";
            } else {
                for (int j = 0; j < capacity; j++) {
                    if (frame[j] == -1) {
                        std::cout << "  ";
                    } else {
                        std::cout << frame[j] << " ";
                    }
                }
                std::cout << "\n";
            }
        }
    }
}

int main() {
    std::vector<int> pages; //  références de pages
    int n;           // Nombre de références de pages
    int capacity;    // Taille du frame de la mémoire
    int choice;
    std::vector<int> frameContents;
    // Efface le fichier de résultats au début de chaque exécution.
    std::ofstream file("if3602.txt", std::ios::out);
    if (file.is_open()) {
        file.close();
    }

    do {
        std::cout << "\nMenu TP IF3602-2024/2025 (Réalisateur: Nom et Prénom de chaque membre)\n";
        std::cout << "Veuillez choisir l'option à exécuter :\n";
        std::cout << "1. Chargement (fichier texte) ou saisie des données\n";
        std::cout << "2. Simulation avec Algorithme FIFO\n";
        std::cout << "3. Simulation avec Algorithme optimal\n";
        std::cout << "4. Simulation avec Algorithme LRU\n";
        std::cout << "5. Simulation avec la seconde chance\n";
        std::cout << "6. Affichage des Exécutions Effectuées.\n";
        std::cout << "7. Graphiques des algorithmes simulés\n";
        std::cout << "8. Quitter l'Application.\n";
        std::cout << "Entrez votre choix : ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                if (getUserInput(pages, n, capacity) == -1) {
                    // Gérer l'erreur ou retourner au menu principal
                    break;
                }
                break;
            case 2: {
                int pageFaults;
                fifo(pages, capacity, frameContents, pageFaults);
                displayResults("FIFO", pageFaults);
                saveResults("FIFO", pageFaults, "if3602.txt");
                displaySimulation(pages, capacity, frameContents, "FIFO");
                break;
            }
            case 3: {
                int pageFaults;
                optimal(pages, capacity, frameContents, pageFaults);
                displayResults("Optimal", pageFaults);
                saveResults("Optimal", pageFaults, "if3602.txt");
                displaySimulation(pages, capacity, frameContents, "Optimal");
                break;
            }
            case 4: {
                int pageFaults;
                lru(pages, capacity, frameContents, pageFaults);
                displayResults("LRU", pageFaults);
                saveResults("LRU", pageFaults, "if3602.txt");
                displaySimulation(pages, capacity, frameContents, "LRU");
                break;
            }
            case 5: {
                int pageFaults;
                secondChance(pages, capacity, frameContents, pageFaults);
                displayResults("Second Chance", pageFaults);
                saveResults("Second Chance", pageFaults, "if3602.txt");
                displaySimulation(pages, capacity, frameContents, "Second Chance");
                break;
            }
            case 6:
                displaySavedResults("if3602.txt");
                break;
            case 7:
                displaySimulation(pages, capacity, frameContents, "FIFO");
                break;
            case 8:
                std::cout << "Exiting application.\n";
                break;
            default:
                std::cout << "Choix invalide. Veuillez réessayer.\n";
        }
    } while (choice != 8);

    return 0;
}