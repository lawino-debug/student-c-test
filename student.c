#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Constants and definitions
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_TITLE 100
#define MAX_DESC 300
#define MAX_LOCATION 100
#define MAX_MEDIA 100
#define MAX_CONDITION 50
#define DATA_FILE "heritage_data.txt"

// Structure definitions
typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} User;

typedef struct {
    int id;
    char name[MAX_TITLE];
    char description[MAX_DESC];
    char location[MAX_LOCATION];
    char condition[MAX_CONDITION];
    char mediaFile[MAX_MEDIA];
} HeritageItem;

// Function declarations
int login();
int validateCredentials(char* username, char* password);
void autoBackup();
int saveHeritageItem(HeritageItem* item);
int loadAllHeritageItems(HeritageItem** items);
void saveAllItems(HeritageItem* items, int count);
void addItem();
void viewAllItems();
void searchItem();
void editItem();
void deleteItem();
void generatePublicReport();
void generateDetailedReport();
int validateHeritageData(HeritageItem* item);
void displayMainMenu();
void displayOfficerMenu();
void publicSearch();
void clearInputBuffer();

// Authentication functions
int login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    printf("\n=== LOGIN ===\n");
    printf("Username: ");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("Password: ");
    fgets(password, MAX_PASSWORD, stdin);
    password[strcspn(password, "\n")] = 0;
    
    if (validateCredentials(username, password)) {
        printf("Login successful! Welcome, %s!\n", username);
        return 1;
    } else {
        printf("Invalid credentials! Please try again.\n");
        return 0;
    }
}

int validateCredentials(char* username, char* password) {
    if (strcmp(username, "officer") == 0 && strcmp(password, "password") == 0) {
        return 1;
    }
    return 0;
}

// Backup functions
void autoBackup() {
    printf("Performing automatic backup...\n");
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char backupFilename[100];
    snprintf(backupFilename, sizeof(backupFilename), 
             "backup_%04d%02d%02d_%02d%02d%02d.txt",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    FILE* source = fopen("heritage_data.txt", "r");
    FILE* backup = fopen(backupFilename, "w");
    
    if (source && backup) {
        char ch;
        while ((ch = fgetc(source)) != EOF) {
            fputc(ch, backup);
        }
        printf("Backup created successfully: %s\n", backupFilename);
    } else {
        printf("Error: Backup failed!\n");
    }
    
    if (source) fclose(source);
    if (backup) fclose(backup);
}

// File handling functions
int saveHeritageItem(HeritageItem* item) {
    FILE* file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        return 0;
    }
    
    fprintf(file, "ID: %d\n", item->id);
    fprintf(file, "Name: %s\n", item->name);
    fprintf(file, "Description: %s\n", item->description);
    fprintf(file, "Location: %s\n", item->location);
    fprintf(file, "Condition: %s\n", item->condition);
    fprintf(file, "Media: %s\n", item->mediaFile);
    fprintf(file, "---\n");
    
    fclose(file);
    return 1;
}

int loadAllHeritageItems(HeritageItem** items) {
    FILE* file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    HeritageItem* tempItems = NULL;
    int count = 0;
    int capacity = 10;
    
    tempItems = (HeritageItem*)malloc(capacity * sizeof(HeritageItem));
    if (tempItems == NULL) {
        fclose(file);
        return 0;
    }
    
    char line[500];
    HeritageItem currentItem;
    int readingItem = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        if (strncmp(line, "ID: ", 4) == 0) {
            sscanf(line + 4, "%d", &currentItem.id);
            readingItem = 1;
        } else if (strncmp(line, "Name: ", 6) == 0) {
            strcpy(currentItem.name, line + 6);
        } else if (strncmp(line, "Description: ", 13) == 0) {
            strcpy(currentItem.description, line + 13);
        } else if (strncmp(line, "Location: ", 10) == 0) {
            strcpy(currentItem.location, line + 10);
        } else if (strncmp(line, "Condition: ", 11) == 0) {
            strcpy(currentItem.condition, line + 11);
        } else if (strncmp(line, "Media: ", 7) == 0) {
            strcpy(currentItem.mediaFile, line + 7);
        } else if (strcmp(line, "---") == 0 && readingItem) {
            if (count >= capacity) {
                capacity *= 2;
                HeritageItem* newTemp = (HeritageItem*)realloc(tempItems, capacity * sizeof(HeritageItem));
                if (newTemp == NULL) {
                    free(tempItems);
                    fclose(file);
                    return 0;
                }
                tempItems = newTemp;
            }
            tempItems[count++] = currentItem;
            readingItem = 0;
            memset(&currentItem, 0, sizeof(HeritageItem));
        }
    }
    
    fclose(file);
    
    *items = tempItems;
    return count;
}

