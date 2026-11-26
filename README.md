# Base1

SDL3 + C++ 手写的 2D 游戏框架与玩法原型（Bomberman 类）。
目的不是做一个通用引擎，而是在不依赖现成引擎的前提下，把 2D 游戏需要的几个基础子系统各自实现一遍，理清它们的职责边界与协作方式。

> 持续开发中（WIP）。

## 结构

```
main.cpp
  └─ Game ──────────── 窗口、主循环、帧时间
       └─ SceneManager ── 场景切换
            └─ IScene ─┬─ GameplayScene      主玩法场景
                       ├─ BombTestScene      炸弹逻辑验证场景
                       └─ GrenadeTestScene   手雷逻辑验证场景

Entity (抽象基类)
  ├─ Player
  ├─ Bomb / Grenade
  ├─ Block / BreakableBlock
  └─ Collectible / Door

支撑层：Math / Config / Resource / InputManager / SpriteAnimation / DebugDraw / DebugUi
```

`IScene` 定义完整生命周期：`Initialize / Update / Render / Shutdown / DrawDebugUi`。
`Entity` 以虚函数驱动 `Update / Render`，实体由 `GameplayScene` 用 `std::unique_ptr` 持有，通过模板工厂创建：

```cpp
template<typename T, typename... Args>
T *CreateEntity(Args &&...args);
```

## 几个主要实现点

### 输入系统：设备层与 Gameplay 层分离

两层枚举 + 映射表：

```cpp
enum Key { W, A, S, D, ArrowUp, ..., Space, F1, F2, F3, F4 };          // 设备层
enum class InputAction { MoveUp, MoveDown, ThrowGrenade, ... };        // Gameplay 层

std::map<InputAction, std::set<Key>> input_action_to_keys;
```

游戏逻辑只查询 `IsActionPressed(InputAction::MoveUp)`，不关心具体按键。value 用 `set<Key>` 是为了支持一个动作绑定多个键（`W` 和 `↑` 都能上移）。

提供完整的绑定管理：`BindKey` / `RebindKey` / `UnbindKey` / `UnbindAllKeys` / `IsKeyBound` / `IsKeyBoundToAnyAction` / `HasAllBindings`。

按键状态是 4 态而非 bool：

```cpp
enum class InputState { Pressed, Held, Released, Up };
```

区分「本帧刚按下」和「持续按住」，因为很多逻辑需要边沿触发而不是电平触发。

### 碰撞：X/Y 轴分离解算

如果把 XY 位移一起测试，斜向移动碰到墙时整体失败，角色会贴在墙上无法移动。这里分轴解算：

```cpp
Math::Rect GameplayScene::ResolveMovement(Entity const &moving_entity,
                                          Math::Rect const &next_bounds_x,
                                          Math::Rect const &next_bounds_y) const
{
    Math::Rect resolved_bounds = moving_entity.GetBounds();

    if (IsAreaFree(next_bounds_x, &moving_entity))
        resolved_bounds.position.x = next_bounds_x.position.x;

    // 用已解算的 x 构造 y 测试框
    Math::Rect const y_test_bounds{
        { resolved_bounds.position.x, next_bounds_y.position.y },
        resolved_bounds.size
    };

    if (IsAreaFree(y_test_bounds, &moving_entity))
        resolved_bounds.position.y = y_test_bounds.position.y;

    return resolved_bounds;
}
```

关键是第二次测试使用的是**已经解算过的 X 坐标**，这样被阻挡的轴失败、另一轴仍可通过，表现为沿墙滑动。

已知局限：离散检测，高速移动会穿透（需要扫掠检测）；`IsAreaFree` 暴力遍历全部实体，实体数量大时需要空间划分。

### 碰撞体与触发器分离

两者语义不同——碰撞体阻挡移动，触发器只产生事件。`Entity` 上给了独立的查询接口：

```cpp
virtual bool CanCheckCollision() const;   // 是否作为阻挡体参与移动解算
virtual bool CanCheckTrigger() const;     // 是否作为触发源参与触发检测
virtual bool CanRender() const;
virtual bool CanRenderDebugBounds() const;
```

于是 `Block` 走阻挡路径，`Collectible` / `Door` 走触发路径，同一实体也可以同时具备两种性质。触发分发按类型走不同处理：`HandleCollectibleTrigger` / `HandleDoorTrigger` / `HandleGenericTrigger`。

### 相机：跟随 + 世界边界约束

```cpp
float ClampCameraAxis(float desired_position, float world_size, float viewport_size);
void FollowTarget(Camera2D &camera, Vector2 const &center, Vector2 const &world_size);
Vector2 WorldToScreen(Vector2 const &world, Camera2D const &camera);
```

相机跟随目标中心，但按轴 clamp 到 `[0, world_size - viewport_size]`，避免走到地图边缘时画面露出世界外的空白。

### 内建调试设施

- `DebugDraw` / `DebugUi` / `DebugWorldGrid` / `DebugWorldMarker`
- 运行时开关（F1-F4）：debug overlay、碰撞日志、collectible 日志、trigger 日志、debug bounds、世界网格
- 默认值集中在 `Config::Debug`
- 日志按类别分开（`[collision]` / `[collectible]` / `[trigger]`），可独立开关

### 独立测试场景

`BombTestScene` 和 `GrenadeTestScene` 用于单独验证爆炸范围、引信时序、破坏逻辑，不必在主玩法场景里反复构造场景。

## 构建

依赖 SDL3，离线放置到 `external/SDL3`（需能找到 `SDL3Config.cmake`），或构建时指定路径。

```powershell
./build.ps1
```

常用参数：

```powershell
./build.ps1 -NoRun
./build.ps1 -Clean
./build.ps1 -Config Release
./build.ps1 -SDL3Root C:/path/to/SDL3
```

也可直接用 CMake：

```bash
cmake -S . -B build
cmake --build build --config Release
```

## 待办

- 空间划分替换碰撞的暴力遍历
- 扫掠检测处理高速移动穿透
- 帧动画系统补事件派发
- 资源管理补引用计数与卸载
- Tilemap 与关卡数据外部化
