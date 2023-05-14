package FinishedTestClasses;


import FinishedClasses.Card;
import org.junit.Assert;
import org.junit.Test;


public class CardTest {
    @Test
    public void testConsultCredit(){
        Card card = new Card(1234, "User");
        card.CreditLoad(100, "User");
        Assert.assertEquals(100, card.ConsultCredit());
    }

    @Test
    public void testCreditLoad(){
        Card card = new Card(1234,"User");
        card.CreditLoad(100, "User");
        Assert.assertEquals(100, card.ConsultCredit()); // Testeo si el balance es igual al valor de 100.
    }

    @Test
    public void testPlayGame() {
        Card card = new Card(1234, "User");
        card.CreditLoad(150, "User");

        // Llama a playGame y almacena la cadena devuelta en una variable
        String actualOutput = card.playGame(0); // {"Temple Run": 100, "Asphalt": 200, "Whack-a-mole": 300, "Ninja Fruit": 400} {0 , 1 , 2 , 3}

        // Compara la cadena devuelta con la salida capturada en outContent
        String expectedOutput = "A continuación: Temple run";
        Assert.assertEquals(expectedOutput, actualOutput);
    }


    @Test
    public void testCreditTransfer(){
        Card card1 = new Card(1234, "User 1");
        Card card2 = new Card(4321, "User 2");
        card1.CreditLoad(100, "User 1");
        card1.CreditTransfer(50, card2, "User 1", "User 2");
        Assert.assertEquals(50, card2.ConsultCredit());
    }
}
