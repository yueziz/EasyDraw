// ========================================================
// 单元测试文件 (终极修正版)
// 适用环境：任何版本的 Visual Studio (Community/Enterprise)
// 注意：请确保在项目属性 -> C/C++ -> 预编译头 中选择“不使用预编译头”
// ========================================================

#include <gtest/gtest.h>

// 引入 Windows 基础头文件 (解决 GDI+ 报错的关键)
#include <windows.h>
#include <objidl.h>

// 引入项目核心头文件
// 如果报错找不到文件，请在属性 -> C/C++ -> 常规 -> 附加包含目录中添加 $(SolutionDir)EasyDraw
#include "Core.h"       
#include "Serializer.h" 

#include <cstdio>       // 用于 remove
#include <vector>
#include <memory>

// ========================================================
// GDI+ 测试环境 (必须有这个，否则测试会崩)
// ========================================================
class GdiPlusFixture : public ::testing::Test {
protected:
    static ULONG_PTR gdiplusToken;
    static GdiplusStartupInput gdiplusStartupInput;

    static void SetUpTestSuite() {
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }
    static void TearDownTestSuite() {
        GdiplusShutdown(gdiplusToken);
    }
};

ULONG_PTR GdiPlusFixture::gdiplusToken = 0;
GdiplusStartupInput GdiPlusFixture::gdiplusStartupInput;

// ========================================================
// 测试 1: 工厂模式覆盖
// ========================================================

TEST_F(GdiPlusFixture, Factory_CreateLine) {
    auto shape = ShapeFactory::Create(ShapeType::Line, Point(10, 10));
    ASSERT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetType(), ShapeType::Line);

    // 验证属性
    auto line = std::dynamic_pointer_cast<LineShape>(shape);
    ASSERT_NE(line, nullptr);
    EXPECT_EQ(line->start.X, 10);
}

TEST_F(GdiPlusFixture, Factory_CreateRect) {
    auto shape = ShapeFactory::Create(ShapeType::Rect, Point(50, 50));
    ASSERT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetType(), ShapeType::Rect);
}

TEST_F(GdiPlusFixture, Factory_CreateEllipse) {
    auto shape = ShapeFactory::Create(ShapeType::Ellipse, Point(20, 20));
    ASSERT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetType(), ShapeType::Ellipse);
}

TEST_F(GdiPlusFixture, Factory_CreateFreehand) {
    auto shape = ShapeFactory::Create(ShapeType::Freehand, Point(5, 5));
    ASSERT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetType(), ShapeType::Freehand);

    auto freehand = std::dynamic_pointer_cast<FreehandShape>(shape);
    ASSERT_NE(freehand, nullptr);
    EXPECT_FALSE(freehand->points.empty());
}

// ========================================================
// 测试 2: 序列化覆盖
// ========================================================
class SerializerTest : public GdiPlusFixture {
protected:
    std::string testFile = "test_data.txt";
    void TearDown() override {
        std::remove(testFile.c_str());
    }
};

TEST_F(SerializerTest, SaveAndLoad_Integration) {
    std::vector<std::shared_ptr<IShape>> originalShapes;

    // 1. 创建直线
    auto line = std::make_shared<LineShape>(Point(0, 0), Point(100, 100));
    line->SetStyle(Color::Red, 5.0f);
    originalShapes.push_back(line);

    // 2. 创建矩形
    auto rect = std::make_shared<RectShape>(Rect(10, 10, 200, 150));
    rect->SetStyle(Color::Blue, 2.0f);
    originalShapes.push_back(rect);

    // 3. 保存
    ASSERT_TRUE(Serializer::Save(originalShapes, testFile));

    // 4. 加载
    std::vector<std::shared_ptr<IShape>> loadedShapes;
    ASSERT_TRUE(Serializer::Load(loadedShapes, testFile));

    // 5. 验证
    ASSERT_EQ(loadedShapes.size(), 2);

    // 检查直线属性
    auto pLine = std::dynamic_pointer_cast<LineShape>(loadedShapes[0]);
    ASSERT_NE(pLine, nullptr);
    EXPECT_EQ(pLine->GetType(), ShapeType::Line);
    EXPECT_EQ(pLine->color.GetValue(), Color::Red);
    EXPECT_FLOAT_EQ(pLine->width, 5.0f);

    // 检查矩形属性
    auto pRect = std::dynamic_pointer_cast<RectShape>(loadedShapes[1]);
    ASSERT_NE(pRect, nullptr);
    EXPECT_EQ(pRect->GetType(), ShapeType::Rect);
    EXPECT_EQ(pRect->rect.Width, 200);
}