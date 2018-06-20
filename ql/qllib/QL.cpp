#include "qllib/QL.h"
#include "qllib/Stav.h"
#include <iostream> 
#include <map>
#include <string>
#include <iterator>
#include <ctime> 
#include "qllib/QInit.h"
#include <fstream>
#include <sstream>

using namespace std;

Stav* zaciatocnyStav;
Stav* minulyStav;
int minulaAkcia;

int algaritmus = 1; //0 sarsa 1 q learning

int pocetAkcii;//2 stavy -> 0-1 .. magic numbers mozno prerobit na enum ale boh vie ako to tu vsetko funguje :D :D 
int pocetFeatur;

float GAMMA = 0.9;//zlava
float EPSILONx1000 = 0.75*1000;//kolko ide random
float ALPHA = 0.05;//learning rate
float zmenaALPHA = 1;// 0.9999;
float zmenaEPSILON = 1;// 0.999;
QInit* qInit = nullptr;




//QL::QStav stav;
std::map<Stav*, QL::QStav*, CompareStav>* stavy = new map <Stav*, QL::QStav*, CompareStav>();


QL::QL(Stav* stav, int pocFeatur, int pocAkcii,QInit* qin)
{

    qInit = qin;
    srand(time(NULL));
	minulyStav = stav->Clone();
    zaciatocnyStav = stav->Clone();
	QStav* qStav = new QStav(pocAkcii,qInit);

	stavy->insert(make_pair(stav->Clone(), qStav));

	pocetAkcii = pocAkcii;
    pocetFeatur = pocFeatur;
    pocetUceni = 0;
	
}


QL::~QL()
{
}

void ZmenAlgaritmus(int algarit) {
    algaritmus = algarit;
}

int QL::DajPocetUceni()
{
    return pocetUceni;
}

void QL::PremazMinulyStav() {
    minulyStav = zaciatocnyStav;
    minulaAkcia = 0;
}

void QL::SetHyperparemeters(float alpha, float gamma, float epsilon)
{
    ALPHA = alpha;
    GAMMA = gamma;
    EPSILONx1000 = epsilon * 1000;

}

//Nejako vybere akciu
// povodne parameter Stav* stav,  momentalne berie aktualny stav 
int QL::ChooseAction(bool vybratNajlepsie, Stav* stav)
{

    int nahoda = rand() % 1000;
	//zatial napevno potom ako parameter asi abo co 
	//mozno nejaka vazena nahoda podla hodnoty q
	if (nahoda < EPSILONx1000 && !vybratNajlepsie)
	{
		//nahodne
		return rand() % pocetAkcii;
	} else
	{
        //Vyberame najlepsiu
		auto pairr = stavy->find(stav);
        if (pairr == stavy->end()) {
            //Taky stav este nebol -> random akcia
            return rand() % pocetAkcii;
        }
        QL::QStav* qSt = pairr->second;
		float najlepsia = qSt->DajNajvyssieQ();
		if (najlepsia <= 0.0)
		{
			//Nemame ziadnu najlepsiu -> random (za predpokladu ze nie su zaporne)
			return rand() % pocetAkcii;
		}
		return qSt->DajNajlepsiuAkciu();
	}
}

void QL::UpravHyperparametre()
{
    ALPHA *= zmenaALPHA;
    if (ALPHA < 0.05)
    {
        ALPHA = 0.05;// aby to nebehalo uplne zbytocne
    }

    EPSILONx1000 *= zmenaEPSILON;
    if (EPSILONx1000 < 100)
    {
        EPSILONx1000 = 100;//10% random
    }
}

void QL::Learn(int odmena, Stav* novyStav,int vykonanaAkcia, bool koniec)
{
    pocetUceni++;
	float cieloveQ;


        if (!koniec) {
            //ak je to posledny novyStav -> ponom sa zacina nanovo
            auto pairNovy = stavy->find(novyStav);
            if (pairNovy == stavy->end())
            {
                QStav* qStav = new QStav(pocetAkcii, qInit);
                auto pair = std::pair<Stav*, QStav*>(novyStav->Clone(), qStav);
                stavy->insert(pair);
                pairNovy = stavy->find(novyStav);
            }
            //k odmene este dame nieco z noveho stavu 
            cieloveQ = odmena + GAMMA * pairNovy->second->DajNajvyssieQ();
            //cieloveQ = odmena + GAMMA * (*pairNovy->second.DajQHodnoty())[vykonanaAkcia];

        }
        else {
            //tak to ma typek ale nevidim dovod preco by tam nemohlo byt aj tamto hmmm...
            cieloveQ = odmena;
        }

	
	auto pairStary = stavy->find(minulyStav);
	if (pairStary == stavy->end())
	{
		//este tam nie je -> tu by toreticne nikdy nemalo vojst po upravach 
		QStav* qStav = new QStav(pocetAkcii,qInit);
        auto pair = std::pair<Stav*, QStav*>(minulyStav->Clone(), qStav);
        printf("ukladam");
        stavy->insert(pair);
        pairStary = stavy->find(minulyStav);
    }


    pairStary->second->UpdateQ(vykonanaAkcia, cieloveQ, ALPHA,algaritmus);
    minulaAkcia = vykonanaAkcia;

	minulyStav = novyStav->Clone();

    this->UpravHyperparametre();

}

