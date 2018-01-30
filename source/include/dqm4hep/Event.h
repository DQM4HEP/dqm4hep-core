/// \file Event.h
/*
 *
 * Event.h header template automatically generated by a class generator
 * Creation date : jeu. mars 26 2015
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


#ifndef DQM4HEP_EVENT_H
#define DQM4HEP_EVENT_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

namespace dqm4hep {

  namespace core {

    /** EventType enumerator
     */
    enum EventType
    {
      UNKNOWN_EVENT = 0,
      RAW_DATA_EVENT = 1,
      RECONSTRUCTED_EVENT = 2,
      PHYSICS_EVENT = 3,
      CUSTOM_EVENT = 4
    };

    /** Event class.
     *
     *  Base event class used a main interface. Basic information on
     *  the event structure can be optionally provided by the user.
     *  For a real event wrapping, the user must implements the EventBase
     *  interface and provide by providing a real event implementation.
     *  Template methods of this class (i.e getEvent<T>()) refers to a
     *  BaseEvent<T> implementation.
     */
    class Event
    {
    public:
      /** Destructor
       */
      virtual ~Event();

      /** Set the event type
       */
      void setType(EventType type);

      /** Get the event type
       */
      EventType getType() const;

      /** Get the event source
       */
      void setSource(const std::string &sourceName);

      /** Set the event source
       */
      const std::string &getSource() const;

      /** Set the event time stamp
       */
      void setTimeStamp(const TimePoint &timePoint);

      /** Get the event time stamp
       */
      const TimePoint &getTimeStamp() const;

      /** Set the real event implementation size,
       *  as extracted after its serialization (unit bytes)
       */
      void setEventSize(uint64_t eventSize);

      /** Get the real event implementation size,
       *  as extracted after its deserialization (unit bytes)
       */
      uint64_t getEventSize() const;

      /** Set the event number
       */
      void setEventNumber(uint32_t eventNumber);

      /** Get the event number
       */
      uint32_t getEventNumber() const;

      /** Set the run number
       */
      void setRunNumber(uint32_t runNumber);

      /** Get the run number
       */
      uint32_t getRunNumber() const;

      /** Clear the event.
       *  Should call the real event implementation
       *  destructor if owned by the event wrapper
       */
      virtual void clear();

      /** Returns the true event implementation
       */
      template <typename T>
      T *getEvent() const;

      /** Set the event.
       *  Ownership transfered to event instance if isOwner argument set to true
       */
      template <typename T>
      void setEvent(T *pEvent, bool isOwner = true);

      /** Write the base event information in the xdrstream device
       */
      xdrstream::Status writeBase(xdrstream::IODevice *pDevice) const;

      /** Read the base event information from the xdrstream device
       */
      xdrstream::Status readBase(xdrstream::IODevice *pDevice);

    protected:
      /** Private constructor
       */
      Event();

    protected:
      EventType                          m_type;          ///< The event type
      std::string                        m_source;        ///< The event source
      TimePoint                          m_timeStamp;     ///< The event time stamp
      uint64_t                           m_eventSize;     ///< The serialized event size (unit bytes)
      uint32_t                           m_eventNumber;   ///< The event number
      uint32_t                           m_runNumber;     ///< The run number
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** EventBase interface
     */
    template <typename T>
    class EventBase : public Event
    {
    public:
      /** Constructor
       */
      EventBase();

      /** Constructor with real event
       */
      EventBase(T *pEvent);

      /** Destructor
       */
      ~EventBase() override;

      /** Returns the real event implementation
       */
      T *getEvent() const;

      /** Set the real event implementation
       */
      void setEvent(T *pEvent, bool isOwner = true);

      /** Whether the event wrapper owns the real event implementation
       */
      bool isOwner() const;

      /** The implementation of this method may delete the real event implementation if needed.
       *  ATTN : do not forget the check the event ownership using the isOwner() method
       */
      void clear() override;

    protected:
      T                    *m_pEvent;        ///< The real event implementation
      bool                  m_isOwner;       ///< Whether the event wrapper owns the real event implementation
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *Event::getEvent() const
    {
      const EventBase<T> *const pEventBase(dynamic_cast<const EventBase<T> *const>(this));

      if(nullptr == pEventBase)
        return nullptr;

      return pEventBase->getEvent();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Event::setEvent(T *pEvent, bool isOwner)
    {
      EventBase<T> *const pEventBase(dynamic_cast<EventBase<T> *const>(this));

      if(NULL == pEventBase)
      {
        if(isOwner)
          delete pEvent;

        return;
      }

      pEventBase->clear();
      pEventBase->setEvent(pEvent, isOwner);
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setType(EventType type)
    {
      m_type = type;
    }

    //-------------------------------------------------------------------------------------------------

    inline EventType Event::getType() const
    {
      return m_type;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setSource(const std::string &sourceName)
    {
      m_source = sourceName;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Event::getSource() const
    {
      return m_source;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setTimeStamp(const TimePoint &timeStamp)
    {
      m_timeStamp = timeStamp;
    }

    //-------------------------------------------------------------------------------------------------

    inline const TimePoint &Event::getTimeStamp() const
    {
      return m_timeStamp;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setEventSize(uint64_t eventSize)
    {
      m_eventSize = eventSize;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint64_t Event::getEventSize() const
    {
      return m_eventSize;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setEventNumber(uint32_t eventNumber)
    {
      m_eventNumber = eventNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint32_t Event::getEventNumber() const
    {
      return m_eventNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setRunNumber(uint32_t runNumber)
    {
      m_runNumber = runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint32_t Event::getRunNumber() const
    {
      return m_runNumber;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::EventBase() :
      Event(),
      m_isOwner(true),
      m_pEvent(nullptr)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::EventBase(T *pEvent) :
      Event(),
      m_isOwner(true),
      m_pEvent(pEvent)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::~EventBase()
    {
      this->clear();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *EventBase<T>::getEvent() const
    {
      return m_pEvent;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void EventBase<T>::setEvent(T *pEvent, bool isOwner)
    {
      this->clear();
      m_pEvent = pEvent;
      m_isOwner = isOwner;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void EventBase<T>::clear()
    {
      Event::clear();
      
      if(nullptr != m_pEvent && this->isOwner())
        delete m_pEvent;

      m_pEvent = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool EventBase<T>::isOwner() const
    {
      return m_isOwner;
    }

  }

}

#endif  //  DQM4HEP_EVENT_H
