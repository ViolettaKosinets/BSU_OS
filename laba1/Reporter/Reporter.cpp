#include <iostream>
#include <fstream>
#include <string>


using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[]) {
	ifstream in(argv[1], ios::binary);
	ofstream out(argv[2]);
	out << argv[0] << endl;
	out << "Employee number, employee name, hours, salary" << endl;;
	float salary_per_hour = atof(argv[3]);
	employee e;
	while (in.read((char*)&e, sizeof(employee))) {
		out << e.num << " " << e.name << " " << e.hours << " " << e.hours * salary_per_hour << endl;
	}
	in.close();
	out.close();
	return 0;
}