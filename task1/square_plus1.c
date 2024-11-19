#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Make file descriptor arrays for pipes
    int parentPipe[2];
    int childSquarePipe[2];
    int childPlusPipe[2];

    // Make pipes and check for errors
    if (pipe(parentPipe) == -1) {
        perror("Parent pipe failed. Exiting program.");
        return 1;
    }
    if (pipe(childSquarePipe) == -1) {
        perror("Parent pipe failed. Exiting program.");
        return 1;
    }
    if (pipe(childPlusPipe) == -1) {
        perror("Parent pipe failed. Exiting program.");
        return 1;
    }

    pid_t childSquarePid = fork();

    if (childSquarePid < 0) {
        perror("Fork failed. Exiting program.");
        return 1;
    } else if (childSquarePid == 0) {
        // Child square process
        close(parentPipe[1]); // Close unused write end of parent pipe
        close(childSquarePipe[0]); // Close unused read end of this child's pipe
        close(childPlusPipe[0]); // Close unused read end of other child's pipe
        close(childPlusPipe[1]); // Close unused write end of other child's pipe

        int number;
        while (read(parentPipe[0], &number, sizeof(int)) > 0) {
            number = number * number; // Square the value
            if (write(childSquarePipe[1], &number, sizeof(int)) == -1) {
                perror("Failed to write to childSquare. Exiting program.");
                return 1;
            }
        }
        close(parentPipe[0]); // Close parent pipe read end
        close(childSquarePipe[1]); // Close child pipe write end

        exit(EXIT_SUCCESS); //TODO: figure out what to do here. Is it needed?
    }

    pid_t childPlusPid = fork();
    if (childPlusPid < 0) {
        perror("Fork failed. Exiting program.");
        return 1;
    } else if (childPlusPid == 0) {
        // Child plus process
        close(parentPipe[0]); // Close unused read end of parent pipe
        close(parentPipe[1]); // Close unused write end of parent pipe
        close(childSquarePipe[1]); // Close unused write end of other child's pipe
        close(childPlusPipe[0]); // Close unused read end of this child's pipe

        int number;
        while (read(childSquarePipe[0], &number, sizeof(int)) > 0) {
            number = number + 1; // Square the value
            if (write(childPlusPipe[1], &number, sizeof(int)) == -1) {
                perror("Failed to write to childPlus. Exiting program.");
                return 1;
            }
        }
        close(childSquarePipe[0]); // Close child square pipe read end
        close(childPlusPipe[1]); // Close child plus pipe write end

        exit(EXIT_SUCCESS); //TODO: figure out what to do here. Is it needed?
    }

    // Else: parent process
    close(parentPipe[0]); // Close unused read end of parent
    close(childSquarePipe[0]); // Close unused read end of pipe2
    close(childSquarePipe[1]); // Close unused write end of pipe2
    close(childPlusPipe[1]); // Close unused write end of pipe3


    int number;
    while (scanf("%d", &number) != EOF) { // Source: https://www.geeksforgeeks.org/scanf-in-c/
        if (write(parentPipe[1], &number, sizeof(int)) == -1) {
            perror("Failed to write to parent. Exiting program.");
            return 1; 
        }

        if (read(childPlusPipe[0], &number, sizeof(int)) > 0) {
                printf("Output: %d\n", number);
        } else {
            perror("Failed to read from childPlus in parent. Exiting program.");
            return 1; 
        }
    }

    close(parentPipe[1]); // Close parent write end
    close(childPlusPipe[0]); // Close childPlus read end

    // Wait for children to finish
    waitpid(childSquarePid, NULL, 0);
    waitpid(childPlusPid, NULL, 0);

    return 0;
}