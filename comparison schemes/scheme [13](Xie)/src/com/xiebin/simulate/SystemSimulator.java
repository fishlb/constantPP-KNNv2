package com.xiebin.simulate;

import com.xiebin.algorithm.Elgamal;
import com.xiebin.entity.*;
import com.xiebin.utils.CommunicationAccumulator;
import com.xiebin.utils.DataLoader;
import com.xiebin.utils.ParameterGenerator;
import com.xiebin.utils.TimeAccumulator;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.util.*;

public class SystemSimulator {
    private Map<String, Object> map;

    public SystemSimulator() {
        map = new HashMap<>();
    }

    public void setConfig(int classNum, int K, int pLength, int qLength, int alpha, int datasetSize, String datasetFile) {
        map.put("classNum", classNum);
        map.put("K", K);
        map.put("pLength", pLength);
        map.put("qLength", qLength);
        map.put("alpha", alpha);
        map.put("datasetSize", datasetSize);
        map.put("originalDatasetFile", datasetFile);
        map.put("userRequestFilename", datasetFile);
    }

    public void simulate(PrintWriter writer, boolean printHeader) throws IOException {
        System.out.println("\n--- Testing for " + map.get("originalDatasetFile") + " (K=" + map.get("K") + ") ---");
        
        systemInitial();
        dataOutsourcing();
        
        // the time of core protocols
        long tReq = userRequestGenerating();
        long tEdc = EDC();
        long tTbkf = TBKF();
        
        resultReturning();
        resultReading();

        // total latency
        double latencySec = (tReq + tEdc + tTbkf) / 1000.0;
        
        String filename = (String) map.get("originalDatasetFile");
        String shortName = new File(filename).getName(); 
        int n = (Integer) map.get("datasetSize");
        int k = (Integer) map.get("K");
        int c = (Integer) map.get("classNum");
        EncryptNode userReq = (EncryptNode) map.get("userRequest");
        int m = userReq.getEncryptNums().length;

        StringBuilder sb = new StringBuilder();
        if (printHeader) {
            sb.append(String.format("===== Query Latency Test (k = %d) =====\n", k));
        }
        sb.append("================*******==================\n");
        sb.append(String.format("Dataset         : %s\n", shortName));
        sb.append(String.format("#Sample (n)     : %d\n", n));
        sb.append(String.format("#Feature(m)     : %d\n", m));
        sb.append(String.format("#Label(c)       : %d\n", c));
        sb.append(String.format("Latency (1 query) = %.3f s\n\n", latencySec));

        System.out.print(sb.toString());

        if (writer != null) {
            writer.print(sb.toString());
            writer.flush();
        }
    }

    public void systemInitial() {
        ElgamalParameters elgamalParameters = new ParameterGenerator().generate((Integer) map.get("pLength"), (Integer) map.get("qLength"));
        KeyPair keyPair = elgamalParameters.generateKeyPair();
        Elgamal elgamal = new Elgamal(elgamalParameters);
        map.put("elgamalParameters", elgamalParameters);
        map.put("keyPair", keyPair);
        map.put("elgamal", elgamal);
    }

    public void dataOutsourcing() throws IOException {
        List<Node> nodes = DataLoader.load((String) map.get("originalDatasetFile"), (Integer) map.get("datasetSize"));
        ElgamalParameters elgamalParameters = (ElgamalParameters) map.get("elgamalParameters");
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        BigInteger alpha = new BigInteger("" + map.get("alpha"));
        int dataCount = 0;
        
        List<EncryptNode> ED = new ArrayList<>();
        for (Node node : nodes) {
            dataCount++;
            Ciphertext[] encryptNums = new Ciphertext[node.getNums().length];
            for (int i = 0; i < encryptNums.length; i++)
                encryptNums[i] = elgamal.encrypt(new BigInteger("" + node.getNums()[i]));
            Ciphertext encryptLabel = elgamal.encrypt(alpha.pow(node.getLabel()));
            EncryptNode encryptNode = new EncryptNode(encryptNums, encryptLabel);
            ED.add(encryptNode);
        }
        map.put("ED", ED);
    }

    public long userRequestGenerating() throws IOException {
        Node userNode = DataLoader.load((String) map.get("userRequestFilename"), -1).get(0);
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        
        long startTime = System.currentTimeMillis();
        Ciphertext[] ciphertexts = new Ciphertext[userNode.getNums().length];
        for (int i = 0; i < ciphertexts.length; i++) {
            ciphertexts[i] = elgamal.encrypt(new BigInteger("" + userNode.getNums()[i]));
        }
        EncryptNode encryptUserNode = new EncryptNode(ciphertexts, null);
        long endTime = System.currentTimeMillis();
        
        map.put("userRequest", encryptUserNode);
        return endTime - startTime;
    }

