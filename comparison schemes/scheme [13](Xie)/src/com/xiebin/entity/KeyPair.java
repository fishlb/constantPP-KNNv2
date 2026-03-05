package com.xiebin.entity;

import java.math.BigInteger;

public class KeyPair {
    private BigInteger userSecretKey;
    private BigInteger convertKey1;
    private BigInteger convertKey2;
    private BigInteger sk1;
    private BigInteger sk2;

    public KeyPair(BigInteger userSecretKey, BigInteger convertKey1, BigInteger convertKey2, BigInteger sk1, BigInteger sk2) {
        this.userSecretKey = userSecretKey;
        this.convertKey1 = convertKey1;
        this.convertKey2 = convertKey2;
        this.sk1 = sk1;
        this.sk2 = sk2;
    }

    public BigInteger getSk1() {
        return sk1;
    }

    public void setSk1(BigInteger sk1) {
        this.sk1 = sk1;
    }

    public BigInteger getSk2() {
        return sk2;
    }

    public void setSk2(BigInteger sk2) {
        this.sk2 = sk2;
    }


    public BigInteger getUserSecretKey() {
        return userSecretKey;
    }

    public void setUserSecretKey(BigInteger userSecretKey) {
        this.userSecretKey = userSecretKey;
    }

    public BigInteger getConvertKey1() {
        return convertKey1;
    }

    public void setConvertKey1(BigInteger convertKey1) {
        this.convertKey1 = convertKey1;
    }

    public BigInteger getConvertKey2() {
        return convertKey2;
    }

    public void setConvertKey2(BigInteger convertKey2) {
        this.convertKey2 = convertKey2;
    }

}
