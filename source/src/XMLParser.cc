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
#include <dqm4hep/Logging.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XMLParser.h>

namespace dqm4hep {

  namespace core {

    XMLParser::XMLParser() {
      /* nop */
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::parse(const std::string &fname) {
      this->reset();
      m_fileName = fname;

      bool loaded = m_document.LoadFile(m_fileName);

      if (!loaded) {
        dqm_error("XMLParser::parse(): couldn't load xml file ['{0}', row: {1}, col: {2}] : {3}", m_fileName,
                  m_document.ErrorRow(), m_document.ErrorCol(), m_document.ErrorDesc());
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if (this->processIncludes()) {
        try {
          this->resolveIncludes();
        } catch (StatusCodeException &exception) {
          dqm_error("XMLParser::parse(): fail to resolve includes, {0}", exception.toString());
          throw exception;
        }
      }

      if (this->processConstants()) {
        try {
          this->resolveConstants();
        } catch (StatusCodeException &exception) {
          dqm_error("XMLParser::parse(): fail to resolve constants, {0}", exception.toString());
          throw exception;
        }
      }

      if (this->processDatabase()) {
        try {
          this->resolveDbParameters();
        } catch (StatusCodeException &exception) {
          dqm_error("XMLParser::parse(): fail to resolve db parameters, {0}", exception.toString());
          throw exception;
        }
      }
      
      try {
        this->resolveForLoops();
      } catch (StatusCodeException &exception) {
        dqm_error("XMLParser::parse(): fail to resolve for loops, {0}", exception.toString());
        throw exception;
      }
    }

    //----------------------------------------------------------------------------------------------------

    const std::string &XMLParser::fileName() const {
      return m_fileName;
    }

    //----------------------------------------------------------------------------------------------------

    const TiXmlDocument &XMLParser::document() const {
      return m_document;
    }

    //----------------------------------------------------------------------------------------------------

    TiXmlDocument &XMLParser::document() {
      return m_document;
    }

    //----------------------------------------------------------------------------------------------------

    const StringMap &XMLParser::constants() const {
      return m_constants;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::reset() {
      m_constants.clear();
      m_fileName.clear();
      m_document.Clear();
      m_databases.clear();
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessIncludes(bool process) {
      m_processIncludes = process;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processIncludes() const {
      return m_processIncludes;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setAllowNestedIncludes(bool allow) {
      m_allowNestedIncludes = allow;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::allowNestedIncludes() const {
      return m_allowNestedIncludes;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessConstants(bool process) {
      m_processConstants = process;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processConstants() const {
      return m_processConstants;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setProcessDatabase(bool process) {
      m_processDatabase = process;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::processDatabase() const {
      return m_processDatabase;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::setAllowEnvVariables(bool allow) {
      m_allowEnvVariables = allow;
    }

    //----------------------------------------------------------------------------------------------------

    bool XMLParser::allowEnvVariables() const {
      return m_allowEnvVariables;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveIncludes() {
      TiXmlElement *root = m_document.RootElement();

      std::string filePath;
      this->getFilePath(m_fileName, filePath);

      try {
        this->resolveIncludes(filePath, root);
      } catch (StatusCodeException &exception) {
        dqm_error("XMLParser::resolveIncludes(): fail to resolve include in root document '{0}', {1}", m_fileName,
                  exception.toString());
        throw exception;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveConstants() {
      this->readConstants(&m_document);
      this->replaceAllConstants();
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveDbParameters() {
      this->readDatabases();
      this->resolveAllDbSelect(this->document().RootElement());
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveIncludes(const std::string &referencePath, TiXmlNode *node) {
      TiXmlElement *child = node->FirstChildElement();

      while (true) {
        if (child == nullptr)
          break;

        if (child->Value() != std::string("include")) {
          this->resolveIncludes(referencePath, child);
          child = child->NextSiblingElement();
          continue;
        }

        // process the include and returns the parsed document
        TiXmlDocument doc;
        this->resolveInclude(referencePath, child, doc);

        TiXmlElement *rootInclude = doc.RootElement();

        if (std::string(rootInclude->Value()) != "dqm4hep") {
          dqm_error("XMLParser::resolveIncludes(): no <dqm4hep> root element in included file '{0}'", doc.Value());
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        // add them to the xml tree
        TiXmlNode *includeAfter(child);

        for (TiXmlElement *includeElement = rootInclude->FirstChildElement(); includeElement != nullptr;
             includeElement = includeElement->NextSiblingElement())
          includeAfter = node->InsertAfterChild(includeAfter, *includeElement);

        // case where at least one element has been inserted from the include file
        if(includeAfter != child) {
          node->RemoveChild(child);
          child = includeAfter->NextSiblingElement();          
        }
        // case where nothing was inserted
        else {
          dqm_debug( "XMLParser::resolveIncludes(): No element in included file" );
          // Find the previous element to point to if any
          TiXmlElement *previousElement(nullptr);
          TiXmlNode *previousNode(child);
          while(nullptr != previousNode->PreviousSibling()) {
            if(nullptr != previousNode->PreviousSibling()->ToElement()) {
              previousElement = previousNode->PreviousSibling()->ToElement();
              break;
            }
            else {
              previousNode = previousNode->PreviousSibling();
            }
          }
          // node->RemoveChild(child);
          // if a previous element is available, 
          // continue the while from it
          if(nullptr != previousElement) {
            dqm_debug( "XMLParser::resolveIncludes(): Previous element found" );
            std::cout << "Child: " << child->ValueStr() << std::endl;
            std::cout << "Previous: " << previousElement->ValueStr() << std::endl;
            node->RemoveChild(child);
            child = previousElement;
          }
          // if no previous element is available,
          // restart from the first child of input
          else {
            dqm_debug( "XMLParser::resolveIncludes(): No previous element" );
            node->RemoveChild(child);
            child = node->FirstChildElement();
          }
        }

      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveInclude(const std::string &referencePath, TiXmlElement *element, TiXmlDocument &doc) {
      if (element->Attribute("ref") == nullptr) {
        dqm_error("XMLParser::resolveInclude(): missing attribute \"ref\" in element <{0}/>", element->Value());
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      std::string ref(element->Attribute("ref"));

      if (ref.size() < 5 || ref.substr(ref.size() - 4) != ".xml") {
        dqm_error("XMLParser::resolveInclude(): invalid ref file name \"{0}\" in element <{1}/>", ref,
                  element->Value());
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      std::string refFileName;
      this->getRelativeFileName(ref, referencePath, refFileName);

      bool loadOkay = doc.LoadFile(refFileName);

      if (!loadOkay) {
        dqm_error("XMLParser::resolveInclude(): error in file [{0}, row: {1}, col: {2}] : {3}", refFileName,
                  doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc());
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      // process nested includes if enabled
      if (this->allowNestedIncludes()) {
        std::string includeReferencePath;
        this->getFilePath(refFileName, includeReferencePath);
        this->resolveIncludes(includeReferencePath, &doc);
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::getFilePath(const std::string &fName, std::string &filePath) {
      filePath = "";
      size_t idx = fName.find_last_of("/");

      if (idx != std::string::npos)
        filePath = fName.substr(0, idx);
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::getRelativeFileName(const std::string &fName, const std::string &relativeTo,
                                        std::string &relativeFileName) {
      if (fName.at(0) != '/')
        relativeFileName = relativeTo + "/" + fName;
      else
        relativeFileName = fName;
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::readConstants(TiXmlNode *node) {
      for (TiXmlElement *element = node->FirstChildElement(); element != nullptr;
           element = element->NextSiblingElement()) {
        if (element->Value() == std::string("constants"))
          this->readConstantsSection(element);
        else
          this->readConstants(element);
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::replaceAllConstants() {
      TiXmlElement *root = m_document.RootElement();

      for (TiXmlElement *element = root->FirstChildElement(); element != nullptr;
           element = element->NextSiblingElement())
        this->resolveConstantsInElement(element);
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::readConstantsSection(TiXmlElement *constantsElement) {
      for (TiXmlElement *element = constantsElement->FirstChildElement("constant"); element != nullptr;
           element = element->NextSiblingElement("constant")) {
        this->resolveConstantsInElement(element);

        if (element->Attribute("name") == nullptr) {
          dqm_error("XMLParser::readConstantsSection(): constant element without name !");
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        std::string name(element->Attribute("name"));

        if (name.empty()) {
          dqm_error("XMLParser::readConstantsSection(): parsed empty constant name !");
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        if (m_constants.end() != m_constants.find(name)) {
          dqm_error("XMLParser::readConstantsSection(): constant \"{0}\" defined twice !", name);
          throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }

        std::string value;

        if (element->Attribute("value") != nullptr) {
          value = element->Attribute("value");
        } else {
          if (element->FirstChild() != nullptr)
            value = element->FirstChild()->Value();
        }

        if (!m_constants.insert(StringMap::value_type(name, value)).second) {
          dqm_error("XMLParser::readConstantsSection(): couldn't add constant \"{0}\" to constant map !", name);
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::performConstantReplacement(std::string &value) {
      size_t pos = value.find("${");
      std::string original(value);

      while (pos != std::string::npos) {
        size_t pos2 = value.find_first_of("}", pos + 2);

        if (pos2 == std::string::npos) {
          dqm_error("XMLParser::performConstantReplacement(): couldn't parse constant value !");
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const std::string key(value.substr(pos + 2, pos2 - pos - 2));
        auto findConstant = m_constants.find(key);
        const std::string replacementValue(findConstant != m_constants.end() ? findConstant->second : "");

        if (replacementValue.empty()) {
          dqm_error("XMLParser::performConstantReplacement(): constant \"{0}\" not found in available constants !",
                    key);
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        value.replace(pos, (pos2 + 1 - pos), replacementValue);
        pos2 = pos + replacementValue.size(); // new end position after replace
        pos = value.find("${", pos2);         // find next possible key to replace
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::performEnvVariableReplacement(std::string &value) {
      if (!this->allowEnvVariables())
        return;

      size_t pos = value.find("$ENV{");
      std::string original(value);

      while (pos != std::string::npos) {
        size_t pos2 = value.find_first_of("}", pos + 5);

        if (pos2 == std::string::npos) {
          dqm_error("XMLParser::performEnvVariableReplacement(): couldn't parse env variable name !");
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const std::string key(value.substr(pos + 5, pos2 - pos - 5));
        char *env = getenv(key.c_str());

        if (env == nullptr) {
          dqm_error("XMLParser::performEnvVariableReplacement(): env variable \"{0}\" not found !", key);
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const std::string replacementValue(env);

        value.replace(pos, (pos2 + 1 - pos), replacementValue);
        pos2 = pos + replacementValue.size(); // new end position after replace
        pos = value.find("$ENV{", pos2);      // find next possible key to replace
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveConstantsInElement(TiXmlElement *element) {
      // replace constants in all attributes
      for (TiXmlAttribute *attribute = element->FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        std::string value(attribute->ValueStr());

        this->performConstantReplacement(value);

        if (this->allowEnvVariables())
          this->performEnvVariableReplacement(value);

        attribute->SetValue(value);
      }

      for (TiXmlNode *node = element->FirstChild(); node != nullptr; node = node->NextSibling()) {
        if (node->Type() == TiXmlNode::TINYXML_ELEMENT) {
          TiXmlElement *child = node->ToElement();
          this->resolveConstantsInElement(child);
        } else if (node->Type() == TiXmlNode::TINYXML_TEXT) {
          TiXmlText *child = node->ToText();
          std::string value(child->ValueStr());

          this->performConstantReplacement(value);

          if (this->allowEnvVariables())
            this->performEnvVariableReplacement(value);

          child->SetValue(value);
        }
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::readDatabases() {
      TiXmlDocument &doc(this->document());

      TiXmlElement *databases = doc.RootElement()->FirstChildElement("databases");

      if (databases == nullptr)
        return;

      for (TiXmlElement *db = databases->FirstChildElement("db"); db != nullptr; db = db->NextSiblingElement("db")) {
        const char *dbID = db->Attribute("id");

        if ((dbID == nullptr) || std::string(dbID).empty()) {
          dqm_error("ERROR: <db> tag has no 'id' attribute or is empty, line {0}", db->Row());
          throw StatusCodeException(STATUS_CODE_NOT_FOUND);
        }

        if (m_databases.find(dbID) != m_databases.end()) {
          dqm_error("ERROR: <db> tag with id '{0}', line {1}, is duplicated !", dbID, db->Row());
          throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }

        std::string dbHost = db->Attribute("host") != nullptr ? db->Attribute("host") : "localhost";
        std::string dbUser = "DQM4HEP"; // always has read access
        std::string dbDb = db->Attribute("db") != nullptr ? db->Attribute("db") : "DQM4HEP";

        std::shared_ptr<DBInterface> dbInterface(std::make_shared<DBInterface>());
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, dbInterface->connect(dbHost, dbUser, "", dbDb));

        m_databases[dbID] = dbInterface;
      }
    }

    //----------------------------------------------------------------------------------------------------

    void XMLParser::resolveAllDbSelect(TiXmlElement *pXmlElement) {
      if (m_databases.empty())
        return;

      for (TiXmlElement *element = pXmlElement->FirstChildElement(); element != nullptr;
           element = element->NextSiblingElement()) {
        if (std::string(element->Value()) == "select") {
          std::string dbID = element->Attribute("db") != nullptr ? element->Attribute("db") : "";
          std::string dbTable = element->Attribute("table") != nullptr ? element->Attribute("table") : "";

          if (dbID.empty() || dbTable.empty()) {
            dqm_error("<select> element, row {0}, column {1}: invalid/missing attributes 'db' or 'table'",
                      element->Row(), element->Column());
            throw StatusCodeException(STATUS_CODE_FAILURE);
          }

          auto findIter = m_databases.find(dbID);

          if (m_databases.end() == findIter) {
            dqm_error("<select> element, row {0}, column {1}: db id '{2}' not found !", element->Row(),
                      element->Column(), dbID);
            throw StatusCodeException(STATUS_CODE_NOT_FOUND);
          }

          std::shared_ptr<DBInterface> dbInterface(findIter->second);
          StringMap parameterMap;
          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, dbInterface->getTableParameters(dbTable, parameterMap));

          TiXmlNode *pInsertAfter(element), *pParentElement(element->Parent()->ToElement());

          for (auto parameterIter : parameterMap) {
            dqm_info("Selected parameter '{0}' = '{1}'", parameterIter.first, parameterIter.second);

            auto pParameterElement = new TiXmlElement("parameter");
            pParameterElement->SetAttribute("name", parameterIter.first);
            pParameterElement->SetAttribute("value", parameterIter.second);
            pInsertAfter = pParentElement->InsertAfterChild(pInsertAfter, *pParameterElement);
            delete pParameterElement;
          }

          TiXmlElement *previousElement(nullptr);
          TiXmlNode *previousNode(element);
          
          while(nullptr != previousNode->PreviousSibling()) {
            if(nullptr != previousNode->PreviousSibling()->ToElement()) {
              previousElement = previousNode->PreviousSibling()->ToElement();
              break;
            }
            else {
              previousNode = previousNode->PreviousSibling();
            }
          }
          // if a previous element is available, 
          // continue the loop from it
          if(nullptr != previousElement) {
            pParentElement->RemoveChild(element);
            element = previousElement;
          }
          // if no previous element is available,
          // restart from the first child of input
          else {
            pParentElement->RemoveChild(element);
            element = pParentElement->FirstChildElement();
          }
        } else {
          // recursive call on childs !
          this->resolveAllDbSelect(element);
        }
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveForLoops() {
      TiXmlElement *root = m_document.RootElement();
      resolveForLoops(root);
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveForLoops(TiXmlElement *element) {
      // first, resolve for loops in child elements 
      for(auto child = element->FirstChildElement(); nullptr != child ; child = child->NextSiblingElement()) {
        resolveForLoops(child);
      }
      // if loop element, resolve it
      if(element->Value() == std::string("for")) {
        auto parent = element->Parent();
        if(nullptr == parent) {
          return;
        }
        int begin(0), end(0), increment(0);
        std::string loopId;
        if(TIXML_SUCCESS != element->QueryIntAttribute("begin", &begin)) {
          dqm_error( "XMLParser::resolveForLoops: <for> element with invalid 'begin' attribute !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        if(TIXML_SUCCESS != element->QueryIntAttribute("end", &end)) {
          dqm_error( "XMLParser::resolveForLoops: <for> element with invalid 'end' attribute !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        if(TIXML_SUCCESS != element->QueryStringAttribute("id", &loopId) or loopId.empty()) {
          dqm_error( "XMLParser::resolveForLoops: <for> element with invalid 'id' attribute !" );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        // optional attribute
        element->QueryIntAttribute("increment", &increment);
        const bool forwardLoop(begin < end);
        if((forwardLoop and increment < 0) or (not forwardLoop and increment > 0)) {
          dqm_error( "XMLParser::resolveForLoops: <for> element with infinite loop detected !" );
          throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
        }
        std::cout << "Found a valid for loop: id = " << loopId << std::endl; 
        for(int i=begin ; i<=end ; i+=increment) {
          for(auto child = element->FirstChild() ; nullptr != child ; child = child->NextSibling()) {
            auto cloneChild = child->Clone();
            if(nullptr != cloneChild) {
              std::cout << "Resolve ..." << std::endl;
              resolveForLoop(cloneChild, loopId, i);
              std::cout << "Resolve ... OK" << std::endl;
              parent->InsertBeforeChild(element, *cloneChild);
              delete cloneChild;
            }            
          }
        }
        // remove the element itself from the tree
        parent->RemoveChild(element);
      }
    }
    
    //----------------------------------------------------------------------------------------------------
    
    void XMLParser::resolveForLoop(TiXmlNode *node, const std::string &id, int value) {
      std::string loopIDStr = "$FOR{" + id + "}";
      std::string valueStr = typeToString(value);
      if(node->Type() == TiXmlNode::TINYXML_ELEMENT) {
        TiXmlElement *nodeElement = node->ToElement();
        // replace occurences in all attributes
        for(auto attr = nodeElement->FirstAttribute() ; nullptr != attr ; attr = attr->Next()) {
          std::string attrValue(attr->ValueStr());
          size_t pos = attrValue.find(loopIDStr, 0);
          while(pos != std::string::npos) {
            attrValue.replace(pos, loopIDStr.size(), valueStr);
            pos = attrValue.find(loopIDStr, pos+valueStr.size());
          }
          attr->SetValue(attrValue);
        }
        // make it recursive
        for(auto child = nodeElement->FirstChild() ; nullptr != child ; child = child->NextSibling()) {
          resolveForLoop(child, id, value);
        }
      }
      else if((node->Type() == TiXmlNode::TINYXML_COMMENT) or (node->Type() == TiXmlNode::TINYXML_TEXT)) {
        std::string nodeValue = node->ValueStr();
        size_t pos = nodeValue.find(loopIDStr, 0);
        while(pos != std::string::npos) {
          nodeValue.replace(pos, loopIDStr.size(), valueStr);
          pos = nodeValue.find(loopIDStr, pos+valueStr.size());
        }
        node->SetValue(nodeValue);
      }
    }
  }
}
