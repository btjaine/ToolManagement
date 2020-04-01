#include "pch.h"
#include "UIListEx.h"
#include "MsgWnd.h"

/************************** CListUIEx Start **************************/
CListUIEx::CListUIEx()
{
	m_strExpandFileName.Empty();
	m_nItemTrace = 0;
}

CListUIEx::~CListUIEx()
{
}
LPCTSTR CListUIEx::GetClass() const
{
	// �����������ȥ����ͷ����C�ַ�
	return _T("ListUIEx");
}
CButtonUI* CListUIEx::GetRowButton(int nRow, int nCol)//��ȡ��ͷ��ť
{
	CHorizontalLayoutUI* m_phr = (CHorizontalLayoutUI*)GetListSubItem(nRow, nCol);
	if (!m_phr)  return NULL;
	return (CButtonUI*)m_phr->GetItemAt(0);
}
CButtonUI* CListUIEx::GetIconButton(int nRow, int nCol)//��ȡICON��ť
{
	CHorizontalLayoutUI* m_phr = (CHorizontalLayoutUI*)GetListSubItem(nRow, nCol);
	if (!m_phr)  return NULL;
	return (CButtonUI*)m_phr->GetItemAt(1);
}
CLabelUI* CListUIEx::GetTextButton(int nRow, int nCol)//��ȡText��ť
{
	CHorizontalLayoutUI* m_phr = (CHorizontalLayoutUI*)GetListSubItem(nRow, nCol);
	if (!m_phr)  return NULL;
	return (CLabelUI*)m_phr->GetItemAt(2);
}
void CListUIEx::DoEvent(TEventUI& event)
{

	__super::DoEvent(event);
}
LPVOID CListUIEx::GetInterface(LPCTSTR pstrName)
{
	// 		�����xml�ؼ�������
	if (_tcsicmp(pstrName, _T("ListEx_YIN")) == 0) return static_cast<CListUIEx*>(this);
	return CListExUI::GetInterface(pstrName);
}
void CListUIEx::SetExpandIconFile(LPCTSTR ImgFile)//����Expand��ͼ��
{
	m_strExpandFileName = ImgFile;
}
//************************************
// Method:    DeleteItem
// FullName:  CListUIEx::DeleteItem
// Access:    public 
// Returns:   void
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int nItem
// Description:ɾ���ڵ�
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
bool CListUIEx::DeleteItem(int nItem)//ɾ���ڵ�
{
	bool m_bresult = true;
	CListLabelElementExUI* m_pItem = GetListItem(nItem);
	if (!m_pItem)  return false;
	if (m_pItem->GetChildItemCount()<=0)
	{
		CListLabelElementExUI* m_pItemParent = m_pItem->GetItemParent();
		if (!m_pItemParent)//û�и���������Ͱ��Լ�ɾ��
		{
			CListUI::RemoveAt(nItem);

			return true;
		}
		m_pItemParent->SetChildItemCount(m_pItemParent->GetChildItemCount()- 1);
		//�Ƴ�
		CListUI::RemoveAt(nItem);
		//���ĸ�item��rowͼ��
		if (m_pItemParent->GetChildItemCount()<=0)
		{
			if (!m_strExpandFileName.IsEmpty())//ֻ�е�һ�п������ü�ͷͼ��
			{
				CButtonUI* m_pRowLabel = (CButtonUI*)((CHorizontalLayoutUI*)m_pItemParent->GetItemAt(0))->GetItemAt(0);
				if (m_pRowLabel)
				{
					CDuiString m_strBk;
					int m_iPosition = (m_pRowLabel->GetFixedHeight() - 9) / 2;
					m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='18,0,27,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
					m_pRowLabel->SetAttribute(_T("bkimage"), m_strBk);
				}
			}
		}
	}
	else
	{
		m_bresult = false;
		//DuiMessageBox(NULL, _T("There is a child node, can not be deleted !"),_T("Error Information"), DUI_DEFAULT);
		CMsgWnd::MessageBox(nullptr, _T("�ÿ�ϵͳ"), _T("����ӽڵ㲻�ܱ�ɾ��!"), true);
	}
	return m_bresult;
}

