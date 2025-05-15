#include "RoomServer.h"
#include "RoomServantImp.h"
//#include "GameDataDef.h"

using namespace std;

//
RoomServer g_app;

//本地RoomObj具体地址
string g_sLocalRoomObj;

/////////////////////////////////////////////////////////////////
void RoomServer::initialize()
{
    //initialize application here:
    //...

    addServant<RoomServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".RoomServantObj");

    // 获取本地RoomObj具体地址
    TC_Endpoint ep = Application::getEpollServer()->getBindAdapter(ServerConfig::Application + "." + ServerConfig::ServerName + ".RoomServantObjAdapter")->getEndpoint();
    g_sLocalRoomObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".RoomServantObj" + "@" + ep.toString();

    //拉取远程配置
    addConfig(ServerConfig::ServerName + ".conf");

    //配置优先
    TC_Config conf;
    conf.parseFile(ServerConfig::BasePath + ServerConfig::ServerName + ".conf");
    std::string sRoomObj = conf.get("/Main/RoomServer<ProxyObj>", "");
    if (sRoomObj != "")
    {
        g_sLocalRoomObj = sRoomObj;
    }

    //
    LOG_DEBUG << "local RoomServer objAddr:" << g_sLocalRoomObj << endl;

    //初始化外部接口对象
    initOuterFactory();

    //初始化游戏服务
    initGameServer();

    //注册动态加载命令
    TARS_ADD_ADMIN_CMD_NORMAL("reload", RoomServer::reloadSvrConfig);

    //比赛配置加载命令
    TARS_ADD_ADMIN_CMD_NORMAL("matchconfig", RoomServer::reloadMatchConfig);

    //检查房间信息命令
    TARS_ADD_ADMIN_CMD_NORMAL("show", RoomServer::showRoomInfo);

    //清除用户表格命令
    TARS_ADD_ADMIN_CMD_NORMAL("ClearUserMap", RoomServer::roomStatusChange);

    //开始游戏比赛命令
    TARS_ADD_ADMIN_CMD_NORMAL("StartMatchGame", RoomServer::startMatchGame);

    //清除游戏比赛命令
    TARS_ADD_ADMIN_CMD_NORMAL("cleanMatchGame", RoomServer::cleanMatchGame);

    //显示机器人信息
    TARS_ADD_ADMIN_CMD_NORMAL("robot", RoomServer::showRobot);

    //重新加载机器人
    TARS_ADD_ADMIN_CMD_NORMAL("loadrobot", RoomServer::loadRobot);

    //清理加载机器人
    TARS_ADD_ADMIN_CMD_NORMAL("cleanrobot", RoomServer::cleanRobot);

    //统计过在玩数量
    TARS_ADD_ADMIN_CMD_NORMAL("stat", RoomServer::statOnline);
}

/////////////////////////////////////////////////////////////////

/**
* 初始化外部接口对象
* @return int 0-成功 其它-失败
*/
void RoomServer::initOuterFactory()
{
    m_pOuter = OuterFactorySingleton::getInstance();
}

/**
* 初始化游戏服务
* @return int 0-成功 其它-失败
*/
void RoomServer::initGameServer()
{
    m_pRoom = CRoomSingleton::getInstance();
    m_pPlayerMng = CPlayerMngSingleton::getInstance();
    m_pAI = CAIMngSingleton::getInstance();
}

/////////////////////////////////////////////////////////////////
void RoomServer::showInfo(const string &params, string &result)
{
    //1.显示用户, user:uid
    //2.显示Room, room:roomid
    //3.显示桌子, table:uid
    //4.所有用户, allusers
    std::string sName = params;
    std::string sParams = params;
    string::size_type pos = params.find_first_of(":");
    if (pos != string::npos)
    {
        sName = params.substr(0, pos);
        sParams = params.substr(pos + 1);
    }

    //
    string sResult = "";
    if(sName == "user")
    {
        m_pPlayerMng->showUInfo(sParams, sResult); //显示用户信息
    }
    else if(sName == "room")
    {
        m_pRoom->showRInfo(sParams, sResult); //显示房间信息
    }
    else if(sName == "table")
    {
        m_pPlayerMng->showTInfo(sParams, sResult); //显示桌子信息
    }
    else if(sName == "help")
    {
        sResult = "please input command:params\r\ndisplay user, user:uid\r\ndisplay room, room:roomid\r\ndisplay table, table:uid\r\n";
    }
    else if(sName == "allusers")
    {
        m_pPlayerMng->showUserLog();
    }
    else
    {
        result = "input command error\r\nTry 'command help' for more information\r\n";
    }

    result = "show info success, params:" + sParams + "\r\n" + sResult + "\r\n";
}

