#pragma once
#include "Core.h" // 必须包含 Core.h (v3.1)，其中定义了 IShape, DrawCommand, CommandManager

// CEasyDrawDoc 类
// 继承 IModelActions 是为了让 Command 对象能回调 Doc 进行数据修改
class CEasyDrawDoc : public CDocument, public IModelActions
{
protected: // 仅从序列化创建
	CEasyDrawDoc() noexcept;
	DECLARE_DYNCREATE(CEasyDrawDoc)

	// === 数据成员 ===
public:
	// 存放所有图形的容器
	std::vector<std::shared_ptr<IShape>> m_shapes;

	// 命令管理器 (Undo/Redo)
	CommandManager m_cmdMgr;

	// === IModelActions 接口实现 ===
	// DrawCommand 会调用这两个函数来真正修改数据
public:
	void AddShapeToModel(std::shared_ptr<IShape> s) override {
		m_shapes.push_back(s);
		UpdateAllViews(NULL); // 数据变了，通知视图重绘
	}

	void RemoveShapeFromModel() override {
		if (!m_shapes.empty()) {
			m_shapes.pop_back();
			UpdateAllViews(NULL); // 数据变了，通知视图重绘
		}
	}

	// === 视图 (View) 交互接口 ===
	// 当用户画完一个图形，View 调用此函数提交
	void CommitShape(std::shared_ptr<IShape> s) {
		// 创建一个绘制命令
		// 注意：这里需要 Core.h 中有 DrawCommand 的完整定义
		auto cmd = std::make_shared<DrawCommand>(*this, s);

		// 执行命令 (Execute 内部会调用 AddShapeToModel)
		m_cmdMgr.Execute(cmd);
	}

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// === F-09 单元测试要求：重写序列化流程 ===
	// 必须在此声明，否则 EasyDrawDoc.cpp 里的实现会报错 "member function not declared"
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 实现
public:
	virtual ~CEasyDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于搜索处理程序的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};