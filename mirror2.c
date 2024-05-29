#define _XOPEN_SOURCE 500
#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ftw.h> 
#include <stdbool.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <libgen.h>
#include <fnmatch.h>
#include <fcntl.h>
bool first_directory_encountered = false;
// Define _GNU_SOURCE for FTW_ACTIONRETVAL
#define PORT 7865
#define MAX_CONNECTIONS 20
#define BUFFER_SIZE 1024
#define MAX_COMMAND_LENGTH 256
#define MAX_RESPONSE_LENGTH 1024
#define MAX_DIRS 300
#define MAXLINE 1024
#define MAX_DIR_LENGTH 1024
#define EOF_MARKER "EOF\n"
#define FILE_LIST "file_list2.txt" 
#include <time.h>
int i=0;
const char *home_dir;
//char directories[MAX_DIRS][MAX_RESPONSE_LENGTH];  // Array to store directory names
int num_dirs = 0; 
FILE *tar_file;
typedef struct {
    int socket;
    char ip_address[INET_ADDRSTRLEN];
} ClientStruct;

typedef struct {
    char path[MAX_DIR_LENGTH];
    time_t creation_time;
} DirectoryInfo;

DirectoryInfo directories_info[MAX_DIRS];

ClientStruct connections[MAX_CONNECTIONS];
int active_connections = 0;

typedef struct {
    char filename[MAX_DIR_LENGTH];
    off_t size;
    time_t creation_time;
    mode_t permissions;
} FileInfo;
FileInfo file_info;

char directories[MAX_DIRS][MAX_DIR_LENGTH];// Array to store directory paths
int num_directories = 0; // Number of directories encountered
int num;

 char response[BUFFER_SIZE];


int conn=0;



// Global variables to store size range
static off_t size1, size2;

static time_t target_time;

static int is_before_target_date(const char *fpath, const struct stat *sb) {
    return sb->st_mtime < target_time;
}

// Callback function for nftw
static int process_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *filename = basename(fpath); // Extract filename from the path
    if (filename != NULL && filename[0] != '.' && typeflag == FTW_F && sb->st_size >= size1 && sb->st_size <= size2) {
        // Append the file path to the list
        FILE *file_list = fopen("file_list.txt", "a");
        if (file_list == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        fprintf(file_list, "%s\n", fpath);
        fclose(file_list);
    }
    return 0; // Continue traversal
}

void create_tar_archive() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execlp("tar", "tar", "-czf", "temp.tar.gz", "--files-from", "file_list.txt", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error creating tar archive.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void create_tar_archive3() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execlp("tar", "tar", "-czf", "temp.tar.gz", "--files-from", "output.txt", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error creating tar archive.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void create_tar_archive4() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execlp("tar", "tar", "-czf", "temp.tar.gz", "--files-from", "output2.txt", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error creating tar archive.\n");
            exit(EXIT_FAILURE);
        }
    }
}

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_D && fpath[ftwbuf->base] == '.' && ftwbuf->level != 0) // if directory and name starts with '.'
        return FTW_SKIP_SUBTREE;

    if (tflag == FTW_F && is_before_target_date(fpath, sb)) {
        printf("%s\n", fpath);
        FILE *file = fopen("output.txt", "a"); // Open file in append mode
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%s\n", fpath);
        fclose(file);
    }
    return 0; // continue traversal
}
void find_files_before_time(const char *directory, time_t target_time, FILE * output_file) {
    struct stat file_stat;

    // Open the directory
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Open file for writing file paths
    

    // Iterate over files in the directory
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Construct the full path to the file
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        // Get file information
        if (stat(filepath, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        // Check if it's a regular file
        if (!S_ISREG(file_stat.st_mode)) {
            continue; // Skip if not a regular file
        }

        // Check if file creation time is before target time
        if (file_stat.st_ctime < target_time) {
            fprintf(output_file, "%s\n", filepath);
        }
    }

    // Close the output file
    //fclose(output_file);
    closedir(dir);
}
void find_files_after_time(const char *directory, time_t target_time, FILE *output_file) {
    struct stat file_stat;

    // Open the directory
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Open file for writing file paths
    
    // Iterate over files in the directory
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Construct the full path to the file
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        // Get file information
        if (stat(filepath, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        // Check if it's a regular file
        if (!S_ISREG(file_stat.st_mode)) {
            continue; // Skip if not a regular file
        }
        
        // Check if file creation time is after target time
        if (file_stat.st_ctime >= target_time) {
            printf("hiiii");
            fprintf(output_file, "%s\n", filepath);
        }
    }

    // Close the output file
   // fclose(output_file);
    closedir(dir);
}

void create_tar_archive2() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execlp("tar", "tar", "-czf", "archive.tar.gz", "-T", FILE_LIST, NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error creating tar archive.\n");
            exit(EXIT_FAILURE);
        }
    }
}





