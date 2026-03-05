#include "include/secknn.h"
#include <immintrin.h>
#include <wmmintrin.h>
#include <openssl/aes.h>

#include <iostream>
#include <limits>

using namespace std;

void printMenu()
{
    cout << "\n=====================================\n";
    cout << "   constantPP-KNN Test Console\n";
    cout << "=====================================\n";

    cout << "1) TABLE V - Plaintext baseline\n";
    cout << "2) TABLE V - Ours (Sequential)\n";
    cout << "3) TABLE V - Ours (Parallel)\n";
    cout << "4) Fig.3a - Offline (Sequential)\n";
    cout << "5) Fig.3a - Offline (Parallel)\n";
    cout << "6) Fig.3b - Offline (Sequential)\n";
    cout << "7) Fig.3b - Offline (Parallel)\n";
    cout << "8) Fig.4a - Latency (Sequential)\n";
    cout << "9) Fig.4a - Latency (Parallel)\n";
    cout << "10) Fig.4b - Latency (Sequential)\n";
    cout << "11) Fig.4b - Latency (Parallel)\n";
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
            testPlainAcc();
            break;

        case 2:
            cout << "[TABLE V - Ours (Sequential)]\n";
            testConstKNNAcc(0);
            break;

        case 3:
            cout << "[TABLE V - Ours (Parallel)]\n";
            testConstKNNAcc(1);
            break;

        /* Fig.3 */
        case 4:
            cout << "[Fig.3a - Offline (Sequential)]\n";
            test_offline_cost(0, 0);
            break;

        case 5:
            cout << "[Fig.3a - Offline (Parallel)]\n";
            test_offline_cost(0, 1);
            break;

        case 6:
            cout << "[Fig.3b - Offline (Sequential)]\n";
            test_offline_cost(1, 0);
            break;

        case 7:
            cout << "[Fig.3b - Offline (Parallel)]\n";
            test_offline_cost(1, 1);
            break;

        /* Fig.4 */
        case 8:
            cout << "[Fig.4a - Latency (Sequential)]\n";
            testLaten(0, 0);
            break;

        case 9:
            cout << "[Fig.4a - Latency (Parallel)]\n";
            testLaten(0, 1);
            break;

        case 10:
            cout << "[Fig.4b - Latency (Sequential)]\n";
            testLaten(1, 0);
            break;

        case 11:
            cout << "[Fig.4b - Latency (Parallel)]\n";
            testLaten(1, 1);
            break;

        default:
            cout << "\nInvalid option.\n";
        }
    }

    return 0;
}