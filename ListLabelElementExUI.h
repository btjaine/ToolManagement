#ifndef LISTLABELELEMENTEX_H
#define LISTLABELELEMENTEX_H
#pragma once
//下面的包含根据项目实际路径修改
#include "pch.h"

class CListLabelElementExUI :
	public CListContainerElementUI
{
public:
	CListLabelElementExUI();
	~CListLabelElementExUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void DoEvent(TEventUI& event);
	virtual void DrawItemBk(HDC hDC, const RECT& rcItem);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
public://2016-01-31 支持
	int GetChildItemCount();
	int SetChildItemCount(int nNumber);
// 	int AddChildItemNumber();//增加子Item的数量--有父子item有类内部调用
// 	int SubChildItemNumber();//减少子Item的数量--有父子item有类内部调用
	bool SetExpandFlag(bool bFlag);//设置当前展开状态
	bool GetExpandFlag();//获取当前当前状态----主要是为了和父item一致
	CListLabelElementExUI* GetItemParent();//得到父节点指针
	CListLabelElementExUI* SetItemParent(CListLabelElementExUI *pParent);//设置父节点指针
private:
	int m_iChildItemNumber;//旗下儿子节点Item数量
	int m_iTotalChildItemNumber;//旗下所有的子节点的数量，包括儿子，孙子，重孙子，.........所有，所有的子节点
	bool m_bExpandFlag;//当前子节点展开的状态--手上级影响的
	CListLabelElementExUI* m_pItemParent;//父节点指针
};
#endif

