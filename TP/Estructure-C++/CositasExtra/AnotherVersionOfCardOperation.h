#include <iostream>
#include <string>
#include "Game.h"
#include <map>

/*En esta otra version de CardOperation, estuve viendo como podemos tratar un mapa "dict" directamente
para poder realizar las operaciones de los metodos.*/

using namespace std; // Evita que tengamos que poner std::

class User{
  public:
    String name;
};

class Operations: public User{ // Considero que esta clase debería ser abstracta
public:
    virtual bool verifyUidCode() {}
    virtual bool verifyPassword(int passwordAttempt) {}
    virtual int getBalance() {}
    virtual void addAmount(int amount) {}
    virtual void transferAmount(int amount) {}
    virtual void play(int index) {}
    virtual void gamble(int amount) {}
};



class Card : public Operations { // Esta clase hereda de la de Operaciones y necesariamente debe realizar las operaciones. Cada uno de los métodos.
  private:
      int code;     // Clase que representa las tarjetas con lector RFID
      int uid;      // HEXA -> 'uid' de la tarjeta
      int password; // Personal Password
  public:
      int balance;  // Saldo

  map<String, int> dictOfGames = {"Temple run": 50; "Asphalt": 75; "Whack-a-mole": 45; "Ninja Fruit": 30;};

  User arrayOfUsers[10] = {"Juan", "Marcos", "Lucas", "Carlos", "Bruno", "Luisa", "Elsa", "Javier", "Julieta", "Leandro"};

// Constructor
  public:
      Card(int uid, int password) {
          this->balance = 0;
          this->uid = uid;
          this->password = password;
      }
      bool verifyUidCode() override { return uid == this->code; }
      bool verifyPassword(int passwordAttempt) override { return passwordAttempt == password; }
      int getBalance() override { return balance; }
      void addAmount(int amount) override { balance += amount; }

      void play(int gameIndex) {
      if (dictOfGames[gameIndex].key > balance) {
           Serial.println("Saldo insuficiente.");
      }
      else {cout << "A continuación: " << dictOfGames[gameIndex].value << endl;}
      }

      void transferAmount(int amount) {
        if (amount <= balance) {
            balance += amount;
            cout << "Transferencia exitosa." << endl;
        }
      else {cout << "Saldo Insuficiente." << endl;}
      }

      void gamble(int amount) {} // Falta definir el juego
};