# 烘焙系统设计文档

> 为现有合成流程（CreateScene）增加烘焙阶段，让玩家实际参与"烤蛋糕"过程。
> 这份文档用于指导未来实现，包含数据结构、流程改动、UI交互、存档扩展。

---

## 1. 概述

### 1.1 设计目标

在**选完基底蛋糕 + 材料**之后、**抹面裱花装饰**之前，插入一个**烘焙小游戏**：

```
  当前流程：
  基类 → 选材料 → (等待1s) → 装饰 → 成品

  改造后流程：
  基类 → 选材料 → 🔥 烘烤 → 装饰 → 成品
                     ↓
              品质决定基础属性系数
```

### 1.2 核心体验

玩家不是"选完等结果"，而是需要在烘焙过程中做出操作抉择：
- 烤箱温度控制（高中低三档可调）
- 烘焙时长选择（在合适时机"出炉"）
- 操作结果影响蛋糕品质等级

---

## 2. 架构改动总览

### 2.1 新增 / 修改文件

| 文件 | 类型 | 说明 |
|------|------|------|
| `include/RUI_BakingMinigame.h` | **新增** | 烘焙小游戏核心逻辑 |
| `include/RUI_BakingData.h` | **新增** | 烘焙参数、品质定义 |
| `include/RUI_Product.h` | **修改** | BakingQuality 加入 ProducedProduct |
| `include/RUI_CreateEvent.h` | **修改** | Stage 新增 `bake` 状态 |
| `include/RUI_CreateScene.h` | **修改** | CreateStage 新增 `Baking` |
| `include/RUI_Cook.h` | **修改** | 增加烘焙技能属性 |
| `include/RUI_Dessert.h` | **修改** | 增加 bakeParams（每款蛋糕的烤焙参数） |
| `save/Desserts.rui` | **修改** | 每行末尾增加烘焙参数 |
| `save/Cook.rui` | **修改** | 增加厨师烘焙经验/技能存档 |
| `resources/texture/` | **新增** | 烤箱 / 进度条 / 品质指示器贴图 |

### 2.2 不修改的文件

- `RUI_GameScene.*` / `RUI_GameWorld.*` — 烘焙只在合成场景发生
- `RUI_MaterialManager.*` / `RUI_DessertManager.*` — 只加字段，不改变加载方式
- `RUI_GameSerializer.*` — 烘焙数据随 Product / Cook 存档

---

## 3. Stage::bake — 烘焙阶段状态机

### 3.1 在现有 Stage enum 中插入

```cpp
// include/RUI_CreateEvent.h — 第566行附近
enum class Stage
{
    size,        // 选盘子      (已有)
    base,        // 选基类蛋糕   (已有)
    bake,        // 🔥 烘烤     (新增 ← 插入在这里)
    create,      // 选3种材料   (已有)
    decorate     // 装饰       (已有)
};
```

**为什么插在 base 之后、create 之前？**

因为现实逻辑是：**材料混合 + 入炉烤焙 → 蛋糕胚完成 → 装饰**。但你的系统里 create 阶段是往蛋糕里加"风味材料"，这些材料应该在烘焙**前**就混合进去。所以更合理的流程是：

```
选基类 → 选3种风味材料 → 🔥 烘烤 → 装饰
                       ↑
                  新材料在这里入炉
```

对应的 enum 顺序：

```cpp
enum class Stage
{
    size,        // 选盘子
    base,        // 选基类蛋糕
    create,      // 选风味材料（奶油/草莓/巧克力等）
    bake,        // 🔥 烘烤 ← 这里
    decorate     // 装饰
};
```

> **注意**：现有代码中 create 阶段完成后自动 `CStage = Stage::decorate`，需要改为 `CStage = Stage::bake`。

### 3.2 烘焙阶段 UI 布局

