#define CARD_H
#define CARD_H

Card::Card(int userID, String userName, int balance, int password) {
  this->userID = userID;
  this->userName = userName;
  this->balance = balance;
  this->password = password;
}

void Card::creditTransfer(int amount, Card& recipientCard) {
  if (balance >= amount) {
    balance -= amount;
    recipientCard.balance += amount;
    Serial.println("Credits transferred successfully.");
  } else {
    Serial.println("Insufficient balance for transfer.");
  }
}

void Card::creditLoad(int amount) {
  balance += amount;
  Serial.println("Credits loaded successfully.");
}

void Card::consultCredit() {
  Serial.print("User: ");
  Serial.println(userName);
  Serial.print("Balance: ");
  Serial.println(balance);
}
