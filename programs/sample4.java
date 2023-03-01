package programs;

import java.util.*;
 
// Class 1
class A {
 
    // Main driver method
    public static void main(String args[]){
 
        int a = 10, b = 4, res;
        System.out.println("a is " + a + " and b is " + b);
        
        a++;
        res = a + b;
        System.out.println("a+b is " + res);
        
        a-=2;
        res = a - b;
        System.out.println("a-b is " + res);
        
        b--;
        a*=3;
        res = a * b;
        System.out.println("a*b is " + res);
        
        ++a;
        b+=7;
        res = a / b;
        System.out.println("a/b is " + res);
        
        --b;
        b/=2;
        res = a % b; 
        System.out.println("a%b is " + res);


        int n1 = 5, n2 = 10, mx;
        System.out.println("First number: " + n1);
        System.out.println("Second number: " + n2);
        mx = (n1 > n2) ? n1 : n2;
        System.out.println("Maximum value is = " + mx);


        int y = 5;
        int z = 7;
        System.out.println("y&z = " + (y & z));
        System.out.println("y|z = " + (y | z));
        System.out.println("y^z = " + (y ^ z));
        System.out.println("~y = " + ~y);
        y &= z;
        System.out.println("y= " + y);


        int e = 10, f = 20, g = 20, h = 0;
        System.out.println("e = " + f);
        System.out.println("f = " + g);
        System.out.println("g = " + h);
        if ((e < f) && (f == g)) {
            h = e + f + g;
            System.out.println("The sum is: " + h);
        }
        else
            System.out.println("False conditions");
    }
}