```
┌─────────────────────────────────────────────┐
│  🔥 烘焙中                                 │
│                                             │
│   ┌──────────────────────────────────┐      │
│   │          烤箱内部                  │      │
│   │    ┌──────────────────────┐      │      │
│   │    │  蛋糕在烤               │      │      │
│   │    │  🍰                    │      │      │
│   │    └──────────────────────┘      │      │
│   │    < 上火: ████████░░ 70% >      │      │
│   │    < 下火: ██████░░░░ 60% >      │      │
│   └──────────────────────────────────┘      │
│                                             │
│   温度: [ 低 🔵 ] [ 中 🟢 ] [ 高 🔴 ]       │
│                                             │
│   烘焙进度: ████████████░░░░░░ 65%           │
│   建议出炉: ░░░░░░░░░░░███████ ← 最佳区间    │
│                                             │
│        [ 🔥 继续烤 ]  [ ✅ 出炉 ]             │
└─────────────────────────────────────────────┘
```

### 3.3 烘焙阶段输入/更新/渲染

参照现有 `Stage::create` 的代码结构，在 `CreateRUIEvent` 中对应位置添加：

#### 3.3.1 update() — 第151行附近

```cpp
// 在 PProduct.Calculate(CurrentTime) 之前/之后
if (CStage == Stage::bake)
{
    bakingMinigame.Update(CurrentTime);
}
```

#### 3.3.2 onRender() — 第176行附近

```cpp
case Stage::bake:
{
    bakingMinigame.Render(Renderer);
    break;
}
```

#### 3.3.3 input() — 第288行附近

鼠标悬停逻辑：

```cpp
case Stage::bake:
{
    // 悬停"出炉"按钮 → 手型光标
    if (bakingMinigame.IsFinishButtonHovered(mx, my))
    {
        j = 1;
        SDL_SetCursor(...);
    }
    // 悬停温度档位 → 手型光标
    if (bakingMinigame.IsTempButtonHovered(mx, my))
    {
        j = 1;
        SDL_SetCursor(...);
    }
    break;
}
```

点击逻辑：

```cpp
case Stage::bake:
{
    // 点击"出炉" → 锁定品质，进入下一阶段
    if (bakingMinigame.IsFinishButtonClicked(mx, my))
    {
        bakingMinigame.LockQuality();
        PProduct.SetQuality(bakingMinigame.GetQuality());
        CStage = Stage::decorate;
    }
    // 点击温度档位 → 切换火力
    if (bakingMinigame.IsTempButtonClicked(mx, my, &tempLevel))
    {
        bakingMinigame.SetTemperature(tempLevel);
    }
    break;
}
```

---

## 4. 烘焙小游戏核心机制

### 4.1 核心模型

每次烘焙的本质：

```
一个隐藏的"烤箱内部状态"在随时间变化：
  - 蛋糕内部温度（决定是否烤熟）
  - 表面焦化度（决定是否烤焦）
  - 水分含量（决定是否太干）

玩家的操作：
  - 选择上火/下火温度档位
  - 选择何时"出炉"

结果由出炉时刻的状态决定 → 品质等级
```

### 4.2 三档温度设计

| 档位 | 升温速度 | 焦化速度 | 水分流失 | 适用场景 |
|------|---------|---------|---------|---------|
| 🔵 低火（120°C） | 慢 | 极慢 | 慢 | 需要长时间均匀受热的厚蛋糕 |
| 🟢 中火（160°C） | 中 | 中 | 中 | 万金油，容错最高 |
| 🔴 高火（200°C） | 快 | 快 | 快 | 追求外酥里嫩，但容易烤焦 |

### 4.3 三条隐藏曲线

每条曲线的值范围都是 `0.0 ~ 1.0`，每帧根据当前温度档位更新：

```cpp
struct BakeState {
    float internalTemp;    // 内部熟度 0.0~1.0
    float surfaceBrown;    // 表面焦化 0.0~1.0
    float moisture;        // 水分保留 1.0~0.0（从1开始下降）
    float elapsed;         // 已烤时间（秒，用于UI显示）
};
```

**更新公式（每帧 60fps，dt ≈ 0.0167s）：**