void RoomServer::destroyApp()
{
    //destroy application here:
    //...
    LOG_DEBUG << "reloadServer: " << endl;

    m_pPlayerMng->serverRetSetNotify();
    m_pAI->closeThirdPartyMng();

    // 在这个点让私人房间的桌子全总结算，正在进行的不再等待结束
    cleanPRTable();
}

/*
* 配置变更，重新加载配置
*/
bool RoomServer::reloadSvrConfig(const string &command, const string &params, string &result)
{
    try
    {
        //加载配置
        getOuterFactoryPtr()->load();

        result = "reload server config success.";
        LOG_DEBUG << "reloadSvrConfig: " << result << endl;

        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "reloadSvrConfig: " << result << endl;
    return true;
}

/**
 * 加载比赛场配置
*/
bool RoomServer::reloadMatchConfig(const string &command, const string &params, string &result)
{
    try
    {
        getOuterFactoryPtr()->loadMatchConfig();
        result = "reload match config success.";
        LOG_DEBUG << "reloadMatchConfig: " << result << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "reloadMatchConfig: " << result << endl;
    return true;
}

bool RoomServer::showRoomInfo(const string &command, const string &params, string &result)
{
    try
    {
        LOG_DEBUG << "show room info success, params : " << params << endl;
        showInfo(params, result);
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "showRoomInfo: " << result << endl;
    return true;
}

/*
* 上报用户服务映射map
*/
int RoomServer::roomStatusChange(const string &command, const string &params, string &result)
{
    try
    {
        ostringstream os;
        std::map<string, CTableMng *> &mapRID2TMng = m_pRoom->getRID2TMngMap();
        for (auto iter = mapRID2TMng.begin(); iter != mapRID2TMng.end(); ++iter)
        {
            os << iter->first << ", ";
        }

        result = "room status change success.";
        LOG_DEBUG << "ClearUserMap: " << result << ", roomid list = { " << os.str()  << "}" << endl;
        return 0;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "ClearUserMap: " << result << endl;
    return 0;
}

/**
 * 开启某比赛场
*/
bool RoomServer::startMatchGame(const string &command, const string &params, string &result)
{
    try
    {
        LOG_DEBUG << "start match game success, params : " << params << endl;
        string sRoomID = params;
        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        ReqInfo stReqInfo;
        stReqInfo.nMsgID = 10000;

        TClientParam clientparam;
        UserBaseInfoExt stUserBaseInfo;

        int iRet = pTableMng->onMessage(stReqInfo, clientparam, stUserBaseInfo);
        if (iRet != 0)
        {
            result = "table mng onMessage err, sRoomID: " + sRoomID + ", iRet: " + I2S(iRet);
            return false;
        }

        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "startMatchGame: " << result << endl;
    return true;
}

/**
 * 清理某比赛场
*/
bool RoomServer::cleanMatchGame(const string &command, const string &params, string &result)
{
    try
    {
        string sRoomID = params;
        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        ReqInfo stReqInfo;
        stReqInfo.nMsgID = 10001;

        TClientParam clientparam;
        UserBaseInfoExt stUserBaseInfo;
        int iRet = pTableMng->onMessage(stReqInfo, clientparam, stUserBaseInfo);
        if (iRet != 0)
        {
            result = "table mng onMessage err, sRoomID: " + sRoomID + ", iRet: " + I2S(iRet);
            return false;
        }

        result = "clean match game success.";
        LOG_DEBUG << "cleanMatchGame: " << result << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "cleanMatchGame: " << result << endl;
    return true;
}

bool RoomServer::statOnline(const string &command, const string &params, string &result)
{
    try
    {
        std::string sRoomID = params;
        std::string sLevel = params;
        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        result = pTableMng->statOnline();
        LOG_DEBUG << "StatOnline: " << result << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "StatOnline. " << result << endl;
    return true;
}

bool RoomServer::showRobot(const string &command, const string &params, string &result)
{
    try
    {
        std::string sRoomID = params;
        std::string sLevel = params;
        string::size_type pos = params.find_first_of("|");
        if (pos != string::npos)
        {
            sRoomID = params.substr(0, pos);
            sLevel = params.substr(pos + 1);
        }

        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        ReqInfo stReqInfo;
        stReqInfo.nMsgID = 10003;
        stReqInfo.lUID = S2L(sLevel);

        TClientParam clientparam;
        UserBaseInfoExt stUserBaseInfo;
        int iRet = pTableMng->onMessage(stReqInfo, clientparam, stUserBaseInfo);
        if (iRet != 0)
        {
            result = "table mng onMessage err, sRoomID: " + sRoomID + ", iRet: " + I2S(iRet);
            return false;
        }

        result = "robot info success.";
        LOG_DEBUG << "showRobot: " << result << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "showRobot. " << result << endl;
    return true;
}

bool RoomServer::loadRobot(const string &command, const string &params, string &result)
{
    try
    {
        string sRoomID = params;
        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        ReqInfo stReqInfo;
        stReqInfo.nMsgID = 10004;

        TClientParam clientparam;
        UserBaseInfoExt stUserBaseInfo;
        int iRet = pTableMng->onMessage(stReqInfo, clientparam, stUserBaseInfo);
        if (iRet != 0)
        {
            result = "table mng onMessage err, sRoomID: " + sRoomID + ", iRet: " + I2S(iRet);
            return false;
        }

        result = "load robot success.";
        LOG_DEBUG << "loadRobot: " << result << ", sRoomID: " << sRoomID << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "loadRobot. " << result << ", sRoomID: " << params << endl;
    return true;
}

bool RoomServer::cleanRobot(const string &command, const string &params, string &result)
{
    try
    {
        string sRoomID = params;
        auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
        if (!pTableMng)
        {
            result = "find table mng fail, sRoomID: " + sRoomID;
            return false;
        }

        // pTableMng->clearTableRobot();
        result = "clean robot success.";
        LOG_DEBUG << "cleanRobot: " << result << ", sRoomID: " << sRoomID << endl;
        return true;
    }
    catch (TC_Exception const &e)
    {
        result = string("catch tc exception: ") + e.what();
    }
    catch (std::exception const &e)
    {
        result = string("catch std exception: ") + e.what();
    }
    catch (...)
    {
        result = "catch unknown exception.";
    }

    result += "\n fail, please check it.";
    LOG_DEBUG << "cleanRobot. " << result << ", sRoomID: " << params << endl;
    return true;
}

void RoomServer::cleanPRTable()
{
    try
    {
        ostringstream os;
        std::map<string, CTableMng *> &mapRID2TMng = m_pRoom->getRID2TMngMap();
        for (auto iter = mapRID2TMng.begin(); iter != mapRID2TMng.end(); ++iter)
        {
            LOG_DEBUG << "cleanPRTable roomid: " << iter->first << endl;
            auto eGameMode = OuterFactorySingleton::getInstance()->parseGameMode(iter->first);
            if (eGameMode == E_PRIVATE_ROOM_MODE)
            {
                auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(iter->first);
                if (nullptr == pTableMng)
                {
                    LOG_ERROR << "cleanPRTable roomid: " << iter->first << endl;
                    return;
                }

                pTableMng->onFinish();
            }
        }
    }
    catch (std::exception const &e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
}



/////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception &e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }

    return -1;
}

/////////////////////////////////////////////////////////////////

