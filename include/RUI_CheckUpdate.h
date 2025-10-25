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
                UnlockDessert(customerManager, 0, dessertManager, 3, chatFrame, textManager, unlockFrame,isShowing,1,50);               
                UnlockCustomer(customerManager, 3, 10, chatFrame, textManager,unlockFrame, isShowing,2, 100);
                UnlockMaterial(customerManager, 1, materialManager, 4, chatFrame, textManager,unlockFrame, isShowing,1,100);
            }
        }
        void UnlockDessert(CustomerManager& customerManager,int CustomerID, DessertManager& dessertManager,int DessertID,ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing,int TextID, int UnlockPreference)
        {
            if(customerManager.Customers[CustomerID].GetCustomerPreference() >= UnlockPreference&& dessertManager.Desserts[DessertID].GetWhetherUnlock() == 0)
            {
                dessertManager.Desserts[DessertID].SetWhetherUnlock(1);
                dessertManager.Save();
                chatFrame.setTitle(customerManager.GetCustomerName(CustomerID));
                chatFrame.setContent(textManager.CustomerText[TextID]);
                unlockFrame.SetContent(dessertManager.GetDessertName(DessertID));
                isShowing = 1;
            }
        }
        void UnlockCustomer(CustomerManager& customerManager,int CustomerID, int UnlockCustomerID,ChatFrame& chatFrame, TextManager& textManager, UnlockFrame& unlockFrame,bool& isShowing,int TextID,int UnlockPreference)
        {
            if(customerManager.GetCustomerPreference(CustomerID) >= UnlockPreference & customerManager.Customers[UnlockCustomerID].GetWhetherAppear() == 0)
            {
                customerManager.Customers[UnlockCustomerID].SetWhetherAppear(1);
                customerManager.Save();
                customerManager.update();
                chatFrame.setTitle(customerManager.GetCustomerName(CustomerID));
                chatFrame.setContent(textManager.CustomerText[TextID]);
                unlockFrame.SetContent(customerManager.GetCustomerName(UnlockCustomerID));
                isShowing = 1;
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
};