#include "Room.h"
#include <dlfcn.h>
#include "xtime_userface.h"
#include "MJTableMng/MJTableMng.h"
#include "DDZTableMng/DDZTableMng.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//日志单例get函数
extern "C"
{
    tars::TarsRollLogger *GetRollLogger()
    {
        return TarsRollLogger::getInstance();
    }

    tars::TarsTimeLogger::TimeLogger *GetTimeLogger(const string &sFile)
    {
        return TarsTimeLogger::getInstance()->logger(sFile);
    }
}

CRoom::CRoom()
{
    try
    {
        int iRet = initialize();
        if (iRet != 0)
        {
            terminate();
        }
    }
    catch(exception &e)
    {
        LOG_ERROR << "init room error, exception : " << e.what() << endl;
        terminate();
    }
    catch(...)
    {
        LOG_ERROR << "init room error, unknow exception." << endl;
        terminate();
    }
}

CRoom::~CRoom()
{
    for (auto iter = m_mapRID2TMng.begin(); iter != m_mapRID2TMng.end(); ++iter)
    {
        if (iter->second)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }
}

int CRoom::initialize()
{
    ostringstream os;
    os << "initialize game roomid list = { ";

    auto &gg = OuterFactorySingleton::getInstance()->getGamegroup();
    for (auto iter = gg.mapGameGroup.begin(); iter != gg.mapGameGroup.end(); ++iter)
    {
        int iRet = initGameSo(iter->second);
        if (iRet != 0)
        {
            LOG_ERROR << "initialize game so error, ret : " << iRet << ", roomid : " << iter->second.sRoomID << endl;
            return -1;
        }

        iRet = initTableMng(iter->second);
        if (iRet != 0)
        {
            LOG_ERROR << "initialize tableMng error, ret : " << iRet << ", roomid : " << iter->second.sRoomID << endl;
            return -2;
        }

        roomStatusChange(iter->second.sRoomID);
        os << iter->second.sRoomID << ", ";
    }

    LOG_DEBUG << os.str() << "}" << endl;
    return 0;
}

int CRoom::initGameSo(TableConfInfo &stTableConfInfo)
{
    //初始化游戏so
    string szSoFileName = OuterFactorySingleton::getInstance()->getGameSo() + "/lib" + I2S(stTableConfInfo.iGameID) + ".so";
    szSoFileName = TC_File::simplifyDirectory(szSoFileName);
    LOG_DEBUG << "dll file name: " << szSoFileName << endl;

    //载入动态库,游戏句柄
    void *handle = dlopen(szSoFileName.c_str(), RTLD_NOW);
    if (!handle)
    {
        LOG_ERROR << "dlopen: " << dlerror() << endl;
        return -1;
    }

    //关联动态库的创建游戏函数
    stTableConfInfo.pfnInitGame = (CreateGameFunc)dlsym(handle, "CreateGame");
    if (!stTableConfInfo.pfnInitGame)
    {
        LOG_ERROR << "load so:" << szSoFileName << "|dlsym: " << dlerror() << endl;
        return -2;
    }

    //获取GetRollIns函数指针
    initLoggerGetterFunc pInitLoggerGetter = (initLoggerGetterFunc)(dlsym(handle, "initLoggerGetter"));
    if (!pInitLoggerGetter)
    {
        LOG_ERROR << "load so:" << szSoFileName << "|dlsym: " << dlerror() << endl;
        return -3;
    }

    //执行SetGetRollIns函数 给So的GetRollIns函数指针赋值
    (*pInitLoggerGetter)(GetRollLogger, GetTimeLogger);

    //获取initSoTimer函数指针，赋值函数指针
    initSoTimer_t pInitSoTimer = (initSoTimer_t)dlsym(handle, "initSoTimer");
    if (!pInitSoTimer)
    {
        LOG_ERROR << "load so:" << szSoFileName << "|dlsym: " << dlerror() << endl;
        return -4;
    }

    //执行initSoTimer函数
    (*pInitSoTimer)(SetTimerHost, SetCbTimerHost, KillTimer, GetTimerRemaining);
    return 0;
}

