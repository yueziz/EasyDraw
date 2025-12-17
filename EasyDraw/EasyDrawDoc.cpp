// EasyDrawDoc.cpp: CEasyDrawDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "EasyDraw.h"
#endif

#include "EasyDrawDoc.h"
#include "Serializer.h" // <--- 必须包含序列化器头文件

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEasyDrawDoc

IMPLEMENT_DYNCREATE(CEasyDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CEasyDrawDoc, CDocument)
END_MESSAGE_MAP()


// CEasyDrawDoc 构造/析构

CEasyDrawDoc::CEasyDrawDoc() noexcept
{
	// TODO: 在此添加一次性构造代码

}

CEasyDrawDoc::~CEasyDrawDoc()
{
}

BOOL CEasyDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	m_shapes.clear(); // 清空数据

	return TRUE;
}

// CEasyDrawDoc 序列化

void CEasyDrawDoc::Serialize(CArchive& ar)
{
	// 由于我们重写了 OnOpen/OnSave，这个函数不会被调用
	// 但必须保留空实现以满足 MFC 宏的要求
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

// =========================================================
//  F-09 单元测试要求：重写 OnSaveDocument / OnOpenDocument
//  注意：不调用基类 CDocument::On... (除了必要的路径设置)
// =========================================================

BOOL CEasyDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// 1. 将 MFC 的宽字符路径转为 std::string
	CT2CA pszConvertedAnsiString(lpszPathName);
	std::string filename(pszConvertedAnsiString);

	// 2. 调用我们的 Serializer (不走 MFC Archive)
	if (Serializer::Save(m_shapes, filename)) {
		SetModifiedFlag(FALSE); // 保存成功，标记为“未修改”
		return TRUE;
	}
	return FALSE;
}

BOOL CEasyDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// 【不调用】CDocument::OnOpenDocument(lpszPathName);

	// 1. 转换路径
	CT2CA pszConvertedAnsiString(lpszPathName);
	std::string filename(pszConvertedAnsiString);

	// 2. 清空当前数据
	m_shapes.clear();

	// 3. 调用 Serializer 加载
	if (Serializer::Load(m_shapes, filename)) {
		// 4. 手动设置文档状态 (因为没有调用基类，必须手动做这些)
		SetPathName(lpszPathName); // 设置当前文件路径
		SetModifiedFlag(FALSE);    // 标记为“未修改”
		UpdateAllViews(NULL);      // 刷新视图显示
		return TRUE;
	}

	return FALSE;
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CEasyDrawDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CEasyDrawDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CEasyDrawDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CEasyDrawDoc 诊断

#ifdef _DEBUG
void CEasyDrawDoc::AssertValid() const {
	CDocument::AssertValid();
}

void CEasyDrawDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEasyDrawDoc 命令