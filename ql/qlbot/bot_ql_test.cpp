#include <iostream>
#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"
#include <limits>
#include "qllib/QInitZealot.h"
#include "sc2api/sc2_score.h"
#include "qllib/Stav.h"
#include "../../tests/feature_layers_shared.h"
#include "qlbot/bot_ql_test.h"


QlBot::QlBot(int width, int height, float square_size) :
    restarts_(0), pi(atan(1) * 4)
{
    //todo: parametricke prepinanie ucenia a cesta k csv s q hodnotami
    srand(time(NULL));
    stepNum = 0;
    GAMMA = 0.9;
    ALPHA = 0.1;
    EPSILON = 0.7;
    this->width = width;
    this->height = height;
    x = 0;
    y = 0;
    this->square_size = square_size;
    zstav_ = new TestState();
    state_ = new Stav(new vector<int>(2, 0));//TODO zatial napevno .. asi odstranim z parametra take a nejako to oriesim v kniznici
    ql_ = new QL(state_, 2, 4, new QInitZealot());
    ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);
    try
    {
        ql_->Load("minihraJamy.csv");
        printf("NACITANE");
    }
    catch (const std::exception& e)
    {
        printf("NENACITANE -> uci sa od zaciatku");
    }
}


void QlBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
    reward = 0;
    global_reward = reward;
    //Control()->SaveReplay()
    Debug()->DebugCreateUnit(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, *start_point, Observation()->GetPlayerID());
    Debug()->DebugEnemyControl();
    Debug()->DebugShowMap();
    Debug()->SendDebug();
    /*auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2s);
    auto end = std::chrono::high_resolution_clock::now();
    auto upper_left = new sc2::Point3D(units[0]->pos.x, units[0]->pos.y, units[0]->pos.z);
    auto lower_right = new sc2::Point3D(upper_left->x + 1, upper_left->y + 1, upper_left->z);
    Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Red);
    */
}

void QlBot::OnStep()
{
    uint32_t game_loop = Observation()->GetGameLoop();
    if (game_loop % 20 != 0)
    {
        return;
    }
    reward = 0;
    if (!draw && Observation()->GetGameLoop() < 10)
    {
        return;
    }
    stepNum++;
    if (stepNum % 200 == 0)
    {
        this->ql_->Save("minihraJamy.csv");
        printf("ulozene\n");
    }
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    //cout << units.size();
    if (!draw)
    {
        float x = units[0]->pos.x - square_size / 2;
        float y = units[0]->pos.y + square_size / 2;
        float z = units[0]->pos.z;
        for (int i = width - 1; i >= 0; i--)
        {
            for (int j = height - 1; j >= 0; j--)
            {
                auto upper_left = new sc2::Point3D(x + i * square_size, y - j * square_size, z);
                auto lower_right = new sc2::Point3D(x + (i + 1)* square_size, y - (j + 1) * square_size, z + 0.01);
                if (j == 0)
                {
                    if (i == width - 1)
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Green);
                    }
                    else if (i > 0)
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::Red);
                    }
                    else
                    {
                        Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::White);
                    }

                }
                else
                {
                    Debug()->DebugBoxOut(*upper_left, *lower_right, sc2::Colors::White);
                }


            }
        }
        Debug()->DebugMoveCamera(*new sc2::Point2D(x + width * square_size / 2, y - (height + 3) * square_size / 2));
        Debug()->SendDebug();
        draw = true;
    }
    //cout << Observation()->GetCameraPos().x << "   " << Observation()->GetCameraPos().y;
    if (units.empty())
    {
        return;
    }
    if (units[0]->orders.empty())
    {

        if (y == 0 && x != 0)
        {
            if (x == width - 1)
            {
                reward = 1;
            }
            else
            {
                reward = -10;
            }
            global_reward += reward;
            reward_now += reward - 0.001*reward_now;
            x = 0;
            y = 0;
            zstav_->set_x(y);
            zstav_->set_y(x);
            Debug()->DebugKillUnit(units[0]);
            Debug()->SendDebug();

            Debug()->DebugCreateUnit(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, *start_point, Observation()->GetPlayerID());
            Debug()->SendDebug();
            ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);
            return;
        }
        ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);


        if (units.empty())
        {
            //this->Vypis(" Nemame jednotky.");
            return;//ak nemame vojakov step sa nedeje (padlo)
        }

        int akcia = ql_->ChooseAction(false, new Stav(zstav_->to_array()));
        sc2::Units jednotkyNepriatelov = Observation()->GetUnits(sc2::Unit::Enemy);
        /*cout << "x:" << zstav_->get_x() << " y: " << zstav_->get_y() << endl;*/
        int x_new = x, y_new = y;
        //cout << akcia << endl;
        switch (akcia)
        {
        case 0: y_new = y - 1; /*cout << "hore" << endl;*/  break; //hore
        case 1: x_new = x + 1; /*cout << "doprava" << endl;*/ break; //doprava
        case 2: y_new = y + 1; /*cout << "dole" << endl; */break; //dole
        case 3: x_new = x - 1; /*cout << "dolava" << endl; */break; //dolava

        default: break;
        }
        lastAction = akcia;

        if (x_new < 0)
            return;
        if (x_new > width - 1)
            return;

        if (y_new < 0)
            return;
        if (y_new > height - 1)
            return;


        x = x_new;
        y = y_new;
        zstav_->set_x(y);
        zstav_->set_y(x);

        Actions()->UnitCommand(units[0], sc2::ABILITY_ID::MOVE, *new sc2::Point2D(start_point->x + x * square_size, start_point->y - y * square_size), false);
        printf("\nscore %6.3f %6.3f\n", global_reward, reward_now);


    }


}

