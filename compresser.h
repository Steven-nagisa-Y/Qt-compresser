#ifndef COMPRESSER_H
#define COMPRESSER_H
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <ctime>
using namespace std;

struct element
{
    int weight;
    int lchild, rchild, parent;
};
void huffman_tree(element *hufftree, int *w, int n);       //构造叶子节点长为n的哈夫曼树
void select(element *hufftree, int &a, int &b, int n);     //找出权值最小的两个节点
void huffman_code(element *hufftree, int n, string *code); //对哈夫曼树进行编码
void fun(char *str, int len, int &n);                      //对文件的二进制串的数量进行统计 返回含有各个字符频率的数组
string elc(int n);                                         //根据index生成等长二进制编码
void hfmCompress();                                        //压缩函数
void decompress();                                         //解压函数
int conversion(string code);                               //二进制码转十进制
string i_to_s(int l);
int s_to_i(string str);
int getprefix(char *str, int l, string &type, int &len); //返回去除了前缀的新下标
string getstr(char *str, int begin, int l);
int dictionaries[256]; // dictionaries对应的是hufftree的下标 知hufftree下标可知哈夫曼编码code[dictionaries[index]]
int freq[256];
string equal_length_code[256]; //等长的二进制编码

void select(element *hufftree, int &a, int &b, int n)
{
    int t1, t2;
    for (int i = 0; i < 2 * n - 1; i++)
    {
        if (hufftree[i].parent == -1)
        {
            t1 = i;
            a = i;
            break;
        }
    }
    for (int i = 0; i < 2 * n - 1; i++)
    {
        if (hufftree[i].parent == -1 && i != a)
        {
            t2 = i;
            b = i;
            break;
        }
    }
    for (int i = 0; i < 2 * n - 1; i++)
    {
        if (hufftree[i].parent == -1 && i != a && (i < n || hufftree[i].lchild != -1)) //比较所有有权值的节点
            if (hufftree[i].weight < hufftree[a].weight)
                a = i;
    }
    if (a == b)
        b = t1;
    for (int i = 0; i < 2 * n - 1; i++)
    {
        if (hufftree[i].parent == -1 && i != a && i != b && (i < n || hufftree[i].lchild != -1))
            if (hufftree[i].weight < hufftree[b].weight)
                b = i;
    }
}

void huffman_tree(element *hufftree, int *w, int n)
{
    for (int i = 0; i < 2 * n - 1; i++)
    {
        hufftree[i].parent = -1;
        hufftree[i].lchild = -1;
        hufftree[i].rchild = -1;
    }
    for (int i = 0; i < n; i++)
        hufftree[i].weight = w[i];
    for (int i = n; i < 2 * n - 1; i++)
    {
        int a, b;
        select(hufftree, a, b, n);
        hufftree[a].parent = i;
        hufftree[b].parent = i;
        hufftree[i].lchild = a; //左最小
        hufftree[i].rchild = b;
        hufftree[i].weight = hufftree[a].weight + hufftree[b].weight;
    }
}

void huffman_code(element *hufftree, int n, string *code)
{
    for (int i = 0; i < n; i++)
    {
        int p = i;
        while (p != 2 * n - 2)
        {
            if (hufftree[hufftree[p].parent].lchild == p)
                code[i] = '0' + code[i];
            else
                code[i] = '1' + code[i];
            p = hufftree[p].parent;
        }
    }
}

void fun(char *str, int len, int &n)
{
    n = 0;

    for (int i = 0; i < len; i++) // ascii范围是-128-127
    {
        int ascii = str[i];
        if (ascii >= 0)
            freq[ascii]++;
        else
        {
            ascii = ascii * -1 + 127;
            freq[ascii]++;
        }
    }
    for (int index = 0; index < 256; index++) // index范围是0-255 经常需要转换
        if (freq[index] != 0)
            n++;
}

string elc(int n)
{
    int a[8];
    int i = 0;
    string str = "";
    while (n != 0)
    {
        a[i] = n % 2;
        n /= 2;
        i++;
    }

    for (n = i - 1; n >= 0; n--)
    {
        if (a[n] == 1)
            str += '1';
        else
            str += '0';
    }
    int l = str.length();
    if (l < 8)
        for (int k = 0; k < 8 - l; k++)
            str = '0' + str;
    return str;
}

