#ifndef COMBOEX_H
#define COMBOEX_H
#pragma once
//下面的包含根据项目实际路径修改
#include "pch.h"
#include"ListLabelElementExUI.h"

class CComboExUI :public CComboUI
{
public:
	CComboExUI();
	~CComboExUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	virtual CDuiString GetComboText() const;
	virtual void PaintText(HDC hDC);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
public:
	//添加CControlUI 对象到Item
	void SetItemData(int nItem, CControlUI* pControl);
	//获取CHorizontalLayoutUI对象控件
	CHorizontalLayoutUI*GetListSubItem(int iItemIndex) const;
	//添加Item
	int InsertItem(int nHeight = 21);
	//设置ICON
	void SetItemIcon(int nItem, LPCTSTR ImgFile, int nWidth=16, int nHeight=16,int nPaddingLeft=0,int nPaddingTop=0,int nPaddingRight=0,int nPaddingBottom=0);
	//设置ItemText
	void SetItemText(int nItem, LPCTSTR Text, int nFontId=0,int nAlign = 1);
	//设置use数据--指针信息
	bool  SetItemTag(int nItem, LPVOID strUserTag);
	//设置用户UseData--文字信息
	bool  SetItemUseData(int nItem, LPCTSTR strUserData);
	//获取use数据--文字信息
	CDuiString GetItemUseData(int nItem) const;
	//获取Item指针信息（可以是一个结构的指针）
	UINT_PTR GetItemTag(int nItem);
	//设置Client
	bool SetItemInset(int iItemIndex, int iLeft=2, int iTop=2, int iRight=2, int iBottom=2);
	//获取Client
	bool GetItemInset(int iItemIndex, RECT& rc);
	//获取ItemText
	CDuiString GetItemText(int nItem) const;
	//获取ComboUseDate
	CDuiString GetComboUseData();
};
#endif