//************************************
// Method:    ExpandItem
// FullName:  CListUIEx::ExpandItem
// Access:    public 
// Returns:   bool
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int iItemIndex
// Parameter: bool bExpand
// Description:չ��/����ָ����
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
bool CListUIEx::ExpandItem(int iItemIndex, bool bExpand)//չ��ָ����
{
	if (iItemIndex == GetCurSel())
	{
		m_nItemTrace = GetCurSel();//�����أ�����ɾ����������Ȼ����ʾ������һ��վλ��
	}
	if (iItemIndex < 0)
	{
		return false;
	}
	//��ȡ�ؼ�ָ��
	CListLabelElementExUI* m_pItem = GetListItem(iItemIndex);
	if (!m_pItem)
	{
		return false;
	}
	//����չ��״̬
	m_pItem->SetExpandFlag(bExpand);
	//���ü�ͷ
	if (!m_strExpandFileName.IsEmpty() && m_pItem->GetChildItemCount() > 0)
	{
		CButtonUI* m_pLable = GetRowButton(iItemIndex,0);//(CLabelUI*)GetListSubItem(iItemIndex, 0)->GetItemAt(0);
		if (m_pLable)
		{
			CDuiString m_strBk;
			int m_iPosition = (m_pLable->GetFixedHeight() - 9) / 2;
			if (bExpand)
			{
				m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='9,0,18,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
			}
			else
			{
				m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='0,0,9,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
			}
			m_pLable->SetAttribute(_T("bkimage"), m_strBk);
		}
	}
	//��ȡ�ж��ٸ��ӽڵ�
	for (int i = 1; i <= m_pItem->GetChildItemCount(); i++)
	{
		m_nItemTrace++;
		//ѭ������
		CListLabelElementExUI* m_pChildItem = GetListItem(m_nItemTrace);
		if (m_pChildItem)
		{
			m_pChildItem->SetVisible(bExpand);
			//��������
			ExpandItem(m_nItemTrace, bExpand);
		}
	}
	return true;
}
//************************************
// Method:    GetItemChildCount
// FullName:  CListUIEx::GetItemChildCount
// Access:    public 
// Returns:   bool
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int iItemIndex
// Description:��ȡָ���е����е�����
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
int  CListUIEx::GetItemChildCount(int iItemIndex)//��ȡָ�������е�����
{
	if (iItemIndex < 0)
	{
		return -1;
	}
	return GetListItem(iItemIndex)->GetChildItemCount();
}

