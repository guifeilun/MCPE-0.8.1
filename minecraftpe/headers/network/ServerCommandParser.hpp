#pragma once
#include <_types.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

struct GuiMessage;
struct ServerCommandParser
{

	struct Token
	{
		std::string field_0;
		int32_t n;
		int32_t isNumeric;
		bool isEmpty;

		Token(const std::string&);
	};

	struct Command
	{
		//array of integers where 0 is non numeric, and anything else is numeric?
		std::vector<int> numericParams;
		int32_t exceptedParamCnt;
		//first element in the array seems to be player's username
		std::function<std::string (const std::vector<ServerCommandParser::Token>&)> func;

		~Command();
		std::string checkParameters(const std::vector<ServerCommandParser::Token>&);
	};

	std::string retStr;
	std::unordered_map<std::string, std::unique_ptr<ServerCommandParser::Command>> commands;

	ServerCommandParser();
	std::string* executeCommand(const GuiMessage&);
};
