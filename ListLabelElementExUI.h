#ifndef LISTLABELELEMENTEX_H
#define LISTLABELELEMENTEX_H
#pragma once
//����İ���������Ŀʵ��·���޸�
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
public://2016-01-31 ֧��
	int GetChildItemCount();
	int SetChildItemCount(int nNumber);
// 	int AddChildItemNumber();//������Item������--�и���item�����ڲ�����
// 	int SubChildItemNumber();//������Item������--�и���item�����ڲ�����
	bool SetExpandFlag(bool bFlag);//���õ�ǰչ��״̬
	bool GetExpandFlag();//��ȡ��ǰ��ǰ״̬----��Ҫ��Ϊ�˺͸�itemһ��
	CListLabelElementExUI* GetItemParent();//�õ����ڵ�ָ��
	CListLabelElementExUI* SetItemParent(CListLabelElementExUI *pParent);//���ø��ڵ�ָ��
private:
	int m_iChildItemNumber;//���¶��ӽڵ�Item����
	int m_iTotalChildItemNumber;//�������е��ӽڵ���������������ӣ����ӣ������ӣ�.........���У����е��ӽڵ�
	bool m_bExpandFlag;//��ǰ�ӽڵ�չ����״̬--���ϼ�Ӱ���
	CListLabelElementExUI* m_pItemParent;//���ڵ�ָ��
};
#endif

