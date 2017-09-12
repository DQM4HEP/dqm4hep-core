/*
 *
 * DQMPluginManager.h header template automatically generated by a class generator
 * Creation date : lun. ao�t 17 2015
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


#ifndef DQM4HEP_PLUGINMANAGER_H
#define DQM4HEP_PLUGINMANAGER_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Singleton.h"
#include "dqm4hep/Plugin.h"

//-------------------------------------------------------------------------------------------------

#define DQM_PLUGIN_DECL( ClassName , ClassStr ) \
    class DQMPlugin_##ClassName : public dqm4hep::core::Plugin, public ClassName \
    { \
    public: \
    DQMPlugin_##ClassName (bool shouldRegister = true) : dqm4hep::core::Plugin(ClassStr, shouldRegister), ClassName() {} \
    dqm4hep::core::Plugin *create() const { return new DQMPlugin_##ClassName (false) ; } \
    }; \
    static DQMPlugin_##ClassName instance_DQMPlugin_##ClassName

//-------------------------------------------------------------------------------------------------

namespace dqm4hep {

  namespace core {

    class Plugin;

    /** PluginManager class.
     *  Responsible for loading shared libraries
     *  that contains Plugin instances
     */
    class PluginManager : public Singleton<PluginManager>
    {
      friend class Singleton<PluginManager>;
      friend class Plugin;

      typedef std::map<const std::string, Plugin*> PluginMap;

    public:
      /** Load shared libraries from the environment variable 4HEP_PLUGIN_DLL
       */
      StatusCode loadLibraries();

      /** Load the shared libraries
       */
      StatusCode loadLibraries( const StringVector &libraryNameList );

      /** Load the shared library
       */
      StatusCode loadLibrary( const std::string &libraryName );

      /** Get the plug-in by name
       */
      const Plugin *getPlugin( const std::string &pluginName ) const;

      /** Get the plug-in clone. A new plug-in instance is allocated and returned
       *  to the user. Ownership of the plug-in transfered to the caller.
       */
      template <typename T>
      T *createPluginClass( const std::string &pluginName ) const;

      /** Whether the plug-in is registered within the plug-in manager
       */
      bool isPluginRegistered( const std::string &pluginName ) const;

      /** Get the plug-in name list
       */
      StringVector getPluginNameList() const;

    private:
      /** Constructor
       */
      PluginManager();

      /** Destructor
       */
      ~PluginManager();

      /** Register the plug-in. Only available by plug-ins themselves
       */
      StatusCode registerPlugin( Plugin *pPlugin );

    private:
      PluginMap              m_pluginMap;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *PluginManager::createPluginClass( const std::string &pluginName ) const
    {
      const Plugin *pPlugin = this->getPlugin( pluginName );

      if(!pPlugin)
        return 0;

      Plugin *pClass = pPlugin->create();

      if(NULL == pClass)
        return 0;

      return dynamic_cast<T *>(pClass);
    }

  }

}

#endif  //  DQM4HEP_PLUGINMANAGER_H
