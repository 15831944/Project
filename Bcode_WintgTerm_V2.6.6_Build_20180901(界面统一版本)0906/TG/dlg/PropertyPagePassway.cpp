// PropertyPagePassway.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "PropertyPagePassway.h"
#include "DlgPasswayInsert.h"
#include "DlgMessageBox.h"
#include "widgets.h"                 //UM �����Ϣ����
#include "Tooltipwnd.h"              //��ʾ��
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
byte         ddtid;               //����̨id ����num
byte          ddtid_default;       //ȱʡ�ĵ���̨

#define ITEMNUM  7

PASS_WAY_UTILITY    utility[100];      //��·��������
PASS_WAY_INFO       info[100];         //��·����
PASS_WAY_STA        sta[100];          //��·��վ����
int ActivePage;   //��ǰ�Ļҳ
int utility_count;   //��·��Χ����
int passway_count;   //��·��Ŀ
int station_count;   //��վ��Ŀ

short analysis_train(short len, const char*point, short prefix_len, char* prefix, ulong *number);

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayAppoint property page

IMPLEMENT_DYNCREATE(CPropertyPagePasswayAppoint, CPropertyPage)

CPropertyPagePasswayAppoint::CPropertyPagePasswayAppoint() : CPropertyPage(CPropertyPagePasswayAppoint::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPagePasswayAppoint)
    m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 7;
	m_nRows = 1;
    m_pGridCtrl = NULL;
	col1=col2=col3=col4=col5=col6=	FALSE;
	Flag=FALSE;
	Flag_moredata=FALSE;
    ndialogInited=0;
	//}}AFX_DATA_INIT
}

CPropertyPagePasswayAppoint::~CPropertyPagePasswayAppoint()
{
	if(m_pGridCtrl)
	    delete m_pGridCtrl;
}

void CPropertyPagePasswayAppoint::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPagePasswayAppoint)
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_SAVE, m_saveCtrl);
	DDX_Control(pDX, IDC_NEW, m_newCtrl);
	DDX_Control(pDX, IDC_DELETE, m_deleteCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPagePasswayAppoint, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPagePasswayAppoint)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID, OnGridItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit) //LVN_ITEMCHANGING
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
    ON_MESSAGE(UM_DDT_CHANGED_MSG,OnDdtChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayAppoint message handlers
/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement property page

IMPLEMENT_DYNCREATE(CPropertyPagePasswayManagement, CPropertyPage)

CPropertyPagePasswayManagement::CPropertyPagePasswayManagement() : CPropertyPage(CPropertyPagePasswayManagement::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPagePasswayManagement)
		// NOTE: the ClassWizard will add member initialization here
    m_nFixCols_1 = 1;
	m_nFixRows_1 = 1;
	m_nCols_1 = 3;
	m_nRows_1 = 1;
    m_pGridCtrl_1 = NULL;
 
    m_nFixCols_2 = 1;
	m_nFixRows_2 = 1;
	m_nCols_2 = 6;
	m_nRows_2 = 2;
    m_pGridCtrl_2 = NULL;
    s_col2=s_col3=s_col4=s_col5=FALSE;
    Flag_sta=FALSE;
	Flag_info=FALSE;
    p_col1=p_col2=FALSE;
	Flag_moredata=TRUE;
	//}}AFX_DATA_INIT
}

CPropertyPagePasswayManagement::~CPropertyPagePasswayManagement()
{
	if(m_pGridCtrl_1)
	    delete m_pGridCtrl_1;
    if(m_pGridCtrl_2)
	    delete m_pGridCtrl_2;
}

void CPropertyPagePasswayManagement::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPagePasswayManagement)
	DDX_Control(pDX, IDC_STATIC2, m_Frame2);
	DDX_Control(pDX, IDC_STATIC1, m_Frame1);
	DDX_Control(pDX, IDC_STATION_SAVE, m_s_saveCtrl);
	DDX_Control(pDX, IDC_STATION_NEW, m_s_newCtrl);
	DDX_Control(pDX, IDC_STATION_DELETE, m_s_deleteCtrl);
	DDX_Control(pDX, IDC_SAVE, m_saveCtrl);
	DDX_Control(pDX, IDC_DELETE, m_deleteCtrl);
	DDX_Control(pDX, IDC_NEW, m_newCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPagePasswayManagement, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPagePasswayManagement)
	ON_BN_CLICKED(IDC_NEW, OnPassWayNew)
	ON_BN_CLICKED(IDC_DELETE, OnPassWayDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID, OnGridItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit) //LVN_ITEMCHANGING
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GRID2, OnGrid2ItemChanged)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID2, OnGrid2StartEdit)//GVN_BEGINLABELEDIT
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID2, OnGrid2EndEdit) //LVN_ITEMCHANGING
	ON_BN_CLICKED(IDC_SAVE, OnPassWaySave)
	ON_BN_CLICKED(IDC_STATION_DELETE, OnStationDelete)
	ON_BN_CLICKED(IDC_STATION_NEW, OnStationNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPagePasswayManagement message handlers
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPassway

IMPLEMENT_DYNAMIC(CPropertySheetPassway, CPropertySheet)

CPropertySheetPassway::CPropertySheetPassway(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage,CWintgDoc *pDoc)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
 	AddPage(&Page2);
	AddPage(&Page1);
    m_pDoc=pDoc;
}

CPropertySheetPassway::CPropertySheetPassway(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,CWintgDoc *pDoc)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSP_USEHICON;
  	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
  	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
 	AddPage(&Page2);
	AddPage(&Page1);
    m_pDoc=pDoc;
}

CPropertySheetPassway::~CPropertySheetPassway()
{
	if(Page1)
	delete Page1;
	if(Page2)
	delete Page2;
    	
}


BEGIN_MESSAGE_MAP(CPropertySheetPassway, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertySheetPassway)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_MESSAGE(UM_PASSWAY_UTILITY,OnPassWayUtility)
    ON_MESSAGE(UM_PASSWAY_INFO,OnPassWayInfo)
	ON_MESSAGE(UM_PASSWAY_STATION,OnPassWayStation)
	ON_CBN_SELENDOK(IDC_COMBO_CONSOLEID, OnSelendokComboDdtname)////////////////////
		//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPassway message handlers

void CPropertyPagePasswayAppoint::OnNew() 
{  
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
       return;
   if(Flag)    
   {   Flag=FALSE;
	   InitCell(utility_count+1);  //��ʼ��cell  ��ʾ���
    
       col1 = col2 = col3 = col4 = col5 = col6=FALSE;
       return;
   }
   if( col1 & col2 & col3 & col4 & col5 & col6)
   {
     
	   InitCell(utility_count+1);  //��ʼ��cell  ��ʾ���
       col1 = col2 = col3 = col4 = col5 = col6=FALSE;
   }
  

}

void CPropertyPagePasswayAppoint::OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
                         
}

void CPropertyPagePasswayAppoint::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{   
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    CGridCellCombo *pCell;

  	options4.RemoveAll();
    for(int i=0;i<passway_count;i++)                //passwayname �б�
    options4.Add(_T(info[i].Pass_way_name));

	CString content1;
    content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //�����������
    
	switch(pItem->iColumn)
	{
	case 1:
		break;
	case 2:
		  
		break;
	case 3:
	     m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,3);
		  pCell->SetOptions(options3);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
  
			break;
    case 4:
	     m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,4);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;

	case 5:
	     m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,5);
		  pCell->SetOptions(options5);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	case 6:
	     m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,6);
		  pCell->SetOptions(options6);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	}
 

}

