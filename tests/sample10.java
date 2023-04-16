package m2_test_cases;

public class sample10 {
    int w;

    int compute(int a,int b){
        return a+b;
    }

    public void main(){
        sample10 x;
        int a=5;
        int b=10;
        for(int i=0;i<a;i++){
            for(int j=0;j<5;j++){
                x.compute(i, j);
            } 
        }
    }
}
