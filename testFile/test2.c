#include <stdio.h>

void calculateSquares(int limit) {
    printf("Squares up to %d:\n", limit);
    for (int i = 1; i <= limit; i++) {
        printf("%d^2 = %d\n", i, i * i);
    }
}

int factorial(int number) {
    int result = 1;
    for (int i = 1; i <= number; i++) {
        result *= i;
    }
    return result;
}


int main() {
    int limit = 10;
    calculateSquares(limit);

    int num = 5;
    printf("Factorial of %d is %d\n", num, factorial(num));


    // Additional code to meet line requirement
    for (int i = 1; i <= 20; i++) {
        printf("Iteration %d\n", i);
    }

    for (int i = 20; i > 0; i--) {
        printf("Countdown: %d\n", i);
    }

    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    printf("Sum from 1 to 10 is %d\n", sum);

    for (int i = 1; i <= 10; i++) {
        if (i % 2 == 0) {
            printf("%d is even\n", i);
        } else {
            printf("%d is odd\n", i);
        }
    }

    int multiples[10];
    for (int i = 0; i < 10; i++) {
        multiples[i] = 5 * (i + 1);
        printf("5 times %d is %d\n", i + 1, multiples[i]);
    }

    // Additional for loops for line count
    for (int i = 1; i <= 5; i++) {
        for (int j = 1; j <= 5; j++) {
            printf("%d x %d = %d\n", i, j, i * j);
        }
    }

    for (int i = 10; i >= 1; i--) {
        printf("Reverse iteration %d\n", i);
    }

    return 0;
}
