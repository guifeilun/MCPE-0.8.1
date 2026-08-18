#include <network/ServerCommandParser.hpp>
#include <util/GuiMessage.hpp>
#include <util/Util.hpp>

ServerCommandParser::Token::Token(const std::string& s)
	: field_0(s) {
	int v4 = sscanf(s.data(), "%d", &this->n);
	this->isNumeric = v4 != 0;
	this->isEmpty = this->field_0.length() == 0;
}

std::string ServerCommandParser::Command::checkParameters(const std::vector<ServerCommandParser::Token>& a3) {
	if(a3.size() > this->exceptedParamCnt) {
		int v9 = 0;
		int v10 = a3.size() - 1;
		while(v9 < this->numericParams.size() && v9 < v10) {
			if(this->numericParams[v9] && a3[v9 + 1].isNumeric != 1) {
				return "Error: Expected a number for parameter " + Util::toString(v9);
			}
			++v9;
		}
		return "";
	} else {
		return "Error: Not enough parameters, expected " + Util::toString(this->exceptedParamCnt);
	}
}
/*static ServerCommandParser::Command* testCmd = new ServerCommandParser::Command{{0, 0, 1}, 2, [](const std::vector<ServerCommandParser::Token>& params) {
		return params[0].field_0+" has successfully received your cliff client! owo: "+params[1].field_0+" "+params[2].field_0+" "+params[3].field_0;
}};*/
ServerCommandParser::ServerCommandParser() {
//	this->commands.insert({"cliff", std::unique_ptr<ServerCommandParser::Command>(testCmd)});
}
ServerCommandParser::Command::~Command(){

}
std::string* ServerCommandParser::executeCommand(const GuiMessage& msg) {
	std::vector<ServerCommandParser::Token> v24;
	std::string v18;
	int v2 = 0;
	int v5 = 1;
	while(1) {
		const char* data = msg.field_8.data();
		int v8 = msg.field_8.length();
		if(v5 > v8) break;
		if(v5 >= v8) {
			if(v2) {
				printf("1 adding token: %s\n", v18.c_str());
				v24.emplace_back(ServerCommandParser::Token(v18));
				goto LABEL_19;
				//goto ADD_TOKEN;
			}
		} else {
			unsigned char v6 = data[v5];
			if(v2) {
				if(v2 == 1) {
					if(v6 != '\t' && v6 != ' ' && data[v5]) {
						v18 += v6;
						goto LABEL_19;
					}
ADD_TOKEN:
					printf("2 adding token: %s\n", v18.c_str());
					v24.emplace_back(ServerCommandParser::Token(v18));
					v18.clear();
					goto LABEL_19;
				}
				if(v6 == '"' || !data[v5]) {
					goto ADD_TOKEN;
				}
				v18 += v6;
LABEL_18:
				v2 = 2;
				goto LABEL_19;
			}
			if(v6 == '"') goto LABEL_18;
			if(v6 != '\t' && v6 != ' ') {
				if(data[v5]) {
					v2 = 1;
					v18 += v6;
				}
			}
		}
LABEL_19:
		++v5;
	}
	if(v24.empty()) {
		this->retStr = "Error: no command provided";
		return &this->retStr;
	}
	std::string v19 = v24[0].isEmpty ? Util::EMPTY_STRING : v24[0].field_0;
	auto&& cmd = this->commands.find(v19);
	v24[0] = ServerCommandParser::Token(msg.field_C); //inlined
	if(cmd == this->commands.end()) {
		std::string v21 = "Error: Command " + v19 + " not found";
		this->retStr = v21;
	} else {
		while(1) {
			this->retStr.clear();
			std::string v20 = cmd->second->checkParameters(v24);
			if(v20.size() == 0) break;
			this->retStr = v20;
			++cmd; //TODO check is this how it is handled
			if(cmd == this->commands.end()) return &this->retStr;
		}
		while(1) {
			if(cmd->second->numericParams.size() + 1 <= v24.size()) {
				break;
			}
			v24.emplace_back(ServerCommandParser::Token(""));
		}

		this->retStr = cmd->second->func(v24);
	}

	return &this->retStr;
}
