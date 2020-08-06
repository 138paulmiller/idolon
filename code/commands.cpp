#include "pch.hpp"

#include "commands.hpp"

#include <sstream>



CommandDesc::CommandDesc(const std::string & name, const std::string & help )
	:name(name), help(help)
{

}
bool operator==(const CommandDesc& lhs, const CommandDesc& rhs)
{
	return lhs.name == rhs.name;
}

std::size_t CommandDesc::Hash::operator()(const CommandDesc& desc) const
{
	return std::hash<std::string>()(desc.name);
}

void HelpAll(const CommandTable& commands, std::string & help)
{
	help.clear();
	for(auto it : commands)
		help += it.first.name + "\n - " + it.first.help + "\n";

}

void Help(const CommandTable& commands, const std::string & name, std::string & help)
{
	help.clear();
	auto it = commands.find({name});
	if(it != commands.end())
		help = name + " : " + it->first.help;
}

void Execute(const std::string & input, const CommandTable & commands ) 
{
	//split string by white space. 
	std::vector<char* > args;
	std::string word;
	std::istringstream iss;
	iss.str(input);
	while (std::getline(iss, word, ' '))
	{
		char* copy = new char[word.size()+1];
		for(int i = 0; i < word.size(); i++)
			copy[i] = word[i];
		copy[word.size()] = '\0';
		args.push_back(copy);
	}
	
	Execute(args.size(), &args[0], commands);
	
	for (int i = 0; i < args.size(); i++)
		delete args[i];
}

void Execute(int argc, char** argv, const CommandTable& commands)
{
	Args args; 
	//ignore first arg
	int i = 0;
	while( i < argc)
	{
		CommandTable::const_iterator it = commands.find({argv[i]});
		if(it == commands.end())
		{
			LOG("Execute: Invalid Command (%s)\n", argv[i]);
			return;
		}
		//parse until non cmd is found
		const Command & command  = it->second; 
		i++;
		while(i < argc)
		{
			const char *arg=  argv[i];
			it = commands.find({arg});
			if (it == commands.end()) //if does not exist
				args.push_back(arg);
			else
				break;
			i++;
		}
		command(args);
		args.clear();
	}
}
