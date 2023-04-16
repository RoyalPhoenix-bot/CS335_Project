class sample3 {
    public static void main() {
      
        int months = 26;
        int weeks = 13;
        int count=0;

        for(int i=0;i<months;i++){
            for(int j=0;j<weeks;j++){
                int k;
                count++;
                if(count>100){
                    break;
                }
            }
        }
        int day = count%7;
    }        
}
  