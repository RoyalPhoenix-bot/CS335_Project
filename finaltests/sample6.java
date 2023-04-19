// Recurring function calls
public class sample6 {
    
    public static int sub(int l, int m) {
        return l - m;
    }

    public static int add(int a, int b) {
        int x=5,y=4;
        int z = sub(x,y);
        return a + b;
    }

    public static void main() {
       int p = 1;
       int q = 2;
       int c = add(p,q);
       return;
    }
}
