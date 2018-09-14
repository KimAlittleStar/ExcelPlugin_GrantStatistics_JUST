#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "direct.h"
#include "windows.h"
#define DEBUG_Printf(ftm, ...) 
//printf(ftm, ##__VA_ARGS__)                                                /*!< �������� ����Ҫ���Ե�ʱ�� ���궨�� ����� debug_Printf*/
#define MAX_EXCEL_LINE 1000                                                  /*!< ��֧�ֵĴ������������� ֻ���޸Ĵ˴�����            */
#define MAX_EXCEL_ROW 200                                                    /*!< ͬ��                                                   */    

#define TABLE_SWITCH "NewTable"                                             /*!< ��ͬһ���ļ���������ڶ�����                         */
                                                                            /*!<        ������ ����ַ�������ʶ�±�ʼ                 */
typedef unsigned char uint8;                                                /*!< �Զ������� �޷���8λ����(�����ϵͳ��ֲ)               */
typedef signed char int8;                                                   /*!< �Զ������� �з���8λ����(�����ϵͳ��ֲ)��ͬ�� char    */
typedef unsigned short uint16;
typedef signed short int16;                                                 /*!< ��ͬ�� short                                           */
typedef unsigned int uint32;
typedef signed int int32;                                                   /*!< ��ͬ�� int                                             */
typedef double DOUBLE;                                                      /*!< ��ͬ�� double                                          */

#ifdef _WINDOWS_
/*!< ���������window.h ͷ�ļ� */

enum WIN_DOS_COLOS_TYPE
{  
    WIN_DOS_COLOR_Black='0',
    WIN_DOS_COLOR_Blue='1',
    WIN_DOS_COLOR_Green='2',
    WIN_DOS_COLOR_LakeBlue='3',
    WIN_DOS_COLOR_Red='4',
    WIN_DOS_COLOR_Purple='5',
    WIN_DOS_COLOR_Yellow='6',
    WIN_DOS_COLOR_White='7',
    WIN_DOS_COLOR_Grey='8',
    WIN_DOS_COLOR_LightBlue='9',
    WIN_DOS_COLOR_LightGreen='A',
    WIN_DOS_COLOR_LightAGreen='B',
    WIN_DOS_COLOR_LightRed='C',
    WIN_DOS_COLOR_Lavender='D',
    WIN_DOS_COLOR_LightYellow='E',
    WIN_DOS_COLOR_BrightWhite='F'   
};



#endif


enum COURSETYPE                                                             /*!< ���� ö�� �γ�����                                     */
{
    Professional_compulsory,                                                /*!< רҵ���޿�                                             */
    Professional_elective,                                                  /*!< רҵѡ�޿� 											*/
    Public_compulsory,                                                      /*!< �������޿� ���� �ۺ��� ʵ��ʵϰ��                      */
    Public_elective,                                                        /*!< ����ѡ�޿�                                             */
    TYPE_ERR                                                                /*!< �������Ͷ�����                                         */
};

typedef struct
{
    uint32 line; /*!< �� */
    uint32 row;  /*!< �� */
    int8 *data[MAX_EXCEL_LINE][MAX_EXCEL_ROW];
} EXCEL;


int8 overbuff = '\0';                                                       /*!< �������ַ������ִ�����߲��ʱ ʹ���ַ��Ϊ�� ������Ϊ��   */
int8 (*judgeGrand)(int8 *);                                                 /*!< �洢�ж�ѧ��/�ɼ��Ƿ���Ч�ĺ���ָ�� �Թ������ж�ʹ��       */

int8 Path[400];

void printExcel(EXCEL *ex);
DOUBLE toDouble(const int8 *str);
int32 toInt(const int8 *str);
EXCEL *getExcel(FILE *excel);
void closeExcel(EXCEL *excel);
uint16 find_title(EXCEL *exc);
void get_subjectValue_type(uint16 titleline, EXCEL *exc, DOUBLE *valuebuff, uint8 *typebuff);
DOUBLE *getGrant(EXCEL *exc);
int8 creat_Excel(int8 *filename, EXCEL *excel, DOUBLE *grant);
int8 judgeGrant_halfyear(int8 *grant);
int8 judgeGrant_year(int8 *grant);
int8 show_UI(void);
FILE *getFile(int8 *filename);
void CLOSE_ALL(EXCEL *excel, DOUBLE *grand, int8 *filename);
int8 process_Excel(int8 *filename);
void exit_app(void);
int8* getPath(int8* filename);

