#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_TITLE_LENGTH 100
#define HASH_TABLE_SIZE 100

typedef struct Song {
    char title[MAX_TITLE_LENGTH];
    char artist[MAX_TITLE_LENGTH];
    char mood[MAX_TITLE_LENGTH];
    struct Song *prev;
    struct Song *next;
} Song;

Song *head = NULL;
Song *tail = NULL;
int isPremium = 0; // Initially not a premium user

typedef struct HashTable {
    Song *table[HASH_TABLE_SIZE];
} HashTable;

// Initialize the hash table
void initializeHashTable(HashTable *hashTable) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashTable->table[i] = NULL;
    }
}

// Hash function to compute the index for a given song title
int hashFunction(char *title) {
    int hash = 0;
    for (int i = 0; title[i] != '\0'; i++) {
        hash = (hash + title[i]) % HASH_TABLE_SIZE;
    }
    return hash;
}

// Insert a song into the hash table
void insertIntoHashTable(HashTable *hashTable, Song *song) {
    int index = hashFunction(song->title);
    hashTable->table[index] = song;
}

Song *searchHashTable(HashTable *hashTable, char *title) {
    int index = hashFunction(title);
    return hashTable->table[index];
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addMusic(HashTable *hashTable) {
    Song *newSong = (Song *)malloc(sizeof(Song));
    printf("Enter song title: ");
    scanf("%99[^\n]", newSong->title);
    clearInputBuffer();
    printf("Enter artist name: ");
    scanf("%99[^\n]", newSong->artist);
    clearInputBuffer();
    printf("Enter mood: ");
    scanf("%99[^\n]", newSong->mood);
    clearInputBuffer();
    newSong->prev = NULL;
    newSong->next = NULL;

    if (head == NULL) {
        head = newSong;
        tail = newSong;
        // Make it circular
        head->next = tail;
        tail->prev = head;
    } else {
        tail->next = newSong;
        newSong->prev = tail;
        tail = newSong;
        // Make it circular
        tail->next = head;
        head->prev = tail;
    }

    // Insert the song into the hash table
    insertIntoHashTable(hashTable, newSong);

    printf("Song added to the playlist.\n");
}

void addMusicFromCSV(HashTable *hashTable, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_TITLE_LENGTH * 3]; // Assuming each field can be as long as MAX_TITLE_LENGTH
    while (fgets(line, sizeof(line), file)) {
        char *title = strtok(line, ",");
        char *artist = strtok(NULL, ",");
        char *mood = strtok(NULL, "\n"); // Use '\n' as the delimiter for the last field
        if (title && artist && mood) {
            // Check if fields exceed MAX_TITLE_LENGTH and dynamically allocate memory if needed
            if (strlen(title) >= MAX_TITLE_LENGTH || strlen(artist) >= MAX_TITLE_LENGTH || strlen(mood) >= MAX_TITLE_LENGTH) {
                printf("Fields exceed maximum length. Skipping song.\n");
                continue;
            }

            Song *newSong = (Song *)malloc(sizeof(Song));
            strcpy(newSong->title, title);
            strcpy(newSong->artist, artist);
            strcpy(newSong->mood, mood);
            newSong->prev = NULL;
            newSong->next = NULL;

            if (head == NULL) {
                head = newSong;
                tail = newSong;
                // Make it circular
                head->next = tail;
                tail->prev = head;
            } else {
                tail->next = newSong;
                newSong->prev = tail;
                tail = newSong;
                // Make it circular
                tail->next = head;
                head->prev = tail;
            }

            // Insert the song into the hash table
            insertIntoHashTable(hashTable, newSong);
        } else {
            printf("Invalid line in CSV file: %s\n", line);
        }
    }

    fclose(file);
}