int find_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    
    


    if (typeflag == FTW_F)  { // File found
        
        char *filename = strrchr(fpath, '/');
        if (filename != NULL && strcmp(filename + 1, file_info.filename) == 0) { // Match filename
            // Populate file information
            strcpy(file_info.filename, filename + 1);
            file_info.size = sb->st_size;
            file_info.creation_time = sb->st_ctime;
            file_info.permissions = sb->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);

            // Send file information to the client
           
            snprintf(response, sizeof(response), "Filename: %s\nSize: %ld bytes\nDate created: %s\nFile permissions: %o\n", 
                     file_info.filename, file_info.size, ctime(&file_info.creation_time), file_info.permissions);
                     return 1;
           
        }
        
    }
    return 0; // Continue traversal
}

// int compare_directories_by_creation_time(const void *a, const void *b) {
//     const DirectoryInfo *dir1 = (const DirectoryInfo *)a;
//     const DirectoryInfo *dir2 = (const DirectoryInfo *)b;
//     return (int)(dir1->creation_time - dir2->creation_time);
// }

// int sendbydate(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
//      const char *file_name = basename((char *)fpath);
//       if (typeflag == FTW_D && file_name[0] == '.' && strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0) {
//     return FTW_SKIP_SUBTREE; // Skip hidden directories (except '.' and '..') and their subdirectories
// }
//     if (typeflag == FTW_D) {
//         // Check if there's still space in the directories array
//         if (num < MAX_DIRS) {
//             // Copy the directory path into the directories array without the delimiter
//             size_t len = strlen(fpath);
//             if (len > 0 && fpath[len - 1] == '/') {
//                 len--; // Exclude the delimiter if it's present at the end
//             }
//             if (len >= MAX_DIR_LENGTH) {
//                 fprintf(stderr, "Directory path too long\n");
//                 return -1;
//             }
//             strncpy(directories_info[num].path, fpath, len);
//             directories_info[num].path[len] = '\0'; // Ensure null-termination
//             directories_info[num].creation_time = sb->st_ctime; // Store creation time

//             // Increment the number of directories encountered
//             num++;
//         } else {
//             fprintf(stderr, "Maximum number of directories reached\n");
//             return -1; // Return an error if the array is full
//         }
//     }
//     return 0; // Continue traversal
// }

