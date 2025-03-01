// CMSC 341 - Spring 2024 - Project 4
#include "vacdb.h"
#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>     //used to get the current time
// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester{
  public:
    bool normalInsertion();
    bool errorInsertion();
    bool errorFind();
    bool nonCollideFind();
    bool collideFind();
    bool nonCollideRemove();
    bool collideRemove();
    bool rehashInsertion();
    bool rehashLoadFactor();
    bool rehashRemoval();
    bool rehashDeleteRatio();
    int findHelper(string name, int serial, prob_t probing, int tableSize, Patient** table, hash_fn hashCode);
    int probingHelper(Patient patient, prob_t probing, int tableSize, Patient** table, hash_fn hashCode);
};

unsigned int hashCode(const string str);

string namesDB[8] = {"rose", "maisa", "kristen", "aaron", "luis", "ishan", "jack", "alex"};

int main() {
  Tester tester;
  
  { //Test normal insertion
    cout << "Normal Insertion" << endl;
    
    if (tester.normalInsertion()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  }
  
  { //Test error find
    cout << "Find Operation: Error" << endl;
    
    if (tester.errorFind()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  }

  { //Test noncolliding find
    cout << "Find Operation: Non-Colliding Keys" << endl;
    
    if (tester.nonCollideFind()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  }
 
   { //Test colliding find
    cout << "Find Operation: Colliding Keys (no rehash)" << endl;
    
    if (tester.collideFind()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  }  
  
  { //Test noncolliding remove
    cout << "Remove Operation: Non-Colliding Keys" << endl;
    
    if (tester.nonCollideRemove()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 

  { //Test colliding remove
    cout << "Remove Operation: Colliding Keys (no rehash)" << endl;
    
    if (tester.collideRemove()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 
  
  { //Test insertion with rehash
    cout << "Insertion that requires rehash" << endl;
    
    if (tester.rehashInsertion()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 
  
  { //Test removal with rehash
    cout << "Removal that requires rehash" << endl;
    
    if (tester.rehashRemoval()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 
  
  { //Test rehash load factor completion
    cout << "Rehash: Load Factor Completion" << endl;
    
    if (tester.rehashLoadFactor()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 
  
  { //Test rehash delete ratio completion
    cout << "Rehash: Delete Ratio Completion" << endl;
    
    if (tester.rehashDeleteRatio()) { 
      cout << "\tTest passed!" << endl;
    } else {
      cout << "\tTest failed!" << endl;
    }
  } 

  return 0;
}


unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}

/***************************
FUNCTION IMPLEMENTATION HERE
***************************/

bool Tester::normalInsertion() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  dataList.push_back(Patient(namesDB[0], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(0));
  dataIndex.push_back(hashCode(namesDB[0]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[1], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(1));
  dataIndex.push_back(hashCode(namesDB[1]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[2], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(2));
  dataIndex.push_back(hashCode(namesDB[2]) % MINPRIME);
  
  pass = pass && (vacdb.m_currentSize == 3);
  
  for (int i= 0; i < vacdb.m_currentSize; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    pass = pass && (vacdb.m_currentTable[dataIndex.at(i)]->getKey() == dataList.at(i).getKey()) && (vacdb.m_currentTable[dataIndex.at(i)]->getSerial() == dataList.at(i).getSerial());
  }
  
  return pass;
}


bool Tester::errorFind() {
  vector<Patient> dataList;
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  
  pass = pass && (vacdb.m_currentCap == MINPRIME);
  
  for (int i = 0; i < 10; i++) {
    dataList.push_back(Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true));
    vacdb.insert(dataList.at(i));
  }
  
  Patient testPatient = Patient("shem", RndID.getRandNum(), true);
  
  //test passed if getPatient function returns an empty Patient object
  if (vacdb.getPatient(testPatient.getKey(), testPatient.getSerial()) == Patient()) {
    return true;
  }
  
  return false; 
}


bool Tester::nonCollideFind() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  VacDB vacdb(MINPRIME, hashCode, LINEAR);
  
  bool pass = true; 
  
  pass = pass && (vacdb.m_currentCap == MINPRIME);
  
  dataList.push_back(Patient(namesDB[0], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(0));
  dataIndex.push_back(hashCode(namesDB[0]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[1], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(1));
  dataIndex.push_back(hashCode(namesDB[1]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[2], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(2));
  dataIndex.push_back(hashCode(namesDB[2]) % MINPRIME);
  
  pass = pass && (vacdb.m_currentSize == 3);
  
  for (int i= 0; i < vacdb.m_currentSize; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    pass = pass && (vacdb.m_currentTable[dataIndex.at(i)]->getKey() == dataList.at(i).getKey()) && (vacdb.m_currentTable[dataIndex.at(i)]->getSerial() == dataList.at(i).getSerial());
  }
  
  return pass;
}


bool Tester::collideFind() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuantity(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 49;
  int index = 0;
  
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    } 
  }
  
  for (int i = 0; i < vacdb.m_currentSize; i++) {
    //pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == dataList.at(i));
    pass = pass && (vacdb.m_currentTable[dataIndex.at(i)]->getKey() == dataList.at(i).getKey()) && (vacdb.m_currentTable[dataIndex.at(i)]->getSerial() == dataList.at(i).getSerial());
  }
  
  return pass; 
}


bool Tester::nonCollideRemove() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, LINEAR);
  
  bool pass = true;
  
  pass = pass && (vacdb.m_currentCap == MINPRIME);
  
  dataList.push_back(Patient(namesDB[0], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(0));
  dataIndex.push_back(hashCode(namesDB[0]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[1], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(1));
  dataIndex.push_back(hashCode(namesDB[1]) % MINPRIME);
  
  dataList.push_back(Patient(namesDB[2], RndID.getRandNum(), true));
  vacdb.insert(dataList.at(2));
  dataIndex.push_back(hashCode(namesDB[2]) % MINPRIME);
  
  pass = pass && (vacdb.m_currentSize == 3);
  
  for (int i = 0; i < vacdb.m_currentSize; i++) {
    //pass = pass && (vacdb.getPatient(data->getKey(), data->getSerial()) == dataList.at(i));
    pass = pass && (vacdb.m_currentTable[dataIndex.at(i)]->getKey() == dataList.at(i).getKey()) && (vacdb.m_currentTable[dataIndex.at(i)]->getSerial() == dataList.at(i).getSerial());
  }

  //removal test    
  /**for (int i = 0; i < 3; i++) {
    vacdb.remove(dataList.at(i));
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == Patient());
    pass = pass && !(vacdb.m_currentTable[dataIndex.at(i)]->getUsed());
  }

  pass = pass && (vacdb.m_currNumDeleted == 3);
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);**/

  return pass;
}


bool Tester::collideRemove() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 49;
  int index = 0;

  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  //insertion
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    } 
  }
  
  for (int i = 0; i < vacdb.m_currentSize; i++) {
    //pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == dataList.at(i));
    pass = pass && (vacdb.m_currentTable[dataIndex.at(i)]->getKey() == dataList.at(i).getKey()) && (vacdb.m_currentTable[dataIndex.at(i)]->getSerial() == dataList.at(i).getSerial());
  }
  
  //removal test
  
  /**for(int i = 0; i < 30; i++) {
    vacdb.remove(dataList.at(i));
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == Patient());
    pass = pass && !(vacdb.m_currentTable[dataIndex.at(i)]->getUsed());
  }

  pass = pass && (vacdb.m_currNumDeleted == 30);
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);**/
  
  return pass;
}


bool Tester::rehashInsertion() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  vector<int> tempIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 51;
  int index = 0;

  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  //insertion
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    }
  }
  
  tempIndex.push_back(1);
  
  /**for (int i = 0; i < size; i++) {
    tempIndex.push_back(findHelper(dataList.at(i).getKey(), dataList.at(i).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }
  
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);
  
  for(int i = 0; i < size; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    //pass = pass && (vacdb.m_currentTable[tempIndex.at(i)] == dataList.at(i));
  }**/
  
  return pass;
}


bool Tester::rehashLoadFactor() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  vector<int> tempIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 55;
  int index = 0;

  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  //insertion
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    }
  }
  
  tempIndex.push_back(1);
  
  /**for (int i = 0; i < size; i++) {
    tempIndex.push_back(findHelper(dataList.at(i).getKey(), dataList.at(i).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }
  
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);
  
  for(int i = 0; i < size; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    //pass = pass && (vacdb.m_currentTable[tempIndex.at(i)] == dataList.at(i));
  }**/
  
  return pass;
}


bool Tester::rehashRemoval() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  vector<int> tempIndex;
  vector<int> newIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 71;
  int index = 0;

  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  //insertion
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    }
  }
  
  tempIndex.push_back(1);
  
  /**for (int i = 0; i < size; i++) {
    tempIndex.push_back(findHelper(dataList.at(i).getKey(), dataList.at(i).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }
  
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);
  
  for (int i = 0; i < size; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    //pass = pass && (vacdb.m_currentTable[tempIndex.at(i)] == dataList.at(i));
  } 
  
  for (int i = 0; i < size; i++){
    pass = pass && vacdb.remove(dataList.at(i));
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == Patient());
    pass = pass && !(vacdb.m_currentTable[tempIndex.at(i)]->getUsed());
  }

  //this is the trigger
  pass = pass && vacdb.remove(dataList.at(56));
  pass = pass && (vacdb.m_currentSize == 14);

  for(int i = 0; i < 13; i++){
    newIndex.push_back(findHelper(dataList.at(i+57).getKey(), dataList.at(i+57).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }

  for(int i = 0; i < 13; i++){
      pass = pass && (vacdb.getPatient(dataList.at(i+57).getKey(), dataList.at(i+57).getSerial()) == dataList.at(i+57));
      //pass = pass && (vacdb.m_currentTable[newIndex.at(i)] == dataList.at(i+57));
  }
  
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);**/
  
  return pass;
}


bool Tester::rehashDeleteRatio() {
  vector<Patient> dataList;
  vector<int> dataIndex;
  vector<int> tempIndex;
  vector<int> newIndex;
  
  Random RndID(MINID,MAXID);
  Random RndName(0,9);
  Random RndQuality(0,50);
  VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
  
  bool pass = true;
  bool insert = true;
  
  int size = 70;
  int index = 0;

  
  pass = pass && (vacdb.m_currentCap == MINPRIME);

  //insertion
  for (int i = 0; i < size; i++) {
    Patient data = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
    
    index = probingHelper(data, DOUBLEHASH, MINPRIME, vacdb.m_currentTable, hashCode);
    insert = vacdb.insert(data);
    
    if (insert) {
      pass = pass && insert;
      
      dataList.push_back(data);
      dataIndex.push_back(index);
    } else {
      pass = pass && (vacdb.getPatient(data.getKey(), data.getSerial()) == data);
      
      size--;
    }
  }
  
  tempIndex.push_back(1);
  
  for (int i = 0; i < size; i++) {
    tempIndex.push_back(findHelper(dataList.at(i).getKey(), dataList.at(i).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }
  
  /*pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);
  
  for (int i = 0; i < size; i++) {
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == dataList.at(i));
    //pass = pass && (vacdb.m_currentTable[tempIndex.at(i)] == dataList.at(i));
  } 
  
  for (int i = 0; i < size; i++){
    pass = pass && vacdb.remove(dataList.at(i));
    pass = pass && (vacdb.getPatient(dataList.at(i).getKey(), dataList.at(i).getSerial()) == Patient());
    pass = pass && !(vacdb.m_currentTable[tempIndex.at(i)]->getUsed());
  }

  //this is the trigger
  pass = pass && vacdb.remove(dataList.at(56));
  pass = pass && (vacdb.m_currentSize == 14);

  for(int i = 0; i < 13; i++){
    newIndex.push_back(findHelper(dataList.at(i+57).getKey(), dataList.at(i+57).getSerial(), DOUBLEHASH, vacdb.m_currentCap, vacdb.m_currentTable, hashCode));
  }

  for(int i = 0; i < 13; i++){
      pass = pass && (vacdb.getPatient(dataList.at(i+57).getKey(), dataList.at(i+57).getSerial()) == dataList.at(i+57));
      //pass = pass && (vacdb.m_currentTable[newIndex.at(i)] == dataList.at(i+57));
  }
  
  pass = pass && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldCap == 0) && (vacdb.m_oldSize == 0);**/
  
  return pass;
}


int Tester::findHelper(string name, int serial, prob_t probing, int tableSize, Patient** table, hash_fn hashCode) {
  int hash = hashCode(name) % tableSize;
  int increment = 1;
  
  while (table[hash] != nullptr) {
    if (probing == QUADRATIC){
        hash = (hash + increment * increment) % tableSize;
    } else if (probing == LINEAR) {
      hash = (hash + increment) % tableSize;
    } else if (probing == DOUBLEHASH) {
      hash = ((hash % tableSize) + increment * (11 - (hash % 11))) % tableSize;
    } else {
      return hash;
    }
    
    if(table[hash] == nullptr) {
        return -1;
    }
    
    if (table[hash]->getKey() == name && table[hash]->getSerial() == serial && table[hash]->getUsed() == true) {
      return hash;
    } else {
      increment++;
    }
  }
  
  return -1;
}


int Tester::probingHelper(Patient patient, prob_t probing, int tableSize, Patient** table, hash_fn hashCode) {
  int index = hashCode(patient.getKey()) % tableSize;
  int increment = 1;
  
  while (table[index] != nullptr) {
    if (probing == QUADRATIC) {
      index = (index + increment * increment) % tableSize;
    } else if (probing == LINEAR) {
      index = (index + increment) % tableSize;
    } else if (probing == DOUBLEHASH) {
      index = ((index % tableSize) + increment * (11 - (index % 11))) % tableSize;
    }
    
    increment++;
  }
  
  return index;
}