```cpp
void UpdateBakeState(BakeState& state, int tempLevel, float dt)
{
    // 温度档位对应的速率系数
    float heatRate[3] = {0.10f, 0.25f, 0.50f};    // 升温速率
    float brownRate[3] = {0.02f, 0.08f, 0.25f};   // 焦化速率
    float dryRate[3] = {0.03f, 0.10f, 0.20f};     // 水分流失速率

    state.internalTemp += heatRate[tempLevel] * dt;
    state.surfaceBrown += brownRate[tempLevel] * dt;
    state.moisture -= dryRate[tempLevel] * dt;

    // 钳制
    state.internalTemp = Clamp01(state.internalTemp);
    state.surfaceBrown = Clamp01(state.surfaceBrown);
    state.moisture = Clamp01(state.moisture);
    state.elapsed += dt;
}
```

### 4.4 品质判定逻辑

```cpp
BakeQuality DetermineQuality(const BakeState& state)
{
    // 1. 基本条件：内部熟度必须达到"熟透"区间
    if (state.internalTemp < 0.60f)
        return BakeQuality::Raw;         // ❌ 夹生

    // 2. 检查是否烤焦
    if (state.surfaceBrown > 0.90f)
        return BakeQuality::Burnt;       // ❌ 烤焦

    // 3. 检查是否太干
    if (state.moisture < 0.20f)
        return BakeQuality::Dry;         // ❌ 太干

    // 4. 综合评分——在"最佳区间"内
    //    内部熟度 0.70~0.95
    //    表面焦化 0.30~0.75
    //    水分保留 0.40~0.80
    float score = 0.0f;
    score += ScoreRange(state.internalTemp, 0.70f, 0.95f);
    score += ScoreRange(state.surfaceBrown, 0.30f, 0.75f);
    score += ScoreRange(state.moisture, 0.40f, 0.80f);

    if (score >= 2.6f) return BakeQuality::Perfect;    // 🌟 完美
    if (score >= 2.0f) return BakeQuality::Good;       // 👍 良好
    if (score >= 1.2f) return BakeQuality::Acceptable; // 👌 一般
    return BakeQuality::Overdone;                      // ⚠️ 略过
}
```

### 4.5 品质等级定义

```cpp
// include/RUI_BakingData.h
enum class BakeQuality : int
{
    Raw        = -2,  // 夹生——无法出售
    Burnt      = -1,  // 烤焦——无法出售
    Overdone   = 0,   // 略过——可卖，低价
    Acceptable = 1,   // 一般——正常价
    Good       = 2,   // 良好——价格 ×1.25
    Perfect    = 3    // 完美——价格 ×1.5，额外好评
};
```

### 4.6 品质系数表

| 品质 | 价格系数 | 顾客满意度 | 解锁要求 |
|------|---------|-----------|---------|
| Raw | ❌ 不可出售 | — | — |
| Burnt | ❌ 不可出售 | — | — |
| Overdone | ×0.7 | -20% | — |
| Acceptable | ×1.0 | 基准 | — |
| Good | ×1.25 | +15% | — |
| Perfect | ×1.5 | +30% | 新手教程后即可达成 |

---

## 5. 对现有系统的改动

### 5.1 ProducedProduct — 新增烘焙品质字段

```cpp
// include/RUI_Product.h — ProducedProduct 类（第238行附近）

class ProducedProduct
{
public:
    // ... 现有方法 ...

    // ★ 新增
    void SetQuality(BakeQuality q) { quality = q; }
    BakeQuality GetQuality() const { return quality; }

    float GetPriceMultiplier() const
    {
        switch (quality)
        {
            case BakeQuality::Overdone:   return 0.7f;
            case BakeQuality::Acceptable: return 1.0f;
            case BakeQuality::Good:       return 1.25f;
            case BakeQuality::Perfect:    return 1.5f;
            default:                      return 1.0f;
        }
    }

    int GetFinalPrice()
    {
        return (int)(GetProductPrice() * GetPriceMultiplier());
    }

private:
    // ... 现有字段 ...
    BakeQuality quality = BakeQuality::Acceptable;  // ★ 新增
};
```

### 5.2 ProducingProduct — 暂存烘焙品质

