#include "Game.h"
#include <iostream>
#include <string>
#include <SD.h>

const char* fileName = "usuarios.txt";

const char* logName = "registros.log";

const int chipSelect = 5; // Asigno a que pin deberia estar el cs
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

    int ConsultCredit() {Serial.print("Su saldo actual es " + balance};

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
        info = senderName + " realizó una transferencia de " + creditToTransfer + " créditos a " + receiverName;
        Serial.println(info);
        addOperation(info);
      }
      else{
        Serial.println("Saldo Insuficiente.");
      }
    }

    void CreditLoad(int CreditToLoad, std::string userToLoad){
      balance = balance + CreditToLoad;
      Serial.println("Se realizó una carga de: " + CreditToLoad + " créditos a " + userToLoad);
      info = "Se realizó una carga de: " + CreditToLoad + " créditos a " + userToLoad;
      addOperation(info);
    }

    private:
    void addUser(int code, String owner, int uid, int password){ // int userID, int credits. Revisar si es necesario poner std::string. Verlo, si falla.
          if(!SD.begin(chipSelect)){
            Serial.println("Error al iniciar la tarjeta SD");
            return;
          }

          File file = SD.open(fileName, FILE_WRITE);
          if(!file){
            Serial.println("Error al abrir el archivo");
            return;
          }

          file.print(code);
          file.print("|");
          file.println(owner);
          file.print("|");
          file.println(uid);
          file.print("|");
          file.println(password);

          file.close();

          Serial.println("Usuario agregado al archivo");
      }

    void addOperation(String info){ // Revisar si es necesario poner std::string. Verlo, si falla.
      if(!SD.begin(chipSelect)){
        Serial.println("Error al iniciar la tarjeta SD");
        return;
      }

      File file = SD.open(logName, FILE_WRITE); // Abre el archivo en modo "append" para agregar datos al final.

      if(!file){
        Serial.println("Error al abrir el archivo");
        return;
      }

      file.println(info);

      file.close();

      Serial.println("Registro de operacion, agregado al archivo.");
    }

    String findUserNameByID(int userID){          // userID == uid
      File file = SD.open(fileName, FILE_READ);
      String line;                                // Revisar si es necesario poner std::string. Verlo, si falla.
      while(file.available()){
        line = file.readStringUntil('\n');
        int id = line.substring(0, line.indexOf(",").toInt());
        if(id == userID){
          String userName = line.substring(line.indexOf(",") + 1, line.length()- 1);
          file.close();
          return userName;
        }
      }
      file.close();
      return "El usuario que quiere encontrar, no existe.";
      }

};

