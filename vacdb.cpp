// CMSC 341 - Spring 2024 - Project 4
#include "vacdb.h"
VacDB::VacDB(int size, hash_fn hash, prob_t probing = DEFPOLCY) {
  if (size == 0 || !isPrime(size)) {
    //set m_currentCap to smallest prime number greater than the user's value
    int nextPrime = findNextPrime(size);
    m_currentCap = nextPrime;
  } else if (size < MINPRIME) {
    m_currentCap = MINPRIME;
  } else if (size > MAXPRIME) {
    m_currentCap = MAXPRIME;
  } else {
    m_currentCap = size;
  }
  
  //allocates memory for hash table
  m_currentTable = new Patient*[m_currentCap];
  
  for (int i = 0; i < m_currentCap; i++) {
    m_currentTable[i] = nullptr;
  }
  
  m_currentSize = 0;
  m_currNumDeleted = 0;
  m_currProbing = probing;
  
  //initializes old member variables
  m_oldTable = nullptr;
  m_oldCap = 0;
  m_oldSize = 0;
  m_oldNumDeleted = 0;
  m_oldProbing = probing;
  
  m_hash = hash;
  m_newPolicy = probing;
  m_transferIndex = 1;
}


VacDB::~VacDB() {
  //deletes the current table
  if (m_currentTable != nullptr) {
    for (int i = 0; i < m_currentCap; i++) {
      delete m_currentTable[i];
    }
  }
  
  delete [] m_currentTable;
}


void VacDB::changeProbPolicy(prob_t policy){
  m_newPolicy = policy;
  m_oldProbing = m_currProbing;
  m_currProbing = m_newPolicy;
}


bool VacDB::insert(Patient patient){
  //returns false if serial number value does not fall into range
  if (patient.getSerial() < MINID || patient.getSerial() > MAXID) {
    return false;
  }
  
  unsigned int index = m_hash(patient.getKey()) % m_currentCap;
  unsigned int increment = 1;
  
  //collision handling based on probing policy
  while (m_currentTable[index] != nullptr) {
    if (m_currProbing == QUADRATIC) {
      index = (index + increment * increment) % m_currentCap;
    } else if (m_currProbing == LINEAR) {
      index = (index + increment) % m_currentCap;
    } else if (m_currProbing == DOUBLEHASH) {
      index = ((index % m_currentCap) + increment * (11 - (index % 11))) % m_currentCap;
    }
    
    increment++;
  }
    
  //inserts patient into current table
  m_currentTable[index] = new Patient(patient);
  
  //increments size
  m_currentSize++;
  
  //rehash if load factor is greater than 0.5
  if (lambda() > 0.5) {
    //intializes old table
    m_oldCap = m_currentCap;
    m_oldSize = m_currentSize;
    m_oldTable = m_currentTable;
    
      
    //finds capacity of new table, which is the next smallest prime
    int num = (m_currentSize - m_currNumDeleted) * 4;
    int newCap = findNextPrime(num);
    
    //initializes new table
    m_currentTable = new Patient*[newCap];
    for (int i = 0; i < m_currentCap; i++) {
      m_currentTable[i] = nullptr;
    }
    
    if (m_oldTable != nullptr) {
      rehash();    
    }
  }
  
  return true;
}


bool VacDB::remove(Patient patient){
  unsigned int index = m_hash(patient.getKey()) % m_currentCap;
  unsigned int increment = 1;
  
  //searches for the patient in the current table
  while (m_currentTable[index] != nullptr) {
    if (getPatient(patient.getKey(), patient.getSerial()) == patient) {
      //marks it as deleted
      m_currentTable[index]->setUsed(true);
      m_currNumDeleted++;
      
      //rehash if no. deleted buckets is more than 80% of total number of occupied buckets
      if (deletedRatio() > 0.8) {
        //intializes old table
        m_oldCap = m_currentCap;
        m_oldSize = m_currentSize;
        m_oldTable = m_currentTable;
        
          
        //finds capacity of new table, which is the next smallest prime
        int num = (m_currentSize - m_currNumDeleted) * 4;
        int newCap = findNextPrime(num);
        
        //initializes new table
        m_currentTable = new Patient*[newCap];
        for (int i = 0; i < m_currentCap; i++) {
          m_currentTable[i] = nullptr;
        }  
        
        if (m_oldTable != nullptr) {
          rehash();    
        }
       }    
      
      return true;
    }

    //collision handling based on probing policy    
    if (m_currProbing == QUADRATIC) {
      index = (index + increment * increment) % m_currentCap;
    } else if (m_currProbing == LINEAR) {
      index = (index + increment) % m_currentCap;
    } else if (m_currProbing == DOUBLEHASH) {
      index = ((index % m_currentCap) + increment * (11 - (index % 11))) % m_currentCap;
    }
    
    increment++;
  }
  
  //searches for the patient in the old table
  if (m_oldTable != nullptr) {
    index = m_hash(patient.getKey()) % m_oldCap;
    increment = 1;
    
    while (m_oldTable[index] != nullptr) {
      if (getPatient(patient.getKey(), patient.getSerial()) == patient) {
      //marks it as deleted
      m_oldTable[index]->setUsed(true);
      m_oldNumDeleted++;
      
      //rehash if no. deleted buckets is more than 80% of total number of occupied buckets
      if (deletedRatio() > 0.8) {
        //intializes old table
        m_oldCap = m_currentCap;
        m_oldSize = m_currentSize;
        m_oldTable = m_currentTable;
        
          
        //finds capacity of new table, which is the next smallest prime
        int num = (m_currentSize - m_currNumDeleted) * 4;
        int newCap = findNextPrime(num);
        
        //initializes new table
        m_currentTable = new Patient*[newCap];
        for (int i = 0; i < m_currentCap; i++) {
          m_currentTable[i] = nullptr;
        }
        
        if (m_oldTable != nullptr) {
          rehash();    
        }
       }
      
        return true;
     }
      
      //collision handling based on probing policy    
      if (m_currProbing == QUADRATIC) {
        index = (index + increment * increment) % m_currentCap;
      } else if (m_currProbing == LINEAR) {
        index = (index + increment) % m_currentCap;
      } else if (m_currProbing == DOUBLEHASH) {
        index = ((index % m_currentCap) + increment * (11 - (index % 11))) % m_currentCap;
      }
      
      increment++;
    }
  }

  //object is not found, return false
  return false;
}