```cpp
// include/RUI_Product.h — ProducingProduct 类（第14行附近）

class ProducingProduct
{
public:
    // ★ 新增
    void SetQuality(BakeQuality q) { finalQuality = q; }
    BakeQuality GetQuality() const { return finalQuality; }

private:
    // ... 现有字段 ...
    BakeQuality finalQuality = BakeQuality::Acceptable;  // ★ 新增
};
```

### 5.3 Product.rui — 存档格式扩展

当前格式：
```
ProductID BaseDessertID DecorationID[3] PlateSize
```

改为：
```
ProductID BaseDessertID DecorationID[3] PlateSize BakeQuality
```

`ProductManager::Save/Load` 对应修改——增加第6个字段的读写。

### 5.4 Desserts.rui — 每款蛋糕增加烘焙参数

当前每行：
```
ID 甜度 饱腹感 口感 名字 路径 是否基类 是否解锁 等级
```

修改为（新增 3 个参数）：
```
ID 甜度 饱腹感 口感 名字 路径 是否基类 是否解锁 等级 推荐温度 烤焙时长 厚度系数
```

| 新增字段 | 类型 | 说明 |
|---------|------|------|
| 推荐温度 | int | 0=低/1=中/2=高，提示玩家用什么温度烤 |
| 烤焙时长 | float | 基准烤焙时间（秒），影响进度条总长度 |
| 厚度系数 | float | 0.8~1.5，越厚的蛋糕升温越慢，出炉时机更宽松 |

**示例数据：**
```
0 20 20 20 戚风蛋糕 chiffon 1 1 1 1 8.0 1.0
2 35 40 35 巧克力蛋糕 chocolatecake 0 0 1 1 10.0 1.2
3 10 10 10 果冻 jelly 1 1 1 2 5.0 0.6
```

> 现有 Dessert::InitLevel() 需要增加对应参数的读取（第15行附近）。

### 5.5 Cook — 烘焙技能系统

```cpp
// include/RUI_Cook.h — Cook 类（第9行附近）

class Cook
{
public:
    void Init()
    {
        // ... 现有初始化 ...
        bakeLevel = 1;           // ★ 新增：烘焙等级 1~10
        bakeExp = 0;             // ★ 新增：烘焙经验值
        nextLevelExp = 100;      // ★ 新增：升下一级所需经验
    }

    // ★ 新增方法
    int GetBakeLevel() const { return bakeLevel; }
    void AddBakeExp(int exp)
    {
        bakeExp += exp;
        if (bakeExp >= nextLevelExp)
        {
            bakeLevel++;
            bakeExp -= nextLevelExp;
            nextLevelExp = (int)(nextLevelExp * 1.5f);
            // 升级触发通知
        }
    }

    // 厨师等级影响烘焙"最佳区间"宽度（更高等级 = 容错更大）
    float GetBakeBonus() const
    {
        // Lv1 → 0.0 (无加成)
        // Lv5 → 0.05 (最佳区间拓宽 5%)
        // Lv10 → 0.12 (最佳区间拓宽 12%)
        return (bakeLevel - 1) * 0.013f;
    }

private:
    // ... 现有字段 ...
    int bakeLevel;        // ★ 新增
    int bakeExp;          // ★ 新增
    int nextLevelExp;     // ★ 新增
};
```

**经验获取规则：**

| 事件 | 经验值 |
|------|-------|
| 出品一个甜品（不论品质） | +5 |
| Good 品质出品 | +15 |
| Perfect 品质出品 | +40 |
| Raw/Burnt 失败品 | +2（聊胜于无的安慰） |

**等级对烘焙的影响（GetBakeBonus）：**

```
厨师烘焙等级每升1级 → 品质判定区间拓宽 1.3%
即：Lv10 的厨师，判定 Perfect 的区间比 Lv1 宽了约 12%
```

体现在 `DetermineQuality` 中的 `ScoreRange` 函数——对高等级厨师，各指标的最佳区间边界向外扩展 `bakeBonus`。

---

## 6. UI 视觉效果

### 6.1 新增贴图资源

