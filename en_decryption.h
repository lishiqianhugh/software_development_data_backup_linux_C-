#ifndef EN_DECRYPTION_H
#define EN_DECRYPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <openssl/aes.h>

#define AESKEY "df98b715d5c6ed2b25817b6f255411a1"	//HEX密钥
#define AESIV "2841ae97419c2973296a0d4bdfe19a4f"	//HEX初始向量




//将文本形式的HEX串进行转换
unsigned char* str2hex(char *str)
 {
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len%2) == 0);
    ret = (unsigned char *)malloc(str_len/2);
    for (i =0;i < str_len; i = i+2 )
    {
        sscanf(str+i,"%2hhx",&ret[i/2]);
    }
    return ret;
}

unsigned char *getfileall(char *fname)
{
    FILE *fp;
    char *str;
    char txt[1000];
    int filesize;
    if ((fp=fopen(fname,"r"))==NULL){
        printf("打开文件%s错误\n",fname);
        return NULL;
    }

    fseek(fp,0,SEEK_END);

    filesize = ftell(fp);
    str=(char *)malloc(filesize);
    str[0]=0;

    rewind(fp);
    while((fgets(txt,1000,fp))!=NULL){
        strcat(str,txt);
    }
    fclose(fp);
    return (unsigned char*)str;
}

void *putsfileall(char *fname, char *txt)
{
    FILE *fp;
    if ((fp=fopen(fname,"w"))==NULL){
        printf("打开文件%s错误\n",fname);
        return NULL;
    }
    fputs(txt,fp);
    fclose(fp);
}

//int test()
//{
//    AES_KEY encryptkey;
//    AES_KEY decryptkey;

//    unsigned char *key;
//    unsigned char *stdiv;

//    key = str2hex(AESKEY);
//    stdiv = str2hex(AESIV);
//    AES_set_encrypt_key(key,128,&encryptkey);
//    AES_set_decrypt_key(key,128,&decryptkey);

//    unsigned char plain_text [32];

//    memcpy(plain_text, "AES encrypt in openssl demo", 27);
//    memset(plain_text + 27, 0, 5);
//    //需要将加密区块长度填充为16字节整数倍，此处使用zero-padding，即末尾全用0填充
//    printf("plain_text: ");
//    for(int i = 0; i < 32; i++)
//    {
//        printf("%02X", plain_text[i]);
//    }
//    printf("\n");

//    unsigned char encrypted_text [32];

//    memset(encrypted_text, 0, 32);
//    unsigned char tmpiv[16];
//    memcpy(tmpiv, stdiv, 16);
//    AES_cbc_encrypt(plain_text, encrypted_text, 32, &encryptkey, tmpiv, AES_ENCRYPT);
//    //初始向量这个参数每次使用都会将其改变，有兴趣的话可以把调用前后这个地址下的内容打印出来。所以如果要多次加密且每次使用固定的初始向量，可以先用tmpiv接收

//    printf("encrypted_text: ");
//    for(int i = 0; i < 32; i++)
//    {
//        printf("%02X", encrypted_text[i]);
//    }
//    printf("\n");


//    unsigned char decrypted_text [32];

//    memset(decrypted_text, 0, 32);
//    memcpy(tmpiv, stdiv, 16);
//    AES_cbc_encrypt(encrypted_text, decrypted_text, 32, &decryptkey, tmpiv, AES_DECRYPT);

//    printf("decrypted_text: ");
//    for(int i = 0; i < 32; i++)
//    {
//        printf("%02X", decrypted_text[i]);
//    }
//    printf("\n");

//    return 0;

//}

int encrypt(char *src_path, char *dest_path)
{
    unsigned char *plain_text;
    //read data from source file
//    FILE *ifp, *ofp;
//    ifp = fopen(src_path, "r");
//    if (ifp == NULL)
//    {
//        printf("打开文件失败:%s\n",src_path);
//        return 0; //如果打开失败，则输出错误信息
//    }
//    ofp = fopen(dest_path, "w");
//    if (ofp == NULL)
//    {
//        printf("打开文件失败:%s\n",dest_path);
//        return 0; //如果打开失败，则输出错误信息
//    }

    plain_text = getfileall(src_path);

    AES_KEY encryptkey;

    unsigned char *key;
    unsigned char *stdiv;

    AES_set_encrypt_key(key,128,&encryptkey);

    key = str2hex(AESKEY);
    stdiv = str2hex(AESIV);

    unsigned char encrypted_text [32];

    memset(encrypted_text, 0, 32);
    unsigned char tmpiv[16];
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(plain_text, encrypted_text, 32, &encryptkey, tmpiv, AES_ENCRYPT);
    //初始向量这个参数每次使用都会将其改变，有兴趣的话可以把调用前后这个地址下的内容打印出来。所以如果要多次加密且每次使用固定的初始向量，可以先用tmpiv接收

    //write data to destination file
    putsfileall(dest_path,(char *)encrypted_text);


//    printf("encrypted_text: ");
//    for(int i = 0; i < 32; i++)
//    {
//        printf("%02X", encrypted_text[i]);
//    }
//    printf("\n");
    return 0;
}

int decrypt(char *src_path, char *dest_path)
{
    unsigned char *encrypted_text;
    //read data from source file



    AES_KEY decryptkey;

    unsigned char *key;
    unsigned char *stdiv;

    AES_set_decrypt_key(key,128,&decryptkey);

    key = str2hex(AESKEY);
    stdiv = str2hex(AESIV);

    unsigned char decrypted_text [32];

    memset(decrypted_text, 0, 32);
    unsigned char tmpiv[16];
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(encrypted_text, decrypted_text, 32, &decryptkey, tmpiv, AES_DECRYPT);

    //write data to destination file



//    printf("decrypted_text: ");
//    for(int i = 0; i < 32; i++)
//    {
//        printf("%02X", decrypted_text[i]);
//    }
//    printf("\n");
    return 0;
}

#endif // EN_DECRYPTION_H