//************************************
// Method:    InsertItemWithParent
// FullName:  CListUIEx::InsertItemWithParent
// Access:    public 
// Returns:   int
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int iParentItemIndex
// Parameter: bool bParent
// Parameter: bool bHandInsert----�Ƿ�Ϊ�ֶ���ӣ�false����ʾ�ڳ�ʼ����ʱ������ݿ���ֻ�ܶ�ȡ��   true:��ʾ�ֶ���ӣ���ӵ�ʱ��ֱ���ڸ�Item������һ������
// Parameter: int nHeight
// Description:
// CreateTime:2016/02/01
// Last Alter Time:
//************************************
int  CListUIEx::InsertItemWithParent(int iParentItemIndex, bool bParent, bool bHandInsert /*= false*/, int nHeight)
{
	if (iParentItemIndex == -1)//��Item ==-1��Ҳ�ǰ��ղ��Ǹ�������ӵ�
	{
		return InsertItem(nHeight,0);
	}
	if (!bParent)//���Ǹ��ڵ�
	{
		return InsertItem(nHeight,0);
	}
	if (iParentItemIndex < 0) return -1;
	//����Item�����
	//��ȡiParentItemIndex�Ķ���
	CListLabelElementExUI* m_pParent = GetListItem(iParentItemIndex);
	int m_iChildItemAddPosition = 0;
	if (!bHandInsert)
	{
		m_iChildItemAddPosition = GetCount();//���е�λ��+�ӵ����������������ӵ�λ��--����ط�������//////////////////////////////////////////////
	}
	else
	{
		m_iChildItemAddPosition = iParentItemIndex + 1;//��һ����ӽ�ȥ
	}
	//����һ��
	InsertItem(m_iChildItemAddPosition, nHeight);
	//���ø��ڵ��ͷͼ��ͼ��
	if (!m_strExpandFileName.IsEmpty())
	{
		CButtonUI* m_pLable = GetRowButton(iParentItemIndex, 0);//(CLabelUI*)GetListSubItem(iParentItemIndex, 0)->GetItemAt(0);
		if (m_pLable)
		{
			
			m_pLable->SetFixedWidth(9);
			CDuiString m_strBk;
			int m_iPosition = (m_pLable->GetFixedHeight() - 9) / 2;
			m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='9,0,18,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
			m_pLable->SetAttribute(_T("bkimage"), m_strBk);
		}
	}
	//����Item����Item����+1
	m_pParent->SetChildItemCount(m_pParent->GetChildItemCount() + 1);
	//�����ӽڵ�--���ڵ�ָ��
	GetListItem(m_iChildItemAddPosition)->SetItemParent(m_pParent);
	//��������
	CHorizontalLayoutUI* m_ph = GetListSubItem(iParentItemIndex, 0);
	if (m_ph)
	{
		RECT m_rc = m_ph->GetInset();
		SetItemInset(m_iChildItemAddPosition, 0, m_rc.left + 20);
	}
	return m_iChildItemAddPosition;
}
//ͨ�����봴����  
BOOL CListUIEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeaderStr, UINT uTextStyle/* = DT_LEFT*/, int nWidth/* = -1*/)
{
	CListHeaderItemUI *pHeaderItem = new CListHeaderItemUI;
	pHeaderItem->SetTextStyle(uTextStyle | DT_VCENTER | DT_SINGLELINE);
	pHeaderItem->SetText(lpszColumnHeaderStr);

	pHeaderItem->SetAttribute(_T("sepimage"), _T("Headerctrl_Sperator.bmp"));
	pHeaderItem->SetAttribute(_T("sepwidth"), _T("1"));
	pHeaderItem->SetAttribute(_T("pushedimage"), _T("headerctrl_down.bmp"));
	pHeaderItem->SetAttribute(_T("hotimage"), _T("headerctrl_hot.bmp"));
	pHeaderItem->SetAttribute(_T("normalimage"), _T("headerctrl_normal.bmp"));
	pHeaderItem->SetFixedWidth(nWidth);

	return InsertColumn(nCol, pHeaderItem);
}


BOOL CListUIEx::InsertColumn(int nCol, CListHeaderItemUI *pHeaderItem)
{
	CListHeaderUI *pHeader = CListUI::GetHeader();
	if (pHeader == NULL)
	{
		return FALSE;
	}

	if (pHeader->AddAt(pHeaderItem, nCol))
	{
		return TRUE;
	}

	delete pHeaderItem;
	pHeaderItem = NULL;
	return FALSE;
}

