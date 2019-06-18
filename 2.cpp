/*Главный модуль программы*/
#include <iostream>
#include <cctype>
#include "ch.h"
using namespace std;
int main(int argc, char **argv)
{
	int key;
	string text;
	unsigned op;
	do {
		try {
			cout<<"Input operation (0-exit, 1-encrypt, 2-decrypt): ";
			cin>>op;
			if (op > 2)
				cout<<"Illegal operation\n";
			else if (op > 0) {
				cout<<"Ciphr ready. Input text: ";
				cin>>text;
				cout<<"Input key: ";
				cin>>key;
				Ciphr ciphr(key, text);
				if (op==1) {
					cout<<"Encrypted text: "<<ciphr.encrypt(text)<<endl;
				} else {
					cout<<"Decrypted text: "<<ciphr.decrypt(text)<<endl;
				}
			}
		} catch (const Error & e) {
			cerr<<"Error: "<<e.what()<<endl;
		}
	} while (op != 0);
	return 0;
}

/*Заголовочный файл модуля*/
#pragma once
#include <string>
#include <stdexcept>
using namespace std;
class Ciphr
{
	char** value;
	int b, a;
	string getValidText(const string& s);
public:
	Ciphr() = delete;
	Ciphr(int key, const string& s);
	string encrypt(const string& open_text);
	string decrypt(const string& ciphr_text);
	~Ciphr();
};
class Error: public invalid_argument
{
public:
	explicit Error (const string& what_arg):
		invalid_argument(what_arg) {}
	explicit Error (const char* what_arg):
		invalid_argument(what_arg) {}
};

Модуль шифра табличной перестановки

#include "ch.h"
Ciphr::Ciphr(int key, const string& s)
{
	if (key < 2 or key > getValidText(s).size())
		throw Error(string("Invalid key"));
	a = key;
	b = ((getValidText(s).size() - 1)/a + 1);
	value = new char * [b];
	for (int i = 0; i < b; i++) {
		value[i] = new char [a];
	}
}
string Ciphr::encrypt(const string& open_text)
{
	//заполнение таблицы "*"
	for (int i=0; i < b; i++)
		for (int j=0; j < a; j++)
			value[i][j] = '*';
	//заполнение таблицы текстом
	for (int i = 0; i < getValidText(open_text).size();
	     i++)
		value[i/a][i%a] = getValidText(open_text)[i];
	//вывод таблицы в строку
	string ciphr_text;
	for (int i = a - 1; i >= 0; i --)
		for (int j = 0; j < b; j++)
			ciphr_text = ciphr_text + value[j][i];
	return ciphr_text;
}
string Ciphr::decrypt(const string& ciphr_text)
{
	//заполнение таблицы "*"
	for (int i=0; i < b; i++)
		for (int j=0; j < a; j++)
			value[i][j] = '*';
	//заполнение таблицы текстом
	for (int i = 0; i < getValidText(ciphr_text).size();
	     i++)
		value[i%b][i/b] = getValidText(ciphr_text)[i];
	//вывод таблицы в строку
	string open_text;
	for (int i = 0; i < b; i ++)
		for (int j = a - 1; j >= 0; j--)
			open_text = open_text + value[i][j];
	return open_text;
}
Ciphr::~Ciphr()
{
	for (int i = 0; i < b; i++) {
		delete [] value[i];
	}
	delete [] value;
}
string Ciphr::getValidText(const string& s)
{
	string tmp(s);
	if (s.empty())
		throw Error("Empty text");
	for (auto c:tmp)
		if (!isalpha(c)) {
			throw Error(string("Invalid text"));
		}
	return tmp;
}
