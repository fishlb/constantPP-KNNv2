package com.xiebin.entity;

import java.math.BigInteger;
import java.util.Random;

public class ElgamalParameters {
    private BigInteger commonSecretKey;
    private BigInteger commonPublicKey;
    private BigInteger N;
    private BigInteger N2;
    private BigInteger q;
    private BigInteger p;
    private BigInteger g;
    public ElgamalParameters(BigInteger q, BigInteger p, BigInteger g, BigInteger commonSecretKey) {
        this.q = q;
        this.p = p;
        this.g = g;
        this.N=p.multiply(q);
        this.N2=N.pow(2);
        this.commonSecretKey=commonSecretKey;
        this.commonPublicKey=g.modPow(commonSecretKey,N2);
    }
    public KeyPair generateKeyPair(){
        BigInteger order=p.subtract(BigInteger.ONE).multiply(q.subtract(BigInteger.ONE)).divide(new BigInteger("2"));
        BigInteger N_div_4=this.N.divide(new BigInteger("4"));
        BigInteger userSecretKey=new BigInteger(N_div_4.bitLength(),new Random()).mod(N_div_4);
        BigInteger convertKey=userSecretKey.subtract(commonSecretKey).mod(order);
        BigInteger convertKey1=new BigInteger(order.bitLength(),new Random()).mod(order);
        BigInteger convertKey2=convertKey.subtract(convertKey1).mod(order);
        BigInteger sk1=new BigInteger(order.bitLength(),new Random()).mod(order);
        BigInteger sk2=commonSecretKey.subtract(sk1).mod(order);
        return new KeyPair(userSecretKey,convertKey1,convertKey2,sk1,sk2);
    }
    public BigInteger getCommonSecretKey() {
        return commonSecretKey;
    }

    public void setCommonSecretKey(BigInteger commonSecretKey) {
        this.commonSecretKey = commonSecretKey;
    }

    public BigInteger getCommonPublicKey() {
        return commonPublicKey;
    }

    public void setCommonPublicKey(BigInteger commonPublicKey) {
        this.commonPublicKey = commonPublicKey;
    }

    public BigInteger getN() {
        return N;
    }

    public void setN(BigInteger n) {
        N = n;
    }

    public BigInteger getN2() {
        return N2;
    }

    public void setN2(BigInteger n2) {
        N2 = n2;
    }

    public BigInteger getQ() {
        return q;
    }

    public void setQ(BigInteger q) {
        this.q = q;
    }

    public BigInteger getP() {
        return p;
    }

    public void setP(BigInteger p) {
        this.p = p;
    }

    public BigInteger getG() {
        return g;
    }

    public void setG(BigInteger g) {
        this.g = g;
    }
}
