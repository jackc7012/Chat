#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include <vector>
#include <sstream>

// 文本网络通信数据量长度
const unsigned int DATA_LENGTH = 1024 * 1;

// 传送文件网络通信数据量长度
const unsigned int FILE_DATA_LENGTH = 1024 * 10;

const std::string VERIFY_CODE = "abcdefghjklmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ123456789";

typedef std::vector<std::string>    DataRecordLine;
typedef std::vector<DataRecordLine> DataRecords;

// 密码加密
std::string Encryption(const std::string& pwd);

// 密码解密
std::string Decryption(const std::string& pwd);

// 验证码验证
bool VerifyCode(const std::string& code, const std::string& code_verify);

// 字符小写
void ToLow(std::string& code);

// 字符串组合
std::string CombineString(const DataRecords& dataRecords);

// 分割字符串
void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

void SplitString(const char* be_converted, const char separator, char** dest, int& size);

// 获取系统时间
std::string GetSystemTime();

// 
std::string toDbString(const std::string& src);

std::string dbJoin(const std::vector<long long>& srcList);

std::string dbJoin(const std::vector<std::string>& srcList);

// sql注入过滤
bool checkValid(const std::string& src);

bool checkSqlValid(const std::string& src);

// 字符串去除收尾空格
std::string trim(const std::string& src);

#endif  //__PUBLIC_H__
