#include "OuterFactoryImp.h"
#include "RoomServer.h"
#include "PlayerMng.h"
#include "AsyncUserInfoCallback.h"
#include "AsyncMatchCallback.h"
#include "AsyncAdditionalConsumeCallback.h"
#include "AsyncRepurchaseCheckCallback.h"
#include "AsyncAdditionalCheckCallback.h"
#include "AsyncRepurchaseConsumeCallback.h"
#include "AsyncQuitCallback.h"
#include "AsyncSignUpCallback.h"
#include "AsyncRewardCallback.h"
#include "AsyncGetCardsCallback.h"
#include "AsyncUpdateRiseCallback.h"
#include "AsyncUserSegmentInfoCallback.h"
#include "AsyncUpdateUserSegmentInfoCallback.h"
#include "AsyncDoRobotDecide.h"
#include "AsyncAIDecide.h"
#include "AsyncAIMsgTransmit.h"
#include "GameRecordProto.h"
#include "third.pb.h"
#include "TimeUtil.h"

using namespace tars;
using namespace JFGame;
using namespace wbl;
using namespace gamerecord;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
OuterFactoryImp::OuterFactoryImp() : _pushPrx(NULL), _hallServantPrx(NULL), _configServantPrx(NULL), _matchServantPrx(NULL), _pFileConf(NULL)
{
    createAllObject();
}

OuterFactoryImp::~OuterFactoryImp()
{
    deleteAllObject();
}

void OuterFactoryImp::deleteAllObject()
{
    if (_pFileConf)
    {
        delete _pFileConf;
        _pFileConf = NULL;
    }
}

void OuterFactoryImp::createAllObject()
{
    __TRY__

    //删除
    deleteAllObject();

    //本地配置文件
    _pFileConf = new tars::TC_Config();

    //
    // FDLOG_ACCOUNT_ACT_LOG_FORMAT;
    // FDLOG_CLASSIC_SERVICE_FEE_LOG_FORMAT;

    //加载配置
    load();

    //加载比赛配置
    loadMatchConfig();

    __CATCH__
}

