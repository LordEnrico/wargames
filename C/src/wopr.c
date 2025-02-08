#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>
#include <termios.h>
#include <stdbool.h>

#define CHARACTER_DELAY 7500  // 1000 = 1ms
#define MAX_TARGETS 4
#define MAX_STRING_LENGTH 20
#define INBOX 1
#define SENT_ITEMS 2

// Struct for user data
typedef struct {
    char username[100];
    char password[100];
    char name[100];
    int access_level;
    char last_logon[100];
} User;

// Define the Mail structure
typedef struct {
    char sender[100];
    char recipient[100];
    char subject[100];
    char message[500];
    char date[15];      // Format: DD-MM-YYYY
    char time[10];      // Format: HH:MM:SS
} Mail;

int game_running = 0;
int defcon = 5;
int hints = 0; // 0 = disabled, 1 = enabled
int shell_gpt = 0;  // 0 = disabled, 1 = enabled

void fix_backspace_key() {
	char system_command[100];
	snprintf(system_command, sizeof(system_command), "stty erase ^H");
    system(system_command);
}

void delayed_print(const char* str) {
    for (int i = 0; str[i]; i++) {
        putchar(str[i]);
        fflush(stdout);
        usleep(CHARACTER_DELAY);
    }
}

void not_delayed_print(const char* str) {
    for (int i = 0; str[i]; i++) {
        putchar(str[i]);
        fflush(stdout);
        usleep(500);
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clear_screen() {
    printf("\033[2J\033[H");
}

void author() {
    int asciiValues[] = {65, 78, 68, 89, 32, 71, 76, 69, 78, 78};
    int i;

    printf("\n");
    for(i = 0; i < 10; i++) {
        printf("%c", asciiValues[i]);
    }
    printf("\n\n");
}

void show_date() {
    time_t current_time = time(NULL);
    struct tm* time_info = localtime(&current_time);
    char date_string[100];
    strftime(date_string, sizeof(date_string), "\nDATE: %Y-%m-%d\n\n", time_info);
    delayed_print(date_string);
}

void show_time() {
    time_t current_time = time(NULL);
    struct tm* time_info = localtime(&current_time);
    char time_string[100];
    strftime(time_string, sizeof(time_string), "\nTIME: %H:%M:%S\n\n", time_info);
    delayed_print(time_string);
}     

void show_list() {
    delayed_print("\nUSE SYNTAX: LIST <TYPE>\n\n");
}

void connect_internet() {
        char command[200];
        snprintf(command, sizeof(command), "./internet.sh");
        system(command);

}

void connect_arpanet() {
        char command[200];
        snprintf(command, sizeof(command), "./telehack.sh");
        system(command);

}

const char *check_status_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    
    if (!file) {
        // If the file doesn't exist, create it with default content "enabled"
        file = fopen(filename, "w");
        if (!file) {
            perror("Error creating file");
            return "error";
        }
        fprintf(file, "enabled");
        fclose(file);
        
        // Return the default status
        return "enabled";
    }

    char status[10];  // enough to hold "enabled" or "disabled" and a null terminator
    if (fscanf(file, "%9s", status) != 1) {
        fclose(file);
        return "error";
    }

    fclose(file);

    if (strcmp(status, "enabled") == 0) {
        return "enabled";
    } else if (strcmp(status, "disabled") == 0) {
        return "disabled";
    } else {
        return "error";
    }
}

int set_status_to_file(const char *filename, int status_input) {
    const char *status;
    if (status_input == 0) {
        status = "disabled";
    } else if (status_input == 1) {
        status = "enabled";
    } else {
        fprintf(stderr, "Invalid input: %d\n", status_input);
        return -1;  // Return an error code
    }

    // Open the file for writing, which will create it if it doesn't exist
    // or overwrite its content if it does.
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Unable to open file for writing");
        return -1;  // Return an error code
    }

    fprintf(file, "%s", status);
    fclose(file);

    return 0;  // Successful write
}

