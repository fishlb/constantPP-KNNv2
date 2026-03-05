package com.xiebin.utils;

public class TimeAccumulator {
    private long sumTime;

    public TimeAccumulator() {
        this.sumTime=0;
    }
    public void add(long time){
        this.sumTime+=time;
    }
    public TimeAccumulator(long sumTime) {
        this.sumTime = sumTime;
    }

    public long getSumTime() {
        return sumTime;
    }

    public void setSumTime(long sumTime) {
        this.sumTime = sumTime;
    }
    public void printInfor(String stage){
        System.out.println(stage+":"+sumTime+"ms");
    }
}
