#include "Game.h"
#include <iostream>
#include <string>

class Card{
  public:
    int balance;
    Game[] arrayOfGames = {Game("Temple run", 100),
                           Game("Asphalt", 200),
                           Game("Whack-a-mole", 300),
                           Game("Ninja Fruit", 400)};
  private:
    int uid;
    int password;
    int code;

  public:
    Card(int code, std::string owner): code(code), balance(0), uid(uid), password(password) {}

    bool verifyUidCode(){return uid == code; }

    bool verifyPassword(int passwordAttempt){ return passwordAttempt == password;}

    int ConsultCredit() {return balance;}

    std::string playGame(int gameIndex){
      if(arrayOfGames[gameIndex].cost > balance){
          Serial.println("Saldo insuficiente");
      }
      else{
        Serial.println("A continuacion: " + arrayOfGames[gameIndex].name);
      }
    }

    void CreditTransfer(int creditToTransfer, Card userToTransfer, std::string senderName, std::string receiverName){
      if(this->balance > creditToTransfer){
        this->balance = this->balance - creditTotransfer;
        userToTransfer.balance = userToTransfer.balance + creditToTransfer;
        Serial.println(senderName + " realizó una transferencia de " + creditToTransfer " créditos a " + receiverName);
      }
      else{
        Serial.println("Saldo Insuficiente.");
      }
    }

    void CreditLoad(int CreditToLoad, std::string userToLoad){
      this->balance = this->balance + CreditToLoad;
      Serial.println("Se realizó una carga de: " + CreditToLoad + " créditos a " + userToLoad);
    }
};