BOOL CListUIEx::SetHeaderItemData(int nColumn, CControlUI* pControl)
{
	CListHeaderUI *pHeader = CListUI::GetHeader();
	if (pHeader == NULL)
	{
		return FALSE;
	}
	CListHeaderItemUI *pHeaderItem = (CListHeaderItemUI *)pHeader->GetItemAt(nColumn);
	return pHeaderItem->Add(pControl);

}
//************************************
// Method:    InsertItem
// FullName:  CListUIEx::InsertItem
// Access:    public 
// Returns:   int
// Qualifier: Jaine(1427208421@qq.com)
// Parameter: int nHeight
// Description:
// CreateTime:2015/12/26
// Last Alter Time:
//************************************
int CListUIEx::InsertItemEx(int nHeight, int FontSize)
{
	m_iCurItemHeight = nHeight;
	CListLabelElementExUI *pListItem = new CListLabelElementExUI;
	pListItem->SetFixedHeight(nHeight);/*�̶�һ���и�*/
	RECT m_rc;
	m_rc.left = 1;
	m_rc.top = 3;
	m_rc.right = 3;
	m_rc.bottom = 3;
	pListItem->SetInset(m_rc);
	int  nItem = GetCount();//��ȡ��������ʲôλ�����
	if (NULL != m_pHeader)
	{
		int nHeaderCount = m_pHeader->GetCount();
		for (int i = 0; i < nHeaderCount; i++)
		{
			CHorizontalLayoutUI* pSubHor = new CHorizontalLayoutUI;
			//��ȡ��Ӧ��HeadItem�Ŀ��
			CListHeaderItemUI *pHeaderItem = static_cast<CListHeaderItemUI*>(m_pHeader->GetItemAt(i));
			RECT rtHeader = pHeaderItem->GetPos();
			pSubHor->SetFixedWidth(rtHeader.right - rtHeader.left);
			//ÿ��column һ����ͷͼ�꣬һ����ͼ�꣬һ��text
			CButtonUI *m_pLabel = new CButtonUI;//ͼ��
			m_pLabel->SetFixedWidth(1);
			m_pLabel->SetFixedHeight(nHeight);

			//�ڼ�һ��ͼ�꣬�����ż�ͷ֮���ͼ��
			CButtonUI *m_pRowLabel = new CButtonUI;//ͼ��
			m_pRowLabel->SetFixedWidth(1);
			m_pRowLabel->SetFixedHeight(nHeight);

			if (!m_strExpandFileName.IsEmpty() && i == 0)//ֻ�е�һ�п������ü�ͷͼ��
			{
				m_pRowLabel->SetFixedWidth(9);
				CDuiString m_strBk;
				int m_iPosition = (nHeight - 9) / 2;
				m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='18,0,27,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
				m_pRowLabel->SetAttribute(_T("bkimage"), m_strBk);//SetAttribute(_T("dotlineattr"), _T("bkimage=&quot;file = 'list\\hotline.png' dest='0,0,18,18' source='0,0,18,18'&quot;")
			}
			//������߾�
			RECT m_rc;
			m_rc.left = 0;
			m_rc.top = 0;
			m_rc.right = 4;
			m_rc.bottom = 0;
			m_pLabel->SetPadding(m_rc);
			m_pRowLabel->SetPadding(m_rc);
			//��ͷͼ��
			pSubHor->Add(m_pRowLabel);
			pSubHor->Add(m_pLabel);
			//������Ϣ
			CLabelUI *pLabel = new CLabelUI;
			pLabel->SetFont(FontSize);
			pSubHor->Add(pLabel);

			//��ӵ�CListLabelElementExUI
			pListItem->Add(pSubHor);
		}
	}
	if (!CListUI::AddAt(pListItem, nItem))
	{
		delete pListItem;
		pListItem = NULL;
		return -1;
	}
	return nItem;
}
//************************************
// Method:    InsertItem
// FullName:  CListUIEx::InsertItem
// Access:    public 
// Returns:   int
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int nItem
// Parameter: int nHeight
// Description:��ָ���в���һ��
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
int CListUIEx::InsertItem(int nItem, int nHeight)
{
	m_iCurItemHeight = nHeight;
	CListLabelElementExUI *pListItem = new CListLabelElementExUI;
	if (!pListItem)
	{
		return -1;
	}
	pListItem->SetFixedHeight(nHeight);/*�̶�һ���и�*/

	RECT m_rc;
	m_rc.left = 1;
	m_rc.top = 0;
	m_rc.right = 3;
	m_rc.bottom = 0;
	pListItem->SetInset(m_rc);
	pListItem->SetAttribute(L"valign", L"center");

	if (NULL != m_pHeader)
	{
		int nHeaderCount = m_pHeader->GetCount();
		RECT m_rc;
		for (int i = 0; i < nHeaderCount; i++)
		{
			CHorizontalLayoutUI* pSubHor = new CHorizontalLayoutUI;
			//��ȡ��Ӧ��HeadItem�Ŀ��
			CListHeaderItemUI *pHeaderItem = static_cast<CListHeaderItemUI*>(m_pHeader->GetItemAt(i));
			RECT rtHeader = pHeaderItem->GetPos();
			pSubHor->SetFixedWidth(rtHeader.right - rtHeader.left);
			
			
			//ÿ��column һ����ͷͼ�� һ��ͼ�꣬һ��text
			CButtonUI *m_pLabel = new CButtonUI;//ͼ��
			m_pLabel->SetFixedWidth(1);
			m_pLabel->SetFixedHeight(nHeight);
			
			//�ڼ�һ��ͼ�꣬�����ż�ͷ֮���ͼ��
			CButtonUI *m_pRowLabel = new CButtonUI;//ͼ��
			m_pRowLabel->SetFixedWidth(1);
			m_pRowLabel->SetFixedHeight(nHeight);
			//װ��Ĭ��ͼƬ
			if (!m_strExpandFileName.IsEmpty() && i == 0)//ֻ�е�һ�п������ü�ͷͼ��
			{
				m_pRowLabel->SetFixedWidth(9);
				CDuiString m_strBk;
				int m_iPosition = (nHeight - 9) / 2;
				m_strBk.Format(_T("file = '%s' dest='0,%d,9,%d' source='18,0,27,9'"), m_strExpandFileName.GetData(), m_iPosition - 2, m_iPosition + 7);
				m_pRowLabel->SetAttribute(_T("bkimage"), m_strBk);//SetAttribute(_T("dotlineattr"), _T("bkimage=&quot;file = 'list\\hotline.png' dest='0,0,18,18' source='0,0,18,18'&quot;")
			}
			m_rc.left = 0;
			m_rc.top = 0;
			m_rc.right = 4;
			m_rc.bottom = 0;
			m_pLabel->SetPadding(m_rc);
			m_pRowLabel->SetPadding(m_rc);
			m_pLabel->SetCursor(DUI_HAND);
			m_pLabel->SetName(L"BT_LIST_MAIN_ICO");
			//��ͷͼ��
			pSubHor->Add(m_pRowLabel);
			//����ǰ���ͼ��
			pSubHor->Add(m_pLabel);
			//������Ϣ
			CLabelUI *pLabel = new CLabelUI;
			pLabel->SetAttribute(L"valign", L"center");
			pSubHor->Add(pLabel);
			//��ӵ�CListLabelElementExUI
			pListItem->Add(pSubHor);
		}
	}
	if (!CListUI::AddAt(pListItem, nItem))
	{
		delete pListItem;
		pListItem = NULL;
		return -1;
	}
	return nItem;
}


