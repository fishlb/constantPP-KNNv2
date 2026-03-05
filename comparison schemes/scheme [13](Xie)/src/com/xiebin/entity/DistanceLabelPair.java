package com.xiebin.entity;

import com.xiebin.utils.Counter;

public class DistanceLabelPair implements Counter {
    private Ciphertext distance;
    private Ciphertext label;

    public DistanceLabelPair(Ciphertext distance, Ciphertext label) {
        this.distance = distance;
        this.label = label;
    }

    public Ciphertext getDistance() {
        return distance;
    }

    public void setDistance(Ciphertext distance) {
        this.distance = distance;
    }

    public Ciphertext getLabel() {
        return label;
    }

    public void setLabel(Ciphertext label) {
        this.label = label;
    }

    @Override
    public long getBits() {
        return this.distance.getBits()+this.getLabel().getBits();
    }
}