void saveAllItems(HeritageItem* items, int count) {
    FILE* file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        printf("Error: Cannot open file for writing!\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "ID: %d\n", items[i].id);
        fprintf(file, "Name: %s\n", items[i].name);
        fprintf(file, "Description: %s\n", items[i].description);
        fprintf(file, "Location: %s\n", items[i].location);
        fprintf(file, "Condition: %s\n", items[i].condition);
        fprintf(file, "Media: %s\n", items[i].mediaFile);
        fprintf(file, "---\n");
    }
    
    fclose(file);
}

// Heritage management functions
void addItem() {
    HeritageItem newItem;
    
    printf("\n=== ADD HERITAGE ITEM ===\n");
    
    // Get item details
    printf("Enter heritage site name: ");
    fgets(newItem.name, MAX_TITLE, stdin);
    newItem.name[strcspn(newItem.name, "\n")] = 0;
    
    printf("Enter description: ");
    fgets(newItem.description, MAX_DESC, stdin);
    newItem.description[strcspn(newItem.description, "\n")] = 0;
    
    printf("Enter location (County): ");
    fgets(newItem.location, MAX_LOCATION, stdin);
    newItem.location[strcspn(newItem.location, "\n")] = 0;
    
    printf("Enter condition (Good/Fair/Poor): ");
    fgets(newItem.condition, MAX_CONDITION, stdin);
    newItem.condition[strcspn(newItem.condition, "\n")] = 0;
    
    printf("Enter media file name: ");
    fgets(newItem.mediaFile, MAX_MEDIA, stdin);
    newItem.mediaFile[strcspn(newItem.mediaFile, "\n")] = 0;
    
    // Validate data
    if (!validateHeritageData(&newItem)) {
        printf("Error: Invalid data entered. Please check your inputs.\n");
        return;
    }
    
    // Find the next available ID
    HeritageItem* existingItems = NULL;
    int existingCount = loadAllHeritageItems(&existingItems);
    int nextId = 1;
    if (existingCount > 0) {
        nextId = existingItems[existingCount - 1].id + 1;
    }
    free(existingItems);
    
    newItem.id = nextId;
    if (saveHeritageItem(&newItem)) {
        printf("Heritage item added successfully! ID: %d\n", newItem.id);
        autoBackup();
    } else {
        printf("Error: Failed to save heritage item.\n");
    }
}