// Callback function to populate directory paths
int populate_dir_paths(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
     const char *file_name = basename((char *)fpath);
      if (typeflag == FTW_D && file_name[0] == '.' && strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0) {
    return FTW_SKIP_SUBTREE; // Skip hidden directories (except '.' and '..') and their subdirectories
}
     if (typeflag == FTW_D) {
        // Check if there's still space in the directories array
        if (num_directories < MAX_DIRS) {
            // Copy the directory path into the directories array without the delimiter
            size_t len = strlen(fpath);
            if (len > 0 && fpath[len - 1] == '/') {
                len--; // Exclude the delimiter if it's present at the end
            }
            if (len >= MAX_DIR_LENGTH) {
                fprintf(stderr, "Directory path too long\n");
                return -1;
            }
            strncpy(directories[num_directories], fpath, len);
            directories[num_directories][len] = '\0';
             // Ensure null-termination

            // Increment the number of directories encountered
            num_directories++;
            
            } else {
            fprintf(stderr, "Maximum number of directories reached\n");
            return -1; // Return an error if the array is full
        }

         for (int i = 0; i < num_directories - 1; i++) {
        for (int j = 0; j < num_directories - i - 1; j++) {
            if (strcmp(directories[j], directories[j + 1]) > 0) {
                char temp[MAX_DIR_LENGTH];
                strcpy(temp, directories[j]);
                strcpy(directories[j], directories[j + 1]);
                strcpy(directories[j + 1], temp);
            }
        }
    }
    }
    return 0; // Continue traversal

}
void search_files(const char *dir_path, const char **extensions, int num_extensions, FILE *file_list) {
   
    

    DIR *dir;
    struct dirent *entry;
    
    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden directories
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        
        struct stat statbuf;
        if (stat(path, &statbuf) == -1) {
            perror("Error stating file");
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) {
            // Recursively search subdirectories
            search_files(path, extensions, num_extensions, file_list);
        } else {
            // Check if entry name ends with one of the extensions
            const char *file_ext = strrchr(entry->d_name, '.');
           
            if (file_ext != NULL) {
                file_ext++; // Move past the '.'
                for (int i = 0; i < num_extensions; i++) {
                   // printf("Comparing extension: %s\n", extensions[i]);
                   // printf("%s file extention\n", file_ext);
                    if (strcmp(file_ext, extensions[i]) == 0) {
                        fprintf(file_list, "%s\n", path); // Write to file
                        //printf("%s\n", path); // Print to standard output
                        break; // Found matching extension, move to next entry
                        
                    }
                }
            }
        }
    }
    
    closedir(dir);
}





