#pragma once

#include"RUI_CustomerManager.h"
#include"RUI_DessertManager.h"
#include"RUI_MaterialManager.h"
#include"RUI_TextManager.h"
#include"RUI_ChatFrame.h"
#include <fstream>
#include <sstream>
#include <vector>

struct CheckStructure
{
    ChatFrame& chatFrame;
    TextManager& textManager;
    UnlockFrame& unlockFrame;
    bool& isShowing;

    CheckStructure(ChatFrame& ch, TextManager&th, UnlockFrame& uh, bool& showing):
        chatFrame(ch),textManager(th),unlockFrame(uh),isShowing(showing){}
};

struct UnlockCustomerList
{
    int CustomerID;
    int UnlockCustomerID;
    int TextID;
    int UnlockPreference;
    UnlockCustomerList(int cID, int ucID, int tID, int uPre):
        CustomerID(cID),UnlockCustomerID(ucID),TextID(tID),UnlockPreference(uPre){}
};

class CheckUpdate
{
    private:

    public:
        CheckUpdate() = default;
        ~CheckUpdate() = default;

        void init()
        {
            Load();
        }

        void update(CustomerManager& customerManager, DessertManager& dessertManager, MaterialManager& materialManager, ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing, std::vector<Customer>&Customers)
        {
            CheckStructure Structure(chatFrame, textManager, unlockFrame, isShowing);
            UnlockDessert(customerManager, 0, dessertManager, 3, chatFrame, textManager, unlockFrame,isShowing,1,50);
            /*蛋糕胚解锁区*/
            for(int i = 0; i < UnlockCustomers.size();i++)
            {
                UnlockCustomer(customerManager, Structure, UnlockCustomers[i], Customers);
            }
            /*顾客解锁区*/
            UnlockMaterial( customerManager, 1, materialManager, 4, chatFrame, textManager,unlockFrame, isShowing,1,100);
            /*材料解锁区*/
        }
        void UnlockDessert(CustomerManager customerManager,int CustomerID, DessertManager& dessertManager,int DessertID,ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing,int TextID, int UnlockPreference)
        {
            if(customerManager.Customers[CustomerID].GetCustomerPreference() >= UnlockPreference && dessertManager.Desserts[DessertID].GetWhetherUnlock() == 0)
            {
                dessertManager.Desserts[DessertID].SetWhetherUnlock(1);
                dessertManager.Save();
                chatFrame.setTitle(customerManager.GetCustomerName(CustomerID));
                chatFrame.setContent(textManager.CustomerText[TextID]);
                unlockFrame.SetContent(dessertManager.GetDessertName(DessertID));
                isShowing = 1;
            }
        }
        void UnlockCustomer(CustomerManager& customerManager,CheckStructure& structure, UnlockCustomerList us, std::vector<Customer> Customers)
        {
            if(customerManager.GetCustomerPreference(us.CustomerID) >= us.UnlockPreference && customerManager.Customers[us.UnlockCustomerID].GetWhetherAppear() == 0)
            {
                customerManager.Customers[us.UnlockCustomerID].SetWhetherAppear(1);
                customerManager.Save();
                customerManager.update();
                for(int i = 0; i < Customers.size(); i++)
                {
                    std::string name = Customers[i].GetCustomerName();
                    int id = customerManager.GetCustomerIDByName(name);
                    customerManager.Customers[id].SetHasJoined(1);
                }
                structure.chatFrame.setTitle(customerManager.GetCustomerName(us.CustomerID));
                structure.chatFrame.setContent(structure.textManager.CustomerText[us.TextID]);
                structure.unlockFrame.SetContent(customerManager.GetCustomerName(us.UnlockCustomerID));
                structure.isShowing = 1;
            }
        }
        void UnlockMaterial(CustomerManager& customerManager,int CustomerID, MaterialManager& materialManager,int MaterialID,ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing,int TextID,int UnlockPreference)
        {
            if(customerManager.Customers[CustomerID].GetCustomerPreference() >= UnlockPreference&& materialManager.DecorationMaterial[MaterialID].GetWhetherUnlock() == 0)
            {
                materialManager.DecorationMaterial[MaterialID].SetWhetherUnlock(1);
                materialManager.Save();
                chatFrame.setTitle(customerManager.GetCustomerName(CustomerID));
                chatFrame.setContent(textManager.CustomerText[TextID]);
                unlockFrame.SetContent(materialManager.GetDecorationName(MaterialID));
                isShowing = 1;
            }
        }
        void Load()
        {
            UnlockCustomers.clear();
            std::ifstream File("./save/UnlockList.rui");
            std::string string;
            int line = 0;
            while(std::getline(File,string))
            {
                line++;
                std::istringstream iss(string);   
                if(string[0] == '#')
                    continue;
                int cID;
                int ucID;
                int tID;
                int uPre;
                iss >> cID >> ucID >> tID >> uPre;
                UnlockCustomerList a(cID,ucID,tID,uPre);
                UnlockCustomers.push_back(a);
            }
        }
        private:
        std::vector<UnlockCustomerList> UnlockCustomers;
};
