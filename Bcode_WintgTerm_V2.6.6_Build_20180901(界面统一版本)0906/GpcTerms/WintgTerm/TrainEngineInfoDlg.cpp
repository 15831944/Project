// UserListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainEngineInfoDlg.h"
#include "function.h"
#include "tgmsg.h"
#include "messages.h"
#include "memshare.h"
#include "resource.h"
#include "widgets.h"
#include ".\trainengineinfodlg.h"
#import  <msxml.dll> named_guids

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainEngineInfoDlg dialog
#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
{MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
	if(pNode) \
{ \
	_variant_t v; \
	v = pNode->GetnodeTypedValue(); \
	dest = CString((v.bstrVal)); \
	SysFreeString(v.bstrVal); \
	v.Clear(); \
} \
			else dest.Empty();};

#define  GET_NODE_INTVALUE(dest,pXMLDoc,node) \
  {MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
   if(pNode) \
			{ \
				_variant_t v; \
				v = pNode->GetnodeTypedValue(); \
				dest = atoi(CString((v.bstrVal))); \
				SysFreeString(v.bstrVal); \
                v.Clear(); \
			} \
			else dest=-1;};

char* item[ITEM_NUM]={"����","���ص绰","���񵣵���", "��������", "����", "���ڵص�", "����ʱ��", "Ԥ�Ƴ���ʱ��"};

CTrainEngineInfoDlg::CTrainEngineInfoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTrainEngineInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrainEngineInfoDlg)
	//}}AFX_DATA_INIT
}


void CTrainEngineInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrainEngineInfoDlg)
	DDX_Control(pDX, IDC_LIST_ENGINE, m_engine_list);
	DDX_Control(pDX, IDC_LIST_ENGINE_CREW, m_list_box);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrainEngineInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CTrainEngineInfoDlg)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST_ENGINE_CREW, OnLbnSelchangeListEngineCrew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainEngineInfoDlg message handlers