void handle_client(int client_socket) {
   
    char buffer[BUFFER_SIZE];
    home_dir = getenv("HOME");

    while (1) {
        num_directories = 0;
        num=0;
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = read(client_socket, buffer, sizeof(buffer));

        if (bytes_received <= 0) {
            
            close(client_socket);
            return;
        }

        // Process the received command
        if (strcmp(buffer, "dirlist -a\n") == 0) {
    //        if (nftw(home_dir, list_directories, 10, FTW_PHYS) == -1) {
    //     perror("nftw");
    //     return 1;
    // }
    //   for (int i = 0; i < num_dirs; i++) {
    //     ssize_t bytes_sent = write(client_socket, directories[i], strlen(directories[i]));
    //     if (bytes_sent < 0) {
    //         perror("Error sending response to client");
    //     }
    // }
        if (send_directory_structure(client_socket, home_dir) == -1) {
        perror("Sending directory structure failed");
        exit(EXIT_FAILURE);
    }
        } 
        else if(strcmp(buffer, "dirlist -t\n")==0){
            printf("%s",buffer);
            
if (send_directory_by_date(client_socket, home_dir) == -1) {
        perror("Sending directory structure failed");
        exit(EXIT_FAILURE);
    }
        }

        else if(strncmp(buffer,"w24fn ", 6)== 0){
            char filename[MAX_DIR_LENGTH];
            strncpy(filename, buffer + 6, strlen(buffer) - 6);
            filename[strlen(buffer) - 7] = '\0'; // Remove newline character

            // Set filename in file_info struct
            strcpy(file_info.filename, filename);
            printf("alallal");
           if (send_file_info(client_socket, home_dir) == -1) {
        perror("Sending directory structure failed");
        exit(EXIT_FAILURE);
    }

           

        }
        else if(strncmp(buffer,"w24fz ",6 )==0){
            char *token = strtok(buffer, " ");
           token = strtok(NULL, " "); // Get the next token (size1)
    if (token == NULL) {
        printf("Missing size1\n");
        return EXIT_FAILURE;
    }
     size1 = (off_t)strtoll(token, NULL, 10);

    token = strtok(NULL, " "); // Get the next token (size2)
    if (token == NULL) {
        printf("Missing size2\n");
        return EXIT_FAILURE;
    }
    size2 = (off_t)strtoll(token, NULL, 10);

    // if (nftw(home_dir, process_file, 10, FTW_PHYS) == -1) {
    //     perror("nftw");
    //     exit(EXIT_FAILURE);
    // }
    FILE *file_list = fopen("file_list.txt", "w");
    if (file_list == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // Start traversal from the root directory
    traverse_directory_collect_files(home_dir, file_list);
    fseek(file_list, 0, SEEK_END);

    fclose(file_list);
    long file_size = ftell(file_list);
    if(file_size==0){
        send(client_socket,"No file Found",14,0);
        send(client_socket, "END_OF_FILE", strlen("END_OF_FILE"), 0);
    }

    // Check if any files are found within the size range

   else if (access("file_list.txt", F_OK) != -1) {
        // Create tar archive of selected files
        create_tar_archive();
        printf("temp.tar.gz is created\n");
        tar_file = fopen("temp.tar.gz", "rb");
        if (tar_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
        size_t bytes_read;
        int last_chunk = 0;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, tar_file)) > 0) {
        ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("send");
            fclose(tar_file);
            exit(EXIT_FAILURE);
        }

        // Debugging statement to print the number of bytes sent
        printf("Bytes sent: %zd\n", bytes_sent);
        
        
    }
    
    fclose(tar_file);
    char null_terminator = '\0';
   send(client_socket, "END_OF_FILE", strlen("END_OF_FILE"), 0);

     remove("temp.tar.gz");
    } 
    
    
    
    else {
        printf("No file found\n");
        // You can implement sending "No file found" message to the client here
    }
        }
        else if (strncmp(buffer, "w24ft ", 6) == 0) {
            
            int count = 0;
    // Command "w24ft" received, parse the command to extract extensions
    char *extensions[4];
           
    char *token = strtok(buffer," ");
    token = strtok(NULL," ");
    while (token != NULL && count < 3) {
        size_t length = strlen(token);
        if (token[length - 1] == '\n') {
            token[length - 1] = '\0';
        }
        extensions[count] = token;
        count++;
        token = strtok(NULL, " ");
    }
    extensions[count] = NULL;
    
    printf("%d",count);
    for(int j=0;j<count;j++){
        printf("%s",extensions[j]);
    }
    
    FILE *file_list2 = fopen(FILE_LIST, "w");
    if (file_list2 == NULL) {
        perror("Error opening file list");
        return 1;
    }
    search_files(home_dir, extensions, count, file_list2);
    fseek(file_list2, 0, SEEK_END);
    long file_size = ftell(file_list2);
    fclose(file_list2);
    
    printf("the size of file is%ld",file_size);
    if(file_size==0){
        send(client_socket,"No_file_Found",14,0);
        //send(client_socket, "NOT_FILE", strlen("END_OF_FILE"), 0);
    }
    
    else if (access("file_list2.txt", F_OK) != -1) {
        // Create tar archive of selected files
        create_tar_archive2();
        printf("archive.tar.gz is created\n");
        tar_file = fopen("archive.tar.gz", "rb");
        if (tar_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
        size_t bytes_read;
        int last_chunk = 0;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, tar_file)) > 0) {
        ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("send");
            fclose(tar_file);
            exit(EXIT_FAILURE);
        }

        // Debugging statement to print the number of bytes sent
        //
        
        
    }
    
    fclose(tar_file);
    
    char null_terminator = '\0';
   send(client_socket, "END_OF_FILE", strlen("END_OF_FILE"), 0);
   remove("archive.tar.gz");
   remove(file_list2);

    } 
    

       
    }
   else if (strncmp(buffer, "w24fdb ", 7) == 0) {
    char *token = strtok(buffer, " "); // Tokenize the buffer
    if (token == NULL) {
        fprintf(stderr, "Invalid buffer format\n");
        exit(EXIT_FAILURE);
    }

    // Get the next token
    token = strtok(NULL, " ");
    if (token == NULL) {
        fprintf(stderr, "Missing date argument\n");
        exit(EXIT_FAILURE);
    }

    // Remove the newline character if present
    size_t len = strlen(token);
    if (len > 0 && token[len - 1] == '\n') {
        token[len - 1] = '\0';
    }

    // Debug print: Print the extracted date token
    printf("Date token: %s\n", token);

    // Parse the date string
    struct tm tm_target;
    if (strptime(token, "%Y-%m-%d",&tm_target) == NULL) {
        fprintf(stderr, "Invalid date format\n");
        exit(EXIT_FAILURE);
    }
    

    // Convert to time_t
    target_time = mktime(&tm_target);
    if (target_time == -1) {
        fprintf(stderr, "Failed to convert date to time_t\n");
        exit(EXIT_FAILURE);
    }
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("fopen");
        //closedir(dir);
        exit(EXIT_FAILURE);
    }
    //printf("Target time: %lld\n", (long long)target_time);
    find_files_before_time(home_dir,target_time,output_file);
    fseek(output_file, 0, SEEK_END);
    long file_size = ftell(output_file);
    fclose(output_file);
    
    printf("the size of file is%ld",file_size);
    if(file_size==0){
        
        send(client_socket,"No_file_Found",14,0);
        //send(client_socket, "NOT_FILE", strlen("END_OF_FILE"), 0);
    }
    
    else{
     create_tar_archive3();
        printf("temp.tar.gz is created\n");
        tar_file = fopen("temp.tar.gz", "rb");
        if (tar_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
        size_t bytes_read;
        int last_chunk = 0;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, tar_file)) > 0) {
        ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("send");
            fclose(tar_file);
            exit(EXIT_FAILURE);
        }

        // Debugging statement to print the number of bytes sent
        //printf("Bytes sent: %zd\n", bytes_sent);
        
        
    }
    
    fclose(tar_file);
    char null_terminator = '\0';
   send(client_socket, "END_OF_FILE", strlen("END_OF_FILE"), 0);
    //unlink("temp.tar.gz");
    //unlink("output.txt");
    remove("temp.tar.gz");
    remove("output.txt");
    }
}
   else if (strncmp(buffer, "w24fda ", 7) == 0) {
    unlink("temp.tar.gz");
    char *token = strtok(buffer, " "); // Tokenize the buffer
    if (token == NULL) {
        fprintf(stderr, "Invalid buffer format\n");
        exit(EXIT_FAILURE);
    }

    // Get the next token
    token = strtok(NULL, " ");
    if (token == NULL) {
        fprintf(stderr, "Missing date argument\n");
        exit(EXIT_FAILURE);
    }

    // Remove the newline character if present
    size_t len = strlen(token);
    if (len > 0 && token[len - 1] == '\n') {
        token[len - 1] = '\0';
    }

    // Debug print: Print the extracted date token
    printf("Date token: %s\n", token);

    // Parse the date string
    struct tm tm_target;
    if (strptime(token, "%Y-%m-%d", &tm_target) == NULL) {
        fprintf(stderr, "Invalid date format\n");
        exit(EXIT_FAILURE);
    }
    tm_target.tm_hour = 0;  // Set time to midnight
    tm_target.tm_min = 0;
    tm_target.tm_sec = 0;

    // Convert to time_t
    target_time = mktime(&tm_target);
    if (target_time == -1) {
        fprintf(stderr, "Failed to convert date to time_t\n");
        exit(EXIT_FAILURE);
    }
    FILE *output_file = fopen("output2.txt", "a");
    if (output_file == NULL) {
        perror("fopen");
        
        exit(EXIT_FAILURE);
    }


    //printf("Target time: %lld\n", (long long)target_time);
    find_files_after_time(home_dir,target_time,output_file);
    fseek(output_file, 0, SEEK_END);
    long file_size = ftell(output_file);
    fclose(output_file);
    
    printf("the size of file is%ld",file_size);
    if(file_size==0){
        send(client_socket,"No_file_Found",14,0);
        //send(client_socket, "NOT_FILE", strlen("END_OF_FILE"), 0);
    }
    else{
     create_tar_archive4();
        printf("temp.tar.gz is created\n");
        tar_file = fopen("temp.tar.gz", "rb");
        if (tar_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
        size_t bytes_read;
        int last_chunk = 0;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, tar_file)) > 0) {
        ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("send");
            fclose(tar_file);
            exit(EXIT_FAILURE);
        }

        // Debugging statement to print the number of bytes sent
     //   printf("Bytes sent: %zd\n", bytes_sent);
        
        
    }
    
    fclose(tar_file);
    char null_terminator = '\0';
   send(client_socket, "END_OF_FILE", strlen("END_OF_FILE"), 0);
   //unlink("temp.tar.gz");
   //unlink("output.txt");
   remove("temp.tar.gz");
   remove("output2.txt");
    }
}
    else if(strcmp(buffer,"quitc\n")==0){
        //close(client_socket);
        break;
    }
           
        
        
        else {
            // Echo back the received command as response
            printf("bla");
            write(client_socket, buffer, strlen(buffer));
        }
    }
}
bool is_empty(const char *str) {
    return str[0] == '\0';
}

