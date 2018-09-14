#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "direct.h"
#include "windows.h"
#define DEBUG_Printf(ftm, ...) 
//printf(ftm, ##__VA_ARGS__)                                                /*!< 调试助手 当需要调试的时候 将宏定义 定义成 debug_Printf*/
#define MAX_EXCEL_LINE 1000                                                  /*!< 所支持的处理表格的最大行数 只需修改此处即可            */
#define MAX_EXCEL_ROW 200                                                    /*!< 同上                                                   */    

#define TABLE_SWITCH "NewTable"                                             /*!< 在同一个文件中允许存在多个表格                         */
                                                                            /*!<        其中以 这个字符串来标识新表开始                 */
typedef unsigned char uint8;                                                /*!< 自定义类型 无符号8位整形(方便多系统移植)               */
typedef signed char int8;                                                   /*!< 自定义类型 有符号8位整形(方便多系统移植)等同于 char    */
typedef unsigned short uint16;
typedef signed short int16;                                                 /*!< 等同于 short                                           */
typedef unsigned int uint32;
typedef signed int int32;                                                   /*!< 等同于 int                                             */
typedef double DOUBLE;                                                      /*!< 等同于 double                                          */

#ifdef _WINDOWS_
/*!< 如果加入了window.h 头文件 */

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


enum COURSETYPE                                                             /*!< 定义 枚举 课程类型                                     */
{
    Professional_compulsory,                                                /*!< 专业必修课                                             */
    Professional_elective,                                                  /*!< 专业选修课 											*/
    Public_compulsory,                                                      /*!< 公共必修课 附带 综合类 实践实习类                      */
    Public_elective,                                                        /*!< 公共选修课                                             */
    TYPE_ERR                                                                /*!< 以上类型都不是                                         */
};

typedef struct
{
    uint32 line; /*!< 行 */
    uint32 row;  /*!< 列 */
    int8 *data[MAX_EXCEL_LINE][MAX_EXCEL_ROW];
} EXCEL;


int8 overbuff = '\0';                                                       /*!< 当处理字符串出现错误或者差错时 使其地址不为空 但内容为空   */
int8 (*judgeGrand)(int8 *);                                                 /*!< 存储判断学分/成绩是否有效的函数指针 以供后面判断使用       */

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
    int8 filename[400] = {0};                                   /*!< 定义文件名缓存                                            */
    int i, j;
    char c;
    EXCEL *excel;
    DOUBLE *grand;                                              /*!< 平均绩点存储数组指针                                      */    
    int8 *ptemp = NULL;                                         /*!< temp tmp 等命名 都代表中间变量没有实际意义,应以上下文确定 */
    
    
    show_UI();                                                  /*!< 显示 UI 使用 BUG声明                                      */
    
	if (argc > 1)                                               
    {/*!< 如果有外界直接传入 文件 进来 */
        for (i = 1; i < argc; i++)
        {/*!< 遍历处理表格文件 */
            strcpy(filename, argv[i]);
            if (process_Excel(filename) == -1)
            {
                printf("%s 文件处理失败请检查\n", filename);
                break;
            }
        }
    }
    else
    {/*!< 否则则只处理一个文件 */
        if (process_Excel(filename) == -1)
        {
        	 printf("%s 文件处理失败请检查\n", filename);
		} 
    }
    exit_app();
    getchar(); 
    return 0;
}

/**********************************************************************************
* 函数名称: process_Excel
* 函数功能: 处理表格 依据表头信息新建一个 Excel表
* 函数输入: void input:源Excel文件路径
* 函数输出: void output:处理状态 0:成功 -1 :失败
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
int8 process_Excel(int8 *filename)
{
    FILE *fp;
    EXCEL *excel;
    DOUBLE *grand;

    fp = getFile(filename);             /*!< 得到文件流指针 */
    if (fp == NULL)
    {
        printf("文件打开错误 请检查文件 %s\r\n", filename);
        return -1;
    }
    
    getPath(filename);

    while (!feof(fp))                  /*!< 当前文件指针并没有完全读取完毕 */
    {
        excel = getExcel(fp);          /*!< 获取表格信息                   */
        if (excel == NULL)
        {
            getchar();
            return -1;
        }

        grand = getGrant(excel);      /*!< 获得平均绩点分数组            */

        if(creat_Excel(filename, excel, grand) == 0) /*!< 新建 表格  成功     */
        {
             CLOSE_ALL(excel, grand, filename); /*!< 回收 内存       */
        }else
        {
            return -1;
        }

                
    }
    return 0;
}