int CListUIEx::InsertItem(int nItem, int nHeight, CListContainerElementUI *pListItem)
{
	pListItem->SetFixedHeight(nHeight);
	if (!CListUI::AddAt(pListItem, nItem))
	{
		delete pListItem;
		pListItem = NULL;
		return -1;
	}
	return nItem;
}
//SetItemData
//nItem:�ڼ���
//nColumn:�ڼ���
//Text:����
//nAlign:���� ��ʽ 0�� �����  1���м� 2���Ҷ���
void CListUIEx::SetItemTextEx(int nItem, int nColumn, LPCTSTR Text, int nAlign, DWORD dwColor)//�˺���������
{
	//CHorizontalLayoutUI *pSubHor = GetListSubItem(nItem, nColumn);
	CLabelUI* pLabel = GetTextButton(nItem, nColumn);//(CLabelUI*)pSubHor->GetItemAt(2);
	if (!pLabel){
		return;
	}
	if (nAlign == 0)
	{
		pLabel->SetAttribute(L"align", L"left");
	}
	else if (nAlign == 1)
	{
		pLabel->SetAttribute(L"align", L"center");
	}
	else
	{
		pLabel->SetAttribute(L"align", L"right");
	}
	pLabel->SetAttribute(L"valign", L"center");
	//��ʾ���˵�������...����
	pLabel->SetAttribute(L"textpadding", L"4,0,1,0");
	pLabel->SetAttribute(L"endellipsis", L"true");
	pLabel->SetTextColor(dwColor);
	pLabel->SetText(Text);//�ؼ����Ծ͸����������ð�,�Ҽ�����һ��
}

void CListUIEx::SetItemData(int nItem, int nColumn, CControlUI* pControl)
{
	CHorizontalLayoutUI *pSubHor = GetListSubItem(nItem, nColumn);
	pSubHor->Add(pControl);//��ӵ����ؼ�
}

