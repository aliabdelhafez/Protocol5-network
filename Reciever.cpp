#include <iostream>
#include <fstream>
#include <ctgmath>
#define inputFilePath "receiver.txt"
using namespace std;
int main()
{
	int counter = 0;
	cout << endl << endl << "\t\t\t\t\t\tReceiver" << endl << endl;
	while (1)
	{
		string s;
		string s_old;
		int i = 0;
		ifstream infile(inputFilePath);
		while (!infile.eof())
		{
			getline(infile, s);
			if (i >= counter &&(s.length()>5)&&(s_old!=s))
			{
				cout << s << endl;
				s_old=s;
				counter++;
			}
			i++;
		}
		infile.close();

	}
	return 0;

}
