#ifndef BACKUP_H
#define BACKUP_H

/*
 * backup.cpp
 *
 *  Created on: Sep 21, 2021
 *      Author: lishiqian
 */


//  /home/lishiqian/eclipse-workspace/backup/src/data
//  /home/lishiqian/eclipse-workspace/backup/src/data_cp
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
/*
//文件夹复制采用dfs或者bfs进行搜索是等价的
void search_dfs(char mode ,char *src_path, char *dest_path);
//复制文件
void copy_file(const char *src_file, const char *dst_file);
//复制文件夹
void copy_dir(const char *src_dir, const char *dst_dir);
//复制软链接
void copy_sln(const char *src_file, const char *dst_file);
//修改文件属性与源文件保持一致
void changeAttr(const char *src, const char *dst);

int compress(const char *filename,const char *outputfile);
int uncompress(const char *filename,const char *outputfile);
int backup(char mode, char *src_path, char *dest_path);*/

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
    time_buf[1].tv_sec = attr_of_src.st_mtime;
    time_buf[0].tv_sec = attr_of_src.st_atime;
    if(lutimes(dst, time_buf) == -1)
    {
        printf("%s\n", dst);
        perror("lutimes");
    }

    struct utimbuf tbuf;
    tbuf.actime = attr_of_src.st_atime;
    tbuf.modtime = attr_of_src.st_mtime;
    utime(dst, &tbuf);

    struct stat dst_attr_of_src;
    lstat(dst, &dst_attr_of_src);
    if (dst_attr_of_src.st_mtime != attr_of_src.st_mtime)
        printf("%s : %d\n", dst, attr_of_src.st_mtime);
}

void copy_file(const char *src_file, const char *dest_file)
{
    int src_fd = open(src_file, O_RDONLY);
    int dest_fd = creat(dest_file, O_WRONLY);

    unsigned char buf[1024];
    while (read(src_fd, buf, sizeof(buf)) > 0)
    {
        write(dest_fd, buf, sizeof(buf));
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
            else if(mode=='z') compress(src_file, dest_file);
            else if(mode=='u') uncompress(src_file, dest_file);
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
    }
}

int backup(char mode, char *src_path, char *dest_path)
{
    if(mode=='n'){
        strcpy(src_path,"/home/lishiqian/eclipse-workspace/backup/src/data");
        strcpy(dest_path,"/home/lishiqian/eclipse-workspace/backup/src/data_cp");
    }
    else if(mode=='z'){
        strcpy(src_path,"/home/lishiqian/eclipse-workspace/backup/src/data");
        strcpy(dest_path,"/home/lishiqian/eclipse-workspace/backup/src/data_zip");
    }
    else if(mode=='u'){
        strcpy(src_path,"/home/lishiqian/eclipse-workspace/backup/src/data_zip");
        strcpy(dest_path,"/home/lishiqian/eclipse-workspace/backup/src/data_unzip");
    }

//    if (argc < 3)
//    {
//        printf("No file or directory specifiedn");
//        exit(-1);
//    }
//    if (argc > 3)
//    {
//        printf("Too many argumentsn");
//        exit(-1);
//    }

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
    }

    return 0;
}


#endif // BACKUP_H
