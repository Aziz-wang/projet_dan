#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// Function to simulate FIFO page replacement algorithm
int fifo(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialize frame as empty
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
            frameIndex = (frameIndex + 1) % capacity; // Circular buffer
        }
        //for debugging
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Function to simulate Optimal page replacement algorithm
int optimal(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialize frame as empty
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
                if (k == n) { // Not found in future
                    replaceIndex = j;
                    break;
                }
            }
            if(replaceIndex == -1){
                 replaceIndex = 0;
            }
            frame[replaceIndex] = pages[i];
        }
        //for debugging
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Function to simulate LRU page replacement algorithm
int lru(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    int time[capacity]; // Keep track of when each page was last used
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialize frame as empty
        time[i] = 0;
    }
    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                time[j] = currentTime++; // Update last used time
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
        //for debugging
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Function to simulate Second Chance page replacement algorithm
int secondChance(int pages[], int n, int capacity, int* frameContents, int* pageFaults) {
    *pageFaults = 0;
    int frame[capacity];
    bool referenced[capacity]; // Keep track of reference bit
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialize frame as empty
        referenced[i] = false;
    }
    int frameIndex = 0;

    for (int i = 0; i < n; i++) {
        bool pageHit = false;
        for (int j = 0; j < capacity; j++) {
            if (frame[j] == pages[i]) {
                pageHit = true;
                referenced[j] = true; // Set reference bit
                break;
            }
        }
        if (!pageHit) {
            (*pageFaults)++;
            while (true) {
                if (!referenced[frameIndex]) {
                    frame[frameIndex] = pages[i];
                    referenced[frameIndex] = true; // Set reference bit for new page
                    frameIndex = (frameIndex + 1) % capacity;
                    break;
                } else {
                    referenced[frameIndex] = false; // Clear reference bit
                    frameIndex = (frameIndex + 1) % capacity;
                }
            }
        }
        //for debugging
        /*printf("Frame: ");
        for(int j=0; j<capacity; j++){
           printf("%d ", frame[j]);
        }
        printf("\n");*/
    }
    return 0;
}

// Function to get user input for page references and frame size
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
                perror("Memory allocation error");
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
                perror("Error opening file");
                return -1;
            }
            // First, read the number of pages
            if (fscanf(file, "%d", n) != 1) {
                fprintf(stderr, "Error reading number of pages from file.\n");
                fclose(file);
                return -1;
            }

            *pages = (int*)malloc(*n * sizeof(int));
            if (*pages == NULL) {
                perror("Memory allocation error");
                fclose(file);
                return -1;
            }

            // Then, read the page references
            for (int i = 0; i < *n; i++) {
                if (fscanf(file, "%d", &(*pages)[i]) != 1) {
                    fprintf(stderr, "Error reading page reference %d from file.\n", i + 1);
                    fclose(file);
                    free(*pages);
                    *pages = NULL;
                    return -1;
                }
            }
            //Finally read the capacity
            if (fscanf(file, "%d", capacity) != 1) {
                fprintf(stderr, "Error reading capacity from file.\n");
                fclose(file);
                free(*pages);
                *pages = NULL;
                return -1;
            }
            fclose(file);
            break;
        }
        case 3:
            return 1; // Return to main menu
        default:
            printf("Choix invalide.\n");
            return -1;
    }
    return 0;
}

// Function to display the results of the simulation
void displayResults(char* algorithmName, int pageFaults) {
    printf("Algorithme %s: %d fautes de page\n", algorithmName, pageFaults);
}

// Function to save the results to a file
void saveResults(char* algorithmName, int pageFaults, const char* filename) {
    FILE* file = fopen(filename, "a"); // Append to the file
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "Algorithme %s | %d\n", algorithmName, pageFaults);
    fclose(file);
}

// Function to display the saved results from the file
void displaySavedResults(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    char line[256]; // Assuming a reasonable line length
    printf("\n--- Résultats des Exécutions Précédentes ---\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line); // Print each line from the file
    }
    fclose(file);
}

//Function to display the simulation graphically.
void displaySimulation(int pages[], int n, int capacity, int* frameContents, char* algorithmName){
     int pageFaults = 0;
     int frame[capacity];
    for (int i = 0; i < capacity; i++) {
        frame[i] = -1; // Initialize frame as empty
    }
     if(strcmp(algorithmName, "FIFO") == 0){
        fifo(pages, n, capacity, frameContents, &pageFaults);
     }
     else if(strcmp(algorithmName, "Optimal") == 0){
        optimal(pages, n, capacity, frameContents, &pageFaults);
     }
      else if(strcmp(algorithmName, "LRU") == 0){
        lru(pages, n, capacity, frameContents, &pageFaults);
     }
     else if(strcmp(algorithmName, "Second Chance") == 0){
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
             if(strcmp(algorithmName, "Second Chance") == 0){
                printf("F ");
             }
             else{
                printf("F ");
             }

        } else {
            printf("  ");
        }

    }
    printf("\n");
     printf("Pages logiques -->\n");
     for(int i = 0; i<n; i++){
        printf("  ");
     }
    printf("\n");

    if(strcmp(algorithmName, "Second Chance") == 0){
         int frame2[capacity];
        bool referenced[capacity];
        for(int i = 0; i<capacity; i++){
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
            for(int j = 0; j<capacity; j++){
                if(frame2[j] == -1){
                    printf("  ");
                }
                else{
                 printf("%d%c ", frame2[j], referenced[j] ? '+' : '-');
                }
            }

            printf("\n");
        }
    }
    else{
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
                 for(int j = 0; j<capacity; j++){
                    if(frame[j] == -1){
                         printf("  ");
                    }
                    else{
                        printf("%d ", frame[j]);
                    }

                 }
                printf("\n");
            }
        }
    }

}

int main() {
    int pages[100]; // Maximum 100 page references
    int n;           // Number of page references
    int capacity;    // Size of the memory frame
    int choice;
    int frameContents[capacity];
    // Clear the results file at the start of each run.
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
                    // Handle error or return to main menu
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
                 printf("Functionality not implemented.\n");
                 break;
            case 8:
                printf("Exiting application.\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choice != 8);

    // Free allocated memory
    if (pages != NULL) {
        free(pages);
    }

    return 0;
}

