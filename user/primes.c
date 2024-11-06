#include "types.h"
#include "user.h"
#include "fcntl.h"

void sieve(int read_fd) {
    int prime;
    if (read(read_fd, &prime, sizeof(int)) <= 0) {
        close(read_fd);
        exit(0);
    }
    printf("Prime: %d\n", prime);

    int next_pipe[2];
    pipe(next_pipe);

    if (fork() == 0) {
        close(next_pipe[1]);  // Close unused write end
        sieve(next_pipe[0]);
    } else {
        close(next_pipe[0]);  // Close unused read end
        int num;
        while (read(read_fd, &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(next_pipe[1], &num, sizeof(int));
            }
        }
        close(next_pipe[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int max = 100;  // Upper limit for primes
    int first_pipe[2];
    pipe(first_pipe);

    if (fork() == 0) {
        close(first_pipe[1]);
        sieve(first_pipe[0]);
    } else {
        close(first_pipe[0]);
        for (int i = 2; i <= max; i++) {
            write(first_pipe[1], &i, sizeof(int));
        }
        close(first_pipe[1]);
        wait(0);
    }
    exit(0);
}
