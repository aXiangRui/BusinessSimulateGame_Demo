#pragma once

#include"RUI_CustomerManager.h"
#include"RUI_DessertManager.h"
#include"RUI_MaterialManager.h"
#include"RUI_TextManager.h"
#include"RUI_ChatFrame.h"
#include"RUI_ChatFrame.h"

class CheckUpdate
{
    private:

    public:
        CheckUpdate() = default;
        ~CheckUpdate() = default;

        void update(CustomerManager& customerManager, DessertManager& dessertManager, MaterialManager& materialManager, ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing)
        {
            for(int i = 0; i < customerManager.GetCustomersSize(); i++)
            {
                if(customerManager.Customers[0].GetCustomerPreference() >= 50 && dessertManager.Desserts[3].GetWhetherUnlock() == 0)
                {
                    dessertManager.Desserts[3].SetWhetherUnlock(1);
                    dessertManager.Save();
                    chatFrame.setTitle(customerManager.GetCustomerName(0));
                    chatFrame.setContent(textManager.CustomerText[1]);
                    unlockFrame.SetContent(dessertManager.GetDessertName(3));
                    isShowing = 1;
                    //如果0号顾客好感度达到50且果冻未解锁就解锁果冻
                }
                if(customerManager.GetCustomerPreference(3) >= 100 && customerManager.Customers[10].GetWhetherAppear() == 0)
                {
                    customerManager.Customers[10].SetWhetherAppear(1);
                    customerManager.Save();
                    customerManager.update();
                    chatFrame.setTitle(customerManager.GetCustomerName(3));
                    chatFrame.setContent(textManager.CustomerText[2]);
                    unlockFrame.SetContent(customerManager.GetCustomerName(10));
                    isShowing = 1;
                    //如果3号顾客好感度达到100且未解锁10号顾客就解锁10号
                }
            }
        }
};