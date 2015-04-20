#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
#include <vector>
#include <cmath> // abs()
#include <string> // c_str(), length(), substr()

/*
 * HashTable implementation when key = string
 */

template <class T>
class HashTable
{
    // Linked list struct to contain key value pairs
    struct Elem
    {
        std::string key;
        T value;
        Elem *next;
        Elem *prev;
        
        // Constructor
        Elem(std::string key_, T value_) : key(key_), value(value_), next(NULL), prev(NULL){}
    };

    private:
        // Array of pointers to hold elements
        Elem **table;

        // Table Size
        long table_size;

    protected:
    public:
        HashTable(long size);
        ~HashTable();

        // Inserts a key value pair TODO: make sure duplicate keys cannot be inserted
        void insert(std::string key, T value);

        // Deletes a key value pair from the table
        void delete_kv(std::string key, T value);

        // Finds first element that matches key, returns pointer to value
        T* get_value(std::string key);

        // Returns vector of pointers to values currently in table
        std::vector<T*> GetAllValues();

        // Prints contents of table and corresponding indicies
        void print();
        
        // Returns table size
        inline long get_table_size(){return table_size;}

        // Hash Function, 
        long get_index(std::string key);
       
};

template <class T>
HashTable<T>::HashTable(long size) : table_size(size)
{
    table = new Elem*[table_size];
    for (int i = 0; i < table_size; ++i) {
        table[i] = NULL;
    }
}

template <class T>
HashTable<T>::~HashTable()
{
    Elem *toDelete, *next;
    for (int i = 0; i < table_size; ++i) {
        toDelete = table[i];
        while (toDelete != NULL) {
            next = toDelete->next;
            delete toDelete;
            toDelete = next;
        }
    }
    delete [] table;
}

template <class T>
long HashTable<T>::get_index(std::string key)
{
    int intLength = key.length() / 4;
    long sum = 0;

    for (int j = 0; j < intLength; j++) {
        std::string c = key.substr(j * 4, (j * 4) + 4).c_str();
        long mult = 1;
        for (int k = 0; k < c.length(); k++) {
            sum += c[k] * mult;
            mult *= 256;
        }
    }

    std::string c = key.substr(intLength * 4).c_str();
    long mult = 1;
    for (int k = 0; k < c.length(); k++) {
        sum += c[k] * mult;
        mult *= 256;
    }

    return (std::abs(sum) % table_size);
}

template <class T>
void HashTable<T>::insert(std::string key, T value)
{
    long index = get_index(key);
    Elem *iter = table[index];
    Elem *prev = NULL;

    if (iter == NULL) {
        table[index] = new Elem(key,value);
    } else {
        while (iter->next != NULL) {
            iter = iter->next;
        }
        iter->next = new Elem(key,value);
    }
}

template <class T>
void HashTable<T>::delete_kv(std::string key, T value)
{
    long index = get_index(key);
    Elem *toDelete = table[index];

    if (toDelete == NULL) {
        std::cerr << "Error, key value pair not found";
        return;
    } else if (toDelete->key == key && toDelete->value == value) {
        table[index] = table[index]->next;
        delete toDelete;
    } else {
        while (toDelete->next != NULL && (toDelete->key != key || toDelete->value != value)) {
            toDelete = toDelete->next;
        }
        if (toDelete->key != key || toDelete->value != value) {
            std::cerr<<"Error, key value pair not found";
            return;
        }
        if (toDelete->prev != NULL)
            toDelete->prev->next = toDelete->next;
        if (toDelete->next != NULL)
            toDelete->next->prev = toDelete->prev;
        delete toDelete;
    }
}

template <class T>
void HashTable<T>::print()
{
    for (int i = 0; i < table_size; ++i) {
        if (table[i] != NULL) {
            Elem * iter = table[i];
            while (iter != NULL) {
                std::cout << i << ": " << iter->key << ' ' << iter->value << std::endl;
                iter = iter->next;
            }
        }
    }
}

template <class T>
T* HashTable<T>::get_value(std::string key)
{
    long index = get_index(key);
    Elem *iter = table[index];

    // Element is Null, return "not found"
    if (iter == NULL) {
        return NULL;
    // Element found
    } else if (iter->key == key) {
        return &iter->value;
    // Else if collision element, try to find elem
    } else {
        while (iter != NULL && iter->key != key) {
            iter = iter->next;
        }
        // Still not found
        if (iter == NULL)
            return NULL;
        // Found at some point in linked list
        else
            return &iter->value;
    }
    
}

template <class T>
std::vector<T*> HashTable<T>::GetAllValues()
{
    std::vector<T*> values;
    for (int i = 0; i < table_size; ++i)
    {
        if (table[i] != NULL) {
            values.push_back(&(table[i]->value));
        }
    }
    return values;
}


#endif//HASHTABLE_H