void CPropertyPagePasswayAppoint::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;   //��õ�ǰ���к������Ľṹ
  
	CString content,content1;
	char  point[30];byte result;
    int length;

    content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //�����������
    if(atoi(content1)<=0) 
	{ *pResult = (1)? -1 : 0;
	  return;
	}

    content=m_pGridCtrl->GetItemText(pItem->iRow,pItem->iColumn);   //�����������
	strcpy(point,content);	
	length=strlen(point);
 
	switch(pItem->iColumn)
	{
	case 1:   
		if (FALSE == analysis_train(length, point, 
			sizeof(utility[pItem->iRow-1].PREFIX), utility[pItem->iRow-1].PREFIX, 
			&(utility[pItem->iRow -1].START_TRAIN_NUM)))
		{
			break;
		}
	     
		col1=TRUE;
		break;

	case 2:  		
		content1=m_pGridCtrl->GetItemText(pItem->iRow,1);   //�����������
		if(content1=="")
		{ 	 *pResult = (1)? -1 : 0;   
		break;
		}
		{
			char tmp[128];

			if (FALSE == analysis_train(length, point, 
				sizeof(utility[pItem->iRow-1].PREFIX), tmp, 
				&(utility[pItem->iRow -1].END_TRAIN_NUM)))
			{
				break;
			}
			if (strcmp(utility[pItem->iRow-1].PREFIX, tmp))
			{
				AfxMessageBox("���κ�ǰ׺Ӧ��ͬ");
				break;
			}
		}
		col2=TRUE;
		break;

	case 3:   content1=m_pGridCtrl->GetItemText(pItem->iRow,2);   //�����������
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
	 
		result=(content=="����" ? 0: 1);
        utility[pItem->iRow-1].DIRECTIONS=result;  //��������
	    m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
		col3=TRUE;
		break;
    case 4:  content1=m_pGridCtrl->GetItemText(pItem->iRow,3);   //�����������
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
        utility[pItem->iRow -1].PASS_WAY_ID=Get_Passwayid(point) ;  //��������
	    m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 
		col4=TRUE;
		break;
	case 5:  content1=m_pGridCtrl->GetItemText(pItem->iRow,4);   //�����������
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
	    result=(content=="ʼ��" ? 1: 2);
		//content.Format("%d",result);AfxMessageBox(content);
        utility[pItem->iRow-1].ACPT_FLAG=result;  //��������
		 m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell)); 
        col5=TRUE;
		break;
	case 6: content1=m_pGridCtrl->GetItemText(pItem->iRow,5);   //�����������
		     if(content1=="")
			 { 	 *pResult = (1)? -1 : 0;   
			     break;
			 }
        result=(content=="�յ�" ? 3: 4);
        utility[pItem->iRow-1].GVT_FLAG=result;  //��������
	  	m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCell)); 
        col6=TRUE;
		break;
	default:break;
	}
    //m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0x00));    
	if(pItem->iRow >utility_count+1)  
	 { 
		 *pResult = (1)? -1 : 0;                       //��ֹ�༭��Ч 
         m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xFF));
	     return;
	 }
	if( col1 & col2 & col3 & col4 & col5 & col6)
	  utility_count+=1;
 
}
//1 ��·��Ϣ�б�
LRESULT CPropertySheetPassway::OnPassWayInfo(WPARAM wParam,LPARAM lParam)
{ 
 
	if(!Page2.Flag_moredata)
		return  TRUE;                  //û�ж������ݷ���
    char *ptr=(char *)lParam;
	byte more_data,ddt_id_t;
	ushort count;              
    CString tempstr,str1;
//    char passway_name[20];
 
    more_data =*ptr;  //more data?
	Page2.Flag_moredata=more_data ? 1:0;
	ptr+=1;     

    ddt_id_t=*ptr;    //ddt_id  
     
	ptr+=1;
    Page2.Flag_info =TRUE;

    count=NettoHostUShort(*(USHORT *)ptr);          //count
	if(count<=0) return TRUE;
	passway_count+=count;
 
	ptr+=2;
    
	for(int i=passway_count-count;i<passway_count;i++)
	{
		info[i].Pass_way_id=NettoHostUShort(*(USHORT *)(ptr));            //passway_id
		ptr+=2;
		
		strncpy(info[i].Pass_way_name, ptr, sizeof(info[i].Pass_way_name)-1);  
		ptr+=40;
		
		info[i].Begin_sta=NettoHostUShort(*(USHORT *)(ptr));            //start starion //
	 
		ptr+=2;
		
		info[i].End_sta=NettoHostUShort(*(USHORT *)(ptr));            //end station   //
	 
		ptr+=2;
	}

 
    Page2.Show_Passway_Info();
 
	return TRUE;

}
 
//2 ������·��ϸ
LRESULT CPropertySheetPassway::OnPassWayStation(WPARAM wParam,LPARAM lParam)
{ 
    char *ptr=(char *)lParam;
	byte ddt_id_t;
	ushort count;              
    CString tempstr,str1;
     
    ddt_id_t= *ptr;                                  //ddt_id  
    if(ddt_id_t!=ddtid) return  TRUE;
	ptr+=1;
    Page2.Flag_sta=TRUE;                     

    int Pass_way_id=NettoHostUShort(*(USHORT *)(ptr));            //passway_id
	ptr+=2;

    count=NettoHostUShort(*(USHORT *)ptr);                        //count
	if(count<=0) return TRUE;
	ptr+=2;
    station_count=count;     

	for(int i=0;i<count;i++)
	{
		sta[i].Pass_way_id=Pass_way_id;                                  //passway_id

		sta[i].Sta_lst_idx=NettoHostUShort(*(USHORT *)(ptr));            //sta_idx  
		ptr+=2;
		
        sta[i].Station=NettoHostUShort(*(USHORT *)(ptr));                //sta_no 
		ptr+=2;

		sta[i].Entry= *ptr;                                             //entry / 
		ptr+=1;
		
        sta[i].Exit=*ptr;            //exit 
		ptr+=1;
	}

     
    Page2.Show_StationInfo();
    
    return TRUE;
}

//3 ָ����·��Ϣ
LRESULT CPropertySheetPassway::OnPassWayUtility(WPARAM wParam,LPARAM lParam)
{   
	if(!Page1.Flag_moredata )
		return  TRUE;

    char *ptr=(char *)lParam;
	byte more_data,ddt_id_t;
	ushort count;              
 
    more_data =*ptr;  //more data?
	Page1.Flag_moredata=more_data ? 1:0;
	ptr+=1;     

    ddt_id_t=*ptr;                                 //ddt_id *********  
	ptr+=1;
    
	Page1.Flag =TRUE;

    count=NettoHostUShort(*(USHORT *)ptr);          //count
	if(count<=0) return TRUE;

	utility_count+=count;
	ptr+=2;
    
	for(int i=utility_count-count;i<utility_count;i++)
	{
		strncpy(utility[i].PREFIX, ptr, 9);   
		ptr+=10;
		
		utility[i].START_TRAIN_NUM=NettoHostULong(*(unsigned long  *)(ptr));  //start starion //
		ptr+=4;

	    utility[i].END_TRAIN_NUM=NettoHostULong(*(unsigned long  *)(ptr));  //end  starion //
		ptr+=4;
		
		utility[i].Station=NettoHostUShort(*(USHORT *)(ptr));
		ptr+=2;

		utility[i].Entry=*ptr;    ptr++;
		utility[i].Exit=*ptr;     ptr++;

		utility[i].DIRECTIONS=*ptr;                                               //direction   //
		ptr+=1;
        
	    utility[i].PASS_WAY_ID=NettoHostUShort(*(USHORT *)(ptr));                 //passway_id  //
		ptr+=2;

	    utility[i].ACPT_FLAG=*ptr;                                                 //acpt_flag  //
		ptr+=1;

        utility[i].GVT_FLAG=*ptr;                                                  //gvt_flag  //
		ptr+=1;

	}

    Page1.m_pGridCtrl->SetRedraw (FALSE);   
    for(int j=0;j<utility_count;j++)
      Page1.Show_Utility(j,j+1);
    Page1.m_pGridCtrl->SetRedraw (TRUE);   
    Page1.m_pGridCtrl->Invalidate();   
    
	return TRUE;
}
void CPropertySheetPassway::OnSelendokComboDdtname() 
{ 
	CString sName;                   //��������
    int num=m_Combo1.GetCurSel();
    int n = m_Combo1.GetLBTextLen(num);
    m_Combo1.GetLBText( num, sName.GetBuffer(n) );

    char *  ddt[20];         
    int nSectionNum,nSectionList[30];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]);
	gpcdef.GetSectionList(nSectionNum,nSectionList);
	for (int i = 0; i < nSectionNum; i++)   
	{ 
	    ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
	    if(ddt[i]==sName)
	      ddtid=nSectionList[i];  //��õ�ǰѡ���ĵ���̨
	}
 
 
	Page2.LoadPassWayInfo();             //

	if(ActivePage==1){
		Page1.LoadUtility(ddtid);
	}

	if(ddtid_default!=ddtid)
	{  
		if(ActivePage==1)
		{   
			Page1.m_newCtrl.EnableWindow(FALSE);
			Page1.m_deleteCtrl.EnableWindow(FALSE);
			Page1.m_saveCtrl.EnableWindow(FALSE);
		}
		Page2.m_newCtrl.EnableWindow(FALSE);
		Page2.m_deleteCtrl.EnableWindow(FALSE);
		Page2.m_saveCtrl.EnableWindow(FALSE);
		Page2.m_s_newCtrl.EnableWindow(FALSE);
		Page2.m_s_deleteCtrl.EnableWindow(FALSE);
		Page2.m_s_saveCtrl.EnableWindow(FALSE);
	}
	else
	{   
		if(ActivePage==1)
		{
			Page1.m_newCtrl.EnableWindow(TRUE);
			Page1.m_deleteCtrl.EnableWindow(TRUE);
			Page1.m_saveCtrl.EnableWindow(TRUE);
		}
		Page2.m_newCtrl.EnableWindow(TRUE);
		Page2.m_deleteCtrl.EnableWindow(TRUE);
		Page2.m_saveCtrl.EnableWindow(TRUE);
		Page2.m_s_newCtrl.EnableWindow(TRUE);
		Page2.m_s_deleteCtrl.EnableWindow(TRUE);
		Page2.m_s_saveCtrl.EnableWindow(TRUE);
		
	}

	Page2.OnChangeSection(ddtid);
    
	if(Page1.ndialogInited == 1)
	    Page1.SendMessage(UM_DDT_CHANGED_MSG);

}
BOOL CPropertySheetPassway::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
 
 	CRect rectSheet;
	GetWindowRect(rectSheet);
	rectSheet.right+=130;
	rectSheet.bottom+=-30;
    MoveWindow(rectSheet);
   
    CRect TempRect(0,0,100,100);
	m_Combo1.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_SORT | 
		WS_BORDER | WS_VSCROLL,TempRect,this,IDC_COMBO_CONSOLEID );
     
    ddtid=m_pDoc->m_nCurrentSectionNo;  
    ddtid_default=ddtid;

    char *  ddt[20];    //����̨����     
    int nSectionNum,nSectionList[30];
	nSectionNum = sizeof(nSectionList) / sizeof(nSectionList[0]); //���ε���Ŀ
	gpcdef.GetSectionList(nSectionNum,nSectionList);	//�����б�
	for (int i = 0; i < nSectionNum; i++)//������ʾ������
	{
	     ddt[i] = gpcdef.GetSectionName(nSectionList[i]);
		 m_Combo1.InsertString (i,_T(ddt[i]) ); //���� 
		 if(ddtid_default==nSectionList[i])
	     m_Combo1.SetCurSel(i);//��ʾĬ������
	}	
     

    m_Static1.Create("   ��ѡ���е�̨ ",WS_CHILD | WS_VISIBLE | SS_LEFT,
		 TempRect,this,IDC_STATIC_CONSOLEID);
    CWnd * pStatic=GetDlgItem(IDC_STATIC_CONSOLEID);
	CRect rectStatic;
	pStatic->GetWindowRect(rectStatic);
      
    CPropertyPage * pPage=GetPage(0);
	CRect rectPage;
	pPage->GetWindowRect(rectPage);
    //�е�̨1
	rectStatic.top=rectPage.top+60;
	rectStatic.bottom=rectStatic.top+20;
	rectStatic.left=rectPage.right+10;
	rectStatic.right=rectStatic.left+130;
	ScreenToClient(rectStatic);
	pStatic->MoveWindow(rectStatic);
    
	//combox2
    CWnd * pCombo=GetDlgItem(IDC_COMBO_CONSOLEID); //IDC_COMBO_CONSOLEID
	if(pCombo==NULL)  
		return FALSE;
	CRect rectCombo;
	pCombo->GetWindowRect(rectCombo);
      
	rectCombo.top=rectPage.top+90;
	rectCombo.bottom=rectCombo.top+60;
	rectCombo.left=rectPage.right+20;
	rectCombo.right=rectCombo.left+100;
	ScreenToClient(rectCombo);
	pCombo->MoveWindow(rectCombo);
    
	//���ذ�ť3
    CWnd *pbtnOk=GetDlgItem(IDOK);
	ASSERT(pbtnOk);
	CRect rectOkBtn;
	pbtnOk->GetWindowRect(rectOkBtn);

	int cx=rectOkBtn.Width();
	int cy=rectOkBtn.Height();
	rectOkBtn.top=rectPage.top+350;  //rectCombo.bottom
	rectOkBtn.bottom=rectOkBtn.top+cy+10;
	rectOkBtn.left=rectPage.right+30;
	rectOkBtn.right=rectOkBtn.left+cx+10;
	ScreenToClient(rectOkBtn);
	pbtnOk->MoveWindow(rectOkBtn);

	int ids[]={ID_APPLY_NOW, IDCANCEL, IDOK, IDHELP};
	for( i=0;i<sizeof ids / sizeof ids[0];i++)
	{
      CWnd * pWnd=GetDlgItem(ids[i]);
	  if(pWnd==NULL)
        break;
      switch(i)
	  {
	  case 0:
		     //pWnd->SetWindowText(_T("����"));
			 //pWnd->EnableWindow(TRUE);
             pWnd->ShowWindow(FALSE);
			 break;
	  case 1:
		     //pWnd->SetWindowText(_T("ȡ��"));
             pWnd->ShowWindow(FALSE);
			 break;

	  case 2:
             pWnd->SetWindowText(_T("����"));
			 break;

	  case 3:
		     pWnd->ShowWindow(FALSE);
		     break;
	  default:break;
	  }// switch
	}// for
    
	InitDlgPosition(this);
     
	Page2.LoadPassWayInfo();   //����̨���еľ�·
	return bResult;
}

