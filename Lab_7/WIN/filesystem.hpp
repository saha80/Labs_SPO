#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class filesystem
{
public:
	explicit filesystem(std::fstream& file) : file_(file)
	{

	}
#define FS_CMD_DEF(cmd) void (##cmd)(const std::vector<std::string>&);
	FS_CMD_DEF(dir)
	FS_CMD_DEF(cd)
	FS_CMD_DEF(print)
	FS_CMD_DEF(mkdir)
	FS_CMD_DEF(createnew)
	FS_CMD_DEF(rd)
	FS_CMD_DEF(del)
	FS_CMD_DEF(rename)
#undef FS_CMD_DEF
private:
	std::fstream& file_;
};

#define FS_CMD(cmd) inline void filesystem::##cmd(const std::vector<std::string>& args)
FS_CMD(dir)
{
	
}
FS_CMD(cd)
{

}
FS_CMD(print)
{

}
FS_CMD(mkdir)
{

}
FS_CMD(createnew)
{

}
FS_CMD(rd)
{

}
FS_CMD(del)
{

}
FS_CMD(rename)
{

}
#undef FS_CMD