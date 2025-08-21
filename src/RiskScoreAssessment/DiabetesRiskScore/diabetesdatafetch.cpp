#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "diabetes.h"
using namespace std;
//Here, we have made a function which can read .csv file and consider only HbA1c colunm
//Convert these string to double and saves to vector "values"

vector<double>values;
void readCSV(const string& filename){
  ifstream file (filename);
  if(!file.is_open()){
    cerr<<"Error: Could not open the file \n";
  }

  string line;
  //int targetColum=6;  //Column 2
  int targetColum=12;
  getline(file,line);
  while (getline(file,line)) {
    stringstream ss(line);
    string cell;
    int colIndex=0;
    
    while (getline(ss,cell,',')){
        if (colIndex==targetColum){
            double val = stod(cell); 
            values.push_back(val);
            break;
        }
        colIndex++;
    }
  }
  file.close();

//Print extracted column
/*cout<<"Extracted column:first 10 values\n";
for (size_t i=0; i<columnData.size() && i<10; i++){
    cout<<columnData[i]<<endl;
}
cout<<"Type of data: "<<typeid(columnData).name()<<endl;*/
    

}