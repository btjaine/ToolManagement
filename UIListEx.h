#ifndef LISTEX_H
#define LISTEX_H
#pragma once
//下面的包含根据项目实际路径修改
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
	 * 插入列,照旧,从0开始
	 *
	 * @param	nCol					列数,需要从0开始依次插入
	 * @param	lpszColumnHeaderStr		每一列标题
	 * @param	uTextStyle				标题文字对齐方式
	 * @param	nWidth					列宽
	 *
	 * @return	返回True为成功,False为失败
	 */
	BOOL InsertColumn(
		int nCol,
		LPCTSTR lpszColumnHeaderStr,
		UINT uTextStyle = DT_LEFT,
		int nWidth = -1
		);
	BOOL InsertColumn(int nCol, CListHeaderItemUI *pHeaderItem);
	/**
	 * 插入行,照旧,从0开始
	 *
	 * @param	nItem					行数,需要从0开始依次插入
	 *
	 * @return	成功则返回行数,否则返回-1
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
	 * 根据索引获取行控件
	 *
	 * @param	iIndex					行数,从0到最大行数
	 *
	 * @return	成功时返回子控件地址,否则返回NULL
	 */
	CListLabelElementExUI* GetListItem(int iIndex);

	/**
	 * 获取具体位置的控件
	 *
	 * @param	iIndex					行数,从0到最大行数
	 * @param	iSubIndex					列数,从0到最大行数
	 *
	 * @return	成功时返回子控件地址,否则返回NULL
	 */
	CHorizontalLayoutUI* GetListSubItem(int iIndex, int iSubIndex);
	BOOL SetHeaderItemData(int nColumn, CControlUI* pControl);
	bool SetItemInset(int nRow, int nItemIndex, int iLeft = 0, int iTop = 0, int iRight = 0, int iBottom = 0);
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true);



	bool GetItemInset(int nRow, int nItemIndex, RECT& rc);
	CDuiString GetItemText(int nRow, int nCol);
	CDuiString GetItemUseData(int nItem);
	UINT_PTR GetItemTag(int nItem);

	//2016-01-31:支持Expand
	bool ExpandItem(int iItemIndex, bool bExpand);//展开指定行
	int  GetItemChildCount(int iItemIndex);//获取指定行子行的数量
	int  InsertItemWithParent(int iParentItemIndex, bool bParent = false, bool bHandInsert=false,int nHeight = 26);//带父节点功能，添加一行
	bool DeleteItem(int nItem);//删除节点
	void SetExpandIconFile(LPCTSTR ImgFile);//设置Expand的图标
	//2016-02-01增加后去每个Row Icon Text的UI指针
	CButtonUI* GetRowButton(int nRow, int nCol);//获取箭头按钮
	CButtonUI* GetIconButton(int nRow, int nCol);//获取ICON按钮
	CLabelUI* GetTextButton(int nRow, int nCol);//获取Text按钮
	//2016-04-13
	void SetRowButtonName(int nItem, int nColumn, LPCTSTR strName);//设置RowButtonName--以后可以在父程序中进行相关操作
private:
	int m_nItemTrace;//行数跟踪
	int m_iCurItemHeight;//当前的行高
	CDuiString  m_strExpandFileName;
};
#endif