void hfmCompress(string filename)
{
    int fl = filename.length();
    for (int i = 0; i < fl; i++)
        if (filename[i] == '\\')
        {
            filename.insert(i, "\\\\");
            i += 2;
        }
    for (int i = 0; i < 256; i++)
        equal_length_code[i] = elc(i);

    for (int i = 0; i < 256; i++)
        freq[i] = 0;

    for (int i = 0; i < 256; i++)
        dictionaries[i] = -1;

    string name;
    string type;
    for (int i = filename.length() - 1; i >= 0; i--)
        if (filename[i] == '.')
        {
            name = filename.substr(0, i);
            // cout<<name<<endl;
            type = filename.substr(i + 1);
            // cout<<type<<endl;
            break;
        }
    int n;
    ifstream f1;
    char *fn = (char *)filename.data();
    f1.open(fn, ifstream::binary);
    f1.seekg(0, ios::end);
    int l = f1.tellg();

    double l1 = l;
    f1.seekg(0, ios::beg);
    char *str = new char[l];
    f1.read(str, l);

    f1.close();
    fun(str, l, n);

    element *hufftree = new element[2 * n - 1]; //哈夫曼树共有2n-1个节点
    string *h_code = new string[n];             // ascii码对应的权值code[dictionaries[index]] index范围为0-255 0-127是正常的 128-255另作转换
    int *w = new int[n];

    int k = 0;
    for (int i = 0; i < 256; i++) //将零散分布的freq放入到整齐的w[k]
        if (freq[i] != 0)
        {
            w[k] = freq[i]; // w是权
            dictionaries[i] = k;
            k++;
        }

    huffman_tree(hufftree, w, n); //构建哈夫曼树
    /*for(int i=0;i<2*n-1;i++)
        cout<<hufftree[i].weight<<endl;*/

    huffman_code(hufftree, n, h_code); //根据哈夫曼树生成每个字符对应的哈夫曼编码
    /*for(int i=0;i<n;i++)
        cout<<h_code[i]<<endl;*/

    if (n == 1)
        h_code[0] = '0';

    // cout<<l<<endl;
    string code = "";
    for (int i = 0; i < l; i++) //将哈夫曼字符转化为哈夫曼编码并
    {
        int ascii = str[i];
        if (ascii >= 0)
            code += h_code[dictionaries[ascii]];
        else
            code += h_code[dictionaries[ascii * -1 + 127]];
    }
    // cout<<"step2\n";
    int len = code.length();
    // cout<<len<<endl;
    // cout<<len;
    if (len % 8 != 0) //不够八位的补上0
    {
        int complement = 8 - l % 8;
        for (int i = 0; i < complement; i++)
            code += '0';
    }
    // cout<<"step3\n";
    string newstr = ""; //压缩后的新字符串
    for (int i = 0; i * 8 < len; i++)
    {
        string s = "";
        for (int j = 0; j < 8; j++) //获得八位二进制码
            s += code[i * 8 + j];
        int index = conversion(s); //转化为十进制 八位二进制生成一个字节
        // cout<<index;
        if (index < 128)
            newstr += char(index);
        else
            newstr += char((index - 127) * -1);
    }
    string prefix;
    prefix += type + '|';
    prefix += i_to_s(l); //前缀 解压时用
    for (int i = 0; i < 256; i++)
    {
        if (freq[i] != 0)
        {
            prefix += '|'; //字符前后用||分隔
            if (i < 128)
                prefix += char(i);
            else
                prefix += char((i - 127) * -1);
            prefix += i_to_s(freq[i]);
        }
    }
    prefix += '|';
    prefix += "stop";
    newstr = prefix + newstr;

    // cout<<prefix<<"\n";
    l = newstr.length();
    double l2 = l;
    // cout<<endl;
    // cout<<l;

    string compFile = name + ".hfmc";
    char *lf = (char *)compFile.data();

    ofstream f2;
    f2.open(lf, ofstream::binary);
    const char *a = newstr.data();
    f2.write(a, l);
    f2.close();
    // cout<<"压缩完成\n压缩率为"<<l2/l1*100<<"%"<<endl;

    delete[] hufftree;
    delete[] w;
    delete[] h_code;
    delete[] str;
}

