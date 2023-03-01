package programs;

class A{
    void method1(){}
    void method2(){}
}
    
  class B extends A{  
    void method3(){}
    void method4(){}
}

class Employee {
    private int empid;
    private String ename;
}

public class sample1 {
      
    static String Employee_name;
    static float Employee_salary;
  
    static void set(String n, float p) {
        Employee_name  = n;
        Employee_salary  = p;
    }
  
    static void get() {
        System.out.println("employee name: " + Employee_name );
        System.out.println("employee salary: " + Employee_salary);
    }

    public static void main(String args[]) {
        sample1.set("ABC", 60000.0f);
        sample1.get();
    }
}