int main(int argc, char const *argv[])
{
    FILE *fp;
    int8 filename[400] = {0};                                   /*!< �����ļ�������                                            */
    int i, j;
    char c;
    EXCEL *excel;
    DOUBLE *grand;                                              /*!< ƽ������洢����ָ��                                      */    
    int8 *ptemp = NULL;                                         /*!< temp tmp ������ �������м����û��ʵ������,Ӧ��������ȷ�� */
    
    
    show_UI();                                                  /*!< ��ʾ UI ʹ�� BUG����                                      */
    
	if (argc > 1)                                               
    {/*!< ��������ֱ�Ӵ��� �ļ� ���� */
        for (i = 1; i < argc; i++)
        {/*!< �����������ļ� */
            strcpy(filename, argv[i]);
            if (process_Excel(filename) == -1)
            {
                printf("%s �ļ�����ʧ������\n", filename);
                break;
            }
        }
    }
    else
    {/*!< ������ֻ����һ���ļ� */
        if (process_Excel(filename) == -1)
        {
        	 printf("%s �ļ�����ʧ������\n", filename);
		} 
    }
    exit_app();
    getchar(); 
    return 0;
}

/**********************************************************************************
* ��������: process_Excel
* ��������: ������ ���ݱ�ͷ��Ϣ�½�һ�� Excel��
* ��������: void input:ԴExcel�ļ�·��
* �������: void output:����״̬ 0:�ɹ� -1 :ʧ��
* ���� :author:@Kim_alittle_star
**********************************************************************************/
int8 process_Excel(int8 *filename)
{
    FILE *fp;
    EXCEL *excel;
    DOUBLE *grand;

    fp = getFile(filename);             /*!< �õ��ļ���ָ�� */
    if (fp == NULL)
    {
        printf("�ļ��򿪴��� �����ļ� %s\r\n", filename);
        return -1;
    }
    
    getPath(filename);

    while (!feof(fp))                  /*!< ��ǰ�ļ�ָ�벢û����ȫ��ȡ��� */
    {
        excel = getExcel(fp);          /*!< ��ȡ�����Ϣ                   */
        if (excel == NULL)
        {
            getchar();
            return -1;
        }

        grand = getGrant(excel);      /*!< ���ƽ�����������            */

        if(creat_Excel(filename, excel, grand) == 0) /*!< �½� ���  �ɹ�     */
        {
             CLOSE_ALL(excel, grand, filename); /*!< ���� �ڴ�       */
        }else
        {
            return -1;
        }

                
    }
    return 0;
}

/**********************************************************************************
* ��������: toDouble
* ��������: ���ַ���ת��Ϊ double�� ת��ʧ��ʱ����0.0 
* ��������: void input:Ŀ���ַ���
* �������: void output:ת����� ���� 
*               Ex: input: "12.3523fdgdf" return :12.3323(double)
* ���� :author:@Kim_alittle_star
**********************************************************************************/
DOUBLE toDouble(const int8 *str)
{
    DOUBLE ret;
    int8 *ptemp = (int8 *)str;
    int8 *err = NULL;
    while ((*ptemp < '0' || *ptemp > '9') && *ptemp != '-' && *ptemp != '\0')
    {
        ptemp++;
    }
    ret = strtod(ptemp, &err);
    if (*err != '\0')
    {
        DEBUG_Printf("toDouble err was %s\n", err);
    }
    return ret;
}

/**********************************************************************************
* ��������: toInt
* ��������: �ַ���ת int��
* ��������: void input:str
* �������: void output:���� 
*       Ex: "32323shdf" return :32323
* ���� :author:@Kim_alittle_star
**********************************************************************************/
int32 toInt(const int8 *str)
{
    int32 ret;
    int8 *ptemp = (int8 *)str;
    int8 *err = NULL;
    while ((*ptemp < '0' || *ptemp > '9') && *ptemp != '-' && *ptemp != '\0')
    {
        ptemp++;
    }
    ret = (int32)strtol(ptemp, &err, 10);
    if (err != NULL)
    {
        DEBUG_Printf("toInt  was %d\n", ret);
    }
    return ret;
}

