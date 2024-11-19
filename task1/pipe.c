#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2]; // Array to hold the pipe's file descriptors
    pid_t pid;
    char write_msg[] = "Hello from parent to child!";
    char read_msg[100];

    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid > 0) {
        // Parent process
        close(fd[0]); // Close unused read end of the pipe
        write(fd[1], write_msg, strlen(write_msg) + 1); // Write message to pipe
        close(fd[1]); // Close the write end of the pipe after writing
    } else {
        // Child process
        close(fd[1]); // Close unused write end of the pipe
        read(fd[0], read_msg, sizeof(read_msg)); // Read message from pipe
        printf("Child received message: %s\n", read_msg); // Print the received message
        close(fd[0]); // Close the read end of the pipe
    }

    return 0;
}