BOOL CPropertyPagePasswayAppoint::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);
	}

	options3.Add(_T("����"));
    options3.Add(_T("����"));
    options5.Add(_T("����"));
    options5.Add(_T("ʼ��"));
    options6.Add(_T("�յ�"));
    options6.Add(_T("����"));

	TCHAR title[ITEMNUM][18]={_T("���"), _T("��ʼ����"),_T("��������"),
		_T("����"),_T(" ��· "),_T("����/ʼ��"), _T("�յ�/����")};
    
   if(m_pGridCtrl!=NULL)
	return FALSE;
	m_pGridCtrl=new CGridCtrl;
	if(!m_pGridCtrl) return FALSE;
	CStatic * pWnd_S;
	pWnd_S=(CStatic *)GetDlgItem(IDC_GRID);


	//CRect rect(20,70,670,370);
	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl->Create (rc,this,IDC_GRID);
	//m_pGridCtrl->SetEditable(FALSE);            
	//m_pGridCtrl->EnableDragAndDrop (TRUE);       

	try{
		m_pGridCtrl->SetRowCount(m_nRows);                //��
		m_pGridCtrl->SetColumnCount(m_nCols);             //��
		m_pGridCtrl->SetFixedRowCount(m_nFixRows);        //�̶���
		m_pGridCtrl->SetFixedColumnCount(m_nFixCols);     //�̶���
		    
		m_pGridCtrl->SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		//m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));   //����ɫ
		m_pGridCtrl->SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
	    m_pGridCtrl->SetFixedBkColor(RGB(255,242,240));
         //RGB(166,202,240
        m_pGridCtrl->SetColumnWidth(0, 60);
        m_pGridCtrl->SetColumnWidth(1, 90);
        m_pGridCtrl->SetColumnWidth(2, 90);
        m_pGridCtrl->SetColumnWidth(3, 70);
        m_pGridCtrl->SetColumnWidth(4, 180);
		m_pGridCtrl->SetColumnWidth(5, 80);
        m_pGridCtrl->SetColumnWidth(6, 80);
	 
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

  DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#ifndef _WIN32_WCE
    dwTextStyle |= DT_END_ELLIPSIS;
#endif
 
//  int row,col;
 
//	for ( row = 0; row < 5; row++)
	 for (int col = 0; col < ITEMNUM; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;  
 		Item.strText.Format(title[col]);
		m_pGridCtrl->SetItem(&Item);
	}
  
    InitCell(1);  //��ʼ��cellcombo
	utility_count=0;
 
	LoadUtility(ddtid);
    
	ndialogInited=1;

	return TRUE;  
}
void CPropertyPagePasswayManagement::OnGridItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
  	//int nRow = m_pGridCtrl_1->GetFocusCell().row;
  
	 /*
    if (nRow >= 1 && nRow<=passway_count)       
    {   current_passway=nRow-1;                //��ǰ��·�ţ�
	    LoadPassWayStation(info[nRow-1].Pass_way_id);
	    m_pGridCtrl_1->Invalidate();
    }	
	*/
}

void CPropertyPagePasswayManagement::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
    //m_pGridCtrl_1->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xE0));
 
}



void CPropertyPagePasswayManagement::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;      

}
void CPropertyPagePasswayManagement::OnGrid2ItemChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
 
}

void CPropertyPagePasswayManagement::OnGrid2StartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{    
   
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    CGridCellCombo *pCell;
    CStringArray options4;
	options4.Add(_T("1"));
    options4.Add(_T("2"));
	options4.Add(_T("3"));
  	options4.Add(_T("4"));

	CString content1;
    content1=m_pGridCtrl_2->GetItemText(pItem->iRow,0);   //�����������
    if(atoi(content1)<=0) 
	return;

	CStringArray options;
	for(int i=0;i<nStationNum;i++)
    options.Add( _T(gpcdef.GetStationName(nStationList[i])) );
    
	switch(pItem->iColumn)
	{
	case 1:
		break;
	case 2:
		  
		break;
	case 3:
	     m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
		  pCell->SetOptions(options);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
	case 4:
		  m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,4);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
		break;
	case 5:
		  m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
          pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,5);
		  pCell->SetOptions(options4);
          pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
   
		break;
	default:break;
		
	}

}


void CPropertyPagePasswayManagement::OnGrid2EndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
     
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;   
    
 
}
BOOL CPropertyPagePasswayManagement::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	 
	if(gpcdef.GetUserIdentify() != GPC_TEST)
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_DELETE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_STATION_NEW);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_STATION_DELETE);
		pWnd->EnableWindow(FALSE);
 
	}
 
	TCHAR title[3][40]={_T("���"),_T("��·��"), _T("��·����") };
	if(m_pGridCtrl_1!=NULL)
	return FALSE;
	m_pGridCtrl_1=new CGridCtrl;
	if(!m_pGridCtrl_1) return FALSE;
	CStatic * pWnd_S;
	pWnd_S=(CStatic *)GetDlgItem(IDC_GRID);
	//CRect rect(10,70,280,400);
	CRect rc;
	m_Frame1.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_1->Create (rc,this,IDC_GRID);
 
	try{
		m_pGridCtrl_1->SetRowCount(m_nRows_1);              
		m_pGridCtrl_1->SetColumnCount(m_nCols_1);           
		m_pGridCtrl_1->SetFixedRowCount(m_nFixRows_1);        
		m_pGridCtrl_1->SetFixedColumnCount(m_nFixCols_1);     
		    
		m_pGridCtrl_1->SetFixedTextColor(RGB(0,0,0));      //�ı���ɫ
		//m_pGridCtrl->SetFixedBkColor(RGB(192,192,192));   //����ɫ
		m_pGridCtrl_1->SetGridLineColor(RGB(166,202,240));       //�ߵ���ɫ
        //m_pGridCtrl_1->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
       	m_pGridCtrl_1->SetFixedBkColor(RGB(255,242,240));
        m_pGridCtrl_1->SetColumnWidth(0, 36);    // ����������
        m_pGridCtrl_1->SetColumnWidth(1, 46);
        m_pGridCtrl_1->SetColumnWidth(2, 150);
	
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

  DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#ifndef _WIN32_WCE
    dwTextStyle |= DT_END_ELLIPSIS;
#endif
 
  int col;
 
//	for ( row = 0; row < 5; row++)
	 for ( col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
 		Item.strText.Format(title[col]);
		m_pGridCtrl_1->SetItem(&Item);
	}
 

/********************��·����վ**************************/	 
  TCHAR title2[6][18]={_T("���"),_T("��·��"), _T("վ���"),_T("��վ"),
                       _T("���"), _T("����")};
	if(m_pGridCtrl_2!=NULL)
	return FALSE;
	m_pGridCtrl_2=new CGridCtrl;
	if(!m_pGridCtrl_2) return FALSE;
 
	//CRect rect2(300,70,690,340);
	//CRect rc;
	m_Frame2.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-10,-30,-10,-10);
	m_pGridCtrl_2->Create (rc,this,IDC_GRID2);
 
	try{
		m_pGridCtrl_2->SetRowCount(m_nRows_2);            
		m_pGridCtrl_2->SetColumnCount(m_nCols_2);            
		m_pGridCtrl_2->SetFixedRowCount(m_nFixRows_2);       
		m_pGridCtrl_2->SetFixedColumnCount(m_nFixCols_2);     
		m_pGridCtrl_2->SetFixedTextColor(RGB(0,0,0));      
		m_pGridCtrl_2->SetGridLineColor(RGB(166,202,240));       
	    m_pGridCtrl_2->SetFixedBkColor(RGB(255,242,240));
        m_pGridCtrl_2->SetColumnWidth(0, 36);
        m_pGridCtrl_2->SetColumnWidth(1, 50);
        m_pGridCtrl_2->SetColumnWidth(2, 50);
        m_pGridCtrl_2->SetColumnWidth(3, 56);
        m_pGridCtrl_2->SetColumnWidth(4, 40);
		m_pGridCtrl_2->SetColumnWidth(5, 40);
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}


//	for ( row = 0; row < 5; row++)
	 for ( col = 0; col < 6; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = col;
  		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
 		Item.strText.Format(title2[col]);
		m_pGridCtrl_2->SetItem(&Item);
	}

    m_pGridCtrl_2->AutoSizeColumns();
	m_pGridCtrl_2->Invalidate(FALSE);
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
    gpcdef.GetStationList(gpcdef.GetFirstSection(),nStationNum,nStationList);
   

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPropertyPagePasswayAppoint::LoadUtility(int consoleid)
{
	SetBlankCells(0);
	Flag_moredata=TRUE;
    utility_count=0;

	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 

	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_UTILITY); 
	ptr += 2;   
	
 	*ptr = ddtid;    //consoleid
	ptr += 1;   
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����·��������ʧ�ܣ�");
		return ;
    }
	utility_count=0;             //����0��
	return ;
}
 

