# 🎨 EasyDraw - Vector Graphics System

**EasyDraw** 是一个基于 Windows MFC 框架和 GDI+ 图形引擎开发的轻量级面向对象矢量绘图系统。

该项目旨在演示现代 C++ (Smart Pointers) 在传统 MFC 框架中的应用，并实现了经典的设计模式（工厂模式、命令模式）以支持可扩展的图形系统和撤销/重做功能。

## ✨ 功能特性 (Features)

* **🛠️ 多种绘图工具**：支持直线、矩形、椭圆、随手画 (Freehand) 等基础图形绘制。

* **🎨 样式控制**：支持画笔颜色切换（红/蓝）及线宽调整（粗/细）。

* **↩️ 撤销与重做**：基于命令模式 (Command Pattern) 实现无限级（受内存限制）撤销/重做。

* **💾 持久化存储**：支持自定义格式的文件保存 (Save) 与加载 (Load)，实现图形数据的序列化与反序列化。

* **⚡ 高效渲染**：采用双缓冲技术 (Double Buffering) 彻底消除绘图闪烁，利用 GDI+ 实现抗锯齿渲染。

* **🌐 多语言支持**：支持中文/英文界面动态切换（解决了 MFC 菜单资源句柄管理的 Crash 问题）。

* **🖱️ 交互体验**：支持鼠标拖拽实时预览绘图效果。

## 🏗️ 系统架构与技术栈 (Architecture)

### 技术栈

* **语言标准**: C++14 / C++17

* **UI 框架**: Microsoft Foundation Classes (MFC)

* **图形引擎**: GDI+ (封装为 `CGdiObjectWrapper` 实现 RAII 资源管理)

* **单元测试**: GoogleTest (gtest)

### 设计模式 (Design Patterns)

本项目严格遵循 **SOLID** 原则，应用了以下设计模式：

1. **工厂模式 (Factory Pattern)**:

   * `ShapeFactory` 类负责根据 `ShapeType` 枚举创建具体的图形对象 (`LineShape`, `RectShape` 等)，实现了对修改关闭，对扩展开放。

2. **命令模式 (Command Pattern)**:

   * 通过 `IAppCommand` 接口和 `DrawCommand` 类封装用户的绘图请求。

   * `CommandManager` 使用双栈结构 (`undoStack`, `redoStack`) 管理操作历史。

3. **原型模式 (Prototype Pattern)**:

   * `IShape::Clone()` 接口支持对象的深拷贝，用于预览和潜在的复制粘贴功能。

4. **MVC 变体 (Document-View)**:

   * **Model**: `Core.h` 中的图形类及 `CEasyDrawDoc` (数据管理)。

   * **View**: `CEasyDrawView` (渲染与交互)。

   * **Controller**: 消息映射机制处理用户输入。

## 🧪 单元测试与代码质量 (Unit Testing)

项目集成了 **GoogleTest** 框架，针对核心业务逻辑进行了严格的单元测试，代码覆盖率 **> 70%**。

* **测试范围**:

  * `ShapeFactory`: 验证所有图形类型的创建逻辑及边界条件。

  * `Serializer`: 验证文件保存与读取的数据一致性（集成测试）。

* **运行测试**:

  * 打开 Visual Studio 的“测试资源管理器” (Test Explorer)。

  * 运行 `EasyDrawTests` 项目。

## 🚀 快速开始 (Getting Started)

### 环境要求

* Visual Studio 2019 或 2022

* 工作负载：使用 C++ 的桌面开发 (Desktop development with C++)

* 组件：MFC for v142/v143 build tools

### 编译与运行

1. 克隆本仓库：

2. 使用 Visual Studio 打开 `EasyDraw.sln`。

3. 确保启动项目设置为 **EasyDraw**。

4. 选择配置 (Debug/Release, x64/x86)。

5. 点击 **运行 (F5)**。

### 运行单元测试

1. 在解决方案中右键点击 `EasyDrawTests` 项目。

2. 选择 **生成 (Build)**。

3. 打开菜单栏 `测试 (Test)` -> `运行所有测试 (Run All Tests)`。

## 📂 项目结构 (Project Structure)

 EasyDraw/
 
 ├── Core.h              # 核心模型定义 (IShape, ShapeFactory, CommandManager)
 
 ├── Serializer.h/.cpp   # 文件序列化逻辑
 
 ├── EasyDrawDoc.h/.cpp  # 文档类 (管理数据模型)
 
 ├── EasyDrawView.h/.cpp # 视图类 (处理绘图交互与渲染)
 
 ├── MainFrm.h/.cpp      # 主框架窗口 (处理菜单与多语言)
 
 ├── EasyDraw.h/.cpp     # 应用程序入口与 GDI+ 初始化
 
 └── res/                # 资源文件 (图标, 菜单, 字符串表)

 
 EasyDrawTests/          # 单元测试项目
 
 ├── UnitTests.cpp       # GoogleTest 测试用例
 
 └── ...


## 📷 截图 (Screenshots)

### 主界面与绘图

*(此处建议替换为实际运行截图)*

### 单元测试通过

*(此处建议替换为单元测试全绿的截图)*
