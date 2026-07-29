#pragma once

#include "RUI_CreateEvent.h"
#include "RUI_MenuButton.h"
#include "RUI_ResourceManager.h"
#include "RUI_MusicManager.h"
#include "RUI_DessertManager.h"
#include "RUI_MaterialManager.h"
#include "RUI_ChooseFrame.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <cstdio>

extern int WindowWidth;
extern int WindowHeight;
extern RUI_SceneManager SceneManager;

// ============================================================================
// 经典合成模式
// ============================================================================
class ClassicMode
{
public:
    ClassicMode() = default;
    ~ClassicMode() = default;

    MusicPlayer CreateMusic;
    std::vector<MenuButton> buttons;
    CreateRUIEvent Event;

    void onEnter()
    {
        if (!Mix_PlayingMusic())
        {
            CreateMusic.setMusic(ResourceManager::instance()->FindMusic("createmusic"));
            CreateMusic.play(-1);
        }
        BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

        MenuButton Btn0((WindowWidth - 320) / 2, 520, 320, 64, "返回店中", 0);
        buttons.push_back(Btn0);

        std::vector<int> Size;
        for (int i = 0; i < 3; i++)
            Size.push_back(16 * (i + 1));

        Event.load();
        Event.ChooseSize(Size);
        Event.ChooseBase();
        Event.ChooseDecoration();
    }

    void onUpdate() { Event.update(); }

    void onRender(SDL_Renderer* Renderer)
    {
        SDL_SetRenderDrawColor(Renderer, 10, 10, 10, 255);
        SDL_RenderClear(Renderer);
        SDL_RenderCopy(Renderer, BackgroundTexture, nullptr, &BackgroundRect);
        Event.onRender(Renderer);
        SDL_RenderPresent(Renderer);
    }

    void onInput(const SDL_Event& event) { Event.input(event); }

    void onExit()
    {
        Event.quit();
        CreateMusic.quit();
        buttons.clear();
        SDL_Log("退出经典合成模式");
    }

    bool GetWhetherBack() { return Event.GetWhetherBack(); }

private:
    SDL_Texture* BackgroundTexture = nullptr;
    SDL_Rect BackgroundRect = {0, 0, 800, 600};
};

// ============================================================================
// 自由DIY模式
// ============================================================================
class DIYMode
{
public:
    DIYMode() = default;
    ~DIYMode() = default;

    enum class DIYStage
    {
        Shape, Base, Tier, Flavor, Syrup, Bake,
        Slice, Filling, Frosting, Piping, Toppings,
        Naming, Preview, Confirm
    };

    enum class ChooseShape { Circle, Square, Rectangle, Heart, Star };

    MusicPlayer CreateMusic;
    std::vector<MenuButton> buttons;
    TTF_Font* diyFont = nullptr;

    DessertManager dManager;
    MaterialManager mManager;

    // Shape
    ChooseShape selectedShape = ChooseShape::Circle;
    std::vector<ChooseFrame> shapeFrames;
    std::vector<std::string> shapeNames;
    int shapePage = 0;

    // Base
    int selectedBaseID = -1;
    std::vector<ChooseFrame> baseFrames;
    std::vector<Dessert> baseDesserts;
    int baseIDMap[100];
    int basePage = 0;

    // Bake
    int tempLevel = 1;
    float internalTemp = 0.0f, surfaceBrown = 0.0f, moisture = 1.0f, bakeTime = 0.0f;
    bool isBaking = false;
    Uint32 bakeStartTime = 0;
    SDL_Rect tempButtons[3], finishBakeBtn;

    // Slice
    int sliceCount = 1;
    std::vector<ChooseFrame> sliceFrames;
    int slicePage = 0;

    // Filling
    int fillingCount = 0;
    int fillingMaterials[2] = {-1, -1};
    int currentFilling = 0;
    std::vector<ChooseFrame> fillingFrames;
    std::vector<int> fillingIDMap;
    std::vector<Material> fillingMaterialsList;
    int fillingPage = 0;