```
resources/texture/
  oven_bg.png              ← 烤箱背景（半透明窗口）
  oven_bar_bg.png          ← 进度条背景
  oven_bar_fill.png        ← 进度条填充（颜色渐变）
  oven_temp_low.png        ← 低火按钮
  oven_temp_med.png        ← 中火按钮
  oven_temp_high.png       ← 高火按钮
  oven_finish_btn.png      ← "出炉"按钮
  oven_done_btn.png        ← "出炉"按钮（高亮/可点击态）
  quality_raw.png          ← 品质标记：夹生
  quality_burnt.png        ← 品质标记：烤焦
  quality_overdone.png     ← 品质标记：略过
  quality_acceptable.png   ← 品质标记：一般
  quality_good.png         ← 品质标记：良好
  quality_perfect.png      ← 品质标记：完美
```

> 也可以用 `SDL_RenderFillRect` + 颜色绘制纯 UI，不依赖贴图，前期快速实现。

### 6.2 无贴图快速实现方案

品质指示可以用纯 SDL 绘制：

```cpp
void RenderQualityIndicator(SDL_Renderer* renderer, BakeQuality q, int x, int y)
{
    SDL_Color color;
    const char* label;
    switch (q)
    {
        case BakeQuality::Raw:        color = {200,200,200,255}; label="夹生"; break;
        case BakeQuality::Burnt:      color = {80,80,80,255};    label="烤焦"; break;
        case BakeQuality::Overdone:   color = {180,120,60,255};  label="略过"; break;
        case BakeQuality::Acceptable: color = {60,180,60,255};   label="一般"; break;
        case BakeQuality::Good:       color = {60,60,220,255};   label="良好"; break;
        case BakeQuality::Perfect:    color = {255,215,0,255};   label="完美"; break;
    }
    // 用 SDL_RenderFillRect + TTF_RenderUTF8_Blended 绘制
}
```

### 6.3 玩家指引

为了让玩家知道"应该在什么时候出炉"，UI 上提供两条提示线：

```
烘焙进度: ████████████░░░░░░ 65%
最佳区间: ░░░░░░░██████░░░░░░ ← 绿色范围
           ↑进度条进入此区间的时刻 = 适合出炉
```

绿色最佳区间的位置由蛋糕参数动态计算：
- 起始点 = 推荐时长 × 0.7（进入可接受区间）
- 结束点 = 推荐时长 × 1.3（开始过熟）

厨师等级增加时，这个区间会拓宽（`GetBakeBonus`）。

---

## 7. 完整制作流程（改造后）

```
1. Stage::size
   选盘子（小/中/大）→ SetPlateSize()
    → 下一步

2. Stage::base
   选基类蛋糕 → SetBaseID()
    → 下一步

3. Stage::create
   选最多3种风味材料 → SetCreateCase()
   （第三个选完后自动推进）
    → 下一步（改为 bake，原为 decorate）

4. 🔥 Stage::bake  ←★ NEW
   显示烤箱界面
   玩家控制温度 + 选择出炉时机
   确定后 LockQuality() → SetQuality()
    → 下一步

5. Stage::decorate
   选最多3种装饰材料 → SetDecorationCase()
   点击"完成"→ 创建 ProducedProduct
   → 保存，返回游戏
```

---

## 8. 存档改动

### 8.1 Product.rui 新格式

```
3            ← 产品总数
0 0 4 7 -1 0 3   ← 产品ID=0, 基类=0(戚风), 装饰=[4,7,-1], 盘=0(小), 品质=3(Perfect)
1 2 1 12 -1 1 2   ← 产品ID=1, 基类=2(巧克力蛋糕), 品质=2(Good)
```

### 8.2 Cook.rui 新格式

```
cook
default 200 50 5 120  ← 贴图 x y 烘焙等级 烘焙经验
end
```

### 8.3 GameSerializer 改动

`RUI_GameSerializer.cpp` 中：

