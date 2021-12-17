#include <hello.h>

void print_number (int n) {
    for (int j=1; j<=n; j++) {
        putchar('*');
        if(j % 5 == 0) {
            putchar(' ');
        }
    }
}

void print_primes (int n_max) {
    for (int i = 2; i < n_max; i++) {
        int isprime = 1;
        for (int j = 2; j < i; j++) {
            if (i % j == 0) {
                isprime = 0;
                break;
            }
        }
        if (isprime) {
            print_number(i);
            putchar('\n');
        }
    }
}

int main(void) {
    printf("Hello world!\n");
    print_primes(30);
    return 0;
}