/**********************************************************************************
* 函数名称: toDouble
* 函数功能: 将字符串转换为 double型 转化失败时返回0.0 
* 函数输入: void input:目标字符串
* 函数输出: void output:转换后的 数字 
*               Ex: input: "12.3523fdgdf" return :12.3323(double)
* 作者 :author:@Kim_alittle_star
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
* 函数名称: toInt
* 函数功能: 字符串转 int型
* 函数输入: void input:str
* 函数输出: void output:数字 
*       Ex: "32323shdf" return :32323
* 作者 :author:@Kim_alittle_star
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
* 函数名称: closeExcel
* 函数功能: 释放表格内存 
* 函数输入: void input:表格指针
* 函数输出: void output:void
* 作者 :author:@Kim_alittle_star
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
* 函数名称: strslip
* 函数功能: 字符串 分割函数 
* 函数输入: void input: 源字符串 str 目标字符串数组 link 目标分割符 ch
* 函数输出: void output:字符串数组 大小
*           Ex: input: str = "12,34,45,65"; ch = ','; ->link = {"12","34","45","65"};
*                       return 4;
* 作者 :author:@Kim_alittle_star
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
* 函数名称: getExcel
* 函数功能: 得到表格数据 
* 函数输入: void input:存储表格的文件流 
* 函数输出: void output: 表格 形式的Excel 结构体指针
* 作者 :author:@Kim_alittle_star
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
        errflag = fgets(strbuff, (MAX_EXCEL_ROW * 10), excel); /*!< 得到一行的数据(带 换行符) */
        if (errflag == strbuff)                                /*!< 正常情况 正常处理 */
        {
            DEBUG_Printf("%s\n", strbuff);
            strlengh = strlen(strbuff);

            if (strstr(strbuff, TABLE_SWITCH))                /*!< 检测到 新表标志 跳出 开始获取成绩 */
            {
                break;
            }

            if (*(strbuff + strlengh - 1) == '\n')            /*!< 去除换行符                      */
            {
                *(strbuff + strlengh - 1) = '\0';
            }
            linetemp[i] = malloc(sizeof(int8) * (strlengh + 10));
            strcpy(linetemp[i], strbuff);
            tmp = strslip(linetemp[i], ret->data[i], ',');    /*!< 分割字符串                       */
            if (tmp > maxraw)
            {
                maxraw = tmp;                                 /*!< 保险设置 确保记录 行的数据 一直为最大 */
            }
            else if (tmp < maxraw)                            /*!< 如果出现 分割数据小于最大行数 那么中间肯定出现了错误(文件信息内部错误) */
            {
                for (j = tmp; j < maxraw; j++)                /*!< 遍历 将其 设置成地址不为零但内容为空                                */
                {
                    ret->data[i][j] = &overbuff;
                }
            }
            i++;
        }
        else if (errflag == EOF)
        {
            printf("读取文件错误,请不在要处理中对表格进行其他操作 \n");
            return NULL;
        }
        else if (errflag == NULL && i == 0)
        {
            printf("空文件-----------> 请检查文件 \n");
            return NULL;
        }
        else
        {
            /*!< 读取文件结束 且不为空  */
            ret->line = i;
            ret->row = maxraw;
        }
    }
    if (i >= MAX_EXCEL_LINE || maxraw >= MAX_EXCEL_ROW)
    {
        printf("表格大小超纲,请检查 最大行: %d,最大列 %d\n", MAX_EXCEL_LINE - 1, MAX_EXCEL_ROW - 1);
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
* 函数名称: printExcel
* 函数功能: 打印表格数据
* 函数输入: void input: Excel 结构体指针
* 函数输出: void output:void
* 作者 :author:@Kim_alittle_star
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
* 函数名称: getGrant
* 函数功能: 分析表格数据得到 成绩的数组
* 函数输入: void input:Excel 结构体指针
* 函数输出: void output: double 类型的数组指针 
* 作者 :author:@Kim_alittle_star
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
            DEBUG_Printf("TYPE : %d  学分:%f", course[i], titleValue[i]);
        }
    }
    
    for (i = 0; i < exc->line; i++)
    {
        grantsum = 0.0;
        devsum = 0.0;
        if (i > titleline)
        {/*!< 如果 行数扫描到了 科目详情栏以下 则开始处理 */
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
                        DEBUG_Printf("公共选修课 we do no thing \n");
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
        }/*!< 一行(一位同学)成绩处理完毕 */
        else
        {/*!< 未到 科目详情栏  */
            grant[i] = -1.0;
        }
    }/*!< 所有行处理完毕 */
    return grant;
}

