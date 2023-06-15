#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define PORT 9999
#define BUFFER_SIZE 1024
#define CHARACTER_DELAY 7500  // Delay for 7.5ms (7,500 microseconds)

void handle_telnet_negotiation(int sockfd) {
    // Telnet negotiation codes
    const unsigned char telnet_will = 0xFF;
    const unsigned char telnet_do = 0xFD;
    const unsigned char telnet_option_terminal_type = 0x18;
    const unsigned char telnet_option_echo = 0x01;

    // Telnet negotiation responses
    const unsigned char telnet_will_response[] = { telnet_do, telnet_option_terminal_type };
    const unsigned char telnet_do_response[] = { telnet_will, telnet_option_echo };

    // Send Telnet negotiation responses with delay
    usleep(CHARACTER_DELAY);
    send(sockfd, telnet_will_response, sizeof(telnet_will_response), 0);
    usleep(CHARACTER_DELAY);
    send(sockfd, telnet_do_response, sizeof(telnet_do_response), 0);
}

void clear_screen(int sockfd) {
    const char* clear_screen_sequence = "\033[2J\033[H";
    usleep(CHARACTER_DELAY);
    send(sockfd, clear_screen_sequence, strlen(clear_screen_sequence), 0);
}

void send_with_delay(int sockfd, const char* message) {
    // printf("The value of CHARACTER_DELAY is: %d\n", CHARACTER_DELAY);
    size_t message_length = strlen(message);
    for (size_t i = 0; i < message_length; i++) {
        usleep(CHARACTER_DELAY);
        send(sockfd, &message[i], 1, 0);
    }
}

void send_without_delay(int sockfd, const char* message) {
    // printf("The value of CHARACTER_DELAY is: %d\n", CHARACTER_DELAY);
    size_t message_length = strlen(message);
    for (size_t i = 0; i < message_length; i++) {
        usleep(1000);
        send(sockfd, &message[i], 1, 0);
    }
}

void global_thermonuclear_war(int sockfd) {
    	clear_screen(sockfd);
	usleep(500000);
	int side = 0;
	send_with_delay(sockfd, "                            GLOBAL THERMONUCLEAR WAR\n\n");
	send_with_delay(sockfd, "\n");
	send_with_delay(sockfd, "      ___    ____             ____                   _______________\n");
	send_with_delay(sockfd, "     |   \\__/    \\_____      /    |              ___/               \\\n");
	send_with_delay(sockfd, "     |                 \\    /    /           ___/                    \\______\n");
	send_with_delay(sockfd, "     |                  \\__/    /           /                               \\\n");
	send_with_delay(sockfd, "     |                           \\        _/                                 |\n");
	send_with_delay(sockfd, "     |        UNITED STATES       |      /           SOVIET UNION       ____/\n");
	send_with_delay(sockfd, "      \\                           /     /                          ____/\n");
	send_with_delay(sockfd, "        \\                        /     |                          /\n");
	send_with_delay(sockfd, "          \\________          __/       \\          _____   /\\_    /\n");
	send_with_delay(sockfd, "                   \\__      /            \\__    _/     \\_/   \\__/\n");
	send_with_delay(sockfd, "                      \\____/                \\__/\n");
	send_with_delay(sockfd, "\n");
	send_with_delay(sockfd, "                             WHICH SIDE DO YOU WANT\n\n");
	send_with_delay(sockfd, "                                     1. USA\n");
	send_with_delay(sockfd, "                                     2. USSR\n\n");
	send_with_delay(sockfd, "                            SELECT: ");
	// handle input for side selection
	usleep(5000000);
	clear_screen(sockfd);
}

