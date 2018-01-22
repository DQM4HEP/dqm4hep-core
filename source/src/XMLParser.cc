/// \file XMLParser.cc
/*
 *
 * XMLParser.cc source template automatically generated by a class generator
 * Creation date : mar. juil. 1 2014
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
#include <dqm4hep/XMLParser.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Logging.h>

namespace dqm4hep {

  namespace core {
    
    XMLParser::XMLParser()
    {
      /* nop */
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::parse(const std::string &fname)
    {
      this->reset();
      m_fileName = fname;
      
      bool loaded = m_document.LoadFile(m_fileName);

      if(!loaded)
      {
        dqm_error( "XMLParser::parse(): couldn't load xml file ['{0}', row: {1}, col: {2}] : {3}",
          m_fileName, m_document.ErrorRow(), m_document.ErrorCol(), m_document.ErrorDesc() );
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if(this->processIncludes())
      {
        try
        {
          this->resolveIncludes();
        }
        catch(StatusCodeException &exception)
        {
          dqm_error( "XMLParser::parse(): fail to resolve includes, {0}", exception.toString() );
          throw exception;
        }        
      }

      if(this->processConstants())
      {
        try
        {
          this->resolveConstants();
        }
        catch(StatusCodeException &exception)
        {
          dqm_error( "XMLParser::parse(): fail to resolve constants, {0}", exception.toString() );
          throw exception;
        }        
      }
      
      if(this->processDatabase())
      {
        try
        {
          this->resolveDbParameters();
        }
        catch(StatusCodeException &exception)
        {
          dqm_error( "XMLParser::parse(): fail to resolve db parameters, {0}", exception.toString() );
          throw exception;
        }        
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    const std::string &XMLParser::fileName() const
    {
      return m_fileName;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    const TiXmlDocument &XMLParser::document() const
    {
      return m_document;      
    }
    
    //----------------------------------------------------------------------------------------------------

    TiXmlDocument &XMLParser::document()
    {
      return m_document;
    }    
    
    //----------------------------------------------------------------------------------------------------

    const StringMap &XMLParser::constants() const
    {
      return m_constants;
    }
    
    //----------------------------------------------------------------------------------------------------

    void XMLParser::reset()
    {
      m_constants.clear();
      m_fileName.clear();
      m_document.Clear();
      m_databases.clear();
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::setProcessIncludes(bool process)
    {
      m_processIncludes = process;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    bool XMLParser::processIncludes() const
    {
      return m_processIncludes;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::setAllowNestedIncludes(bool allow)
    {
      m_allowNestedIncludes = allow;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    bool XMLParser::allowNestedIncludes() const
    {
      return m_allowNestedIncludes;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::setProcessConstants(bool process)
    {
      m_processConstants = process;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    bool XMLParser::processConstants() const
    {
      return m_processConstants;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::setProcessDatabase(bool process)
    {
      m_processDatabase = process;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    bool XMLParser::processDatabase() const
    {
      return m_processDatabase;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::setAllowEnvVariables(bool allow)
    {
      m_allowEnvVariables = allow;
    }
    
    //----------------------------------------------------------------------------------------------------

    bool XMLParser::allowEnvVariables() const
    {
      return m_allowEnvVariables;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveIncludes()
    {
      TiXmlElement *root = m_document.RootElement();
      
      std::string filePath;
      this->getFilePath(m_fileName, filePath);
      
      try
      {
        this->resolveIncludes(filePath, root);
      }
      catch(StatusCodeException &exception)
      {
        dqm_error( "XMLParser::resolveIncludes(): fail to resolve include in root document '{0}', {1}", m_fileName, exception.toString() );
        throw exception;
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveConstants()
    {
      this->readConstants(&m_document);
      this->replaceAllConstants();
    }
    
    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveDbParameters()
    {
      this->readDatabases();
      this->resolveAllDbSelect(this->document().RootElement());
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveIncludes(const std::string &referencePath, TiXmlNode* node)
    {
      TiXmlElement* child = node->FirstChildElement();

      while( 1 )
      {
        if( ! child )
          break;

        if( child->Value() != std::string("include") ) 
        {
          this->resolveIncludes(referencePath, child);
          child = child->NextSiblingElement();
          continue;
        }

        // process the include and returns the parsed document
        TiXmlDocument document;
        this->resolveInclude(referencePath, child, document);
        
        TiXmlElement *rootInclude = document.RootElement();
        
        if( std::string(rootInclude->Value()) != "dqm4hep" )
        {
          dqm_error( "XMLParser::resolveIncludes(): no <dqm4hep> root element in included file '{0}'", document.Value() );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        // add them to the xml tree
        TiXmlNode *includeAfter(child);

        for(TiXmlElement *includeElement = rootInclude->FirstChildElement() ; includeElement ; includeElement =  includeElement->NextSiblingElement())
          includeAfter = node->InsertAfterChild( includeAfter, *includeElement );

        // tricky lines :
        // 1) Remove the include element
        node->RemoveChild(child);
        // 2) Go to the next sibling element of the last included element to skip nested <include> elements
        child = includeAfter->NextSiblingElement();
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveInclude(const std::string &referencePath, TiXmlElement* element, TiXmlDocument &document)
    {
      if( ! element->Attribute("ref") ) {
        dqm_error( "XMLParser::resolveInclude(): missing attribute \"ref\" in element <{0}/>", element->Value() );
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      std::string ref ( element->Attribute("ref") ) ;

      if( ref.size() < 5 || ref.substr( ref.size() - 4 ) != ".xml" ) {
        dqm_error( "XMLParser::resolveInclude(): invalid ref file name \"{0}\" in element <{1}/>", ref, element->Value() );
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      std::string refFileName;
      this->getRelativeFileName(ref, referencePath, refFileName);

      bool loadOkay = document.LoadFile( refFileName ) ;

      if( !loadOkay ) {
        dqm_error( "XMLParser::resolveInclude(): error in file [{0}, row: {1}, col: {2}] : {3}", refFileName, document.ErrorRow(), document.ErrorCol(), document.ErrorDesc() );
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      
      // process nested includes if enabled
      if(this->allowNestedIncludes())
      {
        std::string includeReferencePath;
        this->getFilePath(refFileName, includeReferencePath);        
        this->resolveIncludes(includeReferencePath, &document);
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::getFilePath(const std::string &fileName, std::string &filePath)
    {
      filePath = "";
      size_t idx = fileName.find_last_of("/");
      
      if(idx != std::string::npos)
          filePath = fileName.substr(0, idx);
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::getRelativeFileName(const std::string &fileName, const std::string &relativeTo, std::string &relativeFileName)
    {
      if( fileName.at(0) != '/' )
        relativeFileName = relativeTo + "/" + fileName;
      else
        relativeFileName = fileName;
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::readConstants(TiXmlNode *node)
    {
      for( TiXmlElement* element = node->FirstChildElement() ; element ; element = element->NextSiblingElement() )
      {
        if ( element->Value() == std::string("constants") )
          this->readConstantsSection(element);
        else
          this->readConstants(element);
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::replaceAllConstants()
    {
      TiXmlElement *root = m_document.RootElement();
      
      for( TiXmlElement* element = root->FirstChildElement() ; element ; element = element->NextSiblingElement() )
        this->resolveConstantsInElement(element);
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::readConstantsSection(TiXmlElement *constants)
    {
      for( TiXmlElement* element = constants->FirstChildElement("constant") ; element ; element = element->NextSiblingElement("constant") )
      {
        this->resolveConstantsInElement(element);
        
        if( ! element->Attribute("name") )
        {
          dqm_error( "XMLParser::readConstantsSection(): constant element without name !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        std::string name( element->Attribute("name") );

        if( name.empty() ) 
        {
          dqm_error( "XMLParser::readConstantsSection(): parsed empty constant name !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        
        if( m_constants.end() != m_constants.find( name ) ) 
        {
          dqm_error( "XMLParser::readConstantsSection(): constant \"{0}\" defined twice !", name );
          throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }

        std::string value;

        if( element->Attribute("value") ) 
        {
          value = element->Attribute("value");
        }
        else
        {
          if( element->FirstChild() )
            value = element->FirstChild()->Value();
        }

        if( ! m_constants.insert( StringMap::value_type(name, value) ).second ) 
        {
          dqm_error( "XMLParser::readConstantsSection(): couldn't add constant \"{0}\" to constant map !", name );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::performConstantReplacement(std::string &value)
    {        
      size_t pos = value.find("${");
      std::string original(value);

      while( pos != std::string::npos ) 
      {
        size_t pos2 = value.find_first_of( "}", pos+2 );

        if( pos2 == std::string::npos ) 
        {
          dqm_error( "XMLParser::performConstantReplacement(): couldn't parse constant value !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const std::string key( value.substr( pos+2 , pos2-pos-2 ));
        auto findConstant = m_constants.find( key );
        const std::string replacementValue( findConstant != m_constants.end() ? findConstant->second : "" );

        if( replacementValue.empty() ) {
          dqm_error( "XMLParser::performConstantReplacement(): constant \"{0}\" not found in available constants !", key );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        value.replace( pos , (pos2+1-pos) , replacementValue );
        pos2 = pos + replacementValue.size(); // new end position after replace
        pos = value.find("${", pos2); // find next possible key to replace
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::performEnvVariableReplacement(std::string &value)
    {
      if(!this->allowEnvVariables())
        return;
      
      size_t pos = value.find("$ENV{");
      std::string original(value);

      while( pos != std::string::npos ) 
      {
        size_t pos2 = value.find_first_of( "}", pos+5 );

        if( pos2 == std::string::npos ) 
        {
          dqm_error( "XMLParser::performEnvVariableReplacement(): couldn't parse env variable name !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const std::string key( value.substr( pos+5 , pos2-pos-5 ));
        char *env = getenv(key.c_str());
        
        if(!env)
        {
          dqm_error( "XMLParser::performEnvVariableReplacement(): env variable \"{0}\" not found !", key );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        
        const std::string replacementValue(env);

        value.replace( pos , (pos2+1-pos) , replacementValue );
        pos2 = pos + replacementValue.size(); // new end position after replace
        pos = value.find("$ENV{", pos2); // find next possible key to replace
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveConstantsInElement(TiXmlElement *element)
    {
      // replace constants in all attributes
      for(TiXmlAttribute *attribute = element->FirstAttribute(); attribute ; attribute = attribute->Next())
      {
        std::string value(attribute->ValueStr());
        
        this->performConstantReplacement(value);
        
        if(this->allowEnvVariables())
          this->performEnvVariableReplacement(value);
        
        attribute->SetValue(value);
      }

      for(TiXmlNode *node = element->FirstChild() ; node ; node = node->NextSibling())
      {
        if(node->Type() == TiXmlNode::TINYXML_ELEMENT)
        {
          TiXmlElement *child = node->ToElement();
          this->resolveConstantsInElement(child);
        }
        else if(node->Type() == TiXmlNode::TINYXML_TEXT)
        {
          TiXmlText *child = node->ToText();
          std::string value(child->ValueStr());
          
          this->performConstantReplacement(value);
          
          if(this->allowEnvVariables())
            this->performEnvVariableReplacement(value);
          
          child->SetValue(value);
        }
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::readDatabases()
    {
      TiXmlDocument &document(this->document());
      
      TiXmlElement *databases = document.RootElement()->FirstChildElement("databases");
      
      if(!databases)
        return;
      
      for(TiXmlElement *db = databases->FirstChildElement("db") ; db ; db = db->NextSiblingElement("db"))
      {
        const char *dbID = db->Attribute("id");
        
        if(!dbID || std::string(dbID).empty())
        {
          dqm_error( "ERROR: <db> tag has no 'id' attribute or is empty, line {0}", db->Row() );
          throw StatusCodeException(STATUS_CODE_NOT_FOUND);
        }
        
        if(m_databases.find(dbID) != m_databases.end())
        {
          dqm_error( "ERROR: <db> tag with id '{0}', line {1}, is duplicated !", dbID, db->Row() );
          throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }
        
        std::string dbHost = db->Attribute("host") ? db->Attribute("host") : "localhost";
        std::string dbUser = "DQM4HEP"; // always has read access
        std::string dbDb = db->Attribute("db") ? db->Attribute("db") : "DQM4HEP";
        
        std::shared_ptr<DBInterface> dbInterface(std::make_shared<DBInterface>());
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, dbInterface->connect(dbHost, dbUser, "", dbDb));
        
        m_databases[dbID] = dbInterface;
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveAllDbSelect(TiXmlElement *pXmlElement)
    {
      if(m_databases.empty())
        return;
      
      for( TiXmlElement* element = pXmlElement->FirstChildElement() ; element ; element = element->NextSiblingElement() )
      {
        if(std::string(element->Value()) == "select")
        {
          std::string dbID = element->Attribute("db") ? element->Attribute("db") : "";
          std::string dbTable = element->Attribute("table") ? element->Attribute("table") : "";
          
          if(dbID.empty() || dbTable.empty())
          {
            dqm_error( "<select> element, row {0}, column {1}: invalid/missing attributes 'db' or 'table'", element->Row(), element->Column() );
            throw StatusCodeException(STATUS_CODE_FAILURE);
          }
          
          auto findIter = m_databases.find(dbID);
          
          if(m_databases.end() == findIter)
          {
            dqm_error( "<select> element, row {0}, column {1}: db id '{2}' not found !", element->Row(), element->Column(), dbID );
            throw StatusCodeException(STATUS_CODE_NOT_FOUND);
          }
          
          std::shared_ptr<DBInterface> dbInterface(findIter->second);
          StringMap parameterMap;
          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, dbInterface->getTableParameters(dbTable, parameterMap));
          
          TiXmlNode *pInsertAfter(element), *pParentElement(element->Parent()->ToElement());
          
          for(auto parameterIter : parameterMap)
          {
            dqm_info( "Selected parameter '{0}' = '{1}'", parameterIter.first, parameterIter.second );
            
            TiXmlElement *pParameterElement = new TiXmlElement("parameter");
            pParameterElement->SetAttribute("name", parameterIter.first);
            pParameterElement->SetAttribute("value", parameterIter.second);
            pInsertAfter = pParentElement->InsertAfterChild(pInsertAfter, *pParameterElement);
            delete pParameterElement;
          }
          
          pParentElement->RemoveChild(element);
        }
        else
        {
          // recursive call on childs !
          this->resolveAllDbSelect(element);
        }
      }
    }

  }

}