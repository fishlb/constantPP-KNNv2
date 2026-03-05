import com.xiebin.simulate.SystemSimulator;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

public class App {
    public static void main(String[] args) throws Exception {
        File dir = new File("evalResult");
        if (!dir.exists()) {
            dir.mkdirs();
        }

        System.out.println("Starting simulation...");

        // 1. fixed k=5
        int[] nList = {100, 200, 300, 400, 500};
        try (PrintWriter pwN = new PrintWriter(new FileWriter("evalResult/Fig.4a_latency_result_scheme [13].txt"))) {
            boolean first = true;
            for (int n : nList) {
                SystemSimulator sim = new SystemSimulator();
                // sim.setConfig(3, 5, 256, 256, 1000, n, "test" + n + "-60.csv");
                sim.setConfig(3, 5, 256, 256, 1000, n, "src/dataset/test" + n + "-60.csv");
                sim.simulate(pwN, first);
                first = false; 
            }
        }

        // 2. fixed n=400
        int[] kList = {5, 10, 15, 20, 25};
        try (PrintWriter pwK = new PrintWriter(new FileWriter("evalResult/Fig.4b_latency_result_scheme [13].txt"))) {
            for (int k : kList) {
                SystemSimulator sim = new SystemSimulator();
                // sim.setConfig(3, k, 256, 256, 1000, 400,"dataset/test400-60.csv");
                sim.setConfig(3, k, 256, 256, 1000, 400, "src/dataset/test400-60.csv");
                sim.simulate(pwK, true); 
            }
        }

        System.out.println("All batch tests completed! Results saved in evalResult/ directory.");
    }
}