void VacDB::rehash() {
  //keeps track of 25% of table
  int stepSize = m_currentCap / 4;
  
  //iterates over the old table
  for (int i = 0; i < m_oldCap; i++) {
    if (m_oldTable[i] != nullptr && !m_oldTable[i]->getUsed()) {
        unsigned int index = m_hash(m_oldTable[i]->getKey()) % m_currentCap;
        unsigned int increment = 1;
        
        while (m_currentTable[index] != nullptr) {
          //collision handling based on probing policy    
          if (m_currProbing == QUADRATIC) {
            index = (index + increment * increment) % m_currentCap;
          } else if (m_currProbing == LINEAR) {
            index = (index + increment) % m_currentCap;
          } else if (m_currProbing == DOUBLEHASH) {
            index = ((index % m_currentCap) + increment * (11 - (index % 11))) % m_currentCap;
          }
          
          increment++;
        }
        
        //transfers live data to the new table
        m_currentTable[index] = m_oldTable[i];
        m_currentSize++;
  
        //marks the old entry as transferred
        m_transferIndex++;
    }
  
    //checks if current step of rehashing is completed
    if (m_transferIndex >= stepSize) {
      //deallocate memory for the old table if rehashing is completed
      if (i == m_oldCap - 1) {
        for (int j = 0; j < m_oldCap; j++) {
          delete m_oldTable[j];        
        }
        
        delete [] m_oldTable;
        
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
      }
      
      return;
    }
  }
}


const Patient VacDB::getPatient(string name, int serial) const {
  unsigned int index = m_hash(name) % m_currentCap;
  unsigned int increment = 1;
  
  //collision handling based on probing policy
  while (m_currentTable[index] != nullptr) {
    if (m_currentTable[index]->getKey() == name && m_currentTable[index]->getSerial() == serial) {
      return *m_currentTable[index];
    }
  
    if (m_currProbing == QUADRATIC) {
      index = (index + increment * increment) % m_currentCap;
    } else if (m_currProbing == LINEAR) {
      index = (index + increment) % m_currentCap;
    } else if (m_currProbing == DOUBLEHASH) {
      index = ((index % m_currentCap) + increment * (11 - (index % 11))) % m_currentCap;
    }
    
    increment++;
  }
  
  return Patient();
}


bool VacDB::updateSerialNumber(Patient patient, int serial){
  //loops through the current table
  for (int i = 0; i < m_currentCap; i++) {
    //checks if the current patient matches the given patient
    if (m_currentTable[i]->getKey() == patient.m_name) {
      m_currentTable[i]->setSerial(serial);
      return true;
    }
  }
  
  return false;
}


float VacDB::lambda() const {
  return (float)m_currentSize / (float)m_currentCap;
}


float VacDB::deletedRatio() const {
  return (float)m_currNumDeleted / (float)m_currentSize;
}


void VacDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool VacDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int VacDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME - 1;
    for (int i = current; i < MAXPRIME; i++) { 
        for (int j = 2; j*j <=i; j++) {
            if (i % j == 0) 
                break;
            else if (j + 1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Patient* patient ) {
    if ((patient != nullptr) && !(patient->getKey().empty()))
        sout << patient->getKey() << " (" << patient->getSerial() << ", "<< patient->getUsed() <<  ")";
    else
        sout << "";
  return sout;
}

bool operator==(const Patient& lhs, const Patient& rhs){
    // since the uniqueness of an object is defined by name and serial number
    // the equality operator considers only those two criteria
    return ((lhs.getKey() == rhs.getKey()) && (lhs.getSerial() == rhs.getSerial()));
}

bool Patient::operator==(const Patient* & rhs){
    // since the uniqueness of an object is defined by name and serial number
    // the equality operator considers only those two criteria
    return ((getKey() == rhs->getKey()) && (getSerial() == rhs->getSerial()));
}