void CPropertyPagePasswayManagement::LoadPassWayInfo()
{
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 
	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_INFO); 
	ptr += 2;   
	
 	*ptr = ddtid;  //Console id  (ddtid
	ptr += 1;  
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����·����ʧ�ܣ�");
		return ;
    }
    Flag_moredata=TRUE;                //���Խ���subao����
	passway_count=0;  
	return ;
}

void CPropertyPagePasswayManagement::LoadPassWayStation(short PassWayId)
{
 
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

 

	ptr += 2;           //for msglen

 	*(ushort *)ptr =  (LOAD_PASSWAY_STATION); 
	ptr += 2;   
	
 	*ptr = ddtid;  //Console id
	ptr += 1;  

	*(ushort *)ptr = NettoHostUShort(PassWayId);  //PassWayId
	ptr += 2;

 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����·����ʧ�ܣ�");
		return ;
    }
    
	return ;
}

BOOL CPropertyPagePasswayAppoint::PreTranslateMessage(MSG* pMsg) 
{
 
	 if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN
            || pMsg->wParam == VK_ESCAPE )
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;                    // DO NOT process further
        }
    }
    	  
	return CPropertyPage::PreTranslateMessage(pMsg);
 
}

BOOL CPropertyPagePasswayManagement::PreTranslateMessage(MSG* pMsg) 
{
 	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

LRESULT CPropertyPagePasswayAppoint::OnDdtChanged(WPARAM wParam, LPARAM lParam)
{
   LoadUtility(ddtid);
   return TRUE;

}
void CPropertyPagePasswayAppoint::OnDelete() 
{   
	if( !gpcdef.GetOperatorRight(WINTG_EDIT))
    return;

	ASSERT_VALID(m_pGridCtrl);
	int nRow = m_pGridCtrl->GetFocusCell().row;
    if (nRow >= 1 && nRow<=utility_count)
    {
		if(AfxMessageBox("Ҫɾ��������·?",MB_YESNO | MB_DEFBUTTON2 |MB_ICONQUESTION) != IDYES  )
			return; 
		m_pGridCtrl->DeleteRow(nRow);	
		m_pGridCtrl->Invalidate();
		
		Del_Utility(nRow-1);
   	    utility_count-=1;
		for(int i=nRow-1;i<utility_count;i++)
			utility[i]=utility[i+1];
		utility_count-=1;
		
	}

}

void CPropertyPagePasswayAppoint::OnSave() 
{
	if(!gpcdef.GetOperatorRight(WINTG_EDIT))
        return;
	Set_Utility();         //���þ�·��CAD
    
	LoadUtility(ddtid);    //��CAD����·
}

void CPropertyPagePasswayManagement::OnPassWayNew() 
{  
	CDlgPasswayInsert dlg;
	if(dlg.DoModal() == IDOK)
	{
		short passwayid;
		CString passwayname;

		passwayid = dlg.m_nPasswayId;
		passwayname = dlg.m_nPasswayName;

		passway_count ++;

		info[passway_count-1].Pass_way_id = passwayid;
	 
        strcpy(info[passway_count-1].Pass_way_name, passwayname);

		m_pGridCtrl_1->InsertRow("����");

		int row = passway_count;
		TCHAR buffer[10];
		for (int  col = 0; col < 3; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //��
			Item.col = col;  //��
			Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

			if(col==0)
				sprintf(buffer,"%d",row);
			else if(col==1)
			    sprintf(buffer,"%d",passwayid);
		    else if(col==2)
			   sprintf(buffer,"%s",passwayname );
	
			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl_1);
			m_pGridCtrl_1->SetItem(&Item);            
		}  
		m_pGridCtrl_1->Invalidate();
	}

	
 
}

//ɾ��ѡ���ľ�·
void CPropertyPagePasswayManagement::OnPassWayDelete() 
{
	int nRow = m_pGridCtrl_1->GetFocusCell().row;
    if (nRow >= 1 && nRow<=passway_count)
    {   
		if(AfxMessageBox("Ҫɾ���˾�·?",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES  )
			return; 

		Del_Passway(info[nRow-1].Pass_way_id);

		passway_count = 0;
		//���¼���
		LoadPassWayInfo();
	}

}

//��ʾ��·��Ϣ
void CPropertyPagePasswayManagement::Show_Passway_Info( )
{
	//m_pGridCtrl_1->SetRedraw (FALSE);   
 
    if(passway_count <0) return;
    m_nRows_1 = passway_count+1;
	m_pGridCtrl_1->SetRowCount(m_nRows_1);

    TCHAR buffer[60];
	for (int row = 1; row < m_nRows_1; row++)
	for (int  col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;   
		Item.col = col;   
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%d",info[row-1].Pass_way_id);
			break;
		case 2:
            sprintf(buffer,"%s",info[row-1].Pass_way_name);
			break;
		default:
			break;
		} 
		Item.strText.Format(buffer);
		m_pGridCtrl_1->SetItem(&Item);
	}   

    m_pGridCtrl_1->AutoSizeColumns();
    //m_pGridCtrl_1->SetRedraw (TRUE);    
    m_pGridCtrl_1->Invalidate(); 

}


int  CPropertyPagePasswayManagement::GetStationNoByName(CString sName)
{

	for(int i=0;i<nStationNum;i++)
    if(!strcmp(gpcdef.GetStationName(nStationList[i]),sName ))
		return nStationList[i];
	return 0;

}
BOOL  CPropertyPagePasswayManagement::GetStationInfoFromMMI( )
{

    BOOL bWrong = FALSE;
    int nRowCount;
	do{
         
		 nRowCount = m_pGridCtrl_2->GetRowCount();
		for (int row = 1; row < nRowCount; row++)
		{
			for (int  col = 0; col < 6; col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;  //��
				Item.col = col;  //��
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
				CString strText;
				strText = m_pGridCtrl_2->GetItemText(row,col);
				if( strText.IsEmpty())
				{
					bWrong = TRUE;
					break;
				}

				switch(col)
				{

				case 0:
					break;
				case 1:  //��·��
					sta[row-1].Pass_way_id = atoi(strText);
					break;
				case 2:  //վ���
					sta[row-1].Sta_lst_idx = atoi(strText) ;
					break;
				case 3: //��վ
					sta[row-1].Station = GetStationNoByName(strText) ;
					break;
				case 4://���
					sta[row-1].Entry = atoi(strText);
					break;
				case 5://����
					sta[row-1].Exit = atoi(strText) ;
					break;
				default:
					break;
				} 

			}   
			if(bWrong)
				break;
		}
		if(bWrong) 
			return FALSE;
	}while(0);


	station_count = nRowCount-1;
	return TRUE;

}
void CPropertyPagePasswayManagement::Show_StationInfo( )
{   
   
 
    if(station_count <0) return;
    m_nRows_2 = station_count+1;
	m_pGridCtrl_2->SetRowCount(m_nRows_2);


    TCHAR buffer[20];
	for (int row = 1; row < m_nRows_2; row++)
	for (int  col = 0; col < 6; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;  //��
		Item.col = col;  //��
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%d",sta[row-1].Pass_way_id);
			break;
		case 2:
            sprintf(buffer,"%d",sta[row-1].Sta_lst_idx);
			break;
   	    case 3:
            sprintf(buffer,"%s",gpcdef.GetStationName(sta[row-1].Station));
			break;
     	case 4:
            sprintf(buffer,"%d",sta[row-1].Entry);
			break;
      	case 5:
            sprintf(buffer,"%d",sta[row-1].Exit);
			break;
		default:
			break;
		}//switch
		Item.strText.Format(buffer);
		ASSERT(m_pGridCtrl_2);
		m_pGridCtrl_2->SetItem(&Item);
	}  //for
    m_pGridCtrl_2->AutoSizeColumns();
   
   m_pGridCtrl_2->Invalidate();  
}

