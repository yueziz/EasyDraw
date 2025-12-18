 EasyDraw 项目 AI 辅助开发日志

项目名称：EasyDraw 矢量绘图系统

记录时间：2025-12-01 至 2023-12-20

主要模型：Gemini 

开发环境：Visual Studio, C++, MFC, GoogleTest

1. 核心架构与设计阶段

1.1 图形类结构设计

任务：利用c++做一个简易绘图板(GDI+撤销/重做)。
输入提示词 (Prompt)：

我正在用 C++ 和 MFC 开发一个绘图软件。请帮我设计核心图形类 Core.h。
要求：

定义一个基类 IShape，包含颜色、线宽属性和纯虚函数 Draw。

实现直线、矩形、椭圆的子类。

使用 std::shared_ptr 进行内存管理。

提供一个 RAII 风格的 GDI+ 初始化类 CGdiObjectWrapper。

AI 生成内容摘要：

提供了 IShape 抽象基类定义。

实现了 LineShape, RectShape, EllipseShape。

提供了 CGdiObjectWrapper 用于自动管理 GdiplusStartup 和 GdiplusShutdown。

1.2 设计模式实现

任务：引入工厂模式和命令模式以支持扩展性和撤销功能。
输入提示词 (Prompt)：

在现有的 Core.h 基础上，请添加以下设计模式的实现：

简单工厂模式：ShapeFactory，根据枚举 ShapeType 创建图形对象。

命令模式：IAppCommand 接口和 CommandManager 类，利用两个栈 (undoStack, redoStack) 实现撤销和重做功能。

AI 生成内容摘要：

生成了 ShapeFactory::Create 方法。

定义了 IModelActions 接口解耦文档与命令。

实现了 DrawCommand 和 CommandManager。

2. 功能实现阶段

2.1 序列化 (F-09 需求)

任务：实现图形数据的保存与加载，不使用 MFC 的 CArchive，要求自定义文本格式。
输入提示词 (Prompt)：

请为 IShape 添加 Save(ostream&) 和 Load(istream&) 接口。
然后编写 Serializer 类，实现将 std::vector<shared_ptr<IShape>> 保存到文本文件和从文件读取的功能。
注意：需要处理多态对象的创建，利用之前定义的工厂类。

AI 生成内容摘要：

在 Core.h 中添加了序列化虚函数。

生成了 Serializer.h 和 Serializer.cpp。

实现了基于 TypeID -> Color -> Data 顺序的流式读写逻辑。

2.2 多语言支持与菜单修复

任务：解决 MFC 切换菜单时句柄销毁导致的崩溃问题。
输入提示词 (Prompt)：

我在 MFC View 中切换菜单资源时程序崩溃了。
代码是 menu.LoadMenu(...) 然后 SetMenu(...)。请分析原因并修复。

AI 生成内容摘要：

分析：指出 CMenu 是局部变量，析构时会销毁 Windows 菜单句柄，导致 SetMenu 设置了无效句柄。

修复：在 MainFrm.cpp 中提供了 SwitchLanguage 函数，使用 newMenu.Detach() 分离句柄所有权，确保菜单资源驻留。

3. 测试与质量保证阶段

3.1 单元测试环境搭建

任务：在 MFC 项目中集成 GoogleTest，并解决 GDI+ 依赖。
输入提示词 (Prompt)：

我需要对 ShapeFactory 和 Serializer 进行单元测试。
请提供一个 UnitTests.cpp 文件。
注意：因为使用了 GDI+ 对象，测试开始前必须初始化 GDI+，否则会崩溃。请使用 GoogleTest 的 Fixture 功能。

AI 生成内容摘要：

生成了 GdiPlusFixture 类，在 SetUpTestSuite 中初始化 GDI+。

编写了针对 ShapeFactory 创建逻辑的测试用例。

编写了针对 Serializer 保存后加载数据一致性的集成测试用例。

3.2 解决预编译头冲突 (Key Bug Fix)

任务：解决 MFC 项目 (pch.h) 与 GoogleTest 控制台项目不兼容导致的编译错误。
输入提示词 (Prompt)：

Serializer.cpp 被主程序和测试程序共用。
主程序是 MFC，需要 #include "pch.h"。测试程序是控制台，不需要。
编译报错 C1010: unexpected end of file while looking for precompiled header。请提供解决方案。

AI 生成内容摘要：

方案：建议在测试项目属性中对 Serializer.cpp 设置“不使用预编译头”。

代码修改：在 Serializer.cpp 中移除 pch.h，并显式包含 <windows.h> 以解决 GDI+ 依赖缺失的问题。

3.3 代码覆盖率优化

任务：解决 Visual Studio 代码覆盖率显示为 0% 的问题。
输入提示词 (Prompt)：

单元测试全通过了，但 VS 企业版的代码覆盖率显示 ShapeFactory 是 0%。
我的代码实现都写在头文件里。

AI 生成内容摘要：

分析：编译器对头文件中的函数进行了内联 (Inline) 优化，导致覆盖率工具无法插桩。

操作指南：指导修改项目属性 -> C/C++ -> 优化 -> 内联函数扩展 设置为 已禁用 (/Ob0)。

4. 文档生成阶段

4.1 README 生成

任务：生成项目 GitHub 文档。
输入提示词 (Prompt)：

根据 Core.h (架构), Serializer.cpp (功能) 和 UnitTests.cpp (测试)，为这个项目生成一份专业的 GitHub README.md。
包含功能特性、技术栈、设计模式说明、快速开始和测试截图占位符。

AI 生成内容摘要：

生成了包含 Shields.io 徽章的 Markdown 文档。

自动总结了 SOLID 原则和 MVC 架构在项目中的应用。

提供了清晰的编译和测试运行指南。

5. 总结

在 EasyDraw 项目中，AI 辅助完成了约 60% 的基础代码编写（核心类、序列化逻辑）以及 90% 的单元测试代码。特别是在解决 MFC 资源句柄管理 和 混合项目编译配置 等棘手问题上，AI 提供了关键的解决方案，显著缩短了开发周期。
