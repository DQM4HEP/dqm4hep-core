/// \file DQMGenericEvent.h
/*
 *
 * DQMGenericEvent.h header template automatically generated by a class generator
 * Creation date : lun. mars 7 2016
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


#ifndef DQM4HEP_GENERICEVENT_H
#define DQM4HEP_GENERICEVENT_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/Event.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

namespace dqm4hep {

  namespace core {

    class GenericEventStreamer;

    /** GenericEvent class
     *
     *  Basic event implementation with a list of vectors stored in maps.
     *  See GenericEvent::setValues() and GenericEvent::getValues() to
     *  respectively set and get values into these map.
     *
     *  To get this kind of event within an analysis module, proceed like this :
     *
     *  \code
     *
     *  StatusCode MyModule::processEvent(Event *pEvent) {
     *
     *  	GenericEvent *pGenericEvent = pEvent->getEvent<GenericEvent>();
     *
     *      // Access contents via getValues()
     *  	FloatVector temperatures;
     *  	pGenericEvent->getValues("Temperature", temperatures);
     *  	// ...
     *  }
     *
     *  \endcode
     */
    class GenericEvent
    {
    public:
      /** Constructor
       */
      GenericEvent();

      /** Destructor
       */
      ~GenericEvent();

      /** Set a vector of values identified by key.
       *
       *  Attention : Template interface restricted to the following types :
       *    - vector<int>
       *    - vector<float>
       *    - vector<double>
       *    - vector<string>
       *  In case where an another parameter type is passed, the code will compile
       *  but will do nothing and return failure.
       */
      template <typename T>
      StatusCode setValues(const std::string &key, const T &vals);

      /** Get a vector of values identified by key.
       *
       *  Attention : Template interface restricted to the following types :
       *    - vector<int>
       *    - vector<float>
       *    - vector<double>
       *    - vector<string>
       *  In case where an another parameter type is passed, the code will compile
       *  but will do nothing and return failure.
       */
      template <typename T>
      StatusCode getValues(const std::string &key, T &vals) const;

    private:
      typedef  std::map<std::string, IntVector> IntVectorMap;
      typedef  std::map<std::string, FloatVector> FloatVectorMap;
      typedef  std::map<std::string, DoubleVector> DoubleVectorMap;
      typedef  std::map<std::string, StringVector> StringVectorMap;

      IntVectorMap                    m_intValues = {};
      FloatVectorMap                  m_floatValues = {};
      DoubleVectorMap                 m_doubleValues = {};
      StringVectorMap                 m_stringValues = {};

      friend class GenericEventStreamer;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /**
     * @brief GenericEventStreamer class
     */
    class GenericEventStreamer : public EventStreamer
    {
    public:
      /** Constructor
       */
      GenericEventStreamer();

      /** Destructor
       */
      ~GenericEventStreamer() override;

      /** Factory method to create the corresponding event to this streamer.
       *  The event is expected to contains an allocated wrapped event
       */
      EventPtr createEvent() const override;

      /** Serialize the event and store it into a data stream.
       */
      StatusCode write(const EventPtr &event, xdrstream::IODevice *pDevice) override;

      /** De-serialize the lcio event.
       */
      StatusCode read(EventPtr &event, xdrstream::IODevice *pDevice) override;
    };

  }

}

#endif  //  DQM4HEP_GENERICEVENT_H
