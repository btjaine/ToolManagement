#ifndef COMBOEX_H
#define COMBOEX_H
#pragma once
//����İ���������Ŀʵ��·���޸�
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
	//���CControlUI ����Item
	void SetItemData(int nItem, CControlUI* pControl);
	//��ȡCHorizontalLayoutUI����ؼ�
	CHorizontalLayoutUI*GetListSubItem(int iItemIndex) const;
	//���Item
	int InsertItem(int nHeight = 21);
	//����ICON
	void SetItemIcon(int nItem, LPCTSTR ImgFile, int nWidth=16, int nHeight=16,int nPaddingLeft=0,int nPaddingTop=0,int nPaddingRight=0,int nPaddingBottom=0);
	//����ItemText
	void SetItemText(int nItem, LPCTSTR Text, int nFontId=0,int nAlign = 1);
	//����use����--ָ����Ϣ
	bool  SetItemTag(int nItem, LPVOID strUserTag);
	//�����û�UseData--������Ϣ
	bool  SetItemUseData(int nItem, LPCTSTR strUserData);
	//��ȡuse����--������Ϣ
	CDuiString GetItemUseData(int nItem) const;
	//��ȡItemָ����Ϣ��������һ���ṹ��ָ�룩
	UINT_PTR GetItemTag(int nItem);
	//����Client
	bool SetItemInset(int iItemIndex, int iLeft=2, int iTop=2, int iRight=2, int iBottom=2);
	//��ȡClient
	bool GetItemInset(int iItemIndex, RECT& rc);
	//��ȡItemText
	CDuiString GetItemText(int nItem) const;
	//��ȡComboUseDate
	CDuiString GetComboUseData();
};
#endif
