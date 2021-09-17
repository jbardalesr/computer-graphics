#include <iostream>
#include <math.h>

using namespace std;

// With templates we can pass a value as parameter
// typename===class
// FUNCTION TEMPLATE
template <typename T>
T myMax(T x, T y)
{
    return (x > y) ? x : y;
}

// CLASS TEMPLATE
template <class T>
class mypair
{
    T a, b;

public:
    mypair(T first, T second)
    {
        a = first;
        b = second;
    }
    T sum()
    {
        return std::sqrt(a * a + b * b);
    }
};

// STRUCTURE TEMPLATE
template <typename T>
struct Person
{
    T age;
    string name;
};

int main()
{
    auto maxInt = myMax<int>(3, 7);
    cout << "typename int: " << maxInt << endl;
    cout << "typename double: " << myMax<double>(3.0, 7.0) << endl;
    cout << "typename char: " << myMax<char>('g', 'e') << endl;

    mypair<int> myobject(12, 5);
    cout << "pythagorean: " << myobject.sum() << endl;

    Person<int> person;
    cout << person.age << " " << person.name << endl;
}
