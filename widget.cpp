//#pragma execution_character_set("utf-8")

#include "widget.h"
#include "ui_widget.h"

#include<QApplication>
#include<QMouseEvent>
#include<QDebug>
#include<cstdlib>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <time.h>
#include <sys/time.h>

#include"huffman.h"
//#include"en_decryption.h"

using namespace std;

int backup(char mode, char *src_path, char *dest_path);

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);

    connect(ui->selectfile, &QPushButton::clicked, this, &Widget::selectSrcFilename);

    connect(ui->selectdest, &QPushButton::clicked, this, &Widget::selectDestFilename);

    connect(ui->backup, &QPushButton::clicked, this, &Widget::Backup);

    connect(ui->compress, &QPushButton::clicked, this, &Widget::Compress);

    connect(ui->uncompress, &QPushButton::clicked, this, &Widget::Uncompress);

    connect(ui->pack, &QPushButton::clicked, this, &Widget::Pack);

    connect(ui->unpack, &QPushButton::clicked, this, &Widget::Unpack);

    connect(ui->encrypt, &QPushButton::clicked, this, &Widget::Encrypt);

    connect(ui->decrypt, &QPushButton::clicked, this, &Widget::Decrypt);

}

Widget::~Widget()
{
    delete ui;
}

QString Widget::selectSrcFilename()
{
    QFileDialog *fileDialog = new QFileDialog(this);//创建一个QFileDialog对象
    fileDialog->setWindowTitle(tr("OPening directory"));//设置文件保存对话框的标题
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
//    fileDialog->setNameFilter(tr("Images(*.dcm *.jpg *.jpeg *.bmp *.mhd *.raw)"));
    fileDialog->setFileMode(QFileDialog::Directory);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //还有另一种形式QFileDialog::List，这个只是把文件的文件名以列表的形式显示出来
    // fileDialog->setGeometry(10,30,300,200);//设置文件对话框的显示位置

    if(fileDialog->exec() == QDialog::Accepted)
    {
        src_path = fileDialog->selectedFiles()[0];//得到用户选择的第一个文件名
        qDebug() << "Source path: " << src_path <<endl;
        // write to text block
        ui->src_text->setText(src_path);
        fileDialog->close();
        return src_path;
        }
    else
        return 0;
 }


QString Widget::selectDestFilename()
{
    QFileDialog *fileDialog = new QFileDialog(this);//创建一个QFileDialog对象
    fileDialog->setWindowTitle(tr("OPening directory"));//设置文件保存对话框的标题
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
//    fileDialog->setNameFilter(tr("Images(*.dcm *.jpg *.jpeg *.bmp *.mhd *.raw)"));
//    fileDialog->setFileMode(QFileDialog::Directory);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //还有另一种形式QFileDialog::List，这个只是把文件的文件名以列表的形式显示出来
    // fileDialog->setGeometry(10,30,300,200);//设置文件对话框的显示位置

    if(fileDialog->exec() == QDialog::Accepted)
    {
        dest_path = fileDialog->selectedFiles()[0];//得到用户选择的第一个文件名
        qDebug() << "Destination path: " << dest_path <<endl;
        // write to text block
        ui->dest_text->setText(dest_path);
        fileDialog->close();
        return dest_path;
        }
    else
        return 0;
 }

void Widget::Backup()
{
    char mode='n';
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
    backup(mode, src_char, dest_char);
}

void Widget::Compress()
{
    char mode='z';
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
//    backup(mode, src_char,dest_char);
    char buffer[100];
    chdir(src_char);
    chdir("..");
    sprintf(buffer,"zip -r %s %s",dest_char, strrchr(src_char,'/')+1);
    printf("%s",buffer);
    system(buffer);
}

void Widget::Uncompress()
{
    char mode='u';
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
//    backup(mode, src_char,dest_char);
    char buffer[100];
    sprintf(buffer,"unzip %s -d %s",src_char, dest_char);
    system(buffer);
}


