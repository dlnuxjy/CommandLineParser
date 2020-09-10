#include <iostream>
#include "CommandLineParser.h"

int main(int argc, char** argv)
{
	const String keys =
		"{help h usage ? |      | print this message   }"
		"{@b             |200.0 | double test		   }"
		"{s              |str   | string test          }"
		"{n count        |100   | count of objects     }"
		"{t timestamp    |      | use time stamp       }"
		;
	//解析命令行
	CommandLineParser parser(argc, argv, keys);
	//程序版本信息
	parser.about("Application name v1.0.0");

	//帮助信息
	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	//整形
	int n = parser.get<int>("n");
	std::cout << "n = "<< n << std::endl;

	//-@b参数的double类型,命令行传参的时候可以省去-@b,直接输入值
	double k = parser.get<double>("@b");
	std::cout << "@b = " << k << std::endl;

	//字符串类型
	std::string s = parser.get<std::string>("s");
	std::cout << "s = " << s << std::endl;

	//判断传参中是否包含指定项目
	if (parser.has("t")) {
		int t = parser.get<int>("t");
		std::cout << "t = " << t << std::endl;
	}

	//检测错误
	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	return 0;
}