//************************************
// Method:    SetItemInset
// FullName:  DuiLib::CListUIEx::SetItemInset
// Access:    public 
// Returns:   int
// Qualifier: Jaine(1427208421@qq.com)
//Parameter:  int nRow:�ڼ���
// Parameter: int nItemIndex���ڼ���
// Parameter: int iLeft��Insert����߾�
// Parameter: int iTop:Insert���ϱ߾�
// Parameter: int iRight:Insert���ұ߾�
// Parameter: int iBottom:Insert���±߾�
// Description:����Item��inset,���ͻ���������
// CreateTime:2015/12/25
// Last Alter Time:
//************************************
bool CListUIEx::SetItemInset(int nRow, int nItemIndex, int iLeft, int iTop, int iRight, int iBottom)
{
	if (NULL == m_pHeader)   return false;
	//��ȡ����
	int nHeaderCount = m_pHeader->GetCount();
	if (nItemIndex > nHeaderCount)  return false;
	//��ȡ������CListContainerElement
	CListLabelElementExUI* m_pItemContainE = (CListLabelElementExUI*)GetItemAt(nRow);
	if (m_pItemContainE == NULL)  return false;
	//��ȡ���е�CHorizontalLayoutUUI
	CHorizontalLayoutUI* m_pItemHor = GetListSubItem(nRow, nItemIndex);//(CHorizontalLayoutUI*)m_pItemContainE->GetItemAt(nItemIndex);
	if (m_pItemHor == NULL)  return false;

	RECT m_rc;
	m_rc = m_pItemHor->GetInset();
	m_rc.left += iLeft;
	m_rc.top += iTop;
	m_rc.right += iRight;
	m_rc.bottom += iBottom;
	m_pItemHor->SetInset(m_rc);
	return true;
}
//************************************
// Method:    GetItemInset
// FullName:  DuiLib::CListUIEx::GetItemInset
// Access:    public 
// Returns:   bool��ִ�гɹ� true   ʧ�ܣ�false
// Qualifier: Jaine(1427208421@qq.com)
// Parameter: int nRow
// Parameter: int nItemIndex
//parameter: rc;�����ڱ߾�
// Description:����ListItemĳ��ĳ�е�Inset��ֵ
// CreateTime:2015/12/25
// Last Alter Time:
//************************************
bool CListUIEx::GetItemInset(int nRow, int nItemIndex, RECT& rc)
{
	if (NULL == m_pHeader)   return false;
	//��ȡ����
	int nHeaderCount = m_pHeader->GetCount();
	if (nItemIndex > nHeaderCount)  return false;
	//��ȡ������CListContainerElement
	CListLabelElementExUI* m_pItemContainE = (CListLabelElementExUI*)GetItemAt(nRow);
	if (m_pItemContainE == NULL)  return false;
	//��ȡ���е�CHorizontalLayoutUUI
	CHorizontalLayoutUI* m_pItemHor = GetListSubItem(nRow, nItemIndex); //(CHorizontalLayoutUI*)m_pItemContainE->GetItemAt(nItemIndex);
	if (m_pItemHor == NULL)  return false;
	rc = m_pItemHor->GetInset();
	return TRUE;
}

//************************************
// Method:    GetListItem
// FullName:  CListUIEx::GetListItem
// Access:    public 
// Returns:   CListLabelElementExUI*
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: int iIndex
// Description:��ȡָ����CListLabelElementExUIָ��
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
CListLabelElementExUI* CListUIEx::GetListItem(int iIndex)
{
	return static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(iIndex));
}

