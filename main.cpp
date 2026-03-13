#include "include/secknn.h"
#include <immintrin.h>
#include <wmmintrin.h>
#include <openssl/aes.h>
#include <iostream>
#include <limits>
using namespace std;
// global variables
const int num_threads =max(1u, min(6u, thread::hardware_concurrency()));// paraellel threads for Fig.3 & Fig.4, we set it to 6 for our experiments, you can adjust it according to your machine
int k = 5;//for Table V and Fig.3a,Fig.4a 
vector<int> k_values = {5, 10, 15, 20, 25};//for Fig.3b,Fig.4b
double split_ratio = 0.2;//stratified train / test split, we employ 8:2 

void printMenu()
{
    cout << "\n=====================================\n";
    cout << "   constantPP-KNN Test Console\n";
    cout << "=====================================\n";

    cout << "1) TABLE V - Plaintext baseline\n";
    cout << "2) TABLE V - Ours \n";
    cout << "3) Fig.3a - Offline (Sequential)\n";
    cout << "4) Fig.3a - Offline (Parallel)\n";
    cout << "5) Fig.3b - Offline (Sequential)\n";
    cout << "6) Fig.3b - Offline (Parallel)\n";
    cout << "7) Fig.4a - Latency (Sequential)\n";
    cout << "8) Fig.4a - Latency (Parallel)\n";
    cout << "9) Fig.4b - Latency (Sequential)\n";
    cout << "10) Fig.4b - Latency (Parallel)\n";
    cout << "0) Exit\n";
    cout << "-------------------------------------\n";
    cout << "Enter number: ";
}

int main()
{
    while (true)
    {
        printMenu();

        int choice;

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0)
        {
            cout << "\nExiting program.\n";
            break;
        }

        cout << "\n========== Running Experiment ==========\n";

        switch (choice)
        {
        /* TABLE V */
        case 1:
            cout << "[TABLE V - Plaintext baseline]\n";
            testPlainAcc(k, split_ratio);
            break;

        case 2:
            cout << "[TABLE V - Ours ]\n";
            testConstKNNAcc(k, split_ratio, num_threads);
            break;

        /* Fig.3 */
        case 3:
            cout << "[Fig.3a - Offline (Sequential)]\n";
            test_offline_cost(k, k_values, 0, 0, 1);
            break;

        case 4:
            cout << "[Fig.3a - Offline (Parallel)]\n";
            test_offline_cost(k, k_values, 0, 1, num_threads);
            break;

        case 5:
            cout << "[Fig.3b - Offline (Sequential)]\n";
            test_offline_cost(k, k_values, 1, 0, 1);
            break;

        case 6:
            cout << "[Fig.3b - Offline (Parallel)]\n";
            test_offline_cost(k, k_values, 1, 1, num_threads);
            break;

        /* Fig.4 */
        case 7:
            cout << "[Fig.4a - Latency (Sequential)]\n";
            testLaten(k, k_values, 0, 0, 1);
            break;

        case 8:
            cout << "[Fig.4a - Latency (Parallel)]\n";
            testLaten(k, k_values, 0, 1, num_threads);
            break;

        case 9:
            cout << "[Fig.4b - Latency (Sequential)]\n";
            testLaten(k, k_values, 1, 0, 1);
            break;

        case 10:
            cout << "[Fig.4b - Latency (Parallel)]\n";
            testLaten(k, k_values, 1, 1, num_threads);
            break;

        default:
            cout << "\nInvalid option.\n";
        }
    }

    return 0;
}