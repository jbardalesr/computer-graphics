#include <bits/stdc++.h>

using namespace std;

class Animal
{
private:
    string type;
    string id;

public:
    // overload of contructor
    Animal(string _type)
    {
        type = _type;
        id = "UUID1987842";
    }

    Animal(string _type, string _id)
    {
        type = _type;
        id = _id;
    }

    void printType();

    void printId()
    {
        cout << "Your id is: " << id << endl;
    }

    ~Animal()
    {
        // free memory
        cout << "Bye" << endl;
    }
};

void Animal::printType()
{
    cout << "Type of animal is " << type << endl;
}

/* Inheritance
class Canino : public Animal
{
private:
    int age;

public:
    int getAge()
    {
        return age;
    }
};
*/

int main()
{
    Animal animal1("Dog", "7878");
    animal1.printId();
    animal1.printType();

    return 0;
}