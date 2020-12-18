#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>
#include <mutex>
#include <queue>

using namespace std;

typedef struct Log
{
    int tape;
    int sort;
    char status;
    timeval time;
} Log;

timeval t;

vector<Log> res;

mutex m, m1, m2, m3;

typedef vector<int> Array;
typedef vector<thread> Thread;
std::queue<int> que1;
std::queue<int> que2;
std::queue<int> que3;

Array a;
Array b;
Array c;
int len_mas = 10000;

void generate_massive(Array &a, Array &b, Array &c, int n)
{
    a.resize(n);
    b.resize(n);
    c.resize(n);
    for (int j = 0; j < n; j++)
    {
        a[j] = rand();
        b[j] = a[j];
        c[j] = a[j];
    }
}

void shekerSort(Array mass, int count)
{
    int left = 0, right = count - 1; // левая и правая границы сортируемой области массива
    int flag = 1;                    // флаг наличия перемещений
    // Выполнение цикла пока левая граница не сомкнётся с правой
    // и пока в массиве имеются перемещения
    while ((left < right) && flag > 0)
    {
        flag = 0;
        for (int i = left; i < right; i++) //двигаемся слева направо
        {
            if (mass[i] > mass[i + 1]) // если следующий элемент меньше текущего,
            {                          // меняем их местами
                double t = mass[i];
                mass[i] = mass[i + 1];
                mass[i + 1] = t;
                flag = 1; // перемещения в этом цикле были
            }
        }
        right--;                           // сдвигаем правую границу на предыдущий элемент
        for (int i = right; i > left; i--) //двигаемся справа налево
        {
            if (mass[i - 1] > mass[i]) // если предыдущий элемент больше текущего,
            {                          // меняем их местами
                double t = mass[i];
                mass[i] = mass[i - 1];
                mass[i - 1] = t;
                flag = 1; // перемещения в этом цикле были
            }
        }
        left++; // сдвигаем левую границу на следующий элемент
    }
}

void bubbleSort(Array num, int size)
{
    // Для всех элементов
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = (size - 1); j > i; j--) // для всех элементов после i-ого
        {
            if (num[j - 1] > num[j]) // если текущий элемент меньше предыдущего
            {
                int temp = num[j - 1]; // меняем их местами
                num[j - 1] = num[j];
                num[j] = temp;
            }
        }
    }
}

void bubbleSortFg(Array a, int size)
{
    int i;
    int tmp;
    char flag;
    do
    {
        flag = 0;
        for (i = 1; i < size; i++)
        {
            if (a[i] < a[i - 1])
            {
                tmp = a[i];
                a[i] = a[i - 1];
                a[i - 1] = tmp;
                flag = 1;
            }
        }
    } while (flag);
}

// Функция сортировки прямыми включениями
void inclusionSort(Array num, int size)
{
    // Для всех элементов кроме начального
    for (int i = 1; i < size; i++)
    {
        int value = num[i]; // запоминаем значение элемента
        int index = i;      // и его индекс
        while ((index > 0) && (num[index - 1] > value))
        { // смещаем другие элементы к концу массива пока они меньше index
            num[index] = num[index - 1];
            index--; // смещаем просмотр к началу массива
        }
        num[index] = value; // рассматриваемый элемент помещаем на освободившееся место
    }
}

// Функция сортировки прямым выбором
void selectionSort(Array num, int size)
{
    int min, temp; // для поиска минимального элемента и для обмена
    for (int i = 0; i < size - 1; i++)
    {
        min = i; // запоминаем индекс текущего элемента
        // ищем минимальный элемент чтобы поместить на место i-ого
        for (int j = i + 1; j < size; j++) // для остальных элементов после i-ого
        {
            if (num[j] < num[min]) // если элемент меньше минимального,
                min = j;           // запоминаем его индекс в min
        }
        temp = num[i]; // меняем местами i-ый и минимальный элементы
        num[i] = num[min];
        num[min] = temp;
    }
}

// Функция быстрой сортировки
void quickSort(Array numbers, int left, int right)
{
    int pivot;          // разрешающий элемент
    int l_hold = left;  //левая граница
    int r_hold = right; // правая граница
    pivot = numbers[left];
    while (left < right) // пока границы не сомкнутся
    {
        while ((numbers[right] >= pivot) && (left < right))
            right--;       // сдвигаем правую границу пока элемент [right] больше [pivot]
        if (left != right) // если границы не сомкнулись
        {
            numbers[left] = numbers[right]; // перемещаем элемент [right] на место разрешающего
            left++;                         // сдвигаем левую границу вправо
        }
        while ((numbers[left] <= pivot) && (left < right))
            left++;        // сдвигаем левую границу пока элемент [left] меньше [pivot]
        if (left != right) // если границы не сомкнулись
        {
            numbers[right] = numbers[left]; // перемещаем элемент [left] на место [right]
            right--;                        // сдвигаем правую границу вправо
        }
    }
    numbers[left] = pivot; // ставим разрешающий элемент на место
    pivot = left;
    left = l_hold;
    right = r_hold;
    if (left < pivot) // Рекурсивно вызываем сортировку для левой и правой части массива
        quickSort(numbers, left, pivot - 1);
    if (right > pivot)
        quickSort(numbers, pivot + 1, right);

}


