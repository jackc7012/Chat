#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include <vector>
#include <sstream>

// �ı�����ͨ������������
const unsigned int DATA_LENGTH = 1024 * 1;

// �����ļ�����ͨ������������
const unsigned int FILE_DATA_LENGTH = 1024 * 10;

const std::string VERIFY_CODE = "abcdefghjklmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ123456789";

typedef std::vector<std::string>    DataRecordLine;
typedef std::vector<DataRecordLine> DataRecords;

// �������
std::string Encryption(const std::string& pwd);

// �������
std::string Decryption(const std::string& pwd);

// ��֤����֤
bool VerifyCode(const std::string& code, const std::string& code_verify);

// �ַ�Сд
void ToLow(std::string& code);

// �ַ������
std::string CombineString(const DataRecords& dataRecords);

// �ָ��ַ���
void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

void SplitString(const char* be_converted, const char separator, char** dest, int& size);

// ��ȡϵͳʱ��
std::string GetSystemTime();

// 
std::string toDbString(const std::string& src);

std::string dbJoin(const std::vector<long long>& srcList);

std::string dbJoin(const std::vector<std::string>& srcList);

// sqlע�����
bool checkValid(const std::string& src);

bool checkSqlValid(const std::string& src);

// �ַ���ȥ����β�ո�
std::string trim(const std::string& src);

#endif  //__PUBLIC_H__
