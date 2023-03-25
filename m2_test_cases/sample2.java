package programs;

public class sample3 {
    public static void main(String[] args) {
        int intVar = 100;
        long longVar = intVar;
        float floatVar = longVar;
        double doubleVar = 100.42;

        double dVar = 3.141596;
        long lVar = (long) dVar;
        int iVar = (int) dVar;

        double exp = intVar + longVar + dVar * floatVar + lVar + doubleVar + iVar;
    }
}

public class B extends sample3{
    int add (int num1,int num2){
        int x = num1;
        int y = num2;
        int z = num1 + num2;
        return z;
    }

    public static void main(String[] args){
        int a,b;
        int c;
        c = add(a,b);
    }
}