void OuterFactoryImp::load()
{
    _pFileConf->parseFile(ServerConfig::BasePath + ServerConfig::ServerName + ".conf");
    LOG_DEBUG << "init config file succ, base path: " << ServerConfig::BasePath + ServerConfig::ServerName + ".conf" << endl;

    _sPushObj = (*_pFileConf).get("/Main/RouterServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "PushObj ProxyObj: " << _sPushObj << endl;

    _sHallServantObj = (*_pFileConf).get("/Main/HallServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "HallServantObj ProxyObj: " << _sHallServantObj << endl;

    _sConfigServerObj = (*_pFileConf).get("/Main/ConfigServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sConfigServerObj ProxyObj: " << _sConfigServerObj << endl;

    _sMatchServerObj = (*_pFileConf).get("/Main/MatchServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sMatchServerObj ProxyObj: " << _sMatchServerObj << endl;

    _sAIServantObj = (*_pFileConf).get("/Main/AIRobot<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sAIServantObj ProxyObj: " << _sAIServantObj << endl;

    _sSNGServantObj = (*_pFileConf).get("/Main/SNGRobot<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sSNGServantObj ProxyObj: " << _sSNGServantObj << endl;

    _sQSServantObj = (*_pFileConf).get("/Main/QSRobot<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sQSServantObj ProxyObj: " << _sQSServantObj << endl;

    _sKOServantObj = (*_pFileConf).get("/Main/KORobot<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sKOServantObj ProxyObj: " << _sKOServantObj << endl;

    _sPRServantObj = (*_pFileConf).get("/Main/PRRobot<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sPRServantObj ProxyObj: " << _sPRServantObj << endl;

    _sPushServantObj = (*_pFileConf).get("/Main/PushServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sPushObj _sPushServantObj: " << _sPushServantObj << endl;

    _sActivityServantObj = (*_pFileConf).get("/Main/ActivityServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sActivityServantObj ProxyObj: " << _sActivityServantObj << endl;

    _sGameRecordObj = (*_pFileConf).get("/Main/GameRecordServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sGameRecordObj ProxyObj: " << _sGameRecordObj << endl;

    _sGMObj = (*_pFileConf).get("/Main/GMServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sGMObj ProxyObj: " << _sGMObj << endl;

    _sGlobalServantObj = (*_pFileConf).get("/Main/GlobalServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sGlobalServantObj ProxyObj: " << _sGlobalServantObj << endl;

    _sActivityServantObj = (*_pFileConf).get("/Main/ActivityServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sActivityServantObj ProxyObj: " << _sActivityServantObj << endl;

    _sLog2DBServantObj = (*_pFileConf).get("/Main/Log2DBServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sLog2DBServantObj ProxyObj: " << _sLog2DBServantObj << endl;

    _sSocialServantObj = (*_pFileConf).get("/Main/SocialServer<ProxyObj>", "");
    ROLLLOG_DEBUG << "_sSocialServantObj ProxyObj: " << _sSocialServantObj << endl;

    //读取所有配置
    readAllConfig();

    //读取服务配置
    readSystemConfig();
    printSystemConfig();

    readMatchMailConfig();

    loadAlgConfig();
}

void OuterFactoryImp::readSystemConfig()
{
    _taskProcNum = S2I((*_pFileConf).get("/Main/SystemConfig<TaskProcNum>", "50"));
    _taskProcTimeout = S2I((*_pFileConf).get("/Main/SystemConfig<TaskProcTimeout>", "50"));
    _dealFlag = S2I((*_pFileConf).get("/Main/SystemConfig<DealFlag>", "0"));
}

void OuterFactoryImp::printSystemConfig()
{
    ROLLLOG_DEBUG << "_dealFlag=" << _dealFlag << endl;
    ROLLLOG_DEBUG << "_taskProcNum=" << _taskProcNum << endl;
    ROLLLOG_DEBUG << "_taskProcTimeout=" << _taskProcTimeout << endl;
}

void OuterFactoryImp::readMatchMailConfig()
{
    sMatchMailRewardContent  = (*_pFileConf).get("/Main/MatchMail<reward_content>");
    sMatchMailQuitContent  = (*_pFileConf).get("/Main/MatchMail<quit_content>");
    ROLLLOG_DEBUG << "sMatchMailRewardContent=" << sMatchMailRewardContent <<", sMatchMailQuitContent: "<< sMatchMailQuitContent << endl;

    mapMatchName.clear();
    auto vecDomainKey = (*_pFileConf).getDomainVector("/Main/MatchMail");
    for (auto &domain : vecDomainKey)
    {
        string subDomain = "/Main/MatchMail/" + domain;
        string match_name = (*_pFileConf).get(subDomain + "<name>");
        mapMatchName.insert(std::make_pair(TC_Common::strto<int>(domain), match_name));
        ROLLLOG_DEBUG << "domain=" << domain << ", match_name: "<< match_name << endl;
    }
    return;
}

int OuterFactoryImp::getDealFlag()
{
    return _dealFlag;
}

//配牌数据
void OuterFactoryImp::readDealCards()
{
    if (_dealFlag != 1)
        return;

    getGMServantPrx()->async_getCards(new AsyncGetCardsCallback());
}

void OuterFactoryImp::printDealCards()
{
    ROLLLOG_DEBUG << "deal cards: " << printTars(getCardsResp) << endl;
}

void OuterFactoryImp::setDealCards(const gm::GetCardsResp &rsp)
{
    getCardsResp = rsp;
    ROLLLOG_DEBUG << "deal cards: " << printTars(getCardsResp) << endl;
}

const gm::GetCardsResp &OuterFactoryImp::getDealCards()
{
    readDealCards();
    return getCardsResp;
}

//加载比赛配置
void OuterFactoryImp::loadMatchConfig()
{
    __TRY__

    WriteLocker lock(m_rwlock);

    //比赛配置
    readAllMatchConfig();
    printAllMatchConfig();

    //增值服务配置
    readBaseServiceConfig();
    printBaseServiceConfig();

    //游戏库配置
    readGameSo();
    printGameSo();

    __CATCH__
}

// 读取所有配置
void OuterFactoryImp::readAllConfig()
{
    //读取房间Room配置
    loadRoomConfig();
    printRoomConfig();
}

void OuterFactoryImp::loadRoomConfig()
{
    try
    {
        config::ListGameRoomServerReq reqSysConf;
        config::ListGameRoomServerResp rspSysConf;
        reqSysConf.serverAddress = g_sLocalRoomObj;
        int iRet = getConfigServantPrx()->listGameRoomServer(reqSysConf, rspSysConf);
        if (iRet != 0)
        {
            LOG_ERROR << "load room config server error, ret : " << iRet << endl;
        }

        setGameGroup(rspSysConf);
    }
    catch(exception &e)
    {
        LOG_ERROR << "load room config error, exception : " << e.what() << endl;
        terminate();
    }
    catch(...)
    {
        LOG_ERROR << "load room config error, unknow exception." << endl;
        terminate();
    }
}

void OuterFactoryImp::printRoomConfig()
{
    ostringstream os;
    os << endl;
    os << "load game group config from systemconfig, " << endl;
    for (auto iter = m_stGameGroup.mapGameGroup.begin(); iter != m_stGameGroup.mapGameGroup.end(); ++iter)
    {
        os  << "room " << iter->second.sRoomID
            << " config info = { "
            << "  iGameID: " << iter->second.iGameID
            << ", sRoomID: " << iter->second.sRoomID
            << ", iMaxTableCount: " << iter->second.iMaxTableCount
            << ", iMinSeatCount: " << iter->second.iMinSeatCount
            << ", iMaxSeatCount: " << iter->second.iMaxSeatCount
            << ", iBasePoint: " << iter->second.iBasePoint
            << ", lTickoutGold: " << iter->second.lTickoutGold
            << ", iInitPoint: " << iter->second.iInitPoint
            << ", lMinGold: " << iter->second.lMinGold
            << ", lMaxGold: " << iter->second.lMaxGold
            << ", sRoomName: " << iter->second.sRoomName
            << ", sTableRule" << iter->second.sTableRule
            << ", iAgentID: " << iter->second.iAgentID
            << ", iReportTime: " << iter->second.iReportTime
            << ", iAutoSitTime: " << iter->second.iAutoSitTime
            << ", iServiceFee: " << iter->second.iServiceFee
            << ", iProfit: " << iter->second.iAutoSitTime
            << ", bigBlind: " << iter->second.bigBlind
            << ", smallBlind: " << iter->second.smallBlind
            << ", robotNum: " << iter->second.robotNum
            << " }"
            << endl;
        os << endl;
    }

    ROLLLOG_DEBUG << os.str() << endl;
}

//根据涨盲轮数排序
bool BlindRoundCmp(const config::MatchBlind &blind1, const config::MatchBlind &blind2)
{
    return blind1.round < blind2.round;
}

//比赛配置
void OuterFactoryImp::readAllMatchConfig()
{
    getConfigServantPrx()->ListAllMatchConfig(listAllMatchConfigResp);

    //房间对应的比赛配置
    for (auto it = listAllMatchConfigResp.mapMatchRoomConfig.begin(); it != listAllMatchConfigResp.mapMatchRoomConfig.end(); ++it)
    {
        auto itt = mapMatchRoomConfig.find(it->second.roomID);
        if(itt == mapMatchRoomConfig.end())
        {
            map<int, MatchRoom> subitem;
            subitem.insert(std::make_pair(it->second.id, it->second));
            mapMatchRoomConfig.insert(std::make_pair(it->second.roomID, subitem));
        }
        else
        {
            auto ittt = itt->second.find(it->second.id);
            if(ittt == itt->second.end())
            {
                itt->second.insert(std::make_pair(it->second.id, it->second));
            }
            else
            {
                ittt->second = it->second;
            }
        }
    }

    //升盲配置排序
    for (auto itblind = listAllMatchConfigResp.mapMatchBlindConfig.begin(); itblind != listAllMatchConfigResp.mapMatchBlindConfig.end(); ++itblind)
    {
        //排序
        sort(itblind->second.begin(), itblind->second.end(), BlindRoundCmp);
    }
}

void OuterFactoryImp::printAllMatchConfig()
{
    FDLOG_CONFIG_INFO << "listAllMatchConfigResp : " << printTars(listAllMatchConfigResp) << endl;
}

//
const ListAllMatchConfigResp &OuterFactoryImp::getAllMatchConfig()
{
    return listAllMatchConfigResp;
}

//锦标赛房间配置
const MatchRoom *OuterFactoryImp::getMatchRoomConfig(const string &roomID, int matchID)
{
    auto it = mapMatchRoomConfig.find(roomID);
    if(it != mapMatchRoomConfig.end())
    {
        auto itt = it->second.find(matchID);
        if(itt != it->second.end())
        {
            return &itt->second;
        }
    }
    return nullptr;
}

const map<int, MatchRoom> *OuterFactoryImp::getMatchRoomConfig(const string &roomID)
{
    auto it = mapMatchRoomConfig.find(roomID);
    if(it != mapMatchRoomConfig.end())
    {
        return &it->second;
    }
    return nullptr;
}

//锦标赛房间配置
const map<string, MatchRoom> &OuterFactoryImp::getMatchRoomConfig()
{
    return mapOKMatchRoomConfig;
}

//增值服务配置
void OuterFactoryImp::readBaseServiceConfig()
{
    getConfigServantPrx()->ListBaseServiceConfig(baseServiceConfig);
}

void OuterFactoryImp::printBaseServiceConfig()
{
    FDLOG_CONFIG_INFO << "ListBaseServiceConfig : " << printTars(baseServiceConfig) << endl;
}

const ListBaseServiceConfigResp &OuterFactoryImp::getBaseServiceConfig()
{
    return baseServiceConfig;
}

//
void OuterFactoryImp::readGameSo()
{
    gameSoPath = (*_pFileConf).get("/Main/SystemConfig<ResPath>", "");
}

//
void OuterFactoryImp::printGameSo()
{
    FDLOG_CONFIG_INFO << "gameSoPath : " << gameSoPath << endl;
}

//
std::string &OuterFactoryImp::getGameSo()
{
    return gameSoPath;
}


//
int OuterFactoryImp::getTaskProcNum()
{
    return _taskProcNum;
}

//
int OuterFactoryImp::getTaskProcTimeout()
{
    return _taskProcTimeout;
}

void OuterFactoryImp::setGameGroup(config::ListGameRoomServerResp &rspSysConf)
{
    for (auto iter = rspSysConf.gameRoomList.begin(); iter != rspSysConf.gameRoomList.end(); ++iter)
    {
        TableConfInfo stTableConfInfo;
        stTableConfInfo.iGameID = (*iter).gameId;
        stTableConfInfo.sRoomID = (*iter).roomId;
        stTableConfInfo.iMaxTableCount = (*iter).deskNum;
        stTableConfInfo.iMinSeatCount = (*iter).minPlayers;
        stTableConfInfo.iMaxSeatCount = (*iter).maxPlayers;
        stTableConfInfo.iBasePoint = (*iter).antes;
        stTableConfInfo.iInitPoint = (*iter).initPoint;
        stTableConfInfo.lTickoutGold = (*iter).kickoutBelowGold;
        stTableConfInfo.lMinGold = (*iter).minGold;
        stTableConfInfo.lMaxGold = (*iter).maxGold;
        stTableConfInfo.sTableRule = (*iter).tableRule;
        stTableConfInfo.sRoomName = (*iter).roomName;
        stTableConfInfo.iAgentID = (*iter).agentId;
        stTableConfInfo.iReportTime = (*iter).dynamicReportTime;
        stTableConfInfo.iAutoSitTime = (*iter).autoSitWaitTime;
        stTableConfInfo.iServiceFee = (*iter).serviceFee;
        stTableConfInfo.iProfit = (*iter).profit;
        stTableConfInfo.bigBlind = (*iter).bigBlind;
        stTableConfInfo.smallBlind = (*iter).smallBlind;
        stTableConfInfo.robotNum = (*iter).robotNum;
        stTableConfInfo.sGameConfig = (*iter).playOptionList;
        m_stGameGroup.mapGameGroup.insert(pair<string, TableConfInfo>(stTableConfInfo.sRoomID, stTableConfInfo));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

const JFGame::PushPrx OuterFactoryImp::getRouterPushPrx(const long uid)
{
    if (!_pushPrx)
    {
        _pushPrx = Application::getCommunicator()->stringToProxy<JFGame::PushPrx>(_sPushObj);
        ROLLLOG_DEBUG << "Init _sPushObj succ, _sPushObj: " << _sPushObj << endl;
    }

    if (_pushPrx)
    {
        return _pushPrx->tars_hash(uid);
    }

    return NULL;
}

const hall::HallServantPrx OuterFactoryImp::getHallServantPrx(const long uid)
{
    if (!_hallServantPrx)
    {
        _hallServantPrx = Application::getCommunicator()->stringToProxy<hall::HallServantPrx>(_sHallServantObj);
        ROLLLOG_DEBUG << "Init _sUserInfoObj succ, _sHallServantObj: " << _sHallServantObj << endl;
    }

    if (_hallServantPrx)
    {
        return _hallServantPrx->tars_hash(uid);
    }

    return NULL;
}

const config::ConfigServantPrx OuterFactoryImp::getConfigServantPrx()
{
    if (!_configServantPrx)
    {
        _configServantPrx = Application::getCommunicator()->stringToProxy<config::ConfigServantPrx>(_sConfigServerObj);
        ROLLLOG_DEBUG << "Init _sConfigServerObj succ, _sConfigServerObj: " << _sConfigServerObj << endl;
    }

    return _configServantPrx;
}

const match::MatchServantPrx OuterFactoryImp::getMatchServantPrx(const long uid)
{
    if (!_matchServantPrx)
    {
        _matchServantPrx = Application::getCommunicator()->stringToProxy<match::MatchServantPrx>(_sMatchServerObj);
        ROLLLOG_DEBUG << "Init _sMatchServerObj succ, _sMatchServerObj: " << _sMatchServerObj << endl;
    }

    if (_matchServantPrx)
    {
        return _matchServantPrx->tars_hash(uid);
    }

    return NULL;
}

const ai::AIServantPrx OuterFactoryImp::getAIServantPrx(const int type, const long uid)
{
    string sServantObj = "";
    switch (type)
    {
    case GAME_TYPE_SNG:
        sServantObj = _sSNGServantObj;
        break;
    case GAME_TYPE_QS:
        sServantObj = _sQSServantObj;
        break;
    case GAME_TYPE_AI:
        sServantObj = _sAIServantObj;
        break;
    case GAME_TYPE_KO:
        sServantObj = _sKOServantObj;
        break;
    case GAME_TYPE_PR:
        sServantObj = _sPRServantObj;
        break;
    default:
        break;
    }

    if (sServantObj == "")
    {
        ROLLLOG_ERROR << "get servantObj  failed, type: " << type << endl;
        return NULL;
    }

    if (!_AIServantPrx)
    {
        _AIServantPrx = Application::getCommunicator()->stringToProxy<ai::AIServantPrx>(sServantObj);
        ROLLLOG_DEBUG << "Init sServantObj , sServantObj: " << sServantObj << endl;
    }

    if (_AIServantPrx)
    {
        return _AIServantPrx->tars_hash(uid);
    }

    return NULL;
}

const ai::AIServantPrx OuterFactoryImp::getAIServantPrx(const int type, const string &key)
{
    string sServantObj = "";
    switch (type)
    {
    case GAME_TYPE_SNG:
        sServantObj = _sSNGServantObj;
        break;
    case GAME_TYPE_QS:
        sServantObj = _sQSServantObj;
        break;
    case GAME_TYPE_AI:
        sServantObj = _sAIServantObj;
        break;
    case GAME_TYPE_KO:
        sServantObj = _sKOServantObj;
        break;
    case GAME_TYPE_PR:
        sServantObj = _sPRServantObj;
        break;
    default:
        break;
    }

    if (!_AIServantPrx)
    {
        _AIServantPrx = Application::getCommunicator()->stringToProxy<ai::AIServantPrx>(sServantObj);
        ROLLLOG_ERROR << "Init sServantObj failed, sServantObj: " << sServantObj << endl;
    }

    if (_AIServantPrx)
    {
        return _AIServantPrx->tars_hash(tars::hash<string>()(key));
    }

    return NULL;
}

const push::PushServantPrx OuterFactoryImp::getPushServerPrx(const long uid)
{
    if (!_pushServantPrx)
    {
        _pushServantPrx = Application::getCommunicator()->stringToProxy<push::PushServantPrx>(_sPushServantObj);
        ROLLLOG_DEBUG << "Init _sPushServantObj succ, _sPushServantObj: " << _sPushServantObj << endl;
    }

    if (_pushServantPrx)
    {
        return _pushServantPrx->tars_hash(uid);
    }

    return NULL;
}

const gamerecord::GameRecordServantPrx OuterFactoryImp::getGameRecordPrx(const long uid)
{
    if (!_recordServantPrx)
    {
        _recordServantPrx = Application::getCommunicator()->stringToProxy<gamerecord::GameRecordServantPrx>(_sGameRecordObj);
        ROLLLOG_DEBUG << "Init _sGameRecordObj succ, _sGameRecordObj: " << _sGameRecordObj << endl;
    }

    if (_recordServantPrx)
    {
        return _recordServantPrx->tars_hash(uid);
    }

    return NULL;
}

//游戏日志入库代理
const gamerecord::GameRecordServantPrx OuterFactoryImp::getGameRecordPrx(const string &key)
{
    if (!_recordServantPrx)
    {
        _recordServantPrx = Application::getCommunicator()->stringToProxy<gamerecord::GameRecordServantPrx>(_sGameRecordObj);
        LOG_DEBUG << "Init _sGameRecordObj succ, _sGameRecordObj: " << _sGameRecordObj << endl;
    }

    if (_recordServantPrx)
    {
        return _recordServantPrx->tars_hash(tars::hash<string>()(key));
    }

    return NULL;
}


const gm::GMServantPrx OuterFactoryImp::getGMServantPrx()
{
    if (!_gmServantPrx)
    {
        _gmServantPrx = Application::getCommunicator()->stringToProxy<gm::GMServantPrx>(_sGMObj);
        ROLLLOG_DEBUG << "Init _sGMObj succ, _sGameRecordObj: " << _sGMObj << endl;
    }

    return _gmServantPrx;
}

//公共代理
const global::GlobalServantPrx OuterFactoryImp::getGlobalServantPrx(const long uid)
{
    if (!_globalServantPrx)
    {
        _globalServantPrx = Application::getCommunicator()->stringToProxy<global::GlobalServantPrx>(_sGlobalServantObj);
        ROLLLOG_DEBUG << "Init _sGlobalServantObj succ, _sGlobalServantObj : " << _sGlobalServantObj << endl;
    }

    if (_globalServantPrx)
    {
        return _globalServantPrx->tars_hash(uid);
    }

    return NULL;
}

//活动服务
const activity::ActivityServantPrx OuterFactoryImp::getActivityServantPrx(const long uid)
{
    if (!_activityServantPrx)
    {
        _activityServantPrx = Application::getCommunicator()->stringToProxy<activity::ActivityServantPrx>(_sActivityServantObj);
        ROLLLOG_DEBUG << "Init _sActivityServantObj succ, _sActivityServantObj : " << _sActivityServantObj << endl;
    }

    if (_activityServantPrx)
    {
        return _activityServantPrx->tars_hash(uid);
    }

    return NULL;
}

//日志入库服务代理
const DaqiGame::Log2DBServantPrx OuterFactoryImp::getLog2DBServantPrx(const long uid)
{
    if (!_log2DBServantPrx)
    {
        _log2DBServantPrx = Application::getCommunicator()->stringToProxy<DaqiGame::Log2DBServantPrx>(_sLog2DBServantObj);
        ROLLLOG_DEBUG << "Init _sLog2DBServantObj succ, _sLog2DBServantObj : " << _sLog2DBServantObj << endl;
    }

    if (_log2DBServantPrx)
    {
        return _log2DBServantPrx->tars_hash(uid);
    }

    return NULL;
}

//社交服务代理
const Social::SocialServantPrx OuterFactoryImp::getSocialServantPrx(const long uid)
{
    if (!_socialServantPrx)
    {
        _socialServantPrx = Application::getCommunicator()->stringToProxy<Social::SocialServantPrx>(_sSocialServantObj);
        ROLLLOG_DEBUG << "Init _sSocialServantObj succ, _sSocialServantObj : " << _sSocialServantObj << endl;
    }

    if (_socialServantPrx)
    {
        return _socialServantPrx->tars_hash(uid);
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//格式化时间
string OuterFactoryImp::GetTimeFormat()
{
    string sFormat("%Y-%m-%d %H:%M:%S");
    time_t t = time(NULL);
    struct tm *pTm = localtime(&t);
    if (pTm == NULL)
    {
        return "";
    }

    char sTimeString[255] = "\0";
    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), pTm);
    return string(sTimeString);
}

// 解析RoomID字段结构
int OuterFactoryImp::parseRoomIDInfo(string sRoomID, RoomIDInfo &stRoomIDInfo)
{
    /**
     * RoomID统一在数据库中约定，比如：RoomID-100001
     * 含义：1:00:001 <--> 模式|场次|游戏
     * 游戏模式：1房卡模式，2快速开始，3自由坐桌，4比赛场。。。
     * 游戏场次：1初级场，2中级场，3高级场。。。可扩展100个
     * 游戏：1转转麻将，2南昌麻将。。。 可扩展999个
     */

    //参数错误
    if (sRoomID.length() == 0)
    {
        return -1;
    }

    //拆分
    vector<string> vecitems = split(sRoomID, ":");
    if (vecitems.size() != 3)
    {
        return -2;
    }

    // 保存信息
    stRoomIDInfo.eGameMode = (Eum_Game_Mode)S2I(vecitems[0]);
    stRoomIDInfo.eGameScreen = (Eum_Game_Screen)S2I(vecitems[1]);
    stRoomIDInfo.iGame = S2I(vecitems[2]);

    // 游戏模式
    if (stRoomIDInfo.eGameMode < E_MODE_START || E_MODE_END < stRoomIDInfo.eGameMode)
    {
        return -3;
    }

    // 游戏场次
    if (stRoomIDInfo.eGameScreen < E_SCREEN_START || E_SCREEN_END < stRoomIDInfo.eGameScreen)
    {
        return -4;
    }

    // 游戏ID
    if (stRoomIDInfo.iGame < 0)
    {
        return -5;
    }

    return 0;
}

//获取游戏模式:3-经典场,4-MTT,5-SNG
int OuterFactoryImp::getGameModeFromRoomID(string sRoomID)
{
    //参数错误
    if (sRoomID.length() == 0)
    {
        return 0;
    }

    //拆分
    vector<string> vecitems = split(sRoomID, ":");
    if (vecitems.size() != 3)
    {
        return 0;
    }

    return S2I(vecitems[0]);
}

//获取游戏人数
int OuterFactoryImp::getPlayersNumFromRoomID(string sRoomID)
{
    //参数错误
    if (sRoomID.length() == 0)
    {
        return 0;
    }

    //拆分
    vector<string> vecitems = split(sRoomID, ":");
    if (vecitems.size() != 3)
    {
        return 0;
    }

    string num = vecitems[1].substr(0, 1);
    if (num.length() == 0)
    {
        return 0;
    }

    return S2I(num);
}

// 解析RoomServerID
int64_t OuterFactoryImp::parseRoomServerID(const string sIP, const short nPort)
{
    // ip+port换算成RoomServerID
    int64_t iRet64 = IP2INT(sIP);
    iRet64 = iRet64 << 16;
    iRet64 += nPort;
    return iRet64;
}


//解析游戏模式
Eum_Game_Mode OuterFactoryImp::parseGameMode(const std::string sRoomID)
{
    Eum_Game_Mode eGameMode = E_MODE_START;

    RoomIDInfo stRoomIDInfo;
    int iRet = parseRoomIDInfo(sRoomID, stRoomIDInfo);
    if (iRet == 0)
    {
        eGameMode = stRoomIDInfo.eGameMode;
    }
    else
    {
        LOG_ERROR << "parse game mode err by roomid, roomid:" << sRoomID << ", ret:" << iRet << endl;
    }

    return eGameMode;
}

//拆分字符串
vector<std::string> OuterFactoryImp::split(const string &str, const string &pattern)
{
    return SEPSTR(str, pattern);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//发送消息客户端
int OuterFactoryImp::toClient(const RespInfo &stRespInfo)
{
    if (stRespInfo.vecMsgIDList.empty())
    {
        LOG_ERROR << "no msg to send client, uid: " << stRespInfo.lUID << endl;
        return 0;
    }

    XGameComm::TPackage tPkg;
    ostringstream os;
    os << "msgid list = { ";
    for (auto it = stRespInfo.vecMsgIDList.begin(); it != stRespInfo.vecMsgIDList.end(); ++it)
    {
        auto mh = tPkg.add_vecmsghead();
        mh->set_nmsgid(*it);

        switch (*it)
        {
        case XGameComm::E_MSGID_GAME_SO_NOTIFY:
        case XGameComm::E_MSGID_SIZZLER_SO_NOTIFY:
        case XGameComm::E_MSGID_OFFLINE_NOTIFY:
        case XGameComm::E_MSGID_SERVER_RESET_NOTIFY:
        case XGameComm::E_MSGID_USER_NOT_EXIST_NOTIFY:
        case XGameComm::E_MSGID_ROOMID_NOT_EXIST_NOTIFY:
        case XGameComm::E_MSGID_WEALTH_NOT_ENOUGH_NOTIFY:
        case XGameComm::E_MSGID_TABLE_NOT_ENOUGHT_NOTIFY:
        case XGameComm::E_MSGID_WEALTH_OVER_NOTIFY:
        case XGameComm::E_MSGID_CHAT_TE_NOTIFY:
        case XGameComm::E_MSGID_CHAT_AUDIO_NOTIFY:
        case XGameComm::E_MSGID_GPS_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_JOIN_QUEUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_QUIT_QUEUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_SITDOWN_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_EXIT_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_RECOME_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_CONTINUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_ENTER_SUCC_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_FREE_SITDOWN_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_STAND_NOTIFY:
        //case XGameKo::E_KNOCKOUT_MSGID_JOIN_MATCH_NOTIFY:
        //case XGameKo::E_KNOCKOUT_MSGID_EXIT_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_SIGN_UP_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_QUIT_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_BEGIN_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_SITDOWN_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_EXIT_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RANK_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RISE_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_KNOCKOUT_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RECOME_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_GAME_BEGIN_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_GET_TABLE_LIST_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_WATCH_GAME_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_REPURCHASE_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_ADDITIONAL_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_BLIND_INFO_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_REWARD_NOTIFY:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_NOTIFY);
            break;
        default:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_RESPONSE);
            break;
        }

        os << *it << ", ";
    }
    os << "}";

    HeadInfo stHeadInfo;
    CPlayerMngSingleton::getInstance()->getUHInfoFromUMap(stRespInfo.lUID, stHeadInfo);
    tPkg.set_igameid(stHeadInfo.iGameID);
    tPkg.set_sroomid(stHeadInfo.sRoomID);

    auto ptuid = tPkg.mutable_stuid();
    ptuid->set_luid(stRespInfo.lUID);

    for (auto it = stRespInfo.vecMsgDataList.begin(); it != stRespInfo.vecMsgDataList.end(); ++it)
    {
        string msg;
        msg.assign((*it).begin(), (*it).end());
        tPkg.add_vecmsgdata(msg);
    }

    if (stHeadInfo.sServantAddr == "" || CPlayerMngSingleton::getInstance()->isRobot(stRespInfo.lUID))
    {
        //LOG_ERROR << stRespInfo.lUID << "|" << stHeadInfo.sRoomID << "|" << "to client msg error, router addr is null." << endl;
        return 0;
    }

    if (stHeadInfo.sServantAddr != "")
    {
        auto pPushPrx = Application::getCommunicator()->stringToProxy<JFGame::PushPrx>(stHeadInfo.sServantAddr);
        if (pPushPrx)
        {
            pPushPrx->tars_hash(tPkg.stuid().luid())->async_doPushBuf(NULL, tPkg.stuid().luid(), pbToString(tPkg));
        }

        LOG_DEBUG << "Push message to client(1), uid: " << tPkg.stuid().luid()
                  << ", pkgLen: " << pbToString(tPkg).length()
                  << ", msgID: " << stRespInfo.vecMsgIDList[0]
                  << ", servantAddr:" << stHeadInfo.sServantAddr << endl;

        //过滤游戏消息
        if (stRespInfo.vecMsgIDList[0] != XGameComm::E_MSGID_GAME_SO_NOTIFY)
        {
            LOG_DEBUG << stRespInfo.lUID << "|" << "toClientMsg, info = { uid:" << stRespInfo.lUID
                      << ", roomid:" << stHeadInfo.sRoomID << ", " << os.str()
                      << ", msgDataSize:" << stRespInfo.vecMsgDataList.size()
                      << " }" << endl;
        }
    }
    else
    {
        LOG_ERROR << stRespInfo.lUID << "|" << stHeadInfo.sRoomID << "|" << "to client msg error, router addr is null." << endl;
    }

    return 0;
}

//发送给客户端
int OuterFactoryImp::toClientPb(const RespInfo &stRespInfo)
{
    if (stRespInfo.vecMsgIDList.empty())
    {
        LOG_ERROR << "no msg to send client, uid: " << stRespInfo.lUID << endl;
        return 0;
    }

    //data
    XGameComm::TPackage tPkg;

    //
    ostringstream os;
    os << "msgid list = { ";
    for (auto it = stRespInfo.vecMsgIDList.begin(); it != stRespInfo.vecMsgIDList.end(); ++it)
    {
        auto mh = tPkg.add_vecmsghead();
        mh->set_nmsgid(*it);

        switch (*it)
        {
        case XGameComm::E_MSGID_GAME_SO_NOTIFY:
        case XGameComm::E_MSGID_SIZZLER_SO_NOTIFY:
        case XGameComm::E_MSGID_OFFLINE_NOTIFY:
        case XGameComm::E_MSGID_SERVER_RESET_NOTIFY:
        case XGameComm::E_MSGID_USER_NOT_EXIST_NOTIFY:
        case XGameComm::E_MSGID_ROOMID_NOT_EXIST_NOTIFY:
        case XGameComm::E_MSGID_WEALTH_NOT_ENOUGH_NOTIFY:
        case XGameComm::E_MSGID_TABLE_NOT_ENOUGHT_NOTIFY:
        case XGameComm::E_MSGID_WEALTH_OVER_NOTIFY:
        case XGameComm::E_MSGID_CHAT_TE_NOTIFY:
        case XGameComm::E_MSGID_CHAT_AUDIO_NOTIFY:
        case XGameComm::E_MSGID_GPS_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_JOIN_QUEUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_QUIT_QUEUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_SITDOWN_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_EXIT_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_RECOME_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_CONTINUE_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_ENTER_SUCC_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_FREE_SITDOWN_NOTIFY:
        case XGameQs::E_QUICK_START_MSGID_STAND_NOTIFY:
        //case XGameKo::E_KNOCKOUT_MSGID_JOIN_MATCH_NOTIFY:
        //case XGameKo::E_KNOCKOUT_MSGID_EXIT_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_SIGN_UP_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_QUIT_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_BEGIN_MATCH_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_SITDOWN_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_EXIT_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RANK_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RISE_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_KNOCKOUT_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_RECOME_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_GAME_BEGIN_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_GET_TABLE_LIST_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_WATCH_GAME_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_REPURCHASE_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_ADDITIONAL_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_BLIND_INFO_NOTIFY:
        case XGameKo::E_KNOCKOUT_MSGID_REWARD_NOTIFY:
        case XGamePr::E_PRIVATE_MSGID_SITDOWN_NOTIFY:
        case XGamePr::E_PRIVATE_MSGID_DISMISS_ROOM_NOTIFY:
        case XGameSh::E_SH_MSGID_MATCH_2C:
        case XGameSh::E_SH_MSGID_EXIT_2C:
        case XGameDDZ::E_DDZ_MSGID_MATCH_2C:
        case XGameDDZ::E_DDZ_MSGID_EXIT_2C:
        case XGameMJ::E_MJ_MSGID_MATCH_2C:
        case XGameMJ::E_MJ_MSGID_EXIT_2C:
        case XGameSNG::E_SNG_MSGID_ENTER_2C:
        case XGameSNG::E_SNG_MSGID_PROCESS_2C:
        case XGameSNG::E_SNG_MSGID_RANK_RESULT_2C:
        case XGameSNG::E_SNG_MSGID_GAME_END_2C:
        case XGameMtt::E_MTT_MSGID_ENTER_2C:
        case XGameMtt::E_MTT_MSGID_MATCH_BEGIN_2C:
        case XGameMtt::E_MTT_MSGID_DISMISS_2C:
        case XGameMtt::E_MTT_MSGID_RANK_RESULT_2C:
        case XGameMtt::E_MTT_MSGID_RAISE_BLIND_2C:
        case XGameMtt::E_MTT_MSGID_RANK_BROAD_2C:
        case XGameMtt::E_MTT_MSGID_UPGRADE_2C:
        case XGameMtt::E_MTT_MSGID_ROOM_INFO_2C:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_NOTIFY);
            break;

        default:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_RESPONSE);
            break;
        }

        os << *it << ", ";
    }

    os << "}";

    HeadInfo stHeadInfo;
    CPlayerMngSingleton::getInstance()->getUHInfoFromUMap(stRespInfo.lUID, stHeadInfo);
    tPkg.set_igameid(stHeadInfo.iGameID);
    tPkg.set_sroomid(stHeadInfo.sRoomID);

    auto ptuid = tPkg.mutable_stuid();
    ptuid->set_luid(stRespInfo.lUID);

    /*    if (CPlayerMngSingleton::getInstance()->getUStateFromUMap(stRespInfo.lUID) == E_PLAYER_OFFLIEN &&
            (stRespInfo.vecMsgIDList[0] != XGameKo::E_KNOCKOUT_MSGID_LOGIN_ROOM_RESP && stRespInfo.vecMsgIDList[0] != XGameQs::E_QUICK_START_MSGID_LOGIN_ROOM_RESP))
        {
            LOG_DEBUG << stRespInfo.lUID << "|" << stHeadInfo.sRoomID << "|" << "user is offline." << endl;
            return 0;
        }
    */
    for (auto it = stRespInfo.vecMsgDataList.begin();  it != stRespInfo.vecMsgDataList.end(); ++it)
    {
        string msg;
        msg.assign((*it).begin(), (*it).end());
        tPkg.add_vecmsgdata(msg);
    }

    if (stHeadInfo.sServantAddr == "" || CPlayerMngSingleton::getInstance()->isRobot(stRespInfo.lUID))
    {
        //LOG_ERROR << stRespInfo.lUID << "|" << stHeadInfo.sRoomID << "|" << "to client msg error, router addr is null." << endl;
        return 0;
    }

    auto pPushPrx = Application::getCommunicator()->stringToProxy<JFGame::PushPrx>(stHeadInfo.sServantAddr);
    if (pPushPrx)
    {
        pPushPrx->tars_hash(tPkg.stuid().luid())->async_doPushBufByRoomID(NULL, tPkg.stuid().luid(), pbToString(tPkg), stHeadInfo.sRoomID, stRespInfo.vecMsgIDList[0]);
    }

    LOG_DEBUG << "Push message to client(2), uid: " << tPkg.stuid().luid()
              << ", pkgLen: " << pbToString(tPkg).length()
              << ", msgID: " << stRespInfo.vecMsgIDList[0]
              << ", servantAddr:" << stHeadInfo.sServantAddr << endl;

    //过滤游戏消息
    if (stRespInfo.vecMsgIDList[0] != XGameComm::E_MSGID_GAME_SO_NOTIFY)
    {
        LOG_DEBUG << stRespInfo.lUID << "|" << "toClientMsg, info = { uid:" << stRespInfo.lUID
                  << ", roomid:" << stHeadInfo.sRoomID << ", " << os.str()
                  << ", msgDataSize:" << stRespInfo.vecMsgDataList.size()
                  << " }" << endl;
    }

    return 0;
}

//发送给客户端
int OuterFactoryImp::toDelayClientPb(const RespInfo &stRespInfo, const string addr)
{
    if (stRespInfo.vecMsgIDList.empty())
    {
        LOG_ERROR << "no msg to send client, uid: " << stRespInfo.lUID << endl;
        return 0;
    }

    //data
    XGameComm::TPackage tPkg;

    //
    ostringstream os;
    os << "msgid list = { ";
    for (auto it = stRespInfo.vecMsgIDList.begin(); it != stRespInfo.vecMsgIDList.end(); ++it)
    {
        auto mh = tPkg.add_vecmsghead();
        mh->set_nmsgid(*it);

        switch (*it)
        {
        case XGamePr::E_PRIVATE_MSGID_SITDOWN_NOTIFY:
        case XGamePr::E_PRIVATE_MSGID_DISMISS_ROOM_NOTIFY:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_NOTIFY);
            break;

        default:
            mh->set_nmsgtype(XGameComm::MSGTYPE::MSGTYPE_RESPONSE);
            break;
        }

        os << *it << ", ";
    }

    os << "}";

    HeadInfo stHeadInfo;
    CPlayerMngSingleton::getInstance()->getUHInfoFromUMap(stRespInfo.lUID, stHeadInfo);
    tPkg.set_igameid(stHeadInfo.iGameID);
    tPkg.set_sroomid(stHeadInfo.sRoomID);

    auto ptuid = tPkg.mutable_stuid();
    ptuid->set_luid(stRespInfo.lUID);

    for (auto it = stRespInfo.vecMsgDataList.begin();  it != stRespInfo.vecMsgDataList.end(); ++it)
    {
        string msg;
        msg.assign((*it).begin(), (*it).end());
        tPkg.add_vecmsgdata(msg);
    }

    auto pPushPrx = Application::getCommunicator()->stringToProxy<JFGame::PushPrx>(addr);
    if (pPushPrx)
    {
        pPushPrx->tars_hash(tPkg.stuid().luid())->async_doPushBufByRoomID(NULL, tPkg.stuid().luid(), pbToString(tPkg), stHeadInfo.sRoomID, stRespInfo.vecMsgIDList[0]);
    }

    LOG_DEBUG << "Push message to client(3), uid: " << tPkg.stuid().luid()
              << ", pkgLen: " << pbToString(tPkg).length()
              << ", msgID: " << stRespInfo.vecMsgIDList[0]
              << ", servantAddr:" << addr << endl;

    //过滤游戏消息
    if (stRespInfo.vecMsgIDList[0] != XGameComm::E_MSGID_GAME_SO_NOTIFY)
    {
        LOG_DEBUG << stRespInfo.lUID << "|" << "toClientMsg, info = { uid:" << stRespInfo.lUID
                  << ", roomid:" << stHeadInfo.sRoomID << ", " << os.str()
                  << ", msgDataSize:" << stRespInfo.vecMsgDataList.size()
                  << " }" << endl;
    }

    return 0;
}

void OuterFactoryImp::logRobotWinStat(const string roomid, map<int, std::map<int, long>> &m_RobotWinStat)
{
    DaqiGame::TLog2DBReq tLog2DBReq;
    tLog2DBReq.sLogType = 30;

    vector<string> veclog;
    veclog.push_back(roomid);

    long total_win = 0;
    for(auto it : m_RobotWinStat)
    {
        for(auto itt : it.second)
        {
            total_win += itt.second;
        }
    }
    veclog.push_back(L2S(total_win));

    time_t t = time(NULL);
    t -= 24 * 3600;
    auto pTm = localtime(&t);

    char sTimeString[255] = "\0";
    std::string farmat_str = "%Y-%m-%d";
    strftime(sTimeString, sizeof(sTimeString), farmat_str.c_str(), pTm);
    veclog.push_back(string(sTimeString));
    tLog2DBReq.vecLogData.push_back(veclog);
    g_app.getOuterFactoryPtr()->asyncLog2DB(tars::hash<string>()(roomid), tLog2DBReq);
}

void OuterFactoryImp::logDayStat(const string roomid, map<string, TableDayStat> &dayStat, const std::map<long, long> &m_UGame)
{
    time_t t = time(NULL);
    //t -= 24 * 3600;
    auto pTm = localtime(&t);

    char sTimeString[255] = "\0";
    std::string farmat_str = "%Y-%m-%d";
    strftime(sTimeString, sizeof(sTimeString), farmat_str.c_str(), pTm);

    for (auto item : dayStat)
    {
        DaqiGame::TLog2DBReq tLog2DBReq;
        tLog2DBReq.sLogType = 31;
        vector<string> veclog;

        vector<std::string> vParams = split(item.first, "|");
        if (vParams.size() != 3)
        {
            continue;
        }

        if(m_UGame.size() > 0)
        {
            veclog.push_back(roomid);
            veclog.push_back(vParams[0]);
            veclog.push_back(vParams[1]);
            veclog.push_back(vParams[2]);
            veclog.push_back(L2S(item.second.round_num));
            veclog.push_back(L2S(0)); // 游戏人数
            veclog.push_back(L2S(item.second.game_time));
            veclog.push_back(L2S(0));// 下注率
            veclog.push_back(L2S(item.second.robot_win));
            veclog.push_back(L2S(item.second.sys_recyle));
            veclog.push_back(L2S(item.second.sys_output));
            veclog.push_back(L2S(item.second.total_win));
            veclog.push_back(L2S(item.second.total_lose));
            veclog.push_back(L2S(item.second.s_Create.size()));
            veclog.push_back(string(sTimeString));
            tLog2DBReq.vecLogData.push_back(veclog);
            g_app.getOuterFactoryPtr()->asyncLog2DB(tars::hash<string>()(roomid), tLog2DBReq);
        }

        for(auto subitem : m_UGame)
        {
            tLog2DBReq.sLogType = 32;
            tLog2DBReq.vecLogData.clear();
            veclog.clear();
            veclog.push_back(roomid);
            veclog.push_back(vParams[0]);
            veclog.push_back(vParams[1]);
            veclog.push_back(vParams[2]);
            veclog.push_back(L2S(subitem.first)); //uid

            auto it = item.second.m_UGame.find(subitem.first);
            auto playNum = it != item.second.m_UGame.end() ? it->second : 0;
            veclog.push_back(L2S(playNum)); //play num

            if(playNum <= 0)
            {
                continue;
            }

            auto itt = item.second.m_UBet.find(subitem.first);
            auto betNum =  itt != item.second.m_UBet.end() ? itt->second : 0;
            veclog.push_back(L2S(betNum)); //bet num

            veclog.push_back(string(sTimeString));
            tLog2DBReq.vecLogData.push_back(veclog);
            g_app.getOuterFactoryPtr()->asyncLog2DB(tars::hash<string>()(roomid), tLog2DBReq);
        }
    }
}

void OuterFactoryImp::logCoinChange(const long lPlayerID, const long changeCoin, const int changeType)
{
    long currGold = CPlayerMngSingleton::getInstance()->getUGoldCoinFromUMap(lPlayerID);

    LOG_DEBUG<<"lPlayerID: "<< lPlayerID << ", changeCoin: "<< changeCoin << ", changeType: "<< changeType << ", currGold: "<< currGold << endl;

    if(changeCoin == 0 || CPlayerMngSingleton::getInstance()->isRobot(lPlayerID))
    {
        return;
    }

    DaqiGame::TLog2DBReq tLog2DBReq;
    tLog2DBReq.sLogType = 28;

    vector<string> veclog;
    veclog.push_back(L2S(lPlayerID));
    veclog.push_back(I2S(10000));
    veclog.push_back(L2S(changeType));
    veclog.push_back(L2S(changeCoin));
    veclog.push_back(L2S(currGold));
    veclog.push_back(I2S(0));

    tLog2DBReq.vecLogData.push_back(veclog);
    asyncLog2DB(lPlayerID, tLog2DBReq); 
}

void OuterFactoryImp::logGameRoundInfo(const string &roomID, const int iTableID, long roundID, int maxSeat, int blindlevel,RoomSo::TGAME_GameFinish *pGameFinish, long sys_output, long sys_recyle)
{
    if (!pGameFinish)
    {
        LOG_ERROR << "pGameFinish is nulllptr." << endl;
        return ;
    }

    std::string sDetialID = L2S(TNOW) + I2S(iTableID);

    bool exist_player = false;
    for (auto item : pGameFinish->mapUBet)
    {
        if (CPlayerMngSingleton::getInstance()->isRobot(item.first))
            continue;

        exist_player = true;
        auto winCount = 0;
        auto curCount = 0;
        auto it = pGameFinish->mapUWinInfo.find(item.first);
        if (it != pGameFinish->mapUWinInfo.end())
        {
            winCount = it->second;
        }

        auto itt = pGameFinish->realUWealth.find(item.first);
        if (itt != pGameFinish->realUWealth.end())
        {
            curCount = itt->second;
        }

        DaqiGame::TLog2DBReq tLog2DBReq;
        tLog2DBReq.sLogType = 29;
        vector<string> veclog;
        veclog.push_back(roomID);
        veclog.push_back(L2S(roundID));
        veclog.push_back(I2S(maxSeat));
        veclog.push_back(L2S(blindlevel));
        veclog.push_back(L2S(item.first));
        veclog.push_back(L2S(winCount));
        veclog.push_back(L2S(curCount));
        veclog.push_back(L2S(curCount - winCount));
        veclog.push_back(L2S(iTableID));
        veclog.push_back(sDetialID);

        int is_bet = 0;
        auto subit = std::find_if(pGameFinish->mapUserActInfo.begin(), pGameFinish->mapUserActInfo.end(), [item](std::pair<const short int, RoomSo::TGAME_UserActInfo>& info)->bool{
            return item.first == info.second.uid;
        });
        if(subit != pGameFinish->mapUserActInfo.end())
        {
            is_bet = subit->second.intoPoolCount > 0? 1 : 0;
        }
        veclog.push_back(L2S(is_bet));
        veclog.push_back(L2S(pGameFinish->day_stat.game_time));
        veclog.push_back(L2S(pGameFinish->day_stat.robot_win));

        auto eGameMode = OuterFactorySingleton::getInstance()->parseGameMode(roomID);
        if(E_SNG_MODE == eGameMode)
        {
            veclog.push_back(L2S(sys_output));
            veclog.push_back(L2S(sys_recyle));
        }
        else
        {
            veclog.push_back(L2S(0));
            veclog.push_back(L2S(pGameFinish->day_stat.sys_recyle));
        }
        
        tLog2DBReq.vecLogData.push_back(veclog);
        g_app.getOuterFactoryPtr()->asyncLog2DB(tars::hash<string>()(roomID), tLog2DBReq);
    }

    if(exist_player)
    {
        //牌局详情
        Pb::GameDetails game_details;
        game_details.ParseFromString(pGameFinish->game_details);
        std::string sDetialInfo;
        game_details.SerializeToString(&sDetialInfo);

        DaqiGame::TLog2DBReq tLog2DBReq;
        tLog2DBReq.sLogType = 35;
        vector<string> veclog;
        veclog.push_back(sDetialID);
        veclog.push_back(sDetialInfo);

        tLog2DBReq.vecLogData.push_back(veclog);
        g_app.getOuterFactoryPtr()->asyncLog2DB(tars::hash<string>()(roomID), tLog2DBReq);
    }

    return;
}

//推送用户帐号信息
int OuterFactoryImp::async2PushUserAccountInfo(const long lPlayerID)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    pHallServant->async_pushUserAccountInfo(NULL, lPlayerID);
    return 0;
}

//同步玩家金币变化
int OuterFactoryImp::sync2UserWealthToRoom(const long lPlayerID)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    userinfo::GetUserDetailToRoomReq req;
    req.uid = lPlayerID;
    userinfo::GetUserDetailToRoomResp resp;
    int ret = pHallServant->getUserDetailToRoom(req, resp);
    if (0 != ret)
    {
        LOG_ERROR << "sync2UserWealthToRoom failed. lPlayerID:" << lPlayerID << endl;
        return -1;
    }

    //更新玩家金币
    long lRoomGold = CPlayerMngSingleton::getInstance()->getUGoldCoinFromUMap(lPlayerID);
    long lUGoldCoin = resp.gold - lRoomGold;
    if (lUGoldCoin < 0)
    {
        lUGoldCoin = 0;
        LOG_ERROR << "game data error, lPlayerID: " << lPlayerID << ", rsp.gold: "  << resp.gold << ", lRoomGold: " << lRoomGold << endl;
    }

    CPlayerMngSingleton::getInstance()->updateUGoldCoinByUID(lPlayerID, lUGoldCoin);
    return 0;
}

//获取用户信息接口
int OuterFactoryImp::async2GetUserBasic(const long lPlayerID)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    userinfo::GetUserBasicReq req;
    req.uid = lPlayerID;
    pHallServant->async_getUserBasic(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "uid: " << lPlayerID << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//获取用户信息接口
int OuterFactoryImp::async2ModifyUserInfo(const userinfo::ModifyUserInfoReq &req)
{
    auto pHallServant = getHallServantPrx(req.intside_uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    pHallServant->async_modifyUserInfo(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "req.intside_uid: " << req.intside_uid << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//获取用户信息接口
int OuterFactoryImp::async2GetPropsInfo(const long lPlayerID)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    hall::UserGiftReq req;
    req.uid = lPlayerID;
    req.group_id = 81000;
    pHallServant->async_getUserGift(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "lPlayerID: " << lPlayerID << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//获取用户信息接口
int OuterFactoryImp::async2ChangePropsInfo(const long lPlayerID, int cost_num)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    hall::UserGiftReq req;
    req.uid = lPlayerID;
    req.group_id = 81000;
    req.number = cost_num;
    pHallServant->async_changeUserGift(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "lPlayerID: " << lPlayerID << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//获取用户帐号信息
int OuterFactoryImp::async2GetUserAccount(const long lPlayerID)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    userinfo::GetUserAccountReq req;
    req.uid = lPlayerID;
    pHallServant->async_getUserAccount(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "uid: " << lPlayerID << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//修改用户帐号接口
int OuterFactoryImp::async2ModifyUAccount(const userinfo::ModifyUserAccountReq &req)
{
    auto pHallServant = getHallServantPrx(req.uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    pHallServant->async_modifyUserAccountEx(new AsyncUserInfoCallback(req), req);
    LOG_DEBUG << "uid: " << req.uid << ", " << printTars(req)  << "}\n <<<< End \n\n\n\n" << endl;
    return 0;
}

//经典场玩家行为信息
int OuterFactoryImp::asyncReportQSUserActInfo(const RoomSo::TGAME_GameFinish &req)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        gamerecord::ReportQSUserActInfoReq reportReq;
        reportReq.uid = it->second.uid;
        reportReq.hdCardCount = it->second.hdCardCount;
        reportReq.poolCount = it->second.poolCount;
        reportReq.winCount = it->second.winCount;
        reportReq.showCount = it->second.showCount;
        reportReq.callCount = it->second.callCount;
        reportReq.raiseCount = it->second.raiseCount;
        reportReq.betCount = it->second.betCount;
        reportReq.intoPoolWinCount = it->second.intoPoolWinCount;
        reportReq.intoPoolCount = it->second.intoPoolCount;
        reportReq.cardType = it->second.cardType;
        reportReq.winNum = it->second.change;
        reportReq.pumpNum = it->second.pumpNum;
        for (auto itcard : it->second.cards)
        {
            tars::Char card = itcard & 0X00FF;
            reportReq.cards.push_back(card);
        }

        auto pRecordServant = getGameRecordPrx(reportReq.uid);
        if (!pRecordServant)
        {
            LOG_ERROR << "pRecordServant is nullptr" << endl;
            continue;
        }

        pRecordServant->async_reportQSUserActInfo(NULL, reportReq);
    }

    return 0;
}

//AI场玩家行为信息
int OuterFactoryImp::asyncReportAIUserActInfo(const RoomSo::TGAME_GameFinish &req, int blindlevel)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        // if(CPlayerMngSingleton::getInstance()->isRobot(it->second.uid))
        // {
        //     continue;
        // }
        auto pRecordServant = getGameRecordPrx(it->second.uid);
        if (!pRecordServant)
        {
            LOG_ERROR << "pRecordServant is nullptr" << endl;
            continue;
        }

        gamerecord::ReportAIUserActInfoReq req_1;
        gamerecord::ReportAIUserCardInfoReq req_2;
        req_1.uid = it->second.uid;
        req_1.hdCardCount = it->second.hdCardCount;
        req_1.winCount = it->second.winCount;
        req_1.blindlevel = blindlevel;
        req_1.cardType = it->second.cardType;
        auto itt = req.mapUWinInfo.find(it->second.uid);
        if(itt != req.mapUWinInfo.end())
        {
            req_1.coinchange = itt->second;
            req_2.coinchange = itt->second;
            req_2.win = it->second.winCount == 1 ? 1 : 0;
        }
        for (auto itcard : it->second.cards)
        {
            tars::Char card = itcard & 0X00FF;
            req_1.cards.push_back(card);
        }

        req_2.uid = it->second.uid;
        for (auto itcard : it->second.hdCards)
        {
            tars::Char card = itcard & 0X00FF;
            req_2.cards.push_back(card);
        }

        pRecordServant->async_reportAIUserActInfo(NULL, req_1, req_2);
        LOG_DEBUG << "req_1: " << printTars(req_1) << "req_2: " << printTars(req_2) << endl;
    }

    return 0;
}

//比赛场玩家行为信息
int OuterFactoryImp::asyncReportKOUserActInfo(const gamerecord::ReportKOUserActInfoReq &req)
{
    auto pRecordServant = getGameRecordPrx(req.uid);
    if (!pRecordServant)
    {
        LOG_ERROR << "pRecordServant is nullptr" << endl;
        return -1;
    }

    pRecordServant->async_reportKOUserActInfo(NULL, req);
    return 0;
}

int OuterFactoryImp::asyncReportUGameInfo(const string &sRoomID, const string &sRoomKey, long lPlayerID, int iStatus, int iBlindLevel, long createID, int seatNum, long createTime, long endTime, int iPet, RoomSo::TGAME_PlayerCal &stCal)
{
    auto pRecordServant = getGameRecordPrx(sRoomKey);
    if (!pRecordServant)
    {
        LOG_ERROR << "pRecordServant is nullptr" << endl;
        return -1;
    }

    gamerecord::RoomCalStatReq req;
    req.sRoomID = sRoomID;
    req.sRoomKey = sRoomKey;
    req.iStatus = iStatus;
    req.iBlindLevel = iBlindLevel;
    req.lGameTime = createTime;
    req.lEndTime = endTime;
    req.lCreaterID = createID;
    req.seatCount = seatNum;
    req.iPet = iPet;

    if (lPlayerID > 0)
    {
        UInfo stUInfo;
        CPlayerMngSingleton::getInstance()->getUInfoFromUMap(lPlayerID, stUInfo);

        gamerecord::UserStat user_stat;
        user_stat.lUid = lPlayerID;
        user_stat.sNickName = stUInfo.sNickName;
        user_stat.iBuyInCount = stCal.buyInCount;
        user_stat.lBuyInNum = stCal.buyInNum;
        user_stat.lWinNum = stCal.winNum;
        user_stat.lFree = stCal.free;

        req.info[lPlayerID] = user_stat;
    }

    LOG_DEBUG<<"asyncReportUGameInfo: " << printTars(req) << endl;
    pRecordServant->async_reportPrGameInfo(NULL, req);
    return 0;
}

// 异步调用UserStateServer, ongame
void OuterFactoryImp::asyncRequest2UserStateOnGame(long iUin, const string &sRoomID, int tableID, long gold, int matchID)
{
    auto pPushServant = getPushServerPrx(iUin);
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    userstate::ReportGameStateReq req;
    req.uid = iUin;
    req.state = userstate::E_GAME_STATE_ON;
    req.gameAddr = g_sLocalRoomObj;
    req.sRoomID = sRoomID;
    req.tableID = tableID;
    req.gold    = gold;
    req.matchID = matchID;
    pPushServant->async_reportGameState(NULL, req);
}

// 异步调用UserStateServer, offgame
void OuterFactoryImp::asyncRequest2UserStateOffGame(long iUin)
{
    auto pPushServant = getPushServerPrx(iUin);
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    userstate::ReportGameStateReq req;
    req.uid = iUin;
    req.state = userstate::E_GAME_STATE_OFF;
    pPushServant->tars_hash(iUin)->async_reportGameState(NULL, req);
}

//返回补偿金币
void OuterFactoryImp::asyncRequest2UserReturnCompensateGold(const string &sRoomID)
{
    auto pPushServant = getPushServerPrx(tars::hash<string>()(sRoomID));
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    pPushServant->async_returnCompensateGold(NULL, sRoomID);
}

//房间玩家集合
void OuterFactoryImp::asyncRequest2RoomUsers(const string &sRoomID, const string &uids, map<int, std::map<int, long>> &m_RobotWinStat, long totalRobotWin)
{
    if (sRoomID.empty())
    {
        return;
    }

    string Uids;
    string blindUids_5;
    string blindUids_9;
    std::map<int, std::vector<long>> m_blindUsers_5;
    std::map<int, std::vector<long>> m_blindUsers_9;
    vector<std::string> vUids = split(uids, "|");
    for (auto uid : vUids)
    {
        if(CPlayerMngSingleton::getInstance()->isRobot(S2L(uid)) || !CPlayerMngSingleton::getInstance()->isExistU2TableMap(S2L(uid)))
        {
            continue;
        }

        Uids += ("|" + uid);

        Eum_UState eUState = CPlayerMngSingleton::getInstance()->getUStateFromUMap(S2L(uid));
        if (eUState != E_PLAYING_GAME)
        {
            LOG_DEBUG << "uids:" << uid << ", eUState: " << eUState << endl;
            continue;
        }

        int level = CPlayerMngSingleton::getInstance()->getUTableBlindLevel(S2L(uid), sRoomID[0] - '0');
        int seatNum = CPlayerMngSingleton::getInstance()->getUTableMaxSeat(S2L(uid), sRoomID[0] - '0');

        LOG_DEBUG << "uids:" << uid << ", level: " << level << ", seatNum: " << seatNum << endl;
        if (level > 0)
        {
            if (seatNum == 5)
            {
                auto it = m_blindUsers_5.find(level);
                if (it == m_blindUsers_5.end())
                {
                    m_blindUsers_5[level] = std::vector<long> {S2L(uid)};
                }
                else
                {
                    it->second.push_back(S2L(uid));
                }
            }
            else
            {
                auto it = m_blindUsers_9.find(level);
                if (it == m_blindUsers_9.end())
                {
                    m_blindUsers_9[level] = std::vector<long> {S2L(uid)};
                }
                else
                {
                    it->second.push_back(S2L(uid));
                }
            }
        }
    }

    bool bFirstFlag = true;
    for (auto item : m_blindUsers_5)
    {
        string subItem = I2S(item.first) + ":" + I2S(item.second.size());
        blindUids_5 += (bFirstFlag ? "" : "|" ) + subItem;
        bFirstFlag = false;
    }

    bFirstFlag = true;
    for (auto item : m_blindUsers_9)
    {
        string subItem = I2S(item.first) + ":" + I2S(item.second.size());
        blindUids_9 += (bFirstFlag ? "" : "|" ) + subItem;
        bFirstFlag = false;
    }

    //机器人输赢
    string robotWin_5;
    string robotWin_9;
    for(auto it : m_RobotWinStat)
    {
        if(it.first == 5)
        {
            bFirstFlag = true;
            for(auto item : it.second)
            {
                string subItem = I2S(item.first) + ":" + L2S(item.second);
                robotWin_5 += (bFirstFlag ? "" : "|" ) + subItem;
                bFirstFlag = false;
            }
        }
        else
        {
            bFirstFlag = true;
            for(auto item : it.second)
            {
                string subItem = I2S(item.first) + ":" + L2S(item.second);
                robotWin_9 += (bFirstFlag ? "" : "|" ) + subItem;
                bFirstFlag = false;
            }
        }
    }

    auto pPushServant = getPushServerPrx(tars::hash<string>()(sRoomID));
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    LOG_DEBUG << "uids:" << Uids << ", blindUids_5:" << blindUids_5 << ", blindUids_9: " << blindUids_9 << ", robotWin_5: " << robotWin_5 << ", robotWin_9: " << robotWin_9 << endl;
    pPushServant->async_reportRoomUsers(NULL, sRoomID, Uids, blindUids_5, blindUids_9, robotWin_5, robotWin_9);
}

//推送比赛开始消息
void OuterFactoryImp::asyncRequest2PushMatchBegin(const push::PushMsgReq &msg)
{
    auto pPushServant = getPushServerPrx(rand() % 2);
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    pPushServant->async_pushMsg(NULL, msg);
}

//推送邀请消息
void OuterFactoryImp::asyncRequest2PushInvitePlayer(const push::PushMsgReq &msg)
{
    auto pPushServant = getPushServerPrx(rand() % 2);
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return;
    }

    pPushServant->async_pushMsg(NULL, msg);
}

//获取比赛信息
int OuterFactoryImp::asyncGetMatchUserInfo(const GetMatchInfoParam &reqGetMatchInfo, const match::MatchUserInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_getMatchUserInfo(new AsyncMatchCallback(reqGetMatchInfo), req);
    return 0;
}

//取消比赛
int OuterFactoryImp::asyncCancelMatch(const GetMatchInfoParam &reqGetMatchInfo, const match::CancelMatchReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_cancelMatch(NULL, req);
    return 0;
}

//清理比赛
int OuterFactoryImp::asyncCleanMatch(const GetMatchInfoParam &reqGetMatchInfo, const match::CleanMatchReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.Uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_cleanMatch(NULL, req);
    return 0;
}

//检查重购资格
int OuterFactoryImp::asyncRepurchaseCheck(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_getUserSignUpInfo(new AsyncRepurchaseCheckCallback(reqGetMatchInfo), req);
    return 0;
}

//重购
int OuterFactoryImp::asyncRepurchaseConsume(const GetMatchInfoParam &reqGetMatchInfo, const match::ConsumeBuyResReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_consumeBuyRes(new AsyncRepurchaseConsumeCallback(reqGetMatchInfo), req);
    return 0;
}

//检查增购资格
int OuterFactoryImp::asyncAdditionalCheck(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_getUserSignUpInfo(new AsyncAdditionalCheckCallback(reqGetMatchInfo), req);
    return 0;
}

//更新玩家晋级状态
int OuterFactoryImp::asyncUpdateRise(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_getUserSignUpInfo(new AsyncUpdateRiseCallback(reqGetMatchInfo), req);
    return 0;
}

//增购
int OuterFactoryImp::asyncAdditionalConsume(const GetMatchInfoParam &reqGetMatchInfo, const match::ConsumeBuyResReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.uid);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_consumeBuyRes(new AsyncAdditionalConsumeCallback(reqGetMatchInfo), req);
    return 0;
}

//上报用户信息
int OuterFactoryImp::asyncReportUserInfo(const match::ReportUserGameInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reportUserGameInfo(NULL, req);
    return 0;
}

int OuterFactoryImp::asyncReportRoomOnlineCount(const string roomID, long smallBlind, long count)
{
    auto checkSUM = roomID.length() < 7 ? 1 : atol(roomID.substr(2, 4).c_str());
    auto pMatchServant = getMatchServantPrx(checkSUM);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }
    LOG_DEBUG << "roomID: " << roomID << ", count:" << count << endl;
    pMatchServant->async_reportOnlineCount(NULL, roomID, smallBlind, count);
    return 0;
}

//上报比赛信息
int OuterFactoryImp::asyncReportMatchInfo(const int matchID, const match::ReportMatchInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reportMatchInfo(NULL, req);
    return 0;
}

//上报滚轮赛奖励
int OuterFactoryImp::asyncReportMatchReward(const match::RewardGoods &req, int matchID, int tableID)
{
    auto pMatchServant = getMatchServantPrx(matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reportMatchReward(NULL, req, matchID, tableID);
    return 0;
}

//上报玩家淘汰信息
int OuterFactoryImp::asyncReportUserKnockoutInfo(const match::ReportUserKnockoutInfoReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reportUserKnockoutInfo(NULL, req);
    return 0;
}

//上报玩家淘汰信息
int OuterFactoryImp::asyncReportUserKnockoutInfo(const int iMatchID, const long lPlayerID)
{
    auto pMatchServant = getMatchServantPrx(iMatchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    match::ReportUserKnockoutInfoReq req;
    req.matchID = iMatchID;
    req.vecUin.push_back(lPlayerID);
    pMatchServant->async_reportUserKnockoutInfo(NULL, req);
    return 0;
}

//上报玩家状态信息
int OuterFactoryImp::asyncReportUserStateInfo(const match::ReportUserStateReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reportUserState(NULL, req);
    return 0;
}

//上报玩家状态信息
int OuterFactoryImp::asyncReportUserStateInfo(const int iMatchID, const long lPlayerID, match::UserState state)
{
    auto pMatchServant = getMatchServantPrx(iMatchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    match::ReportUserStateReq req;
    req.matchID = iMatchID;
    req.mapState[lPlayerID] = state;
    pMatchServant->async_reportUserState(NULL, req);
    return 0;
}

//发送奖励
int OuterFactoryImp::asyncReward(const GetMatchInfoParam &reqGetMatchInfo, const match::RewardReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_reward(new AsyncRewardCallback(reqGetMatchInfo), req);
    return 0;
}

//报名
int OuterFactoryImp::asyncSignUp(const GetMatchInfoParam &reqGetMatchInfo, const match::SignUpReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_signUp(new AsyncSignUpCallback(reqGetMatchInfo), req);
    return 0;
}

//取消报名
int OuterFactoryImp::asyncQuit(const GetMatchInfoParam &reqGetMatchInfo, const match::QuitReq &req)
{
    auto pMatchServant = getMatchServantPrx(req.matchID);
    if (!pMatchServant)
    {
        LOG_ERROR << "pMatchServant is nullptr" << endl;
        return -1;
    }

    pMatchServant->async_quit(new AsyncQuitCallback(reqGetMatchInfo), req);
    return 0;
}

//用户服务映射map
int OuterFactoryImp::asyncCheckUser(const long lPlayerID, const std::string sRoomID)
{
    int iRet = 0;

    /*
    int64_t tTime = TNOWMS;

    userservermap::GetRoomStatusOfUserReq req;
    req.uid = lPlayerID;

    CoroParallelBasePtr sharedPtr = new CoroParallelBase(1);

    CoroUserServerMapCallbackPtr cb = new CoroUserServerMapCallback(req);
    cb->setCoroParallelBasePtr(sharedPtr);
    getUserServerMapPrx()->coro_getRoomStatusOfUser(cb, req);

    coroWhenAll(sharedPtr);

    LOG_DEBUG << lPlayerID << "|" << sRoomID << "|" << "check user coro callback success, ret:" << cb->m_respRetGRSOU
        << ", costTime:" << (TNOWMS - tTime) << "ms" << endl;

    if (cb->m_respRetGRSOU == 0)
    {
        if (cb->m_respGRSOU.roomId != sRoomID)
        {
            LOG_ERROR << lPlayerID << "|" << "user exist other Room, reqRoomID:" << sRoomID
                << ", existRoomID:" << cb->m_respGRSOU.roomId << endl;
            return -1;
        }
    }
    else
    {
        iRet = cb->m_respRetGRSOU;
    }
    */

    return iRet;
}

//ai robot
int OuterFactoryImp::async2GetRobot(const int type, const ai::TGetRobotReq &req)
{
    auto pAiServant = getAIServantPrx(type, req.sRoomID);
    if (!pAiServant)
    {
        LOG_ERROR << "pAiServant is nullptr" << endl;
        return -1;
    }

    LOG_DEBUG << "async2GetRobot, req4Robot:" << logTars(req) << endl;
    FDLOG_ROBOT_INFO << "async2GetRobot, req4Robot:" << logTars(req) << endl;
    pAiServant->async_getRobot(NULL, req);
    return 0;
}

//
int OuterFactoryImp::async2RetrieveRobot(const int type, const string &sRoomID)
{
    auto pAiServant = getAIServantPrx(type, sRoomID);
    if (!pAiServant)
    {
        LOG_ERROR << "pAiServant is nullptr" << endl;
        return -1;
    }

    ai::TRetrieveRobotReq req;
    req.sRoomID = sRoomID;
    LOG_DEBUG << "retrieve robot, req4Robot:" << logTars(req) << endl;
    FDLOG_ROBOT_INFO << "retrieve robot, req4Robot:" << logTars(req) << endl;
    pAiServant->async_RetrieveRobot(NULL, req);
    return 0;
}

//上报任务数据
int OuterFactoryImp::async2TaskReport(const task::TaskReportReq &req)
{
    auto pHallServant = getHallServantPrx(req.uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    LOG_DEBUG << "task msg, TaskReportReq:" << logTars(req) << endl;
    pHallServant->async_report(NULL, req);
    return 0;
}

//上报任务数据
int OuterFactoryImp::async2TaskCollect(const task::CollectReq &req)
{
    if (CPlayerMngSingleton::getInstance()->isRobot(req.uid))
    {
        return 0;
    }

    auto pHallServant = getHallServantPrx(req.uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    LOG_DEBUG << "task msg, CollectReq:" << logTars(req) << endl;
    pHallServant->async_collectRecord(NULL, req);
    return 0;
}

//参与AI场游戏
int OuterFactoryImp::async2TaskCollectAI(const RoomSo::TGAME_GameFinish &req, const string &roomID)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        auto uid = it->second.uid;
        if(CPlayerMngSingleton::getInstance()->isRobot(uid))
        {
            continue;
        }
        auto pHallServant = getHallServantPrx(uid);
        if (!pHallServant)
        {
            LOG_ERROR << "pHallServant is nullptr" << endl;
            continue;
        }

        task::CollectReq collectReq;
        collectReq.uid = uid;
        task::TaskData taskData;
        taskData.taskType = task::E_TASK_SUB_TYPE_AI_ROOM;
        taskData.data.clear();
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);

        if (it->second.change > 0)
        {
            //AI场房间赢得筹码数量
            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_AI_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_ANY_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINCHIPS_ANY;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);
        }

        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
        pHallServant->async_collectRecord(NULL, collectReq);
    }

    return 0;
}

//参与AOF场游戏
int OuterFactoryImp::async2TaskCollectAOF(const RoomSo::TGAME_GameFinish &req, const string &roomID)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        auto uid = it->second.uid;
        if(CPlayerMngSingleton::getInstance()->isRobot(uid))
        {
            continue;
        }
        auto pHallServant = getHallServantPrx(uid);
        if (!pHallServant)
        {
            LOG_ERROR << "pHallServant is nullptr" << endl;
            continue;
        }

        task::CollectReq collectReq;
        collectReq.uid = uid;
        task::TaskData taskData;
        taskData.taskType = task::E_TASK_SUB_TYPE_AOF_ROOM;
        taskData.data.clear();
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);

        if (it->second.change > 0)
        {
            //AOF场房间赢得筹码数量
            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_AOF_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_ANY_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINCHIPS_ANY;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);
        }

        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
        pHallServant->async_collectRecord(NULL, collectReq);
    }

    return 0;
}

//参与经典场游戏
int OuterFactoryImp::async2TaskCollectQS(const RoomSo::TGAME_GameFinish &req, const string &roomID)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        auto uid = it->second.uid;
        if(CPlayerMngSingleton::getInstance()->isRobot(uid))
        {
            continue;
        }
        auto pHallServant = getHallServantPrx(uid);
        if (!pHallServant)
        {
            LOG_ERROR << "pHallServant is nullptr" << endl;
            continue;
        }

        task::CollectReq collectReq;
        collectReq.uid = uid;
        task::TaskData taskData;
        taskData.taskType = task::E_TASK_SUB_TYPE_SPECIFY_QS_ROOM;
        taskData.data["roomId"] = roomID;
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);
        taskData.taskType = task::E_TASK_SUB_TYPE_ANY_QS_ROOM;
        taskData.data.clear();
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);

        if (it->second.change > 0)
        {
            //经典场房间赢得筹码数量
            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_QS_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_ANY_ROOM;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINCHIPS_QS;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINCHIPS_ANY;
            taskData.data.clear();
            taskData.data["value"] = L2S(it->second.change);
            collectReq.detail.push_back(taskData);

            taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINROUND_QS;
            taskData.data.clear();
            taskData.data["value"] = "1";
            collectReq.detail.push_back(taskData);
        }

        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
        pHallServant->async_collectRecord(NULL, collectReq);
    }

    return 0;
}

//参与SNG比赛
int OuterFactoryImp::async2TaskCollectSNG(long uid, const string &roomID)
{
    //更新SNG比赛任务进度
    task::CollectReq collectReq;
    collectReq.uid = uid;
    task::TaskData taskData;
    taskData.taskType = task::E_TASK_SUB_TYPE_ANY_SNG_ROOM;
    taskData.data["value"] = "1";
    collectReq.detail.push_back(taskData);
    async2TaskCollect(collectReq);
    return 0;
}

//参与比赛
int OuterFactoryImp::async2TaskCollectKOPlayingIn(long uid, const string &roomID)
{
    if(CPlayerMngSingleton::getInstance()->isRobot(uid))
    {
        return 0;
    }
    auto pHallServant = getHallServantPrx(uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    int iGameMode = getGameModeFromRoomID(roomID);
    if (iGameMode == E_SNG_MODE)
    {
        task::E_TASK_SUB_TYPE specifyType = task::E_TASK_SUB_TYPE_NONE;
        task::E_TASK_SUB_TYPE anyType = task::E_TASK_SUB_TYPE_NONE;
        int iPlayersNum = getPlayersNumFromRoomID(roomID);
        if (iPlayersNum == 3)
        {
            specifyType = task::E_TASK_SUB_TYPE_SPECIFY_SNG3_ROOM;
            anyType = task::E_TASK_SUB_TYPE_ANY_SNG3_ROOM;
        }
        else if (iPlayersNum == 5)
        {
            specifyType = task::E_TASK_SUB_TYPE_SPECIFY_SNG5_ROOM;
            anyType = task::E_TASK_SUB_TYPE_ANY_SNG5_ROOM;
        }
        else if (iPlayersNum == 9)
        {
            specifyType = task::E_TASK_SUB_TYPE_SPECIFY_SNG9_ROOM;
            anyType = task::E_TASK_SUB_TYPE_ANY_SNG9_ROOM;
        }
        else
        {
            LOG_ERROR << "getPlayersNumFromRoomID iPlayersNum:" << iPlayersNum << " error, roomID:" << roomID << endl;
        }

        task::CollectReq collectReq;
        collectReq.uid = uid;

        task::TaskData taskData;
        taskData.taskType = specifyType;
        taskData.data["roomId"] = roomID;
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);

        taskData.taskType = anyType;
        taskData.data.clear();
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);

        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
        pHallServant->async_collectRecord(NULL, collectReq);
    }

    return 0;
}

//获得比赛冠军奖励
int OuterFactoryImp::async2TaskCollectKOChampion(long uid, const string &roomID)
{
    if(CPlayerMngSingleton::getInstance()->isRobot(uid))
    {
        return 0;
    }
    auto pHallServant = getHallServantPrx(uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    int iGameMode = getGameModeFromRoomID(roomID);
    if ((iGameMode == E_KNOCKOUT_MODE) || (iGameMode == E_SNG_MODE))
    {
        task::CollectReq collectReq;
        collectReq.uid = uid;

        task::TaskData taskData;
        taskData.taskType = (iGameMode == E_KNOCKOUT_MODE) ? task::E_TASK_SUB_TYPE_MTT_MATCH_CHAMPION : task::E_TASK_SUB_TYPE_SNG_MATCH_CHAMPION;
        taskData.data["value"] = "1";
        collectReq.detail.push_back(taskData);
        pHallServant->async_collectRecord(NULL, collectReq);
        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
    }

    return 0;
}

//获得比赛场奖励筹码数量
int OuterFactoryImp::async2TaskCollectKORewardChips(long uid, const string &roomID, const long &lChips)
{
    if(CPlayerMngSingleton::getInstance()->isRobot(uid))
    {
        return 0;
    }
    auto pHallServant = getHallServantPrx(uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    int iGameMode = getGameModeFromRoomID(roomID);
    if (((iGameMode == E_KNOCKOUT_MODE) || (iGameMode == E_SNG_MODE)) && (lChips > 0))
    {
        //比赛场奖励筹码数量
        task::CollectReq collectReq;
        collectReq.uid = uid;

        task::TaskData taskData;
        taskData.taskType = task::E_TASK_SUB_TYPE_WINCHIPS_ANY_ROOM;
        taskData.data["value"] = L2S(lChips);
        collectReq.detail.push_back(taskData);

        taskData.taskType = task::E_TASK_SUB_TYPE_RECHARGE_WINCHIPS_ANY;
        taskData.data.clear();
        taskData.data["value"] = L2S(lChips);
        collectReq.detail.push_back(taskData);

        LOG_DEBUG << "task msg, CollectReq:" << logTars(collectReq) << endl;
        pHallServant->async_collectRecord(NULL, collectReq);
    }

    return 0;
}

//拉取玩家段位信息
int OuterFactoryImp::asyncGetUserSegmentInfo(const userinfo::UserSeasonInfoGetReq &req)
{
    auto pHallServant = getHallServantPrx(req.uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    pHallServant->async_QueryUserSeasonInfo(new AsyncUserSegmentInfoCallback(req.uid), req);
    return 0;
}

//更新玩家段位
int OuterFactoryImp::asyncUpdateUserSegmentInfo(const GetMatchInfoParam &reqGetMatchInfo, const string &reward, const userinfo::UserSeasonInfoUpdateReq &req)
{
    auto pHallServant = getHallServantPrx(req.uid);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    pHallServant->async_UpdateUserSeasonInfo(new AsyncUpdateUserSegmentInfoCallback(reqGetMatchInfo, reward), req);
    return 0;
}

////更新玩家经验
int OuterFactoryImp::asyncUpdateUserLevelExperience(const long lPlayerID, const int experience)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    userinfo::UpdateUserExperienceReq req;
    req.uid = lPlayerID;
    req.experience = experience;
    LOG_DEBUG << "asyncUpdateUserLevelExperience lPlayerID:" << lPlayerID << ", experience:" << experience << endl;
    pHallServant->async_UpdateUserLevelExperience(NULL, req);

   /* async2GetUserBasic(lPlayerID);*/
    return 0;
}

int OuterFactoryImp::asyncCreateRoomCost(const long lPlayerID, const int props_id, const int number)
{
    auto pHallServant = getHallServantPrx(lPlayerID);
    if (!pHallServant)
    {
        LOG_ERROR << "pHallServant is nullptr" << endl;
        return -1;
    }

    GoodsManager::DeleteGoodsReq deleteGoodsReq;
    deleteGoodsReq.uid = lPlayerID;
    deleteGoodsReq.goods.goodsID = props_id;
    deleteGoodsReq.goods.count = abs(number);
    LOG_DEBUG << "asyncCreateRoomCost lPlayerID:" << lPlayerID << ", props_id:" << props_id << ", number:" << number << endl;
    pHallServant->async_deleteGoodsFromPlayer(NULL, deleteGoodsReq);
    return 0;
}

//保存活动财富
int OuterFactoryImp::async2ActivitySaveMoney(long lPlayerID, long bet)
{
   /* auto pActivityServant = getActivityServantPrx(lPlayerID);
    if (!pActivityServant)
    {
        LOG_ERROR << "pActivityServant is nullptr" << endl;
        return -1;
    }

    goldpig::SaveMoneyReq req;
    req.uid = lPlayerID;
    req.num = bet;
    pActivityServant->async_saveMoney(NULL, req);*/
    return 0;
}

//日志入库
void OuterFactoryImp::asyncLog2DB(const long uid, const DaqiGame::TLog2DBReq &req)
{
    auto pLog2DBServant = getLog2DBServantPrx(uid);
    if (!pLog2DBServant)
    {
        LOG_ERROR << "pLog2DBServant is nullptr" << endl;
        return;
    }

    LOG_DEBUG<<"asyncLog2DB uid: "<< uid << endl;
    pLog2DBServant->async_log2db(NULL, req);
}

//上报房间统计信息到大厅
int OuterFactoryImp::asyncReportRoomTableStat(const hall::RoomTableStat &data)
{
    //多节点上报大厅
    auto proxyList = Application::getCommunicator()->getEndpoint(_sHallServantObj);
    for (auto proxy : proxyList)
    {
        auto sProxyObj = _sHallServantObj + "@" + proxy.toString();
        auto pHallServant = Application::getCommunicator()->stringToProxy<hall::HallServantPrx>(sProxyObj);
        if (!pHallServant)
        {
            LOG_DEBUG << "pHallServant is nullptr." << endl;
            continue;
        }

        pHallServant->async_reportRoomTableStat(NULL, data);
    }

    return 0;
}

//上报房间桌子信息到大厅
int OuterFactoryImp::asyncReportRoomTableInfo(const push::RoomTableInfo &data)
{
    auto pPushServant = getPushServerPrx(tars::hash<string>()(data.roomid));
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return -1;
    }

    pPushServant->async_reportRoomTableInfo(NULL, data);
    return 0;
}

int OuterFactoryImp::asyncDoRobotDecide(const int type, const GetMatchInfoParam &reqGetMatchInfo, const RoomSo::TRobotDecideReq &req)
{
    auto pAIServant = getAIServantPrx(type, reqGetMatchInfo.lPlayerID);
    if (!pAIServant)
    {
        LOG_ERROR << "pAIServant is nullptr" << endl;
        return -1;
    }

    pAIServant->async_doRobotDecide(new AsyncDoRobotDecide(reqGetMatchInfo), req);
    return 0;
}

int OuterFactoryImp::asyncDoAiDecide(const int type, const ai::TAIBrainDecideReq &req)
{
    auto pAIServant = getAIServantPrx(type, req.robotID);
    if (!pAIServant)
    {
        LOG_ERROR << "pAIServant is nullptr" << endl;
        return -1;
    }

    LOG_DEBUG << "asyncDoAiDecide. req:" << printTars(req) << endl;
    pAIServant->async_doAIDecide(NULL, req);
    return 0;
}

int OuterFactoryImp::asyncDoAiForward(const int type, const ai::TAIMsgTransmitReq &req)
{
    auto pAIServant = getAIServantPrx(type, req.uid);
    if (!pAIServant)
    {
        LOG_ERROR << "pAIServant is nullptr" << endl;
        return -1;
    }

    pAIServant->async_doAIMsgForward(NULL, req);
    return 0;
}

void OuterFactoryImp::getRobotList(vector<tars::Int64> &robotList)
{
    wbl::ReadLocker lock(m_rwlock);
    getConfigServantPrx()->ListRobot(robotList);
}

int OuterFactoryImp::asyncBroadcastBlind(push::E_Reward_Mold_ID id,  const RoomSo::TGAME_GameFinish &gameFinish, int iBlindLevel)
{
    config::BroadcastConfigResp cfgResp;
    auto iRet = g_app.getOuterFactoryPtr()->getConfigServantPrx()->getBroadcastConfig(cfgResp);
    if (iRet != 0)
    {
        ROLLLOG_ERROR << " error, notify failed!" << endl;
        return iRet;
    }
    auto iterCfg = cfgResp.data.find(id);
    if (iterCfg == cfgResp.data.end())
    {
        ROLLLOG_ERROR << " borad id err! id:" << id << endl;
        return -1;
    }

    for (auto &it : gameFinish.mapUserActInfo)
    {
        auto uid = it.second.uid;
        if(CPlayerMngSingleton::getInstance()->isRobot(uid))
            continue;

        auto user = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(uid);
        if (!user)
            continue;

        ROLLLOG_DEBUG << "change:" << it.second.change << " condtion:" << iterCfg->second.condtion 
                      <<  " level:" << iterCfg->second.level << " iBlindLevel:" << iBlindLevel << endl;

        if (it.second.change < iterCfg->second.condtion)
            continue;

        if (iBlindLevel < iterCfg->second.level)
            continue;

        push::RewardNotify notify;
        notify.id = id;
        notify.type = 1;//系统广播
        notify.data = iterCfg->second.content;

        notify.param.sNickName = user->getUInfo().sNickName;
        notify.param.sPlace = iBlindLevel;
        notify.param.intervalTime = iterCfg->second.intervalTime;
        notify.param.views = iterCfg->second.views;

        push::PropsChangeNotify props;
        props.iPropsID = 20000;
        props.lCurrCount = it.second.change;
        notify.param.sPrize.push_back(props);
        asyncBroadcast(notify);
        ROLLLOG_DEBUG << "notify : " << printTars(notify) << endl;
    }

    return iRet;
}

int OuterFactoryImp::asyncBroadcastMatch(push::E_Reward_Mold_ID id, long lPlayerID, int ranking, const TSNGTableConfInfo &tableInfo)
{
    int iRet = 0;
    config::BroadcastConfigResp cfgResp;
    iRet = g_app.getOuterFactoryPtr()->getConfigServantPrx()->getBroadcastConfig(cfgResp);
    if (iRet != 0)
    {
        ROLLLOG_ERROR << " error, notify failed!" << endl;
        return iRet;
    }
    auto iterCfg = cfgResp.data.find(id);
    if (iterCfg == cfgResp.data.end())
    {
        ROLLLOG_ERROR << " borad id err! id:" << id << endl;
        return -1;
    }

    if (ranking > iterCfg->second.condtion)
        return 1;

    auto it = tableInfo.mapReward.find(ranking);
    if (it == tableInfo.mapReward.end())
        return 1;

    if (CPlayerMngSingleton::getInstance()->isRobot(lPlayerID))
        return 1;

    auto user = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lPlayerID);
    if (!user)
        return 1;

    push::RewardNotify notify;
    notify.id = id;
    notify.type = 1;
    notify.data = iterCfg->second.content;

    notify.param.sNickName = user->getUInfo().sNickName;
    notify.param.sPlace = tableInfo.iBetId;
    notify.param.ranking = ranking;
    notify.param.intervalTime = iterCfg->second.intervalTime;
    notify.param.views = iterCfg->second.views;

    push::PropsChangeNotify props;
    props.iPropsID = 20000;
    props.lCurrCount = it->second;
    notify.param.sPrize.push_back(props);
    asyncBroadcast(notify);
    ROLLLOG_DEBUG << "notify : " << printTars(notify) << endl;

    return iRet;
}

int OuterFactoryImp::asyncBroadcast(const push::RewardNotify &notify)
{
    auto iRet = 0;
    auto pPushServant = getPushServerPrx(rand() % 2);
    if (!pPushServant)
    {
        LOG_ERROR << "pPushServant is nullptr" << endl;
        return -1;
    }

    pPushServant->async_pushBroadcast(NULL, notify);

    // {
    //     DaqiGame::TLog2DBReq tLog2DBReq;
    //     tLog2DBReq.sLogType = 36;

    //     vector<string> veclog;
    //     veclog.push_back("1");
    //     veclog.push_back(notify.data);
    //     veclog.push_back("ysx");
    //     veclog.push_back("1");
    //     veclog.push_back("");
    //     veclog.push_back("");

    //     tLog2DBReq.vecLogData.push_back(veclog);
    //     asyncLog2DB(rand() % 2, tLog2DBReq);
    // }

    return iRet;
}

void OuterFactoryImp::asyncReportBoxProgressRate(const RoomSo::TGAME_GameFinish &req, int iBlindLevel, long lBigBlind)
{
    for (auto it = req.mapUserActInfo.begin(); it != req.mapUserActInfo.end(); ++it)
    {
        auto uid = it->second.uid;
        if(CPlayerMngSingleton::getInstance()->isRobot(uid))
        {
            continue;
        }

        auto pActivityServant = getActivityServantPrx(uid);
        if (!pActivityServant)
        {
            LOG_ERROR << "pActivityServant is nullptr" << endl;
            continue;
        }

        if (it->second.change <= 0)
        {
            continue;
        }

        int addition = 100;
        int round = 0;
        auto playerPtr = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(uid);
        if (playerPtr)
            round = playerPtr->getRound();
        if (round >= 8)
            addition += 50;
        if (round >= 20)
            addition += 50;
        if (round >= 35)
            addition += 50;
        if (round >= 55)
            addition += 50;

        //经典场房间赢得筹码数量
        Box::BoxProgressRateReq boxReq;
        boxReq.uid = uid;
        boxReq.betId = iBlindLevel;
        boxReq.progressRate = it->second.change * addition / std::max<long>(lBigBlind, 1);

        LOG_DEBUG << "uid:" << uid 
        << ", iBlindLevel:" << iBlindLevel 
        << ", lBigBlind:" << lBigBlind 
        << ", addition:" << addition 
        << ", round:" << round 
        << ", it->second.change:" << it->second.change 
        << ", boxReq:" << logTars(boxReq) << endl;
        
        pActivityServant->async_reportBoxProgressRate(NULL, boxReq);
    }
}

//邮件通知比赛奖励
int OuterFactoryImp::asyncMailNotifyReward(long lUid, int iRank, int iMatchID, const map<int, long>& mapReward)
{
    mail::TSendMailReq tSendMailReq;
    tSendMailReq.uid = lUid;
    mail::TMailData &mailData = tSendMailReq.data;
    
    auto it = mapMatchName.find(iMatchID);
    if(it != mapMatchName.end())
    {
        mailData.title = it->second;

        auto vecMailTemplate = split(sMatchMailRewardContent, "&");
        LOG_DEBUG<<"sMatchMailRewardContent: "<< sMatchMailRewardContent<< ", size: "<< vecMailTemplate.size()<<endl;
        if(vecMailTemplate.size() == 4)
        {
            mailData.content = vecMailTemplate[0] + GetTimeFormat() + vecMailTemplate[1] + it->second + vecMailTemplate[2] + I2S(iRank) + vecMailTemplate[3];
        }
    }

    bool has_reward = false;
    vector<mail::TMailDataItem> &awards = mailData.awards;
    for(auto item : mapReward)
    {
        if(!has_reward && item.second != 0)
        {
            has_reward = true;
        }
        mail::TMailDataItem rewardItem;
        rewardItem.goodsID = item.first;
        rewardItem.count = item.second;
        awards.push_back(rewardItem);
    }
    mailData.type = mail::E_MAIL_TYPE_USER;//个人邮件

    if(has_reward)
    {
        mail::TSendMailResp tSendMailResp;
        int iRet = getHallServantPrx(lUid)->sendMailToUserFromSystem(tSendMailReq, tSendMailResp);
        if(iRet != 0)
        {
            LOG_ERROR<< "send mail err. lUid: "<< lUid<< ", iRank: "<< iRank << endl;
            return iRet;
        }
    }
    return 0;
}

//邮件通知退赛
int OuterFactoryImp::asyncMailNotifyQuit(long lUid, int iMatchID, const map<int, long>& mapReward)
{
    mail::TSendMailReq tSendMailReq;
    tSendMailReq.uid = lUid;
    mail::TMailData &mailData = tSendMailReq.data;
    
    LOG_DEBUG<<"lUid: "<< lUid << ", iMatchID: "<< iMatchID << endl;
    auto it = mapMatchName.find(iMatchID);
    if(it != mapMatchName.end())
    {
        mailData.title = it->second;

        auto vecMailTemplate = split(sMatchMailQuitContent, "&");
        if(vecMailTemplate.size() == 2)
        {
            mailData.content = vecMailTemplate[0] + it->second + vecMailTemplate[1];
        }
    }

    bool has_reward = false;
    vector<mail::TMailDataItem> &awards = mailData.awards;
    for(auto item : mapReward)
    {
        if(!has_reward && item.second != 0)
        {
            has_reward = true;
        }
        mail::TMailDataItem rewardItem;
        rewardItem.goodsID = item.first;
        rewardItem.count = item.second;
        awards.push_back(rewardItem);
    }
    mailData.type = mail::E_MAIL_TYPE_USER;//个人邮件

    if(has_reward)
    {
        mail::TSendMailResp tSendMailResp;
        int iRet = getHallServantPrx(lUid)->sendMailToUserFromSystem(tSendMailReq, tSendMailResp);
        if(iRet != 0)
        {
            LOG_ERROR<< "send mail err. lUid: "<< lUid << endl;
            return iRet;
        }
    }
    return 0;
}

void OuterFactoryImp::loadAlgConfig()
{
    algConfigPath = (*_pFileConf).get("/Main/algConfig<path>", "./");
    ROLLLOG_DEBUG << "algConfigPath: " << algConfigPath << endl;

    aiPlatformOpen = S2I((*_pFileConf).get("/Main/aiPlatform<open>", "0"));
    aiPlatformToken = (*_pFileConf).get("/Main/aiPlatform<token>", "654321");
    aiPlatformAddr = (*_pFileConf).get("/Main/aiPlatform<host>", "127.0.0.1");
    aiPlatformPort = S2I((*_pFileConf).get("/Main/aiPlatform<port>", "10000"));

    //seatnum
    string s_seat_num=  (*_pFileConf).get("/Main/SeatNumber<num>", "2:6:9");
    for(auto seat_num : split(s_seat_num, ":"))
    {
        if(seat_num.empty())
        {
            continue;
        }
        vSeatNum.push_back(S2I(seat_num));
    }

    ROLLLOG_DEBUG << "algConfigPath: " << algConfigPath << endl;
    ROLLLOG_DEBUG << "aiPlatformOpen: " << aiPlatformOpen << endl;
    ROLLLOG_DEBUG << "aiPlatformToken: " << aiPlatformToken << endl;
    ROLLLOG_DEBUG << "aiPlatformAddr: " << aiPlatformAddr << endl;
    ROLLLOG_DEBUG << "aiPlatformPort: " << aiPlatformPort << endl;
    ROLLLOG_DEBUG << "vSeatNum size: " << vSeatNum.size() << endl;
}

//
std::string OuterFactoryImp::getAlgConfigPath()
{
    return algConfigPath;
}

int OuterFactoryImp::getAiPlatformOpen()
{
    return aiPlatformOpen;
}

//
std::string OuterFactoryImp::getAiPlatformToken()
{
    return aiPlatformToken;
}

//
std::string OuterFactoryImp::getAiPlatformAddr()
{
    return aiPlatformAddr;
}

//
std::vector<int>& OuterFactoryImp::getSeatNumber()
{
    return vSeatNum;
}

//
int OuterFactoryImp::getAiPlatformPort()
{
    return aiPlatformPort;
}