void decompress(string filename)
{
    clock_t start, end;
    int fl = filename.length();
    for (int i = 0; i < fl; i++)
        if (filename[i] == '\\')
        {
            filename.insert(i, "\\\\");
            i += 2;
        }

    for (int i = 0; i < 256; i++)
        equal_length_code[i] = elc(i);

    for (int i = 0; i < 256; i++)
        freq[i] = 0;

    for (int i = 0; i < 256; i++)
        dictionaries[i] = -1;

    string hc[256];

    string name;
    string type;
    for (int i = filename.length() - 1; i >= 0; i--)
    {
        if (filename[i] == '.')
        {
            name = filename.substr(0, i);
            break;
        }
    }
    // cout<<name<<endl;

    ifstream f1;
    char *fn = (char *)filename.data();
    f1.open(fn, ifstream::binary);
    f1.seekg(0, ios::end);
    int l = f1.tellg();
    f1.seekg(0, ios::beg);
    char *str = new char[l];
    f1.read(str, l);
    f1.close();

    int len; //字符的总数

    int begin = getprefix(str, l, type, len); // begin是新的下标

    string code = "";
    for (int i = begin; i < l; i++)
    {
        int ascii = str[i];
        if (ascii >= 0)
            code += equal_length_code[ascii];
        else
            code += equal_length_code[ascii * -1 + 127];
    }

    // cout<<code.length();
    int n = 0; //原哈夫曼树叶子节点个数
    for (int i = 0; i < 256; i++)
        if (freq[i] != 0)
            n++;

    element *hufftree = new element[2 * n - 1]; //哈夫曼树共有2n-1个节点
    string *h_code = new string[n];             // index范围为0-255 0-127是正常的 128-255另作转换
    int *w = new int[n];

    int k = 0;
    for (int i = 0; i < 256; i++) //将零散分布的freq放入到整齐的w[k]
        if (freq[i] != 0)
        {
            w[k] = freq[i]; // w是权
            dictionaries[i] = k;
            k++;
        }

    huffman_tree(hufftree, w, n); //构建哈夫曼树
    /*for(int i=0;i<2*n-1;i++)
        cout<<hufftree[i].weight<<endl;*/

    huffman_code(hufftree, n, h_code); //根据哈夫曼树生成每个字符对应的哈夫曼编码
    /*for(int i=0;i<n;i++)
        cout<<h_code[i]<<endl;*/

    if (n == 1)
        h_code[0] = '0';

    for (int i = 0; i < 256; i++)
        if (freq[i] != 0)
            hc[i] = h_code[dictionaries[i]];

    int max = 0;
    for (int i = 1; i < 256; i++)
        if (freq[i] > freq[max])
            max = i;
    unsigned int min_length = h_code[dictionaries[max]].length();
    // cout<<min_length;

    string newstr = "";
    int cl = code.length();
    int a = 0;

    string temp;
    bool found;

    start = clock();
    for (int k = 0; k < len; k++) // len原文件的字符数
    {
        temp = ""; //不断读取code中的二进制码 找到哈夫曼编码
        while (a < cl)
        {
            found = false;
            temp += code[a];
            if (temp.length() < min_length)
            {
                a++;
                continue;
            }

            for (int index = 0; index < 256; index++)
            {
                if (temp == hc[index])
                {
                    found = true;
                    if (index < 128)
                        newstr += char(index);
                    else
                        newstr += char((index - 127) * -1);
                    break;
                }
            }
            a++;
            if (found == true)
                break;
        }
    }
    end = clock();
    cout << end - start << endl;
    l = newstr.length(); //原文件长度
    const char *b = newstr.data();

    string compFile = name + '.' + type;
    char *lf = (char *)compFile.data();
    ofstream f2;
    f2.open(lf, ofstream::binary);
    f2.write(b, l);
    f2.close();

    delete[] str;
    delete[] w;
    delete[] hufftree;
    delete[] h_code;
}

int conversion(string code)
{
    int l = code.length();
    int result = 0;
    for (int i = 0; i < l; i++)
        if (code[i] == '1')
            result += pow(2, l - i - 1);
    return result;
}

string i_to_s(int l)
{
    string str = "";
    while (l >= 10)
    {
        str = char(l % 10 + 48) + str;
        l /= 10;
    }
    str = char(l + 48) + str;
    return str;
}

int s_to_i(string str)
{
    int n = 0;
    for (unsigned int i = 0; i < str.length(); i++)
        n = n * 10 + int(str[i] - 48);
    return n;
}

int getprefix(char *str, int l, string &type, int &len)
{
    string newstr;
    // cout<<l<<endl;
    int e = 0; //结束标志stop开始的地方
    for (int i = 0; i < l; i++)
        if (str[i] == 's' && str[i + 1] == 't' && str[i + 2] == 'o' && str[i + 3] == 'p')
            e = i;
    // cout<<str[e]<<str[e+1]<<str[e+2]<<str[e+3];
    int i = 0;
    int front, rear;
    for (i; i < e; i++)
        if (str[i] == '|')
        {
            rear = i;
            type = getstr(str, 0, rear);
            i++;
            break;
        }
    // cout<<type;
    front = rear + 1;
    for (i; i < e; i++)
        if (str[i] == '|')
        {
            rear = i;
            len = s_to_i(getstr(str, front, rear - front));
            break;
        }
    // cout<<len;

    // cout<<str[i+1];
    while (i < e)
    {
        if (str[i] == '|')
        {
            int w;
            i++;
            if (i >= e)
                break;
            int ascii = str[i];

            i++;
            front = i;
            for (i; i < e; i++)
            {
                if (str[i] == '|')
                {
                    rear = i;
                    w = s_to_i(getstr(str, front, rear - front));
                    break;
                }
            }
            if (ascii >= 0)
                freq[ascii] = w;
            else
                freq[ascii * -1 + 127] = w;
        }
    }
    return e + 4;
}

string getstr(char *str, int begin, int l)
{
    string s = "";
    for (int i = begin; i < begin + l; i++)
        s += str[i];
    return s;
}

#endif // COMPRESSER_H
