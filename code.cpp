#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <fstream>

#define MAX_WEIGHT 10
#define MIN_WEIGHT 1
#define MAX_PRICE 10
#define MIN_PRICE 1

using namespace std;
using namespace chrono;
//[price,weight]
class Backpack
{
private:
    void preconstruct(int iloscElementow, int rozmiar)
    {
        backpackCap = rozmiar;
        numberOfItems = iloscElementow;
        storage = new int *[iloscElementow];
    }

public:
    int **storage, backpackCap, numberOfItems;
    Backpack(int iloscElementow, int rozmiar)
    {
        preconstruct(iloscElementow, rozmiar);
        srand(time(NULL));
        for (int n = 0; n < iloscElementow; n++)
        {
            int weight = MIN_WEIGHT + rand() % MAX_WEIGHT;
            int price = MIN_PRICE + rand() % MAX_PRICE;
            storage[n] = new int[2];
            storage[n][0] = price;
            storage[n][1] = weight;
        }
    }
    Backpack(string fileName)
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Can't acces file " << fileName << endl;
            return;
        }
        int iloscElementow;
        file >> iloscElementow >> backpackCap;
        preconstruct(iloscElementow, backpackCap);
        for (int n = 0; n < iloscElementow; n++)
        {
            int weight, price;
            file >> weight >> price;
            storage[n] = new int[2];
            storage[n][0] = price;
            storage[n][1] = weight;
        }
    }
};

void quickSort(int **arr, int start, int end)
{
    if (start >= end)
        return;
    float pivot = arr[start][0] / arr[start][1];
    int count = 0;
    for (int n = start + 1; n <= end; n++)
    {
        if (arr[n][0] / arr[n][1] > pivot)
        {
            count++;
        }
    }
    int idx = start + count;
    swap(arr[idx], arr[start]);
    int n = start, m = end;
    while (n < idx && m > idx)
    {
        while (arr[n][0] / arr[n][1] > pivot)
        {
            n++;
        }
        while (arr[m][0] / arr[m][1] <= pivot)
        {
            m--;
        }
        if (n < idx && m > idx)
        {
            swap(arr[n++], arr[m--]);
        }
    }
    quickSort(arr, start, idx - 1);
    quickSort(arr, idx + 1, end);
}
void printPouch(vector<int> pouch)
{
    cout << "plecak: {";
    for (int n = 0; n < pouch.size(); n++)
    {
        cout << pouch[n] << ((n + 1) != pouch.size() ? ", " : "");
    }
    cout << "}" << endl;
}

// brute force functions
void printMask(Backpack bp, int mask)
{
    cout << "plecak: {";
    for (int n = 0; n < bp.numberOfItems; n++)
    {
        if ((mask & (1 << n)) != 0)
        {
            cout << (n + 1) << (n != bp.numberOfItems - 1 ? ", " : "");
        }
    }
    cout << "}" << endl;
}
int *sumMask(Backpack bp, int mask)
{
    int *collection = new int[2]; //[price,weight]
    collection[0] = 0;
    collection[1] = 0;
    for (int n = 0; n < bp.numberOfItems; n++)
    {
        if ((mask & (1 << n)) != 0)
        {
            collection[0] += bp.storage[n][0];
            collection[1] += bp.storage[n][1];
        }
    }
    return collection;
}
int *bruteForce(Backpack bp)
{
    int *fmax = new int[3]; //[price, weight, mask]
    fmax[0] = -1;
    int maxmask = pow(2, bp.numberOfItems);
    for (int n = 1; n < maxmask; n++)
    {
        int *sum = sumMask(bp, n);
        if (sum[1] <= bp.backpackCap && sum[0] > fmax[0])
        {
            fmax[0] = sum[0];
            fmax[1] = sum[1];
            fmax[2] = n;
        }
    }
    return fmax;
}

// greedy functions
int **copyStorage(Backpack bp)
{
    int **newStorage = new int *[bp.numberOfItems]; //[price,weight,originalIndex]
    for (int n = 0; n < bp.numberOfItems; n++)
    {
        newStorage[n] = new int[3];
        newStorage[n][0] = bp.storage[n][0];
        newStorage[n][1] = bp.storage[n][1];
        newStorage[n][2] = n;
    }
    return newStorage;
}
int *greedy(Backpack bp, vector<int> &pouch)
{
    int *out = new int[2]; //[price, weight]
    out[0] = 0;
    out[1] = 0;
    int **storage = copyStorage(bp);
    quickSort(storage, 0, bp.numberOfItems - 1);
    for (int n = 0; n < bp.numberOfItems && out[1] < bp.backpackCap; n++)
    {
        if (storage[n][1] + out[1] <= bp.backpackCap)
        {
            pouch.push_back(storage[n][2] + 1);
            out[1] += storage[n][1];
            out[0] += storage[n][0];
        }
    }
    return out;
}

