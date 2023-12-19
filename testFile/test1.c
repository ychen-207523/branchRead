#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10

void processData(int data[], int size);
int calculateSum(int data[], int size);
int findMax(int data[], int size);

int main() {
    int data[ARRAY_SIZE];
    int sum, max;

    // Initialize array with random values
    printf("Initializing array with random values...\n");
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        data[i] = rand() % 100; // Random numbers between 0 and 99
        printf("data[%d] = %d\n", i, data[i]);
    }

    printf("\nProcessing data...\n");
    processData(data, ARRAY_SIZE);

    printf("\nCalculating sum...\n");
    sum = calculateSum(data, ARRAY_SIZE);
    printf("Sum of array elements: %d\n", sum);

    printf("\nFinding maximum value...\n");
    max = findMax(data, ARRAY_SIZE);
    printf("Maximum value in the array: %d\n", max);

    return 0;
}

void processData(int data[], int size) {
    printf("Function: processData\n");
    for (int i = 0; i < size; ++i) {
        // Perform some processing on data
        data[i] = data[i] * 2;
        printf("Processed data[%d] = %d\n", i, data[i]);
    }
}

int calculateSum(int data[], int size) {
    printf("Function: calculateSum\n");
    int sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += data[i];
    }
    return sum;
}

int findMax(int data[], int size) {
    printf("Function: findMax\n");
    int max = data[0];
    for (int i = 1; i < size; ++i) {
        if (data[i] > max) {
            max = data[i];
        }
    }
    return max;
}