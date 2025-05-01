#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// Fonction pour simuler l'algorithme de remplacement de page FIFO
int fifo(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
    }
    int frameIndex = 0;

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            (*pageFaults)++;
            frame[frameIndex] = pages[i];
            frameIndex = (frameIndex + 1) % capacity; // Buffer circulaire
        }
        //pour le débogage
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page Optimal
int optimal(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
    }

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            (*pageFaults)++;
            int farthest = -1, replaceIndex = -1;
            for (int j = 0; j < capacity; j++) {
                int k;
                for (k = i + 1; k < n; k++) {
                    if (frame[j] == pages[k]) {
                        if (k > farthest) {
                            farthest = k;
                            replaceIndex = j;
                        }
                        break;
                    }
                }
                if (k == n) { // Non trouvé dans le futur
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
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page LRU
int lru(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    int time[capacity]; // Garde la trace du moment où chaque page a été utilisée pour la dernière fois
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
        time[i] = 0;
    }
    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                time[j] = currentTime++; // Met à jour le temps de la dernière utilisation
                break;
            }
        }
        if (!pageHit) {
            (*pageFaults)++;
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
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Fonction pour simuler l'algorithme de remplacement de page Second Chance
int secondChance(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    bool referenced[capacity]; // Garde la trace du bit de référence
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
        referenced[i] = false;
    }
    int frameIndex = 0;

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                referenced[j] = true; // Met le bit de référence
                break;
            }
        }
        if (!pageHit) {
            (*pageFaults)++;
            while (true) {
                if (!referenced[frameIndex]) {
                    frame[frameIndex] = pages[i];
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
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Fonction pour obtenir l'entrée de l'utilisateur pour les références de page et la taille du frame
int getUserInput(int** pages, int* n, int* capacity) {
    int choice;
    printf("1. Pour saisir manuellement les données\n");
    printf("2. Pour charger les données à partir d'un fichier texte\n");
    printf("3. Pour revenir au menu principal\n");
    printf("Entrez votre choix : ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            printf("Entrez le nombre de références de pages : ");
            scanf("%d", n);
            *pages = (int*)malloc(*n * sizeof(int));
            if (*pages == NULL) {
                perror("Erreur d'allocation de mémoire");
                return -1;
            }
            printf("Entrez les références de pages (séparées par des espaces) : ");
            for (int i = 0; i < *n; i++) {
                scanf("%d", &(*pages)[i]);
            }
            printf("Entrez le nombre de pages de la mémoire tampon : ");
            scanf("%d", capacity);
            break;
        }
        case 2: {
            char filename[100];
            printf("Entrez le nom du fichier texte : ");
            scanf("%s", filename);
            FILE* file = fopen(filename, "r");
            if (file == NULL) {
                perror("Erreur lors de l'ouverture du fichier");
                return -1;
            }
            // D'abord, lire le nombre de pages
            if (fscanf(file, "%d", n) != 1) {
                fprintf(stderr, "Erreur lors de la lecture du nombre de pages à partir du fichier.\n");
                fclose(file);
                return -1;
            }

            *pages = (int*)malloc(*n * sizeof(int));
            if (*pages == NULL) {
                perror("Erreur d'allocation de mémoire");
                fclose(file);
                return -1;
            }

            // Ensuite, lire les références de page
            for (int i = 0; i < *n; i++) {
                if (fscanf(file, "%d", &(*pages)[i]) != 1) {
                    fprintf(stderr, "Erreur lors de la lecture de la référence de page %d à partir du fichier.\n", i + 1);
                    fclose(file);
                    free(*pages);
                    *pages = NULL;
                    return -1;
                }
            }
            // Enfin, lire la capacité
            if (fscanf(file, "%d", capacity) != 1) {
                fprintf(stderr, "Erreur lors de la lecture de la capacité à partir du fichier.\n");
                fclose(file);
                free(*pages);
                *pages = NULL;
                return -1;
            }
            fclose(file);
            break;
        }
        case 3:
            return 1; // Retour au menu principal
        default:
            printf("Choix invalide.\n");
            return -1;
    }
    return 0;
}

// Fonction pour afficher les résultats de la simulation
void displayResults(char* algorithmName, int pageFaults) {
    printf("Algorithme %s: %d fautes de page\n", algorithmName, pageFaults);
}

// Fonction pour sauvegarder les résultats dans un fichier
void saveResults(char* algorithmName, int pageFaults, const char* filename) {
    FILE* file = fopen(filename, "a"); // Ajouter au fichier
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    fprintf(file, "Algorithme %s | %d\n", algorithmName, pageFaults);
    fclose(file);
}

// Fonction pour afficher les résultats sauvegardés à partir du fichier
void displaySavedResults(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    char line[256]; // Supposons une longueur de ligne raisonnable
    printf("\n--- Résultats des Exécutions Précédentes ---\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line); // Affiche chaque ligne du fichier
    }
    fclose(file);
}

//Fonction pour afficher la simulation graphiquement.
void displaySimulation(int pages[], int n, int capacity, int* frameContents, char* algorithmName){
    int pageFaults = 0;
    int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialise le frame comme étant vide
    }
    if (strcmp(algorithmName, "FIFO") == 0) {
        fifo(pages, n, capacity, frameContents, &pageFaults);
    } else if (strcmp(algorithmName, "Optimal") == 0) {
        optimal(pages, n, capacity, frameContents, &pageFaults);
    } else if (strcmp(algorithmName, "LRU") == 0) {
        lru(pages, n, capacity, frameContents, &pageFaults);
    } else if (strcmp(algorithmName, "Second Chance") == 0) {
        secondChance(pages, n, capacity, frameContents, &pageFaults);
    }

    printf("\nRéférences de pages logiques: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", pages[i]);
    }
    printf("\n\n");

    printf("Pages physiques -->\n");
    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                break;
            }
        }
        if (!pageHit) {
            printf("F ");
        } else {
            printf("  ");
        }
    }
    printf("\n");
    printf("Pages logiques -->\n");
    for (int i = 0; i < n; i++) {
        printf("  ");
    }
    printf("\n");

    if (strcmp(algorithmName, "Second Chance") == 0) {
        int frame2[capacity];
        bool referenced[capacity];
        for (int i = 0; i < capacity; i++) {
            frame2[i] = -1;
            referenced[i] = false;
        }
        int frameIndex = 0;
        for (int i = 0; i < n; i++) {
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
                    printf("  ");
                } else {
                    printf("%d%c ", frame2[j], referenced[j] ? '+' : '-');
                }
            }
            printf("\n");
        }
    } else {
        for (int i = 0; i < n; i++) {
            bool pageHit = false;
            for (int j = 0; j < capacity; j++) {
                if (frame[j] == pages[i]) {
                    pageHit = true;
                    break;
                }
            }
            if (!pageHit) {
                printf("  ");
            } else {
                for (int j = 0; j < capacity; j++) {
                    if (frame[j] == -1) {
                        printf("  ");
                    } else {
                        printf("%d ", frame[j]);
                    }
                }
                printf("\n");
            }
        }
    }
}