    // Frosting
    struct FrostingColor { Uint8 r, g, b; const char* name; };
    FrostingColor frostingColors[3] = {
        {255,255,255,"白色"}, {255,240,100,"黄色"}, {100,180,255,"蓝色"}
    };
    int selectedFrostingColor = 0;
    SDL_Rect frostingButtons[3];

    // Piping
    const char* pipingNames[4] = {"无裱花", "星形花嘴", "贝壳纹", "玫瑰挤花"};
    int selectedPiping = 0;
    std::vector<ChooseFrame> pipingFrames;
    int pipingPage = 0;

    // Toppings
    int toppingCount = 0;
    int toppingIDs[3] = {-1, -1, -1};
    std::vector<ChooseFrame> toppingFrames;
    std::vector<int> toppingIDMap;
    std::vector<Material> toppingMatList;
    int toppingPage = 0;

    // Naming
    char cakeName[32] = "我的蛋糕";
    int namingCursor = 0;

    void onEnter()
    {
        if (!Mix_PlayingMusic())
        {
            CreateMusic.setMusic(ResourceManager::instance()->FindMusic("createmusic"));
            CreateMusic.play(-1);
        }
        BackgroundTexture = ResourceManager::instance()->FindTexture("kitchen");
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

        diyFont = TTF_OpenFont("./resources/font/namidiansong.ttf", 24);
        if (!diyFont) SDL_Log("DIY: 字体加载失败");

        MenuButton Btn0((WindowWidth - 320) / 2, 520, 320, 64, "返回店中", 0);
        buttons.push_back(Btn0);

        dManager.InitDessertManager();
        mManager.InitMaterialManager();

        InitShapeStage();
        InitBakeStage();
        InitFrostingStage();

        currentStage = DIYStage::Shape;
    }

    void InitShapeStage()
    {
        shapeFrames.clear();
        shapeNames = {"圆形", "方形", "矩形", "心形", "星形"};
        for (size_t i = 0; i < shapeNames.size(); i++)
        {
            ChooseFrame f;
            f.InitChooseFrame(500, 10 + (int)i * 64 - ((int)i / 8) * 64 * 8, (int)i, shapeNames[i].c_str());
            shapeFrames.push_back(f);
        }
    }

    void InitBaseStage()
    {
        baseFrames.clear();
        baseDesserts.clear();
        for (int i = 0; i < 100; i++) baseIDMap[i] = -1;
        int j = 0;
        for (int i = 0; i < dManager.GetDessertsSize(); i++)
        {
            if (dManager.GetWhetherBase(i) && dManager.GetWhetherUnlock(i))
            {
                ChooseFrame f;
                f.InitChooseFrame(500, 10 + j * 64 - (j / 8) * 64 * 8, j, dManager.GetDessertName(i).c_str());
                baseFrames.push_back(f);
                Dessert d = dManager.Desserts[i];
                d.SetPosition(140, -100);
                d.SetWidth(256, 256);
                baseDesserts.push_back(d);
                baseIDMap[j] = i;
                j++;
            }
        }
    }

    void InitBakeStage()
    {
        tempButtons[0] = {100, 350, 120, 50};
        tempButtons[1] = {250, 350, 120, 50};
        tempButtons[2] = {400, 350, 120, 50};
        finishBakeBtn = {300, 480, 200, 60};
    }

    void InitSliceStage()
    {
        sliceFrames.clear();
        const char* n[3] = {"不切（1片）", "切2片 · 1层夹心", "切3片 · 2层夹心"};
        for (int i = 0; i < 3; i++)
        {
            ChooseFrame f;
            f.InitChooseFrame(500, 10 + i * 64, i, n[i]);
            sliceFrames.push_back(f);
        }
    }

    void InitFillingStage()
    {
        fillingCount = sliceCount - 1;
        currentFilling = 0;
        fillingMaterials[0] = fillingMaterials[1] = -1;
        fillingFrames.clear();
        fillingIDMap.clear();
        fillingMaterialsList.clear();
        int j = 0;
        for (int i = 0; i < mManager.GetDecorationSize(); i++)
        {
            if (mManager.DecorationMaterial[i].GetWhetherUnlock())
            {
                ChooseFrame f;
                f.InitChooseFrame(500, 10 + j * 64 - (j / 8) * 64 * 8, j, mManager.GetDecorationName(i).c_str());
                fillingFrames.push_back(f);
                fillingIDMap.push_back(i);
                fillingMaterialsList.push_back(mManager.DecorationMaterial[i]);
                j++;
            }
        }
    }

