#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Cat
{
public:
    Cat(string name, int tail) : Name(name), TailLen(tail) {}

    bool operator<(const Cat &c) const
    {
        return TailLen < c.TailLen;
    }

    string Name;
    int TailLen;
};

template <typename T>
void BubbleSort(vector<T> &vec)
{
    for (size_t i = 0; i < vec.size(); i++)
    {
        for (size_t j = i + 1; j < vec.size(); j++)
        {
            if (vec[i] > vec[j])
            {
                swap(vec[i], vec[j]);
            }
        }
    }
}

template <typename T>
void SelectionSort(vector<T> &vec)
{
    int index = 0;
    // 使用stl迭代器找小元素的函式
    auto minIter = min_element(vec.begin(), vec.end());

    while (index < vec.size())
    {
        // 找迭代器實際的index
        int minIndex = distance(vec.begin(), minIter);
        if (minIndex != index)
        {
            swap(vec[index], *minIter);
        }

        index++;
        minIter = min_element(vec.begin() + index, vec.end());
    }
}

template <typename T>
void Merge(vector<T> &vec, int left, int right, int mid)
{
    int nLeft = mid - left + 1;
    int nRight = right - mid;

    vector<T> vecLeft(nLeft);
    vector<T> vecRight(nRight);

    for (int i = 0; i < nLeft; i++)
    {
        vecLeft[i] = vec[left + i];
    }

    for (int i = 0; i < nRight; i++)
    {
        vecRight[i] = vec[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;
    while (i < nLeft || j < nRight)
    {
        if (i >= nLeft)
        {
            vec[k++] = vecRight[j++];
            continue;
        }

        if (j >= nRight)
        {
            vec[k++] = vecLeft[i++];
            continue;
        }
        
        if (vecLeft[i] < vecRight[j])
        {
            vec[k++] = vecLeft[i++];
        }
        else
        {
            vec[k++] = vecRight[j++];
        }
    }
}

template <typename T>
void MergeSort(vector<T> &vec, int left, int right)
{
    if (left < right)
    {
        int mid = (left + right) / 2;
        MergeSort(vec, left, mid);
        MergeSort(vec, mid + 1, right);
        Merge(vec, left, right, mid);
    }
}

template <typename T>
int Partition(vector<T> &vec, int left, int right)
{
    int pivot = vec[right];
    int i = left - 1;
    for (int j = left; j < right; j++)
    {
        if (vec[j] < pivot)
        {
            i++;
            swap(vec[i], vec[j]);
        }        
    }
    i++;
    swap(vec[i], vec[right]);
    return i;
}

template <typename T>
void QuickSort(vector<T> &vec, int left, int right)
{
    if (left < right)
    {
        int pivot = Partition(vec, left, right);
        QuickSort(vec, left, pivot - 1);
        QuickSort(vec, pivot + 1, right);
    }    
}

template <typename T>
void InsertSort(vector<T> &vec, bool useBinary = true)
{
    // 預設第一筆是排序的
    int index = 0;
    for (size_t i = 1; i < vec.size(); i++)
    {
        if (useBinary)
        {
            // 使用二分法
            int left = 0;
            int right = index;
            while (left <= right)
            {
                auto mid = (left + right) / 2;
                if (vec[i] < vec[mid])
                {
                    right = mid - 1;
                }
                else
                {
                    left = mid + 1;
                }
            }
            T value = vec[i];
            vec.erase(vec.begin() + i);
            vec.insert(vec.begin() + left, value);
        }
        else
        {
            // 直接逐個比較
            int tmp = index;
            bool isFirst = false;
            while (tmp >= 0)
            {
                if (vec[i] < vec[tmp])
                {
                    int value = vec[i];
                    vec.erase(vec.begin() + i);
                    vec.insert(vec.begin() + i, value);

                    isFirst = true;
                    break;
                }

                tmp--;
            }

            if (isFirst)
            {
                int value = vec[i];
                vec.erase(vec.begin() + i);
                vec.insert(vec.begin(), value);
            }
        }

        index++;
    }
}

template <typename T>
void Heapify(vector<T> &vec, int N, int i)
{
    // root
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < N && vec[left] > vec[largest])
    {
        largest = left;
    }

    if (right < N && vec[right] > vec[largest])
    {
        largest = right;
    }

    if (largest != i)
    {
        swap(vec[i], vec[largest]);

        Heapify(vec, N, largest);
    }    
}

template <typename T>
void HeapSort(vector<T> &vec, int N)
{
    for (int i = N / 2 - 1; i >= 0; i--)
    {
        Heapify(vec, N, i);
    }
    for (int i = N - 1; i > 0; i--)
    {
        swap(vec[0], vec[i]);

        Heapify(vec, i, 0);
    }    
}

int main(int argc, char const *argv[])
{
    vector<int> vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    // BubbleSort(vec);
    // cout << "BubbleSort: " << endl;
    // for (auto &v : vec)
    // {
    //     cout << v << endl;
    // }

    // vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    // InsertSort(vec);
    // cout << "InsertSort: " << endl;
    // for (auto &v : vec)
    // {
    //     cout << v << endl;
    // }

    // vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    // SelectionSort(vec);
    // cout << "SelectionSort: " << endl;
    // for (auto &v : vec)
    // {
    //     cout << v << endl;
    // }

    // vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    // MergeSort(vec, 0, vec.size() -1);
    // cout << "MergeSort: " << endl;
    // for (auto &v : vec)
    // {
    //     cout << v << endl;
    // }

    vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    QuickSort(vec, 0, vec.size() -1);
    cout << "QuickSort: " << endl;
    for (auto &v : vec)
    {
        cout << v << endl;
    }

    vec = {9, 5, 88, 64, 1, 58, 15, 13, 77, 51, 58};
    HeapSort(vec, vec.size());
    cout << "HeapSort: " << endl;
    for (auto &v : vec)
    {
        cout << v << endl;
    }

    /* vector<Cat> catVec = {Cat("meme", 95), Cat("haha", 15), Cat("kiki", 5), Cat("fafa", 195)};
    BubbleSort(catVec);
    for (auto &cat : catVec)
    {
        cout << cat.Name << " " << cat.TailLen << endl;
    } */

    return 0;
}
