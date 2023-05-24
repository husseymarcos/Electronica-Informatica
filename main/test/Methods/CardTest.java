package Methods;
import org.testng.Assert;
import org.testng.annotations.Test;

public class CardTest {

    @Test
    public void testVerifyUidCode() {
    }

    @Test
    public void testVerifyPassword() {
    }

    @Test
    public void testGetBalance() {
        Card c1 = new Card(12345, 3333);
        Assert.assertEquals(c1.getBalance(), 0);
    }

    @Test
    public void testAddAmount() {
    }

    @Test
    public void testPlay() {
    }

    @Test
    public void testTransferAmount() {
    }
}