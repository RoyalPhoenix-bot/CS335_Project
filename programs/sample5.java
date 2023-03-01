package programs;

import java.io.*;

public class sample5 {
    public static void main(String[] args)
    {
        int i = 20;

        if (i == 10)
            System.out.println("i is 10\n");
        else if (i == 15)
            System.out.println("i is 15\n");
        else if (i == 20)
            System.out.println("i is 20\n");
        else
            System.out.println("i is not present\n");
 
        System.out.println("Outside if-else-if");
    }
}

class LoopDemo {
    public static void main(String args[]){
        int j = 1;
        while (j < 6) {
            System.out.println("Hello");
            j++;
        }

        for (int i = 1; i <= 10; i++) {
            System.out.println(i);
        }
    }
}