/**********************************************************************************
* ��������: closeExcel
* ��������: �ͷű���ڴ� 
* ��������: void input:���ָ��
* �������: void output:void
* ���� :author:@Kim_alittle_star
**********************************************************************************/
void closeExcel(EXCEL *excel)
{
    uint16 i;
    for (i = 0; i < excel->line; i++)
    {
        free(excel->data[i][0]);
    }
    free(excel);
}

/**********************************************************************************
* ��������: strslip
* ��������: �ַ��� �ָ�� 
* ��������: void input: Դ�ַ��� str Ŀ���ַ������� link Ŀ��ָ�� ch
* �������: void output:�ַ������� ��С
*           Ex: input: str = "12,34,45,65"; ch = ','; ->link = {"12","34","45","65"};
*                       return 4;
* ���� :author:@Kim_alittle_star
**********************************************************************************/
uint32 strslip(int8 *str, int8 *link[], char ch)
{
    uint32 i = 1;
    int8 *pstr = str;
    link[0] = pstr;
    while (*pstr != '\0' && i < MAX_EXCEL_ROW)
    {
        if (*pstr == ch)
        {
            *pstr = '\0';
            pstr++;
            link[i] = pstr;
            i++;
        }
        else
            pstr++;
    }
    return i;
}

/**********************************************************************************
* ��������: getExcel
* ��������: �õ�������� 
* ��������: void input:�洢�����ļ��� 
* �������: void output: ��� ��ʽ��Excel �ṹ��ָ��
* ���� :author:@Kim_alittle_star
**********************************************************************************/
EXCEL *getExcel(FILE *excel)
{
    EXCEL *ret = NULL;
    int8 ch;
    uint16 i = 0, tmp, j;
    uint32 maxraw = 0;
    int8 *linetemp[MAX_EXCEL_LINE];
    int8 *strbuff;
    int8 *errflag;
    uint32 strlengh = 0;
    ret = (EXCEL *)malloc(sizeof(EXCEL));
    strbuff = (int8 *)malloc(sizeof(int8) * (MAX_EXCEL_ROW * 10));
    while (!feof(excel) && i < MAX_EXCEL_LINE)
    {
        errflag = fgets(strbuff, (MAX_EXCEL_ROW * 10), excel); /*!< �õ�һ�е�����(�� ���з�) */
        if (errflag == strbuff)                                /*!< ������� �������� */
        {
            DEBUG_Printf("%s\n", strbuff);
            strlengh = strlen(strbuff);

            if (strstr(strbuff, TABLE_SWITCH))                /*!< ��⵽ �±��־ ���� ��ʼ��ȡ�ɼ� */
            {
                break;
            }

            if (*(strbuff + strlengh - 1) == '\n')            /*!< ȥ�����з�                      */
            {
                *(strbuff + strlengh - 1) = '\0';
            }
            linetemp[i] = malloc(sizeof(int8) * (strlengh + 10));
            strcpy(linetemp[i], strbuff);
            tmp = strslip(linetemp[i], ret->data[i], ',');    /*!< �ָ��ַ���                       */
            if (tmp > maxraw)
            {
                maxraw = tmp;                                 /*!< �������� ȷ����¼ �е����� һֱΪ��� */
            }
            else if (tmp < maxraw)                            /*!< ������� �ָ�����С��������� ��ô�м�϶������˴���(�ļ���Ϣ�ڲ�����) */
            {
                for (j = tmp; j < maxraw; j++)                /*!< ���� ���� ���óɵ�ַ��Ϊ�㵫����Ϊ��                                */
                {
                    ret->data[i][j] = &overbuff;
                }
            }
            i++;
        }
        else if (errflag == EOF)
        {
            printf("��ȡ�ļ�����,�벻��Ҫ�����жԱ������������� \n");
            return NULL;
        }
        else if (errflag == NULL && i == 0)
        {
            printf("���ļ�-----------> �����ļ� \n");
            return NULL;
        }
        else
        {
            /*!< ��ȡ�ļ����� �Ҳ�Ϊ��  */
            ret->line = i;
            ret->row = maxraw;
        }
    }
    if (i >= MAX_EXCEL_LINE || maxraw >= MAX_EXCEL_ROW)
    {
        printf("����С����,���� �����: %d,����� %d\n", MAX_EXCEL_LINE - 1, MAX_EXCEL_ROW - 1);
        return NULL;
    }
    if (errflag == strbuff)
    {
        ret->row = maxraw;
        ret->line = i;
        //        return ret;
    }
    free(strbuff);
    return ret;
}

