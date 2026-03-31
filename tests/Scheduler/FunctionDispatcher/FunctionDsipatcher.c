#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

struct pair_int_double {
    void* first;
    int second;
};

struct i2c {
    int x;
    char y;
};


void capi_init(int x) {

}

void capi_send(int x, struct i2c i2c) {
    
}

struct pair_int_double map[20];
void init() {
    map[0].first = (void*)&capi_init;
    map[0].second = 10;

    map[1].first = (void*)&capi_send;
    map[1].second = 20;
}

int get_id_from_address(void *func) {
    for (int i = 0; i < 2; i++) {
        if (map[i].first == func)
            return map[i].second;
    }
    return -1;
}


int capi(void (*func)(void), int arg_count, ...) {
    
    int identifier = get_id_from_address(func);
    printf("Function with id assigned to %d \n", identifier);

    return 0;
}

int main() {
    init();
    capi((void (*)(void))capi_init, 1, 'c');
    return 0;
}