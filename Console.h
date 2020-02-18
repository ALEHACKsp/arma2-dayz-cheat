#pragma once

#include <fstream>

enum vlevel
{
	STANDARD = 0,
	PEDANTIC
};

class Logging
{
public:
	/*
	* Constructor
	*/
	Logging(HMODULE moduleHandle, vlevel verbosity = STANDARD, bool file = true, bool console = true)
	{
		module = moduleHandle;
		verboseLevel = verbosity;
		memset(filename, 0, 1024);
		memset(buffer, 0, 1024);

		logFile = file;
		logConsole = console;

		if (console)
			hasConsole = AllocConsole();

		if (file)
		{
			GetModuleFileNameA(module, filename, 1024);
			filename[strlen(filename) - 3] = 't';
			filename[strlen(filename) - 2] = 'x';
			filename[strlen(filename) - 1] = 't';

			fileStream.open(filename); // , std::ios::app );
		}

		fileStream.rdbuf()->pubsetbuf(0, 0);
	}

	/*
	* Destructor
	*/
	~Logging()
	{
		FreeConsole();
		fileStream.close();
	}

private:
	/*
	* Get the path of the selected module
	*
	* @param hModule module handle
	* @param szBuffer buffer to put path in
	* @param nSize max size of buffer
	*/
	void GetModuleFilePath(HMODULE hModule, LPSTR szBuffer, DWORD nSize)
	{
		GetModuleFileNameA(hModule, szBuffer, nSize);

		for (int i = strlen(szBuffer); i > 0; i--)
		{
			if (szBuffer[i] == '\\')
			{
				memset(&szBuffer[i + 1], 0, nSize - 1);
				break;
			}
		}
	}

	HMODULE module;
	BOOL hasConsole;
	char filename[1024];
	char buffer[1024];
	std::ofstream fileStream;
	va_list va_argList;
	SYSTEMTIME time;
	bool logFile;
	bool logConsole;

	vlevel verboseLevel;

	/*
	* Log text to file
	*
	* @param text the text to log
	*/
	void toFile(char* text)
	{
		fileStream << buffer << std::endl;
	}

	/*
	* Log text to console
	*
	* @param text the text to log
	*/
	void toConsole(char* text)
	{
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text, strlen(text), NULL, 0);
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), "\r\n", 2, NULL, 0);
	}

public:
	void Write(vlevel verbosity, const char* format, ...)
	{
		if (verboseLevel < verbosity)
		{
			return;
		}

		GetLocalTime(&time);
		sprintf_s(buffer, "%02d:%02d:%02d ", time.wHour, time.wMinute, time.wSecond);

		va_start(va_argList, format);
		_vsnprintf_s(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 1024, format, va_argList);
		va_end(va_argList);

		if (logFile)
			toFile(buffer);

		if (logConsole)
			toConsole(buffer);
	}

	void Write(const char* format, ...)
	{
		GetLocalTime(&time);
		sprintf_s(buffer, "%02d:%02d:%02d ", time.wHour, time.wMinute, time.wSecond);

		va_start(va_argList, format);
		_vsnprintf_s(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 1024, format, va_argList);
		va_end(va_argList);

		if (logFile)
			toFile(buffer);

		if (logConsole)
			toConsole(buffer);
	}
};