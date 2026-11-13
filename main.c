#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>


int main(int argc, char **argv) {

	// Probably can use local variables instead of function global ones but eh

	// non initialized vars

	char input[128]; // Input for line number

	char curLineNum[32] = {}; // current line number in text
	ssize_t readReturn; // The size of what's returned from read
	char text[4096]; // text chunk
	int fd; // the file descriptor of the selected path
	
	int showLineNum = 0; // should show line number flag

	int linesInFile = 1; // where lines start
	
	// initialized ones
	const char *path = ".";

	// TODO: Make and argument parser nd use that to learn header files

	// get path from arguments
	if (argc >= 2) {
		path = argv[1];
	} else {
		printf("Please provide a path to a file!\n");
		return 1;
	}
	
	//printf("%d\n", argc);

	// if more arguments, make sure they are -l
	if (argc >= 3) {
		// printf("%s", argv[3]);
		if (strcmp("-l", argv[2]) == 0) {
			showLineNum = 1;
		}
	}

	// if even MORe args, make sure its lines, or if not then ask how many lines to show
	// its adog, it's curious come on

	if (argc >= 4) {
		strlcpy(input, argv[3], sizeof(input)); 
		//printf("%s", input);
		//input = argv[2];
	} else {
		printf("How many lines to show: ");
		// Handle this error ig? man says need of use of feof and ferror but too lazy rn need sleep
		char* getResponse = fgets(input, sizeof input, stdin);	
	}

	
	//char* getResponse = fgets(input, sizeof(input), stdin);

	// check to see if it is a number
	long howMuch = strtol(input, NULL, 10);
	
	// printf("%ld\n", howMuch);
	
	// if not number. omg it's.. NaN!!!! :0000 i love lua
	if (howMuch == 0) {
		// if (argc < 3) {
			printf("Not a number!\n");
			//printf("errno: %d\n", errno);
			return 1;
		// }
		howMuch = -1;
	}

	// Open the file and get the descriptor
	fd = open(path, O_RDONLY);
	
	if (fd <= -1) {
		printf("Couldn't find file!\n"); 
		printf("errno: %d\n", errno);
		return 1;
	}	

	//printf("File Descriptor: %d\n", fd);
		
	// Try to read file, sending the buffer to write it to
	int isLineStart = 1;
	int shouldBreak = 0;
	
	// keep reading chunks until EOL, read return 0 when EOL
	while((readReturn = read(fd, text, sizeof text)) > 0) {
		if (readReturn <= -1) {
			printf("Specified path is not a file!");
			printf("errno: %d\n", errno);	
			return 1;
		}

		//printf("readReturn: %zd\n", readReturn);
	
	
		// If read was successful, continue, otherwise return failure
		if (readReturn > 0) {

			// treat every byte
			for (ssize_t i = 0; i < readReturn; i++) {

				// if start and should show line num then do it
				if (isLineStart == 1 && showLineNum == 1) {
					int lineNumSize = snprintf(curLineNum, sizeof curLineNum, "%d", linesInFile);
					write(STDOUT_FILENO, curLineNum, lineNumSize);
					write(STDOUT_FILENO, " ", 1);
					//printf("%d ", curLineNum);
					isLineStart = 0;
				}

				// write byte
				write(STDOUT_FILENO, &text[i], 1);
			
				// if new line then make sure to say that and do the stuff
				if (text[i] == '\n') {
					isLineStart = 1;

					if (howMuch >= 0 && linesInFile == howMuch) {
						shouldBreak = 1;
						break;
					} 

					linesInFile++;
				}
			}
			//strlcat(curLineNum, text, 0);
			//write(STDOUT_FILENO, text, readReturn);
			//printf("%s", text);
			//printf("that was: %d\n", readReturn);
		} else {
			printf("Error reading file!");
			printf("errno: %d\n", errno);
			return 1;
		}

		if (shouldBreak == 1) {
			break;
		}

	}

	printf("That was %d new lines!\n", linesInFile);
	
	// Close file to make sure we dont overflow or some shit idk
	int fdClosed = close(fd);
	
	if (fdClosed <= -1) {
		printf("File wasn't closed properly!");
		printf("errno: %d\n", errno);
	}
	
	//printf("File Closed: %d\n", fdClosed);
	
	// Exit gracefully :)
	return 0;

}
