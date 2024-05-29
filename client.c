#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>


#define SERVER_IP "172.17.0.1"
#define SERVER_PORT 7862
#define MIRROR_PORT 7864
#define MIRROR_PORT2 7865
#define BUFFER_SIZE 1024
#define MAXLINE 1024
#define EOF_MARKER "EOF\n"

char folder_path1[1024];
char folder_path2[1024];
char folder_path3[1024];
char folder_path4[1024];
char *foldername1="fz.tar.gz";
char *foldername2="ft.tar.gz";
char *foldername4="fdb.tar.gz";
char *foldername3="fda.tar.gz";
char *mainFolder="w24Project";
const char *home_dir;
int conectServer(int num){
if(num == 1){
        int client_socket;
    struct sockaddr_in server_address;
    
    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    return(client_socket);
}
else if(num==2){
    int mirror_socket;
    struct sockaddr_in server_address;
    
    // Create socket
    if ((mirror_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(MIRROR_PORT);
     if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(mirror_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    return(mirror_socket);
}
else if(num = 3){
    int mirror_socket;
    struct sockaddr_in server_address;
    
    // Create socket
    if ((mirror_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(MIRROR_PORT2);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(mirror_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    return(mirror_socket);
}

}
int check_for_mirror(const char *data) {
    // Search for "Mirror" in the received data
    if (strstr(data, "Mirror") != NULL) {
        return 1; // "Mirror" found
    } else {
        return 0; // "Mirror" not found
    }
}
int validateftcommand(char *command){
	char *str_copy = malloc(strlen(command) + 1); 
    char ext1[10], ext2[10], ext3[10];
	strcpy(str_copy, command);
	char *token;
	token = strtok(str_copy," ");
	int i=0;
	while (token != NULL) {
        i++;
        token = strtok(NULL, " "); // Pass NULL to continue tokenizing the same string
    }
	if(i<2 ){
	printf("\nPass at least one extension\n");
    free(str_copy);
	return 0;
} 
else if(i>4){
printf("\nUP TO 3 EXTENTION ALLOWED");
free(str_copy);
return 0;

}
else if(i==2){
    free(str_copy);
    return 1;
}

else if(i==4 || i==3 ){
 
sscanf(command, "%*s %s %s %s", ext1, ext2, ext3);

if (strcmp(ext1, ext2) != 0 && strcmp(ext1, ext3) != 0 && strcmp(ext2, ext3) != 0) {
       free(str_copy);
       //printf("hey");
        return 1;
    } else {
        printf("Some extensions are the same or the command is not formatted correctly.\n");
        free(str_copy);
        return 0;
 }
}


}
int validatefn(char *command){
    int i=0;
    char *str_copy = malloc(strlen(command) + 1); 
    strcpy(str_copy, command);
    char *token = strtok(str_copy, " ");
     token = strtok(NULL," "); // Store the date from the second token
    if (token == NULL) {
        printf("Missing filename argument\n");
        free(str_copy);
        return 0;
    }
    while (token != NULL) {
        i++;
        token = strtok(NULL, " "); // Pass NULL to continue tokenizing the same string
    }
    if(i>1){
        printf("Only One Filed Allowed!\n");
        return 0;
    }
    return 1;
    

}
int validatesize(char *command){
    int size1, size2;
    char *str_copy = malloc(strlen(command) + 1); 
    strcpy(str_copy, command);
    char *token = strtok(str_copy, " ");
    token = strtok(NULL, " ");
    if (token == NULL) {
        printf("Missing size1\n");
        return 0;
    }
    size1 = atoi(token);
    token = strtok(NULL, " "); // Get the next token (size2)
    if (token == NULL) {
        printf("Missing size2\n");
        return 0;
    }
    size2 = atoi(token);
    token = strtok(NULL, " ");
    if(token!= NULL){
        printf("More then 2 size provided!\n");
    }
   
    if(size1 <= size2 && size1>=0 && size2>=0){
        free(str_copy);
        return 1;
    }
    else{
        free(str_copy);
        printf("Correct size not provided\n");
    }
    return 0;
}

int validatedir(char *command){
    char *str_copy = malloc(strlen(command) + 1); 
    strcpy(str_copy, command); // Allocate memory and copy command
    char *token = strtok(str_copy, " ");
    token = strtok(NULL," "); // Store the date from the second token
    if (token == NULL) {
        printf("Missing  arguments -a or -t\n");
        free(str_copy);
        return 0;
    }
    if(strcmp(token,"-a\n")==0 || strcmp(token,"-t\n")==0){
        free(str_copy);
        return 1;
    
        }
    printf("Valid dirlist commands -a or t\n");
    free(str_copy);
    return 0;

}

int validatedate(char *command){
struct tm td;
    char *str_copy = malloc(strlen(command) + 1); 
    strcpy(str_copy, command); // Allocate memory and copy command

   

    
    char *token = strtok(str_copy, " ");
    
    // // Find the second token
    

    token = strtok(NULL," "); // Store the date from the second token
    if (token == NULL) {
        printf("Missing date argument\n");
        free(str_copy);
        return 0;
    }

    // Remove the newline character if present
    size_t len = strlen(token);
    if (len > 0 && token[len - 1] == '\n') {
        token[len - 1] = '\0';
    }
    struct tm tm_target;
    if (strptime(token, "%Y-%m-%d", &tm_target) == NULL) {
        printf("incorrect Date Format\n");
        free(str_copy);
        return 0;
    }
    free(str_copy);
    return 1;
    // // // Attempt to parse the date
    // printf("hello");
    // return 0;
}

int main() {
    home_dir=getenv("HOME");
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    int client_socket;
    snprintf(folder_path1, sizeof(folder_path1), "%s/%s/%s", home_dir,mainFolder, foldername1);
    snprintf(folder_path2, sizeof(folder_path2), "%s/%s/%s", home_dir,mainFolder, foldername2);
    snprintf(folder_path3, sizeof(folder_path3), "%s/%s/%s", home_dir,mainFolder, foldername3);
    snprintf(folder_path4, sizeof(folder_path1), "%s/%s/%s", home_dir,mainFolder, foldername4);
    client_socket=conectServer(1);
          ssize_t bytes_received2;
           if ((bytes_received2 = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    if (bytes_received2 == 0) {
        printf("No data received. Continuing with other stuff.\n");
    } else {
        // Null-terminate the received data
        buffer[bytes_received2] = '\0';

        // Check for "MIRROR" command
        if (strcmp(buffer, "MIRROR1") == 0) {
           
          client_socket= conectServer(2);
        } 
        else if (strcmp(buffer, "MIRROR2") == 0) {
           
          client_socket= conectServer(3);
        } 
    }
    while (1) {
        
    
        // Prompt user for command
        printf("Enter command: ");
        fgets(command, BUFFER_SIZE, stdin);
	if (strncmp(command, "w24ft", 5) == 0) {
                int j= validateftcommand(command);
                if(!j){
                continue;
}
 }     
    else if(strncmp(command,"w24fz", 5)==0){
        int z= validatesize(command);
        if(!z){
            continue;
        }
    }

	else if(strncmp(command, "w24fdb", 6) == 0){
        
        
	int d= validatedate(command);
	if(!d){
	continue;
}

    }

else if(strncmp(command, "w24fda", 6) == 0){
        
        
	int d= validatedate(command);
	if(!d){
	continue;
}

    }
    else if(strncmp(command,"w24fn",5)==0){
        int f=validatefn(command);
        if(!f){
	continue;
}
    }

    else if(strncmp(command,"dirlist",7)==0){
        int dl = validatedir(command);
        if(!dl){
	continue;
}
    }
    else if(strncmp(command,"quitc",5)==0){
        printf("Existing. Please wait for server response\n");
    }
    else{
        printf("Please Pass Appropriate Arguments\n");
        continue;
    }
        
        // Send command to server
        if (send(client_socket, command, strlen(command), 0) != strlen(command)) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        

        // Receive response from server
        
        if (strncmp(command, "w24fz ", 6) == 0) {
            FILE *tar_file = fopen(folder_path1, "wb");
            if (tar_file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            ssize_t bytes_received;
            while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
                fwrite(buffer, 1, bytes_received, tar_file);
                buffer[bytes_received] = '\0'; // Null-terminate the received data

    // Check for end-of-file flag
   if (strstr(buffer, "END_OF_FILE") != NULL) {
        printf("File transmission complete.\n");
        break;
    }
            }

            if (bytes_received < 0) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }

            fclose(tar_file);
        } 
        
       else if (strncmp(command, "w24ft ", 6) == 0) {
		
            FILE *tar_file = fopen(folder_path2, "wb");
            if (tar_file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            ssize_t bytes_received;
            while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
                if(strncmp(buffer,"No_file_Found",14)==0){
                    printf("%s\n",buffer);
                    break;
                }
                fwrite(buffer, 1, bytes_received, tar_file);
                buffer[bytes_received] = '\0'; // Null-terminate the received data

    // Check for end-of-file flag
    if (strstr(buffer, "END_OF_FILE") != NULL) {
        printf("File transmission complete.\n");
        break;
    }
    
            }

            if (bytes_received < 0) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }

            fclose(tar_file);
        } 
         else if (strncmp(command, "w24fdb ", 7) == 0) {
            FILE *tar_file = fopen(folder_path4, "wb");
            if (tar_file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            ssize_t bytes_received;
            while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
                if(strncmp(buffer,"No_file_Found",14)==0){
                    printf("%s\n",buffer);
                    //fclose(tar_file);
                    break;
                }
                fwrite(buffer, 1, bytes_received, tar_file);
                buffer[bytes_received] = '\0'; // Null-terminate the received data

    // Check for end-of-file flag
   if (strstr(buffer, "END_OF_FILE") != NULL) {
        printf("File transmission complete.\n");
        break;
    }
            }

            if (bytes_received < 0) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }

            fclose(tar_file);
        } 


         else if (strncmp(command, "w24fda ", 7) == 0) {
            FILE *tar_file = fopen(folder_path3, "wb");
            if (tar_file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            ssize_t bytes_received;
            while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
                if(strncmp(buffer,"No_file_Found",14)==0){
                    printf("%s\n",buffer);
                    //fclose(tar_file);
                    break;
                }
                fwrite(buffer, 1, bytes_received, tar_file);
                buffer[bytes_received] = '\0'; // Null-terminate the received data

    // Check for end-of-file flag
    if (strstr(buffer, "END_OF_FILE") != NULL) {
        printf("File transmission complete.\n");
        break;
    }
            }

            if (bytes_received < 0) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }

            fclose(tar_file);
        } 
        else if(strcmp(command,"quitc\n")==0){
            ssize_t received_bytes = recv(client_socket, buffer, MAXLINE - 1, 0);
        if (received_bytes == 0) {
            printf("Server closed the connection.\n");
            break; // Exit the loop
        } else if (received_bytes < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }
        
        }
        
        else {
            ssize_t received_bytes;
            while ((received_bytes = recv(client_socket, buffer, MAXLINE - 1, 0)) > 0) {
               
                 // Ensure null termination
           
               // printf("%s\n", buffer);
                 fwrite(buffer, 1, received_bytes, stdout);
                 printf("\n");
        // Check if the received data contains a null terminator
        if (memchr(buffer, '\0', received_bytes) != NULL) {
            // Received null terminator, end of message
            break;
        }

            }
            if (received_bytes < 0) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Close socket
   close(client_socket);

    return 0;
}
