#include "qllib/Stav.h"
#include <vector>
#include <string>
using namespace std;



Stav::Stav(vector<int>* st)
{
	stav = st;
}

vector<int>* Stav::dajStavConst() {
	return stav;
}


const vector<int>* Stav::dajStavConst() const
{
	return stav;
}

Stav::~Stav()
{
    if (stav != nullptr) {
        stav->~vector();
    }
}

string Stav::toCSV() const
{
    string pom;
    for (size_t i = 0; i < stav->size(); i++)
    {
        pom += std::to_string((*stav)[i]) + ",";
    }
    return pom;
}

Stav* Stav::Clone()
{
    return new Stav(new vector<int>(*stav));
}

bool Stav::operator< (const Stav stav2)
{
	vector<int> pst1 = *stav;
	const vector<int> pst2 = *stav2.dajStavConst();
	if (pst1.size() != pst2.size()) {
		throw std::invalid_argument("Stavy sa nezhoduju v pocte dimenzii.");
	}
	for (size_t i = 0; i < pst1.size(); i++)
	{
		if (pst1[i] == pst2[i]) {
			continue;
		}
		if (pst1[i] < pst2[i]) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool Stav::operator() (const Stav& stav1, const Stav& stav2) const {
	vector<int> pst1 = *stav1.dajStavConst();
	const vector<int> pst2 = *stav2.dajStavConst();
	if (pst1.size() != pst2.size()) {
		throw std::invalid_argument("Stavy sa nezhoduju v pocte dimenzii.");
	}
	for (size_t i = 0; i < pst1.size(); i++)
	{
		if (pst1[i] == pst2[i]) {
			continue;
		}
		if (pst1[i] < pst2[i]) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;

}


/*
bool operator<(const Stav& st2) const {
	vector<int> pst1 = Stav::stav;
	vector<int> pst2 = st2.dajStavConst;
	if (pst1.size != pst2.size) {
		throw std::invalid_argument("Stavy sa nezhoduju v pocte dimenzii.");
	}
	for (signed int i = 0; i < pst1.size; i++)
	{
		if (pst1[i] == pst2[i]) {
			continue;
		}
		if (pst1[i] < pst2[i]) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}
*/
