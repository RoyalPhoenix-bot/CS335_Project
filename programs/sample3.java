package programs;

public class sample3 {
    public static void main(String[] args) {
        int intVar = 100;
        long longVar = intVar;
        float floatVar = longVar;
        double doubleVar = 100.42;
        boolean booleanVar = true;

        double dVar = 3.141596;
        long lVar = (long) dVar;
        int iVar = (int) dVar;

        System.out.println("Dummy print: " + (short) 3.14F);

        double exp = intVar + longVar + dVar * floatVar + lVar + doubleVar + iVar;
        System.out.println("Exp: " + exp);
        System.out.println("Boolean: " + booleanVar);
    }
}
