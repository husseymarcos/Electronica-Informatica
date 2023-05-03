package Methods;

/*  METODOS A IMPLEMENTAR
 * cargar saldo
 * hacer pago -> Distintos juegos.
 * consultar saldo
 * "ruleta"
 * transferir ('regalar')
 * verificar uid
 * verificar contraseña numerica.
 * volver / siguiente
 */

public interface Operations {

    public default boolean verifyUidCode(){ return false; }

    public default boolean verifyPassword(int passwordAttempt) { return false; }

    public default int getBalance(){ return 2;}

    public default void addAmount(int amount) {;}

    public default void transferAmount(Card otherPlayer, int amount){}

    public default void play(int index){}

    public default void gamble(int amount){}
}
