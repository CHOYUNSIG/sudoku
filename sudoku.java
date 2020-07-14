import java.util.Scanner;
public class sudoku {
    public static void main(String[] args) {
        Scanner scan = new Scanner(System. in);
        System.out.println("SUDOKU ALGORITHM @ Java");
        gameinfo game;
        op opr = new op();
        while (true) {
            game = new gameinfo();
            String xy;
            int x,
            y,
            v;
            int exit = 0;
            game.print();
            System.out.printf("\nPlease enter coordinate value you want to put the number.\nex) A1\nIf you are ready to get the solution, enter 0.\n");
            while (exit != 1) {
                System.out.printf("\n> Location : ");
                xy = scan.nextLine();
                if (xy.charAt(0) == '0') {
                    exit = 1;
                } else if (xy.charAt(0) < 65 || xy.charAt(0) > 73 || xy.charAt(1) < 49 || xy.charAt(1) > 57) {
                    System.out.printf("Erorr. Please try again.\n");
                } else {
                    x = (int)xy.charAt(1) - 49;
                    y = (int)xy.charAt(0) - 65;
                    System.out.printf("> Value : ");
                    v = scan.nextInt();
                    scan.nextLine();
                    if (v == 0) {
                        game.a[x][y] = v;
                        System.out.printf("%c%c is removed.\n", xy.charAt(0), xy.charAt(1));
                        game.print();
                    } else if (v < 1 || v > 9) {
                        System.out.printf("#erorr(Invalid value)\nPlease try again.\n");
                    } else {
                        game.a[x][y] = v + 10;
                        if (opr.errorSearch(game.a) == 1) {
                            game.a[x][y] = 0;
                            System.out.printf("#erorr(Duplicate value)\nPlease try again.\n");
                        }
                        game.print();
                    }
                }
            }
            game.a = opr.sudoku(game.a);
            System.out.printf("\nSolution :\n");
            game.print();
            System.out.printf("\n--------------------------------------------------\n");
        }
    }
}
class gameinfo {
    int [][] a = new int[9][9];
    public gameinfo() {
        for (int i = 0; i < 9; i ++)
            for (int j = 0; j < 9; j ++)
                a[i][j] = 0;


    }
    public void print() {
        int c = 0,
        t = 0;
        System.out.printf("\n | 1 2 3 | 4 5 6 | 7 8 9\n");
        for (int i3 = 0; i3 < 3; i3 ++) {
            System.out.printf("-+-------+-------+-------\n");
            for (int i2 = 0; i2 < 3; i2 ++) {
                System.out.printf("%c", t + 65);
                for (int i = 0; i < 12; i ++) {
                    if (i % 4 == 0) {
                        System.out.printf("|");
                    } else {
                        if (a[c][t] > 10) {
                            System.out.printf("%d", a[c][t] - 10);
                        } else if (a[c][t] == 0) {
                            System.out.printf(" ");
                        } else {
                            System.out.printf("%d", a[c][t]);
                        }
                        c ++;
                    }
                    System.out.printf(" ");
                }
                System.out.printf("\n");
                c = 0;
                t ++;
            }
        }
    }
}
class op {
    public int[][] sudoku(int[][] a) {
        int[] p = {0,0}; // pivot coordinate value
        int key = 0;
        while (a[p[0]][p[1]] != 0) {
            p[0]++;
            if (p[0] == 9) {
                p[0] = 0;
                p[1]++;
                if (p[1] == 9)
                    return a;
            }
        }
        while (key != 1) {
            a[p[0]][p[1]]++;
            if (a[p[0]][p[1]] == 10) {
              a[p[0]][p[1]] = 0;
                while (a[p[0]][p[1]] > 10 || a[p[0]][p[1]] == 0) {
                    p[0]--;
                    if (p[0] < 0) {
                        p[0] = 8;
                        p[1]--;
                    }
                }
            } else if (this.errorSearch(a) == 0) {
                while (a[p[0]][p[1]] != 0) {
                    p[0]++;
                    if (p[0] == 9) {
                        p[0] = 0;
                        p[1]++;
                        if (p[1] == 9) {
                            key = 1;
                            break;
                        }
                    }
                }
            }
        }
        return a;
    }
    public int errorSearch(int[][] a) {
        for (int i = 0; i < 9; i ++)
            for (int x = 0; x < 8; x ++)
                for (int y = x + 1; y < 9; y ++) {
                    if (a[i][x] != 0 && a[i][y] != 0 && (a[i][x] == a[i][y] || a[i][x] + 10 == a[i][y] || a[i][x] == a[i][y] + 10))
                        return 1;
                    if (a[x][i] != 0 && a[y][i] != 0 && (a[x][i] == a[y][i] || a[x][i] + 10 == a[y][i] || a[x][i] == a[y][i] + 10))
                        return 1;
                }
        int[] area = new int[9];
        int t = 0;
        for (int x = 0; x < 3; x ++) {
            for (int y = 0; y < 3; y ++) {
                for (int i = 0; i < 3; i ++) {
                    for (int i2 = 0; i2 < 3; i2 ++) {
                        area[t] = a[x * 3 + i][y * 3 + i2];
                        t ++;
                    }
                }
                t = 0;
                for (int p1 = 0; p1 < 8; p1 ++) {
                    for (int p2 = p1 + 1; p2 < 9; p2 ++) {
                        if (area[p1] != 0 && area[p2] != 0 && (area[p1] == area[p2] || area[p1] + 10 == area[p2] || area[p1] == area[p2] + 10)) {
                            return 1;
                        }
                    }
                }
            }
        }
        return 0;
    } // return 1 -> error
}