int send_file_info(int connfd, const char * dirpath){
     // Use nftw to search for the file
     
            if (nftw(home_dir, find_file, 20, FTW_PHYS) == -1) {
                send(connfd, "No file found\n", strlen("No file found\n"), 0);
                perror("nftw");
                return;
            }
if(is_empty(response)){
    send(connfd,"No File Found!",15,0);
    char end_marker = '\0';
send(connfd, &end_marker, 1, 0);
 memset(&file_info, 0, sizeof(FileInfo));
 
}
else{         
 send(connfd, response, strlen(response), 0);
 char end_marker = '\0';
send(connfd, &end_marker, 1, 0);
 memset(&file_info, 0, sizeof(FileInfo));
}
}

void traverse_directory_collect_files(const char *dir_path, FILE *file_list) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden directories
        if (entry->d_name[0] == '.') {
            continue;
        }

        char path[MAX_DIR_LENGTH]; // Define MAX_PATH_LENGTH according to your needs
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(path, &st) == -1) {
            perror("stat");
            continue;
        }

        // Perform operations on the directory entry
        if (S_ISREG(st.st_mode) && st.st_size >= size1 && st.st_size <= size2) {
            // Append the file path to the list
            fprintf(file_list, "%s\n", path);
        } else if (S_ISDIR(st.st_mode)) {
            // Recursively traverse directories
            traverse_directory_collect_files(path, file_list);
        }
    }
     
    closedir(dir);
   
}


