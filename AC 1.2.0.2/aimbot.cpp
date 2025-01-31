#include<iostream>
#include<algorithm>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "memory.h"
using namespace std;

class Player{
    public:
    int hp;
    float x, y, z;
    float magnitude;
};

constexpr auto localPlayer = 0x109B74;
constexpr auto entitylist = 0x110D90;
constexpr auto hP = 0xF8;
constexpr auto x = 0x4;
constexpr auto y = 0x8;
constexpr auto z = 0xC;
constexpr auto Vx = 0x40;
constexpr auto Vy = 0x44;

int main(){
    Memory m{"ac_client.exe"};
    const auto ModuleBaseAddress = m.GetModuleAddress("ac_client.exe");
    const auto localPlayerAddress = m.Read<uintptr_t>(ModuleBaseAddress + localPlayer);
    Player local;
    vector<Player>enemies;
    float dx, dy, dz,vx,vy;
    while(true){
        local.hp = m.Read<int>(localPlayerAddress + hP);
        local.x = m.Read<float>(localPlayerAddress + x);
        local.y = m.Read<float>(localPlayerAddress + y);
        local.z = m.Read<float>(localPlayerAddress + z);
        cout << "HP: " << local.hp << " X: " << local.x << " Y: " << local.y << " Z: " << local.z << endl;
        enemies.clear();
        for(int i = 0; i < 12; i++){
            const auto entityAddress = m.Read<uintptr_t>(ModuleBaseAddress + entitylist + i * 0x4);
            if(entityAddress == 0){
                continue;
            } 
            auto healthc = m.Read<int>(entityAddress + hP);
            if(healthc <1){
                continue;
            }
            Player enemy;
            enemy.hp = healthc;
            enemy.x = m.Read<float>(entityAddress + x);
            enemy.y = m.Read<float>(entityAddress + y);
            enemy.z = m.Read<float>(entityAddress + z);
            dx = enemy.x - local.x;
            dy = enemy.y - local.y;
            dz = enemy.z - local.z;
            enemy.magnitude = sqrt(dx*dx+dy*dy+dz*dz);
            enemies.push_back(enemy);
        }
        if (!enemies.empty()) {
            sort(enemies.begin(), enemies.end(), [](const Player& a, const Player& b) {
                return a.magnitude < b.magnitude;
            });

            if (GetAsyncKeyState(0x51) & 0x8000){
                dx=enemies[0].x-local.x;
                dy=enemies[0].y-local.y;
                dz=enemies[0].z-local.z;
                float hyp=sqrt(dx*dx+dy*dy);
                vx = (atan2(dy, dx) * 180 / 3.14159265) + 90;
                vy = atan2(dz, hyp) * 180 / 3.14159265;
                m.Write<float>(localPlayerAddress + Vx, vx);
                m.Write<float>(localPlayerAddress + Vy, vy);

            }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return 0;
}