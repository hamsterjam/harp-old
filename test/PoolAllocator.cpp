#include <PoolAllocator.h>
#include <cassert>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    // Test intended functionality real quick
    {
        PoolAllocator p(sizeof(int) * 3);
        int* a = (int*) p.alloc(sizeof(int));
        int* b = (int*) p.alloc(sizeof(int));
        int* c = (int*) p.alloc(sizeof(int));
        int* d = (int*) p.alloc(sizeof(int));

        // Make sure we got expected return results
        assert(a != NULL);
        assert(b != NULL);
        assert(c != NULL);
        assert(d == NULL);

        // Make sure we dont seg fault on access
        *a = 1;
        *b = 2;
        *c = 3;

        assert(*a == 1);
        assert(*b == 2);
        assert(*c == 3);

        // Freeing everything should allow us to allocated 3 more integers
        p.freeAll();
        a = (int*) p.alloc(sizeof(int));
        b = (int*) p.alloc(sizeof(int));
        c = (int*) p.alloc(sizeof(int));
        d = (int*) p.alloc(sizeof(int));

        // Check values again
        assert(a != NULL);
        assert(b != NULL);
        assert(c != NULL);
        assert(d == NULL);
    }

    cout << "Everything looks good!" << endl;
    return 0;
}
