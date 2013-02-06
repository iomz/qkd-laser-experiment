#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#define LEN 32

using namespace std;

int main(int argc, char** argv){
  ofstream ofs;
  int num_of_key;

  if(argc == 1)	num_of_key = 1;
  else{
	istringstream iss(argv[1]);
	iss >> num_of_key;
  }

  //int width = (int)'~' - (int)'!' + 1;
  int width = 2;
  srand((unsigned)time(NULL));
  ostringstream oss;

  for(int i=0;i<num_of_key;i++){
	oss.str("");
	oss << i << ".key";
	
	ofs.open(oss.str().c_str());
	for(int j=0;j<LEN;j++)
	  ofs << /*char((int)'!' + */rand()%width;

	ofs.close();
  }
}
