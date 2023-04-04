class sample2 {
    private int secret_number;
  
    // constructor
    sample2() {
      secret_number = 42;
    }
    void increment(){
        secret_number++;
    }
  
    public static void main(String[] args) {
  
        for(int i=0;i<10;i++){
            sample2 obj = new sample2();
            obj.increment();
        }
    }
}
  