  /// \file DQMCoreTool.cc
/*
 *
 * DQMCoreTool.cc source template automatically generated by a class generator
 * Creation date : mer. juin 17 2015
 *
 * This file is part of DQM4HEP libraries.
 * 
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include "dqm4hep/DQMCoreTool.h"

#include <sys/utsname.h>
#include <unistd.h>

namespace dqm4hep
{

void DQMCoreTool::timeToHMS(time_t t, int &hours, int &minutes, int &seconds)
{
	struct tm *pTmTime = new tm();
	localtime_r(&t, pTmTime);
	hours = pTmTime->tm_hour;
	minutes = pTmTime->tm_min;
	seconds = pTmTime->tm_sec;
	delete pTmTime;
}

//-------------------------------------------------------------------------------------------------

void DQMCoreTool::timeToHMS(time_t t, std::string &timeStr)
{
	// get h m and s
	int hours, minutes, seconds;
	DQMCoreTool::timeToHMS(t, hours, minutes, seconds);

	// convert to str
	std::stringstream ss;
	ss << hours << "h " << minutes << "m " << seconds << "s";
	timeStr = ss.str();
}

//-------------------------------------------------------------------------------------------------

static DQMTimePoint DQMCoreTool::now()
{
	return std::chrono::system_clock::now();
}

//-------------------------------------------------------------------------------------------------

StringVector DQMCoreTool::getSpecialCharacterList()
{
	StringVector specialCharacterList;

	specialCharacterList.push_back("|");
	specialCharacterList.push_back("&");
	specialCharacterList.push_back(";");
	specialCharacterList.push_back("<");
	specialCharacterList.push_back(">");
	specialCharacterList.push_back("(");
	specialCharacterList.push_back(")");
	specialCharacterList.push_back("$");
	specialCharacterList.push_back("\\");
	specialCharacterList.push_back("\'");
	specialCharacterList.push_back("\"");
	specialCharacterList.push_back("\t");
	specialCharacterList.push_back("\n");
	specialCharacterList.push_back("*");
	specialCharacterList.push_back("?");
	specialCharacterList.push_back(",");
	specialCharacterList.push_back("[");
	specialCharacterList.push_back("]");
	specialCharacterList.push_back("#");
	specialCharacterList.push_back("~");
	specialCharacterList.push_back("=");
	specialCharacterList.push_back("%");

	return specialCharacterList;
}

//-------------------------------------------------------------------------------------------------

bool DQMCoreTool::containsSpecialCharacters(const std::string &str)
{
	StringVector specialCharacterList = DQMCoreTool::getSpecialCharacterList();

	for(StringVector::iterator iter = specialCharacterList.begin(), endIter = specialCharacterList.end() ;
			endIter != iter ; ++iter)
	{
		if(str.find(*iter) != std::string::npos)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

void DQMCoreTool::fillHostInfo(DQMHostInfo &info)
{
	// uname
	struct utsname unameStruct;
	uname(&unameStruct);

	// host name
	char host[256];
	gethostname(host, 256);

	info[DQMKey::SYSTEM_NAME] = unameStruct.sysname;
	info[DQMKey::NODE_NAME] = unameStruct.nodename;
	info[DQMKey::RELEASE] = unameStruct.release;
	info[DQMKey::VERSION] = unameStruct.version;
	info[DQMKey::MACHINE] = unameStruct.machine;
	info[DQMKey::HOST_NAME] = host;
}

} 