    public long EDC() {
        List<EncryptNode> ED = (List<EncryptNode>) map.get("ED");
        EncryptNode userRequest = (EncryptNode) map.get("userRequest");
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        CommunicationAccumulator communicationAccumulator = new CommunicationAccumulator();
        
        long startTime = System.currentTimeMillis();
        EncryptNode oppositeUserRequest = new EncryptNode();
        oppositeUserRequest.setEncryptNums(new Ciphertext[userRequest.getEncryptNums().length]);
        for (int i = 0; i < userRequest.getEncryptNums().length; i++)
            oppositeUserRequest.getEncryptNums()[i] = elgamal.multiply(userRequest.getEncryptNums()[i], new BigInteger("-1"));
        
        List<DistanceLabelPair> pairs = new ArrayList<>();
        for (EncryptNode encryptNode : ED) {
            Ciphertext[] differences = new Ciphertext[encryptNode.getEncryptNums().length];
            for (int i = 0; i < differences.length; i++)
                differences[i] = elgamal.add(oppositeUserRequest.getEncryptNums()[i], encryptNode.getEncryptNums()[i]);
            Ciphertext encryptDistance = S3P(differences, communicationAccumulator);
            pairs.add(new DistanceLabelPair(encryptDistance, encryptNode.getEncryptLabel()));
        }
        long endTime = System.currentTimeMillis();
        
        map.put("distanceLabelPairs", pairs);
        return endTime - startTime;
    }

    private Ciphertext S3P(Ciphertext[] encryptVector, CommunicationAccumulator communicationAccumulator) {
        ElgamalParameters elgamalParameters = (ElgamalParameters) map.get("elgamalParameters");
        KeyPair pair = (KeyPair) map.get("keyPair");
        Elgamal elgamal = (Elgamal) map.get("elgamal");

        BigInteger[] B_i_t = new BigInteger[encryptVector.length];
        BigInteger[] d_i_t_1 = new BigInteger[encryptVector.length];
        for (int i = 0; i < B_i_t.length; i++) {
            d_i_t_1[i] = new BigInteger(elgamalParameters.getN().bitLength(), new Random()).mod(elgamalParameters.getN());
            B_i_t[i] = BigInteger.ONE.add(d_i_t_1[i].multiply(elgamalParameters.getN())).multiply(encryptVector[i].getA().modPow(pair.getSk1(), elgamalParameters.getN2())).modPow(new BigInteger("-1"), elgamalParameters.getN2()).multiply(encryptVector[i].getB()).mod(elgamalParameters.getN2());
        }

        BigInteger[] d_i_t_2 = new BigInteger[d_i_t_1.length];
        for (int i = 0; i < d_i_t_2.length; i++)
            d_i_t_2[i] = B_i_t[i].multiply(encryptVector[i].getA().modPow(new BigInteger("-1").multiply(pair.getSk2()), elgamalParameters.getN2())).mod(elgamalParameters.getN2()).subtract(BigInteger.ONE).divide(elgamalParameters.getN());
        
        Ciphertext dis_2 = null;
        for (int i = 0; i < d_i_t_2.length; i++) {
            if (i == 0)
                dis_2 = elgamal.multiply(encryptVector[i], d_i_t_2[i]);
            else
                dis_2 = elgamal.add(dis_2, elgamal.multiply(encryptVector[i], d_i_t_2[i]));
        }
        dis_2 = elgamal.CR(dis_2);

        Ciphertext dis_1 = null;
        for (int i = 0; i < d_i_t_1.length; i++) {
            if (i == 0)
                dis_1 = elgamal.multiply(encryptVector[i], d_i_t_1[i]);
            else
                dis_1 = elgamal.add(dis_1, elgamal.multiply(encryptVector[i], d_i_t_1[i]));
        }
        return elgamal.add(elgamal.CR(dis_1), dis_2);
    }

    public long TBKF() {
        List<DistanceLabelPair> pairs = (List<DistanceLabelPair>) map.get("distanceLabelPairs");
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        Integer K = (Integer) map.get("K");
        CommunicationAccumulator communicationAccumulator = new CommunicationAccumulator();

        long startTime = System.currentTimeMillis();
        for (DistanceLabelPair pair : pairs)
            pair.setLabel(elgamal.CR(pair.getLabel()));
        Collections.shuffle(pairs);

        Collections.shuffle(pairs);
        int[] index = new int[pairs.size() * 2];
        for (int i = index.length - 1; i >= pairs.size(); i--)
            index[i] = i - pairs.size();
        
        for (int i = pairs.size() - 1; i >= 1; i--) {
            if (SCP(pairs.get(index[2 * i]).getDistance(), pairs.get(index[2 * i + 1]).getDistance(), communicationAccumulator) > 0)
                index[i] = index[2 * i + 1];
            else
                index[i] = index[2 * i];
        }
        
        List<Ciphertext> resultLabels = new ArrayList<>();
        for (int i = 0; i < K - 1; i++) {
            resultLabels.add(pairs.get(index[1]).getLabel());
            AL(index, communicationAccumulator);
        }
        resultLabels.add(pairs.get(index[1]).getLabel());
        long endTime = System.currentTimeMillis();
        
        map.put("resultLabels", resultLabels);
        return endTime - startTime;
    }

