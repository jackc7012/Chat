#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include <vector>
#include <sstream>

// �ı�����ͨ������������
const UINT32 DATA_LENGTH = 1024 * 1;

// �����ļ�����ͨ������������
const UINT32 FILE_DATA_LENGTH = 1024 * 10;

const char COMBINE_ONE_INFO = '&';
const char COMBINE_INFOS = ';';
const char COMBINE_FILE = '_';

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
std::string CombineString(const DataRecords& dataRecords, const char sep1 = COMBINE_ONE_INFO, const char sep2 = COMBINE_INFOS);

// �ָ��ַ���
void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

void SplitString(const char* be_converted, const char separator, char** dest, int& size);

// ��ȡϵͳʱ��
std::string GetSystemTime(const int mode = 0);

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
