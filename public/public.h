#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include <vector>
#include <sstream>

const unsigned int DATA_LENGTH = 1024 * 10;

std::string Encryption(const std::string& pwd);

std::string Decryption(const std::string& pwd);

bool VerifyCode(const std::string& code, const std::string& code_verify);

void ToLow(std::string& code);

std::string CombineString(char** be_combined, const int size);

void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

void SplitString(const char* be_converted, const char separator, char** dest, int& size);

std::string GetSystemTime();

std::string toDbString(const std::string& src);

std::string dbJoin(const std::vector<long long>& srcList);

std::string dbJoin(const std::vector<std::string>& srcList);

bool checkValid(const std::string& src);

bool checkSqlValid(const std::string& src);

#endif  //__PUBLIC_H__
