#include <algorithm>
#include <cmath>
#include <iostream>

void abssort(float *x, unsigned int n)
{
    std::sort(x, x + n,
              // lambda expression begins
              [](float a, float b) {
                  return (std::abs(a) < std::abs(b));
              } // end lambda expression
    );
}

int main()
{
    // [&] capture all external variable by reference
    // [=] capture all external value by value
    // [a, &b] capture a by value and b by reference
    int z = 10;
    auto num = [z](float x, float y) {
        return (x * x + 2 * x * y + y * y) * z;
    };

    std::cout << num(5, 2) << std::endl;
    return 0;
}