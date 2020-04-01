#include "pch.h"
#include "ComboExUI.h"
#include "ListLabelElementExUI.h"




CComboExUI::CComboExUI()
{
}


CComboExUI::~CComboExUI()
{
}
LPCTSTR CComboExUI::GetClass() const
{
	// �����������ȥ����ͷ����C�ַ�
	return _T("ComboExUI");
}


LPVOID CComboExUI::GetInterface(LPCTSTR pstrName)
{
	// 		�����xml�ؼ�������
	if (_tcsicmp(pstrName, _T("ComboEx")) == 0) return static_cast<CComboExUI*>(this);
	return CComboUI::GetInterface(pstrName);
}
//************************************
// Method:    SetItemData
// FullName:  CComboExUI::SetItemData
// Access:    public 
// Returns:   void
// Qualifier: Jaine(1427208421@qq.com)
// Parameter: int nItem���ڼ���
// Parameter: CControlUI * pControl����ؿؼ�
// Description:����������ݵ�CHorizontalLayoutUI
// CreateTime:2016/01/04
// Last Alter Time:
//************************************
void CComboExUI::SetItemData(int nItem, CControlUI* pControl)
{
	CHorizontalLayoutUI *pSubHor = GetListSubItem(nItem);
	pSubHor->Add(pControl);//��ӵ����ؼ�
}
//************************************
// Method:    GetListSubItem
// FullName:  CComboExUI::GetListSubItem
// Access:    public 
// Returns:   �ɹ�����:CHorizontalLayoutUI*           ʧ�ܷ���NULL
// Qualifier: Jaine(1427208421@qq.com)
// Parameter: int iItemIndex
// Description:��ȡCListContainerElementUI�е�CHorizontalLayoutUI����
// CreateTime:2016/01/04
// Last Alter Time:
//************************************
CHorizontalLayoutUI* CComboExUI::GetListSubItem(int iItemIndex) const
{
	//��ȡ�����пؼ�
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CComboUI::GetItemAt(iItemIndex));
	if (pListItem == NULL)
		return NULL;
	return static_cast<CHorizontalLayoutUI*>(pListItem->GetItemAt(0));
}
//************************************
// Method:    InsertItem
// FullName:  CComboExUI::InsertItem
// Access:    public 
// Returns:   int:�ɹ������ص�ǰ�����λ�á�   ʧ�ܣ�-1
// Qualifier: Jaine(1427208421@qq.com)
// Parameter: int nHeight
// Description:���һ����Ϣ��Combo
// CreateTime:2016/01/04
// Last Alter Time:
//************************************
int CComboExUI::InsertItem(int nHeight)
{
	//�½�һ��CListContainerElement
	CListLabelElementExUI *pListItem = new CListLabelElementExUI;
	//���ø߶�
	pListItem->SetFixedHeight(nHeight);
	
	//pListItem->SetFixedWidth(200);

	//����Client����
	RECT m_rc;
	m_rc.left = 1;
	m_rc.top = 1;
	m_rc.right = 1;
	m_rc.bottom = 1;
	pListItem->SetInset(m_rc);
	CHorizontalLayoutUI* pSubHor = new CHorizontalLayoutUI;
	//��CHorizontalLayoutUI��ӽ���
	pListItem->Add(pSubHor);
	//���뵽Combo
	int nItem = GetCount();//��ȡ��������ʲôλ�����
	if (!CComboUI::AddAt(pListItem, nItem))
	{
		delete pListItem;
		pListItem = NULL;
		return -1;
	}
	return nItem;
}
void CComboExUI::SetItemIcon(int nItem, LPCTSTR ImgFile, int nWidth, int nHeight, int nPaddingLeft /*= 0*/, int nPaddingTop /*= 0*/, int nPaddingRight /*= 0*/, int nPaddingBottom /*= 0*/)
{
	if (lstrlen(ImgFile) <= 0) return;
	else if (GetListSubItem(nItem)->GetCount() >= 2)//ֻ�������ؼ���ʱ���ȥ���ģ�����Ӧ�ü���һ���жϣ���label��ȥ�޸�
	{
		GetListSubItem(nItem)->GetItemAt(0)->SetBkImage(ImgFile);
		return;
	}
	//����ǵ�һ�Σ����½�һ��label,����ͼƬ
	CLabelUI *m_pLabel = new CLabelUI;
	RECT rc;
	rc.left = nPaddingLeft;
	rc.top = nPaddingTop;
	rc.right = nPaddingRight;
	rc.bottom = nPaddingBottom;
	m_pLabel->SetPadding(rc);
	m_pLabel->SetFixedWidth(nWidth);
	m_pLabel->SetFixedHeight(nHeight);
	m_pLabel->SetBkImage(ImgFile);
	//SetItemData(nItem, m_pLabel);//��ӵ����ؼ�
	CHorizontalLayoutUI *pSubHor = GetListSubItem(nItem);
	pSubHor->AddAt(m_pLabel,0);//��ӵ����ؼ�
}
//SetItemData
//nItem:�ڼ���
//nColumn:�ڼ���
//Text:����
//nAlign:���� ��ʽ 0�� �����  1���м� 2���Ҷ���
void CComboExUI::SetItemText(int nItem, LPCTSTR Text, int nFontId /*= 0*/, int nAlign)//�˺���������
{
	CLabelUI *pLabel = new CLabelUI;
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
	//��������
	pLabel->SetFont(nFontId);
	pLabel->SetAttribute(L"Cursor", L"hand");
	//��ʾ���˵�������...����
	pLabel->SetAttribute(L"textpadding", L"4,1,1,1");
	pLabel->SetAttribute(L"endellipsis", L"true");
	pLabel->SetText(Text);//�ؼ����Ծ͸����������ð�,�Ҽ�����һ��
	SetItemData(nItem, pLabel);//��ӵ����ؼ�
}
bool CComboExUI::SetItemTag(int nItem, LPVOID strUserTag)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CComboUI::GetItemAt(nItem));
	if (pListItem == NULL) return false;
	pListItem->SetTag((UINT_PTR)strUserTag);
	return true;
}
CDuiString CComboExUI::GetItemUseData(int nItem) const
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CComboUI::GetItemAt(nItem));
	if (pListItem == NULL) return L"";
	return pListItem->GetUserData();
}
CDuiString CComboExUI::GetItemText(int nItem) const
{
	CHorizontalLayoutUI*m_pHor = GetListSubItem(nItem);
	CDuiString m_str;
	if (nItem<0)
	{
		return _T("");
	}
	if (!m_pHor)
	{
		return _T("");
	}
	if (GetListSubItem(nItem)->GetCount() >= 2)//ֻ�������ؼ���ʱ���ȥ���ģ�����Ӧ�ü���һ���жϣ���label��ȥ�޸�
	{
		m_str=GetListSubItem(nItem)->GetItemAt(1)->GetText();
	}
	else
	{
		m_str= GetListSubItem(nItem)->GetItemAt(0)->GetText();
	}
	return m_str;
	
}
UINT_PTR CComboExUI::GetItemTag(int nItem)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CComboUI::GetItemAt(nItem));
	if (pListItem == NULL) return NULL;
	return pListItem->GetTag();
}
bool  CComboExUI::SetItemUseData(int nItem, LPCTSTR strUserData)
{
	CListLabelElementExUI *pListItem = static_cast<CListLabelElementExUI*>(CComboUI::GetItemAt(nItem));
	if (pListItem == NULL) return false;
	pListItem->SetUserData(strUserData);
	return true;
}
bool CComboExUI::SetItemInset(int iItemIndex, int iLeft, int iTop, int iRight, int iBottom)
{
	//��ȡ������CListContainerElement
	CListLabelElementExUI* m_pItemContainE = (CListLabelElementExUI*)GetItemAt(iItemIndex);
	if (m_pItemContainE == NULL)  return false;
	//��ȡ���е�CHorizontalLayoutUUI
	CHorizontalLayoutUI* m_pItemHor = (CHorizontalLayoutUI*)m_pItemContainE->GetItemAt(0);
	if (m_pItemHor == NULL)  return false;

	RECT m_rc;
	m_rc = m_pItemHor->GetInset();
	m_rc.left = iLeft;
	m_rc.top = iTop;
	m_rc.right = iRight;
	m_rc.bottom = iBottom;
	m_pItemHor->SetInset(m_rc);
	return true;
}
bool CComboExUI::GetItemInset(int iItemIndex, RECT& rc)
{
	//��ȡ������CListContainerElement
	CListLabelElementExUI* m_pItemContainE = (CListLabelElementExUI*)GetItemAt(iItemIndex);
	if (m_pItemContainE == NULL)  return false;
	//��ȡ���е�CHorizontalLayoutUUI
	CHorizontalLayoutUI* m_pItemHor = (CHorizontalLayoutUI*)m_pItemContainE->GetItemAt(0);
	if (m_pItemHor == NULL)  return false;
	rc = m_pItemHor->GetInset();
	return TRUE;
}
CDuiString CComboExUI::GetComboUseData()
{
	return GetItemUseData(m_iCurSel);
}
CDuiString CComboExUI::GetComboText() const
{
	return GetItemText(m_iCurSel);
}
void CComboExUI::PaintText(HDC hDC)
{
	if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();
	if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	CDuiString sText = GetComboText();
	if (sText.IsEmpty()) return;
	int nLinks = 0;
	if (IsEnabled()) {
		if (m_bShowHtml)
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwTextColor, \
			NULL, NULL, nLinks, 1,DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, \
			m_iFont, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
	else {
		if (m_bShowHtml)
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
			NULL, NULL, nLinks,1, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
			m_iFont, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
}
void CComboExUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CControlUI::DoPaint(hDC, rcPaint,this);
}