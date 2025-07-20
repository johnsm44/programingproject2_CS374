#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE, malloc, free, atoi, atof
#include <string.h> // for strncpy, strlen, strchr, memmove, strcmp
#include <ctype.h>  // For tolower, isdigit

// Define the struct for a movie
typedef struct movie {
    char title[256];
    int year;
    char languages[256]; // Semicolon-separated languages
    float rating;
    struct movie *next; // Pointer to the next movie in the list
} movie;

// Function to create a new movie node
movie* createMovieNode(const char* title, int year, const char* languages, float rating) {
    movie* newNode = (movie*)malloc(sizeof(movie));
    if (newNode == NULL) {
        perror("Failed to allocate memory for new movie node");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->title, title, sizeof(newNode->title) - 1);
    newNode->title[sizeof(newNode->title) - 1] = '\0'; // Ensure null-termination
    newNode->year = year;

    // Remove square brackets from languages string if present
    char cleaned_languages[256];
    strncpy(cleaned_languages, languages, sizeof(cleaned_languages) - 1);
    cleaned_languages[sizeof(cleaned_languages) - 1] = '\0';

    if (cleaned_languages[0] == '[') {
        size_t len = strlen(cleaned_languages);
        if (len > 0 && cleaned_languages[len - 1] == ']') {
            cleaned_languages[len - 1] = '\0'; // Remove ']'
            // Use memmove to shift the string left, effectively removing the '['
            memmove(cleaned_languages, cleaned_languages + 1, strlen(cleaned_languages + 1) + 1);
        }
    }
    strncpy(newNode->languages, cleaned_languages, sizeof(newNode->languages) - 1);
    newNode->languages[sizeof(newNode->languages) - 1] = '\0'; // Ensure null-termination

    newNode->rating = rating;
    newNode->next = NULL;
    return newNode;
}