// dynamic functions
void printMatrix(Backpack bp, int **matrix)
{
    for (int n = 0; n < bp.numberOfItems + 1; n++)
    {
        for (int m = 0; m < bp.backpackCap + 1; m++)
        {
            cout << matrix[n][m] << "  ";
        }
        cout << endl;
    }
}
int *readMatrix(Backpack bp, int **matrix, vector<int> &pouch)
{
    int *out = new int[2]; //[price, weight]
    out[0] = matrix[bp.numberOfItems][bp.backpackCap];
    out[1] = 0;
    int n = bp.numberOfItems;
    int m = bp.backpackCap;
    while (matrix[n][m] != 0)
    {
        if (matrix[n - 1][m] != matrix[n][m])
        {
            pouch.push_back(n);
            out[1] += bp.storage[n - 1][1];
            m -= bp.storage[n - 1][1];
        }
        n--;
    }
    return out;
}
int **dynamicMatrix(Backpack bp)
{
    int **matrix = new int *[bp.numberOfItems + 1];
    for (int n = 0; n < bp.numberOfItems + 1; n++)
    {
        matrix[n] = new int[bp.backpackCap + 1];
        for (int m = 0; m < bp.backpackCap + 1; m++)
        {
            matrix[n][m] = 0;
        }
    }
    for (int n = 0; n < bp.numberOfItems; n++)
    {
        for (int m = 0; m < bp.backpackCap + 1; m++)
        {
            if (m - bp.storage[n][1] >= 0)
                matrix[n + 1][m] = max(matrix[n][m - bp.storage[n][1]] + bp.storage[n][0], matrix[n][m]);
            else
                matrix[n + 1][m] = matrix[n][m];
        }
    }
    return matrix;
}

// tests
void test()
{
    ofstream file("output.txt");
    // Wykonaj dla każdego algorytmu wykres t=f(n) zależności czasu obliczeń t od liczby n przedmiotów, przy stałej pojemności plecaka b.
    file << "\nPart 1\n";

    file << "brute: ";
    for (int n = 8; n <= 26; n += 2)
    {
        // brute n>30 => inf time
        Backpack bp(n, 12);
        auto start = high_resolution_clock::now();
        int *out = bruteForce(bp);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }
    file << "\ngreed: ";
    for (int n = 100; n <= 1000; n += 100)
    {
        Backpack bp(n, 200);
        vector<int> pouch;
        auto start = high_resolution_clock::now();
        greedy(bp, pouch);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }
    file << "\ndynam: ";
    for (int n = 100; n <= 1000; n += 100)
    {
        Backpack bp(n, 200);
        vector<int> pouch;
        auto start = high_resolution_clock::now();
        readMatrix(bp, dynamicMatrix(bp), pouch);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }

    // Wykonaj dla każdego algorytmu wykres t=f(b) zależności czasu obliczeń t od pojemności plecaka b, przy stałej liczbie przedmiotów n.
    file << "\nPart 3\n";

    file << "brute: ";
    for (int n = 8; n <= 26; n += 2)
    {
        // brute n>30 => inf time
        Backpack bp(16, n);
        auto start = high_resolution_clock::now();
        int *out = bruteForce(bp);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }
    file << "\ngreed: ";
    for (int n = 100; n <= 1000; n += 100)
    {
        Backpack bp(500, n);
        vector<int> pouch;
        auto start = high_resolution_clock::now();
        greedy(bp, pouch);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }
    file << "\ndynam: ";
    for (int n = 100; n <= 1000; n += 100)
    {
        Backpack bp(500, n);
        vector<int> pouch;
        auto start = high_resolution_clock::now();
        readMatrix(bp, dynamicMatrix(bp), pouch);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        file << "(" << n << "," << duration.count() << ")";
    }

    // Wykonaj dla każdego algorytmu wykres t=f(n,b) zależności czasu obliczeń t od liczby n przedmiotów i pojemności plecaka b.
    file << "\nPart 4\n";

    for (int m = 10; m <= 20; m++)
    {
        file << endl
             << m << " brute: ";
        for (int n = 10; n <= 20; n++)
        {
            // brute n>30 => inf time
            Backpack bp(n, m);
            auto start = high_resolution_clock::now();
            int *out = bruteForce(bp);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            file << "(" << n << "," << duration.count() << ")";
        }
    }
    for (int m = 100; m <= 1000; m += 100)
    {
        file << endl
             << m << " greed: ";
        for (int n = 100; n <= 1000; n += 100)
        {
            Backpack bp(n, m);
            vector<int> pouch;
            auto start = high_resolution_clock::now();
            greedy(bp, pouch);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            file << "(" << n << "," << duration.count() << ")";
        }
    }
    for (int m = 100; m <= 1000; m += 100)
    {
        file << endl
             << m << " dynam: ";
        for (int n = 100; n <= 1000; n += 100)
        {
            Backpack bp(n, m);
            vector<int> pouch;
            auto start = high_resolution_clock::now();
            readMatrix(bp, dynamicMatrix(bp), pouch);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            file << "(" << n << "," << duration.count() << ")";
        }
    }
    file.close();
}

int main()
{
    test();

    // Backpack bp("test.txt");
    // int backpackSize = 10, noElements = 24;
    // Backpack bp(noElements, backpackSize);

    // cout << "brute force\n";

    // int *brute = bruteForce(bp);
    // cout << "price: " << brute[0] << "\nweight: " << brute[1] << endl;
    // printMask(bp, brute[2]);

    // cout << "\ngreedy\n";

    // vector<int> greedypouch;
    // int *greed = greedy(bp, greedypouch);
    // cout << "price: " << greed[0] << "\nweight: " << greed[1] << endl;
    // printPouch(greedypouch);

    // cout << "\ndynamic programming\n";

    // vector<int> dynamicpouch;
    // int **out = dynamicMatrix(bp);
    // printMatrix(bp, out);
    // int *dyn = readMatrix(bp, out, dynamicpouch);
    // cout << "price: " << dyn[0] << "\nweight: " << dyn[1] << endl;
    // printPouch(dynamicpouch);

    return 0;
}