    void InitFrostingStage()
    {
        for (int i = 0; i < 3; i++)
            frostingButtons[i] = {200 + i * 140, 280, 120, 80};
        selectedFrostingColor = 0;
    }

    void InitPipingStage()
    {
        pipingFrames.clear();
        for (int i = 0; i < 4; i++)
        {
            ChooseFrame f;
            f.InitChooseFrame(500, 10 + i * 64, i, pipingNames[i]);
            pipingFrames.push_back(f);
        }
        selectedPiping = 0;
    }

    void InitToppingsStage()
    {
        toppingCount = 0;
        for (int i = 0; i < 3; i++) toppingIDs[i] = -1;
        toppingFrames.clear();
        toppingIDMap.clear();
        toppingMatList.clear();
        int j = 0;
        for (int i = 0; i < mManager.GetDecorationSize(); i++)
        {
            if (mManager.DecorationMaterial[i].GetWhetherUnlock())
            {
                ChooseFrame f;
                f.InitChooseFrame(500, 10 + j * 64 - (j / 8) * 64 * 8, j, mManager.GetDecorationName(i).c_str());
                toppingFrames.push_back(f);
                toppingIDMap.push_back(i);
                toppingMatList.push_back(mManager.DecorationMaterial[i]);
                j++;
            }
        }
    }

    void InitNamingStage()
    {
        SDL_strlcpy(cakeName, "我的蛋糕", sizeof(cakeName));
        SDL_StartTextInput();
    }

    void onUpdate()
    {
        Uint32 now = SDL_GetTicks();
        if (currentStage == DIYStage::Bake && isBaking)
        {
            float dt = (now - bakeStartTime) / 1000.0f;
            bakeStartTime = now;
            float hr[3] = {0.10f, 0.25f, 0.50f};
            float br[3] = {0.02f, 0.08f, 0.25f};
            float dr[3] = {0.03f, 0.10f, 0.20f};
            internalTemp += hr[tempLevel] * dt;
            surfaceBrown += br[tempLevel] * dt;
            moisture -= dr[tempLevel] * dt;
            if (internalTemp > 1.0f) internalTemp = 1.0f;
            if (surfaceBrown > 1.0f) surfaceBrown = 1.0f;
            if (moisture < 0.0f) moisture = 0.0f;
            bakeTime += dt;
        }
    }

    void onRender(SDL_Renderer* R)
    {
        SDL_SetRenderDrawColor(R, 10, 10, 10, 255);
        SDL_RenderClear(R);
        SDL_RenderCopy(R, BackgroundTexture, nullptr, &BackgroundRect);

        switch (currentStage)
        {
        case DIYStage::Shape:    RenderShapeStage(R); break;
        case DIYStage::Base:     RenderBaseStage(R); break;
        case DIYStage::Bake:     RenderBakeStage(R); break;
        case DIYStage::Slice:    RenderSliceStage(R); break;
        case DIYStage::Filling:  RenderFillingStage(R); break;
        case DIYStage::Frosting: RenderFrostingStage(R); break;
        case DIYStage::Piping:   RenderPipingStage(R); break;
        case DIYStage::Toppings: RenderToppingsStage(R); break;
        case DIYStage::Naming:   RenderNamingStage(R); break;
        case DIYStage::Preview:  RenderPreviewStage(R); break;
        case DIYStage::Confirm:  RenderConfirmStage(R); break;
        default:                 RenderPlaceholderStage(R); break;
        }

        SDL_RenderPresent(R);
    }

