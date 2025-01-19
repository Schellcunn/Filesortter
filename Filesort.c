#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h> // For _mkdir
#include <shlwapi.h> // For PathCombine

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void list_and_sort_files(const char *directory) {
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\*", directory);
    
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory\n");
        return;
    }

    char **files = NULL;
    size_t count = 0;

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            files = realloc(files, sizeof(char *) * (count + 1));
            files[count] = strdup(findData.cFileName);
            count++;
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

    qsort(files, count, sizeof(char *), compare);

    for (size_t i = 0; i < count; i++) {
        printf("%s\n", files[i]);

        // Extract the part before " - "
        char *end = strstr(files[i], " - ");
        if (end) {
            size_t len = end - files[i];
            char folderName[MAX_PATH];
            strncpy(folderName, files[i], len);
            folderName[len] = '\0';

            // Create the folder
            char folderPath[MAX_PATH];
            sprintf(folderPath, "%s\\%s", directory, folderName);
            _mkdir(folderPath);

            // Move the file to the folder
            char sourcePath[MAX_PATH];
            char destinationPath[MAX_PATH];
            PathCombine(sourcePath, directory, files[i]);
            PathCombine(destinationPath, folderPath, files[i]);
            MoveFile(sourcePath, destinationPath);
        }

        free(files[i]);
    }

    free(files);
}
int main() {
    char directory[MAX_PATH];

    printf("Please enter the directory path: ");
    fgets(directory, MAX_PATH, stdin);

    // Remove the trailing newline if present
    size_t len = strlen(directory);
    if (len > 0 && directory[len - 1] == '\n') {
        directory[len - 1] = '\0';
    }

    list_and_sort_files(directory);
    return 0;
}
// hardcoded directory for testing purposes
//int main() {
//    const char *directory = "D:\\Testfolder";  
//    list_and_sort_files(directory);
//    return 0;
//}