/**********************************************************************************
* ��������: printExcel
* ��������: ��ӡ�������
* ��������: void input: Excel �ṹ��ָ��
* �������: void output:void
* ���� :author:@Kim_alittle_star
**********************************************************************************/
void printExcel(EXCEL *ex)
{
    uint32 i, j;
    for (i = 0; i < ex->line; i++)
    {
        for (j = 0; j < ex->row; j++)
        {
            printf("%s\t", ex->data[i][j]);
        }
        printf("\n\n\n\n\n\n");
    }
}

/**********************************************************************************
* ��������: getGrant
* ��������: ����������ݵõ� �ɼ�������
* ��������: void input:Excel �ṹ��ָ��
* �������: void output: double ���͵�����ָ�� 
* ���� :author:@Kim_alittle_star
**********************************************************************************/
DOUBLE *getGrant(EXCEL *exc)
{
    DOUBLE *grant;
    DOUBLE grantsum = 0.0, devsum = 0.0;
    uint16 i, j;
    uint16 titleline = 0;
    DOUBLE titleValue[MAX_EXCEL_ROW];
    uint8 course[MAX_EXCEL_ROW];
    DOUBLE *tmp_D;
    grant = malloc(sizeof(DOUBLE) * exc->line);
    tmp_D = grant;

    titleline = find_title(exc);
    DEBUG_Printf("titleline %d \n", titleline);
    
    get_subjectValue_type(titleline, exc, titleValue, course);
    
    for (i = 0; i < exc->row; i++)
    {
        if (course[i] != TYPE_ERR)
        {
            DEBUG_Printf("TYPE : %d  ѧ��:%f", course[i], titleValue[i]);
        }
    }
    
    for (i = 0; i < exc->line; i++)
    {
        grantsum = 0.0;
        devsum = 0.0;
        if (i > titleline)
        {/*!< ��� ����ɨ�赽�� ��Ŀ���������� ��ʼ���� */
            for (j = 0; j < exc->row; j++)
            {
                if (course[j] != TYPE_ERR)
                {
                    switch (course[j])
                    {
                    case Professional_compulsory:
                        if (judgeGrant_halfyear(exc->data[i][j]))
                        {
                            grantsum += (toInt(exc->data[i][j]) * titleValue[j]);
                            devsum += titleValue[j];
                        }
                        break;
                    case Professional_elective:
                        if (judgeGrand(exc->data[i][j]))
                        {
                            grantsum += (toInt(exc->data[i][j]) * titleValue[j]);
                            devsum += titleValue[j];
                        }
                        break;
                    case Public_compulsory:
                        if (judgeGrant_halfyear(exc->data[i][j]))
                        {
                            grantsum += (toInt(exc->data[i][j]) * titleValue[j]);
                            devsum += titleValue[j];
                        }
                        break;
                    case Public_elective:
                        DEBUG_Printf("����ѡ�޿� we do no thing \n");
                        break;
                    default:
                        DEBUG_Printf("ARG illgle!\n");
                        break;
                    }
                }
            }
            if (devsum < 0.01 && devsum > -1)
            {
                devsum = 1.0;
            }
            grant[i] = grantsum / devsum;
            DEBUG_Printf("name: %s,grantsum %f,devsum %f\n", exc->data[i][1], grantsum, devsum);
        }/*!< һ��(һλͬѧ)�ɼ�������� */
        else
        {/*!< δ�� ��Ŀ������  */
            grant[i] = -1.0;
        }
    }/*!< �����д������ */
    return grant;
}

/**********************************************************************************
* ��������: find_title
* ��������: �ҵ������ı�������������
* ��������: void input:��� �ṹ��ָ��
* �������: void output:��������������
* ���� :author:@Kim_alittle_star
**********************************************************************************/
uint16 find_title(EXCEL *exc)
{
    uint16 i, j;
    uint16 ret = 0xFFFF;
    for (i = 0; i < exc->line; i++)
    {
        for (j = 0; j < exc->row; j++)
        {
            if (*exc->data[i][j] == '\0')
                continue;
            else if (strstr(exc->data[i][j], "רҵ���޿�") || strstr(exc->data[i][j], "�������޿�") || strstr(exc->data[i][j], "רҵѡ�޿�"))
            {
                ret = i;
                break;
            }
        }
        if (ret != 0xFFFF)
            break;
    }
    return ret;
}

