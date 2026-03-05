package com.xiebin.utils;

import com.xiebin.entity.Node;

import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class DataLoader {
    public static List<Node> load(String filename,int sampleNumber) throws IOException {
        // File file = new File("src/dataset/"+filename.trim());
        File file = new File(filename.trim());
        BufferedReader reader=new BufferedReader(new FileReader(file));
        List<Node>nodes=new ArrayList<>();
        String line=null;
        while((line=reader.readLine())!=null){
            String[]strs=line.split(",");
            int label=Integer.valueOf(strs[0]);
            long[]values=new long[strs.length-1];
            for(int i=1;i<strs.length;i++){
                values[i-1]=Long.valueOf(strs[i]);
            }
            Node node=new Node(label,values);
            nodes.add(node);
        }
        reader.close();
        Collections.shuffle(nodes);
        if(sampleNumber==-1)
            return nodes;
        return nodes.subList(0,sampleNumber);
    }
}
