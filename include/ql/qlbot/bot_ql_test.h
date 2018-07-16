#pragma once
#include "sc2api/sc2_agent.h"
#include "qlbot/zealot_state.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"

using namespace std;

struct ResultRecord
{
    float reward;
    float reward_now;
    int win_count;
    int game_count;
    double win_percentage;
};

class QlBot : public sc2::Agent
{
    class TestState
    {
        int x;
        int y;
    public:
        TestState();
        ~TestState();
        void set_x(int x);
        int get_x();
        void set_y(int y);
        int get_y();
        vector<int>* to_array();
    };

    time_t lastUpdate;


    bool is_learning;
    int stepNum;
    double ALPHA;
    double GAMMA;
    double EPSILON;

    bool draw = false;
    uint32_t restarts_;
    const double pi;
    
    int lastAction = 0;
    float reward = 0;
    float reward_now = 0;
    float global_reward = 0;
    
    int x = 0;
    int y = 0;
    
    int width;
    int height;
    float square_size;
    TestState* zstav_;
    QL* ql_;
    Stav* state_;
    sc2::Point2D* start_point = new sc2::Point2D(10 + 3, 10 + 32);

    void GetState() const;
    void Triangulate(float speed, float degree, float& x, float& y);
    void Load();
    void Save();

    string ql_path;
    string res_path;
    
    struct ResultRecord buffer[20];
    struct ResultRecord* buffer_pointer;
    int buffer_size;

    int game_count;
    int win_count;
    double win_percentage;
public:
    QlBot(int width, int height, float square_size, bool is_learning);
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void UlozNaucene();
    string ReportNaKonciHry();
    ~QlBot();

};
