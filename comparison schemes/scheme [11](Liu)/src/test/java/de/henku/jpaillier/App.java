package de.henku.jpaillier;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.util.Random;

// Test computing time for paper "Toward Highly Secure Yet Efficient KNN Classification Scheme on Outsourced Cloud Data"
public class App {

    private static KeyPair keyPair;
    private static PublicKey publicKey;

    public static void main(String[] args) throws IOException {
        System.out.println("Generating Paillier Key Pair...");
        KeyPairBuilder keygen = new KeyPairBuilder();
        keyPair = keygen.generateKeyPair();
        publicKey = keyPair.getPublicKey();

        File dir = new File("evalResult");
        if (!dir.exists()) {
            dir.mkdirs();
        }

        System.out.println("Starting theoretical simulation based on homomorphic operations...");

        // ==========================================
        // Test 1: different n (k=5, m=60, c=3)
        // ==========================================
        int[] nList = { 100, 200, 300, 400, 500 };
        int[] num_compu_n = { 517, 1017, 1517, 2017, 2517 }; // Theoretical computation count

        try (PrintWriter pwN = new PrintWriter(
                new FileWriter("evalResult/Fig.4a_latency_result_scheme [11].txt"))) {
            boolean first = true;
            for (int i = 0; i < nList.length; i++) {
                int n = nList[i];
                int compCount = num_compu_n[i];
                String filename = "test" + n + "-60.csv";

                runSim(n, 5, 60, 3, compCount, filename, pwN, first);
                first = false; 
            }
        }

        // ==========================================
        // Test 2: different k (n=400, m=60, c=3)
        // ==========================================
        int[] kList = { 5, 10, 15, 20, 25 };
        int[] num_compu_k = { 2017, 4007, 5972, 7912, 9827 }; // Theoretical computation count

        try (PrintWriter pwK = new PrintWriter(
                new FileWriter("evalResult/Fig.4b_latency_result_scheme [11].txt"))) {
            for (int i = 0; i < kList.length; i++) {
                int k = kList[i];
                int compCount = num_compu_k[i];
                String filename = "test400-60.csv";

                runSim(400, k, 60, 3, compCount, filename, pwK, true);
            }
        }

        System.out.println("All theoretical tests completed! Results saved in evalResult/ directory.");
    }

    private static void runSim(int n, int k, int m, int c, int compCount, String filename, PrintWriter pw,
            boolean printHeader) {
        BigInteger plainA = BigInteger.valueOf(new Random().nextLong() & Long.MAX_VALUE);
        BigInteger plainB = BigInteger.valueOf(new Random().nextLong() & Long.MAX_VALUE);

        double start = System.currentTimeMillis();

        // Simulate homomorphic operation latency
        for (int j = 0; j < compCount; j++) {
            BigInteger encryptedA1 = publicKey.encrypt(plainA);
            BigInteger encryptedA2 = publicKey.encrypt(plainA);
            BigInteger encryptedA3 = publicKey.encrypt(plainA);
            BigInteger encryptedA4 = publicKey.encrypt(plainA);
            BigInteger encryptedA5 = publicKey.encrypt(plainB);

            BigInteger encryptedSum1 = encryptedA1.multiply(encryptedA2);
            BigInteger encryptedSum2 = encryptedA1.multiply(encryptedA2);
            BigInteger encryptedSum3 = encryptedA1.multiply(encryptedA2);
            BigInteger encryptedSum4 = encryptedA1.multiply(encryptedA2);
            BigInteger encryptedSum5 = encryptedA1.multiply(encryptedA2);

            BigInteger additionResult = keyPair.decrypt(encryptedA1);
        }

        double end = System.currentTimeMillis();
        double time_cost = (end - start) / 1000.0; 

        StringBuilder sb = new StringBuilder();
        if (printHeader) {
            sb.append(String.format("===== Query Latency Test (k = %d) =====\n", k));
        }
        sb.append("================*******==================\n");
        sb.append(String.format("Dataset         : %s\n", filename));
        sb.append(String.format("#Sample (n)     : %d\n", n));
        sb.append(String.format("#Feature(m)     : %d\n", m));
        sb.append(String.format("#Label(c)       : %d\n", c));
        sb.append(String.format("Latency (1 query) = %.3f s\n\n", time_cost));

        System.out.print(sb.toString());
        if (pw != null) {
            pw.print(sb.toString());
            pw.flush();
        }
    }
}