int CRoom::initTableMng(TableConfInfo &stTableConfInfo)
{
    CTableMng *pTableMng = NULL;
    string sRoomID = stTableConfInfo.sRoomID;
    auto eGameMode = OuterFactorySingleton::getInstance()->parseGameMode(sRoomID);
    switch (eGameMode)
    {
    //快速模式
    case E_AOF_MODE:
    case E_QUICK_START_MODE:
    case E_SHORT_DECK_MODE:
        pTableMng = new CQSTableMng();
        break;
    //锦标赛模式
    case E_KNOCKOUT_MODE:
        pTableMng = new MTTableMng();
        break;
    //挑战赛模式
    case E_ROOM_AI_MATCH:
        pTableMng = new CAITableMng();
        break;
    //单桌赛模式
    case E_SNG_MODE:
        pTableMng = new SNGTableMng();
        break;
    //私人场模式
    case E_PRIVATE_ROOM_MODE:
        pTableMng = new CPRTableMng();
        break;
    //梭哈
    case E_SUOHA_MODE:
        pTableMng = new CSHTableMng();
        break;
    //斗地主
    case E_DDZ_MODE:
        pTableMng = new CDDZTableMng();
        break;
    //二人麻将
    case E_MJ_MODE:
        pTableMng = new CMJTableMng();
        break;
    //未知模式
    default:
        LOG_ERROR << "Game mode error, mode : " << eGameMode << endl;
        break;
    }

    if (!pTableMng)
    {
        LOG_ERROR << "pTableMng is null, roomid=" << sRoomID << endl;
        return -1;
    }

    int iRet = pTableMng->initialize(stTableConfInfo);
    if (iRet != 0)
    {
        LOG_ERROR << "initialize tableMng fail, ret : " << iRet << ", roomid : " << sRoomID << endl;
        return -2;
    }

    LOG_DEBUG << "Call initialize tableMng succ, ret : " << iRet << ", roomid : " << sRoomID << endl;

    iRet = insertRID2TMngMap(sRoomID, pTableMng);
    if (iRet != 0)
    {
        LOG_ERROR << "insertRID2TMngMap() fail, ret : " << iRet << ", roomid : " << sRoomID << endl;
        return -3;
    }

    LOG_DEBUG << "Call initTableMng() succ, ret : " << iRet << ", roomid : " << sRoomID << endl;
    return 0;
}

int CRoom::insertRID2TMngMap(std::string sRoomID, CTableMng *pTableMng)
{
    if (isExistRID4Map(sRoomID))
        return 0;

    if (!pTableMng)
        return -1;

    m_mapRID2TMng.insert(pair<string, CTableMng *>(sRoomID, pTableMng));

    if (!isExistRID4Map(sRoomID))
        return -2;

    return 0;
}

bool CRoom::isExistRID4Map(string sRoomID)
{
    auto iter = m_mapRID2TMng.find(sRoomID);
    if (iter != m_mapRID2TMng.end())
        return true;

    return false;
}

CTableMng *CRoom::findTMngByRID(std::string sRoomID)
{
    auto iter = m_mapRID2TMng.find(sRoomID);
    if (iter != m_mapRID2TMng.end())
        return iter->second;

    ostringstream os;
    os <<  ", exist roomid list = { ";
    for (auto it = m_mapRID2TMng.begin(); it != m_mapRID2TMng.end(); ++it)
    {
        os << it->first << ", ";
    }
    os << "}";
    LOG_ERROR << "find tableMng by roomid error, ReqRoomid : " << sRoomID << os.str() << endl;
    return NULL;
}

int CRoom::roomStatusChange(std::string sRoomID)
{
    //userservermap::RoomStatusChangeReq req;
    //req.roomId = sRoomID;
    //req.changeType = 0;
    //OuterFactorySingleton::getInstance()->async2UserServerMap(req);

    auto eGameMode = OuterFactorySingleton::getInstance()->parseGameMode(sRoomID);
    if (eGameMode == E_KNOCKOUT_MODE)
    {
        /*
        gamematch::MatchRoomStatusChangeReq req;
        req.roomId = sRoomID;
        req.changeType = 0;
        OuterFactorySingleton::getInstance()->async2MatchRoomStatusChange(req);
        */
    }

    return 0;
}

void CRoom::showRInfo(const string &params, string &result)
{
    std::string sRoomID = params;
    if (sRoomID == "0")
    {
        unsigned iRCnt = m_mapRID2TMng.size();
        result = "room total, number: " + I2S(iRCnt);
    }
    else if (sRoomID == "1")
    {
        for (auto iter = m_mapRID2TMng.begin(); iter != m_mapRID2TMng.end(); ++iter)
        {
            result = result + iter->first + "\r\n";
        }
    }
    else
    {
        if (isExistRID4Map(sRoomID))
        {
            GameGroup &gg = OuterFactorySingleton::getInstance()->getGamegroup();
            auto iter = gg.mapGameGroup.find(sRoomID);
            if (iter != gg.mapGameGroup.end())
            {
                result = "roomid:" + iter->second.sRoomID + ", agentid:" + I2S(iter->second.iAgentID) +
                         "\r\nmaxTableCount:" + I2S(iter->second.iMaxTableCount) + ", maxSeatCount:" + I2S(iter->second.iMaxSeatCount) + ", minSeatCount:" + I2S(iter->second.iMinSeatCount) +
                         "\r\nbasePoint:" + I2S(iter->second.iBasePoint) + ", initPoint:" + I2S(iter->second.iInitPoint) +
                         "\r\nminGold:" + I2S(iter->second.lMinGold) + ", maxGold:" + I2S(iter->second.lMaxGold) + ", tickoutGold:" + I2S(iter->second.lTickoutGold) +
                         "\r\ntableRule:" + iter->second.sTableRule + ", reportTime:" + I2S(iter->second.iReportTime) + ", autoSitTime:" + I2S(iter->second.iAutoSitTime) +
                         "\r\nserviceFee:" + I2S(iter->second.iServiceFee) + ", profit:" + I2S(iter->second.iProfit) + "\r\n";
            }
        }
        else
        {
            result = "roomid not exist.";
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////