void removeMusic() {
    if (head == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    char titleToRemove[MAX_TITLE_LENGTH];
    printf("Enter the title of the song to remove: ");
    scanf("%99[^\n]", titleToRemove);
    clearInputBuffer();

    Song *current = head;
    while (current != NULL) {
        if (strcmp(current->title, titleToRemove) == 0) {
            if (current == head) {
                head = current->next;
                head->prev = tail;
                tail->next = head;
            } else if (current == tail) {
                tail = current->prev;
                tail->next = head;
                head->prev = tail;
            } else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }
            free(current);
            printf("Song removed from the playlist.\n");
            return;
        }
        current = current->next;
    }

    printf("Song not found in the playlist.\n");
}

void displayPlaylist() {
    if (head == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    printf("Playlist:\n");
    Song *current = head;
    do {
        printf("Title: %s\nArtist: %s\nMood: %s\n\n", current->title, current->artist, current->mood);
        current = current->next;
    } while (current != head);
}

void searchSongs(HashTable *hashTable, char *searchQuery) {
    if (head == NULL) {
        printf("Playlist is empty!\n");
        return;
    }

    // Search for the song title in the hash table
    Song *foundSong = searchHashTable(hashTable, searchQuery);
    if (foundSong) {
        printf("Song found:\n");
        printf("Title: %s\nArtist: %s\nMood: %s\n\n", foundSong->title, foundSong->artist, foundSong->mood);
    } else {
        printf("Song not found in the playlist.\n");
    }
}

void premiumFeatures() {
    isPremium = 1;
    printf("Congratulations! You are now a premium user.\n");
}

void playNextSong() {
    if (head == NULL) {
        printf("Playlist is empty.\n");
        return;
    }

    Song *current = head;
    if (isPremium) {
        int choice;
        printf("Choose playback mode:\n");
        printf("1. Linear\n");
        printf("2. Shuffled\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 2) { // Shuffled playback
            int numSongs = 0;
            Song *temp = head;
            do {
                numSongs++;
                temp = temp->next;
            } while (temp != head);

            // Seed the random number generator
            srand(time(NULL));

            // Generate a random number between 0 and numSongs-1
            int randomIndex = rand() % numSongs;

            // Traverse to the randomly chosen song
            for (int i = 0; i < randomIndex; i++) {
                current = current->next;
            }
        } else { // Linear playback
            current = current->next;
        }
    } else { // Non-premium user
        // Move to the next song in the playlist
        current = current->next;
    }

    printf("Now playing: %s by %s\n", current->title, current->artist);
}

void playPreviousSong() {
    if (head == NULL) {
        printf("Playlist is empty.\n");
        return;
    }

    // Move to the previous song in the playlist
    head = head->prev;

    printf("Now playing: %s by %s\n", head->title, head->artist);
}

int main() {
    HashTable hashTable;
    initializeHashTable(&hashTable);

    int choice;
    do {
        printf("\nPlaylist Menu:\n");
        printf("1. Add Music\n");
        printf("2. Add Music by CSV file\n");
        printf("3. Remove Music\n");
        printf("4. Display Playlist\n");
        printf("5. Search Songs\n");
        printf("6. Premium Features\n");
        printf("7. Play Next Song\n");
        printf("8. Play Previous Song\n");
        printf("9. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1:
                addMusic(&hashTable);
                break;
            case 2:
                addMusicFromCSV(&hashTable, "music.csv");
                break;
            case 3:
                removeMusic();
                break;
            case 4:
                displayPlaylist();
                break;
            case 5: {
                char searchQuery[MAX_TITLE_LENGTH];
                printf("Enter search query: ");
                scanf("%99[^\n]", searchQuery);
                clearInputBuffer();
                searchSongs(&hashTable, searchQuery);
                break;
            }
            case 6:
                premiumFeatures();
                break;
            case 7:
                playNextSong();
                break;
            case 8:
                playPreviousSong();
                break;
            case 9:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 9);

    // Free allocated memory before exiting
    Song *current = head;
    do {
        Song *temp = current;
        current = current->next;
        free(temp);
    } while (current != head);

    return 0;
}