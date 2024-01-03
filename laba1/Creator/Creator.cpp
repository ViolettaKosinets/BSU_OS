#include <iostream>
#include <fstream>
#include "string"


using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	fstream out(argv[1], ios::out | ios::binary);
	int number_of_employees = atoi(argv[2]);
	for (int i = 0; i < number_of_employees; ++i) {
		employee e;
		cout << "Enter employee number: ";
		cin >> e.num;
		string temp;
		getline(cin, temp);
		cout << "Enter employee name(10 symbols max): ";
		cin.getline(e.name, 10);
		cout << "Enter working hours: ";
		cin >> e.hours;
		out.write((char*)&e, sizeof(employee));
	}
	out.close();
	return 0;
}