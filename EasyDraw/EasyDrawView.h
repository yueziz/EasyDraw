#pragma once
#include "Core.h" // 必须包含，否则不知道什么是 ShapeType

class CEasyDrawView : public CView
{
protected:
	CEasyDrawView() noexcept;
	DECLARE_DYNCREATE(CEasyDrawView)

	// === 我们定义的变量 ===
public:
	CEasyDrawDoc* GetDocument() const;

	// 状态变量
	ShapeType m_currentType;     // 当前工具 (直线/矩形/椭圆/随手画)
	Color m_currentColor;        // 当前颜色
	bool m_isDrawing;            // 鼠标是否按住了
	Point m_startPoint;          // 鼠标按下的起点
	float m_lineWidth;           // 当前线宽

	// 预览图形 (拖拽过程中显示的虚影)
	std::shared_ptr<IShape> m_previewShape;

	// === 操作 ===
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

	// === 消息映射函数 (关联菜单和鼠标) ===
public:
	virtual ~CEasyDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// 1. 鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// 2. 双缓冲防闪烁 (屏蔽默认背景擦除)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// 3. 菜单命令 (点击按钮触发)
	afx_msg void OnToolLine();
	afx_msg void OnToolRect();
	afx_msg void OnToolEllipse(); // <--- 新增：椭圆工具
	afx_msg void OnToolFree();

	afx_msg void OnColorRed();
	afx_msg void OnColorBlue();

	afx_msg void OnWidthThin();
	afx_msg void OnWidthThick();

	afx_msg void OnUndo();
	afx_msg void OnRedo();

	// 4. UI 状态更新 (控制按钮是否变灰/按下)
	afx_msg void OnUpdateToolLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolEllipse(CCmdUI* pCmdUI); // <--- 新增：椭圆状态
	afx_msg void OnUpdateToolFree(CCmdUI* pCmdUI);

	afx_msg void OnUpdateColorRed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorBlue(CCmdUI* pCmdUI);

	afx_msg void OnUpdateWidthThin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWidthThick(CCmdUI* pCmdUI);

	// === F-10 多语言支持 ===
	afx_msg void OnLangCN();
	afx_msg void OnLangEN();
	afx_msg void OnLangTC();

	// 辅助函数：执行换肤
	void ChangeMenu(UINT menuID);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CEasyDrawDoc* CEasyDrawView::GetDocument() const
{
	return reinterpret_cast<CEasyDrawDoc*>(m_pDocument);
}
#endif
