#include "qllib/QInitZealot.h"


QInitZealot::QInitZealot()
{
}


QInitZealot::~QInitZealot()
{
}

void QInitZealot::initQ(vector<float>* qHodnoty)
{
    /*
    auto pom = *qHodnoty;
    pom[0] = 0.9;
    pom[1] = 0.0; 
    pom[2] = 0.1;
    */
    /*
    (*qHodnoty)[0] = 0.0;
    (*qHodnoty)[1] = 0.0;
    (*qHodnoty)[2] = 0.0;
    */

    //zacalo sa to pouzivat univerzalne tak to je univerzalne teraz
    for (size_t i = 0; i < qHodnoty->size(); i++)
    {
        (*qHodnoty)[i] = 0.0;
    }

}
