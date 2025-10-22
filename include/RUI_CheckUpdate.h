#pragma once

#include"RUI_CustomerManager.h"
#include"RUI_DessertManager.h"
#include"RUI_MaterialManager.h"
#include"RUI_TextManager.h"
#include"RUI_ChatFrame.h"

class CheckUpdate
{
    private:

    public:
        CheckUpdate() = default;
        ~CheckUpdate() = default;

        void update(CustomerManager& customerManager, DessertManager& dessertManager, MaterialManager& materialManager, ChatFrame& chatFrame, TextManager& textManager, bool& isShowing)
        {
            for(int i = 0; i < customerManager.GetCustomersSize(); i++)
            {
                if(customerManager.Customers[0].GetCustomerPreference() >= 50 && dessertManager.Desserts[3].GetWhetherUnlock() == 0)
                {
                    dessertManager.Desserts[3].SetWhetherUnlock(1);
                    dessertManager.Save();
                    chatFrame.setTitle(customerManager.GetCustomerName(0));
                    chatFrame.setContent(textManager.CustomerText[1]);
                    isShowing = 1;
                    //如果0号顾客好感度达到50且果冻未解锁就解锁果冻
                }
            }
        }
};