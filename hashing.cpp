/*
    Rodrigo Andujar Lugo
    CSCI36200 
    This program reads a dictionary text file and creates a hashmap using a LHS algorithm to group
        words by the first two characters.
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "TimeInterval.h"

// Hashing function - takes in a string to hash and a number to use modulo
// Like "Locality-Sensitive Hashing" or LSH, this hash function maximizes collisions to create clusters
// This function uses the first two letters to create a hash
int hash(std::string str, int modulo)
{
    std::string test;
    int sum;
    for (int i = 0; i < 2; i++)
    {
        test += std::to_string((int)(str[i]));
    }
    sum = std::stoi(test);
    return sum % modulo;
}

//This is the data structure used to store the key/value pairs
class Pair
{
private:
    int key;
    std::string value;

public:
    Pair()
    {
        this->key = -1;
        this->value = "";
    }
    Pair(int key, std::string val)
    {
        this->key = key;
        this->value = val;
    }

    int get_key()
    {
        return this->key;
    }
    std::string get_value()
    {
        return this->value;
    }

    void update_value(std::string new_val)
    {
        this->value = new_val;
    }
};

//This is the datastructure used to store the items in the buckets - used for collisions
class ItemList
{
private:
    Pair *current;
    ItemList *next;
    //inserts item into the bucket
    void insert(Pair *new_item, ItemList *next)
    {
        if (next->current == NULL)
        {
            this->current = new_item;
        }
        else if (next->next == NULL)
        {
            ItemList *next_ = new ItemList();
            next_->current = new_item;
            next->next = next_;
        }
        else
        {
            next->insert(new_item, next->next);
        }
    }

public:
    ItemList()
    {
        this->current = NULL;
        this->next = NULL;
    }
    ItemList(Pair *current, ItemList *next)
    {
        this->current = current;
        this->next = next;
    }
    //used to insert pair into bucket
    void insert(Pair *new_item)
    {
        this->insert(new_item, this);
    }

    Pair *get_item()
    {
        return this->current;
    }
    ItemList *get_next()
    {
        return this->next;
    }
};

//This is the datastructure for the hashmap
class HashMap
{
private:
    Pair **items;
    ItemList **buckets;
    int size;
    int count;

public:
    HashMap()
    {
        this->size = 5831;
        this->count = 0;
        this->items = new Pair *[this->size];
        for (int i = 0; i < this->size; i++)
        {
            this->items[i] = NULL;
        }
        this->buckets = new ItemList *[this->size];
        for (int i = 0; i < this->size; i++)
        {
            buckets[i] = NULL;
        }
    }
    //handles collisions by placing the extra strings in a bucket
    void collisions(HashMap *table, int index, Pair *item)
    {
        ItemList *head = table->buckets[index];
        if (head == NULL)
        {
            head = new ItemList();
            head->insert(item);
            table->buckets[index] = head;
        }
        else
        {
            head->insert(item);
            table->buckets[index] = head;
        }
    }
    //inserts a pair into the hashmap
    void insert(int key, std::string value)
    {

        int index = hash(value, this->size);
        Pair *new_item = new Pair(key, value);
        Pair *c_item = this->items[index];

        if (c_item == NULL)
        {
            if (this->count == this->size)
            {
                std::cout << "Cannot add... table full...";
                return;
            }
            this->items[index] = new_item;
            this->count++;
        }
        else
        {
            if (c_item->get_key() == key)
            {
                this->items[index]->update_value(value);
                return;
            }
            else
            {
                //handle collision
                collisions(this, index, new_item);
                return;
            }
        }
    }
    //is used to return true or false to see if the word exists
    bool search(std::string value)
    {
        int index = hash(value, this->size);
        Pair *item = this->items[index];
        ItemList *head = this->buckets[index];

        if (item == NULL)
            return false;
        else
        {
            if (!value.compare(item->get_value()))
                return true;

            while (head)
            {
                if (!value.compare(head->get_item()->get_value()))
                    return true;
                head = head->get_next();
            }
        }
        return false;
    }
    //This functions prints all of the items in the hashmap
    void print_table()
    {
        std::cout << "Table begin \n------\n";
        ItemList *temp;
        for (int i = 0; i < this->size; i++)
        {

            if (this->items[i])
            {
                std::cout << "key: " << this->items[i]->get_key() << ", Value: " << this->items[i]->get_value() << "\n";
                if (this->buckets[i] != NULL)
                {
                    temp = buckets[i];
                    std::cout << "key: " << temp->get_item()->get_key();
                    while (temp)
                    {
                        std::cout << ", Value: " << temp->get_item()->get_value();
                        temp = temp->get_next();
                    }
                    std::cout << "\n";
                }
            }
        }
        std::cout << "------\nTable end \n";
        std::cout << "------\nTable size \n";
        std::cout << this->count << "\n\n";
    }

    //this function is used to find all the strings that start with the same 2 letters
    std::vector<std::string> *find_similar(std::string value)
    {
        std::vector<std::string> *list = new std::vector<std::string>();
        int index = hash(value, this->size);

        Pair *item = this->items[index];
        ItemList *head = this->buckets[index];

        if (value.size() < 2)
        {
            list->push_back("String is less than 2 char");
            return list;
        }

        if (item != NULL)
        {
            list->push_back(item->get_value());
            while (head)
            {
                list->push_back(head->get_item()->get_value());
                head = head->get_next();
            }
            return list;
        }
        return list;
    }
};

// This function is used to initialize the hashmap given a input file
HashMap read_file(std::string file)
{
    std::string line;
    int count = 0;
    HashMap map;
    if (file.compare("") == 0)
        file = "Dictionary.txt";
    std::ifstream f;
    f.open(file);
    if (f.is_open())
    {
        while (getline(f, line))
        {
            map.insert(count, line);
            count++;
        }
        f.close();
    }
    else
    {
        std::cout << "could not open dictionary\n";
    }
    return map;
}

// This is the function that uses the map to ask the user for a word to fund
void ask_stuff(HashMap map)
{
    TimeInterval timer;
    std::string input;
    std::vector<std::string> *list = new std::vector<std::string>();
    std::cout << "Enter a word to search in the dictionary : ";
    std::cin >> input;
    timer.start();

    if (map.search(input))
    {
        std::cout << "\nTrue \n";

        list = map.find_similar(input);

        if (list->size() > 0)
            for (int i = 0; i < list->size(); i++)
                std::cout << list->at(i) << "\n";
    }
    else
    {
        std::cout << "False \n";
    }
    timer.stop();
    std::cout << timer.GetInterval() << " micro-sec\n";
}
//driver function
int main()
{
    HashMap test = read_file("");
    ask_stuff(test);

    //test.print_table();
}