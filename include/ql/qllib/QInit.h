#pragma once
#include <vector>
using namespace std;

class QInit
{
public:
    QInit();
    ~QInit();
    virtual void initQ(vector<float>* qHodnoty);
};

