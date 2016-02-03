package Answers;

import java.io.IOException;

public class Chap1 {


//    Implement an algorithm to determine if a string has all unique characters.
//    What if you cannot use additional data structures?
    public boolean uniqueChar(String str)
    {

        boolean [] check = new boolean [256]; //cause 256 char possible
        for (int i = 0; i < str.length(); i++)
        {
            if (check[str.charAt(i)])
            {
                return false;
            }
            else
            {
                check[str.charAt(i)] = true;
            }
        }

        return true;
    }

    //Given two strings, write a method to decide if one is a permutation of the other.
    //Alternative way is to sort both string and check if equal
    public boolean permcheck(String s1, String s2)
    {
        if (s1.length() != s2.length())
        {
            return false;
        }
        boolean[] check= new boolean[256];
        for(int i= 0; i < s1.length(); i++)
        {
            check[s1.charAt(i)]= true;
        }
        for(int j= 0; j < s2.length(); j++)
        {
            if(!check[s2.charAt(j)])
            {
                return false;
            }
        }
        return true;
    }

    //Write a method to replace all spaces in a string(Char array ) with'%20'.

    public char [] rep(char [] str, int length)
    {
        int space= 0;
        for (int i = 0; i < length; i++)
        {
            if (str[i] == ' ') {
                space++;
            }
        }
        int newlen= length+ space*2;
        for(int j = length-1; j>= 0; j++)
        {
            if(str[j] != ' ')
            {
                str[newlen -1]= str[j];
                newlen = newlen-1;
            }
            else
            {
                str[newlen-1] ='0';
                str[newlen-2]='2';
                str[newlen-3]='%';
                newlen = newlen-3;
            }
        }
        return str;

    }

    //Implement a method to perform basic string compression using the counts of
    //repeated characters. For example, the string aabcccccaaa would become
    //a2blc5a3.
    public String strCompress(String str)
    {
        String newstr="";
        int cntr= 1;
        char pre=str.charAt(0);
        for (int i = 0; i <str.length(); i++)
        {
            if(str.charAt(i) == pre)
            {
                cntr++;
            }
            else
            {
                newstr += pre+""+cntr;
                pre=str.charAt(i);
                cntr = 1;
            }
        }
        newstr += pre+""+cntr;
        return newstr;
    }

    //Write an algorithm such that if an element in an MxN matrix is 0, its entire row and column are set to 0.
    public int[][] zero(int [][] matrix)
    {
        int rows = matrix.length;
        int col= matrix[0].length;
        boolean[] zero_row = new boolean[rows];
        boolean[] zero_col = new boolean[col];
        for(int i = 0; i < rows; i++)
        {
            for (int j = 0; j < col; j++)
            {
                if(matrix[i][j] == 0)
                {
                    zero_row[i] = true;
                    zero_col[j] = true;
                }
            }
        }
        for(int x =0 ; x <rows;x++)
        {
            for (int y = 0; y<col; y++)
            {
                if (zero_col[x] || zero_row[y])
                {
                    matrix[x][y] = 0;
                }
            }
        }
        return matrix;
    }

//    Assume you have a method isSubstring which checks if one word is a substring of another. Given two strings, s i and
//    s2, write code to check if s2 is a rotation of si using only one call to isSubstring
//    (e.g.,"waterbottle"is a rota- tion of "erbottlewat").
    public boolean isRotation(String s1, String s2)
    {
        if(s1.length() == s2.length())
        {
            s2 += s2;
            System.out.println(s2);
            System.out.println(s2.contains(s1));
            if(s2.contains(s1))
            {
                return true;
            }
        }
        return false;
    }









    public static void main(String args[]) throws IOException
    {
        Chap1 a = new Chap1();
        System.out.println(a.uniqueChar("abcd"));
        System.out.println(a.uniqueChar("abacvd"));
        System.out.println(a.permcheck("abacvd","dvcaba"));
        System.out.println(a.permcheck("abad","aba"));
        System.out.println(a.strCompress("aaabbcccccd"));
        System.out.println(a.isRotation("waterbottle", "erbottlewat"));
    }

}
