#include "IO.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>

IO::IO()
{
	pictures = 0;
	time_t now = time(nullptr);
	struct tm local_time;
	localtime_s(&local_time, &now);
	std::stringstream name;
	
	std::stringstream month;
	if (local_time.tm_mon + 1 < 10)
	{
		month << "0" << local_time.tm_mon + 1;
	}
	else
	{
		month << local_time.tm_mon + 1;
	}
	
	std::stringstream day;
	if (local_time.tm_mday < 10)
	{
		day << "0" << local_time.tm_mday;
	}
	else
	{
		day << local_time.tm_mday;
	}
	
	std::stringstream hour;
	if (local_time.tm_hour < 10)
	{
		hour << "0" << local_time.tm_hour;
	}
	else
	{
		hour << local_time.tm_hour;
	}
	
	std::stringstream min;
	if (local_time.tm_min < 10)
	{
		min << "0" << local_time.tm_min;
	}
	else
	{
		min << local_time.tm_min;
	}
	
	std::stringstream sec;
	if (local_time.tm_sec < 10)
	{
		sec << "0" << local_time.tm_sec;
	}
	else
	{
		sec << local_time.tm_sec;
	}

	name << local_time.tm_year + 1900 << "_"
		<< month.str() << "_"
		<< day.str() << "_"
		<< hour.str() << "_"
		<< min.str() << "_"
		<< sec.str();
	folder_name = "C:\\Users\\Pascal\\Documents\\Simulationen\\" + name.str();
	if (std::filesystem::create_directory(folder_name))
	{
		std::cout << "Created folder " << folder_name << std::endl;
	}
	else
	{
		std::cout << "create_directory failed." << std::endl;
	}
}

void IO::save_picture(char* picture_data, int width, int height)
{
	if (pictures > 20000)
	{
		// don't save more than 20.000 pictures
		return;
	}
	std::stringstream name;
	name << pictures << ".tga";
	std::string file_name = folder_name + "\\" + name.str();
	std::fstream file_out(file_name, std::ios_base::app | std::ios_base::binary);
	if (!file_out.is_open())
	{
		std::cout << "failed to open " << file_name << std::endl;
	}
	else
	{
		char tga_header[] = {
			0,
			0,
			2,
			0, 0,
			0, 0,
			0,
			0, 0,
			0, 0,
			char(width & 0x00FF),
			char((width & 0xFF00) / 256),
			char(height & 0x00FF),
			char((height & 0xFF00) / 256),
			24,
			0
		};
		file_out.write(tga_header, 18);
		file_out.write(picture_data, width * height * 3);
		file_out.close();
		++pictures;
	}
}
