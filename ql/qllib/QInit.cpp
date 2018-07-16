#include "qllib/QInit.h"


QInit::QInit()
{
}


QInit::~QInit()
{
}

void QInit::initQ(vector<float>* qHodnoty)
{
    for (int i = 0; i < qHodnoty->size(); i++)
    {
        (*qHodnoty)[i] = 0.0;
    }
}