void Widget::Pack()
{
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
    char buffer[100];
    sprintf(buffer,"tar -cvf %s -C %s",dest_char, src_char);
    //replace '/' to ' '
    for(int i=strlen(buffer)-1;i>=0;i--)
    {
        if(buffer[i]=='/'){
            buffer[i] = ' ';
            break;
        }
    }
    system(buffer);
}

void Widget::Unpack()
{
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
    char buffer[100];
    sprintf(buffer,"tar -xvf %s -C %s",src_char, dest_char);
    system(buffer);
}

void Widget::Encrypt()
{
    char mode='e';
    string src = (ui->src_text->toPlainText()).toStdString();
    string dest = (ui->dest_text->toPlainText()).toStdString();
    char *src_char = (char*)src.data();
    char *dest_char = (char*)dest.data();
    backup(mode, src_char,dest_char);
}

void Widget::Decrypt()
{

}


//修改路径
void change_path(char *src, char *cat)
{
    strcat(src, (char *)"/");
    strcat(src, cat);
}

//char mode='u'; //'n':normal  'z':zip  'u':unzip

void changeAttr(const char *src, const char *dst)
{
    struct stat attr_of_src;
    lstat(src, &attr_of_src);
    //修改文件属性


    chmod(dst, attr_of_src.st_mode);
    //修改文件用户组
    chown(dst, attr_of_src.st_uid, attr_of_src.st_gid);

    //修改文件访问、修改时间
    struct timeval time_buf[2];
    time_buf[1].tv_sec = attr_of_src.st_mtim.tv_sec;
    time_buf[1].tv_usec = attr_of_src.st_mtim.tv_nsec/1000;
    time_buf[0].tv_sec = attr_of_src.st_atim.tv_sec;
    time_buf[0].tv_usec = attr_of_src.st_atim.tv_nsec/1000;
    if(lutimes(dst, time_buf) == -1)
    {
        printf("%s\n", dst);
        perror("lutimes");
    }

//    struct utimbuf tbuf;
//    tbuf.actime = attr_of_src.st_atime;
//    tbuf.modtime = attr_of_src.st_mtime;
//    utime(dst, &tbuf);

    struct stat dst_attr_of_src;
    lstat(dst, &dst_attr_of_src);
    if (dst_attr_of_src.st_mtime != attr_of_src.st_mtime)
        printf("%s : %ld\n", dst, attr_of_src.st_mtime);
}

void copy_file(const char *src_file, const char *dest_file)
{
    int src_fd = open(src_file, O_RDONLY);
    int dest_fd = creat(dest_file, O_WRONLY);

    char buf;
    while (read(src_fd, &buf, sizeof(buf)) > 0)
    {
        write(dest_fd, &buf, sizeof(buf));
    }

    changeAttr(src_file, dest_file);

    close(src_fd);
    close(dest_fd);
}


void copy_dir(const char *src_dir, const char *dst_dir)
{
    mkdir(dst_dir, 0777);
    changeAttr(src_dir, dst_dir);
}

void copy_sln(const char *src_file, const char *dst_file)
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int len = 0;
    if ((len = readlink(src_file, buf, sizeof(buf))) > 0)
    {
        printf("%s\n", buf);
        if (symlink(buf, dst_file) == -1)
        {
            perror("symlink");
        }
    }
    changeAttr(src_file, dst_file);
}


