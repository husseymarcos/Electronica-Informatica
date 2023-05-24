#include <string>

class Game{
public:
  int cost;
  std:: string name;

  Game(std:: string name, int cost){
    this->name = name;
    this->cost = cost;
  }
}