int compare_directories_by_creation_time(const void *a, const void *b) {
    const DirectoryInfo *dir_info_a = (const DirectoryInfo *)a;
    const DirectoryInfo *dir_info_b = (const DirectoryInfo *)b;
    return difftime(dir_info_a->creation_time, dir_info_b->creation_time);
}

void traverseDirectoryDate(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Exclude hidden files and directories
            char path[MAX_DIR_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

            struct stat st;
            if (stat(path, &st) == 0) {
                if (S_ISDIR(st.st_mode)) { // Check if it's a directory
                    // Store directory path and creation time
                    if (num < MAX_DIRS) {
                        strcpy(directories_info[num].path, path);
                        directories_info[num].creation_time = st.st_ctime;
                        num++;
                        traverseDirectoryDate(path); // Recursive call for subdirectories
                    } else {
                        fprintf(stderr, "Maximum number of directories exceeded\n");
                        closedir(dir);
                        return;
                    }
                }
            } else {
                perror("stat");
            }
        }
    }

    closedir(dir);
}

int send_directory_by_date(int connfd,const char * dirpath){
    // if (nftw(dirpath, sendbydate, 20, FTW_PHYS) == -1) {
    //     perror("nftw");
    //     return -1;
    // }
   traverseDirectoryDate(dirpath);

    // Sort directories by creation time
    qsort(directories_info, num, sizeof(DirectoryInfo), compare_directories_by_creation_time);

    // Send directory paths in the order of creation time
    for (int i = 0; i < num; i++) {
        char buffer[MAX_DIR_LENGTH];
        snprintf(buffer, sizeof(buffer), "\n%s", directories_info[i].path);
        send(connfd, buffer, strlen(buffer), 0);
    }
    char end_marker = '\0';
    send(connfd, &end_marker, 1, 0);
    num = 0; // Reset directory count for next operation
    return 0;
    
}