    public void resultReturning() {
        ElgamalParameters parameters = (ElgamalParameters) map.get("elgamalParameters");
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        KeyPair keyPair = (KeyPair) map.get("keyPair");
        List<Ciphertext> labels = (List<Ciphertext>) map.get("resultLabels");
        
        Ciphertext result = null;
        for (int i = 0; i < labels.size(); i++) {
            if (i == 0) result = labels.get(i);
            else result = elgamal.add(result, labels.get(i));
        }
        result = elgamal.CR(result);
        result.setB(result.getA().modPow(keyPair.getConvertKey2(), parameters.getN2()).multiply(result.getB()).mod(parameters.getN2()));
        result.setB(result.getA().modPow(keyPair.getConvertKey1(), parameters.getN2()).multiply(result.getB()).mod(parameters.getN2()));
        map.put("KNNResult", result);
    }

    private int SCP(Ciphertext dis1, Ciphertext dis2, CommunicationAccumulator communicationAccumulator) {
        ElgamalParameters elgamalParameters = (ElgamalParameters) map.get("elgamalParameters");
        Elgamal elgamal = new Elgamal(elgamalParameters);
        KeyPair keyPair = (KeyPair) map.get("keyPair");

        int r3 = new Random().nextFloat() > 0.5 ? 1 : -1;
        Ciphertext c = null;
        if (r3 == 1)
            c = elgamal.add(dis1, elgamal.multiply(dis2, new BigInteger("-1")));
        else
            c = elgamal.add(dis2, elgamal.multiply(dis1, new BigInteger("-1")));
        BigInteger r1 = new BigInteger(elgamalParameters.getN().bitLength() / 2 - 1, new Random());
        BigInteger r2 = new BigInteger(elgamalParameters.getN().bitLength() / 2 - 1, new Random()).mod(r1);
        Ciphertext d = elgamal.add(elgamal.encrypt(r2), elgamal.multiply(c, r1));
        d.setB(d.getA().modPow(keyPair.getSk2().multiply(new BigInteger("-1")), elgamalParameters.getN2()).multiply(d.getB()).mod(elgamalParameters.getN2()));
        
        BigInteger p = d.getB().multiply(d.getA().modPow(keyPair.getSk1().multiply(new BigInteger("-1")), elgamalParameters.getN2())).mod(elgamalParameters.getN2()).subtract(BigInteger.ONE).divide(elgamalParameters.getN());
        int r4 = 0;
        if (p.compareTo(elgamalParameters.getN().divide(new BigInteger("2"))) < 0)
            r4 = 1;
        else
            r4 = -1;
        return r3 * r4;
    }

    private void AL(int[] index, CommunicationAccumulator communicationAccumulator) {
        List<DistanceLabelPair> pairs = (List<DistanceLabelPair>) map.get("distanceLabelPairs");
        int t = 1;
        Stack<Integer> stack = new Stack<>();
        while (true) {
            stack.push(t);
            if (2 * t + 1 > index.length - 1) {
                index[t] = -1;
                break;
            }
            if (index[2 * t] == index[t]) {
                index[t] = -1;
                t *= 2;
            } else {
                index[t] = -1;
                t = t * 2 + 1;
            }
        }
        while (!stack.isEmpty()) {
            int i = stack.pop();
            if (2 * i + 1 > index.length - 1 || (index[2 * i + 1] == -1 && index[2 * i] == -1))
                continue;
            if (index[2 * i] != -1 && index[2 * i + 1] != -1) {
                if (SCP(pairs.get(index[2 * i]).getDistance(), pairs.get(index[2 * i + 1]).getDistance(), communicationAccumulator) > 0) {
                    index[i] = index[2 * i + 1];
                } else {
                    index[i] = index[2 * i];
                }
            } else {
                if (index[2 * i] != -1)
                    index[i] = index[2 * i];
                else
                    index[i] = index[2 * i + 1];
            }
        }
    }

    public void resultReading() {
        CommunicationAccumulator communicationAccumulator = new CommunicationAccumulator();
        MCS(communicationAccumulator);
    }

    private int MCS(CommunicationAccumulator communicationAccumulator) {
        KeyPair keyPair = (KeyPair) map.get("keyPair");
        Elgamal elgamal = (Elgamal) map.get("elgamal");
        Ciphertext result = (Ciphertext) map.get("KNNResult");
        
        BigInteger value = elgamal.decrypt(result, keyPair.getUserSecretKey());
        int classNum = (Integer) map.get("classNum");
        BigInteger alpha = new BigInteger(map.get("alpha").toString());
        Map<Integer, BigInteger> counts = new HashMap<>();
        BigInteger t = BigInteger.ZERO;

        for (int i = classNum - 1; i >= 0; i--) {
            BigInteger base = alpha.pow(i);
            BigInteger a = value.subtract(t).divide(base);
            counts.put(i, a);
            t = t.add(a.multiply(base));
        }

        int finalResult = 0;
        for (int i = 1; i < classNum; i++) {
            if (counts.get(i).compareTo(counts.get(finalResult)) > 0) {
                finalResult = i;
            }
        }
        return finalResult;
    }
}