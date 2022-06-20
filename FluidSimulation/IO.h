#pragma once
#include <string>

class IO
{
private:
	std::string folder_name;
	int pictures;
public:
	IO();
	void save_picture(char* picture_data, int width, int height);
};
