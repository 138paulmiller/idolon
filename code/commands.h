#pragma once
#include "core.h"

struct CommandDesc 
{
	//command name
	CommandDesc(const std::string & name, const std::string & help = "");
	const std::string name;
	const std::string help; 
};

bool operator==(const CommandDesc& lhs, const CommandDesc& rhs);

struct CommandDescHash
{
	std::size_t operator()(const CommandDesc& desc) const;
};


//Add usage string to command ,with expected arg count. throw excpetion message with error. then have main forward this to shell 
using Args = std::vector<std::string > ;
using Command = std::function<void(Args)>;
using CommandTable = std::unordered_map<CommandDesc, Command, CommandDescHash>;  

void HelpAll(const CommandTable& commands, std::string & help);
void Help(const CommandTable& commands, const std::string & name, std::string & help);

void Execute(const std::string& input, const CommandTable& commands);
void Execute(int argc, char** argv, const CommandTable& commands);