    void onInput(const SDL_Event& event)
    {
        // 文本输入（命名阶段）
        if (event.type == SDL_TEXTINPUT && currentStage == DIYStage::Naming)
        {
            int len = (int)SDL_strlen(cakeName);
            int add = (int)SDL_strlen(event.text.text);
            if (len + add < (int)sizeof(cakeName) - 1)
                SDL_strlcat(cakeName, event.text.text, sizeof(cakeName));
            return;
        }
        if (event.type == SDL_KEYDOWN && currentStage == DIYStage::Naming)
        {
            if (event.key.keysym.sym == SDLK_BACKSPACE)
            {
                int len = (int)SDL_strlen(cakeName);
                if (len > 0) cakeName[len - 1] = '\0';
            }
            return;
        }

        if (event.type == SDL_MOUSEMOTION)
        {
            int mx = event.motion.x, my = event.motion.y;
            switch (currentStage)
            {
            case DIYStage::Shape:   for (auto& f : shapeFrames)  f.isHovered(mx, my); break;
            case DIYStage::Base:    for (auto& f : baseFrames)   f.isHovered(mx, my); break;
            case DIYStage::Slice:   for (auto& f : sliceFrames)  f.isHovered(mx, my); break;
            case DIYStage::Filling: for (auto& f : fillingFrames) f.isHovered(mx, my); break;
            case DIYStage::Piping:   for (auto& f : pipingFrames)  f.isHovered(mx, my); break;
            case DIYStage::Toppings: for (auto& f : toppingFrames) f.isHovered(mx, my); break;
            default: break;
            }
            return;
        }
        if (event.type != SDL_MOUSEBUTTONDOWN) return;

        int mx = event.button.x, my = event.button.y;
        for (auto& btn : buttons)
        {
            if (btn.RUI_isClicked(mx, my)) { btn.ClickApplication(); return; }
        }

        switch (currentStage)
        {
        case DIYStage::Shape:   InputShapeStage(mx, my); break;
        case DIYStage::Base:    InputBaseStage(mx, my); break;
        case DIYStage::Bake:    InputBakeStage(mx, my); break;
        case DIYStage::Slice:   InputSliceStage(mx, my); break;
        case DIYStage::Filling: InputFillingStage(mx, my); break;
        case DIYStage::Frosting: InputFrostingStage(mx, my); break;
        case DIYStage::Piping:   InputPipingStage(mx, my); break;
        case DIYStage::Toppings: InputToppingsStage(mx, my); break;
        case DIYStage::Naming:   InputNamingStage(mx, my); break;
        case DIYStage::Preview:  InputPreviewStage(mx, my); break;
        case DIYStage::Confirm:  InputConfirmStage(mx, my); break;
        default: break;
        }
    }

    void onExit()
    {
        SDL_StopTextInput();
        if (diyFont) TTF_CloseFont(diyFont);
        CreateMusic.quit();
        buttons.clear();
        SDL_Log("退出DIY模式");
    }

    bool GetWhetherBack() { return false; }

private:
    void RenderTextOnButton(SDL_Renderer* R, const char* text, SDL_Rect btn)
    {
        if (!diyFont || !text || !text[0]) return;
        SDL_Color c = {255, 255, 255, 255};
        SDL_Surface* s = TTF_RenderUTF8_Blended(diyFont, text, c);
        if (!s) return;
        SDL_Texture* t = SDL_CreateTextureFromSurface(R, s);
        if (t)
        {
            SDL_Rect d = {btn.x + (btn.w - s->w) / 2, btn.y + (btn.h - s->h) / 2, s->w, s->h};
            SDL_RenderCopy(R, t, nullptr, &d);
            SDL_DestroyTexture(t);
        }
        SDL_FreeSurface(s);
    }

