#include "qlbot/zealot_state.h"


ZealotState::ZealotState() : hp(0), shield(0), najblizsiPrvyVzd(0), najblizsiDruhyVzd(0), najblizsiPrvyHp(0),
                             najblizsiDruhyHp(0), pocetNepriatelov(0), ciel(0)
{
}

ZealotState::~ZealotState()
{
}

void ZealotState::set_hp(float hpPer)
{
    //pod 0.1 sa mu celkom tazko dostava vela vyhrava :D 
    if (hpPer < 0.2)
    {
        hp = 1;
    }
    else if (hpPer < 0.35)
    {
        hp = 2;
    }
    else if (hpPer < 0.55)
    {
        hp = 3;
    }
    else if (hpPer < 0.85)
    {
        hp = 4;
    }
    else
    {
        hp = 5;
    }
}

int ZealotState::get_hp()
{
    return hp;
}

void ZealotState::set_shield(float shieldPer)
{
    //vela stavov s nizkim shieldom -> hned ide dole a neobnovuje sa? 
    if (shieldPer < 0.01)
    {
        shield = 1;
    }
    else if (shieldPer < 0.2)
    {
        shield = 2;
    }
    else if (shieldPer < 0.45)
    {
        shield = 3;
    }
    else if (shieldPer < 0.75)
    {
        shield = 4;
    }
    else
    {
        shield = 5;
    }
}

int ZealotState::get_shield()
{
    return shield;
}

void ZealotState::set_najblizsi_prvy_vzd(float distance)
{
    //3.5-5 nejako malo
    if (distance < 2)
    {
        najblizsiPrvyVzd = 1;
    }
    else if (distance < 3)
    {
        najblizsiPrvyVzd = 2;
    }
    else if (distance < 4.5)
    {
        najblizsiPrvyVzd = 3;
    }
    else if (distance < 6)
    {
        najblizsiPrvyVzd = 4;
    }
    else
    {
        najblizsiPrvyVzd = 5;
    }
}

int ZealotState::get_najblizsi_prvy_vzd()
{
    return najblizsiPrvyVzd;
}

void ZealotState::set_najblizsi_druhy_vzd(float distance)
{
    //pod 5 ide celkom tazko

    if (distance < 6)
    {
        najblizsiDruhyVzd = 1;
    }
    else if (distance < 7)
    {
        najblizsiDruhyVzd = 2;
    }
    else if (distance < 9)
    {
        najblizsiDruhyVzd = 3;
    }
    else if (distance < 12)
    {
        najblizsiDruhyVzd = 4;
    }
    else
    {
        najblizsiDruhyVzd = 5;
    }
}

int ZealotState::get_najblizsi_druhy_vzd()
{
    return najblizsiDruhyVzd;
}

void ZealotState::set_najblizsi_prvy_hp(float hp)
{
    if (hp < 0.2)
    {
        najblizsiPrvyHp = 1;
    }
    else if (hp < 0.35)
    {
        najblizsiPrvyHp = 2;
    }
    else if (hp < 0.55)
    {
        najblizsiPrvyHp = 3;
    }
    else if (hp < 0.75)
    {
        najblizsiPrvyHp = 3;
    }
    else
    {
        najblizsiPrvyHp = 4;
    }
}

int ZealotState::get_najblizsi_prvy_hp()
{
    return najblizsiPrvyHp;
}

void ZealotState::set_najblizsi_druhy_hp(float hp)
{
    if (hp < 0.2)
    {
        najblizsiDruhyHp = 1;
    }
    else if (hp < 0.4)
    {
        najblizsiDruhyHp = 2;
    }
    else if (hp < 0.75)
    {
        najblizsiDruhyHp = 3;
    }
    else
    {
        najblizsiDruhyHp = 4;
    }
}

int ZealotState::get_najblizsi_druhy_hp()
{
    return najblizsiDruhyHp;
}

void ZealotState::set_pocet_nepriatelov(int count)
{
    pocetNepriatelov = count;
}

int ZealotState::get_pocet_nepriatelov()
{
    return pocetNepriatelov;
}

void ZealotState::set_ciel(int jednotka)
{
    ciel = jednotka;
}
int ZealotState::get_ciel()
{
    return ciel;
}

vector<int>* ZealotState::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(8);
    (*pole)[0] = hp;
    (*pole)[1] = shield;
    (*pole)[2] = najblizsiPrvyVzd;
    (*pole)[3] = najblizsiDruhyVzd;
    (*pole)[4] = najblizsiPrvyHp;
    (*pole)[5] = najblizsiDruhyHp;
    (*pole)[6] = pocetNepriatelov;
    (*pole)[7] = ciel;
    return pole;
}

