
#include <iostream>
#include "windows.h"
#include <string>
#include <fstream>
#include <queue>
#include <math.h>

using namespace std;

class Node {
private:


public:
    char Ch;
    int Freq;
    Node* Left, * Right;
    Node(char Ch, int Freq, Node* Left, Node* Right) :Ch(Ch), Freq(Freq), Left(Left), Right(Right) {}
    Node(const Node& rhs) {
        this->Ch = rhs.Ch;
        this->Freq = rhs.Freq;
        this->Left = rhs.Left;
        this->Right = rhs.Right;
    }

    bool isLeaf() { return (Left == NULL) && (Right == NULL); }
    char ch() { return Ch; }
    int freq() { return Freq; }
    Node left() { return *Left; }
    Node right() { return *Right; }
    bool operator<(const Node& other) const {
        return this->Freq > other.Freq;
    }
};
class Huffman {
private:


    //构建Huffman树
    static Node buildTrie(int* freq) {
        //创建一个最小堆优先队列pq，把所有频数大于1的字符创造一个结点并且放入pq。
        priority_queue<Node> pq;
        for (int i = 0; i < 256; i++) {
            unsigned char iChar = i;
            if (freq[iChar] > 0) {
                //printf("%c %d\n",iChar,freq[iChar]);
                Node* temp = new Node(iChar, freq[iChar], NULL, NULL);
                pq.push(*temp);
                delete temp;
            }
        }
        //到pq里的结点数大于1的时候，就弹出两个最小的结点，合并，并放入。知道最后只剩一个结点就为Huffman树

        while (pq.size() > 1) {
            Node* left = new Node(pq.top());
            pq.pop();
            Node* right = new Node(pq.top());
            pq.pop();
            Node* parent = new Node(0, left->freq() + right->freq(), left, right);
            pq.push(*parent);
            delete parent;
        }
        return pq.top();
    }

    //用Huffman树构建编码
    static void buildCode(string* st, Node x, string s) {
        /************************************************
         * s是Huffman编码字符串
         * 如果不是叶子结点，就递归调用
         * 如果往左走，就让s+‘0’;如果往右走，就让s+'1'
         * 如果是叶子结点，就可以把x结点的字符的编码保存到st中
         ************************************************/
        if (!x.isLeaf()) {
            buildCode(st, x.left(), s + '0');
            buildCode(st, x.right(), s + '1');
        }
        else {
            st[x.ch()] = s;
        }
    }

    //将字符串的长度转换成8的倍数
    static string to8(string s) {
        // 将st的长度补成8的倍数
        int size = s.length();
        int mod = 8 - (size % 8);
        string bu = "";
        for (int i = 0; i < mod; i++)
            bu += "0";
        s += bu;
        return s;
    }

    //字符串为01串，把每个字节的’0‘、’1‘当做bit，然后转换为byte
    static unsigned char getByte(string s) {
        unsigned char now = 0;
        for (int i = 0;i < 8;i++) {
            if (s[i] == '1')now++;
            if (i < 7)now <<= 1;
        }
        return now;
    }

    //按照特定规则将Huffman树写入文件
    static void writeTrie(Node x, ofstream &output) {
        //ofstream output(filename.c_str(), ios::out | ios::app | ios::binary);
        //如果是叶子结点，就把true写入，并且把x结点的字符写入文件
        if (x.isLeaf()) {
            bool temp = true;
            output.write((char*)&temp, sizeof(bool));
            char xtemp = x.ch();
            output.write((char*)&xtemp, sizeof(char));
            return;
        }
        //否则如果不是叶子结点，就只把false写入文件
        bool temp = false;
        output.write((char*)&temp, sizeof(bool));
        //output.close();
        writeTrie(x.left(), output);
        writeTrie(x.right(), output);
    }
    static void dfs(Node* now) {
        if (!now)return;
        dfs(now->Left);
        dfs(now->Right);

    }
    static Node* readTrie(ifstream& input, char* filename) {
        //ifstream input(filename.c_str(),ios::out | ios::app | ios::binary);
        bool is_leaf = false;
        input.read((char*)&is_leaf, sizeof(bool));
        if (is_leaf) {
            char x;
            input.read((char*)&x, sizeof(char));
            Node* now = new Node(x, 0, nullptr, nullptr);
            return now;
        }
        Node* now = new Node(0, 0, readTrie(input, filename), readTrie(input, filename));
        return now;
    }
public:

    Huffman() {}
    static int expand(char* input, char* filename) {
        FILE* ifp;
        int len = 0;
        ifp = fopen(input, "rb"); //打开原始文件
        if (ifp == NULL) {
            printf("打开文件失败:%s\n", input);
            return 0; //如果打开失败，则输出错误信息
        }
        ifstream ifs(input, ios::in | ios::app | ios::binary);
        ofstream output(filename);
        bool isroot=0;
        ifs.read((char *)&isroot,sizeof(bool));
        if(isroot){
            char tmp;
            ifs.read((char *)&len,sizeof(int));
            ifs.read((char *)&tmp,sizeof(tmp));
            for(int i=1;i<=len;i++){
                output<<tmp;
            }
            return 1;
        }
        Node* root = readTrie(ifs, input);
        Node* now = root;
        
        ifs.read((char *)&len,sizeof(int));
        //ifs.read((char*)&len, sizeof(int));
        unsigned char opt;
        bool o;
        for (int i = 1;i <= len;i++) {
            if (i % 8 == 1) {
                ifs.read((char*)&opt, sizeof(char));
            }
            bool o = ((opt) >> ((8 - i % 8) % 8)) & 1;
            if (!o) {
                now = now->Left;
            }
            else now = now->Right;
            if (now->isLeaf()) {
                output << now->Ch;
                now = root;
            }
        }

        return 1;

    }
    //用Huffman编码压缩
    static int compress(char* input, char* filename) {
        //创建freq数组，用来保存每个字符出现频数，并且扫描每个字符，统计频数
        FILE* ifp;
        int len = 0;
        ifp = fopen(input, "rb"); //打开原始文件
        if (ifp == NULL) {
            printf("打开文件失败:%s\n", filename);
            return 0; //如果打开失败，则输出错误信息
        }

        int* freq = new int[256];
        for (int i = 0; i < 256; i++)
            freq[i] = 0;
        // for (int i = 0; i < len; i++)
        //     freq[input[i]]++;
        char c;
        while (!feof(ifp)) {
            fread(&c, 1, 1, ifp);
            freq[c]++; //读文件，统计字符出现次数
            len++; //记录文件的字符总数
        }
        freq[c]--;
        len--;


        //调用buildTrie函数构造Huffman树
        Node root = buildTrie(freq);
        
        //创建一个symbol table并调用buildCode用来表示对应字符的Huffman编码
        string* st = new string[256];
        unsigned char* tb = new unsigned char[256];
        buildCode(st, root, "");
        
        //把用‘0’、‘1’表示的编码字符转转化为字节
        // for (int i = 0; i < 256; i++) {
        //     //cout<<st[i]<<endl;
        //     st[i] = to8(st[i]);
        //     tb[i] = getByte(st[i]);

        // }
        //dfs(&root);
        //由于需要Huffman编码才能解码，所以用writeTrie把Huffman编码写入文件以便解码
        ofstream output(filename, ios::out | ios::app | ios::binary);
        bool isroot=root.isLeaf();
        output.write((char *)&isroot,sizeof(bool));
        if(isroot){
            output.write((char *)&len, sizeof(int));
            output.write((char *)&root.Ch,sizeof(char));
            return 1;
        }
        writeTrie(root, output);
        output.flush();
        //以二进制模式打开文件，逐个把对应字符的Huffman编码写入文件
        


        ifp = nullptr;
        len = 0;
        ifp = fopen(input, "rb"); //打开原始文件
        queue<unsigned char> finq;
        queue<unsigned char> q;
        while (!feof(ifp)) {
            char c;
            fread(&c, 1, 1, ifp);
            if (feof(ifp))break;
            string code = st[c];
            for (auto x : st[c]) {
                q.push(x);
                len++;
            }
            if (q.size() >= 8) {
                unsigned char now = 0;
                for (int i = 1;i <= 8;i++) {
                    char tmp = q.front();q.pop();
                    if (tmp == '1')now++;
                    if(i<8)now <<= 1;
                }
                finq.push(now);
            }
            //cout<<code.length()<<" "<<sizeof(char)<<endl;
            //cout<<(int)code<<" "<<code<<'\n';
        }
        if (!q.empty()) {
            int t = 8 - q.size();
            unsigned char now = 0;
            while (!q.empty()) {
                char tmp = q.front();q.pop();
                if (tmp == '1')now++;
                if(!q.empty())now <<= 1;
            }
            now <<= t;
            finq.push(now);
        }
        output.write((char*)&len, sizeof(int));
        while (!finq.empty()) {
            unsigned char now=finq.front();
            output.write((char *)&now,sizeof(char));
            finq.pop();
        }
        delete freq;
        //delete st;
        output.close();
        return 1;
    }
};


// int main() {

//     freopen("Ranulx.in", "r", stdin);
//     freopen("Ranulx.out", "w", stdout);
//     cout << "enter the path:";
//     char path[500];
//     cin >> path;
//     cout << "enter the filename:";
//     char filename[500], output[500];
//     cin >> filename;
//     solver.compress(path, filename);
//     ifstream ifs(filename, ios::in | ios::app | ios::binary);
//     //solver.readTrie(ifs,filename);
//     cin >> output;
//     solver.expand(filename, output);
//     return 0;

// }