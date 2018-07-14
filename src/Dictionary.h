#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <array>

#include "constants.h"

///dictionary class
class Dictionary{
    public:
        //dictionary node
        struct Node {
            Node() {
                for(auto& ele : links) {
                    ele = nullptr;
                }
            }
            std::array<Node*, NUMBER_OF_LETTERS> links;
            bool endOfWord = false;
            bool isFirst = false;
        };
        //constructors
        Dictionary();
        Dictionary(const std::string& file);
        //move constructor
        Dictionary(Dictionary&& d);
        //getters
        const Node* getRoot() const {return mRoot;}
        //returns true if specified word is in dictionary
        bool searchWord(const std::string& word) const;
        //prints contents of dictionary to console
        void print() const;
    private:
        //root of dictionary tree
        Node* mRoot;
        //private recursive functions (for recursive parameter passing)
        bool searchWord(const std::string& word, Node* node, size_t index) const;
        void print(Node* node, const std::string& word) const;
};


#endif //  DICTIONARY_H_INCLUDED