CHorizontalLayoutUI* CListUIEx::GetListSubItem(int iIndex, int iSubIndex)
{
	//��ȡ�����пؼ�
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(iIndex));
	if (pListItem == NULL)
		return NULL;
	return static_cast<CHorizontalLayoutUI*>(pListItem->GetItemAt(iSubIndex));
}
//************************************
// Method:    SetPos
// FullName:  CListUIEx::SetPos
// Access:    virtual public 
// Returns:   void
// Qualifier:Jaine(1427208421@qq.com)
// Parameter: RECT rc
// Parameter: bool bNeedInvalidate
// Description:
// CreateTime:2015/12/25
// Last Alter Time:
//************************************
void CListUIEx::SetPos(RECT rc, bool bNeedInvalidate)
{
	CListUI::SetPos(rc);
	if (m_pHeader == NULL)
	{
		return;
	}
	//�ж�������¼
	int nCount = m_pList->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CListContainerElementUI *pConta = static_cast<CListContainerElementUI*>(GetItemAt(i));
		for (int i = 0; i < m_pHeader->GetCount(); i++)
		{
			CListHeaderItemUI *pHeaderItem = static_cast<CListHeaderItemUI*>(m_pHeader->GetItemAt(i));
			if (pConta != NULL && pHeaderItem != NULL)
			{
				CHorizontalLayoutUI* pHorizontalLayout = (CHorizontalLayoutUI*)pConta->GetItemAt(i);

				RECT rtHeader = pHeaderItem->GetPos();
				RECT rt = pHorizontalLayout->GetPos();
				rt.left = rtHeader.left;
				rt.right = rtHeader.right;
				pHorizontalLayout->SetFixedWidth(rt.right - rt.left);//�˾����Ҫ�������������HeadItem��Ȳ�ͬ������
				pHorizontalLayout->SetPos(rt);
			}
		}
	}
}
CDuiString CListUIEx::GetItemText(int nRow, int nCol)
{
	CDuiString m_str = L"";
// 	CHorizontalLayoutUI* m_pHor = GetListSubItem(nRow, nCol);// (CHorizontalLayoutUI*)GetListSubItem(nRow, nCol);
// 	if (m_pHor)
// 	{
		//��ȡ�����CLabelUI
	m_str = GetTextButton(nRow, nCol)->GetText();// ((CLabelUI*)m_pHor->GetItemAt(1))->GetText();//��Ϊÿ��column��������Label������һ���ǵ�2��Label��ʾtext��
	//}
	return m_str;
}
void CListUIEx::SetItemIcon(int nItem, int nColumn, LPCTSTR ImgFile, int nWidth, int nHeight)
{
	if (lstrlen(ImgFile) <= 0) return;
	//����ǵ�һ�Σ����½�һ��label,����ͼƬ

	//CHorizontalLayoutUI *pSubHor = GetListSubItem(nItem, nColumn);
	CButtonUI* m_pLabel = GetIconButton(nItem, nColumn); //(CLabelUI*)pSubHor->GetItemAt(1);
	if (!m_pLabel){
		return;
	}
	//���������Ϣ
 	m_pLabel->SetFixedWidth(m_iCurItemHeight);
 	m_pLabel->SetFixedHeight(m_iCurItemHeight);
	if (m_iCurItemHeight > nHeight)
	{
		RECT  rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = 0;
		rect.bottom = 0;
		m_pLabel->SetPadding(rect);
		CDuiString m_strPic;
		m_strPic.Format(_T("file = '%s' dest='0,%d,%d,%d' source='0,0,%d,%d'"), ImgFile, (m_iCurItemHeight - nHeight) / 2, nWidth, nHeight + (m_iCurItemHeight - nHeight) / 2, nWidth, nHeight);
		m_pLabel->SetAttribute(_T("bkimage"), m_strPic);
	}
	else
	{
		m_pLabel->SetBkImage(ImgFile);
	}
}
bool  CListUIEx::SetItemUseData(int nItem, LPCTSTR strUserData)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(nItem));
	if (pListItem == NULL) return false;
	pListItem->SetUserData(strUserData);
	return true;
}

bool CListUIEx::SetItemTag(int nItem, LPVOID strUserTag)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(nItem));
	if (pListItem == NULL) return false;
	pListItem->SetTag((UINT_PTR)strUserTag);
	return true;
}
CDuiString CListUIEx::GetItemUseData(int nItem)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(nItem));
	if (pListItem == NULL) return L"";
	return pListItem->GetUserData();
}
UINT_PTR CListUIEx::GetItemTag(int nItem)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CListUI::GetItemAt(nItem));
	if (pListItem == NULL) return NULL;
	return pListItem->GetTag();
}
void CListUIEx::SetRowButtonName(int nItem, int nColumn, LPCTSTR strName)
{
	if (lstrlen(strName) <= 0) return;
	CButtonUI* m_pRowButton = GetRowButton(nItem, nColumn);
	if (m_pRowButton)
	{
		m_pRowButton->SetName(strName);
	}
}
