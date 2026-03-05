package com.xiebin.entity;

import com.xiebin.utils.Counter;

public class EncryptNode implements Counter {
    private Ciphertext[]encryptNums;
    private Ciphertext encryptLabel;
    public EncryptNode(){}
    public EncryptNode(Ciphertext[] encryptNums, Ciphertext encryptLabel) {
        this.encryptNums = encryptNums;
        this.encryptLabel = encryptLabel;
    }

    public Ciphertext[] getEncryptNums() {
        return encryptNums;
    }

    public void setEncryptNums(Ciphertext[] encryptNums) {
        this.encryptNums = encryptNums;
    }

    public Ciphertext getEncryptLabel() {
        return encryptLabel;
    }

    public void setEncryptLabel(Ciphertext encryptLabel) {
        this.encryptLabel = encryptLabel;
    }

    @Override
    public long getBits() {
        long result=0;
        if(this.encryptLabel!=null)
            result+=encryptLabel.getBits();
        for(Ciphertext ciphertext:encryptNums)
            result+=ciphertext.getBits();
        return result;
    }
}
