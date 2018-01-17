/// \file dqm4hep-dump-plugins.cc
/*
 *
 * dqm4hep-dump-plugins.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
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
 *
 * Create a quality test template file a ROOT file.
 * Parses the different directories and populate the
 * qtest file with found objects.
 */

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/DBInterface.h>
#include <dqm4hep/XMLParser.h>

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

using namespace std;
using namespace dqm4hep::core;

struct DBInterfaceMetadata
{
  std::shared_ptr<DBInterface> m_dbInterface;
  std::string      m_host;
  std::string      m_user;
  std::string      m_db;
  std::string      m_password;
};

typedef std::map<std::string, DBInterfaceMetadata> DBInterfaceMetadataMap; 

int main(int argc, char* argv[])
{
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR));

  TCLAP::ValueArg<std::string> xmlInputFileArg(
      "f"
      , "parameter-file"
      , "The xml input file describing the parameters and databases to feed"
      , true
      , ""
      , "string");
  pCommandLine->add(xmlInputFileArg);

  // parse command line
  pCommandLine->parse(argc, argv);
  
  XMLParser parser;
  parser.parse(xmlInputFileArg.getValue());
  
  TiXmlDocument &document(parser.document());
  DBInterfaceMetadataMap dbInterfaceMap;

  TiXmlElement *root = document.RootElement();
  TiXmlElement *databases = root->FirstChildElement("databases");
    
  if(!databases)
  {
    dqm_error( "ERROR: no <databases> tag found in root element !" );
    return 1;
  }
  
  for(TiXmlElement *db = databases->FirstChildElement("db") ; db ; db = db->NextSiblingElement("db"))
  {
    const char *dbID = db->Attribute("id");
    
    if(!dbID || std::string(dbID).empty())
    {
      dqm_error( "ERROR: <db> tag has no 'id' attribute or is empty, line {0}", db->Row() );
      return 1;
    }
    
    if(dbInterfaceMap.find(dbID) != dbInterfaceMap.end())
    {
      dqm_error( "ERROR: <db> tag with id '{0}', line {1}, is duplicated !", dbID, db->Row() );
      return 1;
    }
    
    std::string dbHost = db->Attribute("host") ? db->Attribute("host") : "localhost";
    std::string dbUser = db->Attribute("user") ? db->Attribute("user") : "DQM4HEP";
    std::string dbDb = db->Attribute("db") ? db->Attribute("db") : "DQM4HEP";    
    std::string dbPassword = db->Attribute("password") ? db->Attribute("password") : "";

    DBInterfaceMetadata metadata;
    metadata.m_dbInterface = std::make_shared<DBInterface>();
    metadata.m_host = dbHost;
    metadata.m_user = dbUser;
    metadata.m_db = dbDb;
    metadata.m_password = dbPassword;
    
    dbInterfaceMap[dbID] = metadata; 
  }
  
  int nParameters(0);
  std::set<std::pair<std::string,std::string>> dbTables;
  
  for(TiXmlElement *parameters = root->FirstChildElement("parameters") ; parameters ; parameters = parameters->NextSiblingElement("parameters"))
  {
    std::string dbID = parameters->Attribute("db") ? parameters->Attribute("db") : "";
    std::string dbTable = parameters->Attribute("table") ? parameters->Attribute("table") : "";
    
    auto findIter = dbInterfaceMap.find(dbID);
    
    if(dbInterfaceMap.end() == findIter)
    {
      dqm_error( "ERROR: <parameters> tag, db attribute '{0}' not found in <databases> section, line {1}", dbID, parameters->Row() );
      return 1;
    }
    
    if(!findIter->second.m_dbInterface->isConnected())
    {
      if(findIter->second.m_password.empty())
      {
        std::stringstream passwordText;
        passwordText << "Password [host=" << findIter->second.m_host << " db=" << findIter->second.m_db << " user=" << findIter->second.m_user << "] : ";
        char *password = getpass(passwordText.str().c_str());
        
        if(!password)
        {
          dqm_error( "ERROR: Invalid password !" );
          return 1;
        }
        
        findIter->second.m_password = password;
      }
      
      // connect to db
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
        findIter->second.m_dbInterface->connect(
          findIter->second.m_host, 
          findIter->second.m_user, 
          findIter->second.m_password, 
          findIter->second.m_db)
        );
    }
    
    StringMap parameterMap;
    
    for(TiXmlElement *parameter = parameters->FirstChildElement("parameter") ; parameter ; parameter = parameter->NextSiblingElement("parameter"))
    {
      const std::string name = parameter->Attribute("name");
      std::string value;

      if( parameter->Attribute("value") ) 
      {
        value = parameter->Attribute("value");
      }
      else
      {
        if( parameter->FirstChild() )
          value = parameter->FirstChild()->Value();
      }
      
      parameterMap[name] = value;
      nParameters++;
    }
    
    try
    {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, findIter->second.m_dbInterface->createParameterTable(dbTable, true, false));
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, findIter->second.m_dbInterface->setParameters(dbTable, parameterMap));
    }
    catch(StatusCodeException &exception)
    {
      dqm_error( "MySQL error, couldn't push parameters into table: {0}", exception.toString() );
      return 1;
    }
    
    std::pair<std::string,std::string> dbIdTable(dbID, dbTable);
    dbTables.insert(dbIdTable);
  }
  
  dqm_info( "Successfully pushed {0} parameter(s) into {1} db table(s)" , nParameters, dbTables.size() );

  return 0;
}