int main() {
    int pages[100]; // Maximum 100 références de pages
    int n;           // Nombre de références de pages
    int capacity;    // Taille du frame de la mémoire
    int choice;
    int frameContents[capacity];
    // Efface le fichier de résultats au début de chaque exécution.
    FILE *file = fopen("if3602.txt", "w");
    if (file != NULL) {
        fclose(file);
    }

    do {
        printf("\nMenu TP IF3602-2024/2025 (Réalisateur: Nom et Prénom de chaque membre)\n");
        printf("Veuillez choisir l'option à exécuter :\n");
        printf("1. Chargement (fichier texte) ou saisie des données\n");
        printf("2. Simulation avec Algorithme FIFO\n");
        printf("3. Simulation avec Algorithme optimal\n");
        printf("4. Simulation avec Algorithme LRU\n");
        printf("5. Simulation avec la seconde chance\n");
        printf("6. Affichage des Exécutions Effectuées.\n");
        printf("7. Graphiques des algorithmes simulés\n");
        printf("8. Quitter l'Application.\n");
        printf("Entrez votre choix : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (getUserInput(&pages, &n, &capacity) == -1) {
                    // Gérer l'erreur ou retourner au menu principal
                    break;
                }
                break;
            case 2: {
                int pageFaults;
                fifo(pages, n, capacity, frameContents, &pageFaults);
                displayResults("FIFO", pageFaults);
                saveResults("FIFO", pageFaults, "if3602.txt");
                displaySimulation(pages, n, capacity, frameContents, "FIFO");
                break;
            }
            case 3: {
                int pageFaults;
                optimal(pages, n, capacity, frameContents, &pageFaults);
                displayResults("Optimal", pageFaults);
                saveResults("Optimal", pageFaults, "if3602.txt");
                displaySimulation(pages, n, capacity, frameContents, "Optimal");
                break;
            }
            case 4: {
                int pageFaults;
                lru(pages, n, capacity, frameContents, &pageFaults);
                displayResults("LRU", pageFaults);
                saveResults("LRU", pageFaults, "if3602.txt");
                displaySimulation(pages, n, capacity, frameContents, "LRU");
                break;
            }
            case 5: {
                int pageFaults;
                secondChance(pages, n, capacity, frameContents, &pageFaults);
                displayResults("Second Chance", pageFaults);
                saveResults("Second Chance", pageFaults, "if3602.txt");
                displaySimulation(pages, n, capacity, frameContents, "Second Chance");
                break;
            }
            case 6:
                displaySavedResults("if3602.txt");
                break;
            case 7:
                displaySimulation(pages, n, capacity, frameContents, "FIFO");
                break;
            case 8:
                printf("Exiting application.\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choice != 8);

    // Libère la mémoire allouée
    if (pages != NULL) {
        free(pages);
    }

    return 0;
}

