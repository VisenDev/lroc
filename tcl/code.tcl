source main.tcl

struct Vector2 {
    int x;
    int y;
};

int main (int argc, char ** argv) {
    int i = 1;
    const int b = 2;
    i = b;
    defer {
        free(i);
    } {
        for (i = 0; i < 100; ++i) {
            printf("i = %d\n", i);
        }
        return i;
    }
}

