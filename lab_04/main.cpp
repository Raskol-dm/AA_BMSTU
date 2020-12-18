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
    int an = a.size();
    int am = a[0].size();

    int bn = b.size();
    int bm = b[0].size();

    Matrix c;
    Array mulH;
    Array mulV;

    for (int i = 0; i < an; i++)
        mulH.push_back(0);

    for (int i = 0; i < bm; i++)
        mulV.push_back(0);

    c.resize(an);

    for (int i = 0; i < an; i++)
        for (int j = 0; j < bm; j++)
            c[i].push_back(0);

    int modam = am % 2;
    int modbn = bn % 2;

    for (int i = 0; i < an; i++)
        for (int j = 0; j < am - modam; j += 2)
            mulH[i] = mulH[i] + a[i][j] * a[i][j + 1];

    for (int i = 0; i < bm; i++)
        for (int j = 0; j < bn - modbn; j += 2)
            mulV[i] = mulV[i] + b[j][i] * b[j + 1][i];

    int minam = am - 1;
    for (int i = 0; i < an; i++)
        for (int j = 0; j < bm; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < am / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modam)
                buff = buff + a[i][minam] * b[minam][j];
            c[i][j] = buff;
        }

    return c;
}

void threadMulH(Array &mulH, Matrix a, int an, int am, int modam)
{
    for (int i = 0; i < an; i++)
        for (int j = 0; j < am - modam; j += 2)
            mulH[i] = mulH[i] + a[i][j] * a[i][j + 1];
}

void threadMulV(Array &mulV, Matrix b, int bm, int bn, int modbn)
{
    for (int i = 0; i < bm; i++)
        for (int j = 0; j < bn - modbn; j += 2)
            mulV[i] = mulV[i] + b[j][i] * b[j + 1][i];
}

Matrix ParallelMultiplicationVinograd(Matrix a, Matrix b)
{
    int an = a.size();
    int am = a[0].size();

    int bn = b.size();
    int bm = b[0].size();

    Matrix c;
    Array mulH;
    Array mulV;

    for (int i = 0; i < an; i++)
        mulH.push_back(0);

    for (int i = 0; i < bm; i++)
        mulV.push_back(0);

    c.resize(an);

    for (int i = 0; i < an; i++)
        for (int j = 0; j < bm; j++)
            c[i].push_back(0);

    int modam = am % 2;
    int modbn = bn % 2;

    thread thread_MulH(threadMulH, ref(mulH), a, an, am, modam);
    thread thread_MulV(threadMulV, ref(mulV), b, bm, bn, modbn);

    thread_MulH.join();
    thread_MulV.join();

    int minam = am - 1;
    for (int i = 0; i < an; i++)
        for (int j = 0; j < bm; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < am / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modam)
                buff = buff + a[i][minam] * b[minam][j];
            c[i][j] = buff;
        }

    return c;
}

void first(Array mulV, Array mulH, Matrix a, Matrix b, Matrix &c, int start, int end, int am, int bm, int modam)
{
    int minam = am - 1;
    for (int i = start; i < end; i++)
        for (int j = 0; j < bm; j++)
        {
            int buff = -mulH[i] - mulV[j];
            for (int k = 0; k < am / 2; k++)
                buff = buff + (a[i][2 * k + 1] + b[2 * k][j]) * (a[i][2 * k] + b[2 * k + 1][j]);
            if (modam)
                buff = buff + a[i][minam] * b[minam][j];
            c[i][j] = buff;
        }
}

Matrix ParallelMultiplicationVinogradVer2(Matrix a, Matrix b, int n_Th)
{
    int an = a.size();
    int am = a[0].size();

    int bn = b.size();
    int bm = b[0].size();

    Matrix c;
    Array mulH;
    Array mulV;

    Thread threads;

    for (int i = 0; i < an; i++)
        mulH.push_back(0);

    for (int i = 0; i < bm; i++)
        mulV.push_back(0);

    c.resize(an);

    for (int i = 0; i < an; i++)
        for (int j = 0; j < bm; j++)
            c[i].push_back(0);

    int modam = am % 2;
    int modbn = bn % 2;

    thread thread_MulH(threadMulH, ref(mulH), a, an, am, modam);
    thread thread_MulV(threadMulV, ref(mulV), b, bm, bn, modbn);

    thread_MulH.join();
    thread_MulV.join();

    for (int k = 1; k <= n_Th; k++)
        threads.push_back(thread(first, mulV, mulH, a, b, ref(c), (k - 1) * an / n_Th, k * an / n_Th, am, bm, modam));

    for (auto &th : threads)
        th.join();

    return c;
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
    int n = 1;
    int len_matrix = 2000;
    clock_t start;
    clock_t end;
    Matrix a;
    Matrix b;

    for (int k = 100; k <= len_matrix; k += 100)
    {
        printf("%d:\n", k);

        Matrix().swap(a);
        Matrix().swap(b);

        generate_matrix(a, k);
        generate_matrix(b, k);

        for (int i = 2; i <= 8; i += 2)
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
        start = clock();
        ParallelMultiplicationVinograd(a, b);
        end = clock();
        time += (double)(end - start);
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
