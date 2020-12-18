#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

typedef vector<vector<int>> Matrix;
typedef vector<int> Array;
typedef vector<thread> Thread;

Matrix MultiplicationVinograd(Matrix a, Matrix b)
{
    int n = a.size();
    int ns = a[0].size();

    int m = b.size();
    int ms = b[0].size();

    Matrix mn(n);
    Array mulH(n, 0);
    Array mulV(ms, 0);

    Thread threads;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < ms; j++)
            mn[i].push_back(0);

    int modns = ns % 2;
    int modm = m % 2;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < ns - modns; j += 2)
            mulH[i] = mulH[i] + a[i][j] * a[i][j + 1];

    for (int i = 0; i < ms; i++)
        for (int j = 0; j < m - modm; j += 2)
            mulV[i] = mulV[i] + b[j][i] * b[j + 1][i];

    int minam = ns - 1;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < ms; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < ns / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modns)
                buff = buff + a[i][minam] * b[minam][j];
            mn[i][j] = buff;
        }

    return mn;
}

void threadMulH(Array &mulH, Matrix a, int n, int ns, int modns)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < ns - modns; j += 2)
            mulH[i] = mulH[i] + a[i][j] * a[i][j + 1];
}

void threadMulV(Array &mulV, Matrix b, int ms, int m, int modm)
{
    for (int i = 0; i < ms; i++)
        for (int j = 0; j < m - modm; j += 2)
            mulV[i] = mulV[i] + b[j][i] * b[j + 1][i];
}

Matrix ParallelMultiplicationVinograd(Matrix a, Matrix b)
{
    int n = a.size();
    int ns = a[0].size();

    int m = b.size();
    int ms = b[0].size();

    Matrix mn(n);
    Array mulH(n, 0);
    Array mulV(ms, 0);

    Thread threads;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < ms; j++)
            mn[i].push_back(0);

    int modns = ns % 2;
    int modm = m % 2;

    thread thread_MulH(threadMulH, ref(mulH), a, n, ns, modns);
    thread thread_MulV(threadMulV, ref(mulV), b, ms, m, modm);

    thread_MulH.join();
    thread_MulV.join();

    int minam = ns - 1;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < ms; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < ns / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modns)
                buff = buff + a[i][minam] * b[minam][j];
            mn[i][j] = buff;
        }

    return mn;
}

void first(Array mulV, Array mulH, Matrix a, Matrix b, Matrix &mn, int start, int end, int ns, int ms, int modns)
{
    int minam = ns - 1;
    for (int i = start; i < end; i++)
        for (int j = 0; j < ms; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < ns / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modns)
                buff = buff + a[i][minam] * b[minam][j];
            mn[i][j] = buff;
        }
}

Matrix ParallelMultiplicationVinogradVer2(Matrix a, Matrix b, int thr)
{
    int n = a.size();
    int ns = a[0].size();

    int m = b.size();
    int ms = b[0].size();

    Matrix mn(n);
    Array mulH(n, 0);
    Array mulV(ms, 0);

    Thread threads;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < ms; j++)
            mn[i].push_back(0);

    int modns = ns % 2;
    int modm = m % 2;

    std::thread thread_MulH(threadMulH, ref(mulH), a, n, ns, modns);
    std::thread thread_MulV(threadMulV, ref(mulV), b, ms, m, modm);

    thread_MulH.join();
    thread_MulV.join();

    for (int k = 1; k <= thr; k++)
        threads.push_back(thread(first, mulV, mulH, a, b, ref(mn), (k - 1) * n / thr, k * n / thr, ns, ms, modns));

    for (auto &th : threads)
        th.join();

    return mn;
}

void generate_matrix(Matrix &a, int n)
{
    a.resize(n);

    for (int i = 0; i < n; i++)
        a[i].resize(n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = rand();
}

void MeasureTime()
{
    double time = 0;
    int n = 10;
    int len_matrix = 1000;
    clock_t start;
    clock_t end;
    Matrix a;
    Matrix b;

    for (int k = 100; k <= len_matrix; k += 100)
    {
        printf("%d:\n", k);

        //Matrix().swap(a);
        //Matrix().swap(b);

        generate_matrix(a, k);
        generate_matrix(b, k);

        for (int i = 1; i <= 16; i++)
        {
            printf("%d ", i);
            time = 0;
            for (int j = 0; j < n; j++)
            {
                start = clock();
                ParallelMultiplicationVinogradVer2(a, b, i);
                end = clock();
                time += (double)(end - start);
            }
            cout << "\tParVInVer2\t" << time / n / CLOCKS_PER_SEC << endl;
        }

        time = 0;
        for (int j = 0; j < n; j++)
        {
            start = clock();
            ParallelMultiplicationVinograd(a, b);
            end = clock();
            time += (double)(end - start);
        }
        cout << "\tParVIn\t" << time / n / CLOCKS_PER_SEC << endl;

        time = 0;
        for (int j = 0; j < n; j++)
        {
            start = clock();
            MultiplicationVinograd(a, b);
            end = clock();
            time += (double)(end - start);
        }
        cout << "\tVIn\t" << time / n / CLOCKS_PER_SEC << endl;
    }
}

int main()
{

    MeasureTime();

    return 0;
}
