#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>


int generateNumber();
int main() {
    srand(time(NULL));
    int results[10];
    int i, num,wins;
    int next = 0;
    int occurences[10] = {0,0,0,0,0,0,0,0,0,0};

    #pragma omp parallel for private(i, num) schedule(static, 2) shared(next)
    for (i = 0; i < 10; i++) {
        num = generateNumber();

        // 'paste' number in array
        if (num > 9 && next < 10) {
            results[next] = num / 10;
            next++;
        }

        if (next < 10) {
            results[next] = num % 10;
            next++;
        }
    }

    // print arr
    for (i = 0; i < 10; i++) {
        printf("%d", results[i]);
    }

    // count occurences
    for (i = 0; i < 10; i++) {
        occurences[results[i]]++;

        if (occurences[results[i]] == 2) {
            wins++;
        }
    }

    printf("\nNumber of wins: %d\n", wins);
    
    return 0;
}

int generateNumber() {
    return 1 + (rand() % 25);    
}