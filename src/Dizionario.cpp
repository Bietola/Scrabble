#include "Dizionario.h"

Dizionario::Dizionario():
    mRadice(nullptr)
{
    ;
}
Dizionario::Dizionario(const string &s){
    ifstream f(s.c_str());
    string parola;
    mRadice = new Nodo;
    mRadice->parolaFinita = false;
    mRadice->ePrimo = true;
    while(f>>parola){
        Nodo* iter=mRadice;
        for(auto c : parola){
            Nodo*& nuovaLettera = iter->lettere[c - 'a'];
            if(nuovaLettera==nullptr){
                nuovaLettera=new Nodo;
            }
            else if(nuovaLettera!=nullptr){
                //non fare niente
            }
            iter=nuovaLettera;
        }
        iter->parolaFinita=true;
    }
}

void  Dizionario::stampa(Dizionario::Nodo* nodo, const string& parola)const{
    for(unsigned int i=0; i<26; ++i){
        string nuovaParola = parola;
        if(nodo->lettere[i]==nullptr){
            //non faccio ninte
        }
        else {
            nuovaParola += char(i+'a');
            if(nodo->lettere[i]->parolaFinita) {
               cout << nuovaParola << endl;
            }
            stampa(nodo->lettere[i], nuovaParola);
        }
    }
}
void Dizionario::stampa() const {
    stampa(mRadice, "");
}

/*
void  Dizionario::stampa(string &parola)const{
    for(unsigned int i=0; i<26; ++i){
        if(iter->lettere[i-'a']==nullptr){
            //non faccio ninte
        }
        else if(iter->lettere[i-'a']!=nullptr && iter->parolaFinita){
           parola=parola+(i+'a');
           cout<<parola;

        }
        else if(iter->lettere[i-'a']!=nullptr && !iter->parolaFinita){
            parola=parola+(i+'a');
        }
        stampa(parola);
    }
}*/