/**********************************************************************************
* ��������: get_subjectValue_type
* ��������: �õ�ÿ�ſ�Ŀ��ѧ�����ʺ�ѧ��
* ��������: void input: ��Ŀ������  ,Excel �ṹ�� ,��Ӧѧ������ ,��Ӧѧ����������
* �������: void output:һ������ѧ��
* ���� :author:@Kim_alittle_star
**********************************************************************************/
void get_subjectValue_type(uint16 titleline, EXCEL *exc, DOUBLE *valuebuff, uint8 *typebuff)
{
    uint16 i;
    uint16 ret;
    int8 *ptemp;
    for (i = 0; i < exc->row; i++)
    {
        if (strstr(exc->data[titleline][i], "�������޿�") || strstr(exc->data[titleline][i], "���´�ҵ") || strstr(exc->data[titleline][i], "ʵϰʵ��"))
        {
            typebuff[i] = Public_compulsory;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "רҵ���޿�"))
        {
            typebuff[i] = Professional_compulsory;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "����ѡ�޿�"))
        {
            typebuff[i] = Public_elective;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "רҵѡ�޿�"))
        {
            typebuff[i] = Professional_elective;
            ret++;
        }
        else
        {
            typebuff[i] = TYPE_ERR;
        }
        if (typebuff[i] != TYPE_ERR)
        {
            valuebuff[i] = toDouble(exc->data[titleline][i]);
        }
        else
        {
            valuebuff[i] = -1.0;
        }
    }
}

/**********************************************************************************
* ��������: creat_Excel
* ��������: ����һ���µı�������Ӧ���ļ���
* ��������: void input:֮ǰ��Ե��ļ�·��,Excel �ṹ��ָ�� ,�ɼ����� (���������һ��)
* �������: void output:��
* ���� :author:@Kim_alittle_star
**********************************************************************************/
int8 creat_Excel(int8 *filename, EXCEL *excel, DOUBLE *grant)
{
    FILE *fp;
    uint16 i, j;
    uint8 strlenth;
    int8 newfilename[256];
    int8 *ptemp = excel->data[0][0];
    strcpy(newfilename,Path);
    if (strstr(ptemp, "�ɼ����ܱ�") && strstr(ptemp, "ѧ��"))
    {
        strcat(newfilename, ptemp);
        strcat(newfilename, "(�Ѵ���).xls");
    }
    else
    {
        strcat(newfilename,filename);
        ptemp = newfilename;
        strlenth = strlen(newfilename);
        ptemp += (strlenth - 4);
        sprintf(ptemp, "(�Ѵ���).xls");
    }

    fp = fopen(newfilename, "a+");
    if(fp == NULL)
    {
        printf("�����ļ�ʧ��\n");
        return -1;
    }
    for (i = 0; i < excel->line; i++)
    {
        for (j = 0; j < excel->row; j++)
        {
            fprintf(fp, "%s\t", excel->data[i][j]);
        }

        if (grant[i] > 0)
            fprintf(fp, "%f\t\n", grant[i]);
        else
            fprintf(fp, "\n");
    }
    fclose(fp);
}

int8 judgeGrant_halfyear(int8 *grant)
{
    if (*grant != '\0')
        return 1;
    else
        return 0;
}
int8 judgeGrant_year(int8 *grant)
{
    if (!(*grant == '\0' || *grant == '0'))
        return 1;
    else
        return 0;
}

