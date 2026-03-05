package com.xiebin.entity;

public class Node {
    private int label;
    private long[]nums;

    public Node(int label, long[] nums) {
        this.label = label;
        this.nums = nums;
    }

    public int getLabel() {
        return label;
    }

    public void setLabel(int label) {
        this.label = label;
    }

    public long[] getNums() {
        return nums;
    }

    public void setNums(long[] nums) {
        this.nums = nums;
    }
}