bool userExists(const char* username) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("Error opening users database.\n");
        return false;
    }

    char lineBuffer[100];
    User user;
    while (fgets(lineBuffer, sizeof(lineBuffer), file)) {
        // Trim newline and copy username
        lineBuffer[strcspn(lineBuffer, "\n")] = 0;
        strcpy(user.username, lineBuffer);

        // Compare usernames
        if (strcmp(user.username, username) == 0) {
            fclose(file);
            return true;
        }

        // Skip next 4 lines, which are password, name, access_level, and last_logon for the user.
        for (int i = 0; i < 4; i++) {
            fgets(lineBuffer, sizeof(lineBuffer), file);
        }
    }
    fclose(file);
    return false;
}

void getCurrentDateTime(char* date, char* curr_time) {
    time_t now;
    struct tm newtime;
    
    time(&now);
    newtime = *localtime(&now);
    
    strftime(date, 15, "%d-%m-%Y", &newtime);
    strftime(curr_time, 10, "%H:%M:%S", &newtime);
}

void addMail(Mail mail) {
    getCurrentDateTime(mail.date, mail.time);
    FILE *file = fopen("mail.txt", "a+");
    fwrite(&mail, sizeof(Mail), 1, file);
    fclose(file);
}

// For the deleteAll function:
void deleteAll(const char* username, int mode) {
    FILE *file, *tempFile;
    Mail mail;

    file = fopen("mail.txt", "r");
    tempFile = fopen("tempMail.txt", "w");

    if (file == NULL || tempFile == NULL) {
        printf("ERROR OPENING FILE\n");
        return;
    }

    while (fread(&mail, sizeof(Mail), 1, file)) {
        if (mode == 0) { // Delete all from Inbox
            if (strcmp(mail.recipient, username) != 0) {
                fwrite(&mail, sizeof(Mail), 1, tempFile);
            }
        } else if (mode == 1) { // Delete all from Sent items
            if (strcmp(mail.sender, username) != 0) {
                fwrite(&mail, sizeof(Mail), 1, tempFile);
            }
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("mail.txt");
    rename("tempMail.txt", "mail.txt");
}

void emailFunction(User logged_on_user) {
    char choiceBuffer[10];
    int choice;
    char command[200];
    char buffer[500]; // Buffer to hold formatted output

    do {
        clear_screen();
        delayed_print("WOPR EMAIL SYSTEM\n\n");
        delayed_print("1. CREATE\n2. INBOX\n3. SENT ITEMS\n4. HOUSEKEEPING\n5. EXIT\n\nSELECT OPTION: ");
        
        fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
        choice = atoi(choiceBuffer);

        FILE *file;
        Mail mail;
        Mail mails[100];
        int mailCount = 0;

        switch (choice) {
            case 1:
                delayed_print("RECIPIENT: ");
                fgets(mail.recipient, sizeof(mail.recipient), stdin);
                mail.recipient[strcspn(mail.recipient, "\n")] = 0;

                if (!userExists(mail.recipient)) {
                    delayed_print("USER DOES NOT EXIST!\n");
                    usleep(1000000);
                    continue;
                }

                strcpy(mail.sender, logged_on_user.username);
                delayed_print("SUBJECT: ");
                fgets(mail.subject, sizeof(mail.subject), stdin);
                mail.subject[strcspn(mail.subject, "\n")] = 0;

                delayed_print("MESSAGE: ");
                fgets(mail.message, sizeof(mail.message), stdin);
                mail.message[strcspn(mail.message, "\n")] = 0;

                addMail(mail);
                delayed_print("EMAIL SENT!\n");
                usleep(1000000);
                continue;

            case 2: // INBOX
                file = fopen("mail.txt", "a+");
                while (fread(&mail, sizeof(Mail), 1, file)) {
                    if (strcmp(mail.recipient, logged_on_user.username) == 0) {
                        mails[mailCount++] = mail;
                    }
                }
                fclose(file);

                if (mailCount == 0) {
                    printf("YOU HAVE NO MAIL\n");
                    usleep(1000000);
                    continue;
                }

                int keepCheckingMailsInbox = 1; // Flag

                while (keepCheckingMailsInbox) {
                    sprintf(buffer, "\n%-4s %-20s %-30s %-12s %-10s\n", "No.", "From", "Subject", "Date", "Time");
                    delayed_print(buffer);
                    for (int i = 0; i < mailCount; i++) {
                        printf("%-4d %-20s %-30s %-12s %-10s\n", i + 1, mails[i].sender, mails[i].subject, mails[i].date, mails[i].time);
                    }

                    delayed_print("\nSELECT EMAIL NUMBER (0 = MENU): ");
                    fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
                    int mailChoice = atoi(choiceBuffer);

                    if (mailChoice == 0) {
                        keepCheckingMailsInbox = 0;
                        continue;
                    }

                    if (mailChoice > 0 && mailChoice <= mailCount) {
                        printf("\nFROM: %s\nDATE: %s\nTIME: %s\nSUBJECT: %s\nMESSAGE: %s\n\n", 
                            mails[mailChoice-1].sender, 
                            mails[mailChoice-1].date, 
                            mails[mailChoice-1].time, 
                            mails[mailChoice-1].subject, 
                            mails[mailChoice-1].message);
            
                        delayed_print("1. REPLY TO EMAIL\n2. RETURN TO LIST\n\nSELECT OPTION: ");
                        fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
                        int replyChoice = atoi(choiceBuffer);

                    if (replyChoice == 1) {
                        strcpy(mail.recipient, mails[mailChoice-1].sender);
                        strcpy(mail.sender, logged_on_user.username);
    
                        printf("RE: %s\n", mails[mailChoice-1].subject);
                        strcpy(mail.subject, "RE: ");
                        strcat(mail.subject, mails[mailChoice-1].subject);

                        delayed_print("MESSAGE: ");
                        fgets(mail.message, sizeof(mail.message), stdin);
                        mail.message[strcspn(mail.message, "\n")] = 0;

                        addMail(mail);
                        delayed_print("EMAIL SENT!\n");
                        usleep(1000000);
                    
                    } else if (replyChoice == 2) {
                            continue;
                        } else {
                            delayed_print("INVALID CHOICE. PLEASE TRY AGAIN.\n");
                        }
                    } else {
                        delayed_print("INVALID EMAIL NUMBER. PLEASE TRY AGAIN.\n");
                    }
                }
                continue;

            case 3: // SENT ITEMS
                file = fopen("mail.txt", "a+");
                while (fread(&mail, sizeof(Mail), 1, file)) {
                    if (strcmp(mail.sender, logged_on_user.username) == 0) {
                        mails[mailCount++] = mail;
                    }
                }
                fclose(file);

                if (mailCount == 0) {
                    delayed_print("YOU HAVE NO SENT MAILS\n");
                    usleep(1000000);
                    continue;
                }

                int keepCheckingSentMails = 1;

                while (keepCheckingSentMails) {
                    printf("\n%-4s %-20s %-30s %-12s %-10s\n", "No.", "To", "Subject", "Date", "Time");
                    for (int i = 0; i < mailCount; i++) {
                        printf("%-4d %-20s %-30s %-12s %-10s\n", i + 1, mails[i].recipient, mails[i].subject, mails[i].date, mails[i].time);
                    }

                    delayed_print("\nSELECT EMAIL NUMBER (0 = MENU): ");
                    fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
                    int mailChoice = atoi(choiceBuffer);

                    if (mailChoice == 0) {
                        keepCheckingSentMails = 0;
                        continue;
                    }

                    if (mailChoice > 0 && mailChoice <= mailCount) {
                        printf("\nTO: %s\nDATE: %s\nTIME: %s\nSUBJECT: %s\nMESSAGE: %s\n\n", 
                            mails[mailChoice-1].recipient, 
                            mails[mailChoice-1].date, 
                            mails[mailChoice-1].time, 
                            mails[mailChoice-1].subject, 
                            mails[mailChoice-1].message);
                        
                        delayed_print("1. RETURN TO LIST\n\nSELECT OPTION: ");
                        fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
                        int sentChoice = atoi(choiceBuffer);
                        
                        if (sentChoice == 1) {
                            continue;
                        } else {
                            delayed_print("INVALID CHOICE. PLEASE TRY AGAIN.\n");
                        }
                    } else {
                        delayed_print("INVALID EMAIL NUMBER. PLEASE TRY AGAIN.\n");
                    }
                }
                continue;

            case 4: // HOUSEKEEPING
                clear_screen();
                delayed_print("HOUSEKEEPING\n\n");
                delayed_print("1. DELETE ALL INBOX ITEMS\n2. DELETE ALL SENT ITEMS\n3. RETURN TO MENU\n\nSELECT OPTION: ");
                fgets(choiceBuffer, sizeof(choiceBuffer), stdin);
                int housekeepingChoice = atoi(choiceBuffer);

                switch (housekeepingChoice) {
                    case 1: // DELETE ALL INBOX ITEMS
                        deleteAll(logged_on_user.username, 0);
                        delayed_print("ALL INBOX ITEMS DELETED!\n");
                        usleep(1000000);
                        break;
                    case 2: // DELETE ALL SENT ITEMS
                        deleteAll(logged_on_user.username, 1);
                        delayed_print("ALL SENT ITEMS DELETED!\n");
                        usleep(1000000);
                        break;
                    case 3: // RETURN TO MAIN MENU
                        continue;
                    default:
                        delayed_print("INVALID CHOICE.\n");
                        usleep(1000000);
                        break;
                }
                break;

            case 5:
                delayed_print("EXITING WOPR EMAIL\n");
                usleep(1000000);
                break;

            default:
                delayed_print("INVALID CHOICE.\n");
                usleep(1000000);
                continue;
        }
    } while(choice != 5);
}

void help_joshua() {
    delayed_print("\nCOMMANDS: HELP, LIST, DATE, TIME, DEFCON, AUTHOR");
    delayed_print("\n          ARPANET, INTERNET, EXIT\n\n");
}

void help_user() {
    delayed_print("\nCOMMANDS: HELP, LIST, DATE, TIME, DEFCON, AUTHOR, USERS, MAIL, WHOAMI");
    delayed_print("\n          ARPANET, INTERNET, TIC-TAC-TOE, BACKDOOR, EXIT\n\n");
}

void help_games() {
    delayed_print("\n'GAMES' REFERS TO MODELS, SIMULATIONS, AND GAMES WHICH HAVE TACTICAL AND\nSTRATEGIC APPLICATIONS\n\n");
}

void list_games() {
    delayed_print("\nFALKEN'S MAZE\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("BLACK JACK\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("GIN RUMMY\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("HEARTS\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("BRIDGE\n");
    usleep(600000);
    delayed_print("CHECKERS\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("CHESS\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("POKER\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("FIGHTER COMBAT\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("GUERRILLA ENGAGEMENT\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("DESERT WARFARE\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("AIR-TO-GROUND ACTIONS\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("THEATERWIDE TACTICAL WARFARE\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
    delayed_print("THEATERWIDE BIOTOXIC AND CHEMICAL WARFARE\n");
    fflush(stdout); // flush the output buffer
    usleep(1500000);
    delayed_print("\nGLOBAL THERMONUCLEAR WAR\n\n");
    fflush(stdout); // flush the output buffer
    usleep(600000);
}

void defcon_status() {
    delayed_print("\nDEFCON: ");
    printf("%d",defcon);
    delayed_print("\n\n");
}

void create_root_user() {
    char* username = "root";
    char* password = "password";
    char* name = "root";
    int access_level = 9;
    char* last_logon = "Never";

    FILE* file = fopen("users.txt", "a");
    if (file) {
        // Check if user "root" exists. If true, exit the function.
        if (userExists(username)) return;
        fprintf(file, "%s\n%s\n%s\n%d\n%s\n", username, password, name, access_level, last_logon);
        fclose(file);
    } else {
        // Handle the error, e.g., print an error message
        //printf("Error opening or creating users.txt!\n");
    }
}

void manageUsers() {
    int choice;
    char inputBuffer[256];
    char buffer[500]; // Buffer to hold formatted output

    while (1) {
        delayed_print("\n");
        delayed_print("1. CREATE USER\n2. AMEND USER\n3. DELETE USER\n4. LIST USERS\n\nSELECT OPTION: ");
        
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        if (sscanf(inputBuffer, "%d", &choice) != 1) {
            break;  // Break out of the loop if no valid number is provided
        }

        FILE* file;
        User tempUser;
        char inputUsername[100];

        switch (choice) {
            case 1:
                file = fopen("users.txt", "a");
                if (!file) {
                    printf("Error opening or creating users.txt!\n");
                    return;
                }
                delayed_print("USERNAME           : ");
                fgets(tempUser.username, sizeof(tempUser.username), stdin);
                // Convert username to lowercase
                for (int i = 0; tempUser.username[i]; i++) {
                    tempUser.username[i] = tolower(tempUser.username[i]);
                }
                strtok(tempUser.username, "\n");
                
                delayed_print("PASSWORD           : ");
                fgets(tempUser.password, sizeof(tempUser.password), stdin);
                strtok(tempUser.password, "\n");
                
                delayed_print("NAME               : ");
                fgets(tempUser.name, sizeof(tempUser.name), stdin);
                strtok(tempUser.name, "\n");
                
                delayed_print("ACCESS LEVEL       : ");
                fgets(inputBuffer, sizeof(inputBuffer), stdin);
                sscanf(inputBuffer, "%d", &tempUser.access_level);

                strcpy(tempUser.last_logon, "Never");

                fprintf(file, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                        tempUser.access_level, tempUser.last_logon);

                fclose(file);
                delayed_print("USER ACCOUNTED CREATED.\n");
                break;

            case 2:
                file = fopen("users.txt", "r");
                if (!file) {
                    printf("users.txt not found. Create a user first.\n");
                    return;
                }
                delayed_print("USERNAME TO AMEND  : ");
                fgets(inputUsername, sizeof(inputUsername), stdin);
                strtok(inputUsername, "\n");

                FILE* tempFile = fopen("temp.txt", "w");
                if (!tempFile) {
                    printf("Error creating temp file!\n");
                    fclose(file);
                    return;
                }
                
                int amended = 0;
                while (fscanf(file, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                        &tempUser.access_level, tempUser.last_logon) != EOF) {
                    if (strcmp(tempUser.username, inputUsername) == 0) {
                        delayed_print("NEW PASSWORD       : ");
                        fgets(tempUser.password, sizeof(tempUser.password), stdin);
                        strtok(tempUser.password, "\n");
                        
                        delayed_print("NEW NAME           : ");
                        fgets(tempUser.name, sizeof(tempUser.name), stdin);
                        strtok(tempUser.name, "\n");
                        
                        delayed_print("NEW ACCESS LEVEL   : ");
                        fgets(inputBuffer, sizeof(inputBuffer), stdin);
                        sscanf(inputBuffer, "%d", &tempUser.access_level);

                        amended = 1;
                    }
                    fprintf(tempFile, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                            tempUser.access_level, tempUser.last_logon);
                }

                fclose(file);
                fclose(tempFile);
                remove("users.txt");
                rename("temp.txt", "users.txt");

                if (amended) {
                    delayed_print("USER ACCOUNT AMENDED.\n");
                } else {
                    printf("User not found.\n");
                }
                break;

            case 3:
                file = fopen("users.txt", "r");
                if (!file) {
                    printf("users.txt not found. Create a user first.\n");
                    return;
                }
                delayed_print("USERNAME TO DELETE : ");
                fgets(inputUsername, sizeof(inputUsername), stdin);
                strtok(inputUsername, "\n");

                //check whether user to delete is root
                if (strcmp(inputUsername, "root") == 0) {
                    delayed_print("ACCESS DENIED\n");
                    break;
                }

                FILE* delFile = fopen("delete.txt", "w");
                if (!delFile) {
                    printf("Error creating delete file!\n");
                    fclose(file);
                    return;
                }

                int deleted = 0;
                while (fscanf(file, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                        &tempUser.access_level, tempUser.last_logon) != EOF) {
                    if (strcmp(tempUser.username, inputUsername) != 0) {
                        fprintf(delFile, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                                tempUser.access_level, tempUser.last_logon);
                    } else {
                        deleted = 1;
                    }
                }

                fclose(file);
                fclose(delFile);
                remove("users.txt");
                if (deleted) {
                    rename("delete.txt", "users.txt");
                    delayed_print("USER ACCOUNT DELETED.\n");
                } else {
                    remove("delete.txt");
                    printf("User not found.\n");
                }
                break;

            case 4:
                file = fopen("users.txt", "r");
                if (!file) {
                    printf("users.txt not found. Create a user first.\n");
                    return;
                }
                delayed_print("\nUSERS:\n");
                delayed_print("-------------------------------------------------\n");
                sprintf(buffer, "| %-10s | %-15s | %-15s |\n", "Username", "Name", "Access Level");
                delayed_print(buffer);
                delayed_print("-------------------------------------------------\n");
                while (fscanf(file, "%s\n%s\n%s\n%d\n%s\n", tempUser.username, tempUser.password, tempUser.name, 
                        &tempUser.access_level, tempUser.last_logon) != EOF) {
                    printf("| %-10s | %-15s | %-15d |\n", tempUser.username, tempUser.name, tempUser.access_level);
                }
                delayed_print("-------------------------------------------------\n");
                fclose(file);
                break;

            default:
                printf("Invalid choice. Exiting...\n");
                return;  // Exit the function
        }
    }
    printf("\n");
}

void getPassword(char* password, size_t size) {
    struct termios old, new;
    int n = 0;
    char ch;

    // Disable buffering for terminal I/O so the PASS key is available.
    setvbuf(stdin, NULL, _IONBF, 0); 

    // Disable echo
    tcgetattr(fileno(stdin), &old);
    new = old;
    new.c_lflag &= ~(ECHO | ICANON);  // Disable echo and buffered input
    new.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    printf("PASSWORD: ");
    
    // Read characters one by one, mask them with '*', and store in password array
    while (n < size - 1) {
        ch = getchar();
        if(ch == '\n')
            break;
        putchar('*');
        password[n] = ch;
        n++;
    }
    password[n] = '\0';  // Null terminate the string

    // Restore terminal
    (void) tcsetattr(fileno(stdin), TCSAFLUSH, &old);
}

void guesscode() {
    char LC[] = "CPE1704TKS";
    int LC_percent = 0;
    char buffer[200];
    char input[100];

    srand(time(0)); // Initialize random seed

    int row = 10; // Desired row position
    int col = 32; // Desired column position

    clear_screen();
    delayed_print("\033[7mTERMINAL ECHO: WAR ROOM\033[0m\n");

    for (int A = 1; A <= strlen(LC); A++) {
        int LCG;
        do {
            printf("\033[%d;%dH", row, col);
            for (int B = 1; B <= LC_percent; B++) printf("%c ", LC[B - 1]);
            for (int B = 1; B <= strlen(LC) - LC_percent; B++) printf("- ");
            printf("\n");

            LCG = (rand() % (90 - 48 + 1)) + 48;
            if (LCG > 57 && LCG < 65) continue;

            printf("\033[%d;%dH", row, col);
            for (int B = 1; B <= LC_percent; B++) printf("%c ", LC[B - 1]);
            printf("%c ", (char)LCG);
                        for (int B = 1; B <= strlen(LC) - LC_percent - 1; B++) printf("- ");
            printf("\n");

            usleep(250 * 1000); // delay
        } while ((char)LCG != LC[A - 1]);
        LC_percent++;
    }
    usleep(2000000);
    clear_screen();
    delayed_print("\033[7mTERMINAL ECHO: WAR ROOM\033[0m\n");
    sprintf(buffer, "\033[%d;%dH%s", 10, 32, "\033[5mC P E 1 7 0 4 T K S\033[0m");        
    delayed_print(buffer);
    usleep(10000000);
    sprintf(buffer, "\033[%d;%dH%s", 23, 28, "PRESS ENTER KEY TO CONTINUE\n");
    delayed_print(buffer);

    while(1) {        
        char selection[3]; // to accommodate the character, the '\n', and the null-terminating character
        fgets(selection, sizeof(selection), stdin); // Read user's selection

        // If user just pressed Enter, break the outer loop
        if(selection[0] == '\n' && selection[1] == '\0') {
            break;
        }
    }
    clear_screen();
    snprintf(buffer, sizeof(buffer), "./tic-tac-toe");
    int status = system(buffer); // Only call system(command) once
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        if (exit_status == 1) {
            clear_screen();
            fflush(stdout); // flush the output buffer
            usleep(10000000);
            delayed_print("GREETINGS PROFESSOR FALKEN\n\n");
            if(hints == 1) {
                usleep(2000000);
            } 
            fgets(input, sizeof(input), stdin);
            // Remove trailing newline character
            input[strcspn(input, "\n")] = '\0';
            // Convert input to lowercase
            for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
            }
            //optionally, do something with the input - but not necessary
            usleep(500000);
            delayed_print("\nA STRANGE GAME. ");
            usleep(500000);
            delayed_print("THE ONLY WINNING MOVE IS NOT TO PLAY!\n\n");
            usleep(3000000);
            delayed_print("HOW ABOUT A NICE GAME OF CHESS?\n\n");
            defcon=5;
            game_running = 0;
        } else {
            clear_screen();
            fflush(stdout); // flush the output buffer
            usleep(10000000);
            delayed_print("\nYOU FAILED TO PREVENT WOPR FROM LAUNCHING THE MISSILES. ");
            usleep(500000);
            delayed_print("WWIII HAS COMMENCED!\n\n");
            usleep(5000000);
            defcon=1;
        }
    }
}

void map() {
    clear_screen();

    delayed_print("\n");
    delayed_print("     ____________/\\'--\\__         __                       ___/-\\             \n");
    delayed_print("   _/                   \\     __/  |          _     ___--/      / __          \n");
    delayed_print("  /                      |   /    /          / \\__--           /_/  \\/---\\    \n");
    delayed_print("  |                       \\_/    /           \\                            \\   \n");
    delayed_print("  |'                            /             |                            |  \n");
    delayed_print("   \\                           |            /^                             /  \n");
    delayed_print("    \\__                       /            |                          /---/   \n");
    delayed_print("       \\__                   /              \\              ___    __  \\       \n");
    delayed_print("          \\__     ___    ___ \\               \\_           /   \\__/  /_/       \n");
    delayed_print("              \\  /    \\_/   \\ \\                \\__'-\\    /                    \n");
    delayed_print("               \\/            \\/                      \\__/                     \n");
    delayed_print("\n");

    delayed_print("          UNITED STATES                               SOVIET UNION\n\n");

}

void end_game() {
    int gte = 50;
    int etr = 10;
    char buffer[200];

    for(int gc = 1; gc <= 10; gc++) {
        gte += 1;
        etr -= 1;

        if(gte == 60) {
            gte=0;
        }

        clear_screen();

        sprintf(buffer, "\033[%d;%dH%s", 19, 1, "--------------------------------------------------------------------------------");
        delayed_print(buffer);
        sprintf(buffer, "\033[%d;%dH%s", 20, 1, "GAME TIME ELAPSED");
        delayed_print(buffer);
        sprintf(buffer, "\033[%d;%dH%s", 20, 56, "ESTIMATED TIME REMAINING");
        delayed_print(buffer);

        if(gte >0) {
            sprintf(buffer, "\033[%d;%dH%s", 21, 1, "01 HRS 59 MIN");

        } else {
            sprintf(buffer, "\033[%d;%dH%s", 21, 1, "02 HRS 00 MIN");
        }
        delayed_print(buffer);
        printf(" SEC %02d", gte);

        sprintf(buffer, "\033[%d;%dH%s", 21, 56, "28 HRS 00 MIN");        
        delayed_print(buffer);
        printf(" SEC %02d", etr);

        sprintf(buffer, "\033[%d;%dH%s", 22, 1, "--------------------------------------------------------------------------------");
        delayed_print(buffer);

        usleep(1000000);
    }
    usleep(2000000);
    clear_screen();
    delayed_print("\033[7mTERMINAL ECHO: WAR ROOM\033[0m\n");
    delayed_print("TRZ. 34/53/76               SYS PROC 3435.45.6456           XCOMP STATUS: PV-456\n");
    delayed_print("ACTIVE PORTS: 34,53,75,94                                     CPU TM USED: 23:43\n");
    delayed_print("#45/34/53.           ALT MODE FUNCT: PV-8-AY345              STANDBY MODE ACTIVE\n");
    delayed_print("#543.654      #989.283       #028.392       #099.293      #934.905      #261.372\n");
    delayed_print("\n");

    delayed_print("                         MISSILES TARGETED AND READY\n");
    delayed_print("                         ---------------------------\n\n");
    usleep(1000000);
    delayed_print("\033[5m                             CHANGES LOCKED OUT\033[0m\n");
    delayed_print("                             ------------------\n"); 
    usleep(3000000);

    defcon=1;

    usleep(5000000);
    clear_screen();
    delayed_print("\033[7mTERMINAL ECHO: WAR ROOM\033[0m\n");
    delayed_print("TRZ. 34/53/76               SYS PROC 3435.45.6456           XCOMP STATUS: PV-456\n");
    delayed_print("ACTIVE PORTS: 34,53,75,94                                     CPU TM USED: 23:43\n");
    delayed_print("#45/34/53.           ALT MODE FUNCT: PV-8-AY345              STANDBY MODE ACTIVE\n");
    delayed_print("#543.654      #989.283       #028.392       #099.293      #934.905      #261.372\n");
    delayed_print("\n");

    delayed_print("                            PRIMARY TARGET IMPACT\n");
    delayed_print("                            ---------------------\n\n");
    usleep(1000000);

    delayed_print("                    LORING AIRFORCE BASE      : ");
    usleep(2000000);
    delayed_print("NO IMPACT\n");
    usleep(2000000);

    delayed_print("                    ELMENDORF AIRFORCE BASE   : ");
    usleep(2000000);
    delayed_print("NO IMPACT\n");
    usleep(2000000);

    delayed_print("                    GRAND FORKS AIRFORCE BASE : ");
    usleep(2000000);
    delayed_print("NO IMPACT\n");
    usleep(2000000);

    usleep(10000000);
    guesscode();
    
    //rest of game goes here
    //this should include: Joshua searching/finding launch codes
    //tic-tac-toe sequence

}

void global_thermonuclear_war() {
    int count = 0;
    char side[20];  // Array to store the selected side
    char input;
    int col=0; //print at col
    int row=0; //print at row
    int t;
    char buffer[200];
    char* prompt = "";
    startgame:
    clear_screen();
    map();
    delayed_print("WHICH SIDE DO YOU WANT?\n\n");
    if(count == 0) {
        delayed_print("  1. UNITED STATES\n");
        delayed_print("  2. SOVIET UNION\n\n");
        delayed_print("PLEASE CHOOSE ONE: ");
        if(hints == 1) {
            usleep(2000000);
        } 
        
        scanf(" %c", &input);
        
        if (input == '1') {
            strcpy(side, "UNITED STATES");
        } else if (input == '2') {
            strcpy(side, "SOVIET UNION");
        } else {
            delayed_print("\nINVALID OPTION\n\n");
            usleep(5000000);
        }
        count