void CPropertyPagePasswayAppoint::Show_Utility(int no, int row)
{   
 
	char buf_s[20],buf_e[20];
       _itoa(utility[no].START_TRAIN_NUM, buf_s, 10 );   //���֣����ַ�
        _itoa(utility[no].END_TRAIN_NUM, buf_e, 10 );
    TCHAR buffer[40];
 
	Flag=TRUE;
	for (int  col = 0; col < 7; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;  //��
		Item.col = col;  //��
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;
		switch(col){
		case 0:
			sprintf(buffer,"%d",row);
			break;
		case 1:
			sprintf(buffer,"%s",utility[no].PREFIX);
            strcat(buffer,buf_s);
			break;
		case 2:
            sprintf(buffer,"%s",utility[no].PREFIX);
            strcat(buffer,buf_e);
			break;
		case 3:
            sprintf(buffer,"%s",utility[no].DIRECTIONS ? "����" :"����");
			break;
		case 4:
             sprintf(buffer,"%s",Get_Passwayname(utility[no].PASS_WAY_ID));
			break;
		case 5:
			sprintf(buffer,"%s",utility[no].ACPT_FLAG==1 ? "ʼ��":"����");
			break;
        case 6:
			sprintf(buffer,"%s",utility[no].GVT_FLAG==3 ? "�յ�":"����");
			break;
		default:
			break;
		}//switch
		Item.strText.Format(buffer);
		ASSERT_VALID(m_pGridCtrl);
		m_pGridCtrl->SetItem(&Item);
 
	}  //for
	//m_pGridCtrl->AutoSizeColumns();
    InitCell(row);  //��ʼ��cell
}

BOOL CPropertyPagePasswayManagement::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

    int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID && idCtrl!=IDC_GRID2 ) 
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
	//��Ϣ��������

    NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;;
    int nMessageType=pItem->hdr.code;
    int nCol;
	if(idCtrl==IDC_GRID)
	{  
		if(nMessageType==LVN_ITEMCHANGED)   //����ѡ����Ϣ
		{
			int nRow = m_pGridCtrl_1->GetFocusCell().row;
			 nCol = m_pGridCtrl_1->GetFocusCell().col;

			
			if (nRow >= 1 && nRow<=passway_count )               //��ǰ�еķ�Χ������ķ�Χ��
			{   
				current_passway=nRow-1;                         //��ǰ��·�ţ�
				LoadPassWayStation(info[nRow-1].Pass_way_id);   //��������վ����Ϣ
				current_passway_id = info[nRow-1].Pass_way_id;
				m_pGridCtrl_1->SetSelectedRange(current_passway+1,0,current_passway+1,2, FALSE);
				m_pGridCtrl_1->Invalidate();

	            m_pGridCtrl_2->SetRowCount(1);
			}	
			
		}
		
		
		if(nMessageType==GVN_ENDLABELEDIT)   //�����༭��Ϣ
		{    
		
		}
	}
    //�ڶ���GRIDCTRL	
	else  if(idCtrl==IDC_GRID2)
	{
		if(pItem->iRow < 0) return FALSE ;

		CStringArray optionscf;
		for(int i=0;i<nStationNum;i++)
			optionscf.Add( _T(gpcdef.GetStationName(nStationList[i])) );
		CGridCellCombo *pCellcf;

		m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
		pCellcf= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
		pCellcf->SetOptions(optionscf);
		pCellcf->SetStyle(pCellcf->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);

		//��ʼ�༭
		if(nMessageType==GVN_BEGINLABELEDIT)
		{
			CGridCellCombo *pCell;
			CStringArray options4;
			CString myStr;
			for(int j=1;j<= 12;j++)
			{
				myStr.Format("%2d",j);
			    options4.InsertAt(j-1,myStr);
			}
			
			CStringArray options;
			for(int i=0;i<nStationNum;i++)
				options.Add( _T(gpcdef.GetStationName(nStationList[i])) );
			
			switch(pItem->iColumn)
			{
			case 1:
				break;
			case 2:			
				break;
			case 3:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,3);
				pCell->SetOptions(options);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			case 4:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,4);
				pCell->SetOptions(options4);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			case 5:
				m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
				pCell= (CGridCellCombo*) m_pGridCtrl_2->GetCell(pItem->iRow ,5);
				pCell->SetOptions(options4);
				pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
				break;
			default:break;

			}		
			
		}
		if(nMessageType==GVN_ENDLABELEDIT)
		{
			switch(pItem->iColumn)
			{
			case 1: //��·��
				break;
			case 2: //վ���
				break;
			case 3: //��վ		 
				m_pGridCtrl_2->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
				break;
			case 4 : //���
				m_pGridCtrl_2->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 	 
				break;
			case 5://����
				m_pGridCtrl_2->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell));

				break;

			default: break;	

			}
		}
	//if(idCtrl!=IDC_GRID)
	//{  
	//	int nRow = m_pGridCtrl_1->GetFocusCell().row;
  	//	current_passway = nRow -1;	
   // }
   }

   return CPropertyPage::OnNotify(wParam, lParam, pResult);

}

BOOL CPropertySheetPassway::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    ActivePage=GetActiveIndex();	
 
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

