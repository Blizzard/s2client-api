#pragma once
#include <vector>
#include <map>
using namespace std;

class Stav
{
private:
	vector<int>* stav;// nie q hodnoty ale stav!

public:
	

	Stav(vector<int>* st);
	~Stav();
	//virtual void test() = 0;
	bool operator<(const Stav);
	bool operator()(const Stav&, const Stav&) const;
	vector<int>* dajStavConst();
	const vector<int>* dajStavConst() const;
	string toString();
    string toCSV() const;
    Stav* Clone();
};
/*
struct CompareStav
{
public:
	bool operator() (const Stav& lhs, const Stav& rhs);
};
*/



struct CompareStav
{
	bool operator() (const Stav* stav1, const Stav* stav2) const
	{
		vector<int> pst1 = *stav1->dajStavConst();
		const vector<int> pst2 = *stav2->dajStavConst();
		if (pst1.size() != pst2.size()) {
            printf("Stavy sa nezhoduju v pocte dimenzii.");//bo vinimka dajako neukazuje text abo co 
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

	bool operator() (const Stav& stav1, const Stav& stav2) const
	{
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
};