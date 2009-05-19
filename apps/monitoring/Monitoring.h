/*
 * $Id$
 *
 * Copyright (C) 2009 IPTEGO GmbH
 *
 * This file is part of SEMS, a free SIP media server.
 *
 * SEMS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MONITORING_H_
#define _MONITORING_H_

#include <map>
#include <memory>

#include "AmThread.h"
#include "AmApi.h"
#include "AmArg.h"

#include <time.h>

#define NUM_LOG_BUCKETS 16

struct LogInfo {
  time_t finished; // for garbage collection
LogInfo() 
 : finished(0) { }
  AmArg info;
};

struct LogBucket {
  AmMutex log_lock;
  std::map<string, LogInfo> log;
};
class MonitorGarbageCollector;

class Monitor 
: public AmDynInvokeFactory,
  public AmDynInvoke   
{
  static Monitor* _instance;
  std::auto_ptr<MonitorGarbageCollector> gc_thread;

  LogBucket logs[NUM_LOG_BUCKETS];

  LogBucket& getLogBucket(const string& call_id);

  void clearFinished();

  void log(const AmArg& args, AmArg& ret);
  void logAdd(const AmArg& args, AmArg& ret);
  void markFinished(const AmArg& args, AmArg& ret);
  void clear(const AmArg& args, AmArg& ret);
  void clearFinished(const AmArg& args, AmArg& ret);
  void erase(const AmArg& args, AmArg& ret);
  void get(const AmArg& args, AmArg& ret);
  void getAttribute(const AmArg& args, AmArg& ret);
  void getAttributeActive(const AmArg& args, AmArg& ret);
  void getAttributeFinished(const AmArg& args, AmArg& ret);
  void listAll(const AmArg& args, AmArg& ret);
  void listByFilter(const AmArg& args, AmArg& ret);
  void listByRegex(const AmArg& args, AmArg& ret);
  void listFinished(const AmArg& args, AmArg& ret);
  void listActive(const AmArg& args, AmArg& ret);

 public:

  Monitor(const string& name);
  ~Monitor();
  // DI factory
  AmDynInvoke* getInstance() { return instance(); }
  // DI API
  static Monitor* instance();
  void invoke(const string& method, 
	      const AmArg& args, AmArg& ret);
  int onLoad();
  static unsigned int gcInterval;

  friend class MonitorGarbageCollector;
};

class MonitorGarbageCollector 
: public AmThread,
  public AmEventQueueInterface
 {
  AmSharedVar<bool> running;

 public:
  void run();
  void on_stop();
  void postEvent(AmEvent* e);
};
#endif