void QL::Save(string path)
{
    ofstream file;
    file.open(path);
    file.clear();
    file << "sep=, \n";
    string pomocna;
    pomocna = to_string(pocetFeatur) + "," + to_string(pocetAkcii) + ",\n";
    file << pomocna;
    for (std::map<Stav*, QL::QStav*, CompareStav>::iterator it = stavy->begin(); it != stavy->end(); ++it)
    {
        pomocna = it->first->toCSV() + it->second->toCSV() + "\n";
        file << pomocna;
    }
    
    file.close();
}

void QL::Load(string path)
{
    //ak by sme chceli vediet spravit roll back 
    std::map<Stav*, QL::QStav*, CompareStav>* stavyLoad = new map <Stav*, QL::QStav*, CompareStav>();
    ifstream file;
    file.open(path);
    string line;
    string csvBlock;
    getline(file, csvBlock);//skip def sep
    getline(file, csvBlock, ',');
    pocetFeatur = stoi(csvBlock);
    getline(file, csvBlock, ',');
    getline(file, line);//koniec riadka?
    pocetAkcii = stoi(csvBlock);
    vector<float>* qHodnoty;
    vector<int>* stav;
    
    while (getline(file, line)) {
        //getline(file, line);
        if (line.length() <= 0) {
            break;
        }
        stringstream lineStream;
        lineStream << line;
        stav = new vector<int>();
        stav->resize(pocetFeatur);
        for (size_t i = 0; i < pocetFeatur; i++)
        {
            getline(lineStream, csvBlock, ',');
            (*stav)[i] = stoi(csvBlock);
        }
        qHodnoty = new vector<float>();
        qHodnoty->resize(pocetAkcii);
        for (size_t i = 0; i < pocetAkcii; i++)
        {
            getline(lineStream, csvBlock, ',');
            (*qHodnoty)[i] = stof(csvBlock);
        }
        stavyLoad->insert(make_pair(new Stav(stav), new QStav(qHodnoty)));
        //stavy.insert(make_pair(Stav(stav), QStav(qHodnoty)));
        int i = 5;
    }
    file.close();

    for (std::map<Stav*, QL::QStav*, CompareStav>::iterator it = stavy->begin(); it != stavy->end(); ++it)
    {
        it->first->~Stav();
        it->second->~QStav();
    }
    stavy = stavyLoad;
}





	//Obsahuje Q hodnoty akcii v danom stave
	//vector<float>* qHodnoty;//premenna QStravu ale to co on si one odhadne abo co ?

	

	vector<float>* QL::QStav::DajQHodnoty()
	{
		return qHodnoty;
	}

	QL::QStav::QStav (int pocetAkcii,QInit* qInicialization)
	{
    /*
        if (pocetAkcii == 3)
        {
            int a = 5;
        }*/
		qHodnoty = new vector<float>();//new int[pocetAkcii];
		qHodnoty->resize(pocetAkcii);
        
        if (qInicialization == nullptr) {
            //zmeni q hodnoty na 0 bo c++ ta nwm :D :D 
            for (int i = 0; i < pocetAkcii; i++)
            {
                //qHodnoty->push_back(0.0);
                (*qHodnoty)[i] = 0.0;
                //float a = (*qHodnoty)[i];
            }
        } else
        {
            qInicialization->initQ(qHodnoty);
        }
	}

    QL::QStav::QStav(vector<float>* qHod) {
        qHodnoty = qHod;
    }

    QL::QStav::~QStav() {
        qHodnoty->~vector();
    }



	int QL::QStav::DajNajlepsiuAkciu() {
		int index=0;
		//vector<float> qHod = *qHodnoty;
		float hodnota= (*qHodnoty)[0];
		for (size_t i = 1; i < qHodnoty->size(); i++)
		{
			if ((*qHodnoty)[i] > hodnota) {
				index = i;
				hodnota = (*qHodnoty)[i];
			}
		}
		return index;
	}
	float QL::QStav::DajNajvyssieQ() {
		//int index = 0;
		//vector<float> qHod = *qHodnoty;
		float hodnota = (*qHodnoty)[0];
		//auto max = qHod.size();
		for ( size_t i = 1; i < qHodnoty->size(); i++)
		{
			if ((*qHodnoty)[i] > hodnota) {
				//index = i;
				hodnota = (*qHodnoty)[i];
			}
		}
		return hodnota;
	}

	void QL::QStav::UpdateQ(int akcia, float cieloveQ, float alpha,int mod)
	{
        if (mod == 0) {
            //qlearning inak :D :D :D 
            (*qHodnoty)[akcia] += alpha * (cieloveQ - (*qHodnoty)[akcia]);
        }
        else {
            //qlearning
            (*qHodnoty)[akcia] = (1.0-alpha)*(*qHodnoty)[akcia]  + alpha * cieloveQ ;

        }
	}

	string QL::QStav::toString()
	{
		auto qHod = *qHodnoty;
		std::string str(qHodnoty->begin(), qHodnoty->end());
        return str;
	}

    string QL::QStav::toCSV() const
    {
        string pom;
        for (size_t i = 0; i < qHodnoty->size(); i++)
        {
            pom += std::to_string((*qHodnoty)[i]) + ",";
        }
        return pom;
    }