void QlBot::UlozNaucene()
{
    this->ql_->Save("saveQL.csv");
}


void QlBot::OnGameEnd()
{
    ++restarts_;
    if (restarts_ % 1 == 0)
    {
        this->UlozNaucene();
        //Priebezne uklada to je asi len docasne alebo sa potom zvacsi interval
        //ak by padlo a podobne
        cout << "Ukladam." << endl;
    }
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).l;


    std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}


string QlBot::ReportNaKonciHry()
{
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    float friendly_hp = 0;
    for (auto unit : units)
    {
        friendly_hp += unit->health;
    }
    sc2::Units enemy_units = Observation()->GetUnits(sc2::Unit::Enemy);
    float enemy_hp = 0;
    for (auto unit : enemy_units)
    {
        enemy_hp += unit->health;
    }
    string ret_string = "Replikacia: " + to_string(restarts_) + " Pocet uceni: " + to_string(ql_->DajPocetUceni()) + "\n";
    ret_string += "Ziskana odmena: " + to_string(global_reward) + "\n";
    return ret_string;
}



QlBot::~QlBot()
{
    delete ql_;
    ql_ = nullptr;
    delete state_;
    state_ = nullptr;
    delete zstav_;
    zstav_ = nullptr;
}

QlBot::TestState::TestState()
{
    x = 0;
    y = 0;
}

QlBot::TestState::~TestState()
{
}

void QlBot::TestState::set_x(int x)
{
    this->x = x;
}

int QlBot::TestState::get_x()
{
    return x;
}

void QlBot::TestState::set_y(int y)
{
    this->y = y;
}

int QlBot::TestState::get_y()
{
    return y;
}

vector<int>* QlBot::TestState::to_array()
{
    vector<int>* pole = new vector<int>();
    pole->resize(2);
    (*pole)[0] = x;
    (*pole)[1] = y;
    return pole;
}

void QlBot::GetState() const
{
    zstav_->set_x(x);
    zstav_->set_y(y);
}

void QlBot::Triangulate(float speed, float degree, float& x, float& y)
{
    x = cos(degree * pi / 180) * speed;
    y = sin(degree * pi / 180) * speed;
}