    // ---- Shape ----
    void RenderShapeStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < shapeFrames.size(); i++)
        {
            if (i / 8 == shapePage)
            {
                if (shapeFrames[i].GetIsHovered())
                {
                    shapeFrames[i].onHoverRender(R);
                    SDL_Rect p = {140, 150, 200, 200};
                    SDL_SetRenderDrawColor(R, 200, 180, 100, 255);
                    SDL_RenderFillRect(R, &p);
                    RenderTextOnButton(R, shapeNames[i].c_str(), p);
                }
                else shapeFrames[i].onRender(R);
            }
        }
    }

    void InputShapeStage(int mx, int my)
    {
        for (size_t i = 0; i < shapeFrames.size(); i++)
        {
            if (i / 8 == shapePage && shapeFrames[i].GetIsHovered())
            {
                selectedShape = (ChooseShape)i;
                InitBaseStage();
                currentStage = DIYStage::Base;
                return;
            }
        }
    }

    // ---- Base ----
    void RenderBaseStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < baseFrames.size(); i++)
        {
            if (i / 8 == basePage)
            {
                if (baseFrames[i].GetIsHovered())
                {
                    baseFrames[i].onHoverRender(R);
                    if (i < baseDesserts.size()) baseDesserts[i].onRender(R);
                }
                else baseFrames[i].onRender(R);
            }
        }
    }

    void InputBaseStage(int mx, int my)
    {
        for (size_t i = 0; i < baseFrames.size(); i++)
        {
            if (i / 8 == basePage && baseFrames[i].GetIsHovered())
            {
                selectedBaseID = baseIDMap[i];
                currentStage = DIYStage::Bake;
                isBaking = true;
                bakeStartTime = SDL_GetTicks();
                return;
            }
        }
    }

    // ---- Bake ----
    void RenderBakeStage(SDL_Renderer* R)
    {
        const char* tn[3] = {"低火", "中火", "高火"};
        for (int i = 0; i < 3; i++)
        {
            SDL_SetRenderDrawColor(R, i == tempLevel ? 255 : 150, i == tempLevel ? 100 : 150, i == tempLevel ? 50 : 150, 255);
            SDL_RenderFillRect(R, &tempButtons[i]);
            RenderTextOnButton(R, tn[i], tempButtons[i]);
        }
        SDL_Rect bg = {100, 430, 500, 30};
        SDL_SetRenderDrawColor(R, 80, 80, 80, 255);
        SDL_RenderFillRect(R, &bg);
        SDL_Rect fill = {100, 430, (int)(internalTemp * 500), 30};
        SDL_SetRenderDrawColor(R, 200, 100, 50, 255);
        SDL_RenderFillRect(R, &fill);
        SDL_Rect z = {100 + (int)(0.6f * 500), 425, (int)(0.3f * 500), 40};
        SDL_SetRenderDrawColor(R, 0, 200, 0, 60);
        SDL_RenderFillRect(R, &z);
        SDL_SetRenderDrawColor(R, 200, 80, 80, 255);
        SDL_RenderFillRect(R, &finishBakeBtn);
        RenderTextOnButton(R, "出炉", finishBakeBtn);
    }

    void InputBakeStage(int mx, int my)
    {
        for (int i = 0; i < 3; i++)
        {
            if (mx >= tempButtons[i].x && mx <= tempButtons[i].x + tempButtons[i].w &&
                my >= tempButtons[i].y && my <= tempButtons[i].y + tempButtons[i].h)
            { tempLevel = i; return; }
        }
        if (mx >= finishBakeBtn.x && mx <= finishBakeBtn.x + finishBakeBtn.w &&
            my >= finishBakeBtn.y && my <= finishBakeBtn.y + finishBakeBtn.h)
        {
            isBaking = false;
            currentStage = DIYStage::Slice;
            InitSliceStage();
        }
    }

    // ---- Slice ----
    void RenderSliceStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < sliceFrames.size(); i++)
        {
            if (sliceFrames[i].GetIsHovered()) sliceFrames[i].onHoverRender(R);
            else sliceFrames[i].onRender(R);
        }
        SDL_Rect info = {100, 200, 300, 100};
        SDL_SetRenderDrawColor(R, 80, 80, 80, 255);
        SDL_RenderFillRect(R, &info);
        RenderTextOnButton(R, "选择切几片", info);
    }

    void InputSliceStage(int mx, int my)
    {
        for (size_t i = 0; i < sliceFrames.size(); i++)
        {
            if (sliceFrames[i].GetIsHovered())
            {
                sliceCount = i + 1;
                InitFillingStage();
                currentStage = DIYStage::Filling;
                return;
            }
        }
    }

    // ---- Filling ----
    void RenderFillingStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < fillingFrames.size(); i++)
        {
            if (i / 8 == fillingPage)
            {
                if (fillingFrames[i].GetIsHovered())
                {
                    fillingFrames[i].onHoverRender(R);
                    if ((size_t)fillingIDMap[i] < fillingMaterialsList.size())
                        fillingMaterialsList[i].onRender(R);
                }
                else fillingFrames[i].onRender(R);
            }
        }
        int px = 80, py = 120, pw = 200, ph = 300;
        int lh = ph / (2 * sliceCount - 1);
        for (int s = 0; s < sliceCount; s++)
        {
            SDL_Rect ck = {px, py + ph - (2 * s + 1) * lh, pw, lh};
            SDL_SetRenderDrawColor(R, 200, 160, 100, 255);
            SDL_RenderFillRect(R, &ck);
            if (s < sliceCount - 1)
            {
                SDL_Rect fl = {px, py + ph - (2 * s + 2) * lh, pw, lh};
                int m = fillingMaterials[s];
                SDL_SetRenderDrawColor(R, 180, 80, 80, 255);
                SDL_RenderFillRect(R, &fl);
                if (m >= 0) RenderTextOnButton(R, mManager.GetDecorationName(m).c_str(), fl);
            }
        }
        char buf[64];
        snprintf(buf, 64, "夹心 %d/%d", currentFilling + 1, fillingCount);
        SDL_Rect h = {px, py - 30, 150, 30};
        SDL_SetRenderDrawColor(R, 60, 60, 60, 255);
        SDL_RenderFillRect(R, &h);
        RenderTextOnButton(R, buf, h);
    }

    void InputFillingStage(int mx, int my)
    {
        for (size_t i = 0; i < fillingFrames.size(); i++)
        {
            if (i / 8 == fillingPage && fillingFrames[i].GetIsHovered())
            {
                fillingMaterials[currentFilling] = fillingIDMap[i];
                SDL_Log("夹心%d: %s", currentFilling, mManager.GetDecorationName(fillingIDMap[i]).c_str());
                currentFilling++;
                if (currentFilling >= fillingCount)
                {
                    InitFrostingStage();
                    currentStage = DIYStage::Frosting;
                }
                return;
            }
        }
    }

    // ---- Frosting ----
    void RenderFrostingStage(SDL_Renderer* R)
    {
        for (int i = 0; i < 3; i++)
        {
            SDL_SetRenderDrawColor(R, i == selectedFrostingColor ? 255 : 150, 255, 200, 255);
            SDL_RenderFillRect(R, &frostingButtons[i]);
            SDL_Rect sw = {frostingButtons[i].x + 10, frostingButtons[i].y + 8, 30, 30};
            SDL_SetRenderDrawColor(R, frostingColors[i].r, frostingColors[i].g, frostingColors[i].b, 255);
            SDL_RenderFillRect(R, &sw);
            SDL_SetRenderDrawColor(R, 0, 0, 0, 100);
            SDL_RenderDrawRect(R, &sw);
            RenderTextOnButton(R, frostingColors[i].name, {frostingButtons[i].x + 50, frostingButtons[i].y + 10, 70, 40});
        }
        SDL_Rect pre = {200, 420, 300, 60};
        SDL_SetRenderDrawColor(R, frostingColors[selectedFrostingColor].r, frostingColors[selectedFrostingColor].g, frostingColors[selectedFrostingColor].b, 255);
        SDL_RenderFillRect(R, &pre);
        RenderTextOnButton(R, "抹面色预览", pre);
    }

    void InputFrostingStage(int mx, int my)
    {
        for (int i = 0; i < 3; i++)
        {
            if (mx >= frostingButtons[i].x && mx <= frostingButtons[i].x + frostingButtons[i].w &&
                my >= frostingButtons[i].y && my <= frostingButtons[i].y + frostingButtons[i].h)
            {
                selectedFrostingColor = i;
                SDL_Log("抹面颜色: %s", frostingColors[i].name);
                InitPipingStage();
                currentStage = DIYStage::Piping;
                return;
            }
        }
    }

    // ---- Piping ----
    void RenderPipingStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < pipingFrames.size(); i++)
        {
            if (pipingFrames[i].GetIsHovered()) pipingFrames[i].onHoverRender(R);
            else pipingFrames[i].onRender(R);
        }
        char buf[64];
        snprintf(buf, 64, "\xe5\xbd\x93\xe5\x89\x8d: %s", pipingNames[selectedPiping]);
        SDL_Rect info = {100, 280, 200, 50};
        SDL_SetRenderDrawColor(R, 60, 60, 60, 255);
        SDL_RenderFillRect(R, &info);
        RenderTextOnButton(R, buf, info);
    }
    void InputPipingStage(int mx, int my)
    {
        for (size_t i = 0; i < pipingFrames.size(); i++)
        {
            if (pipingFrames[i].GetIsHovered())
            {
                selectedPiping = i;
                InitToppingsStage();
                currentStage = DIYStage::Toppings;
                return;
            }
        }
    }
    // ---- Toppings ----
    void RenderToppingsStage(SDL_Renderer* R)
    {
        for (size_t i = 0; i < toppingFrames.size(); i++)
        {
            if (i / 8 == toppingPage)
            {
                if (toppingFrames[i].GetIsHovered())
                {
                    toppingFrames[i].onHoverRender(R);
                    if ((size_t)toppingIDMap[i] < toppingMatList.size()) toppingMatList[i].onRender(R);
                }
                else toppingFrames[i].onRender(R);
            }
        }
        char buf[64];
        snprintf(buf, 64, "\xe8\xa3\x85\xe9\xa5\xb0 %d/3", toppingCount);
        SDL_Rect info = {100, 280, 150, 40};
        SDL_SetRenderDrawColor(R, 60, 60, 60, 255);
        SDL_RenderFillRect(R, &info);
        RenderTextOnButton(R, buf, info);
        if (toppingCount > 0)
        {
            SDL_Rect done = {550, 500, 150, 50};
            SDL_SetRenderDrawColor(R, 100, 200, 100, 255);
            SDL_RenderFillRect(R, &done);
            RenderTextOnButton(R, "\xe5\xae\x8c\xe6\x88\x90\xe8\xa3\x85\xe9\xa5\xb0", done);
        }
    }
    void InputToppingsStage(int mx, int my)
    {
        if (toppingCount < 3)
        {
            for (size_t i = 0; i < toppingFrames.size(); i++)
            {
                if (i / 8 == toppingPage && toppingFrames[i].GetIsHovered())
                {
                    toppingIDs[toppingCount++] = toppingIDMap[i];
                    return;
                }
            }
        }
        if (toppingCount > 0 && mx >= 550 && mx <= 700 && my >= 500 && my <= 550)
        {
            InitNamingStage();
            currentStage = DIYStage::Naming;
        }
    }
    // ---- Naming ----
    void RenderNamingStage(SDL_Renderer* R)
    {
        SDL_Rect box = {150, 250, 500, 60};
        SDL_SetRenderDrawColor(R, 200, 200, 200, 255);
        SDL_RenderFillRect(R, &box);
        SDL_SetRenderDrawColor(R, 0, 0, 0, 255);
        SDL_RenderDrawRect(R, &box);
        RenderTextOnButton(R, cakeName, box);
        SDL_Rect tip = {150, 320, 500, 40};
        SDL_SetRenderDrawColor(R, 80, 80, 80, 255);
        SDL_RenderFillRect(R, &tip);
        RenderTextOnButton(R, "\xe8\xbe\x93\xe5\x85\xa5\xe5\x90\x8d\xe5\xad\x97\xef\xbc\x8c\xe7\x82\xb9\xe5\x87\xbb\xe7\xa1\xae\xe8\xae\xa4", tip);
        SDL_Rect done = {550, 500, 150, 50};
        SDL_SetRenderDrawColor(R, 100, 200, 100, 255);
        SDL_RenderFillRect(R, &done);
        RenderTextOnButton(R, "\xe7\xa1\xae\xe8\xae\xa4", done);
    }
    void InputNamingStage(int mx, int my)
    {
        if (mx >= 550 && mx <= 700 && my >= 500 && my <= 550)
        {
            if (SDL_strlen(cakeName) == 0) SDL_strlcpy(cakeName, "\xe6\x97\xa0\xe5\x90\x8d\xe8\x9b\x8b\xe7\xb3\x95", sizeof(cakeName));
            SDL_StopTextInput();
            currentStage = DIYStage::Preview;
        }
    }
    // ---- Preview ----
    void RenderPreviewStage(SDL_Renderer* R)
    {
        char buf[256]; int y = 80;
        auto line = [&](const char* t) {
            SDL_Rect r = {80, y, 640, 30}; SDL_SetRenderDrawColor(R, 50, 50, 50, 255); SDL_RenderFillRect(R, &r); RenderTextOnButton(R, t, r); y += 34;
        };
        snprintf(buf,256,"\xe5\x90\x8d\xe7\xa7\xb0: %s",cakeName); line(buf);
        snprintf(buf,256,"\xe5\xbd\xa2\xe7\x8a\xb6: %s  \xe5\x9f\xba\xe7\xb1\xbb: %s",shapeNames[(int)selectedShape].c_str(),selectedBaseID>=0?dManager.GetDessertName(selectedBaseID).c_str():"?"); line(buf);
        snprintf(buf,256,"\xe5\x88\x87\xe7\x89\x87: %d\xe7\x89\x87  \xe5\xa4\xb9\xe5\xbf\x83: %d\xe5\xb1\x82",sliceCount,fillingCount); line(buf);
        for (int i=0;i<fillingCount;i++) { snprintf(buf,256,"  \xe5\xa4\xb9\xe5\xbf\x83%d: %s",i+1,fillingMaterials[i]>=0?mManager.GetDecorationName(fillingMaterials[i]).c_str():"\xe6\x97\xa0"); line(buf); }
        snprintf(buf,256,"\xe6\x8a\xb9\xe9\x9d\xa2: %s  \xe8\xa3\xb6\xe8\x8a\xb1: %s",frostingColors[selectedFrostingColor].name,pipingNames[selectedPiping]); line(buf);
        snprintf(buf,256,"\xe8\xa3\x85\xe9\xa5\xb0: %d\xe4\xb8\xaa",toppingCount); line(buf);
        SDL_Rect done={300,500,200,60}; SDL_SetRenderDrawColor(R,100,200,80,255); SDL_RenderFillRect(R,&done); RenderTextOnButton(R,"\xe7\xa1\xae\xe8\xae\xa4\xe5\x87\xba\xe5\x93\x81\xef\xbc\x81",done);
    }
    void InputPreviewStage(int mx, int my)
    {
        if (mx>=300 && mx<=500 && my>=500 && my<=560) currentStage = DIYStage::Confirm;
    }
    // ---- Confirm ----
    void RenderConfirmStage(SDL_Renderer* R)
    {
        SDL_Rect box={200,250,400,100}; SDL_SetRenderDrawColor(R,60,120,60,255); SDL_RenderFillRect(R,&box); RenderTextOnButton(R,"\xe5\x87\xba\xe5\x93\x81\xe6\x88\x90\xe5\x8a\x9f\xef\xbc\x81\xe8\xbf\x94\xe5\x9b\x9e\xe5\xba\x97\xe4\xb8\xad",box);
    }
    void InputConfirmStage(int mx, int my)
    {
        if (mx>=200 && mx<=600 && my>=250 && my<=350) { SDL_Log("DIY\xe5\xae\x8c\xe6\x88\x90"); SceneManager.ChooseScene(RUI_SceneManager::SceneType::Game); }
    }
    // ---- 占位 ----
    void RenderPlaceholderStage(SDL_Renderer* R)
    {
        const char* names[] = {"Shape","Base","Tier","Flavor","Syrup","Bake",
            "Slice","Filling","Frosting","Piping","Toppings","Naming","Preview","Confirm"};
        int i = (int)currentStage;
        const char* n = (i >= 0 && i < 14) ? names[i] : "?";
        char buf[128];
        snprintf(buf, 128, "%s — 点击继续", n);
        SDL_Rect a = {200, 230, 400, 100};
        SDL_SetRenderDrawColor(R, 80, 80, 80, 255);
        SDL_RenderFillRect(R, &a);
        RenderTextOnButton(R, buf, a);
    }

    SDL_Texture* BackgroundTexture = nullptr;
    SDL_Rect BackgroundRect = {0, 0, 800, 600};
    DIYStage currentStage = DIYStage::Shape;
};
