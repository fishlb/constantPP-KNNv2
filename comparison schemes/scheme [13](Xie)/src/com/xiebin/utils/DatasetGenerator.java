package com.xiebin.utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class DatasetGenerator {
    public static void generateDataset(int datasetSize,int dimensions,long valueRange,int classNumber,String fileName) throws IOException {
        File file = new File("src/dataset/"+fileName.trim());
        if(file.exists())
            file.delete();
        BufferedWriter writer=new BufferedWriter(new FileWriter(file));
        for(int i=0;i<datasetSize;i++){
            int label=new Random().nextInt(classNumber);
            String line=label+"";
            for(int j=0;j<dimensions;j++)
                line+=","+(long)(new Random().nextDouble()*valueRange);
            writer.write(line);
            if(i!=datasetSize-1)
                writer.newLine();
        }
        writer.close();
    }
    public static void main(String[]args) throws IOException {
        generateDataset(10000,30,5000,5,"testTrain30.csv");
        }
        }
