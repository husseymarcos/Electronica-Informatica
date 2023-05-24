package Methods;

import java.sql.Array;


// Clase que representa las tarjetas con lector RFID
public class Card implements Operations {
    // Código que determina qué tarjetas son válidas y no  .
    private int code;
    private int uid; //HEXA -> 'uid' de la tarjeta
    public int balance; // Saldo
    private int password; // Contrasena personal.
    Game[] arrayOfGames = { new Game("Temple run", 50),
                            new Game("Asphalt", 75),
                            new Game("Whack-a-mole", 45),
                            new Game("Ninja Fruit", 30) };

    //Constructor
    public Card( int uid, int password) {
        this.balance = 0;
        this.uid = uid; // Se lee directamente de la tarjeta. Se contrasta con 'code'.
        this.password = password;
    }

    // Verifica si la tarjeta es valida. Se llama al apoyar la tarjeta contra el lector RFID.
    @Override
    public boolean verifyUidCode() { return uid == this.code; }

    @Override
    public boolean verifyPassword(int passwordAttempt) { return passwordAttempt == password; }

    // Consultar saldo
    @Override
    public int getBalance()  { return balance; }

    // Cargar saldo
    @Override
    public void addAmount(int amount ) { balance += amount; }

    // Jugar un juego. Se tiene un Array de objetos 'Game'. Se compara el indice ingresado con el indice en el Array.
    // En la pantalla LED apareceria, en el mismo orden que en el Array:
    // [1] - XXXXXXXXX
    // [2] - YYYYYYYYY
    @Override
    public void play(int gameIndex){
        if (arrayOfGames[gameIndex].cost > balance){
            System.out.println("Saldo insuficiente.");
        } else {
            System.out.println("A continuación: " + arrayOfGames[gameIndex].name);
        }
    }

    // Regalar saldo.
    @Override
    public void transferAmount(Card otherPlayer, int amount){
        if (amount <= balance){
            otherPlayer.balance += amount;
            System.out.println("Transferencia exitosa.");
        } else {
            System.out.println("Saldo Insuficiente.");
        }
    }

    @Override
    // "Ruleta". Falta definir en qué consistirá el juego.
    public void gamble(int amount){

    }

}
