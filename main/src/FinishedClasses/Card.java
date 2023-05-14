package FinishedClasses;

public class Card{
    private final int code;
    public int balance;
    private int uid;
    private int password;
    Game[] arrayOfGames = { new Game("Temple run", 100),
            new Game("Asphalt", 200),
            new Game("Whack-a-mole", 300),
            new Game("Ninja Fruit", 400) };
    /* Considero que ese arreglo de Juegos, de alguna forma debería estar relacionado a
    dependiendo el nro que toques en el Keypad, te cobra por determinado juego.
     */
    public Card(int code, String owner){ // Acá le pasamos el código que tiene la tarjeta, este valor no puede modificarse. Ver!
        this.code = code; // Considero que esto ya lo tiene predeterminado. Pero el code es único para cada tarjeta. Ver como funciona el uid!
        this.balance = 0;
        this.uid = uid;
        this.password = password; // Tengo que ver como podría determinar la contrasena
    }


    public boolean verifyUidCode() { return this.uid == this.code; } // Ver como podría testear este.


    public boolean verifyPassword(int passwordAttempt) { return passwordAttempt == password; } // Ver como podría testear este.

    public int ConsultCredit() {return this.balance;}

    public String playGame(int gameIndex){ // Este nro igual corresponde a los nros del teclado
        if(arrayOfGames[gameIndex].cost > this.balance){
            return "Saldo insuficiente.";
        }
        else{
            return "A continuación: " + arrayOfGames[gameIndex].name;
        }
    }

    public void CreditTransfer(int creditToTransfer, Card userToTransfer, String senderName, String receiverName) { // Refinar codigo
        if(this.balance > creditToTransfer){ // Me coparia que pudiera mostrar entre quienes fue esa transacción.
            this.balance = this.balance - creditToTransfer;
            userToTransfer.balance = userToTransfer.balance + creditToTransfer;
            System.out.println(senderName + " realizó una transferencia de " + creditToTransfer + " créditos a " + receiverName); // La conversión de int de creditToTransfer, lo realiza java por su cuenta.
        }
        else{
            System.out.println("Saldo Insuficiente.");
        }
    }
// Evaluar un método CreditTransfer, que devuelva los 2 usuarios que interactuan y me diga cuanto dinero se paso y esas cosas.


    public void CreditLoad(int CreditToLoad, String userToLoad){
        this.balance = this.balance + CreditToLoad;
        System.out.println("Se realizó una carga de: " + CreditToLoad + " créditos a " + userToLoad);
    }
}
