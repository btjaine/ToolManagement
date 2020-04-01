#ifndef LISTEX_H
#define LISTEX_H
#pragma once
//����İ���������Ŀʵ��·���޸�
#include "ListLabelElementExUI.h"


class CListUIEx : public CListExUI
{
public:
	CListUIEx();
	virtual ~CListUIEx();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual void DoEvent(TEventUI& event);

	/**
	 * ������,�վ�,��0��ʼ
	 *
	 * @param	nCol					����,��Ҫ��0��ʼ���β���
	 * @param	lpszColumnHeaderStr		ÿһ�б���
	 * @param	uTextStyle				�������ֶ��뷽ʽ
	 * @param	nWidth					�п�
	 *
	 * @return	����TrueΪ�ɹ�,FalseΪʧ��
	 */
	BOOL InsertColumn(
		int nCol,
		LPCTSTR lpszColumnHeaderStr,
		UINT uTextStyle = DT_LEFT,
		int nWidth = -1
		);
	BOOL InsertColumn(int nCol, CListHeaderItemUI *pHeaderItem);
	/**
	 * ������,�վ�,��0��ʼ
	 *
	 * @param	nItem					����,��Ҫ��0��ʼ���β���
	 *
	 * @return	�ɹ��򷵻�����,���򷵻�-1
	 */
	int InsertItemEx(int nHeight = 26, int FontSize=0);
	int InsertItem(int nItem, int nHeight);
	int InsertItem(int nItem, int nHeight, CListContainerElementUI *pListItem);
	void SetItemIcon(int nItem, int nColumn, LPCTSTR ImgFile,int nWidth=18,int nHeight=18);
	void SetItemTextEx(int nItem, int nColumn, LPCTSTR Text, int nAlign=1,DWORD dwColor=RGB(0,0,0));
	void SetItemData(int nItem, int nColumn, CControlUI* pControl);
	bool SetItemUseData(int nItem, LPCTSTR strUserData);
	bool SetItemTag(int nItem, LPVOID strUserTag);
    
	int  SetItemTrace(int nTrace)
	{
		m_nItemTrace = nTrace;
		return nTrace;
	};
	/**
	 * ����������ȡ�пؼ�
	 *
	 * @param	iIndex					����,��0���������
	 *
	 * @return	�ɹ�ʱ�����ӿؼ���ַ,���򷵻�NULL
	 */
	CListLabelElementExUI* GetListItem(int iIndex);

	/**
	 * ��ȡ����λ�õĿؼ�
	 *
	 * @param	iIndex					����,��0���������
	 * @param	iSubIndex					����,��0���������
	 *
	 * @return	�ɹ�ʱ�����ӿؼ���ַ,���򷵻�NULL
	 */
	CHorizontalLayoutUI* GetListSubItem(int iIndex, int iSubIndex);
	BOOL SetHeaderItemData(int nColumn, CControlUI* pControl);
	bool SetItemInset(int nRow, int nItemIndex, int iLeft = 0, int iTop = 0, int iRight = 0, int iBottom = 0);
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true);



	bool GetItemInset(int nRow, int nItemIndex, RECT& rc);
	CDuiString GetItemText(int nRow, int nCol);
	CDuiString GetItemUseData(int nItem);
	UINT_PTR GetItemTag(int nItem);

	//2016-01-31:֧��Expand
	bool ExpandItem(int iItemIndex, bool bExpand);//չ��ָ����
	int  GetItemChildCount(int iItemIndex);//��ȡָ�������е�����
	int  InsertItemWithParent(int iParentItemIndex, bool bParent = false, bool bHandInsert=false,int nHeight = 26);//�����ڵ㹦�ܣ����һ��
	bool DeleteItem(int nItem);//ɾ���ڵ�
	void SetExpandIconFile(LPCTSTR ImgFile);//����Expand��ͼ��
	//2016-02-01���Ӻ�ȥÿ��Row Icon Text��UIָ��
	CButtonUI* GetRowButton(int nRow, int nCol);//��ȡ��ͷ��ť
	CButtonUI* GetIconButton(int nRow, int nCol);//��ȡICON��ť
	CLabelUI* GetTextButton(int nRow, int nCol);//��ȡText��ť
	//2016-04-13
	void SetRowButtonName(int nItem, int nColumn, LPCTSTR strName);//����RowButtonName--�Ժ�����ڸ������н�����ز���
private:
	int m_nItemTrace;//��������
	int m_iCurItemHeight;//��ǰ���и�
	CDuiString  m_strExpandFileName;
};
#endif