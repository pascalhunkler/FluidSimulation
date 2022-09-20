#include "IO.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <glm/glm.hpp>

IO::IO(const IO& io)
{
	this->folder_name = io.folder_name;
	this->pictures = io.pictures;
}

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

void IO::decide_parameters(SimulationScenario& scenario, int& fluid_depth, PressureComputationMethod& method, float& particle_size, float& viscosity, float& gravity, float& stiffness, float& timeStep)
{
	// Let the user decide what scenario to simulate
	for (int i = 0; i < static_cast<int>(SimulationScenario::last); ++i)
	{
		std::cout << i << '\t';
		switch (static_cast<SimulationScenario>(i))
		{

		case SimulationScenario::breakingDam:
			std::cout << "breaking dam" << std::endl;
			break;

		case SimulationScenario::leakyDam:
			std::cout << "leaky dam" << std::endl;
			break;

		case SimulationScenario::droppingFluid:
			std::cout << "dropping fluid" << std::endl;
			break;

		case SimulationScenario::flowingFluid:
			std::cout << "flowing fluid" << std::endl;
			break;

		case SimulationScenario::restingFluid:
			std::cout << "resting fluid" << std::endl;
			break;

		default:
			break;
		}
	}
	int scenario_int;
	std::cin >> scenario_int;

	// choose first scenario if user gives invalid input
	if (scenario_int < 0 || scenario_int >= static_cast<int>(SimulationScenario::last))
	{
		scenario = SimulationScenario::breakingDam;
	}
	else
	{
		scenario = static_cast<SimulationScenario>(scenario_int);
	}

	// Let the user decide about the depth of the fluid
	std::cout << std::endl;
	std::cout << "Type in the depth of the fluid (1-500), default is 20" << std::endl;
	std::cin >> fluid_depth;
	if (fluid_depth < 1 || fluid_depth > 500)
	{
		fluid_depth = 20;
	}

	// Let the user decide about the method of pressure computation
	std::cout << std::endl;
	std::cout << "0" << "\t" << "incompressible" << std::endl;
	std::cout << "1" << "\t" << "compressible" << std::endl;
	int method_int;
	std::cin >> method_int;

	// choose incompressible pressure computation if user gives invalid input
	if (method_int < 0 || method_int >= 2)
	{
		method = PressureComputationMethod::incompressible;
	}
	else
	{
		method = static_cast<PressureComputationMethod>(method_int);
	}

	// Let the user decide about the particle size
	std::cout << std::endl;
	std::cout << "Type in the particle size (1-40), default is 8" << std::endl;
	std::cin >> particle_size;
	if (particle_size < 1 || particle_size > 40)
	{
		particle_size = 8;
	}

	// Let the user decide about the viscosity
	std::cout << std::endl;
	std::cout << "Type in the viscosity (0 - 2000), default is 200" << std::endl;
	std::cin >> viscosity;
	if (viscosity < 0 || viscosity > 2000)
	{
		viscosity = 200;
	}

	// Let the user decide about the gravity
	/*std::cout << std::endl;
	std::cout << "Type in the gravity (0 - 50), default is 9.81" << std::endl;
	std::cin >> gravity;
	if (gravity < 0 || gravity > 50)
	{
		gravity = 9.81;
	}*/
	// gravity is fixed to 9.81
	gravity = 9.81f;

	// If pressure computation method is compressible, decide about the stiffness
	if (method == PressureComputationMethod::compressible)
	{
		std::cout << std::endl;
		std::cout << "Type in the stiffness (0 - 1E+10), default is 1E+6" << std::endl;
		std::cin >> stiffness;
		if (stiffness < 0 || stiffness > 1E+10)
		{
			stiffness = 1E+6;
		}
	}

	// Let the user decide about the time step
	std::cout << std::endl;
	std::cout << "Type in the time step (0.0001 - 0.5), default is 0.01" << std::endl;
	std::cin >> timeStep;
	if (timeStep < 0.0001 || timeStep > 0.5)
	{
		timeStep = 0.01;
	}


	// print parameters in a file
	std::string file_name = folder_name + "\\parameters.txt";
	std::fstream file_out(file_name, std::ios_base::out);
	if (!file_out.is_open())
	{
		std::cout << "failed to open " << file_name << std::endl;
	}
	else
	{
		std::stringstream stream;
		stream << "Szenario: " << scenario_int << std::endl;
		stream << "Flüssigkeitstiefe: " << fluid_depth << std::endl;
		stream << "Druckberechnung: " << method_int << std::endl;
		stream << "Partikelgröße: " << particle_size << std::endl;
		stream << "Viskosität: " << viscosity << std::endl;
		if (method == PressureComputationMethod::compressible)
		{
			stream << "Steifigkeitskonstante: " << stiffness << std::endl;
		}
		stream << "Zeitschritt: " << timeStep << std::endl;
		file_out << stream.str();
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

void IO::print_average_density(float average_density) const
{
	std::string file_name = folder_name + "\\average_density.txt";
	std::fstream file_out(file_name, std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!file_out.is_open())
	{
		std::cout << "failed to open " << file_name << std::endl;
	}
	else
	{
		std::stringstream line_stream;
		if (pictures == 0)
		{
			line_stream << "Simulationsschritt" << "\t" << "Durchschnittsdichte" << "\n";
		}
		line_stream << pictures << "\t" << average_density << "\n";
		file_out << line_stream.str();
	}
}

void IO::print_cfl_condition(const std::vector<Particle>& particles, float timeStep, float particleSize) const
{
	float max_speed = 0;
	for (const Particle& particle : particles)
	{
		float particle_speed = glm::length(particle.velocity);
		if (particle_speed > max_speed)
		{
			max_speed = particle_speed;
		}
	}
	bool cfl_condition = max_speed < particleSize / timeStep;
	std::string file_name = folder_name + "\\cfl_condition.txt";
	std::fstream file_out(file_name, std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!file_out.is_open())
	{
		std::cout << "failed to open " << file_name << std::endl;
	}
	else
	{
		std::stringstream line_stream;
		if (pictures == 1)
		{
			line_stream << "Simulationsschritt" << "\t" << "Durchschnittsdichte" << "\n";
		}
		line_stream << pictures << "\t" << cfl_condition << "\n";
		file_out << line_stream.str();
	}
}

void IO::print_iterations(int iterations) const
{
	std::string file_name = folder_name + "\\iterations.txt";
	std::fstream file_out(file_name, std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!file_out.is_open())
	{
		std::cout << "failed to open " << file_name << std::endl;
	}
	else
	{
		std::stringstream line_stream;
		if (pictures == 0)
		{
			line_stream << "Simulationsschritt" << "\t" << "Iterationen" << "\n";
		}
		line_stream << pictures << "\t" << iterations << "\n";
		file_out << line_stream.str();
	}
}
