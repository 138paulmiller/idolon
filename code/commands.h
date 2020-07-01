#include "core.h"


using Args = std::vector<std::string > ;
using Command = std::function<void(Args)>;
using CommandTable = std::map<std::string, Command>;  

void Execute(const std::string& input, const CommandTable& commands);
void Execute(int argc, char** argv, const CommandTable& commands);
