// =========================================================
// File: Serializer.cpp
// 注意：此文件不再包含 pch.h
// 请确保在 EasyDraw 和 EasyDrawTests 项目属性中，
// 对此文件都设置了“不使用预编译头”！
// =========================================================

#include <windows.h> 
#include "Serializer.h"
// #include "pch.h" <--- 已移除，防止 C1020 错误
#include <fstream>
#include <iostream>

bool Serializer::Save(const std::vector<std::shared_ptr<IShape>>& shapes, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) return false;

    // 1. 写入图形总数
    outFile << shapes.size() << std::endl;

    // 2. 遍历保存每个图形
    for (const auto& shape : shapes) {
        if (shape) {
            // 多态调用：会先进入各个 Shape 的 Save，然后调用 SaveBase
            shape->Save(outFile);
        }
    }
    outFile.close();
    return true;
}

bool Serializer::Load(std::vector<std::shared_ptr<IShape>>& shapes, const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) return false;

    // 1. 读取图形总数
    size_t count = 0;
    if (!(inFile >> count)) return false;

    shapes.clear();
    for (size_t i = 0; i < count; ++i) {
        int typeInt;
        // 2. 预读类型 ID
        if (!(inFile >> typeInt)) break;

        ShapeType type = static_cast<ShapeType>(typeInt);

        // 3. 使用工厂创建“空”对象
        // ShapeFactory 在 Core.h 中定义 (Serializer.h 包含了 Core.h)
        auto shape = ShapeFactory::Create(type, Point(0, 0));

        if (shape) {
            // 4. 恢复数据
            shape->Load(inFile);
            shapes.push_back(shape);
        }
    }
    inFile.close();
    return true;
}