package Answers;

import java.io.IOException;
import java.util.Hashtable;


public class Chap2 {


    public static void main(String args[]) throws IOException
    {
        Chap2 a = new Chap2();
        Node head = new Node(1);

    }

}

class Node {

    Node next = null;
    int data;

    public Node(int d) {
        data = d;
    }

    void appendToTail(int d) {
        Node end = new Node(d);
        Node n = this;
        while (n.next != null) {
            n = n.next;
        }
        n.next = end;
    }

    Node deleteNode(Node head, int d) {

        Node n = head;

        if (n.data == d) {
            return head.next;
        }

        while (n.next != null)
        {
            if (n.next.data == d) {
                n.next = n.next.next;
                return head;
            }

        }
        return head;
    }
    //Remove Duplicates from an unsorted linked list
    public void remDup(Node head)
    {
        Hashtable a = new Hashtable();
        Node pre= null;
        while (head != null)
        {
            if(a.containsKey(head.data))
            {
                pre.next = head.next;
            }
            else
            {
                a.put(head.data, true);
                pre =head;
            }
            head = head.next;
        }

    }
    //Implement an algorithm to find the kth to last element of a singly linked list.
    public Node Kelem(Node head, int k)
    {
        if(head == null)
        {
            return null;
        }
        Node elem  = head;
        Node runner = head;
        int cntr= 0;
        while(cntr < k)
        {
            runner = runner.next;
        }

        while(runner.next != null)
        {
            runner = runner.next;
            elem = elem.next;
        }
        return elem;
    }

    //Implement an algorithm to delete a node in the middle of a singly linked list, given only access to that node.
    public void delOnly(Node node)
    {
        if(node != null) {
            node.data = node.next.data;
            node.next = node.next.next;
        }
    }


}