BOOL CPropertyPagePasswayAppoint::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{   	
	CString content1,content;

	if(ddtid_default!=ddtid)
	{	 
	    m_newCtrl.EnableWindow(FALSE);
        m_deleteCtrl.EnableWindow(FALSE);
        m_saveCtrl.EnableWindow(FALSE); 
    }
   else
   {    
	    m_newCtrl.EnableWindow(TRUE);
        m_deleteCtrl.EnableWindow(TRUE);
        m_saveCtrl.EnableWindow(TRUE);

   }	
 	//#define IDC_GRID                        11004 	
    int   idCtrl = (int) wParam;
	if(idCtrl!=IDC_GRID ) 
	      return CPropertyPage::OnNotify(wParam, lParam, pResult);
    NM_GRIDVIEW *pItem=(NM_GRIDVIEW*)  lParam;;

    int nMessageType=pItem->hdr.code;
//    int j;

    if(nMessageType==GVN_BEGINLABELEDIT)
	{  

		CGridCellCombo *pCell;	
		options4.RemoveAll();
		for(int i=0;i<passway_count;i++)                //passwayname �б�
			options4.Add(_T(info[i].Pass_way_name));		
		//CString content1;
		content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //�����������		
		switch(pItem->iColumn)
		{
		case 1:
			break;
		case 2:
			
			break;
		case 3:
			m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,3);
			pCell->SetOptions(options3);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			
			break;
		case 4:
			m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,4);
			pCell->SetOptions(options4);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
			
		case 5:
			m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,5);
			pCell->SetOptions(options5);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
		case 6:
			m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCellCombo));
			pCell= (CGridCellCombo*) m_pGridCtrl->GetCell(pItem->iRow ,6);
			pCell->SetOptions(options6);
			pCell->SetStyle(pCell->GetStyle() | CBS_DROPDOWNLIST | CBS_SORT);
			break;
		}	
	}
	if(nMessageType==GVN_ENDLABELEDIT)
	{	
		
		CString content,content1;
		char  point[30];byte result;
		int length;
		
		content1=m_pGridCtrl->GetItemText(pItem->iRow,0);   //��ø���������������
		if(atoi(content1)<=0) 
		{ 
			*pResult = (1)? -1 : 0;
		    return FALSE;
		}
		
		content=m_pGridCtrl->GetItemText(pItem->iRow,pItem->iColumn);   //�����������
		strcpy(point,content);	
		length=strlen(point);
    
		//�ж����������Ƿ���ȷ
		switch(pItem->iColumn)
		{
		case 1:   //��ʼ����
			if (FALSE == analysis_train(length, point, 
				sizeof(utility[pItem->iRow-1].PREFIX), utility[pItem->iRow-1].PREFIX, 
				&(utility[pItem->iRow -1].START_TRAIN_NUM)))
			{
				break;
			}
           
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("�����ظ��ļ�¼!");
				break;
			}
			col1=TRUE;
			break;

		case 2:              
			content1=m_pGridCtrl->GetItemText(pItem->iRow,1);   //��õ�һ������
			if(content1=="")
			{ 
				 *pResult = (1)? -1 : 0;   
			     break;
			}

			{
				char tmp[128];

				if (FALSE == analysis_train(length, point, 
					sizeof(utility[pItem->iRow-1].PREFIX), tmp, 
					&(utility[pItem->iRow -1].END_TRAIN_NUM)))
				{
					break;
				}
				if (strcmp(utility[pItem->iRow-1].PREFIX, tmp))
				{
					AfxMessageBox("���κ�ǰ׺Ӧ��ͬ");
					break;
				}
			}

          
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("�����ظ��ļ�¼!");
				break;
			}
			col2=TRUE;
			break;

		case 3:  
			content1=m_pGridCtrl->GetItemText(pItem->iRow,2);   //�����������
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			
			result=(content=="����" ? 0: 1);
			utility[pItem->iRow-1].DIRECTIONS=result;  //��������
			m_pGridCtrl->SetCellType(pItem->iRow ,3, RUNTIME_CLASS(CGridCell)); 
			
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("�����ظ��ļ�¼!");
				break;
			}
			col3=TRUE;
			break;
		case 4:  
			content1=m_pGridCtrl->GetItemText(pItem->iRow,3);   //�����������
			if(content1=="")
			{ 	
				*pResult = (1)? -1 : 0;   
			    break;
			}
			utility[pItem->iRow -1].PASS_WAY_ID=Get_Passwayid(point) ;  //��������
			m_pGridCtrl->SetCellType(pItem->iRow ,4, RUNTIME_CLASS(CGridCell)); 
			
            if(CheckSameUtility(pItem->iRow-1))
			{
				AfxMessageBox("�����ظ��ļ�¼!");
				break;
			}
			col4=TRUE;
			break;
		case 5:  content1=m_pGridCtrl->GetItemText(pItem->iRow,4);   //�����������
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			result=(content=="ʼ��" ? 1: 2);
			//content.Format("%d",result);AfxMessageBox(content);
			utility[pItem->iRow-1].ACPT_FLAG=result;  //��������
			m_pGridCtrl->SetCellType(pItem->iRow ,5, RUNTIME_CLASS(CGridCell)); 
			col5=TRUE;
			break;
		case 6: content1=m_pGridCtrl->GetItemText(pItem->iRow,5);   //�����������
			if(content1=="")
			{ 	 *pResult = (1)? -1 : 0;   
			break;
			}
			result=(content=="�յ�" ? 3: 4);
			utility[pItem->iRow-1].GVT_FLAG=result;  //��������
			m_pGridCtrl->SetCellType(pItem->iRow ,6, RUNTIME_CLASS(CGridCell)); 
			col6=TRUE;
			break;
		default:break;
		}
		if(pItem->iRow >utility_count+1)  
		{ 
			*pResult = (1)? -1 : 0;                       //��ֹ�༭��Ч 
			m_pGridCtrl->SetItemBkColour(pItem->iRow , pItem->iColumn , RGB(0xFF,0xFF,0xFF));
			return FALSE;
		}
		if( col1 & col2 & col3 & col4 & col5 & col6)
			utility_count+=1;		
	}
    if(nMessageType== LVN_ITEMCHANGED)
	{ 
		//*pResult = (1)? -1 : 0;                        //��ֹ��ʼ�༭
 
	}
	 
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CPropertyPagePasswayAppoint::SetBlankCells(int Start)
{
    if(m_pGridCtrl==NULL) return;
    TCHAR buffer[10];
	 for (int row=Start+1;row<100;row++)
	 for (int  col = 0; col < ITEMNUM; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //��
			Item.col = col;  //��
            Item.crBkClr = RGB(0xFF,0xFF,0xFF);        
  			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX;
            strcpy(buffer,"");
 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl);
			m_pGridCtrl->SetItem(&Item);
	 }  //for
   
     m_pGridCtrl->Invalidate();
}

 
void CPropertyPagePasswayAppoint::InitCell(int row)
{   
	 
	TCHAR buffer[10];
	 for (int  col = 0; col < 1; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //��
			Item.col = col;  //��
            Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
  			Item.nFormat = DT_LEFT|DT_WORDBREAK|DT_NOPREFIX;
            sprintf(buffer,"%d",row);
 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl);
			m_pGridCtrl->SetItem(&Item);            
	 }  //for
     m_pGridCtrl->Invalidate();
}

void CPropertyPagePasswayAppoint::Set_Utility()
{

	char buff[4096], *ptr;

	ptr = buff;
	ptr += 2;           //for msg len

	*(ushort *)ptr =  (SET_PASSWAY_UTILITY);   
	ptr += 2;   

	*ptr=ddtid;                                        //ddtid  
	ptr+=1;    

	char *seq = ptr;
	ptr ++;

	char* count_pos = ptr;
	ptr += 2;

	*seq = -1;
	short count = 0;
	for (int i=0; i<utility_count; i++)
	{
		strncpy(ptr,utility[i].PREFIX, 9);
		ptr+=10;

		*(unsigned long *)ptr = NettoHostULong(utility[i].START_TRAIN_NUM);
		ptr += 4;   

		*(unsigned long *)ptr = NettoHostULong(utility[i].END_TRAIN_NUM);
		ptr += 4;   


		*(ushort*) ptr = NettoHostUShort(utility[i].Station);
		ptr += 2;

		*ptr=utility[i].Entry; 
		ptr+=1;

		*ptr=utility[i].Exit; 
		ptr+=1;

		*ptr=utility[i].DIRECTIONS; 
		ptr+=1;    

		*(ushort *)ptr=NettoHostUShort(utility[i].PASS_WAY_ID);
		ptr+=2;

		*ptr=utility[i].ACPT_FLAG;
		ptr+=1;    

		*ptr=utility[i].GVT_FLAG;
		ptr+=1;    

		count++;
		if (ptr+30-buff>sizeof(buff) || i==utility_count-1)
		{
			(*seq)++;
			*(ushort *)count_pos = NettoHostUShort(count);

			count = ptr -buff;
			*(ushort *)buff = count;
			int error=SendTGCommMessage(count,buff);	
			if(error) 
			{ 
				sysprint(0,PMTG.ThreadId,"[PMTG]: ����utilityʧ�ܣ�");
				return ;
			}

			count = 0;
			ptr = count_pos+2;
		}
	}
}

