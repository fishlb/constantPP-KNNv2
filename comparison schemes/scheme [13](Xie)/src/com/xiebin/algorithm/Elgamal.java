package com.xiebin.algorithm;

import com.xiebin.entity.ElgamalParameters;
import com.xiebin.entity.Ciphertext;
import java.math.BigInteger;
import java.util.Random;

public class Elgamal {
    private ElgamalParameters elgamalParams;
    private BigInteger N_div_4;
    public Elgamal(ElgamalParameters elgamalParams) {
        this.elgamalParams = elgamalParams;
        this.N_div_4=elgamalParams.getN().divide(new BigInteger("4"));
    }
    private BigInteger getR(){
        return new BigInteger(N_div_4.bitLength(),new Random()).mod(N_div_4);
    }
    public Ciphertext encrypt(BigInteger value){
        BigInteger r=getR();
        BigInteger A=elgamalParams.getG().modPow(r,elgamalParams.getN2());
        BigInteger B=value.multiply(elgamalParams.getN()).add(BigInteger.ONE).multiply(elgamalParams.getCommonPublicKey().modPow(r,elgamalParams.getN2())).mod(elgamalParams.getN2());
        return new Ciphertext(A,B);
    }
    public Ciphertext CR(Ciphertext ciphertext){
        Ciphertext zero=encrypt(BigInteger.ZERO);
        return add(ciphertext,zero);
    }
    public Ciphertext add(Ciphertext a,Ciphertext b){
        BigInteger A=a.getA().multiply(b.getA()).mod(elgamalParams.getN2());
        BigInteger B=a.getB().multiply(b.getB()).mod(elgamalParams.getN2());
        return new Ciphertext(A,B);
    }
    public Ciphertext multiply(Ciphertext cipher,BigInteger value){
        BigInteger A=cipher.getA().modPow(value,elgamalParams.getN2());
        BigInteger B=cipher.getB().modPow(value,elgamalParams.getN2());
        return new Ciphertext(A,B);
    }
    public BigInteger decrypt(Ciphertext ciphertext,BigInteger sk){
        return ciphertext.getA().modPow(sk.multiply(new BigInteger("-1")),this.elgamalParams.getN2()).multiply(ciphertext.getB()).mod(elgamalParams.getN2()).subtract(BigInteger.ONE).divide(elgamalParams.getN());
    }
}
