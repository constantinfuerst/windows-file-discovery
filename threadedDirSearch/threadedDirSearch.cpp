#include "pch.h"

#ifndef INCLUDES
#define INCLUDES

#include "logger.h"
INITIATE_LOGGER;
#include "search_request.h"

#endif

#define CONSOLE_INDENT std::cout << "	"

search_request new_search;
std::string output_file_name;

/*
 *
 *	GENERAL FUNCTIONS (string converters, input handler and so on)
 *
 */


// convert widestring to sinlge byte
std::string widetosingle(const std::wstring &input) {
	std::wstring_convert < std::codecvt_utf8_utf16 <wchar_t> > converter;
	const std::string output = converter.to_bytes(input);
	return output;
}

// convert single byte to widestring
std::wstring singletowide(const std::string &input) {
	std::wstring_convert < std::codecvt_utf8_utf16 <wchar_t> > converter;
	const std::wstring output = converter.from_bytes(input);
	return output;
}

// checks if a directory specified exists, duh
bool isdir(const std::wstring &dirName_in) {
	const DWORD ftype = GetFileAttributesW(dirName_in.c_str());
	if (ftype == INVALID_FILE_ATTRIBUTES) {
		return FALSE;
	}
	if (ftype & FILE_ATTRIBUTE_DIRECTORY) {
		return TRUE;
	}
	return FALSE;
}

void input_handler() {
	std::string argument;
	{
		LOG_IMPT_NOTICE("Please enter a directory that the programm should search in (remember to put it in \"\" if it contains whitespaces)");
		CONSOLE_INDENT;
		getline(std::cin, argument);
		std::wstring argument_wide = singletowide(argument);
		while (!isdir(argument_wide)) {
			LOG_WARNING("The directory you entered does not seem to exist, please try that again");
			CONSOLE_INDENT;
			getline(std::cin, argument);
			argument_wide = singletowide(argument);
		}
		new_search.pub_dir_to_search = argument_wide;
	}
	{
		LOG_IMPT_NOTICE("Please choose what you would like the programm to do ('search' for a file or 'index' a directory)");
		while (TRUE) {
			CONSOLE_INDENT;
			getline(std::cin, argument);
			if (argument == "index") {
				new_search.pub_mode = "index";
				LOG_IMPT_NOTICE("Please enter an output file (relative paths are supported)");
				std::wofstream woutput_stream;
				while (TRUE) {
					CONSOLE_INDENT;
					getline(std::cin, argument);
					woutput_stream.open(argument, std::fstream::app);
					if (woutput_stream.is_open()) {
						woutput_stream.close();
						break;
					}
					else {
						LOG_WARNING(argument + " is not a valid filename or this application lacks the access rights to the folder specified, please try that again");
					}
				}
				break;
			}
			else if (argument == "search") {
				new_search.pub_mode = "search";
				LOG_IMPT_NOTICE("Please enter a filename or string you would like to search for");
				LOG_WARNING("Using a string that is too general will result in a large amount of results and long runtimes");
				CONSOLE_INDENT;
				getline(std::cin, argument);
				new_search.pub_search_filename = singletowide(argument);
				break;
			}
			else {
				LOG_WARNING(argument + " is not a valid mode setting, please try that again");
			}
		}
	}
	{
		LOG_IMPT_NOTICE("Please choose a priority setting from 'high' or 'normal' ('ultra' and 'low' are not recommended)");
		while (TRUE) {
			CONSOLE_INDENT;
			getline(std::cin, argument);
			if (argument == "ultra") {
				new_search.pub_speed_mode = 0;
				break;
			}
			else if (argument == "high") {
				new_search.pub_speed_mode = 1;
				break;
			}
			else if (argument == "normal") {
				new_search.pub_speed_mode = 2;
				break;
			}
			else if (argument == "low") {
				new_search.pub_speed_mode = 3;
				break;
			}
			else {
				LOG_WARNING(argument + " is not a valid speed setting");
			}
		}
	}
}

void search_output_handler (std::vector<std::wstring> &output) {
	if (new_search.pub_mode == "search" && !output.empty()) {
		LOG_IMPT_NOTICE(L"	" + output[0]);
	}
	else if (new_search.pub_mode == "index") {
		const size_t size = output.size();
		std::wofstream woutput_stream;
		woutput_stream.open(output_file_name, std::fstream::app);
		if (woutput_stream.is_open()) {
			for (int i = 0; i < size; i++) {
				woutput_stream << output[i] << std::endl;
			}
			woutput_stream.close();
		}
		else {
			LOG_ERROR("WRITING TO THE OUTPUT FILE FAILED");
		}
	}
}

int main () {
	std::cout << std::endl;
	input_handler();
	LOG_SUCCESS(L"STARTING SEARCH IN " + new_search.pub_dir_to_search);

	const std::chrono::high_resolution_clock::time_point runtime_start = std::chrono::high_resolution_clock::now();

	new_search.pub_output_handler = &search_output_handler;
	new_search.initiate_search();

	{
		if (new_search.pub_mode == "search") {
			LOG_NOTICE("ended search");
		}
		else {
			if (new_search.filecount != 0) {
				LOG_SUCCESS(std::string("INDEXED ") + std::to_string(new_search.filecount) + std::string(" FILES TO ") + output_file_name);
			}
			else {
				LOG_WARNING("DID NOT DISCOVER ANY FILE");
			}
		}

		const std::chrono::high_resolution_clock::time_point runtime_end = std::chrono::high_resolution_clock::now();
		const std::chrono::milliseconds runtime = std::chrono::duration_cast<std::chrono::milliseconds>(runtime_end - runtime_start);

		if (runtime < std::chrono::milliseconds(5000)) {
			LOG_WARNING("EXECUTION TIME BELOW 5 SECONDS");
			return FALSE;
		}
		std::string runtime_str = std::to_string(runtime.count());
		runtime_str = runtime_str.substr(0, runtime_str.size() - 3) + "," + runtime_str.substr(runtime_str.size() - 3, runtime_str.size());

		LOG_NOTICE("execution time of " + runtime_str + " seconds");
		return TRUE;
	}
}