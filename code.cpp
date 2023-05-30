#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>

#define MAX_WEIGHT 10
#define MIN_WEIGHT 1
#define MAX_PRICE 10
#define MIN_PRICE 1

using namespace std;
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
    Backpack(int rozmiar, int iloscElementow)
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
        cout << pouch[n] << ", ";
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

int main()
{
    Backpack bp("test.txt");

    // int *brute = bruteForce(bp);
    // cout << "price: " << brute[0] << "\nweight: " << brute[1] << endl;
    // printMask(bp, brute[2]);

    // vector<int> greedypouch;
    // greedy(bp, greedypouch);
    // printPouch(greedypouch);

    vector<int> dynamicpouch;
    int **out = dynamicMatrix(bp);
    printMatrix(bp, out);
    int *dyn = readMatrix(bp, out, dynamicpouch);
    cout << "price: " << dyn[0] << "\nweight: " << dyn[1] << endl;
    printPouch(dynamicpouch);

    return 0;
}