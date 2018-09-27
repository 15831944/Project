// AbsEleFixLanzhouProperty.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"

#include "AbsEleFixLanzhouProperty.h"
#include ".\abselefixlanzhouproperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixLanzhouProperty dialog
const BYTE WIDTHSPACE = 5;
const BYTE HEIGHTSPACE = 5;
const BYTE NUM_OF_ROW  = 3;
const BYTE NUM_OF_COL  = 15;

CAbsEleFixLanzhouProperty::CAbsEleFixLanzhouProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent /*=NULL*/)
	: CDialog(CAbsEleFixLanzhouProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsFixProperty)
		m_s_kilo = 0;
		m_e_meter = 0;
		m_s_meter = 0;
		m_e_kilo = 0;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ele_title="";
	m_pFigure = pFigure;
	m_pDoc = pDoc;
	m_pcheckBoxList = NULL;
	m_lockprestation = FALSE;
	m_locknextstation = FALSE;
}

CAbsEleFixLanzhouProperty::~CAbsEleFixLanzhouProperty()
{
	if(m_pcheckBoxList != NULL)
	{
		delete []m_pcheckBoxList;
		m_pcheckBoxList = NULL;
	}
}

void CAbsEleFixLanzhouProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsEleFixLanzhouProperty)
	DDX_Control(pDX, IDC_COMBO_TEXTPOS, m_TextPos);
	DDX_Control(pDX, IDC_COMBO_TITLE, m_title);
	DDX_CBString(pDX, IDC_COMBO_TITLE, m_ele_title);
	DDV_MaxChars(pDX, m_ele_title, 255);  //ʵ�������Ϊ63
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_direction);
		
	DDX_Text(pDX, IDC_EDIT1, m_s_kilo);
	DDV_MinMaxInt(pDX, m_s_kilo, 0, 9999);
	DDX_Text(pDX, IDC_E_METER, m_e_meter);
	DDV_MinMaxInt(pDX, m_e_meter, 0, 9999);
	DDX_Text(pDX, IDC_S_METER, m_s_meter);
	DDV_MinMaxInt(pDX, m_s_meter, 0, 9999);
	DDX_Text(pDX, IDC_E_KILO, m_e_kilo);
	DDV_MinMaxInt(pDX, m_e_kilo, 0, 9999);

	DDX_Check(pDX, IDC_LOCK_PRESTATION, m_lockprestation);
	DDX_Check(pDX, IDC_LOCK_NEXTSTATION, m_locknextstation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAbsEleFixLanzhouProperty, CDialog)
	//{{AFX_MSG_MAP(CAbsEleFixLanzhouProperty)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, OnCbnSelchangeComboDirection)
	ON_BN_CLICKED(IDC_RADIO_SJXY, OnBnClickedRadioSjxy)
	ON_BN_CLICKED(IDC_RADIO_SJXJ, OnBnClickedRadioSjxj)
	ON_BN_CLICKED(IDC_RADIO_SYXY, OnBnClickedRadioSyxy)
	ON_BN_CLICKED(IDC_RADIO_SYXJ, OnBnClickedRadioSyxj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixLanzhouProperty message handlers
BOOL CAbsEleFixLanzhouProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_s_kilo = m_pFigure->position;  
	m_s_meter = m_pFigure->s_meter;
	m_e_kilo  = m_pFigure->e_kilo ;
	m_e_meter = m_pFigure->e_meter;

	m_linecount = 0;
	memset(m_line_list,0,sizeof(m_line_list));

	int upIdx   = m_pFigure->station;
	int downIdx = m_pFigure->index; 
	if(m_pFigure->station > m_pFigure->index)
	{
		upIdx   = m_pFigure->index;
		downIdx = m_pFigure->station; 
	}

	std::vector<int> all_lineno;
	
	m_pDoc->GetMutiNeibAbsLines(upIdx, downIdx, all_lineno);
	for(int i=0; i<all_lineno.size()&&i<128; i++)
	{
		int line_no = all_lineno[i];
		const ABS_LINE* line = m_pDoc->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(line_no);
		if(line!=NULL)
		{
			memset(m_line_list[m_linecount].name, 0, 32);
			strncpy(m_line_list[m_linecount].name, line->sLineName, 31);
			m_line_list[m_linecount].lineNo = line_no;
			m_line_list[m_linecount].dir    = line->nDirect;
			m_linecount++;
		}
	}

	m_direction.AddString("����");   // 0
	m_direction.AddString("����");   // 1 
	m_direction.AddString("������"); // 2
	m_direction.AddString("����"); // 3

	m_direction.SetCurSel((m_pFigure->line&0x0003));

	m_TextPos.AddString("��");
	m_TextPos.AddString("��");
	m_TextPos.AddString("��");
	m_TextPos.AddString("��");
	m_TextPos.AddString("��");

	if(IsLineTextPosBottom(m_pFigure->line))
	{
		m_TextPos.SetCurSel(1);
	}
	else if(IsLineTextPosMiddle(m_pFigure->line))
	{
		m_TextPos.SetCurSel(2);
	}
	else if(IsLineTextPosLeft(m_pFigure->line))
	{
		m_TextPos.SetCurSel(3);
	}
	else if(IsLineTextPosRight(m_pFigure->line))
	{
		m_TextPos.SetCurSel(4);
	}
	else
	{
		m_TextPos.SetCurSel(0);
	}

	//��ȡ����
	CString datadir = m_pDoc->GetDataPath();
	CStdioFile f_words;
	
	if(f_words.Open(datadir+"\\mark_text.cfg", CFile::modeRead))
	{
		CString linetext;
		while(f_words.ReadString(linetext))
		{
			m_title.InsertString(-1,linetext);
		}
		f_words.Close();
	}
	else
	{
		m_title.AddString("��������");
		m_title.AddString("�����촰");
	}
		
	if(m_pFigure->slow_time&0x0001)//��1λΪ1
		m_lockprestation = TRUE;
	else
		m_lockprestation = FALSE;

	if(m_pFigure->slow_time&0x0002)//��2λΪ1
		m_locknextstation = TRUE;
	else
		m_locknextstation = FALSE;

	if(m_pFigure->slow_time&0x0004)//��3λΪ1
		m_DownFlag = TRUE;
	else
		m_DownFlag = FALSE;

	if(m_pFigure->slow_time&0x0008)//��4λΪ1
		m_UpFlag = TRUE;
	else
		m_UpFlag = FALSE;
	
	SetPosCheck();
	
	CString dcmdno, reason;
	m_pDoc->ParseMarkReasonText(m_pFigure->text, dcmdno, reason);

	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->SetWindowText(dcmdno);

    m_ele_title = reason;
			
	UpdateData(FALSE);
	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO_TITLE);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_COMBO_DIRECTION);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_TEXTPOS);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_RADIO_SJXY);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_RADIO_SJXJ);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_RADIO_SYXY);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_RADIO_SYXJ);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_KILO);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_S_METER);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_E_METER);
		pWnd->EnableWindow(FALSE);
	}
	
	CreateCheckBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbsEleFixLanzhouProperty::OnOK() 
{
	if(m_pFigure == NULL)
		CDialog::OnOK();
	
	UpdateData(TRUE);

	m_pFigure->position = m_s_kilo;
	m_pFigure->s_meter = m_s_meter;
	m_pFigure->e_kilo = m_e_kilo;
	m_pFigure->e_meter = m_e_meter;

	CString dcmdno = "";
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_EDIT_DCMDNO);
	if(NULL != pWnd)
		pWnd->GetWindowText(dcmdno);

	CString reason;
	m_title.GetWindowText (reason);
	
	CString str = m_pDoc->CombineReasonText(dcmdno, reason);
	int len = str.GetLength();
	if(len >=  MAX_SCHD_BLOCK_TEXT_LEN)
	{
	     AfxMessageBox("����<ԭ��>̫��,���ܳ���125������,����������!");
		 return;
	}
    strncpy(m_pFigure->text,(LPCTSTR)str, len);
	m_pFigure->text[len] = 0;

	//�������� 	0"����", 1"����", 2"������" 3"����"
	int selIndex = m_direction.GetCurSel();
	m_pFigure->count = 0;
	if(selIndex == 0)
	{
		SetLineDirectionUp(m_pFigure->line);
		for(int j=0; j<m_linecount; j++)
		{
			if((m_line_list[j].dir == 0 || m_line_list[j].dir == 3) && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}
	else if(selIndex == 1)
	{
		SetLineDirectionDown(m_pFigure->line);
		for(int j=0; j<m_linecount; j++)
		{
			if((m_line_list[j].dir == 1 || m_line_list[j].dir == 3) && m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}
	else
	{
		if(selIndex == 2)
			SetLineDirectionDual(m_pFigure->line);
		else
			SetLineDirectionNone(m_pFigure->line);

		for(int j=0; j<m_linecount; j++)
		{
			if(m_pcheckBoxList[j].GetCheck() == 1 && m_pFigure->count < MAX_SCHD_BLOCK_ELE_COUNT)
			{
				m_pFigure->ele_number[m_pFigure->count] = m_line_list[j].lineNo;
				m_pFigure->count++;
			}
		}
	}
	
	//���ֵ�λ�� 	0"��",1"��"
	selIndex = m_TextPos.GetCurSel();
	if(selIndex == 0)
		SetLineTextPosTop(m_pFigure->line);
	else if(selIndex == 1)
		SetLineTextPosBottom(m_pFigure->line);
	else if(selIndex == 2)
		SetLineTextPosMiddle(m_pFigure->line);
	else if(selIndex == 3)
		SetLineTextPosLeft(m_pFigure->line);
	else if(selIndex == 4)
		SetLineTextPosRight(m_pFigure->line);

	//�Ƿ�������·���վ
	if(m_lockprestation)
		m_pFigure->slow_time |= 0x0001;//��1λ��1
	else
		m_pFigure->slow_time &= ~0x0001;//��1λ��0

	if(m_locknextstation)
		m_pFigure->slow_time |= 0x0002;//��2λ��1
	else
		m_pFigure->slow_time &= ~0x0002;//��2λ��0

	// ����Զ��
	if(m_DownFlag) //  1 X X
		m_pFigure->slow_time |= 0x0004;//��3λ��1
	else
		m_pFigure->slow_time &= ~0x0004;//��3λ��0

	if(m_UpFlag) // 1 X X X
		m_pFigure->slow_time |= 0x0008;//��4λ��1
	else
		m_pFigure->slow_time &= ~0x0008;//��4λ��0

	CDialog::OnOK();
}


void CAbsEleFixLanzhouProperty::CreateCheckBox()
{
	if(m_linecount<=0)
		return;

	CRect buttonRect;
	CWnd *boxWnd = GetDlgItem(IDC_STATIC_LINEBOX);
    if(boxWnd == NULL)
		return;

	boxWnd->GetClientRect(&buttonRect);
	boxWnd->MapWindowPoints(this,&buttonRect);

	int width  = (buttonRect.Width() - 2*WIDTHSPACE) / NUM_OF_ROW;
	int height = (buttonRect.Height()- 3*HEIGHTSPACE)/NUM_OF_COL;

	buttonRect.left   = buttonRect.left + WIDTHSPACE;
	buttonRect.right  = buttonRect.left + width;
	buttonRect.top    = buttonRect.top + 4*HEIGHTSPACE;
	buttonRect.bottom = buttonRect.top + height;

	int left = buttonRect.left;
	int right = buttonRect.right;
	int bottom = buttonRect.bottom;
	int top = buttonRect.top;

	m_pcheckBoxList = new CButton[m_linecount];

	for(int i=1;i<=m_linecount;i++)
	{
		CString strBtnName;
		strBtnName.Format("%s(%d)", m_line_list[i-1].name, m_line_list[i-1].lineNo);
		
		m_pcheckBoxList[i-1].Create(strBtnName, 
			                        WS_CHILD | WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX, 
			                        buttonRect, 
									this, 
									IDC_CHECKBOX_START + i);
		m_pcheckBoxList[i-1].SetFont(this->GetFont());
		buttonRect.left   = left   + ((i)%NUM_OF_ROW) * (width);
		buttonRect.right  = right  + ((i)%NUM_OF_ROW) * (width);
		buttonRect.top    = top    + ((i)/NUM_OF_ROW) * (height);
		buttonRect.bottom = bottom + ((i)/NUM_OF_ROW) * (height);
	}

	for(int i=0; i<m_pFigure->count; i++)
	{
		for(int j=0; j<m_linecount; j++)
		{
			if(m_line_list[j].lineNo == m_pFigure->ele_number[i])
			{
				m_pcheckBoxList[j].SetCheck(TRUE);
				break;
			}
		}
	}

	if(!m_pDoc->GetOperatorRight(OPER_RIGHT::MARK_EDIT))
	{
		for(int j=0; j<m_linecount; j++)
		{
			m_pcheckBoxList[j].EnableWindow(FALSE);
		}
	}
}
void CAbsEleFixLanzhouProperty::OnCbnSelchangeComboDirection()
{
	int selIndex = m_direction.GetCurSel();
	for(int i=0; i<m_linecount; i++)
	{
		m_pcheckBoxList[i].SetCheck(0);
		m_pcheckBoxList[i].EnableWindow(TRUE);
		if(selIndex == 0)
		{
			if(m_line_list[i].dir == 0 || m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else if(selIndex == 1)
		{
			if(m_line_list[i].dir == 1 || m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else if(selIndex == 3)
		{
			if(m_line_list[i].dir == 3)
				m_pcheckBoxList[i].SetCheck(1);
			else
				m_pcheckBoxList[i].EnableWindow(FALSE);
		}
		else
		{
			m_pcheckBoxList[i].SetCheck(1);
		}
	}
}

void CAbsEleFixLanzhouProperty::OnBnClickedRadioSjxy()//�Ͻ���Զ 0 1 X X
{
	m_UpFlag = FALSE;
	m_DownFlag = TRUE;
}

void CAbsEleFixLanzhouProperty::OnBnClickedRadioSjxj()//�Ͻ��½�  0 0 X X
{
	m_UpFlag = FALSE;
	m_DownFlag = FALSE;
}

void CAbsEleFixLanzhouProperty::OnBnClickedRadioSyxy()//��Զ��Զ  1 1 X X
{
	m_UpFlag = TRUE;
	m_DownFlag = TRUE;
}

void CAbsEleFixLanzhouProperty::OnBnClickedRadioSyxj()//��Զ�½� 1 0 X X
{
	m_UpFlag = TRUE;
	m_DownFlag = FALSE;
}

void CAbsEleFixLanzhouProperty::SetPosCheck()
{
	CButton* pWnd = NULL;
	if(m_UpFlag == TRUE)
	{
		if(m_DownFlag == TRUE) //��Զ��Զ
		{
			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXY);
			if(NULL != pWnd)
				pWnd->SetCheck(1);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);
		}
		else //��Զ�½�
		{
			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(1);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);
		}
	}
	else
	{
		if(m_DownFlag == TRUE) //�Ͻ���Զ
		{
			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXY);
			if(NULL != pWnd)
				pWnd->SetCheck(1);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

		}
		else //�Ͻ��½�
		{
			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(1);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SJXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXY);
			if(NULL != pWnd)
				pWnd->SetCheck(0);

			pWnd = (CButton*)GetDlgItem(IDC_RADIO_SYXJ);
			if(NULL != pWnd)
				pWnd->SetCheck(0);
		}
	}	
}