/**********************************************************************************
* 函数名称: find_title
* 函数功能: 找到真正的标题栏所在行数
* 函数输入: void input:表格 结构体指针
* 函数输出: void output:标题栏所在行数
* 作者 :author:@Kim_alittle_star
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
            else if (strstr(exc->data[i][j], "专业必修课") || strstr(exc->data[i][j], "公共必修课") || strstr(exc->data[i][j], "专业选修课"))
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
* 函数名称: get_subjectValue_type
* 函数功能: 得到每门科目的学科性质和学分
* 函数输入: void input: 科目所在栏  ,Excel 结构体 ,对应学分数组 ,对应学科性质数组
* 函数输出: void output:一共几门学科
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void get_subjectValue_type(uint16 titleline, EXCEL *exc, DOUBLE *valuebuff, uint8 *typebuff)
{
    uint16 i;
    uint16 ret;
    int8 *ptemp;
    for (i = 0; i < exc->row; i++)
    {
        if (strstr(exc->data[titleline][i], "公共必修课") || strstr(exc->data[titleline][i], "创新创业") || strstr(exc->data[titleline][i], "实习实践"))
        {
            typebuff[i] = Public_compulsory;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "专业必修课"))
        {
            typebuff[i] = Professional_compulsory;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "公共选修课"))
        {
            typebuff[i] = Public_elective;
            ret++;
        }
        else if (strstr(exc->data[titleline][i], "专业选修课"))
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
* 函数名称: creat_Excel
* 函数功能: 创建一个新的表格在相对应的文件夹
* 函数输入: void input:之前相对的文件路径,Excel 结构体指针 ,成绩数组 (附加在最后一列)
* 函数输出: void output:空
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
int8 creat_Excel(int8 *filename, EXCEL *excel, DOUBLE *grant)
{
    FILE *fp;
    uint16 i, j;
    uint8 strlenth;
    int8 newfilename[256];
    int8 *ptemp = excel->data[0][0];
    strcpy(newfilename,Path);
    if (strstr(ptemp, "成绩汇总表") && strstr(ptemp, "学年"))
    {
        strcat(newfilename, ptemp);
        strcat(newfilename, "(已处理).xls");
    }
    else
    {
        strcat(newfilename,filename);
        ptemp = newfilename;
        strlenth = strlen(newfilename);
        ptemp += (strlenth - 4);
        sprintf(ptemp, "(已处理).xls");
    }

    fp = fopen(newfilename, "a+");
    if(fp == NULL)
    {
        printf("创建文件失败\n");
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
    printf("|                                     声明                                            |\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*  已知BUG: ---------->                          version:1.3.0                        *\n");
    printf("*  I  :  最大支持 表格行:%4d 列:%4d                                                 *\n", MAX_EXCEL_LINE, MAX_EXCEL_ROW);
    printf("*  II :  出现错误后检查后关闭重新打开即可                                             *\n");
    printf("*  III:  如后续效果好联系作者进行优化更新                                             *\n");
    printf("*  IV :  auther @KimAlittleStar By 2018.9.13                                          *\n");
    printf("*  V  :  插件由纯C语言制作 有兴趣者联系江理电子协会负责人即可                         *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("|                                      使用声明                                       |\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*   1.本作品 适用于 江西理工大学南昌校区成绩导入格式 的表格计算绩点分                 *\n");
    printf("*   2.先将原始表格另存为 csv 格式文件                                                 *\n");
    printf("*   3.Excel->另存为->其他格式->*.csv //保存 千万要保存                                *\n");
    printf("*   4.将本程序复制到 csv 文件所在文件目录下                                           *\n");
    printf("*   5.输入文件名字 即可得到处理后的表格                                               *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("*   6.表中标分隔符 %-20s                                               *\n", TABLE_SWITCH);
    printf("*   7.在内表中插入一行单独的 内表分隔符 即可实现多表同时计算                          *\n");
    printf("*   8.按回车确认                                                                      *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    while (getchar() != '\n')
    {
    }
    printf(" ------------------------------------------------------------------------------------\n");
    printf("* 统计周期确认：--------->                                                           *\n");
    printf("*       1.一学期统计                                                                 *\n");
    printf("*       2.一学年统计                                                                 *\n");
    printf(" ------------------------------------------------------------------------------------\n");
    printf("按下数字键回车后继续\n");
    c = getchar();
    while (c < '1' || c > '2')
    {
        c = getchar();
    } /*!< 确保输入的东西一定是在选项中  */
    while (getchar() != '\n')
        ; /*!< 确保读取到最后一个 回车 信号 */
    if (c == '2')
    {
        judgeGrand = judgeGrant_year;
        printf("您选择了 学年统计模式 .\n");
    }
    else
    {
        judgeGrand = judgeGrant_halfyear;
        printf("您选择了 学期统计模式 .\n");
    }
    return c;
}

FILE *getFile(int8 *filename)
{
    FILE *fp;
    int8 *ptemp;
    if (*filename == '\0')
    {
        printf(" 输入文件名: ");
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
    fp = fopen(filename, "r"); // fp指针指向文件头部
    return fp;
}

void CLOSE_ALL(EXCEL *excel, DOUBLE *grand, int8 *filename)
{
    /*!< 处理文件名语句过程 */
    int8 strlenth;
    int8 *realname = filename;
    strlenth = strlen(filename);
    realname += (strlenth - 1);
    while (*realname != '\\' && realname != filename)
    {
        realname--;
    }
    /*!< 处理完毕 */
    printf(" ------------------------------------------------------------------------------------\n");
    printf("* 文件:%68s          *\n", realname);
    printf("*                                   处理完成                                         *\n");
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

    sprintf(commentBuff,"title %s","江西理工大学一键获得平均绩点");
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

