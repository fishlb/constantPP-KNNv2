package com.xiebin.utils;

import com.xiebin.entity.ElgamalParameters;

import java.math.BigInteger;
import java.util.Random;

public class ParameterGenerator {
    public ElgamalParameters generate(int pLength,int qLength){
        BigInteger q=new BigInteger(pLength,4096,new Random());
        BigInteger p=new BigInteger(pLength,4096,new Random());
        BigInteger N=p.multiply(q);
        BigInteger N2=N.pow(2);
        BigInteger b=new BigInteger(N2.bitLength(),new Random()).mod(N2);
        while(!N2.gcd(b).toString().equals("1"))
            b=new BigInteger(N2.bitLength(),new Random()).mod(N2);
        BigInteger g=N2.subtract(b.modPow(new BigInteger("2").multiply(N),N2));
        BigInteger commonSecretKey=new BigInteger(N.bitLength(),new Random()).mod(N.divide(new BigInteger("4")));
        return new ElgamalParameters(q,p,g,commonSecretKey);
    }
}
