#pragma once
#include "qllib/Stav.h"

class QInit;

class QL
{
private:
    int pocetUceni;
    void UpravHyperparametre();
public:
	QL(Stav* stav, int pocFeatur, int pocetAkcii, QInit* qin);
	~QL();
	void Learn(int odmena,Stav* stav,int akcia, bool koniec);
	int ChooseAction(bool vybratNajlepsie, Stav* aktualnyStav);
    void Save(string path);
    void Load(string path);
    void SetHyperparemeters(float alpha, float gamma, float epsilon);
    int DajPocetUceni();
    void PremazMinulyStav();
    void ZmenAlgaritmus(int algaritmus);
    
	class QStav
	{
	private:
		//Obsahuje Q hodnoty akcii v danom stave
		vector<float>* qHodnoty;
		public:
            QStav(int pocetAkcii, QInit* qin);
            QStav(vector<float>* qHodnoty);
            ~QStav();
            vector<float>* DajQHodnoty();		
			int DajNajlepsiuAkciu();
			float DajNajvyssieQ();
			void UpdateQ(int akcia, float cieloveQ, float gamma, int mod);
			string toString();
            string toCSV() const;
            
		
	};

    
	
};

