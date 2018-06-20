#pragma once
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
using namespace std;
class ZealotState
{
private:
    int hp;
    int shield;
    int najblizsiPrvyVzd;
    int najblizsiDruhyVzd;
    int najblizsiPrvyHp;
    int najblizsiDruhyHp;
    int pocetNepriatelov;
    int ciel;
public:
    ZealotState();
    ~ZealotState();

    void set_hp(float hpPer);
    int get_hp();
    void set_shield(float shieldPer);
    int get_shield();
    void set_najblizsi_prvy_vzd(float distance);
    int get_najblizsi_prvy_vzd();
    void set_najblizsi_druhy_vzd(float distance);
    int get_najblizsi_druhy_vzd();
    void set_najblizsi_prvy_hp(float hp);
    int get_najblizsi_prvy_hp();
    void set_najblizsi_druhy_hp(float hp);
    int get_najblizsi_druhy_hp();
    void set_pocet_nepriatelov(int count);
    int get_pocet_nepriatelov();
    void set_ciel(int jednotka);
    int get_ciel();
    vector<int>* to_array();

};