void CurrentSort(int a,Array mas, int count)
{
    switch (a)
    {
    case 1:
        shekerSort(mas, count);
    case 2:
        bubbleSort(mas, count);
    case 3:
        bubbleSortFg(mas, count);
    case 4:
        inclusionSort(mas, count);
    case 5:
        selectionSort(mas, count);
    }
}

void way1()
{
    while (true)
    {
        m1.lock();
        if (!que1.empty())
        {
            int taskN = que1.front();
            
            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({1, taskN, 'S', t});
            m.unlock();

            m.lock();
            CurrentSort(taskN, a, len_mas);
            m.unlock();

            que1.pop();

            m2.lock();
            que2.push(taskN);
            m2.unlock();

            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({1, taskN, 'F', t});
            m.unlock();

            if (taskN == 3)
                return;
        }
        m1.unlock();
    }
}

void way2()
{
    while (true)
    {
        m2.lock();
        if (!que2.empty())
        {
            int taskN = que2.front();

            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({2, taskN, 'S', t});
            m.unlock();

            m.lock();
            CurrentSort(taskN, b, len_mas);
            m.unlock();

            que2.pop();

            m3.lock();
            que3.push(taskN);
            m3.unlock();

            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({2, taskN, 'F', t});
            m.unlock();

            if (taskN == 3)
                return;
        }
        m2.unlock();
    }
}

void way3()
{
    while (true)
    {
        m3.lock();
        if (!que3.empty())
        {
            int taskN = que3.front();

            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({3, taskN, 'S', t});
            m.unlock();

            m.lock();
            CurrentSort(taskN, c, len_mas);
            m.unlock();

            que3.pop();

            m.lock();
            gettimeofday(&t, NULL);
            res.push_back({3, taskN, 'F', t});
            m.unlock();

            if (taskN == 3)
                return;
        }
        m3.unlock();
    }
}

void MeasureTime()
{
    double time = 0;
    clock_t start;
    clock_t end;

    generate_massive(a,b,c, len_mas);

    // void (*sortSh)(Array, int) = shekerSort;
    // void (*sortBu)(Array, int) = bubbleSort;
    // void (*sortBuFg)(Array, int) = bubbleSortFg;
    // void (*sortIn)(Array, int) = inclusionSort;
    // void (*sortSe)(Array, int) = selectionSort;
    // void (*sortQs)(Array, int, int) = quickSort;

    for (auto i = 0; i < 5; ++i)
        que1.push((i+1));

    // Thread threads;
    // threads.push_back(thread(way1));
    // threads.push_back(thread(way2));
    // threads.push_back(thread(way3));

    // for (auto &th : threads)
    //     th.join();

    // time = 0;
    // start = clock();
    // shekerSort(a, len_mas);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tShakerSort\t" << time / CLOCKS_PER_SEC << endl;

    // time = 0;
    // start = clock();
    // bubbleSort(a, len_mas);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tBubbleSort\t" << time / CLOCKS_PER_SEC << endl;

    // time = 0;
    // start = clock();
    // inclusionSort(a, len_mas);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tInclusionSort\t" << time / CLOCKS_PER_SEC << endl;

    // time = 0;
    // start = clock();
    // selectionSort(a, len_mas);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tSelectionSort\t" << time / CLOCKS_PER_SEC << endl;

    // time = 0;
    // start = clock();
    // bubbleSortFg(a, len_mas);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tbubbleSortFg\t" << time / CLOCKS_PER_SEC << endl;

    // time = 0;
    // start = clock();
    // quickSort(a,0,len_mas-1);
    // end = clock();
    // time += (double)(end - start);
    // cout << "\tQuickSort\t" << time / CLOCKS_PER_SEC << endl;
}


int main()
{
    setlocale(LC_ALL, "ru");
    MeasureTime();

    // for (auto &re : res)
    //     cout << re.tape << ' ' << re.sort << ' ' << re.status << ' ' << (re.time.tv_sec * 1000) + (re.time.tv_usec / 1000) << '\n';
}