int8 show_UI(void)
{
    int8 c;
    setWindows();
    printf(" ------------------------------------------------------------------------------------\n");
    printf("|                                     ����                                            |\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*  ��֪BUG: ---------->                          version:1.3.0                        *\n");
    printf("*  I  :  ���֧�� �����:%4d ��:%4d                                                 *\n", MAX_EXCEL_LINE, MAX_EXCEL_ROW);
    printf("*  II :  ���ִ�������ر����´򿪼���                                             *\n");
    printf("*  III:  �����Ч������ϵ���߽����Ż�����                                             *\n");
    printf("*  IV :  auther @KimAlittleStar By 2018.9.13                                          *\n");
    printf("*  V  :  ����ɴ�C�������� ����Ȥ����ϵ�������Э�Ḻ���˼���                         *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("|                                      ʹ������                                       |\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*   1.����Ʒ ������ ��������ѧ�ϲ�У���ɼ������ʽ �ı����㼨���                 *\n");
    printf("*   2.�Ƚ�ԭʼ������Ϊ csv ��ʽ�ļ�                                                 *\n");
    printf("*   3.Excel->���Ϊ->������ʽ->*.csv //���� ǧ��Ҫ����                                *\n");
    printf("*   4.���������Ƶ� csv �ļ������ļ�Ŀ¼��                                           *\n");
    printf("*   5.�����ļ����� ���ɵõ������ı��                                               *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*   6.���б�ָ��� %-20s                                               *\n", TABLE_SWITCH);
    printf("*   7.���ڱ��в���һ�е����� �ڱ�ָ��� ����ʵ�ֶ��ͬʱ����                          *\n");
    printf("*   8.���س�ȷ��                                                                      *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    while (getchar() != '\n')
    {
    }
    printf(" ------------------------------------------------------------------------------------\n");
    printf("* ͳ������ȷ�ϣ�--------->                                                           *\n");
    printf("*       1.һѧ��ͳ��                                                                 *\n");
    printf("*       2.һѧ��ͳ��                                                                 *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("�������ּ��س������\n");
    c = getchar();
    while (c < '1' || c > '2')
    {
        c = getchar();
    } /*!< ȷ������Ķ���һ������ѡ����  */
    while (getchar() != '\n')
        ; /*!< ȷ����ȡ�����һ�� �س� �ź� */
    if (c == '2')
    {
        judgeGrand = judgeGrant_year;
        printf("��ѡ���� ѧ��ͳ��ģʽ .\n");
    }
    else
    {
        judgeGrand = judgeGrant_halfyear;
        printf("��ѡ���� ѧ��ͳ��ģʽ .\n");
    }
    return c;
}

FILE *getFile(int8 *filename)
{
    FILE *fp;
    int8 *ptemp;
    if (*filename == '\0')
    {
        printf(" �����ļ���: ");
        gets(filename); 
    }

    ptemp = strstr(filename, ".csv");
    if (ptemp != NULL && *(ptemp + 4) == '\0')
    {
        //do no thing
    }
    else
    {
        strcat(filename, ".csv");
    }
    fp = fopen(filename, "r"); // fpָ��ָ���ļ�ͷ��
    return fp;
}

void CLOSE_ALL(EXCEL *excel, DOUBLE *grand, int8 *filename)
{
    /*!< �����ļ��������� */
    int8 strlenth;
    int8 *realname = filename;
    strlenth = strlen(filename);
    realname += (strlenth - 1);
    while (*realname != '\\' && realname != filename)
    {
        realname--;
    }
    /*!< ������� */
    printf(" ------------------------------------------------------------------------------------\n");
    printf("* �ļ�:%68s          *\n", realname);
    printf("*                                   �������                                         *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    closeExcel(excel);
    free(grand);
}

void exit_app(void)
{
	printf(" ------------------------------------------------------------------------------------\n");
	printf("*                                    WARNING                                         *\n");
	printf("*        This program only works forJiangxi University of Science and Technology     *\n");
	printf("*    (JUST) Anyone has the right to use but does not have the right to edit.         *\n");
	printf("*                                                        Author: @KimAilttleStar     *\n");
	printf(" ------------------------------------------------------------------------------------\n");
}
void setWindows(void)
{
#ifdef _WINDOWS_ 
    int8 commentBuff[100];

    sprintf(commentBuff,"title %s","��������ѧһ�����ƽ������");
    system(commentBuff);
    sprintf(commentBuff,"mode con cols=%d lines=%d",90,35);
    system(commentBuff);
    sprintf(commentBuff,"color %c%c",WIN_DOS_COLOR_Black,WIN_DOS_COLOR_LightRed);
    system(commentBuff);
#endif
} 

int8* getPath(int8* filename)
{
    uint16 strlenth = strlen(filename);
    int8*  ptemp = filename;
    int16 tmp = 0;
    ptemp += (strlenth-1);
    while(*ptemp != '\\' && ptemp  != filename)
    {
        ptemp--;
    }
    if(ptemp > filename)
    {
        tmp = ptemp - filename;
        while(tmp>=0)
        {
            Path[tmp] = filename[tmp];
            tmp--;
        }
    }else
    {
        Path[0] = '\0';
    }
    return Path;
}