void CPropertyPagePasswayManagement::Del_Passway(short id)
{ 
	char buff[128], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;             //for msglen

 	*(ushort *)ptr =  (DEL_PASSWAY); 
	ptr += 2;   
	
 	*ptr = ddtid;        //Console id  
	ptr += 1;  
	
    *(ushort *)ptr = NettoHostUShort(id); 
	ptr += 2;   
	
 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����·����ʧ�ܣ�");
		return ;
    }
	return ;

}
//���þ�·
void CPropertyPagePasswayManagement::Set_Passway()
{
	CString tempstr;

	char buff[2000], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;           //for msg len

 	*(ushort *)ptr =  (SET_PASSWAY);   
	ptr += 2;   

	*ptr=ddtid;                                       
	ptr += 1;    
    
	if(current_passway< 0 || current_passway>100) return;
    *(ushort *)ptr = NettoHostUShort(info[current_passway].Pass_way_id);    // 1 info[0].Pass_way_id
	ptr += 2;

    tempstr=info[current_passway].Pass_way_name;                            // 2 
	strcpy(ptr,tempstr);
    ptr+=40;
    
     // 3  
    *(ushort *)ptr=NettoHostUShort(sta[0].Station);   
	ptr += 2;
    
    // 4
	*(ushort *)ptr = NettoHostUShort(sta[station_count-1].Station); 
	ptr += 2;

   *(ushort *)ptr = NettoHostUShort(station_count);                         //station count 
	ptr += 2;
     
	for (int j=0;j<station_count;j++)
    {
	   *(ushort *)ptr = NettoHostUShort(sta[j].Sta_lst_idx);               // idx
	   ptr += 2;
       
       *(ushort *)ptr = NettoHostUShort(sta[j].Station);                   // no
	   ptr += 2;

	   *ptr=sta[j].Entry;                                     
	   ptr+=1;  
       
       *ptr=sta[j].Exit;                                     
	   ptr+=1;  

	}  


 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����utilityʧ�ܣ�");
		return ;
    }

   return ;

}

void CPropertyPagePasswayAppoint::Del_Utility(short no)
{

	char buff[2000], *ptr;
 	ptr = buff;
	ushort count;

	ptr += 2;           //for msg len

 	*(ushort *)ptr =  (DEL_PASSWAY_UTILITY);   
	ptr += 2;   

	*ptr=ddtid;                                        //ddtid  
	ptr+=1;    
 
	int i =no;
	strncpy(ptr,utility[i].PREFIX, 9);
	ptr+=10;

	*(unsigned long *)ptr = NettoHostULong(utility[i].START_TRAIN_NUM);
	ptr += 4;   

	*(unsigned long *)ptr = NettoHostULong(utility[i].END_TRAIN_NUM);
	ptr += 4;   


	*(ushort*) ptr = NettoHostUShort(utility[i].Station);
	ptr += 2;

	*ptr=utility[i].Entry; 
	ptr+=1;

	*ptr=utility[i].Exit; 
	ptr+=1;

	*ptr=utility[i].DIRECTIONS; 
	ptr+=1;    

	*(ushort *)ptr=NettoHostUShort(utility[i].PASS_WAY_ID);
	ptr+=2;

	*ptr=utility[i].ACPT_FLAG;
	ptr+=1;    

	*ptr=utility[i].GVT_FLAG;
	ptr+=1; 

 	count = ptr -buff;
	*(ushort *)(buff  ) =  (count  );
	int error=SendTGCommMessage(count,buff);	
	if(error) 
	{ 
		sysprint(0,PMTG.ThreadId,"[PMTG]: ����ɾ��utilityʧ�ܣ�");
		return ;
    }
    
	//utility_count-=1;
   return ;


}

void CPropertyPagePasswayManagement::OnPassWaySave() 
{
	 if(! GetStationInfoFromMMI())
	 {
		 CString strText;
         strText="��������,����!";
         CDlgMessageBox dlg(strText);
		 dlg.DoModal();
         return;
	 }

	  Set_Passway();
	  m_pGridCtrl_1->Invalidate();
	  m_pGridCtrl_2->Invalidate();
}

short CPropertyPagePasswayAppoint::Get_Passwayid(char * name)
{
     
	for(int i=0;i<passway_count;i++)
	{	if(strcmp(info[i].Pass_way_name,name)==0)
		return info[i].Pass_way_id;
	}
	return -1;
}


char * CPropertyPagePasswayAppoint::Get_Passwayname(short id)
{
 
	for(int i=0;i<passway_count;i++)
	{ 
	  if(info[i].Pass_way_id==id)
      {   
		  return info[i].Pass_way_name;}
	}
	return NULL;
 
}

//******************ɾ��ĳ����·�ĳ�վ***************************
void CPropertyPagePasswayManagement::OnStationDelete() 
{
   int nRow = m_pGridCtrl_2->GetFocusCell().row;
    if (nRow >= 1 && nRow<=station_count)
    {    
		if(AfxMessageBox("Ҫɾ���˳�վ?",MB_YESNO | MB_DEFBUTTON2 |MB_ICONQUESTION) != IDYES  )
			return; 

	    m_pGridCtrl_2->DeleteRow(nRow);			
		station_count-=1;                     //��վ��Ŀ��1
        for(int i=nRow-1;i<station_count;i++)
			sta[i]=sta[i+1];                  //��վ��������     
	   
        Show_StationInfo();
             	     
    }	
 
}

void CPropertyPagePasswayManagement::OnStationNew() 
{ 
    
	int nRow = m_pGridCtrl_2->GetRowCount();
	m_pGridCtrl_2->SetRowCount(nRow+1);

	TCHAR buffer[10];
	for (int  col = 0; col < 3; col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = nRow;  //��
		Item.col = col;  //��
		Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
		Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

		if(col==0)
			sprintf(buffer,"%d",nRow); //���
		if(col==1)
			sprintf(buffer,"%d",current_passway_id);  //��·ID
		if(col==2)
			sprintf(buffer,"%d",nRow); //վ���
	 
		Item.strText.Format(buffer);
		ASSERT_VALID(m_pGridCtrl_1);
		m_pGridCtrl_2->SetItem(&Item);            
	}  
	m_pGridCtrl_2->Invalidate();
  
}

void CPropertyPagePasswayManagement::InitCell(int row)
{
	TCHAR buffer[10];
	 for (int  col = 0; col < 1; col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;  //��
			Item.col = col;  //��
            Item.crBkClr = RGB(0xFF,0xFf,0xFF);        
  			Item.nFormat = DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_NOPREFIX|DT_SINGLELINE|DT_END_ELLIPSIS;

		
            sprintf(buffer,"%d",row);

 			Item.strText.Format(buffer);
			ASSERT_VALID(m_pGridCtrl_2);
		m_pGridCtrl_2->SetItem(&Item);            
	 }  //for
     m_pGridCtrl_2->Invalidate();
}



BOOL CPropertySheetPassway::PreTranslateMessage(MSG* pMsg) 
{
	return CPropertySheet::PreTranslateMessage(pMsg);
}

BOOL CPropertyPagePasswayAppoint::CheckSameUtility(int no)
{
	for(int i=0; i< utility_count; i++)
	{ 
		if(i==no)
			continue;
		if(
			utility[i].START_TRAIN_NUM==utility[no].START_TRAIN_NUM &&
			utility[i].END_TRAIN_NUM==utility[no].END_TRAIN_NUM &&
			utility[i].DIRECTIONS==utility[no].DIRECTIONS &&
		    utility[i].PASS_WAY_ID==utility[no].PASS_WAY_ID )
		   return TRUE;
 			
	}

	return FALSE;
}

void CPropertyPagePasswayManagement::OnChangeSection(int sectionNum)
{
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
    gpcdef.GetStationList(sectionNum,nStationNum,nStationList);
	return;
}

short analysis_train(short length, const char*point, short prefix_len, char* prefix, ulong *number)
{
	for(int i=0; i<length; i++)
	{
		if(! (point[length-1-i]>= 0x30 && point[length-1-i] <= 0x39)) //�Ƿ������
		{ 
			if (length-i+1 >= prefix_len)
			{
				AfxMessageBox("���κ�ǰ׺����");
				return FALSE;
			}

			memcpy(prefix, point, length-i); 
			prefix[length-i]=0;
			*number = atoi(point+length-i);

			return TRUE;
		}
	}

	//û���ҵ���ĸ����ȫ�������֣�
	memset(prefix, 0x00, prefix_len);
	*number = atoi(point);

	return TRUE;
}
