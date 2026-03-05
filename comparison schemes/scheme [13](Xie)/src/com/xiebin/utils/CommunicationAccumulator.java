package com.xiebin.utils;


import java.math.BigInteger;

public class CommunicationAccumulator {
    private long sumBits;

    public CommunicationAccumulator() {
        this.sumBits=0;
    }

    public CommunicationAccumulator(long sumBits) {
        this.sumBits = sumBits;
    }

    public long getSumBits() {
        return sumBits;
    }

    public void setSumBits(long sumBits) {
        this.sumBits = sumBits;
    }
    public void add(Counter counter){
        this.sumBits+=counter.getBits();
    }

    public void add(BigInteger value){
        this.sumBits+=value.bitLength();
    }
    public void printInfor(String stage){
        // System.out.println(stage+":"+sumBits/8+"B");
        double sumMb = sumBits;
        System.out.println(stage+":"+sumMb/8/1024/1024+" MB");
    }
}