extern int GSLogin(const std::string& szPara, std::string& szResult);
BOOL CTrainEngineInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwStyle = m_engine_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_engine_list.SetExtendedStyle(dwStyle);
	
	m_engine_list.SetBkColor(RGB(192,192,192));
	m_engine_list.SetTextBkColor(RGB(192,192,192));

	m_engine_list.InsertColumn(0,"�����ѯ��",LVCFMT_LEFT,100);
	m_engine_list.InsertColumn(1,"�����ѯ��Ϣ",LVCFMT_LEFT,400);

	for(int col=0; col<ITEM_NUM; col++)
	{
		CString str=item[col];
		int row=m_engine_list.GetItemCount();
		m_engine_list.InsertItem(row, str);
	}
	
	bool rc = true;
	std::string rs;
	if (GSLogin("", rs))
	{
		AfxMessageBox(LPSTR(rs.c_str()));
		rc = false;
		GpcLogSysMessage(0,"GSoap��ʼ��ʧ��.");
	}
	
	if(rc)
	{
		if(RequestEngineInfo())
		{
			for(int i=0; i<m_nEngineCrewInfoCount; i++)
			{
				m_list_box.AddString(m_aEngineCrewInfo[i].jclx);
			}
			ShowEngineCrewInfo(0);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrainEngineInfoDlg::OnLbnSelchangeListEngineCrew()
{
	int nSel=m_list_box.GetCurSel();
	if(nSel!=-1)
	{
		ShowEngineCrewInfo(nSel);
	}
}

void CTrainEngineInfoDlg::ShowEngineCrewInfo(int i)
{
	if(i<0 || i>=m_nEngineCrewInfoCount)
		return;
	
	for(int row=0; row<ITEM_NUM; row++)
	{
		CString str=m_aEngineCrewInfo[i].info[row];
		m_engine_list.SetItemText(row, 1, str);
	}
	return;
}

extern int GSQueryData(const std::string& szPara, std::string& szResult);
bool CTrainEngineInfoDlg::RequestEngineInfo()
{
	bool rc = false;

	m_nEngineCrewInfoCount=0;
	for(int k=0; k<32; k++)
	{
		m_aEngineCrewInfo[k].jclx="";
		for(int n=0; n<ITEM_NUM; n++)
			m_aEngineCrewInfo[k].info[n]="";
	}
	
	//	<OPER_PARA oper_type=0 entity=x>
	//		<QUERY_JCCW CONSOLE_ID="5" TRAIN_INDEX="36444" />
	//	</OPER_PARA>

	CString xml, tmp;
	
	tmp.Format("<OPER_PARA oper_type=\"0\" entity=\"%d\">", mQueryParam.entity);
	xml += tmp;

	tmp.Format("<QUERY_JCCW console_id=\"%d\" train_index=\"%d\" time_begin=\"%d\" time_end=\"%d\" />", 
						mQueryParam.section, mQueryParam.train_index, mQueryParam.shift_start_time-48*3600, mQueryParam.shift_start_time+48*3600);
	xml += tmp;

	tmp="</OPER_PARA>";
	xml += tmp;

	std::string data;
	do 
	{
		GpcLogTrainMessage(0, 0, "��ѯ������Ϣ:%s", xml);
		if (GSQueryData((LPSTR)(LPCTSTR)xml, data))
		{
			CString str;
			str.Format("��ѯ������ϢGSoap����:%s", data.c_str());
			GpcLogTrainMessage(0, 0, str);
			AfxMessageBox(str);
			break;
		}
		
		//{
		//	CString str="��ѯ������ϢGSoap, �����ڴ�ʧ�� ����";
		//	GpcLogTrainMessage(0, 0, str);
		//	AfxMessageBox(str);
		//	break;
		//}

		MSXML::IXMLDOMDocumentPtr pXMLDoc;
		HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
		if(FAILED(hr))
		{
			_com_error er(hr);
			AfxMessageBox(er.ErrorMessage());
			break ;
		}

		if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( data.c_str() )))
		{
			CString str;
			str.Format("%s",data.c_str());
			GpcLogTrainMessage(0, 0, str);
			break ;
		}

		// QUERY_ACK
		MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "QUERY_ACK");
		if(pRoot == NULL)
			break ;

		// JCCW
		MSXML::IXMLDOMNodeListPtr pJCCWList = pRoot->selectNodes( "JCCW" );
		if(pJCCWList==NULL)
			break;				
		
		for(int n = 0; n < pJCCWList->Getlength() && n<32; n++)
		{
			MSXML::IXMLDOMNodePtr pJCCWNode = pJCCWList->Getitem(n);
			if(pJCCWNode==NULL)
				continue;
			//{"����","���ص绰","������", "��������", "����", "���ڵص�", "����ʱ��", "Ԥ�Ƴ���ʱ��"};
			CString str;

			// ����˳��
			short jcsx=0;
			GET_NODE_INTVALUE(jcsx, pJCCWNode, "@jcsx");
			if(jcsx==1)
				m_aEngineCrewInfo[n].jclx="����";
			else
				m_aEngineCrewInfo[n].jclx.Format("����%d", jcsx-1);

			// ����
			GET_NODE_STRVALUE(str, pJCCWNode, "@locomotive");
			GET_NODE_STRVALUE(tmp, pJCCWNode, "@psj");
			if (tmp=="")	tmp="X";
			str+="("+tmp+"��";
			GET_NODE_STRVALUE(tmp, pJCCWNode, "@psd");
			if (tmp=="")	tmp="X";
			str+=tmp+"��)";
			m_aEngineCrewInfo[n].info[0]=str;

			// ���ص绰
			GET_NODE_STRVALUE(str, pJCCWNode, "@gsmr");
			m_aEngineCrewInfo[n].info[1]=str;

			// ���񵣵���
			m_aEngineCrewInfo[n].info[2]="";
			
			// ��������
			GET_NODE_STRVALUE(str, pJCCWNode, "@jblx");
			if(str=="0")
				m_aEngineCrewInfo[n].info[3]="δ֪";
			else if(str=="1")
				m_aEngineCrewInfo[n].info[3]="��׼��";
			else if(str=="2")
				m_aEngineCrewInfo[n].info[3]="��˾��";
			else if(str=="3")
				m_aEngineCrewInfo[n].info[3]="˫�൥˾��";
			else
				m_aEngineCrewInfo[n].info[3]=str;

			// ����
			m_aEngineCrewInfo[n].info[4]="";

			// ���ڵص�
			GET_NODE_STRVALUE(str, pJCCWNode, "@cqdd");
			m_aEngineCrewInfo[n].info[5]=str;

			// ����ʱ��
			GET_NODE_STRVALUE(str, pJCCWNode, "@cqsj");
			m_aEngineCrewInfo[n].info[6]=str;

			// Ԥ�Ƴ���ʱ��
			GET_NODE_STRVALUE(str, pJCCWNode, "@yjclsj");
			m_aEngineCrewInfo[n].info[7]=str;

			str="";
			CString str2;
			MSXML::IXMLDOMNodeListPtr pCWList = pJCCWNode->selectNodes( "CW" );
			if(pCWList!=NULL)
			{
				for(int m = 0; m < pCWList->Getlength() ; m++)
				{
					MSXML::IXMLDOMNodePtr pCWNode = pCWList->Getitem(m);
					if(pCWNode!=NULL)
					{
						CString tmp;
						GET_NODE_STRVALUE(tmp, pCWNode, "@cwym");
						str+=tmp;
						
						str+="(";
						GET_NODE_STRVALUE(tmp, pCWNode, "@sjhm");
						str+=tmp;
						str+=") ";
					}
				// ���񵣵���
					if (0==m)
					{
						CString tmp1, tmp2;
						GET_NODE_STRVALUE(tmp1, pCWNode, "@psj");
						if (tmp1=="")	tmp1="X";
						GET_NODE_STRVALUE(tmp2, pCWNode, "@psd");
						if (tmp2=="")	tmp2="X";
						m_aEngineCrewInfo[n].info[2]=tmp1+"��"+tmp2+"��";
					}
				}
			}

			// ����
			m_aEngineCrewInfo[n].info[4]=str;
		
			m_nEngineCrewInfoCount++;
		}

		rc = true;
	} while (0);
	
	if(!rc)
	{
		AfxMessageBox("����������ѯ���ʧ��!");
	}

	//delete[] data;
	return rc;
}

void CTrainEngineInfoDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DestroyWindow();
}

void CTrainEngineInfoDlg::PostNcDestroy() 
{
	delete this;
}