// Function to add a movie node to the end of the linked list
void addMovieToList(movie** head, movie* newNode) {
    if (*head == NULL) {
        *head = newNode;
    } else {
        movie* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to print menu
void printMenu() {
    printf("\n1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n");
    printf("\nEnter a choice from 1 to 4: ");
}

// 1. Show movies released in the specified year
void showMoviesByYear(movie* head) {
    int searchYear;
    printf("Enter the year for which you want to see movies: ");
    if (scanf("%d", &searchYear) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }

    movie* current = head;
    int found = 0;
    while (current != NULL) {
        if (current->year == searchYear) {
            printf("%s\n", current->title);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("No data about movies released in the year %d\n", searchYear);
    }
}

// 2. Show highest rated movie for each year
void showHighestRatedMoviePerYear(movie* head) {
    // A temporary linked list to store the highest rated movie for each year
    typedef struct YearRating {
        int year;
        float highestRating;
        char title[256];
        struct YearRating *next;
    } YearRating;

    YearRating* yearRatingsHead = NULL;

    movie* currentMovie = head;
    while (currentMovie != NULL) {
        YearRating* currentYearRating = yearRatingsHead;
        int foundYear = 0;
        while (currentYearRating != NULL) {
            if (currentYearRating->year == currentMovie->year) {
                foundYear = 1;
                if (currentMovie->rating > currentYearRating->highestRating) {
                    currentYearRating->highestRating = currentMovie->rating;
                    strncpy(currentYearRating->title, currentMovie->title, sizeof(currentYearRating->title) - 1);
                    currentYearRating->title[sizeof(currentYearRating->title) - 1] = '\0';
                }
                break;
            }
            currentYearRating = currentYearRating->next;
        }

        if (!foundYear) {
            // Add new year to the list
            YearRating* newYearRating = (YearRating*)malloc(sizeof(YearRating));
            if (newYearRating == NULL) {
                perror("Failed to allocate memory for year rating node");
                exit(EXIT_FAILURE);
            }
            newYearRating->year = currentMovie->year;
            newYearRating->highestRating = currentMovie->rating;
            strncpy(newYearRating->title, currentMovie->title, sizeof(newYearRating->title) - 1);
            newYearRating->title[sizeof(newYearRating->title) - 1] = '\0';
            newYearRating->next = NULL;

            if (yearRatingsHead == NULL) {
                yearRatingsHead = newYearRating;
            } else {
                YearRating* temp = yearRatingsHead;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newYearRating;
            }
        }
        currentMovie = currentMovie->next;
    }

    // Print the results
    YearRating* tempYearRating = yearRatingsHead;
    while (tempYearRating != NULL) {
        printf("%d %.1f %s\n", tempYearRating->year, tempYearRating->highestRating, tempYearRating->title);
        tempYearRating = tempYearRating->next;
    }

    // Free the temporary YearRating linked list
    YearRating* currentYR = yearRatingsHead;
    YearRating* nextYR;
    while (currentYR != NULL) {
        nextYR = currentYR->next;
        free(currentYR);
        currentYR = nextYR;
    }
}


// 3. Show the title and year of release of all movies in a specific language
void showMoviesByLanguage(movie* head) {
    char searchTerm[256];
    printf("Enter the language for which you want to see movies: ");
    getchar(); // Consume the newline character left by previous input
    fgets(searchTerm, sizeof(searchTerm), stdin);
    searchTerm[strcspn(searchTerm, "\n")] = '\0'; // Remove trailing newline

    // Convert search term to lowercase for case-insensitive comparison
    for (int i = 0; searchTerm[i]; i++) {
        searchTerm[i] = tolower(searchTerm[i]);
    }

    movie* current = head;
    int found = 0;
    while (current != NULL) {
        // Create a modifiable copy of languages string for tokenization
        char languagesCopy[256];
        strncpy(languagesCopy, current->languages, sizeof(languagesCopy) - 1);
        languagesCopy[sizeof(languagesCopy) - 1] = '\0';

        char* token = strtok(languagesCopy, ";");
        while (token != NULL) {
            // Trim leading/trailing spaces from token
            while (*token == ' ') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';

            char tokenLower[256];
            strncpy(tokenLower, token, sizeof(tokenLower) - 1);
            tokenLower[sizeof(tokenLower) - 1] = '\0';
            for (int i = 0; tokenLower[i]; i++) {
                tokenLower[i] = tolower(tokenLower[i]);
            }

            if (strcmp(tokenLower, searchTerm) == 0) {
                printf("%d %s\n", current->year, current->title);
                found = 1;
                break; // Found the language, no need to check other languages for this movie
            }
            token = strtok(NULL, ";");
        }
        current = current->next;
    }
    if (!found) {
        printf("No data about movies released in %s\n", searchTerm);
    }
}

// Function to free the linked list memory
void freeMovieList(movie* head) {
    movie* current = head;
    movie* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // Changed from `argc != 2` to `argc < 2` for flexibility, though `==2` is fine for this problem.
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies.csv\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    movie* head = NULL;
    // Use char* for getline as it manages memory
    char *currLine = NULL; 
    size_t len = 0; // len will be updated by getline
    int movieCount = 0;
    
    // Read and discard the header line
    // Use getline here too, and free the memory for the header line
    if (getline(&currLine, &len, file) == -1) {
        fprintf(stderr, "Error reading header or empty file.\n");
        fclose(file);
        free(currLine); // Free memory allocated for currLine by the header read
        return EXIT_FAILURE;
    }
    // Free memory after reading the header line, as it's no longer needed
    free(currLine); 
    currLine = NULL; // Reset currLine to NULL and len to 0 for the next getline calls
    len = 0;

    // Now, read the actual movie data lines using getline
    while (getline(&currLine, &len, file) != -1) {
        // Remove trailing newline character if present
        // Note: getline usually includes the newline, so remove it.
        currLine[strcspn(currLine, "\n")] = '\0';

        char title[256];
        int year;
        char languages[256];
        float rating;
        char *ptr = currLine; // Pointer to traverse the line

        // 1. Find the year (4 digits)
        char *year_start = NULL;
        // Iterate through the current line to find the start of the year
        // Make sure to not go past the end of the string
        for (int i = 0; i < strlen(ptr); ++i) {
            if (isdigit(ptr[i]) && i + 3 < strlen(ptr) && // Check for 4 digits
                isdigit(ptr[i+1]) && isdigit(ptr[i+2]) && isdigit(ptr[i+3])) {
                year_start = &ptr[i];
                break;
            }
        }

        if (year_start == NULL) {
            fprintf(stderr, "Error: Could not find year in line: %s\n", currLine);
            continue; // Skip to the next line if parsing failed
        }

        // Extract title: from start of line to year_start - 1
        int title_len = year_start - ptr;
        if (title_len < 0 || title_len >= sizeof(title)) { // Basic validation
            fprintf(stderr, "Error: Invalid title length in line: %s\n", currLine);
            continue;
        }
        strncpy(title, ptr, title_len);
        title[title_len] = '\0';
        // Trim trailing spaces from title
        for (int i = title_len - 1; i >= 0 && title[i] == ' '; --i) {
            title[i] = '\0';
        }

        // Parse year
        year = atoi(year_start);

        // Move pointer past the year and any spaces
        char *languages_start = year_start;
        while (*languages_start != '\0' && isdigit(*languages_start)) {
            languages_start++;
        }
        while (*languages_start != '\0' && *languages_start == ' ') {
            languages_start++;
        }

        // Find the languages block (starts with '[')
        char *bracket_start = strchr(languages_start, '[');
        if (bracket_start == NULL) {
            fprintf(stderr, "Error: Could not find languages block in line: %s\n", currLine);
            continue; // Skip to the next line if parsing failed
        }
        char *bracket_end = strchr(bracket_start, ']');
        if (bracket_end == NULL) {
            fprintf(stderr, "Error: Malformed languages block (missing ']') in line: %s\n", currLine);
            continue; // Skip to the next line if parsing failed
        }

        // Extract languages (including brackets for now, createMovieNode will clean)
        int lang_len = bracket_end - bracket_start + 1;
        if (lang_len <= 0 || lang_len >= sizeof(languages)) { // Basic validation
            fprintf(stderr, "Error: Invalid languages string length in line: %s\n", currLine);
            continue;
        }
        strncpy(languages, bracket_start, lang_len);
        languages[lang_len] = '\0';


        // Move pointer past the languages block and any spaces
        char *rating_start = bracket_end + 1;
        while (*rating_start != '\0' && *rating_start == ' ') {
            rating_start++;
        }

        // Parse rating
        // Check if rating_start points to a valid number string
        if (*rating_start == '\0' || !isdigit(*rating_start) && *rating_start != '.') {
            fprintf(stderr, "Error: Could not find rating in line: %s\n", currLine);
            continue;
        }
        rating = atof(rating_start);
        
        // Skip the last "Value" column if it exists; atof will stop at non-numeric characters.
        // No explicit strtok needed for the last column if it's always after the rating.

        movie* newMovie = createMovieNode(title, year, languages, rating);
        addMovieToList(&head, newMovie);
        movieCount++;
    }

    // IMPORTANT: Free the memory allocated by the last getline call outside the loop
    free(currLine); 
    currLine = NULL; // Good practice to nullify after freeing

    fclose(file);

    printf("Processed file %s and parsed data for %d movies\n", argv[1], movieCount);

    int choice;
    do {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            printf("You entered an incorrect choice. Try again.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                showMoviesByYear(head);
                break;
            case 2:
                showHighestRatedMoviePerYear(head);
                break;
            case 3:
                showMoviesByLanguage(head);
                break;
            case 4:
                // Exit
                break;
            default:
                printf("You entered an incorrect choice. Try again.\n");
        }
    } while (choice != 4);

    freeMovieList(head); // Free all allocated movie nodes
    return EXIT_SUCCESS;
}
