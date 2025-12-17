#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在项目属性中定义
#ifndef SHARED_HANDLERS
#include "EasyDraw.h"
#endif

#include "EasyDrawDoc.h"
#include "EasyDrawView.h"
#include "MainFrm.h" // <--- 【关键修改 1】必须引入 MainFrm 头文件才能调用 SwitchLanguage

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 关联类
IMPLEMENT_DYNCREATE(CEasyDrawView, CView)

// =================================================================
// 消息映射表
// =================================================================
BEGIN_MESSAGE_MAP(CEasyDrawView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND() // 双缓冲关键：屏蔽系统背景擦除

	ON_COMMAND(ID_TOOL_LINE, &CEasyDrawView::OnToolLine)
	ON_COMMAND(ID_TOOL_RECT, &CEasyDrawView::OnToolRect)
	ON_COMMAND(ID_TOOL_ELLIPSE, &CEasyDrawView::OnToolEllipse)
	ON_COMMAND(ID_TOOL_FREE, &CEasyDrawView::OnToolFree)

	ON_COMMAND(ID_COLOR_RED, &CEasyDrawView::OnColorRed)
	ON_COMMAND(ID_COLOR_BLUE, &CEasyDrawView::OnColorBlue)

	ON_COMMAND(ID_WIDTH_THIN, &CEasyDrawView::OnWidthThin)
	ON_COMMAND(ID_WIDTH_THICK, &CEasyDrawView::OnWidthThick)

	ON_COMMAND(ID_EDIT_UNDO_OP, &CEasyDrawView::OnUndo)
	ON_COMMAND(ID_EDIT_REDO_OP, &CEasyDrawView::OnRedo)

	ON_UPDATE_COMMAND_UI(ID_TOOL_LINE, &CEasyDrawView::OnUpdateToolLine)
	ON_UPDATE_COMMAND_UI(ID_TOOL_RECT, &CEasyDrawView::OnUpdateToolRect)
	ON_UPDATE_COMMAND_UI(ID_TOOL_ELLIPSE, &CEasyDrawView::OnUpdateToolEllipse)
	ON_UPDATE_COMMAND_UI(ID_TOOL_FREE, &CEasyDrawView::OnUpdateToolFree)

	ON_UPDATE_COMMAND_UI(ID_COLOR_RED, &CEasyDrawView::OnUpdateColorRed)
	ON_UPDATE_COMMAND_UI(ID_COLOR_BLUE, &CEasyDrawView::OnUpdateColorBlue)

	ON_UPDATE_COMMAND_UI(ID_WIDTH_THIN, &CEasyDrawView::OnUpdateWidthThin)
	ON_UPDATE_COMMAND_UI(ID_WIDTH_THICK, &CEasyDrawView::OnUpdateWidthThick)

	ON_COMMAND(ID_LANG_CN, &CEasyDrawView::OnLangCN)
	ON_COMMAND(ID_LANG_EN, &CEasyDrawView::OnLangEN)
	ON_COMMAND(ID_LANG_TC, &CEasyDrawView::OnLangTC)

END_MESSAGE_MAP()

// =================================================================
// 构造函数
// =================================================================
CEasyDrawView::CEasyDrawView() noexcept
{
	m_currentType = ShapeType::Line;
	m_currentColor = Color::Black;
	m_lineWidth = 2.0f;
	m_isDrawing = false;
}

CEasyDrawView::~CEasyDrawView() {}

BOOL CEasyDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// =================================================================
// 核心绘制 (双缓冲版本)
// =================================================================
void CEasyDrawView::OnDraw(CDC* pDC)
{
	CEasyDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// 1. 获取当前窗口的大小
	CRect rect;
	GetClientRect(&rect);
	if (rect.Width() == 0 || rect.Height() == 0) return;

	// 2. 创建内存画布 (Bitmap)
	Bitmap backBuffer(rect.Width(), rect.Height(), PixelFormat32bppARGB);

	// 3. 创建指向内存画布的画笔
	Graphics gMemory(&backBuffer);
	gMemory.SetSmoothingMode(SmoothingModeAntiAlias);

	// 4. 【在内存里画画】
	gMemory.Clear(Color::White); // 先洗白

	// 4.1 画历史图形
	for (const auto& shape : pDoc->m_shapes) {
		shape->Draw(&gMemory);
	}

	// 4.2 画当前预览图形
	if (m_isDrawing && m_previewShape) {
		m_previewShape->Draw(&gMemory);
	}

	// 5. 【一次性贴图】
	Graphics gScreen(pDC->GetSafeHdc());
	gScreen.DrawImage(&backBuffer, 0, 0);
}

// =================================================================
// 鼠标交互
// =================================================================
void CEasyDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_isDrawing = true;
	m_startPoint = Point(point.x, point.y);

	m_previewShape = ShapeFactory::Create(m_currentType, m_startPoint);

	if (m_previewShape) {
		m_previewShape->SetStyle(m_currentColor, m_lineWidth);
	}

	CView::OnLButtonDown(nFlags, point);
}

void CEasyDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDrawing && m_previewShape) {
		Point endPoint(point.x, point.y);

		// 1. 直线
		if (auto line = std::dynamic_pointer_cast<LineShape>(m_previewShape)) {
			line->end = endPoint;
		}
		// 2. 矩形
		else if (auto rect = std::dynamic_pointer_cast<RectShape>(m_previewShape)) {
			int x = min(m_startPoint.X, endPoint.X);
			int y = min(m_startPoint.Y, endPoint.Y);
			int w = abs(endPoint.X - m_startPoint.X);
			int h = abs(endPoint.Y - m_startPoint.Y);
			rect->rect = Rect(x, y, w, h);
		}
		// 3. 椭圆
		else if (auto ellipse = std::dynamic_pointer_cast<EllipseShape>(m_previewShape)) {
			int x = min(m_startPoint.X, endPoint.X);
			int y = min(m_startPoint.Y, endPoint.Y);
			int w = abs(endPoint.X - m_startPoint.X);
			int h = abs(endPoint.Y - m_startPoint.Y);
			ellipse->rect = Rect(x, y, w, h);
		}
		// 4. 随手画
		else if (auto free = std::dynamic_pointer_cast<FreehandShape>(m_previewShape)) {
			free->AddPoint(endPoint);
		}

		Invalidate(FALSE); // 触发重绘
	}
	CView::OnMouseMove(nFlags, point);
}

void CEasyDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_isDrawing && m_previewShape) {
		CEasyDrawDoc* pDoc = GetDocument();
		if (pDoc) {
			pDoc->CommitShape(m_previewShape);
		}
		m_isDrawing = false;
		m_previewShape = nullptr;
	}
	CView::OnLButtonUp(nFlags, point);
}

// =================================================================
// 菜单命令
// =================================================================
void CEasyDrawView::OnToolLine() { m_currentType = ShapeType::Line; }
void CEasyDrawView::OnToolRect() { m_currentType = ShapeType::Rect; }
void CEasyDrawView::OnToolEllipse() { m_currentType = ShapeType::Ellipse; }
void CEasyDrawView::OnToolFree() { m_currentType = ShapeType::Freehand; }

void CEasyDrawView::OnColorRed() { m_currentColor = Color::Red; }
void CEasyDrawView::OnColorBlue() { m_currentColor = Color::Blue; }

void CEasyDrawView::OnWidthThin() { m_lineWidth = 2.0f; }
void CEasyDrawView::OnWidthThick() { m_lineWidth = 10.0f; }

void CEasyDrawView::OnUndo() {
	CEasyDrawDoc* pDoc = GetDocument();
	if (pDoc) pDoc->m_cmdMgr.Undo();
	Invalidate();
}

void CEasyDrawView::OnRedo() {
	CEasyDrawDoc* pDoc = GetDocument();
	if (pDoc) pDoc->m_cmdMgr.Redo();
	Invalidate();
}

// =================================================================
// UI 状态更新
// =================================================================

void CEasyDrawView::OnUpdateToolLine(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentType == ShapeType::Line);
}
void CEasyDrawView::OnUpdateToolRect(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentType == ShapeType::Rect);
}
void CEasyDrawView::OnUpdateToolEllipse(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentType == ShapeType::Ellipse);
}
void CEasyDrawView::OnUpdateToolFree(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentType == ShapeType::Freehand);
}

void CEasyDrawView::OnUpdateColorRed(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentColor.GetValue() == Color::Red);
}
void CEasyDrawView::OnUpdateColorBlue(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_currentColor.GetValue() == Color::Blue);
}

void CEasyDrawView::OnUpdateWidthThin(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_lineWidth <= 2.0f);
}
void CEasyDrawView::OnUpdateWidthThick(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(m_lineWidth >= 10.0f);
}

// 屏蔽系统默认的背景擦除，防止闪烁
BOOL CEasyDrawView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// 切换到简体中文 (默认 IDR_MAINFRAME)
void CEasyDrawView::OnLangCN() {
	ChangeMenu(IDR_MAINFRAME);
}

// 切换到英文 (IDR_MENU_EN)
void CEasyDrawView::OnLangEN() {
	ChangeMenu(IDR_MENU_EN);
}

// 切换到繁体中文 (IDR_MENU_TC)
void CEasyDrawView::OnLangTC() {
	ChangeMenu(IDR_MENU_TC);
}

// === 核心：动态替换菜单资源 ===
// 【关键修改 2】彻底修复崩溃问题
// 不要在 View 里自己 DestroyMenu，而是调用 MainFrame::SwitchLanguage
void CEasyDrawView::ChangeMenu(UINT menuID)
{
	// 1. 获取主窗口指针
	CWnd* pMainWnd = AfxGetMainWnd();

	// 2. 安全转换为我们的 CMainFrame 类
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(pMainWnd);

	// 3. 让主框架去干活
	if (pMainFrame)
	{
		pMainFrame->SwitchLanguage(menuID);
	}
}

// =================================================================
// 调试
// =================================================================
#ifdef _DEBUG
void CEasyDrawView::AssertValid() const { CView::AssertValid(); }
void CEasyDrawView::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CEasyDrawDoc* CEasyDrawView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEasyDrawDoc)));
	return (CEasyDrawDoc*)m_pDocument;
}
#endif