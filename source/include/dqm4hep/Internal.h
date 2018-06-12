/// \file Internal.h
/*
 *
 * Internal.h header template automatically generated by a class generator
 * Creation date : dim. oct. 5 2014
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

#ifndef DQM4HEP_INTERNAL_H
#define DQM4HEP_INTERNAL_H

// -- std headers
#include <assert.h>
#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <ratio>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <sys/stat.h>
#include <sys/utsname.h>
#include <thread>
#include <unistd.h>

// -- apple headers for stdint.h
#ifdef __APPLE__
#include <_types.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <_types/_uint8_t.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/_pthread/_pthread_types.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <bits/pthreadtypes.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#endif

// -- dqm4hep headers
#include <DQMCoreConfig.h>

//-------------------------------------------------------------------------------------------------

#if defined(__GNUC__) && defined(BACKTRACE)
#include <cstdlib>
#include <execinfo.h>
#endif

// limits definitions platform independent
#ifdef _MSC_VER
#include <float.h>
#define IS_NAN _isnan
#define IS_INF !_finite
#else
#define IS_NAN std::isnan
#define IS_INF std::isinf
#endif

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#define DEPRECATED(func) func
#endif

// Useful macro to silent any exception
#define DQM4HEP_NO_EXCEPTION( Code ) try { Code } catch(...) {}

// xdrstream useful macros
#define XDRSTREAM_SUCCESS(code) \
  if (!XDR_TESTBIT(code, xdrstream::XDR_SUCCESS)) {return dqm4hep::core::STATUS_CODE_FAILURE;}
#define XDRSTREAM_SUCCESS_RESTORE(code, position) \
  if (!XDR_TESTBIT(code, xdrstream::XDR_SUCCESS)) {device->seek(position); return dqm4hep::core::STATUS_CODE_FAILURE;}

namespace dqm4hep {

  namespace core {

    class Event;
    class EventStreamerPlugin;
    class MonitorElement;
    class QualityTest;
    class QualityTestReport;
    class Path;
    class Plugin;
    template <typename T>
    class Directory;
    template <typename T>
    class Storage;
    
    /**
     *  @brief  type traits class helper
     */
    template <typename T>
    class type {
    public:
      // stl types
      typedef std::vector<T> vector;
      typedef std::list<T> list;
      typedef std::set<T> set;
      typedef std::deque<T> deque;
      typedef std::unordered_set<T> uset;
      // pointer types
      typedef T* raw_ptr;
      typedef std::shared_ptr<T> ptr;
      typedef std::unique_ptr<T> uptr;
      typedef std::weak_ptr<T> wptr;
      // stl + pointers
      typedef std::vector<ptr> ptr_vector;
      typedef std::vector<uptr> uptr_vector;
      typedef std::vector<wptr> wptr_vector;
      typedef std::set<ptr> ptr_set;
      typedef std::set<uptr> uptr_set;
      typedef std::set<wptr> wptr_set;
      typedef std::deque<ptr> ptr_deque;
      typedef std::deque<uptr> uptr_deque;
      typedef std::deque<wptr> wptr_deque;
      typedef std::unordered_set<ptr> ptr_uset;
      typedef std::unordered_set<uptr> uptr_uset;
      typedef std::unordered_set<wptr> wptr_uset;
      // miscellanous
      typedef std::map<std::string, T> str_map;
      typedef std::map<std::string, ptr> str_ptr_map;
      typedef std::map<std::string, ptr_vector> str_ptr_vec_map;
      typedef std::unordered_map<std::string, T> str_umap;
      typedef std::unordered_map<std::string, ptr> str_ptr_umap;
      typedef std::unordered_map<std::string, ptr_vector> str_ptr_vec_umap;
    };
    
    
    /**
     *  @brief  time helper class
     */
    class time {
    public:
      // typedefs
      typedef std::chrono::system_clock clock;
      typedef clock::time_point point;
      typedef clock::duration duration;
      typedef clock::period period;

      /**
       *  @brief  Returns the current time
       */
      static inline point now() {
        return clock::now();
      }
      
      /**
       *  @brief  Sleep for a given duration
       * 
       *  @param  d the duration to sleep for 
       */
      static inline void sleep(const duration &d) {
        std::this_thread::sleep_for(d);
      }
      
      /**
       *  @brief  Sleep for n microseconds
       * 
       *  @param  usec the number of micro seconds to sleep for
       */
      static inline void usleep(unsigned int usec) {
        std::this_thread::sleep_for(std::chrono::microseconds(usec));
      }
      
      /**
       *  @brief  Sleep for n milliseconds
       * 
       *  @param  usec the number of milliseconds to sleep for
       */
      static inline void msleep(unsigned int msec) {
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
      }
      
      /**
       *  @brief  Sleep for n nanoseconds
       * 
       *  @param  usec the number of nanoseconds to sleep for
       */
      static inline void nsleep(unsigned int nsec) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(nsec));
      }
      
      /**
       *  @brief  Convert time_t to time point
       *  
       *  @param  t the time_t to convert
       */
      static inline point asPoint(std::time_t t) {
        return clock::from_time_t(t);
      }
      
      /**
       *  @brief  Convert the time point to time_t
       * 
       *  @param  p the time point to convert
       */
      static inline std::time_t asTime(const point &p) {
        return clock::to_time_t(p);
      }

      /**
       *  @brief  Convert the integer to time point
       * 
       *  @param  integer the integer to convert
       */
      static inline point asPoint(int32_t integer) {
        return asPoint(static_cast<std::time_t>(integer));
      }
      
      /**
       *  @brief  Convert the time point to integer
       * 
       *  @param  p the time point to convert
       */
      static inline int32_t asInt(const point &p) {
        return static_cast<int32_t>(asTime(p));
      }
      
      /**
       *  @brief  Convert a time pointto string. Format is HH:MM:SS
       * 
       *  @param  p the time point to convert
       */
      static inline std::string asString(const point &p) {
        auto timeT = asTime(p);
        std::tm timeTm;
        localtime_r(&timeT, &timeTm);
        std::stringstream ss;
        ss << timeTm.tm_hour << ":" << timeTm.tm_min << ":" << timeTm.tm_sec;
        return ss.str();
      }
    };

    // time - DEPRECATED DECLARATIONS !
    typedef time::clock TimeClock;
    typedef time::point TimePoint;
    typedef time::duration TimeDuration;
    // typedef std::chrono::duration<double> TimeDuration;

    // event
    typedef type<Event>::ptr EventPtr;
    typedef type<Event>::ptr_deque EventQueue;
    typedef type<EventStreamerPlugin>::ptr EventStreamerPluginPtr;

    // plugin
    typedef type<Plugin>::ptr PluginPtr;

    // typedefs for streaming
    typedef char dqm_char;
    typedef uint8_t dqm_uchar;
    typedef int16_t dqm_short;
    typedef uint16_t dqm_ushort;
    typedef int16_t dqm_short_int;
    typedef uint16_t dqm_ushort_int;
    typedef int64_t dqm_lint;
    typedef uint64_t dqm_ulint;
    typedef float dqm_real;
    typedef float dqm_float;
    typedef double dqm_double;
    typedef long dqm_long;
    typedef long long dqm_long_long;
    typedef bool dqm_bool;
    typedef int64_t dqm_int;
    typedef uint64_t dqm_uint;
    typedef long unsigned int dqm_stat;

    // basic typedefs
    typedef std::vector<int> IntVector;
    typedef std::vector<unsigned int> UIntVector;
    typedef std::vector<float> FloatVector;
    typedef std::vector<double> DoubleVector;
    typedef std::vector<std::string> StringVector;
    typedef std::set<std::string> StringSet;
    typedef std::map<std::string, std::string> ParameterMap;
    typedef std::pair<std::string, std::string> StringPair;
    typedef std::map<std::string, std::string> StringMap;

    // dqm4hep typedefs
    typedef type<MonitorElement>::ptr MonitorElementPtr;
    typedef type<MonitorElement>::ptr_vector MonitorElementList;
    typedef type<MonitorElement>::str_ptr_map MonitorElementMap;
    typedef type<Directory<MonitorElement>>::ptr MonitorElementDir;
    typedef QualityTest QTest;
    typedef QualityTestReport QReport;
    typedef type<QTest>::ptr QTestPtr;
    typedef type<QReport>::str_map QReportMap;
    typedef std::map<StringPair, QReportMap> QReportContainer;
    typedef type<QTest>::str_ptr_map QTestMap;
    typedef type<Storage<MonitorElement>>::ptr MeStoragePtr;

    //-------------------------------------------------------------------------------------------------

    /** Convert std::string to a type
     */
    template <class T>
    inline bool stringToType(const std::string &s, T &t) {
      std::istringstream iss(s);
      return !(iss >> t).fail();
    }

    //-------------------------------------------------------------------------------------------------

    /** Convert type to std::string
    */
    template <class T>
    inline std::string typeToString(const T &t) {
      std::ostringstream oss;

      if ((oss << t).fail())
        throw;

      return oss.str();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <>
    inline bool stringToType(const std::string &s, std::string &t) {
      t = s;
      return true;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline std::string typeToString(const std::string &t) {
      return t;
    }

    //-------------------------------------------------------------------------------------------------

    /** Screen splash of DQM4HEP
     */
    inline void screenSplash() {
      std::cout << "#######################################################" << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#                    DQM4HEP (DQMCore)" << std::endl;
      std::cout << "#   (Data Quality Monitoring For High Energy Physics)" << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#      Version    : " << DQMCore_VERSION_STR << std::endl;
      std::cout << "#      Started at : " << time::asString(time::now()) << std::endl;
      std::cout << "#      Author     : R. Ete, A. Pingault" << std::endl;
      std::cout << "#      Mail       : <dqm4hep@gmail.com> " << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#######################################################" << std::endl;
    }

    //-------------------------------------------------------------------------------------------------

    /** Tokenize string with delimiter.
     */
    template <typename T>
    inline void tokenize(const std::string &inputString, std::vector<T> &tokens, const std::string &delimiter = " ") {
      std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
      std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);

      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        T value;
        if(not stringToType(inputString.substr(lastPos, pos - lastPos), value)) {
          return;
        }
        tokens.push_back(value);
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
      }
    }
    
    //-------------------------------------------------------------------------------------------------

    /** Tokenize string with delimiter.
     */
    template <>
    inline void tokenize(const std::string &inputString, StringVector &tokens, const std::string &delimiter) {
      std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
      std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);

      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        tokens.push_back(inputString.substr(lastPos, pos - lastPos));
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  Whether the test string matches the wildcard expression 
     *  
     *  @param  testString the string to test
     *  @param  wildcardString the string containing wildcards
     */
    bool wildcardMatch(const std::string &testString, const std::string &wildcardString, bool caseSensitive = true);
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  Replaces all occurences in the spcified string
     *  
     *  @param  subject the string on which to perform replace
     *  @param  search the portion of the input string to search
     *  @param  replace the portion of string to replace with
     */
    std::string &replaceAll(std::string &subject, const std::string& search, const std::string& replace);

    //-------------------------------------------------------------------------------------------------

    /** Convert time_t to hour, minutes and seconds
     */
    // inline void timeToHMS(time_t t, int &hours, int &minutes, int &seconds) {
    //   auto pTmTime = new tm();
    //   localtime_r(&t, pTmTime);
    //   hours = pTmTime->tm_hour;
    //   minutes = pTmTime->tm_min;
    //   seconds = pTmTime->tm_sec;
    //   delete pTmTime;
    // }
    
    //-------------------------------------------------------------------------------------------------
    
    /** Convert TimePoint to time_t
     */
    // inline time_t toTime_t(const TimePoint &timePoint) {
    //   return TimeClock::to_time_t(timePoint);
    // }
    
    //-------------------------------------------------------------------------------------------------
    
    /** Convert time_t to TimePoint
     */
    // inline TimePoint toTimePoint(time_t t) {
    //   return TimeClock::from_time_t(t);
    // }

    //-------------------------------------------------------------------------------------------------

    /** Convert time_t to string format as "HOURSh MINUTESm SECONDSs"
     */
    DEPRECATED(void timeToHMS(std::time_t t, std::string &timeStr));

    //-------------------------------------------------------------------------------------------------

    /** Get the current time (unit seconds)
     */
    DEPRECATED(TimePoint now());

    //-------------------------------------------------------------------------------------------------

    /** Sleep for the specified time duration
     */
    DEPRECATED(void sleep(const TimeDuration &duration));

    //-------------------------------------------------------------------------------------------------

    /** Get the special character list that are to be avoided in some strings
     */
    inline StringVector getSpecialCharacterList() {
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

    /** Whether the string contains a special character
     */
    inline bool containsSpecialCharacters(const std::string &str) {
      StringVector specialCharacterList = dqm4hep::core::getSpecialCharacterList();

      for (auto iter = specialCharacterList.begin(), endIter = specialCharacterList.end(); endIter != iter; ++iter) {
        if (str.find(*iter) != std::string::npos)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    /** Fill host info (uname result + host name). See Key for available keys
     */
    inline void fillHostInfo(StringMap &info) {
      // uname
      struct utsname unameStruct;
      uname(&unameStruct);

      // host name
      char host[256];
      gethostname(host, 256);

      info["system"] = unameStruct.sysname;
      info["node"] = unameStruct.nodename;
      info["release"] = unameStruct.release;
      info["version"] = unameStruct.version;
      info["machine"] = unameStruct.machine;
      info["host"] = host;
    }

    //-------------------------------------------------------------------------------------------------

    /** Specialization for booleans
     */
    template <>
    inline bool stringToType(const std::string &s, bool &t) {
      if (s == "on" || s == "1" || s == "true")
        t = true;
      else if (s == "off" || s == "0" || s == "false")
        t = false;
      else
        return false;

      return true;
    }
    
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get the current process id
     */
    inline int pid() {
#if _WIN32
      return GetCurrentProcessId();
#else
      return ::getpid();
#endif
    }
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  CpuStats struct
     */
    struct CpuStats {
      dqm_float load1m = {0.};  // cpu load average over 1 m
      dqm_float load5m = {0.};  // cpu load average over 5 m
      dqm_float load15m = {0.}; // cpu load average over 15 m
      dqm_float user = {0.};    // cpu user load in percentage
      dqm_float sys = {0.};     // cpu sys load in percentage
      dqm_float tot = {0.};     // cpu user+sys load in percentage
      dqm_float idle = {0.};    // cpu idle percentage
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some memory stats
     *
     *  @param  object the CpuStats object to receive
     */
    void cpuStats(CpuStats &stats, dqm_int sampleTime = 1 /*s*/);
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  MemoryStats struct
     *          All units in Mb
     */
    struct MemoryStats {
      dqm_int vmTot = {0};   // total virtual RAM (physical RAM + swap) in MB
      dqm_int vmUsed = {0};  // used virtual RAM (physical RAM + swap) in MB
      dqm_int vmFree = {0};  // free virtual RAM (physical RAM + swap) in MB
      dqm_int rssTot = {0};  // total physical RAM in MB
      dqm_int rssUsed = {0}; // used  physical RAM in MB
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some memory stats
     *
     *  @param  object the MemoryStats object to receive
     */
    void memStats(MemoryStats &stats);

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  ProcessStats struct
     *          All memory units in Mb
     */
    struct ProcessStats {
      dqm_float cpuTimeUser = {0.}; // user time used by this process in seconds
      dqm_float cpuTimeSys = {0.};  // system time used by this process in seconds
      dqm_float cpuTimeTot = {0.};  // total time used by this process in seconds
      dqm_float cpuUser = {0.};     // cpu user load used by this process in percentage
      dqm_float cpuSys = {0.};      // cpu sys load used by this process in percentage
      dqm_float cpuTot = {0.};      // total (sys+user) cpu load used by this process in percentage
      dqm_long vm = {0L};           // virtual memory used by this process in KB
      dqm_long rss = {0L};          // resident memory used by this process in KB
      timeval lastPollTime {0, 0}; // last time process stats were polled
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get stats from current process
     *
     *  @param  object the ProcessStats object to receive
     */
    void procStats(ProcessStats &stats);
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  INetworkStats struct
     */
    struct INetworkStats {
      dqm_stat tot_rcv_kbytes = {0L};
      dqm_stat tot_rcv_packets = {0L};
      dqm_stat tot_rcv_errs = {0L};
      dqm_stat tot_snd_kbytes = {0L};
      dqm_stat tot_snd_packets = {0L};
      dqm_stat tot_snd_errs = {0L};
      dqm_stat rcv_rate_kbytes = {0L};
      dqm_stat rcv_rate_packets = {0L};
      dqm_stat rcv_rate_errs = {0L};
      dqm_stat snd_rate_kbytes = {0L};
      dqm_stat snd_rate_packets = {0L};
      dqm_stat snd_rate_errs = {0L};
    };

    typedef std::map<std::string, INetworkStats> NetworkStats;

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some network stats
     *
     *  @param  object the NetworkStats object to receive
     */
    void netStats(NetworkStats &stats, dqm_int sampleTime = 1 /*s*/);
  } // namespace core
} // namespace dqm4hep

#endif //  DQM4HEP_ENUMERATORS_H