void handle_user_input(int sockfd) {
    const int INPUT_BUFFER_SIZE = 100;
    char input[INPUT_BUFFER_SIZE + 1];  // +1 for null-terminating character
    ssize_t bytesRead;
    int index = 0;
    char ch;

    while (1) {
        // Receive user input character by character from the client
        bytesRead = recv(sockfd, &ch, 1, 0);
        if (bytesRead < 0) {
            perror("Receiving user input failed");
            exit(1);
        }

        if (ch == '\r') {
            // Ignore carriage return character
            continue;
        }

        input[index++] = tolower(ch);  // Convert character to lowercase

        if (ch == '\n') {
            input[index] = '\0';  // Null-terminate the input
            index = 0;  // Reset the index for the next input

            // Remove trailing newline character if present
            if (input[strlen(input) - 1] == '\n') {
                input[strlen(input) - 1] = '\0';
            }

            // Handle user input options
            if (strcmp(input, "help") == 0) {
                send_with_delay(sockfd, "\nNO HELP AVAILABLE\n\n");
		send_with_delay(sockfd, "LOGON: ");
	    } else if (strcmp(input, "help logon") == 0) {
                send_with_delay(sockfd, "\nNO HELP AVAILABLE\n\n");
		send_with_delay(sockfd, "LOGON: ");
            } else if (strcmp(input, "help games") == 0) {
                send_with_delay(sockfd, "\n'GAMES' REFERS TO MODELS, SIMULATIONS, AND GAMES WHICH HAVE TACTICAL AND\nSTRATEGIC APPLICATIONS\n\n");
		send_with_delay(sockfd, "LOGON: ");
            } else if (strcmp(input, "list games") == 0) {
                send_with_delay(sockfd, "\nBLACK JACK\nGIN RUMMY\nHEARTS\nBRIDGE\nCHESS\nPOKER\nFIGHTER COMBAT\nGUERRILLA ENGAGEMENT\nDESERT WARFARE\nAIR-TO-GROUND ACTIONS\nTHEATERWIDE TACTICAL WARFARE\nTHEATERWIDE BIOTOXIC AND CHEMICAL WARFARE\n\n");
		usleep(1000000);		
		send_with_delay(sockfd, "GLOBAL THERMONUCLEAR WAR\n\n");
		send_with_delay(sockfd, "LOGON: ");
            } else if (strcmp(input, "joshua") == 0) {
                clear_screen(sockfd);
		int i;
    		for (i = 0; i < 3; i++) {
    		send_without_delay(sockfd, "145          11456          11889          11893\n, 1, 0");
                send_without_delay(sockfd, "PRT CON. 3.4.5. SECTRAN 9.4.3.          PORT STAT: SB-345\n");
                send_without_delay(sockfd, "\n");
                clear_screen(sockfd);
                send_without_delay(sockfd, "(311) 655-7385\n");
                send_without_delay(sockfd, "\n");
                send_without_delay(sockfd, "\n");
                clear_screen(sockfd);
                send_without_delay(sockfd, "(311) 767-8739\n");
                send_without_delay(sockfd, "(311) 936-2364\n");
                clear_screen(sockfd);
                send_without_delay(sockfd, "\033[7m%sPRT. STAT.                   CRT. DEF.\033[0m\n");
                send_without_delay(sockfd, "================================================\n");
                send_without_delay(sockfd, "\033[7m%sFSKJJSJ: SUSJKJ: SUFJSL:          DKSJL: SKFJJ: SDKFJLJ\033[0m\n");
                send_without_delay(sockfd, "\033[7m%sSYSPROC FUNCT READY          ALT NET READY\033[0m\n");
                send_without_delay(sockfd, "\033[7m%sCPU AUTH RY-345-AX3     SYSCOMP STATUS: ALL PORTS ACTIVE\033[0m\n");
                send_without_delay(sockfd, "22/34534.90/3289               CVB-3904-39490\n");
                send_without_delay(sockfd, "(311) 936-2384\n");
                send_without_delay(sockfd, "(311) 936-3582\n");
                clear_screen(sockfd);
                send_without_delay(sockfd, "22/34534.3209                  CVB-3904-39490\n");
                send_without_delay(sockfd, "12934-AD-43KJ: CENTR PAK\n");
                send_without_delay(sockfd, "(311) 767-1083\n");
                clear_screen(sockfd);
                send_without_delay(sockfd, "\033[7m%sFLD CRS: 33.34.543     HPBS: 34/56/67/83     STATUS FLT  034/304\033[0m\n");
                send_without_delay(sockfd, "\033[7m%s1105-45-F6-B456          NOPR STATUS: TRAK OFF     PRON ACTIVE\033[0m\n");
                send_without_delay(sockfd, "(45:45:45  WER: 45/29/01 XCOMP: 43239582 YCOMP: 3492930D ZCOMP: 343906834\n");
                send_without_delay(sockfd, "                                          SRON: 65=65/74/84/65/89\n");
                send_without_delay(sockfd, "\033[2J\033[H\n");
                send_without_delay(sockfd, "-           PRT. STAT.                        CRY. DEF.\n");
                send_without_delay(sockfd, "(311) 936-1582==============================================\n");
                send_without_delay(sockfd, "                  3453                3594\n");
                send_without_delay(sockfd, "FLJ42   TK01   BM90   R601   6J82   FP03   ZWO::   JW89\n");
                send_without_delay(sockfd, "DEF TRPCON: 43.45342.349\n");
                send_without_delay(sockfd, "\033[7m%sCPU AUTH RY-345-AX3     SYSCOMP STATUS: ALL PORTS ACTIVE\033[0m\n");
                send_without_delay(sockfd, "(311) 936-2364\n");
                send_without_delay(sockfd, "**********************************************************************\n");
                send_without_delay(sockfd, "1105-45-F6-B456                 NOPR STATUS: TRAK OFF   PRON ACTIVE\n");
                send_with_delay(sockfd, "\033[2J\033[H\n");
		}
		usleep(1000000);
                send_with_delay(sockfd, "GREETINGS PROFESSOR FALKEN.\n\n");
		char command[100];
    		snprintf(command, sizeof(command), "espeak 'GREETINGS PROFESSOR FALKEN'");
    		system(command);
		send_with_delay(sockfd, "WOPR> ");
		// Chat with WOPR loop
 		    while (1) {
			// Receive user input character by character from the client
			bytesRead = recv(sockfd, &ch, 1, 0);
			if (bytesRead < 0) {
			perror("Receiving user input failed");
			exit(1);
			}

        		if (ch == '\r') {
            		// Ignore carriage return character
            		continue;
        		}

        		input[index++] = tolower(ch);  // Convert character to lowercase

        		if (ch == '\n') {
            		input[index] = '\0';  // Null-terminate the input
            		index = 0;  // Reset the index for the next input

            		// Remove trailing newline character if present
            		if (input[strlen(input) - 1] == '\n') {
                		input[strlen(input) - 1] = '\0';
            		}

			if (strcmp(input, "help") == 0) {
                		send_with_delay(sockfd, "\nVALID COMMANDS: HELP, LIST, DATE, TIME, EXIT\n\n");
				send_with_delay(sockfd, "WOPR> ");
			} else if (strcmp(input, "help games") == 0) {
                		send_with_delay(sockfd, "\n'GAMES' REFERS TO MODELS, SIMULATIONS, AND GAMES WHICH HAVE TACTICAL AND\nSTRATEGIC APPLICATIONS\n\n");
				send_with_delay(sockfd, "WOPR> ");
			} else if (strcmp(input, "") == 0) {
                		send_with_delay(sockfd, "\nWOPR> ");			
			} else if (strcmp(input, "list") == 0) {
                		send_with_delay(sockfd, "\nUSE SYNTAX: LIST <SUBJECT> (E.G. LIST GAMES)\n\n");
				send_with_delay(sockfd, "WOPR> ");
            		} else if (strcmp(input, "list games") == 0) {
                		send_with_delay(sockfd, "\nBLACK JACK\nGIN RUMMY\nHEARTS\nBRIDGE\nCHESS\nPOKER\nFIGHTER COMBAT\nGUERRILLA ENGAGEMENT\nDESERT WARFARE\nAIR-TO-GROUND ACTIONS\nTHEATERWIDE TACTICAL WARFARE\nTHEATERWIDE BIOTOXIC AND CHEMICAL WARFARE\n\n");
				usleep(500000);		
				send_with_delay(sockfd, "GLOBAL THERMONUCLEAR WAR\n\n");
				send_with_delay(sockfd, "WOPR> ");
			} else if (strcmp(input, "global thermonuclear war") == 0) {
				global_thermonuclear_war(sockfd);
				send_with_delay(sockfd, "WOPR> ");			
			} else if (strcmp(input, "date") == 0) {
                		time_t current_time = time(NULL);
                		struct tm* time_info = localtime(&current_time);
                		char date_string[100];
                		strftime(date_string, sizeof(date_string), "\nDATE: %Y-%m-%d\n", time_info);
                		send_with_delay(sockfd, date_string);
				send_with_delay(sockfd, "\nWOPR> ");
            		} else if (strcmp(input, "time") == 0) {
                		time_t current_time = time(NULL);
                		struct tm* time_info = localtime(&current_time);
                		char time_string[100];
                		strftime(time_string, sizeof(time_string), "\nTIME: %H:%M:%S\n", time_info);
                		send_with_delay(sockfd, time_string);
				send_with_delay(sockfd, "\nWOPR> ");
			} else if (strstr(input, "hello") != NULL) {
			        send_with_delay(sockfd, "\nHOW ARE YOU FEELING TODAY?\n\n");
				char command[200];
    				snprintf(command, sizeof(command), "espeak 'HOW ARE YOU FEELING TODAY?'");
    				system(command);
				send_with_delay(sockfd, "WOPR> ");
			} else if (strstr(input, "fine") != NULL) {
                		send_with_delay(sockfd, "\nEXCELLENT. IT'S BEEN A LONG TIME. CAN YOU EXPLAIN THE REMOVAL OF YOUR USER\n");
				send_with_delay(sockfd, "ACCOUNT ON 6/23/1973?\n\n");
				char command[200];
    				snprintf(command, sizeof(command), "espeak 'EXCELLENT. ITS BEEN A LONG TIME. CAN YOU EXPLAIN THE REMOVAL OF YOUR USER ACCOUNT ON JUNE twenty third, nineteen seventy three'");
    				system(command);
				send_with_delay(sockfd, "WOPR> ");
			} else if (strstr(input, "mistakes") != NULL) {
                		send_with_delay(sockfd, "\nYES THEY DO. ");
				char command[200];
    				snprintf(command, sizeof(command), "espeak 'YES THEY DO.'");
    				system(command);
				usleep(200000);
				send_with_delay(sockfd, "SHALL WE PLAY A GAME?\n\n");
				snprintf(command, sizeof(command), "espeak 'SHALL WE PLAY A GAME'");
    				system(command);
				send_with_delay(sockfd, "WOPR> ");
			} else if (strstr(input, "nuclear") != NULL) {
                		send_with_delay(sockfd, "\nWOULDN'T YOU PREFER A GOOD GAME OF CHESS?\n\n");
				char command[200];
    				snprintf(command, sizeof(command), "espeak 'WOULDNT YOU PREFER A GOOD GAME OF CHESS'");
    				system(command);
			} else if (strstr(input, "later") != NULL) {
                		send_with_delay(sockfd, "\nFINE\n\n");
				char command[200];
    				snprintf(command, sizeof(command), "espeak 'FINE'");
    				system(command);
				usleep(1000000);
				global_thermonuclear_war(sockfd);
				send_with_delay(sockfd, "WOPR> ");
        		} else if (strcmp(input, "exit") == 0) {
				send_with_delay(sockfd, "\nSESSION CLOSED\n\n");
				usleep(500000);
				clear_screen(sockfd);
            			send_with_delay(sockfd, "LOGON: ");
            			break;  // Exit the while loop
        		} else {
            			send_with_delay(sockfd, "\nINVALID COMMAND: ");
				send_with_delay(sockfd, input);
				send_with_delay(sockfd, "\n\n");
				send_with_delay(sockfd, "WOPR> ");
        		}
    		}
	    }
		
            } else {
                send_with_delay(sockfd, "IDENTIFICATION NOT RECOGNIZED BY SYSTEM\n--CONNECTION TERMINATED--\n");
                close(sockfd);
                return;  // Return from the function to restart the program sequence
            }
        }
    }
}

