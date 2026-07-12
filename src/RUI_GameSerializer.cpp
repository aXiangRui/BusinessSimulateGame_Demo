#include "../include/RUI_GameSerializer.h"
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>

void GameSerializer::Load(
    Clock& timeClock,
    std::vector<Customer>& customers,
    std::vector<Cook>& cooks,
    std::vector<StoreServer>& servers,
    std::vector<Cabinet>& cabinets,
    CustomerManager& customerManager,
    int& totalMoney,
    int& totalCustomers,
    int& totalDessert)
{
    // ===== 1. 读取时间 =====
    {
        std::ifstream file("./save/Time.rui");
        std::string line;
        int time = 0;
        file >> time;
        SDL_Log("当前时间%d", time);
        timeClock.SetClockTime(time);

        int lineNum = 0;
        while (std::getline(file, line))
        {
            lineNum++;
            if (lineNum >= 2)
            {
                std::istringstream iss(line);
                int cID, x, y, stage, choosenumber, chooseid, number;
                std::string cName, cPath;
                iss >> cID >> cName >> cPath >> x >> y >> stage
                    >> choosenumber >> chooseid >> number;

                Customer a;
                for (int i = 0; i < customerManager.GetCustomersSize(); i++)
                {
                    if (cName == customerManager.GetCustomerName(i))
                    {
                        a = customerManager.Customers[i];
                        customerManager.Customers[i].SetHasJoined(1);
                    }
                }

                bool overload = false;
                for (size_t i = 0; i < customers.size(); i++)
                {
                    if (customers[i].GetCustomerID() == cID)
                    {
                        overload = true;
                    }
                }

                if (!overload)
                {
                    customers.push_back(a);
                    customers.back().SetX(x);
                    customers.back().SetY(y);
                    customers.back().SetCurrentStage(stage);
                    customers.back().SetChooseNumber(choosenumber);
                    customers.back().SetChooseID(chooseid);
                    customers.back().SetEatNumber(number);
                }
            }
        }
    }

    // ===== 2. 读取总数据 =====
    {
        std::ifstream file("./save/Total.rui");
        file >> totalCustomers;
        file >> totalMoney;
        file >> totalDessert;
        SDL_Log("读取到总人数为%d", totalCustomers);
    }

    // ===== 3. 读取面包柜 =====
    {
        std::ifstream file("./save/Cabinet.rui");
        int cabinetCount = 0;
        file >> cabinetCount;
        file.ignore();

        // 首次运行/文件为空：从 Furniture.rui 反推 Cabinet 数量
        if (cabinetCount == 0)
        {
            std::ifstream furnFile("./save/Furniture.rui");
            if (furnFile.is_open())
            {
                std::string fline;
                while (std::getline(furnFile, fline))
                {
                    if (fline.empty() || fline[0] == '#') continue;
                    std::istringstream fiss(fline);
                    int ftype;
                    fiss >> ftype;
                    if (ftype == static_cast<int>(FurnitureType::Cabinet))
                        cabinetCount++;
                }
            }
        }

        for (int i = 0; i < cabinetCount; i++)
        {
            std::string line;
            if (!std::getline(file, line))
                break;

            std::istringstream iss(line);
            std::vector<int> values;
            int val;
            while (iss >> val)
                values.push_back(val);

            Cabinet c;
            if (values.size() >= 3)
            {
                // 位置由 Furniture.rui 统一管理，此处用 0,0 占位
                c.InitCabinet(values[0], values[1], values[2], 0, 0);
            }
            cabinets.push_back(c);
        }

        // 兜底：Furniture.rui 中有 Cabinet 但 Cabinet.rui 中没有对应条目
        // 为每个缺少的 Cabinet 创建默认实体
        {
            std::ifstream furnFile("./save/Furniture.rui");
            if (furnFile.is_open())
            {
                std::string fline;
                while (std::getline(furnFile, fline))
                {
                    if (fline.empty() || fline[0] == '#') continue;
                    std::istringstream fiss(fline);
                    int ftype, fid;
                    fiss >> ftype >> fid;
                    if (ftype != static_cast<int>(FurnitureType::Cabinet)) continue;

                    bool found = false;
                    for (auto& cab : cabinets)
                    {
                        if (cab.GetCabinetID() == fid) { found = true; break; }
                    }
                    if (!found)
                    {
                        Cabinet c;
                        c.InitCabinet(fid, 0, 0, 0, 0);  // 空柜子，位置后续覆写
                        cabinets.push_back(c);
                    }
                }
            }
        }
    }

    // ===== 4. 读取员工 =====
    {
        std::ifstream file("./save/Employee.rui");
        cooks.clear();
        servers.clear();

        if (file.is_open())
        {
            std::string line;
            bool readingCook = false;
            bool readingServer = false;

            while (std::getline(file, line))
            {
                if (line == "cook")
                {
                    readingCook = true;
                    readingServer = false;
                    continue;
                }
                else if (line == "servers")
                {
                    readingCook = false;
                    readingServer = true;
                    continue;
                }
                else if (line == "end")
                {
                    if (readingCook)
                    {
                        readingCook = false;
                    }
                    else if (readingServer)
                    {
                        readingServer = false;
                        break;
                    }
                    continue;
                }
                else if (readingCook && !line.empty())
                {
                    std::istringstream iss(line);
                    std::string texture;
                    int x, y;
                    if (iss >> texture >> x >> y)
                    {
                        Cook cook;
                        cook.Init();
                        cook.SetPosition(x, y);
                        cook.SetAddress(texture);
                        cooks.push_back(cook);
                        SDL_Log("读取到厨师数据: %s, x=%d, y=%d", texture.c_str(), x, y);
                    }
                }
                else if (readingServer && !line.empty())
                {
                    std::istringstream iss(line);
                    std::string texture;
                    int x, y, speed;
                    if (iss >> texture >> x >> y >> speed)
                    {
                        StoreServer server;
                        server.init();
                        server.SetPosition(x, y);
                        server.SetSpeed(speed);
                        servers.push_back(server);
                        SDL_Log("读取到服务员数据: %s, x=%d, y=%d, speed=%d",
                                texture.c_str(), x, y, speed);
                    }
                }
            }
        }
        else
        {
            SDL_Log("无法打开Employee.rui文件");
        }
    }
}

