package com.xiebin.entity;

import com.xiebin.utils.Counter;

import java.math.BigInteger;

public class Ciphertext implements Counter {
    private BigInteger A;
    private BigInteger B;
    public Ciphertext(BigInteger A, BigInteger B) {
        this.A = A;
        this.B = B;
    }
    public BigInteger getA() {
        return A;
    }
    public void setA(BigInteger a) {
        A = a;
    }
    public BigInteger getB() {
        return B;
    }
    public void setB(BigInteger b) {
        B = b;
    }

    @Override
    public long getBits() {
        return this.A.bitLength()+this.B.bitLength();
    }
}
