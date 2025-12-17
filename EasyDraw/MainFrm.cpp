// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "EasyDraw.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// 初始化当前菜单 ID 为默认 ID
	m_currMenuID = IDR_MAINFRAME;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// ==========================================================
// 核心：多语言切换实现
// 解决 "Assertion Failed" 的关键在于更新 m_hMenuDefault
// ==========================================================
void CMainFrame::SwitchLanguage(UINT nMenuResourceID)
{
	// 1. 如果目标语言和当前语言一样，啥也不做
	if (m_currMenuID == nMenuResourceID) return;

	// 2. 加载新的菜单资源
	CMenu newMenu;
	if (!newMenu.LoadMenu(nMenuResourceID))
	{
		AfxMessageBox(_T("Failed to load menu resource!")); // 资源加载失败提示
		return;
	}

	// 3. 将 CMenu 对象与 Windows 菜单句柄分离
	// 为什么？因为 CMenu 是局部变量，函数结束时会析构。
	// 如果不 Detach，析构函数会销毁菜单，导致界面菜单消失或崩溃。
	HMENU hNewMenu = newMenu.Detach();

	// 4. 替换当前窗口的菜单
	CMenu* pOldMenu = GetMenu(); // 获取旧菜单指针
	SetMenu(CMenu::FromHandle(hNewMenu)); // 设置新菜单

	// 5. 【关键修正】更新 MFC 内部维护的默认菜单句柄
	// 之前的崩溃就是因为没有更新这个变量，导致 MFC 认为菜单还是旧的，
	// 进而去访问已经无效的资源。
	m_hMenuDefault = hNewMenu;

	// 6. 记录当前 ID
	m_currMenuID = nMenuResourceID;

	// 7. 销毁旧菜单资源 (可选，建议交给系统处理或显式销毁)
	// 注意：由于我们已经更新了 m_hMenuDefault，MFC 不会再引用旧菜单，
	// 这里可以不手动 destroy，但如果为了内存严谨，可以加上。
	// if (pOldMenu) pOldMenu->DestroyMenu(); 

	// 8. 强制重绘菜单栏和重新计算布局
	DrawMenuBar();
	RecalcLayout();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG