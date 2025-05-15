#ifndef __LOGCOMM_H__
#define __LOGCOMM_H__

//
#include <util/tc_logger.h>
#include "servant/RemoteLogger.h"

//
using namespace tars;

//
#define ROLLLOG(level) (LOG->level() << "[" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "]")
#define ROLLLOG_DEBUG (ROLLLOG(debug))
#define ROLLLOG_INFO (ROLLLOG(info))
#define ROLLLOG_WARN (ROLLLOG(warn))
#define ROLLLOG_ERROR (ROLLLOG(error))

//房间日志
#define ROOMLOG_DEBUG(uid, roomid, extendkey) (ROLLLOG(debug) << "|" << uid << "|" << roomid << "|" << extendkey << "|")
#define ROOMLOG_INFO(uid, roomid, extendkey) (ROLLLOG(info) << "|" << uid << "|" << roomid << "|" << extendkey << "|")
#define ROOMLOG_WARN(uid, roomid, extendkey) (ROLLLOG(warn) << "|" << uid << "|" << roomid << "|" << extendkey << "|")
#define ROOMLOG_ERROR(uid, roomid, extendkey) (ROLLLOG(error) << "|" << uid << "|" << roomid << "|" << extendkey << "|")
#define FDLOG_LOG_INFO (FDLOG("roomlog") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

#define FUNC_ENTRY(in) (ROLLLOG(debug) << (string(1, '\n')) << ">>>> Enter " << __FUNCTION__ << "() in( uid=" << in << " )" << endl)
#define FUNC_EXIT(out, msgid, ret) (ROLLLOG(debug) << "\n<<<< Exit " << __FUNCTION__ << "() out( dealTime=" << out << "ms, msgid=" << msgid << " ), ret = " << ret << (string(4, '\n')) << endl)

#define FDLOG_ERROR (FDLOG("error") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")
#define FDLOG_EXCEPT (FDLOG("except") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

//配置信息
#define FDLOG_RUN_CONFIG_INFO (FDLOG("run_config_info") << "|")

//配置信息
#define FDLOG_CONFIG_INFO (FDLOG("config_info") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

//财富信息
#define FDLOG_WEALTH_INFO (FDLOG("wealth_info") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

//机器人
#define FDLOG_ROBOT_INFO (FDLOG("robot_info") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

//服务调用日志
//#define FDLOG_PR_INVOK_SERVER(gameid, roomid) (FDLOG("invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|" << gameid << "|" << roomid << "|")

//房卡场
#define FDLOG_PRIVATE_ACTION_FLOW(gameid, roomid, playerid, action) (FDLOG("action_flow_private") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ <<  "|" << gameid << "|" << roomid << "|" << playerid << "|" << action << "|")

//快速开始
#define FDLOG_QUICK_ACTION_FLOW(gameid, roomid, playerid, action) (FDLOG("action_flow_quick") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ <<  "|" << gameid << "|" << roomid << "|" << playerid << "|" << action << "|")

//自由坐桌
#define FDLOG_FREEING_ACTION_FLOW(gameid, roomid, playerid, action) (FDLOG("action_flow_freeing") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ <<  "|" << gameid << "|" << roomid << "|" << playerid << "|" << action << "|")

//房卡场
// #define FDLOG_UW_INVOK_SERVER(uid, roomid, key, action)     (FDLOG("uw_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ", key:" << key << "\n" << action << " = {\n")
// #define FDLOG_PR_INVOK_SERVER(uid, roomid, roomkey, action) (FDLOG("pr_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ", roomkey:" << roomkey << "\n" << action << " = {\n")
// #define FDLOG_CL_INVOK_SERVER(uid, roomid, roomkey, action) (FDLOG("cl_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ", roomkey:" << roomkey << "\n" << action << " = {\n")
// #define FDLOG_FS_INVOK_SERVER(uid, roomid, tableid, action) (FDLOG("fs_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ", tableid:" << tableid << "\n" << action << " = {\n")
// #define FDLOG_QS_INVOK_SERVER(uid, roomid, tableid, action) (FDLOG("qs_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ", tableid:" << tableid << "\n" << action << " = {\n")
// #define FDLOG_KO_INVOK_SERVER(uid, roomid, matchid, tableid, action) (FDLOG("ko_invok_server") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|\n >>>> Start  uid:" << uid << ", roomid:" << roomid << ",machid"<< matchid <<", tableid:" << tableid << "\n" << action << " = {\n")

//用户登录、登出日志
// #define ACCOUNT_ACT_LOG_TOPIC "account_act_log"
// #define FDLOG_ACCOUNT_ACT_LOG (FDLOG("account_act_log") << "|")

//服务费抽取日志
// #define CLASSIC_SERVICE_FEE_TOPIC "classic_service_fee_log"
// #define FDLOG_CLASSIC_SERVICE_FEE_LOG (FDLOG("classic_service_fee_log") << "|")

// #define FDLOG_INIT_FORMAT(x,y) (TarsTimeLogger::getInstance()->initFormat(x,y))
// #define FDLOG_INIT_FORMAT(x,y,z) (TarsTimeLogger::getInstance()->initFormatWithType<LogByMinute>(x,y,z))
// #define FDLOG_ACCOUNT_ACT_LOG_FORMAT (FDLOG_INIT_FORMAT("account_act_log", "%Y%m%d%H%M", 5))
// #define FDLOG_CLASSIC_SERVICE_FEE_LOG_FORMAT (FDLOG_INIT_FORMAT("classic_service_fee_log", "%Y%m%d%H%M", 5))

//在线信息日志
#define FDLOG_ONLINE_USER_LOG (FDLOG("online_user_log") << "|")

//经典场信息记录日志
#define BASE_MATCH_INFO_LOG_TOPIC "base_match_info_log"
#define FDLOG_BASE_MATCH_INFO_LOG (FDLOG("base_match_info_log") << "|")

//比赛牌局信息记录日志
#define MATCH_INFO_LOG_TOPIC "match_info_log"
#define FDLOG_MATCH_INFO_LOG (FDLOG("match_info_log") << "|")
#define FDLOG_AI_INFO (FDLOG("ai_info") << "|")

//接口性能边界值
#define COST_MS 100

#endif



