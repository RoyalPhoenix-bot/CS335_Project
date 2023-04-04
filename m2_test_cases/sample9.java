public class sample9 {
    public void main(String[] args){
        int counter=3;

        for (int i = 2; i <=10; i++) {
            if(i%12>9){
                break;
            }
            if (i == 4) {
              continue;
            }
            counter++;
        }
    }
}
