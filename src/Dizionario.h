#ifndef DIZIONARIO_H
#define DIZIONARIO_H

#include<array>
#include<string>
#include<fstream>
#include<iostream>

using namespace std;

class Dizionario{
public:
    struct Nodo{
        Nodo() {
            for(auto& ele : lettere)
                ele = nullptr;
        }
        array<Nodo*,26> lettere;
        bool parolaFinita = false;
        bool ePrimo = false;
    };

    Dizionario();
    Dizionario(const string &file);

    const Nodo* getRadice() const {return mRadice;}

    void stampa() const;

private:
    Nodo* mRadice=nullptr;

    void stampa(Nodo* nodo, const string& parola) const;
};

#endif // DIZIONARIO_H
