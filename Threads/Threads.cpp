// Threads.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CustomThreadPool.h"

using namespace std;

void SquareArea(double& res, int& squareSide)
{
    res += squareSide * squareSide;
};

void RectangleArea(double& res, int& SideA, int& SideB)
{
    res += SideA * SideB;
};

void CircleArea(double& res, double& radius)
{
    res += 3.1415 * (radius * radius);
};

int main()
{
    // creating pool with 3 threads
    CustomThreadPool pool(3);

    int squareSide = 5;
    int RectSideA = 4, RectSideB = 3;
    double radius = 7;
    double res1 = 0, res2 = 0, res3 = 0;

    int taskId1 = pool.addTask(SquareArea, ref(res1), ref(squareSide));
    int taskId2 = pool.addTask(RectangleArea, ref(res2), ref(RectSideA), ref(RectSideB));
    int taskId3 = pool.addTask(CircleArea, ref(res3), ref(radius));

    // output result if task finished
    if (pool.IsTaskFinished(taskId1))
    {
        cout << "SquareArea = " << res1 << endl;
    }
    else //else wait for result
    {
        pool.Wait(taskId1);
        cout << "SquareArea = " << res1 << endl;
    }

    //waiting for all tasks complete and output result
    pool.WaitAll();
    cout << "RectangleArea = " << res2 << endl;
    cout << "CircleArea = " << res3 << endl;

    system("pause");
};