void traverseDirectory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    //printf("Directory opened: %s\n", dir_path);

    // Store the directory path
    strcpy(directories[num_directories], dir_path);
    num_directories++;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Exclude hidden files and directories
            char path[MAX_DIR_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

           // printf("Stored directory path: %s\n", path);

            if (entry->d_type == DT_DIR) {
                // Recursive call for subdirectories
                traverseDirectory(path);
            }
        }
    }

    closedir(dir);
    //printf("%s\n", dir_path);
}

int compare(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}
int send_directory_structure(int connfd, const char *dir_path) {
     traverseDirectory(dir_path);
    // Define nftw callback function to send directory structure
    // int send_dir_structure(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
        
    //       if (typeflag == FTW_D) {
    //     char buffer[MAXLINE];
    //     sprintf(buffer, "\n%s", fpath);
    //     send(connfd, buffer, strlen(buffer), 0);
    // }
    // return 0;
    // }

    // // Use nftw to traverse directory structure and send each file/directory name
    // if (nftw(dir_path, send_dir_structure, 20, 0) == -1) {
    //     perror("nftw");
    //     return -1;
    // }

    // return 0;
    // int flags = FTW_PHYS | FTW_ACTIONRETVAL; 
    // if (nftw(dir_path, populate_dir_paths, 20, FTW_PHYS) == -1) {
    //     perror("nftw");
    //     return -1;
    // }


    //directories[num_directories][2] = '\0';
    qsort(directories, num_directories, sizeof(char[MAX_DIR_LENGTH]), compare);
    // Send directory paths in reverse order
    for (int i = 0; i < num_directories; i++) {
        char buffer[MAXLINE];
        snprintf(buffer, sizeof(buffer), "\n%s", directories[i]);
        send(connfd, buffer, strlen(buffer), 0);
    }
    char end_marker = '\0';
    send(connfd, &end_marker, 1, 0);
    // Free allocated memory
    // for (int i = 0; i < num_directories; i++) {
    //     free(directories[i]);
    // }
    
    num_directories=0;
    return 0;
}

void handleMirror(int client_socket){
    write(client_socket, "MIRROR", 7);
    close(client_socket);
}




void start_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t addrlen = sizeof(server_address);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running, listening on port %d\n", PORT);

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        conn++;
        printf("%d",conn);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client connected: %s\n", client_ip);

        

        int pid = fork();
        if (pid == 0) {
            close(server_socket);
            
            handle_client(client_socket);
            close(client_socket);
            exit(0);
            
        } else if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else {
            close(client_socket);
        }
    }
}

int main() {
    start_server();
    return 0;
}
