#include "stdafx.h"

struct tagCharTable{
	char text1[3];
	char text2[2];
}DmisCharTable[]=
{{"��","A"},{"��","B"},{"��","C"},
{"��","D"},{"��","E"},{"��","F"},
{"��","G"},{"��","H"},{"��","I"},
{"��","J"},{"��","K"},{"��","L"},
{"��","M"},{"��","N"},{"��","O"},
{"��","P"},{"��","Q"},{"��","R"},
{"��","S"},{"��","T"},{"��","U"},
{"��","V"},{"��","W"},{"��","X"},
{"��","Y"},{"��","Z"},{"��","A"},
{"��","B"},{"��","C"},{"��","D"},
{"��","E"},{"��","F"},{"��","G"},
{"��","H"},{"��","I"},{"��","J"},
{"��","K"},{"��","L"},{"��","M"},
{"��","N"},{"��","O"},{"��","P"},
{"��","Q"},{"��","R"},{"��","S"},
{"��","T"},{"��","U"},{"��","V"},
{"��","W"},{"��","X"},{"��","Y"},
{"��","Z"},{"��","1"},{"��","2"},
{"��","3"},{"��","4"},{"��","5"},
{"��","6"},{"��","7"},{"��","8"},
{"��","9"},{"��","0"},{"��","+"},
{"��","-"}};

CString ChangeAlpha(CString &input){
	CString temp = input;
	CString tmp;
	CString t_sReturnValue("");

	while(!temp.IsEmpty()){
		if(IsCharAlphaNumeric(temp.GetAt(0))){
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}else if((BYTE)temp.GetAt(0)>=0xA0){
			tmp = temp.Left(2);
			for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++){
				CString t_stmp;
				t_stmp.Format("%s",DmisCharTable[i].text1);
				if(t_stmp == tmp){
					tmp.Format("%s",DmisCharTable[i].text2);
					break;
				}
			}
			t_sReturnValue += tmp;
			temp = temp.Mid(2);
		}else{
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}
	}
	return t_sReturnValue;
}

CString ChangeFromHalf2Full(CString &input){
	CString temp = input;
	CString tmp;
	CString t_sReturnValue("");

	while(!temp.IsEmpty()){
		if(IsCharAlphaNumeric(temp.GetAt(0))){
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}else if((BYTE)temp.GetAt(0)>=0xA0){
			tmp = temp.Left(2);
			for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++){
				CString t_stmp;
				t_stmp.Format("%s",DmisCharTable[i].text1);
				if(t_stmp == tmp){
					tmp.Format("%s",DmisCharTable[i].text2);
					break;
				}
			}
			t_sReturnValue += tmp;
			temp = temp.Mid(2);
		}else{
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}
	}
	return t_sReturnValue;
}