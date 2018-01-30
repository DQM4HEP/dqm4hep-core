/// \file DQMVersion.h
/*
 *
 * DQMVersion.h header template automatically generated by a class generator
 * Creation date : lun. juin 8 2015
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


#ifndef DQM4HEP_VERSION_H
#define DQM4HEP_VERSION_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

namespace dqm4hep {

  namespace core {

    /** Version class
     */
    class Version : public xdrstream::Streamable
    {
    public:
      /** Constructor
       */
      Version();

      /** Constructor with version numbers
       */
      Version( unsigned int major, unsigned int minor, unsigned int patch );

      /** Destructor
       */
      ~Version() override;

      /** Get the major version number
       */
      unsigned int getMajor() const;

      /** Get the minor version number
       */
      unsigned int getMinor() const;

      /** Get the patch version number
       */
      unsigned int getPatch() const;

      /** Set the complete version numbers
       */
      void set( unsigned int major, unsigned int minor, unsigned int patch );

      /** Get the version to string
       */
      const std::string &toString() const;

      /** Assignment operator
       */
      Version &operator =( const Version &version );

      // from xdrstreamm::Streamable
      xdrstream::Status stream(xdrstream::StreamingMode mode, xdrstream::IODevice *pDevice,
          xdrstream::xdr_version_t version = 0) override;

    private:
      unsigned int    m_major;
      unsigned int    m_minor;
      unsigned int    m_patch;
      std::string     m_versionString;
    };

    // compare operators
    bool operator <  ( const Version &lhs, const Version &rhs );
    bool operator <= ( const Version &lhs, const Version &rhs );
    bool operator >  ( const Version &lhs, const Version &rhs );
    bool operator >= ( const Version &lhs, const Version &rhs );
    bool operator == ( const Version &lhs, const Version &rhs );
    bool operator != ( const Version &lhs, const Version &rhs );

  }

}

#endif  //  DQM4HEP_VERSION_H