void viewAllItems() {
    printf("\n=== ALL HERITAGE ITEMS ===\n");
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    
    if (count == 0) {
        printf("No heritage items found.\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        printf("\n--- Item %d ---\n", i + 1);
        printf("ID: %d\n", items[i].id);
        printf("Name: %s\n", items[i].name);
        printf("Location: %s\n", items[i].location);
        printf("Condition: %s\n", items[i].condition);
        printf("Description: %s\n", items[i].description);
        printf("Media: %s\n", items[i].mediaFile);
    }
    
    free(items);
}

void searchItem() {
    char searchTerm[MAX_TITLE];
    int found = 0;
    
    printf("\n=== SEARCH HERITAGE ITEMS ===\n");
    printf("Enter search term (name or location): ");
    fgets(searchTerm, MAX_TITLE, stdin);
    searchTerm[strcspn(searchTerm, "\n")] = 0;
    
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    
    printf("\nSearch Results:\n");
    for (int i = 0; i < count; i++) {
        if (strstr(items[i].name, searchTerm) != NULL || 
            strstr(items[i].location, searchTerm) != NULL) {
            printf("\n--- Found Item ---\n");
            printf("ID: %d\n", items[i].id);
            printf("Name: %s\n", items[i].name);
            printf("Location: %s\n", items[i].location);
            printf("Condition: %s\n", items[i].condition);
            printf("Description: %s\n", items[i].description);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No items found matching '%s'\n", searchTerm);
    }
    
    free(items);
}

void editItem() {
    int id;
    printf("\n=== EDIT HERITAGE ITEM ===\n");
    printf("Enter ID of item to edit: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID entered!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    int foundIndex = -1;
    
    // Find the item
    for (int i = 0; i < count; i++) {
        if (items[i].id == id) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printf("Error: Item with ID %d not found!\n", id);
        free(items);
        return;
    }
    
    printf("\nEditing Item ID: %d\n", id);
    printf("Current Name: %s\n", items[foundIndex].name);
    printf("Enter new name (or press Enter to keep current): ");
    char newName[MAX_TITLE];
    fgets(newName, MAX_TITLE, stdin);
    newName[strcspn(newName, "\n")] = 0;
    if (strlen(newName) > 0) {
        strcpy(items[foundIndex].name, newName);
    }
    
    printf("Current Description: %s\n", items[foundIndex].description);
    printf("Enter new description (or press Enter to keep current): ");
    char newDesc[MAX_DESC];
    fgets(newDesc, MAX_DESC, stdin);
    newDesc[strcspn(newDesc, "\n")] = 0;
    if (strlen(newDesc) > 0) {
        strcpy(items[foundIndex].description, newDesc);
    }
    
    printf("Current Location: %s\n", items[foundIndex].location);
    printf("Enter new location (or press Enter to keep current): ");
    char newLoc[MAX_LOCATION];
    fgets(newLoc, MAX_LOCATION, stdin);
    newLoc[strcspn(newLoc, "\n")] = 0;
    if (strlen(newLoc) > 0) {
        strcpy(items[foundIndex].location, newLoc);
    }
    
    printf("Current Condition: %s\n", items[foundIndex].condition);
    printf("Enter new condition (Good/Fair/Poor) or press Enter to keep current: ");
    char newCond[MAX_CONDITION];
    fgets(newCond, MAX_CONDITION, stdin);
    newCond[strcspn(newCond, "\n")] = 0;
    if (strlen(newCond) > 0) {
        HeritageItem tempItem = items[foundIndex];
        strcpy(tempItem.condition, newCond);
        if (validateHeritageData(&tempItem)) {
            strcpy(items[foundIndex].condition, newCond);
        } else {
            printf("Condition not updated due to validation error.\n");
        }
    }
    
    printf("Current Media File: %s\n", items[foundIndex].mediaFile);
    printf("Enter new media file name (or press Enter to keep current): ");
    char newMedia[MAX_MEDIA];
    fgets(newMedia, MAX_MEDIA, stdin);
    newMedia[strcspn(newMedia, "\n")] = 0;
    if (strlen(newMedia) > 0) {
        strcpy(items[foundIndex].mediaFile, newMedia);
    }
    
    // Save all items back to file
    saveAllItems(items, count);
    printf("Item updated successfully!\n");
    autoBackup();
    
    free(items);
}

void deleteItem() {
    int id;
    printf("\n=== DELETE HERITAGE ITEM ===\n");
    printf("Enter ID of item to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID entered!\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    int foundIndex = -1;
    
    // Find the item
    for (int i = 0; i < count; i++) {
        if (items[i].id == id) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printf("Error: Item with ID %d not found!\n", id);
        free(items);
        return;
    }
    
    printf("\nAre you sure you want to delete this item?\n");
    printf("ID: %d\n", items[foundIndex].id);
    printf("Name: %s\n", items[foundIndex].name);
    printf("Location: %s\n", items[foundIndex].location);
    printf("\nEnter 'YES' to confirm deletion: ");
    
    char confirmation[10];
    fgets(confirmation, 10, stdin);
    confirmation[strcspn(confirmation, "\n")] = 0;
    
    if (strcmp(confirmation, "YES") == 0) {
        // Create a new array without the deleted item
        HeritageItem* newItems = NULL;
        if (count > 1) {
            newItems = (HeritageItem*)malloc((count - 1) * sizeof(HeritageItem));
            if (newItems == NULL) {
                printf("Error: Memory allocation failed!\n");
                free(items);
                return;
            }
            
            // Copy all items except the one to delete
            int newIndex = 0;
            for (int i = 0; i < count; i++) {
                if (i != foundIndex) {
                    newItems[newIndex++] = items[i];
                }
            }
        }
        
        // Save the updated list
        if (count == 1) {
            // If only one item, create empty file
            FILE* file = fopen(DATA_FILE, "w");
            if (file) fclose(file);
            printf("Item deleted successfully! Database is now empty.\n");
        } else {
            saveAllItems(newItems, count - 1);
            printf("Item deleted successfully!\n");
            free(newItems);
        }
        
        autoBackup();
    } else {
        printf("Deletion cancelled.\n");
    }
    
    free(items);
}


void generatePublicReport() {
    printf("\n=== PUBLIC REPORT ===\n");
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    
    if (count == 0) {
        printf("No heritage sites available for public viewing.\n");
        return;
    }
    
    printf("HeritageVault - Public Heritage Sites Report\n");
    printf("===========================================\n");
    printf("Total Heritage Sites Registered: %d\n\n", count);
    
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, items[i].name);
        printf("   Location: %s County\n", items[i].location);
        printf("   Condition: %s\n", items[i].condition);
        printf("   Description: %.100s...\n", items[i].description);
        printf("   --------------------\n");
    }
    
    free(items);
}

void generateDetailedReport() {
    printf("\n=== DETAILED OFFICER REPORT ===\n");
    HeritageItem* items = NULL;
    int count = loadAllHeritageItems(&items);
    
    if (count == 0) {
        printf("No heritage items found in the system.\n");
        return;
    }
    
    printf("HeritageVault - Detailed Officer Report\n");
    printf("======================================\n");
    printf("Report Generated: ");
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%04d-%02d-%02d %02d:%02d:%02d\n", 
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    printf("Total Heritage Sites: %d\n\n", count);
    
    // Count by condition
    int goodCount = 0, fairCount = 0, poorCount = 0;
    for (int i = 0; i < count; i++) {
        char conditionLower[MAX_CONDITION];
        strcpy(conditionLower, items[i].condition);
        for (int j = 0; conditionLower[j]; j++) {
            conditionLower[j] = tolower(conditionLower[j]);
        }
        
        if (strcmp(conditionLower, "good") == 0) goodCount++;
        else if (strcmp(conditionLower, "fair") == 0) fairCount++;
        else if (strcmp(conditionLower, "poor") == 0) poorCount++;
    }
    
    printf("Condition Summary:\n");
    printf("  Good: %d sites\n", goodCount);
    printf("  Fair: %d sites\n", fairCount);
    printf("  Poor: %d sites\n", poorCount);
    printf("\n");
    
    for (int i = 0; i < count; i++) {
        printf("SITE %d:\n", items[i].id);
        printf("  Name: %s\n", items[i].name);
        printf("  Location: %s County\n", items[i].location);
        printf("  Condition: %s\n", items[i].condition);
        printf("  Description: %s\n", items[i].description);
        printf("  Media File: %s\n", items[i].mediaFile);
        printf("  -----------------------------------------\n");
    }
    
    free(items);
}

int validateHeritageData(HeritageItem* item) {
    if (strlen(item->name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return 0;
    }
    if (strlen(item->location) == 0) {
        printf("Error: Location cannot be empty.\n");
        return 0;
    }
    if (strlen(item->condition) == 0) {
        printf("Error: Condition cannot be empty.\n");
        return 0;
    }
    
    char conditionLower[MAX_CONDITION];
    strcpy(conditionLower, item->condition);
    for (int i = 0; conditionLower[i]; i++) {
        conditionLower[i] = tolower(conditionLower[i]);
    }
    
    if (strcmp(conditionLower, "good") != 0 && 
        strcmp(conditionLower, "fair") != 0 && 
        strcmp(conditionLower, "poor") != 0) {
        printf("Error: Condition must be Good, Fair, or Poor.\n");
        return 0;
    }
    
    return 1;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Menu functions
void displayMainMenu() {
    printf("\n=== MAIN MENU ===\n");
    printf("1. Login (County Officer)\n");
    printf("2. Public Search\n");
    printf("3. Generate Public Reports\n");
    printf("4. Exit\n");
}

void displayOfficerMenu() {
    printf("\n=== OFFICER MENU ===\n");
    printf("1. Add Heritage Item\n");
    printf("2. Edit Heritage Item\n");
    printf("3. Delete Heritage Item\n");
    printf("4. View All Items\n");
    printf("5. Search Items\n");
    printf("6. Generate Detailed Report\n");
    printf("7. Logout\n");
}

void publicSearch() {
    printf("\n=== PUBLIC SEARCH ===\n");
    searchItem();
}

// Main function
int main() {
    int choice;
    int loggedIn = 0;
    
    printf("=== HeritageVault - Digital Preservation System ===\n");
    
    while(1) {
        if (!loggedIn) {
            displayMainMenu();
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar();
            
            switch(choice) {
                case 1:
                    loggedIn = login();
                    break;
                case 2:
                    publicSearch();
                    break;
                case 3:
                    generatePublicReport();
                    break;
                case 4:
                    printf("Thank you for using HeritageVault!\n");
                    exit(0);
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        } else {
            displayOfficerMenu();
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar();
            
            switch(choice) {
                case 1:
                    addItem();
                    break;
                case 2:
                    editItem();
                    break;
                case 3:
                    deleteItem();
                    break;
                case 4:
                    viewAllItems();
                    break;
                case 5:
                    searchItem();
                    break;
                case 6:
                    generateDetailedReport();
                    break;
                case 7:
                    loggedIn = 0;
                    printf("Logged out successfully!\n");
                    break;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        }
    }
    
    return 0;
}
