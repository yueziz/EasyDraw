#pragma once
// =========================================================
// File: Core.h
// 版本: 3.1 (整合了椭圆功能 + 修复 Windows ICommand 冲突 + 序列化支持)
// =========================================================

#ifndef NOMINMAX
#define NOMINMAX 
#endif

#include <vector>
#include <stack>
#include <memory>
#include <algorithm>
#include <iostream> // 用于序列化
#include <gdiplus.h>

// 链接 GDI+ 库
#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

// GDI+ 初始化助手
class CGdiObjectWrapper {
private:
    GdiplusStartupInput input;
    ULONG_PTR token;
public:
    CGdiObjectWrapper() { GdiplusStartup(&token, &input, NULL); }
    ~CGdiObjectWrapper() { GdiplusShutdown(token); }
};

// ==========================================
// 绘图逻辑
// ==========================================

// 图形类型枚举
enum class ShapeType { Line, Rect, Ellipse, Freehand, Unknown };

// 图形基类
class IShape {
public:
    Color color;
    float width;
    IShape() : color(Color::Black), width(2.0f) {}
    virtual ~IShape() = default;

    virtual void Draw(Graphics* g) = 0;
    virtual std::shared_ptr<IShape> Clone() const = 0;

    // === 序列化接口 ===
    virtual ShapeType GetType() const = 0;
    virtual void Save(std::ostream& os) const = 0;
    virtual void Load(std::istream& is) = 0;

    void SetStyle(Color c, float w) { color = c; width = w; }

    // 辅助：保存/加载通用属性 (颜色, 线宽)
    void SaveBase(std::ostream& os) const {
        os << (int)GetType() << " " << color.GetValue() << " " << width << " ";
    }
    void LoadBase(std::istream& is) {
        ARGB argb;
        is >> argb >> width;
        color.SetValue(argb);
    }
};

// 直线
class LineShape : public IShape {
public:
    Point start, end;
    LineShape(Point s, Point e) : start(s), end(e) {}

    void Draw(Graphics* g) override {
        Pen pen(color, width);
        g->DrawLine(&pen, start, end);
    }
    std::shared_ptr<IShape> Clone() const override {
        auto s = std::make_shared<LineShape>(start, end);
        s->SetStyle(color, width);
        return s;
    }
    ShapeType GetType() const override { return ShapeType::Line; }

    void Save(std::ostream& os) const override {
        SaveBase(os);
        os << start.X << " " << start.Y << " " << end.X << " " << end.Y << std::endl;
    }
    void Load(std::istream& is) override {
        LoadBase(is);
        is >> start.X >> start.Y >> end.X >> end.Y;
    }
};

// 矩形
class RectShape : public IShape {
public:
    Rect rect;
    RectShape(Rect r) : rect(r) {}

    void Draw(Graphics* g) override {
        Pen pen(color, width);
        g->DrawRectangle(&pen, rect);
    }
    std::shared_ptr<IShape> Clone() const override {
        auto s = std::make_shared<RectShape>(rect);
        s->SetStyle(color, width);
        return s;
    }
    ShapeType GetType() const override { return ShapeType::Rect; }

    void Save(std::ostream& os) const override {
        SaveBase(os);
        os << rect.X << " " << rect.Y << " " << rect.Width << " " << rect.Height << std::endl;
    }
    void Load(std::istream& is) override {
        LoadBase(is);
        is >> rect.X >> rect.Y >> rect.Width >> rect.Height;
    }
};

// 椭圆
class EllipseShape : public IShape {
public:
    Rect rect;
    EllipseShape(Rect r) : rect(r) {}

    void Draw(Graphics* g) override {
        Pen pen(color, width);
        g->DrawEllipse(&pen, rect);
    }
    std::shared_ptr<IShape> Clone() const override {
        auto s = std::make_shared<EllipseShape>(rect);
        s->SetStyle(color, width);
        return s;
    }
    ShapeType GetType() const override { return ShapeType::Ellipse; }

    void Save(std::ostream& os) const override {
        SaveBase(os);
        os << rect.X << " " << rect.Y << " " << rect.Width << " " << rect.Height << std::endl;
    }
    void Load(std::istream& is) override {
        LoadBase(is);
        is >> rect.X >> rect.Y >> rect.Width >> rect.Height;
    }
};

// 随手画
class FreehandShape : public IShape {
public:
    std::vector<Point> points;
    void AddPoint(Point p) { points.push_back(p); }

    void Draw(Graphics* g) override {
        if (points.size() < 2) return;
        Pen pen(color, width);
        pen.SetLineJoin(LineJoinRound);
        g->DrawLines(&pen, points.data(), (INT)points.size());
    }
    std::shared_ptr<IShape> Clone() const override {
        auto s = std::make_shared<FreehandShape>();
        s->points = points;
        s->SetStyle(color, width);
        return s;
    }
    ShapeType GetType() const override { return ShapeType::Freehand; }

    void Save(std::ostream& os) const override {
        SaveBase(os);
        os << points.size() << " ";
        for (const auto& p : points) {
            os << p.X << " " << p.Y << " ";
        }
        os << std::endl;
    }
    void Load(std::istream& is) override {
        LoadBase(is);
        size_t count = 0;
        is >> count;
        points.clear();
        for (size_t i = 0; i < count; ++i) {
            int x, y;
            is >> x >> y;
            points.emplace_back(x, y);
        }
    }
};

// 图形工厂
class ShapeFactory {
public:
    static std::shared_ptr<IShape> Create(ShapeType type, Point start) {
        if (type == ShapeType::Line) return std::make_shared<LineShape>(start, start);
        if (type == ShapeType::Rect) return std::make_shared<RectShape>(Rect(start.X, start.Y, 0, 0));
        if (type == ShapeType::Ellipse) return std::make_shared<EllipseShape>(Rect(start.X, start.Y, 0, 0));
        if (type == ShapeType::Freehand) {
            auto s = std::make_shared<FreehandShape>();
            s->AddPoint(start);
            return s;
        }
        return nullptr;
    }
};

// ==========================================
// 命令模式 (Undo/Redo)
// ==========================================

class IModelActions {
public:
    virtual void AddShapeToModel(std::shared_ptr<IShape> s) = 0;
    virtual void RemoveShapeFromModel() = 0;
    virtual ~IModelActions() = default;
};

class IAppCommand {
public:
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual ~IAppCommand() = default;
};

class DrawCommand : public IAppCommand {
    IModelActions& model;
    std::shared_ptr<IShape> shape;
public:
    DrawCommand(IModelActions& m, std::shared_ptr<IShape> s) : model(m), shape(s) {}
    void Execute() override { model.AddShapeToModel(shape); }
    void Undo() override { model.RemoveShapeFromModel(); }
};

class CommandManager {
    std::stack<std::shared_ptr<IAppCommand>> undoStack;
    std::stack<std::shared_ptr<IAppCommand>> redoStack;
public:
    void Execute(std::shared_ptr<IAppCommand> cmd) {
        cmd->Execute();
        undoStack.push(cmd);
        while (!redoStack.empty()) redoStack.pop();
    }

    void Undo() {
        if (undoStack.empty()) return;
        auto cmd = undoStack.top(); undoStack.pop();
        cmd->Undo();
        redoStack.push(cmd);
    }

    void Redo() {
        if (redoStack.empty()) return;
        auto cmd = redoStack.top(); redoStack.pop();
        cmd->Execute();
        undoStack.push(cmd);
    }
};