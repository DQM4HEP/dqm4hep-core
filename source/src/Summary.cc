  /// \file Summary.cc
/*
 *
 * Summary.cc source template automatically generated by a class generator
 * Creation date : jeu. f�vr. 2 2017
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


#include "dqm4hep/Summary.h"

namespace dqm4hep {

  namespace core {

    Summary::Summary() :
      MonitorObject()
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Summary::Summary(const std::string &header) :
      MonitorObject(),
      m_header(header)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Summary::~Summary()
    {
      clear();
    }

    //-------------------------------------------------------------------------------------------------

    void Summary::setHeader(const std::string &header)
    {
      m_header = header;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Summary::getHeader() const
    {
      return m_header;
    }

    //-------------------------------------------------------------------------------------------------

    void Summary::setEntry(const std::string &entry, const std::string &text)
    {
      m_entries[entry] = text;
    }

    //-------------------------------------------------------------------------------------------------

    void Summary::removeEntry(const std::string &entry)
    {
      auto findIter = m_entries.find(entry);

      if(findIter != m_entries.end())
        m_entries.erase(findIter);
    }

    //-------------------------------------------------------------------------------------------------

    void Summary::clear()
    {
      m_entries.clear();
      m_header.clear();
    }

  }

}