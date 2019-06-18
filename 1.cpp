/*Главный модуль программы*/
#include <iostream>
#include <cctype>
#include <codecvt>
#include <locale>
#include "ch.h"
using namespace std;
void check(const string& Text, const string& key, const bool destructCiphrText=false)
{
	try {
		locale loc("ru_RU.UTF-8");
		wstring_convert<codecvt_utf8<wchar_t>, wchar_t>codec;
		string ciphrText;
		string decryptedText;
		modAlphaCiphr ciphr(key);
		ciphrText = ciphr.encrypt(Text);
		ciphrText = ciphr.encrypt(Text);
		if (destructCiphrText) {
			wstring ws = ciphr.towstr(ciphrText);
			ws.front() = tolower(ws.front(),loc);
			ciphrText = ciphr.fromwstr(ws);
		}
		decryptedText = ciphr.decrypt(ciphrText);
		cout<<"key="<<key<<endl;
		cout<<Text<<endl;
		cout<<ciphrText<<endl;
		cout<<decryptedText<<endl;
	} catch (const ciphr_error & e) {
		cerr<<"Error: "<<e.what()<<endl;
	}
}
int main(int argc, char **argv)
{
	check("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ", "Лаба");
	check("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ","");
	check("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ", "ИКЛМ345");
	check("АбвгДеёжЗиклмНопрстУфхцчшщ ЪыЬЭюя","ТИМП");
	check("345","ИКЛМ");
	check("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ","ИБСТ",true);
	return 0;
}

/*Заголовочный файл*/
#pragma once
#include <vector>
#include <string>
#include <codecvt>
#include <locale>
#include <map>
class modAlphaCiphr
{
private:
	std::wstring numAlpha = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
	std::map <wchar_t,int> alphaNum;
	std::vector <int> key; //ключ
	std::vector<int> convert(const std::wstring& 	ws);
	std::string convert(const std::vector<int>& v);
	std::wstring getValidKey(const std::wstring & 	ws);
	inline std::wstring getValidOpenText(const std::wstring & ws);
	inline std::wstring getValidCiphrText(const std::wstring & ws);
public:
	modAlphaCiphr()=delete;
	modAlphaCiphr(const std::string& skey);
	std::string encrypt(const std::string& 	open_text);
	std::string decrypt(const std::string& 	ciphr_text);
	std::wstring towstr(const std::string& s);
	std::string fromwstr(const std::wstring& ws);
};
class ciphr_error: public std::invalid_argument
{
public:
	explicit ciphr_error (const std::string& what_arg):std::invalid_argument(what_arg) {}
	explicit ciphr_error (const char* what_arg):std::invalid_argument(what_arg) {}
};

/*Модифицированный модуль*/
#include "ch.h"
modAlphaCiphr::modAlphaCiphr(const std::string& skey)
{
	for (unsigned i=0; i<numAlpha.size(); i++) {
		alphaNum[numAlpha[i]]=i;
	}
	std::wstring wskey = towstr(skey);
	key = convert(getValidKey(wskey));;
}
std::string modAlphaCiphr::encrypt(const std::string& open_text)
{
	std::wstring open_wtext = towstr (open_text);
	std::vector<int> work = convert(getValidOpenText(open_wtext));
	for(unsigned i=0; i < work.size(); i++) {
		work[i] = (work[i] + key[i % key.size()]) % alphaNum.size();
	}
	return convert(work);
}
std::string modAlphaCiphr::decrypt(const std::string& ciphr_text)
{
	std::wstring ciphr_wtext = towstr (ciphr_text);
	std::vector<int> work = 	convert(getValidCiphrText(ciphr_wtext));
	for(unsigned i=0; i < work.size(); i++) {
		work[i] = (work[i] + alphaNum.size() - key[i % key.size()]) % alphaNum.size();
	}
	return convert(work);
}
inline std::vector<int> modAlphaCiphr::convert(const std::wstring& ws)
{
	std::vector<int> result;
	for(auto wc:ws) {
		result.push_back(alphaNum[wc]);
	}
	return result;
}
inline std::string modAlphaCiphr::convert(const std::vector<int>& v)
{
	std::wstring wresult;
	for(auto i:v) {
		wresult.push_back(numAlpha[i]);
	}
	std::string result = fromwstr(wresult);
	return result;
}
std::wstring modAlphaCiphr::towstr(const std::string& s)
{
	std::locale loc("ru_RU.UTF-8");
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;
	std::wstring ws = codec.from_bytes(s);
	return ws;
}
std::string modAlphaCiphr::fromwstr(const std::wstring& ws)
{
	std::locale loc("ru_RU.UTF-8");
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;
	std::string s = codec.to_bytes(ws);
	return s;
}
inline std::wstring modAlphaCiphr::getValidKey(const std::wstring & ws)
{
	std::locale loc("ru_RU.UTF-8");
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;
	if (ws.empty())
		throw ciphr_error("Empty key");
	std::wstring tmp(ws);
	for (auto & wc:tmp) {
		if (!isalpha(wc, loc)) {
			std::string s = fromwstr(ws);
			throw ciphr_error(std::string("Invalid 	key")+s);
		}
		if (islower(wc, loc))
			wc = toupper(wc, loc);
	}
	return tmp;
}
inline std::wstring modAlphaCiphr::getValidOpenText(const std::wstring & ws)
{
	std::locale loc("ru_RU.UTF-8");
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;
	std::wstring tmp;
	for (auto wc:ws) {
		if (isalpha(wc, loc)) {
			if (islower(wc,loc))
				tmp.push_back(toupper(wc,loc));
			else
				tmp.push_back(wc);
		}
	}
	if (tmp.empty())
		throw ciphr_error("Empty open text ");
	return tmp;
}
inline std::wstring modAlphaCiphr::getValidCiphrText(const std::wstring & ws)
{
	std::locale loc("ru_RU.UTF-8");
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> codec;
	if (ws.empty())
		throw ciphr_error("Empty ciphr text ");
	for (auto wc:ws) {
		if (!isupper(wc, loc)) {
			std::string s = fromwstr(ws);
			throw ciphr_error(std::string("Invalid 	ciphr text ")+s);
		}
	}
	return ws;
}