void GameSerializer::Save(
    Clock& timeClock,
    std::vector<Customer>& customers,
    std::vector<Cook>& cooks,
    std::vector<StoreServer>& servers,
    std::vector<Cabinet>& cabinets,
    int totalMoney,
    int totalCustomers,
    int totalDessert)
{
    // ===== 1. 写入时间和顾客数据 =====
    {
        std::ofstream file("./save/Time.rui");
        if (!file)
        {
            SDL_Log("Save: failed to open save file for writing");
            return;
        }
        file << timeClock.ReturnAllHour() << std::endl;

        for (size_t i = 0; i < customers.size(); ++i)
        {
            file << customers[i].GetCustomerID() << " "
                 << customers[i].GetCustomerName() << " "
                 << customers[i].GetCustomerPath() << " "
                 << customers[i].getX() << " "
                 << customers[i].getY() << " "
                 << customers[i].GetCurrentStage() << " "
                 << customers[i].GetChooseNumber() << " "
                 << customers[i].GetChooseID() << " "
                 << customers[i].GetEatNumber() << " "
                 << std::endl;
        }
    }

    // ===== 2. 写入总数据 =====
    {
        std::ofstream file("./save/Total.rui");
        file << totalCustomers << std::endl;
        file << totalMoney << std::endl;
        file << totalDessert << std::endl;
    }

    // ===== 3. 写入面包柜（位置由 Furniture.rui 统一管理，此处只存状态） =====
    {
        std::ofstream file("./save/Cabinet.rui");
        file << cabinets.size() << std::endl;
        for (int i = 0; i < (int)cabinets.size(); i++)
        {
            file << cabinets[i].GetCabinetID() << " "
                 << cabinets[i].GetDessertID() << " "
                 << cabinets[i].GetDessertNumber()
                 << std::endl;
        }
    }

    // ===== 4. 写入员工 =====
    {
        std::ofstream file("./save/Employee.rui");
        file << "cook" << std::endl;
        for (int i = 0; i < (int)cooks.size(); i++)
        {
            file << cooks[i].GetAddress() << " "
                 << cooks[i].GetX() << " "
                 << cooks[i].GetY() << std::endl;
        }
        file << "end" << std::endl;
        file << "servers" << std::endl;
        for (int i = 0; i < (int)servers.size(); i++)
        {
            file << servers[i].GetAddress() << " "
                 << servers[i].GetX() << " "
                 << servers[i].GetY() << " "
                 << servers[i].GetSpeed() << std::endl;
        }
        file << "end" << std::endl;
    }
}

// ===== 5. 桌椅套装存取 ==========================================================

void GameSerializer::LoadDeskChairSets(std::vector<DeskChairSet>& sets)
{
    sets.clear();

    std::ifstream file("./save/DeskAndChairs.rui");
    if (!file.is_open())
    {
        SDL_Log("DeskAndChairs.rui 不存在，将使用默认布局");
        return;
    }

    int setCount = 0;
    file >> setCount;
    file.ignore();

    for (int s = 0; s < setCount; s++)
    {
        std::string line;
        if (!std::getline(file, line))
            break;

        std::istringstream iss(line);
        int sid, dCnt, cCnt, place;
        if (!(iss >> sid >> dCnt >> cCnt >> place))
            break;

        DeskChairSet set;
        set.InitSet(sid, dCnt, cCnt, static_cast<PlacementType>(place));

        // 读取桌子格点
        for (int d = 0; d < dCnt; d++)
        {
            if (!std::getline(file, line))
                break;
            std::istringstream diss(line);
            int row, col;
            if (diss >> row >> col)
                set.SetDeskPos(d, {col, row});
        }

        // 读取椅子格点
        for (int c = 0; c < cCnt; c++)
        {
            if (!std::getline(file, line))
                break;
            std::istringstream ciss(line);
            int row, col;
            if (ciss >> row >> col)
                set.SetChairPos(c, {col, row});
        }

        sets.push_back(set);
    }

    SDL_Log("加载了 %d 套桌椅套装", (int)sets.size());
}

void GameSerializer::SaveDeskChairSets(const std::vector<DeskChairSet>& sets)
{
    std::ofstream file("./save/DeskAndChairs.rui");
    if (!file)
    {
        SDL_Log("SaveDeskChairSets: 无法写入文件");
        return;
    }

    file << sets.size() << std::endl;

    for (int s = 0; s < (int)sets.size(); s++)
    {
        const DeskChairSet& set = sets[s];

        file << set.GetSetID() << " "
             << set.GetDeskCount() << " "
             << set.GetChairCount() << " "
             << static_cast<int>(set.GetPlacement()) << std::endl;

        // 桌子格点 (row col)
        for (int d = 0; d < set.GetDeskCount(); d++)
        {
            GridPos pos = set.GetDeskPos(d);
            file << pos.row << " " << pos.col << std::endl;
        }

        // 椅子格点 (row col)
        for (int c = 0; c < set.GetChairCount(); c++)
        {
            GridPos pos = set.GetChairPos(c);
            file << pos.row << " " << pos.col << std::endl;
        }
    }

    SDL_Log("保存了 %d 套桌椅套装", (int)sets.size());
}
