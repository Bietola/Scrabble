#include "Dictionary.h"

//constructors
Dictionary::Dictionary():
    mRoot(nullptr)
{
    ;
}
Dictionary::Dictionary(const std::string& file){
    //open file
    std::ifstream f(file.c_str());
    std::string word;
    //create root
    mRoot = new Node();
    mRoot->isFirst = true;
    //iterate through file
    while(f >> word){
        Node* itr = mRoot;
        for(auto c : word){
            Node*& link = itr->links[c - 'a'];
            if(link == nullptr){
                link = new Node;
            }
            itr = link;
        }
        itr->endOfWord = true;
    }
}
//return true if word is in dictionary
bool Dictionary::searchWord(const std::string& word, Dictionary::Node* node, size_t index) const {
    //word is not in dictionary
    if(!node)
        return false;
    //end of word reached
    if(index == word.size()) {
        //word was found
        if(node->endOfWord)
            return true;
        //word not found
        else
            return false;
    }
    //keep going deeper
    return searchWord(word, node->links[std::tolower(word[index]) - 'a'], index + 1);
}
bool Dictionary::searchWord(const std::string& word) const {
    return searchWord(word, mRoot, 0);
}
//print dictionary
void Dictionary::print(Dictionary::Node* Node, const std::string& word) const {
    for(size_t j = 0; j < NUMBER_OF_LETTERS; j++){
        std::string newWord = word;
        if(Node->links[j] != nullptr) {
            newWord += char(j + 'a');
            if(Node->links[j]->endOfWord) {
               std::cout << newWord << std::endl;
            }
            print(Node->links[j], newWord);
        }
    }
}
void Dictionary::print() const {
    print(mRoot, "");
}
