/// \file Logging.h
/*
 *
 * Logging.h header template automatically generated by a class generator
 * Creation date : ven. oct. 31 2014
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

#ifndef DQM4HEP_LOGGING_H
#define DQM4HEP_LOGGING_H

#include <dqm4hep/Logger.h>

#define dqm_logger_log(logger, level, ...) logger->log(level, __VA_ARGS__)
#define dqm_logger_trace(logger, ...) logger->trace(__VA_ARGS__)
#define dqm_logger_debug(logger, ...) logger->debug(__VA_ARGS__)
#define dqm_logger_warning(logger, ...) logger->warn(__VA_ARGS__)
#define dqm_logger_error(logger, ...) logger->error(__VA_ARGS__)
#define dqm_logger_critical(logger, ...) logger->critical(__VA_ARGS__)
#define dqm_logger_info(logger, ...) logger->info(__VA_ARGS__)

#define dqm_log(level, ...) dqm4hep::core::Logger::mainLogger()->log(level, __VA_ARGS__)
#define dqm_trace(...) dqm4hep::core::Logger::mainLogger()->trace(__VA_ARGS__)
#define dqm_debug(...) dqm4hep::core::Logger::mainLogger()->debug(__VA_ARGS__)
#define dqm_warning(...) dqm4hep::core::Logger::mainLogger()->warn(__VA_ARGS__)
#define dqm_error(...) dqm4hep::core::Logger::mainLogger()->error(__VA_ARGS__)
#define dqm_critical(...) dqm4hep::core::Logger::mainLogger()->critical(__VA_ARGS__)
#define dqm_info(...) dqm4hep::core::Logger::mainLogger()->info(__VA_ARGS__)

#endif //  DQM4HEP_LOGGING_H
