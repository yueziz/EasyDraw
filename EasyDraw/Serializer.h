#pragma once
#include "Core.h"
#include <vector>
#include <string>

class Serializer {
public:
    // 保存所有图形到文件
    static bool Save(const std::vector<std::shared_ptr<IShape>>& shapes, const std::string& filePath);

    // 从文件加载所有图形
    static bool Load(std::vector<std::shared_ptr<IShape>>& shapes, const std::string& filePath);
};