#include <iostream>
#include <cassert>
#include <Vec2.h>

using namespace std;

typedef Vec2<double> Vec2f;
typedef Vec2<int> Vec2i;

int main(int argc, char** argv) {
    // Start out testing basic store and recall
    {
        Vec2i vec(3, 4);

        assert(vec.x == 3);
        assert(vec.y == 4);
    }

    // Test out scalar multiplication
    {
        Vec2i vec(3, 4);

        assert((vec * 2).x == 6);
        assert((5 * vec).y == 20);
        assert((vec / 3).x == 1);

        // Make sure our vector hasn't changed
        assert(vec.x == 3);
        assert(vec.y == 4);

        // Now onto operators that SHOULD change it
        vec *= 4;
        assert(vec.x == 12);
        assert(vec.y == 16);

        vec /= 2;
        assert(vec.x == 6);
        assert(vec.y == 8);
    }

    // Test out vector addition
    {
        Vec2i vec1(1, 2);
        Vec2i vec2(3, 4);

        assert((vec1 + vec2).x == 4);
        assert((vec2 + vec1).y == 6);
        assert((vec2 - vec1).x == 2);

        // Make sure it hasn't changed
        assert(vec1.x == 1 && vec1.y == 2);
        assert(vec2.x == 3 && vec2.y == 4);

        // And now operators that should change it
        vec1 += vec2;
        assert(vec1.x == 4 && vec1.y == 6);

        vec2 -= vec1;
        assert(vec2.x == -1 && vec2.y == -2);
    }

    // Test out dot and cross
    {
        Vec2i vec1(1, 2);
        Vec2i vec2(3, 4);

        assert(vec1.dot(vec2) == 11);
        assert(vec2.dot(vec1) == 11);

        // There are actually 2 possible returns values for cross,
        // the important thing is it's orthogonal to the original
        assert(vec1.dot(vec1.cross()) == 0);
        assert(vec2.dot(vec2.cross()) == 0);

        // It should also have the same norm... I'll just manually do it here.
        assert(vec1.dot(vec1) == vec1.cross().dot(vec1.cross()));
        assert(vec2.dot(vec2) == vec2.cross().dot(vec2.cross()));
    }

    // Test out norm functions
    {
        Vec2i iVec(3, 4);
        Vec2f fVec(3, 4);

        assert(iVec.normSq() == 25);
        // That's all that makes sense for integers

        assert(fVec.normSq() == 25.0);
        assert(fVec.norm() == 5.0);
        // Add in a 0.01 delta incase of floating point errors...
        assert(fVec.normalize().x < 0.601 && fVec.normalize().x > 0.599);
        assert(fVec.normalize().y < 0.801 && fVec.normalize().y > 0.799);
    }

    cout << "Everything looks good!" << endl;
    return 0;
}