void search_dfs(char mode, char *src_path, char *dest_path)
{
    printf("Searching directory:    %s\n", getcwd(NULL, 0));
    DIR *src_dir = opendir(src_path);
    DIR *dest_dir = opendir(dest_path);
    struct dirent *entry = NULL;
    struct stat state_of_entry;
    while ((entry = readdir(src_dir)) != NULL)
    {
//    	chdir(src_path);
        char entry_path[100];
        strcpy(entry_path, src_path);
        change_path(entry_path, entry->d_name);
        lstat(entry_path, &state_of_entry);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        printf("entry->d_name: %s \n", entry->d_name);
        if (S_ISLNK(state_of_entry.st_mode)) //符号链接
        {
            char src_file[1024];
            char dest_file[1024];
            strcpy(src_file, src_path);
            change_path(src_file, entry->d_name);
            strcpy(dest_file, dest_path);
            change_path(dest_file, entry->d_name);
            // printf("src file: %s\n", src_file);
            // printf("dest file: %s\n", dest_file);
            copy_sln(src_file, dest_file);
        }
        else if (S_ISREG(state_of_entry.st_mode)) //普通文件
        {
            char src_file[1024];
            char dest_file[1024];
            strcpy(src_file, src_path);
            change_path(src_file, entry->d_name);
            strcpy(dest_file, dest_path);
            change_path(dest_file, entry->d_name);
            // printf("src file: %s\n", src_file);
            // printf("dest file: %s\n", dest_file);
            if(mode == 'n') copy_file(src_file, dest_file);
            else if(mode=='z') {compress(src_file, dest_file);
                //changeAttr(src_file, dest_file);
            }
            else if(mode=='u') {uncompress(src_file, dest_file);
                //changeAttr(src_file, dest_file);
            }
        }
        else if (S_ISDIR(state_of_entry.st_mode)) //目录
        {
            char src[1024];
            char dest[1024];
            strcpy(src, src_path);
            change_path(src, entry->d_name);
            strcpy(dest, dest_path);
            change_path(dest, entry->d_name);
            // printf("src dir: %s\n", src);
            // printf("dest dir: %s\n", dest);
            copy_dir(src, dest);
            search_dfs(mode, src, dest);
        }
        else //pipe file
        {
            char src_pipe[1024];
            char dest_pipe[1024];
            strcpy(src_pipe, src_path);
            change_path(src_pipe, entry->d_name);
            strcpy(dest_pipe, dest_path);
            change_path(dest_pipe, entry->d_name);
            char buffer[100];
            sprintf(buffer,"mkfifo %s", dest_pipe);
            system(buffer);
        }
    }
}

int backup(char mode, char *src_path, char *dest_path)
{
    char src[1024], dest[1024];
    char *current_dir = getcwd(NULL, 0);

    struct stat state_of_entry;
    lstat(src_path, &state_of_entry);
    if (S_ISDIR(state_of_entry.st_mode)) //目录
    {
        if (chdir(src_path)) //目录错误
        {
            perror("chdir");
            exit(-1);
        }
        strcpy(src, getcwd(NULL, 0)); //获取源文件夹绝对路径
        chdir(current_dir);
        lstat(dest_path, &state_of_entry);
        if (S_ISDIR(state_of_entry.st_mode)) //目录
        {
            if (chdir(dest_path)) //目录错误
            {
                perror("chdir");
                exit(-1);
            }
            strcpy(dest, getcwd(NULL, 0)); //获取目标文件夹绝对路径
            chdir(current_dir);
            chdir(dest);
            // printf("SRC: %sn", src);
            // printf("DEST: %sn", dest);
            chdir(src);
            search_dfs(mode, src, dest);
        }
        else
        {
            printf("error. No destination directory.n");
            exit(-1);
        }
    }

    else //文件直接复制
    {
        char dest[1024];
        lstat(dest_path, &state_of_entry);
        if (S_ISDIR(state_of_entry.st_mode)) //目录
        {
            strcpy(dest, getcwd(NULL, 0)); //获取目标文件夹绝对路径
        }
        else
        {
            strcpy(dest, "./");
            strcat(dest, dest_path);
        }
        if(mode == 'n') copy_file(src_path, dest_path);
        else if(mode=='z') compress(src_path, dest_path);
        else if(mode=='u') uncompress(src_path, dest_path);
//        else if(mode=='e') encrypt(src_path, dest_path);
//        else if(mode=='d') decrypt(src_path, dest_path);
    }

    return 0;
}