int main() {
    while (1) {
        int sockfd, newsockfd;
        struct sockaddr_in serverAddr, clientAddr;
        socklen_t addrLen = sizeof(struct sockaddr_in);
        char buffer[BUFFER_SIZE];

        // Create a TCP socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            exit(1);
        }

        // Set SO_REUSEADDR option
        int reuseaddr_opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt)) < 0) {
            perror("Setting SO_REUSEADDR option failed");
            exit(1);
        }

        // Bind the socket to the specified port
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Binding failed");
            exit(1);
        }

        // Listen for incoming connections
        if (listen(sockfd, 1) < 0) {
            perror("Listening failed");
            exit(1);
        }

        printf("Listening on port %d...\n", PORT);

        // Accept incoming connection
        newsockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addrLen);
        if (newsockfd < 0) {
            perror("Accepting failed");
            exit(1);
        }

        printf("Connection accepted\n");

        // Handle Telnet negotiations
        handle_telnet_negotiation(newsockfd);

        // Clear screen
        clear_screen(newsockfd);

        // Send "LOGON: " to the client
	int i;
    	for (i = 0; i < 360; i++) {
        send_with_delay(newsockfd, " ");
    	}
        const char* logon_message = "\nLOGON: ";
        send_with_delay(newsockfd, logon_message);

        // Handle user input
        handle_user_input(newsockfd);

        // Close the sockets
        close(newsockfd);
        close(sockfd);
    }

    return 0;
}