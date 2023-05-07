#include <iostream>
#include <String>
#include "Game.h"

using namespace std;
/*Acá realice una combinacion entre las clases Operations y Card, faltan algunas cosas por revisar
diria que es necesario analizar a fondo el codigo*/

class Operations {
public:
    virtual bool verifyUidCode() { return false; }
    virtual bool verifyPassword(int passwordAttempt) { return false; }
    virtual int getBalance() { return 2; }
    virtual void addAmount(int amount) {}
    virtual void transferAmount(Card* otherPlayer, int amount) {}
    virtual void play(int index) {}
    virtual void gamble(int amount) {}
};



class Card : public Operations { // Esta clase hereda de la de Operaciones y necesariamente debe realizar las operaciones. Cada uno de los metodos.
  private:
      int code;     // Clase que representa las tarjetas con lector RFID
      int uid;      // HEXA -> 'uid' de la tarjeta
      int password; // Personal Password
  public:
      int balance;  // Saldo
  Game arrayOfGames[4] = { Game("Temple run", 50),
                             Game("Asphalt", 75),
                             Game("Whack-a-mole", 45),
                             Game("Ninja Fruit", 30) };
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
      if (arrayOfGames[gameIndex].cost > balance) {
           Serial.println("Saldo insuficiente.");
      }
      else {cout << "A continuacion: " << arrayOfGames[gameIndex].name << endl;}
      }

      void transferAmount(Card* otherPlayer, int amount) {
        if (amount <= balance) {
            otherPlayer->balance += amount;
            cout << "Transferencia exitosa." << endl;
        }
      else {cout << "Saldo Insuficiente." << endl;}
      }

      void gamble(int amount) {} // Falta definir el juego
};
