#include <iostream>
#include <fstream>
#include <ctgmath>
#define physical_Layer "physicalLayer.txt"

using namespace std;
int main()
{
	int counter = 0;
	cout << endl << endl << "\t\t\t\t\t physicalLayer" << endl<<endl;
	while (1)
	{
		string s;
		int i=0;
		ifstream infile(physical_Layer);
		while (!infile.eof())
		{
			getline(infile, s);
			if (i >= counter&&(s.length() > 5))
			{
				cout << s<<endl;
				counter++;
			}
			i++;
		}
		infile.close();
	}

}
