#include "pch.h"
#include "ListLabelElementExUI.h"


CListLabelElementExUI::CListLabelElementExUI()
{
	m_iChildItemNumber = 0;
	m_bExpandFlag = true;
	m_pItemParent = NULL;
	m_iTotalChildItemNumber = 0;
}


CListLabelElementExUI::~CListLabelElementExUI()
{
}
LPCTSTR CListLabelElementExUI::GetClass() const
{
	// 这个是类名，去掉开头的是C字符
	return _T("ListLabelElementExUI");
}
//************************************
// Method:    GetChildItemCount
// FullName:  CListLabelElementExUI::GetChildItemCount
// Access:    public 
// Returns:   int
// Qualifier: Jaine(Beyond@chinabetter.com)
// Description:获取子item的数量
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
int CListLabelElementExUI::GetChildItemCount()
{
	return m_iChildItemNumber;
}
int CListLabelElementExUI::SetChildItemCount(int nNumber)
{
	if (nNumber<0)
	{
		return -1;
	}
	m_iChildItemNumber = nNumber;
	return m_iChildItemNumber;
}
CListLabelElementExUI* CListLabelElementExUI::GetItemParent()//得到父节点指针
{
	return  m_pItemParent;
}
CListLabelElementExUI* CListLabelElementExUI::SetItemParent(CListLabelElementExUI *pParent)//设置父节点指针
{
	m_pItemParent = pParent;
	return  m_pItemParent;
}
//************************************
// Method:    SetExpandFlag
// FullName:  CListLabelElementExUI::SetExpandFlag
// Access:    public 
// Returns:   bool
// Qualifier: Jaine(Beyond@chinabetter.com)
// Parameter: bool bFlag
// Description:设置当前展开状态
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
bool CListLabelElementExUI::SetExpandFlag(bool bFlag)
{
	m_bExpandFlag = bFlag;
	return  m_bExpandFlag;
}
//************************************
// Method:    GetExpandFlag
// FullName:  CListLabelElementExUI::GetExpandFlag
// Access:    public 
// Returns:   bool
// Qualifier: Jaine(Beyond@chinabetter.com)
// Description:获取当前展开状态
// CreateTime:2016/01/31
// Last Alter Time:
//************************************
bool CListLabelElementExUI::GetExpandFlag()
{
	return m_bExpandFlag;
}
LPVOID CListLabelElementExUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, DUI_CTR_LISTITEM) == 0) return static_cast<IListItemUI*>(this);
	if (_tcsicmp(pstrName, _T("ListLabelElementEx")) == 0) return static_cast<CListLabelElementExUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}
void CListLabelElementExUI::SetPos(RECT rc, bool bNeedInvalidate)
{
	CHorizontalLayoutUI::SetPos(rc, bNeedInvalidate);
	if (m_pOwner == NULL) return;
}
void CListLabelElementExUI::DoEvent(TEventUI& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pOwner != NULL) m_pOwner->DoEvent(event);
		else CContainerUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_DBLCLICK)
	{
		/*if (IsEnabled()) {
				m_pManager->SendNotify((CListUIEx*)this->GetOwner(), _T("dbclick"));//可以接收双击消息
			Activate();
			Invalidate();
		}*/
		return;
	}
	if (event.Type == UIEVENT_KEYDOWN && IsEnabled())
	{
		if (event.chKey == VK_RETURN) {
			Activate();
			Invalidate();
			return;
		}
	}
	if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN)
	{
		if (IsEnabled()){
			Select();
			Invalidate();
			
		}
		return;
	}
	if (event.Type == UIEVENT_BUTTONUP)
	{
		if (IsEnabled()){
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		return;
	}
	if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (IsEnabled()) {
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSELEAVE)
	{
		if ((m_uButtonState & UISTATE_HOT) != 0) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_TIMER)
	{
		m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
		return;
	}
	if (event.Type == UIEVENT_CONTEXTMENU)
	{
		if (IsContextMenuUsed()) {
			m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			return;
		}
	}
	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	if (m_pOwner != NULL) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
}
void CListLabelElementExUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;

	RECT rcTemp = { 0 };
	if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

	CRenderClip clip;
	CRenderClip::GenerateClip(hDC, rcTemp, clip);
	CControlUI::DoPaint(hDC, rcPaint,this);

	DrawItemBk(hDC, m_rcItem);

	if (m_items.GetSize() > 0) {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if (!::IntersectRect(&rcTemp, &rcPaint, &rc)) {
			for (int it = 0; it < m_items.GetSize(); it++) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if (!pControl->IsVisible()) continue;
				if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
				if (pControl->IsFloat()) {
					if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
					pControl->DoPaint(hDC, rcPaint,this);
				}
			}
		}
		else {
			CRenderClip childClip;
			CRenderClip::GenerateClip(hDC, rcTemp, childClip);
			for (int it = 0; it < m_items.GetSize(); it++) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if (!pControl->IsVisible()) continue;
				if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
				if (pControl->IsFloat()) {
					if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
					CRenderClip::UseOldClipBegin(hDC, childClip);
					pControl->DoPaint(hDC, rcPaint,this);
					CRenderClip::UseOldClipEnd(hDC, childClip);
				}
				else {
					if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) continue;
					pControl->DoPaint(hDC, rcPaint,this);
				}
			}
		}
	}

	if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible()) {
		if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) {
			m_pVerticalScrollBar->DoPaint(hDC, rcPaint,this);
		}
	}

	if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible()) {
		if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) {
			m_pHorizontalScrollBar->DoPaint(hDC, rcPaint,this);
		}
	}
}
void CListLabelElementExUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{
	ASSERT(m_pOwner);
	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	DWORD iBackColor = 0;
	if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) iBackColor = pInfo->dwBkColor;

	if ((m_uButtonState & UISTATE_HOT) != 0) {
		iBackColor = pInfo->dwHotBkColor;
	}
	if (IsSelected()) {
		iBackColor = pInfo->dwSelectedBkColor;
	}
	if (!IsEnabled()) {
		iBackColor = pInfo->dwDisabledBkColor;
	}
	if (iBackColor != 0) {
		CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}

	if (!IsEnabled()) {
		if (!pInfo->sDisabledImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage)) pInfo->sDisabledImage.Empty();
			else return;
		}
	}
	if (IsSelected()) {
		if (!pInfo->sSelectedImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage)) pInfo->sSelectedImage.Empty();
			else return;
		}
	}
	if ((m_uButtonState & UISTATE_HOT) != 0) {
		if (!pInfo->sHotImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)pInfo->sHotImage)) pInfo->sHotImage.Empty();
			else return;
		}
	}
	if (!m_sBkImage.IsEmpty()) {
		if (!pInfo->bAlternateBk || m_iIndex % 2 == 0) {
			if (!DrawImage(hDC, (LPCTSTR)m_sBkImage)) m_sBkImage.Empty();
		}
	}

	if (m_sBkImage.IsEmpty()) {
		if (!pInfo->sBkImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)pInfo->sBkImage)) pInfo->sBkImage.Empty();
			else return;
		}
	}

	if (pInfo->dwLineColor != 0)
	{
		if (pInfo->bShowRowLine)
		{
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
			CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
		if (pInfo->bShowColumnLine)
		{
			for (int i = 0; i < pInfo->nColumns; i++) {
				RECT rcLine = { pInfo->rcColumn[i].right - 1, m_rcItem.top, pInfo->rcColumn[i].right - 1, m_rcItem.bottom };
				CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
			}
		}
	}
}