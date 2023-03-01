package programs;

public class sample2 {
    public static void main(String[] args) {
        int rows = 4;
        int columns = 3;
        int depth = 5;
 
        int[][][] array = new int[rows][columns][depth];
 
        int value = 1;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                for (int k = 0; k < depth; k++) {
                    array[i][j][k] = value;
                    value++;
                }
            }
        }
 
        System.out.println("The 3D array is: ");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                for (int k = 0; k < depth; k++) {
                    System.out.print(array[i][j][k] + " ");
                }
                System.out.println();
            }
            System.out.println();
        }
    }
}