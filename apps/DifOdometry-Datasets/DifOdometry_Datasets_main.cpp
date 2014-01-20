/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include "DifOdometry_Datasets.h"

using namespace std;
using namespace mrpt;

const char *default_cfg_txt =
	"; ---------------------------------------------------------------\n"
	"; FILE: Difodo Parameters.txt\n"
	";\n"
	";  MJT @ JANUARY-2014\n"
	"; ---------------------------------------------------------------\n\n"

	"[DIFODO_CONFIG]\n\n"

	";downsample: 1 - 640x480, 2 - 320x240, 4 - 160x120 \n"
	"downsample = 4 \n\n"

	";Indicate the number of rows and columns. They must be equal or inferior to what is indicated with the 'downsample' variable). \n"
	"rows = 60 \n"
	"cols = 80 \n\n"

	";Absolute path of the rawlog file \n"
	"filename = C:/.../file.rawlog \n";


// ------------------------------------------------------
//						MAIN
// ------------------------------------------------------


int main(int num_arg, char *argv[])
{
	try
	{
		//						Read function arguments
		//----------------------------------------------------------------------
		bool use_config_file = false;
		string filename;
		CDifodoDatasets odo;

		if (num_arg < 2 || string(argv[1]) == "--help")
		{
			printf("\n\t       Arguments of the function 'main' \n");
			printf("==============================================================\n\n");
			printf(" --help: Shows this menu... \n\n");
			printf(" --config FICH.txt: Load FICH.txt as config file \n\n");
			printf(" --create-config FICH.txt: Save the default config parameters \n\n");
			printf(" \t\t\t   in FICH.txt and close the program \n\n");
			printf(" --save-logfile: Enable saving a file with results of the pose estimate \n\n");
			system::os::getch();
			return 1;
		}
		else if ( string(argv[1]) == "--create-config")
		{
			filename = argv[2];
			cout << endl << "Desired config filename [e.g. 'cfg.ini']: " << filename;
			ofstream new_file(filename.c_str());
			new_file << string(default_cfg_txt);
			new_file.close();
			cout << endl << "File saved" << endl;
			system::os::getch();
			return 1;
		}
		else
		{
			for (int i=1; i<num_arg; i++)
			{
				if ( string(argv[i]) == "--save-logfile")
				{
					odo.save_results = true;
					odo.CreateResultsFile();
				}

				if ( string(argv[i]) == "--config")
				{
					use_config_file = true;
					filename = argv[i+1];
				}
			}
		}

		//Initial steps. Load configuration from file or default
		//------------------------------------------------------

		if (!use_config_file)
		{
			utils::CConfigFileMemory configDifodo(default_cfg_txt);
			odo.loadConfiguration( configDifodo );
		}
		else
		{
			utils::CConfigFile configDifodo(filename);
			odo.loadConfiguration( configDifodo );
		}

		odo.initializeScene();

		//==============================================================================
		//									Main operation
		//==============================================================================

		int pushed_key = 0;
		bool working = 0, stop = 0;

		odo.reset();

		while (!stop)
		{

			if (odo.window.keyHit())
				pushed_key = odo.window.getPushedKey();
			else
				pushed_key = 0;

			switch (pushed_key) {

			//Capture 1 new frame and calculate odometry
			case  'n':
				if (odo.dataset.size() <= odo.rawlog_count)
				{
					working = 0;
					cout << endl << "Fin de archivo.";
				}
				else
				{
					odo.loadFrame();
					odo.OdometryCalculation();
					odo.filterSpeedAndPoseUpdate();
					cout << endl << "Difodo execution time(ms): " << odo.execution_time;
					odo.updateScene();
				}

				break;

			//Start and stop continous odometry
			case 's':
				working = !working;
				break;

			//Close the program
			case 'e':
				stop = 1;
				if (odo.f_res.is_open())
					odo.f_res.close();
				break;

			//Show statistics
			case 't':
				odo.showStatistics();
				break;

			}

			if (working == 1)
			{
				if (odo.dataset.size() <= odo.rawlog_count)
				{
					working = 0;
					cout << endl << "Fin de archivo.";
					if (odo.f_res.is_open())
						odo.f_res.close();
				}
				else
				{
					odo.loadFrame();
					odo.OdometryCalculation();
					odo.filterSpeedAndPoseUpdate();
					cout << endl << "Difodo execution time(ms): " << odo.execution_time;
					odo.updateScene();
				}
			}
		}

		return 0;

	}
	catch (std::exception &e)
	{
		std::cout << "MRPT exception caught: " << e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		printf("Untyped exception!!");
		return -1;
	}
}