- `Load()` — 读取 Product 时多读一个 `quality` 字段
- `Save()` — 写入 Product 时多写一个 `quality` 字段
- `Load()` — 读取 Cook 时多读 `bakeLevel` 和 `bakeExp`
- `Save()` — 写入 Cook 时多写 `bakeLevel` 和 `bakeExp`

---

## 9. 错误处理 / 边界情况

| 场景 | 处理 |
|------|------|
| 玩家点击"出炉"时机太早（Raw） | 弹出提示"蛋糕还没熟，确定要出炉吗？"，提供"再烤一会儿"选项 |
| 玩家烤焦（Burnt） | 直接提示"烤焦了，重新来吧"，可选择返回基类选择或重烤 |
| 未选择任何温度档位 | 默认中火（160°C），不强制玩家操作 |
| 蛋糕烤太久不管 | 超过最大时间后自动出炉，品质锁定为 Burnt |
| 厨师烘焙等级满级（Lv10）后 | 经验不再增加，但依然显示 Lv10 MAX |
| 无厨师（cooks 为空） | 使用默认参数（Lv1），正常进行烘焙 |
| 读旧存档（无 quality 字段） | 默认 Acceptable，兼容旧数据 |

---

## 10. 分阶段实施建议

### 第一阶段（核心可用，1~2天）

1. 新建 `RUI_BakingData.h` — 定义 `BakeQuality` enum
2. `ProducingProduct` / `ProducedProduct` 增加 quality 字段
3. `CreateRUIEvent::Stage` 插入 `bake`，修改 create→bake→decorate 流转
4. 烘焙小游戏用纯 SDL 绘制（无贴图，`SDL_RenderFillRect` 画界面）
5. 实现核心三条曲线 + 品质判定
6. Product.rui 存档增加 quality 字段
7. 完成一个最简可玩的烘焙流程

### 第二阶段（体验完善，2~3天）

1. 新增烤箱/进度条贴图，替换纯色绘制
2. 最佳区间绿色指引线
3. 品质指示器 + 文字提示
4. 焦了/夹生的确认弹窗
5. 新增 Cook 烘焙等级 + 经验系统
6. Cook 等级影响烘焙容错

### 第三阶段（进阶，按需）

1. 不同基类蛋糕有不同的"推荐温度/时长"
2. Desserts.rui 增加 bakeParams 三字段
3. 不同模具（圆/方/心）影响受热均匀度
4. 每种风味材料微调烘焙曲线（加巧克力的蛋糕更容易焦）

---

## 11. 关键代码引用

| 现有位置 | 用途 |
|---------|------|
| `RUI_CreateEvent.h:566` | Stage enum，插入 bake |
| `RUI_CreateEvent.h:151-174` | `update()`，加入烘焙更新 |
| `RUI_CreateEvent.h:176-286` | `onRender()`，加入烘焙渲染分支 |
| `RUI_CreateEvent.h:374-533` | `input()`，加入烘焙输入处理 |
| `RUI_CreateEvent.h:444-475` | Stage::create 的点击处理——需要改流转目标 |
| `RUI_Product.h:190-209` | `ProducingProduct::Calculate()`—烘焙后品质影响此处的计算 |
| `RUI_Product.h:243-268` | `ProducedProduct::LoadInit()`—加载品质字段 |
| `RUI_Product.h:301-306` | `GetProductPrice()`—价格公式不变，新增 `GetFinalPrice()` |
| `RUI_Cook.h:33-112` | Cook::onUpdate()—不会修改，但需要新加烘焙属性 |
| `RUI_GameSerializer.cpp:197-210` | Cook 读取——增加 bakeLevel/bakeExp |
| `RUI_DessertManager.h:18-54` | Desserts.rui 加载——增加烘焙参数读取 |
| `RUI_Dessert.h:15-36` | `Dessert::InitLevel()`——增加 bakeParams 参数 |

---

> **一句话总结**：在 Stage::create 之后插入 bake 阶段，用三条隐藏曲线（内部熟度/表面焦化/水分）模拟烘焙过程，玩家通过控制温度档位和出炉时机决定品质等级，品质影响价格系数，厨师烘焙等级影响操作容错。
