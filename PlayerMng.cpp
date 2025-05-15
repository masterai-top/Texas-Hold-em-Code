#include "PlayerMng.h"

/**
 *
*/
CPlayerMng::CPlayerMng()
{

}

/**
 *
*/
CPlayerMng::~CPlayerMng()
{

}

//插入用户在线map，即在Room创建一个用户玩家
int CPlayerMng::insertPlayer2UMap(const long lPlayerID)
{
    if (isExistPlayerMap(lPlayerID))
        return -1;

    auto pPlayerPtr = new CPlayer(lPlayerID);
    if (!pPlayerPtr)
        return -2;

    m_mapCPlayer.insert(pair<long, CPlayerPtr>(lPlayerID, pPlayerPtr));
    if (!isExistPlayerMap(lPlayerID))
        return -3;

    // TODO(add): 调用UserInfo服务获取玩家个人信息和财富信息
    // 直接调用，建议单列函数实现
    //OuterFactorySingleton::getInstance()->async2GetUInfo(lPlayerID);
    //OuterFactorySingleton::getInstance()->async2GetUAccount(lPlayerID);

    LOG_DEBUG << lPlayerID << "|" << "insert user to palyer map success, uid: " << lPlayerID << endl;
    return 0;
}

//玩家与桌子的映射关系
int CPlayerMng::insertTable2UTMap(const long lPlayerID, CTablePtr pTablePtr)
{
    if (!pTablePtr)
    {
        return -1;
    }

    //已经存在
    if (isExistU2TableMap(lPlayerID))
    {
        LOG_DEBUG << " user is exist01. lPlayerID:" << lPlayerID << ", iTableID:" << pTablePtr->getTableID() << endl;
        return 0;
    }

    //加入桌子
    m_mapU2Talbe.insert(pair<long, CTablePtr>(lPlayerID, pTablePtr));

    //不存在
    if (!isExistU2TableMap(lPlayerID))
    {
        LOG_DEBUG << " user is exist02. lPlayerID:" << lPlayerID << ", iTableID:" << pTablePtr->getTableID() << endl;
        return -3;
    }

    //进入房间人数加一
    pTablePtr->incEnterRoomUsers(lPlayerID);

#if 0
    if (!isUReport(lPlayerID) && !isRobot(lPlayerID))
    {
        //进入房间上报
        std::string sRoomID = getRoomIDFromUMap(lPlayerID);
        std::string sRoomKey = pTablePtr->getRoomKey();
        userservermap::EnterRoomReq req;
        req.uid = lPlayerID;
        req.roomId = sRoomID;
        req.roomKey = sRoomKey;
        OuterFactorySingleton::getInstance()->async2UserServerMap(req);

        ///
        updateUReportByUID(lPlayerID, true);
    }
#endif

    return 0;
}

int CPlayerMng::insertUInfo2CPlayer(const long lPlayerID, const UInfo &stUInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        LOG_ERROR << lPlayerID << "|" << "insert user info to playerObj error, uid: " << lPlayerID << endl;
        return -1;
    }

    pPlayerPtr->setUInfo(stUInfo);
    LOG_DEBUG << lPlayerID << "|" << "update user info to palyer map success. nickname:" << stUInfo.sNickName << endl;
    return 0;
}

int CPlayerMng::insertUSegment2CPlayer(const long lPlayerID, const USegmentInfo &stUSegmentInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        LOG_ERROR << lPlayerID << "|" << "insert user info to playerObj error, uid: " << lPlayerID << endl;
        return -1;
    }

    pPlayerPtr->setUSegmentInfo(stUSegmentInfo);
    return 0;
}

int CPlayerMng::insertUWInfo2CPlayer(const long lPlayerID, const UWealthInfo &stUWealthInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        LOG_ERROR << lPlayerID << "|" << "insert user wealth info to playerObj error, uid: " << lPlayerID << endl;
        return -1;
    }

    pPlayerPtr->setUWealthInfo(stUWealthInfo);
    return 0;
}

//插入玩家账户信息
int CPlayerMng::insertUAInfo2CPlayer(const long lPlayerID, const UAccountInfo &stUAccountInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        LOG_ERROR << lPlayerID << "|" << "insert user account info to playerObj error, uid: " << lPlayerID << endl;
        return -1;
    }

    pPlayerPtr->setUAccountInfo(stUAccountInfo);
    return 0;
}

// TODO(modifiy): 可能有待修改
int CPlayerMng::insertTChairID2CTable(const long lPlayerID)
{
    //
    if (!isExistPlayerMap(lPlayerID))
    {
        return CHAIRID_ERR;
    }

    //
    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return CHAIRID_ERR;
    }

    //分配椅子
    int iChairID = pTablePtr->sitTable(lPlayerID);
    if (iChairID != CHAIRID_ERR)
    {
        //更新用户状态信息
        updateUStateByUID(lPlayerID, E_PLAYER_SIT_TABLE);
    }

    //
    LOG_DEBUG << lPlayerID << "|" << "allocate user chairid from table, chairid : " << iChairID << endl;
    return iChairID;
}

int CPlayerMng::insertTChairID2CTable(const long lPlayerID, int iChairID)
{
    if (!isExistPlayerMap(lPlayerID))
    {
        return CHAIRID_ERR;
    }

    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return CHAIRID_ERR;
    }

    // 分配椅子
    int iRet = pTablePtr->sitTable(lPlayerID, iChairID);
    if (iRet != 0)
    {
        return CHAIRID_ERR;
    }

    //更新用户状态信息
    updateUStateByUID(lPlayerID, E_PLAYER_SIT_TABLE);

    LOG_DEBUG << lPlayerID << "|" << "allocate user chairid from table success, chairid : " << iChairID << ", tableid : " << pTablePtr->getTableID() << endl;
    return iChairID;
}

// 查询接口
CPlayerPtr CPlayerMng::findPlayerPtrByUID(const long lPlayerID) const
{
    if (!isExistPlayerMap(lPlayerID))
    {
        LOG_DEBUG << lPlayerID << "|" << "user not exist user map" << endl;
        return NULL;
    }

    auto iter = m_mapCPlayer.find(lPlayerID);
    if (iter == m_mapCPlayer.end())
    {
        return NULL;
    }

    if (!(iter->second))
    {
        LOG_DEBUG << lPlayerID << "|" << "find playerPtr by uid error" << endl;
    }

    return iter->second;
}

CTablePtr CPlayerMng::findTablePtrByUID(const long lPlayerID) const
{
    if (!isExistU2TableMap(lPlayerID))
    {
        LOG_DEBUG << lPlayerID << "|" << "user not exist user2Table map." << endl;
        return NULL;
    }

    auto iter = m_mapU2Talbe.find(lPlayerID);
    if (iter == m_mapU2Talbe.end())
    {
        LOG_DEBUG << lPlayerID << "|" << "user not exist user2Table map." << endl;
        return NULL;
    }

    if (!(iter->second))
    {
        LOG_DEBUG << lPlayerID << "|" << "find tablePtr by uid error." << endl;
    }

    return iter->second;
}

// TODO(modifiy)
int CPlayerMng::findCIDByUID(const long lPlayerID) const
{
    if (!isExistPlayerMap(lPlayerID))
    {
        return CHAIRID_ERR;
    }

    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return CHAIRID_ERR;
    }

    int iChairID = pTablePtr->findCIDByUID(lPlayerID);
    return iChairID;
}

int CPlayerMng::findTIDByUID(const long lPlayerID) const
{
    if (!isExistPlayerMap(lPlayerID))
    {
        return TABLEID_ERR;
    }

    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return TABLEID_ERR;
    }

    int iTableID = pTablePtr->getTableID();
    return iTableID;
}

int CPlayerMng::findMIDByUID(const long lPlayerID) const
{
    if (!isExistPlayerMap(lPlayerID))
    {
        return 0;
    }

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return 0;
    }

    KOPlayerInfo stKOPlayerInfo = pPlayerPtr->getKOPlayerInfo();
    return stKOPlayerInfo.iMatchID;
}

std::string CPlayerMng::findRoomKeyByUID(const long lPlayerID) const
{
    std::string sRoomKey = "";

    if (!isExistPlayerMap(lPlayerID))
    {
        return sRoomKey;
    }

    if (!isExistU2TableMap(lPlayerID))
    {
        return sRoomKey;
    }

    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return sRoomKey;
    }

    sRoomKey = pTablePtr->getRoomKey();
    return sRoomKey;
}

Eum_UState CPlayerMng::getUStateFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return E_PLAYER_STATE_ERR;
    }

    Eum_UState eUState = pPlayerPtr->getPlayerState();
    return eUState;
}

Eum_Action CPlayerMng::getAIActionFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return E_NO_ACTION;
    }

    Eum_Action eUState = pPlayerPtr->getAIAction();
    return eUState;
}

std::string CPlayerMng::getUIPFromUMap(const long lPlayerID) const
{
    std::string sPlayerIP = "";

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        sPlayerIP = pPlayerPtr->getPlayerIP();
    }

    if (sPlayerIP == "")
    {
        LOG_ERROR << lPlayerID << "|" << "get user ip from userMap error" << endl;
    }

    return sPlayerIP;
}

int CPlayerMng::getURoomCardFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    int iRoomCard = pPlayerPtr->getURoomCard();
    return iRoomCard;
}

long CPlayerMng::getUGoldCoinFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    long lGoldCoin = pPlayerPtr->getUGoldCoin();
    return lGoldCoin;
}

long CPlayerMng::getUPointFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    long iPoint =  pPlayerPtr->getUPoint();
    return iPoint;
}

//通过玩家ID获取玩家钻石
int CPlayerMng::getUDiamondFromUMap(const long lPlayerID) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    int iDiamond =  pPlayerPtr->getUDiamond();
    return iDiamond;
}

//通过玩家ID获取玩家游戏状态
int CPlayerMng::getUGameStatus(const long lPlayerID) const
{
    /* CTablePtr pTablePtr = findTablePtrByUID(lPlayerID);
     if (pTablePtr)
     {
         TableInfo stTableInfo = pTablePtr->getTableInfo();
         if (stTableInfo.vecWatchList.end() != std::find_if (stTableInfo.vecWatchList.begin(), stTableInfo.vecWatchList.end(), [lPlayerID](long uid)->bool{
             return uid == lPlayerID;
         })) return 1; //观战
         else return 2;//重连
     }*/
    return 0;
}

int CPlayerMng::getUWealthFromUMap(const long lPlayerID, UWealthInfo &stUWealthInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUWealthInfo = pPlayerPtr->getWealthInfo();
    return 0;
}

int CPlayerMng::getUAccountFromUMap(const long lPlayerID, UAccountInfo &stUAccountInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUAccountInfo = pPlayerPtr->getAccountInfo();
    return 0;
}

int CPlayerMng::getUInfoFromUMap(const long lPlayerID, UInfo &stUInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUInfo = pPlayerPtr->getUInfo();
    return 0;
}

int CPlayerMng::getUHInfoFromUMap(const long lPlayerID, HeadInfo &stHeadInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stHeadInfo = pPlayerPtr->getUHeadInfo();
    return 0;
}

int CPlayerMng::getUSegmentInfoFromUMap(const long lPlayerID, USegmentInfo &stUSegmentInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUSegmentInfo = pPlayerPtr->getUSegmentInfo();
    return 0;
}

int CPlayerMng::getUPropsInfoFromUMap(const long lPlayerID,  map<int, map<int, UPropsInfo>> &mUPropsInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    mUPropsInfo = pPlayerPtr->getPropsInfo();
    return 0;
}

int CPlayerMng::getUCalInfoByUID(const long lPlayerID, GameCalInfo &stUGameCalInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }
    stUGameCalInfo = pPlayerPtr->getUGameCalInfo();
    return 0;
}

int CPlayerMng::updateUCalInfoByUID(const long lPlayerID, long roundID, GameCalInfo &stUGameCalInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }
    pPlayerPtr->updateUGameCalInfo(stUGameCalInfo, roundID);
    return 0;
}

void CPlayerMng::addPlayerAddr(const long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return;
    }

    auto stHeadInfo = pPlayerPtr->getUHeadInfo();

    auto it = m_mapCPlayerAddr.find(lPlayerID);
    if(it == m_mapCPlayerAddr.end())
    {
        m_mapCPlayerAddr.insert(std::make_pair(lPlayerID, stHeadInfo.sServantAddr));
    }
    else
    {
        m_mapCPlayerAddr[lPlayerID] = stHeadInfo.sServantAddr;
    }
    LOG_DEBUG << "m_mapCPlayerAddr size: " << m_mapCPlayerAddr.size() << ", lPlayerID:" << lPlayerID << ", addr:" << stHeadInfo.sServantAddr << endl;
}

string CPlayerMng::getPlayerAddr(const long lPlayerID)
{
    auto it = m_mapCPlayerAddr.find(lPlayerID);
    if(it != m_mapCPlayerAddr.end())
    {
        return it->second;
    }
    LOG_DEBUG << "m_mapCPlayerAddr size: " << m_mapCPlayerAddr.size() << ", lPlayerID: " << lPlayerID << endl;
    return "";
}

void CPlayerMng::clearPlayerAddr()
{
    m_mapCPlayerAddr.clear();
}

void CPlayerMng::delPlayerAddr(const long lPlayerID)
{
    auto it = m_mapCPlayerAddr.find(lPlayerID);
    if(it != m_mapCPlayerAddr.end())
    {
        m_mapCPlayerAddr.erase(it);
    }
}

//获取快速开始用户排队信息
int CPlayerMng::getQSUQueue(const long lPlayerID, /**out**/UQueueInfo &stUQueueInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUQueueInfo = pPlayerPtr->getUQueue();
    return 0;
}

int CPlayerMng::getUClubRoom(const long lPlayerID, /**out**/UClubRoom &stUClubRoom) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUClubRoom = pPlayerPtr->getUClubRoom();
    return 0;
}

int CPlayerMng::getUGPS(const long lPlayerID, /**out**/UGPS &stUGPS) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stUGPS = pPlayerPtr->getUGPS();
    return 0;
}

int CPlayerMng::getURobot(const long lPlayerID, /**out**/URobot &stURobot) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stURobot = pPlayerPtr->getURobot();
    return 0;
}

int CPlayerMng::getUFromUMap(const long lPlayerID, PlayerInfo &stPlayerInfo) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    stPlayerInfo = pPlayerPtr->getPlayerInfo();
    return 0;
}

//获取账户信息
int CPlayerMng::getUBaseInfoExtFromUMap(const long lPlayerID, /**out**/UserBaseInfoExt &m_stUserBaseInfoExt) const
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    m_stUserBaseInfoExt = pPlayerPtr->getUserBaseInfoExt();
    return 0;
}

Eum_Game_Mode CPlayerMng::getUModeFromU2TMap(const long lPlayerID) const
{
    Eum_Game_Mode eGameMode = E_MODE_START;
    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        return eGameMode;
    }

    std::string sRoomID = pTablePtr->getRoomID();
    eGameMode = OuterFactorySingleton::getInstance()->parseGameMode(sRoomID);
    return eGameMode;
}

std::string CPlayerMng::getRoomIDFromU2TMap(const long lPlayerID) const
{
    std::string sRoomID = "";
    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (pTablePtr)
    {
        sRoomID = pTablePtr->getRoomID();
    }

    if (sRoomID == "")
    {
        LOG_ERROR << lPlayerID << "|" << "find roomid by uid, from user2Table map" << endl;
    }

    return sRoomID;
}

std::string CPlayerMng::getRoomIDFromUMap(const long lPlayerID) const
{
    std::string sRoomID = "";

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        sRoomID = pPlayerPtr->getURoomID();
    }

    if (sRoomID == "")
    {
        LOG_ERROR << lPlayerID << "|" << "find roomid by uid, from user map." << endl;
    }

    return sRoomID;
}

bool CPlayerMng::isRobot(const long lPlayerID) const
{
    URobot stURobot;
    getURobot(lPlayerID, stURobot);
    return stURobot.bRobot;
}

//用户是否上报
bool CPlayerMng::isUReport(const long lPlayerID) const
{
    bool bIsReport = false;
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bIsReport = pPlayerPtr->isUReport();
    }

    return bIsReport;
}

bool CPlayerMng::isUNoState(const long lPlayerID) const
{
    bool bUNoState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUNoState = pPlayerPtr->isUNoState();
    }

    if (E_PLAYER_STATE_ERR == bUNoState)
    {
        LOG_ERROR << lPlayerID << "is user no State error" << endl;
    }

    return bUNoState;
}

bool CPlayerMng::isULoginSate(const long lPlayerID) const
{
    bool bULoginState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bULoginState = pPlayerPtr->isULoginSate();
    }

    if (E_PLAYER_STATE_ERR == bULoginState)
    {
        LOG_ERROR << lPlayerID << "is user login State error" << endl;
    }

    return bULoginState;
}

bool CPlayerMng::isUEterRoomState(const long lPlayerID) const
{
    bool bUEnterRoomState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUEnterRoomState = pPlayerPtr->isUEterRoomState();
    }

    if (E_PLAYER_STATE_ERR == bUEnterRoomState)
    {
        LOG_ERROR << lPlayerID << "is user enter room State error" << endl;
    }

    return bUEnterRoomState;
}

bool CPlayerMng::isUSitTableState(const long lPlayerID) const
{
    bool bUSitTableState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUSitTableState = pPlayerPtr->isUSitTableState();
    }

    if (E_PLAYER_STATE_ERR == bUSitTableState)
    {
        LOG_ERROR << lPlayerID << "is user sit table State error" << endl;
    }

    return bUSitTableState;
}

bool CPlayerMng::isUAgreedState(const long lPlayerID) const
{
    bool bUAgreedState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUAgreedState = pPlayerPtr->isUAgreedState();
    }

    if (E_PLAYER_STATE_ERR == bUAgreedState)
    {
        LOG_ERROR << lPlayerID << "is user sit table State error" << endl;
    }

    return bUAgreedState;
}

bool CPlayerMng::isUPlayingState(const long lPlayerID) const
{
    bool bUPlayingState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUPlayingState = pPlayerPtr->isUPlayingState();
    }

    if (E_PLAYER_STATE_ERR == bUPlayingState)
    {
        LOG_ERROR << lPlayerID << "is user game playing State error" << endl;
    }

    return bUPlayingState;
}

bool CPlayerMng::isUOfflineState(const long lPlayerID) const
{
    bool bUOfflineState = E_PLAYER_STATE_ERR;

    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        bUOfflineState = pPlayerPtr->isUOfflineState();
    }

    if (E_PLAYER_STATE_ERR == bUOfflineState)
    {
        LOG_ERROR << lPlayerID << "is user game playing State error" << endl;
    }

    return bUOfflineState;
}

void CPlayerMng::eraseUserActiveTime(const long lPlayerID)
{
    if(isRobot(lPlayerID))
    {
        return;
    }
    auto iter = m_mapUActiveTime.find(lPlayerID);
    if(iter != m_mapUActiveTime.end())
    {
        m_mapUActiveTime.erase(iter);
    }
}

void CPlayerMng::updateUserActiveTime(const long lPlayerID)
{
    if(isRobot(lPlayerID))
    {
        return;
    }
    if(isExistPlayerMap(lPlayerID))
    {
        auto iter = m_mapUActiveTime.find(lPlayerID);
        if(iter == m_mapUActiveTime.end())
        {
            m_mapUActiveTime.insert(std::make_pair(lPlayerID, time(NULL)));
        }
        else
        {
            iter->second = time(NULL);
        }
    }
}

bool CPlayerMng::isExistPlayerMap(const long lPlayerID) const
{
    auto iter = m_mapCPlayer.find(lPlayerID);
    if (iter != m_mapCPlayer.end())
        return true;

    return false;
}

//清理玩家
void CPlayerMng::clearAllPlayerMap()
{
    for (auto iter = m_mapCPlayer.begin(); iter != m_mapCPlayer.end();)
    {
        if(isRobot(iter->first))
            continue;
        m_mapCPlayer.erase(iter++);
    }
}

//玩家所在桌子的档位
int CPlayerMng::getUTableBlindLevel(const long lPlayerID, int roomType) const
{
    auto iter = m_mapU2Talbe.find(lPlayerID);
    if (iter != m_mapU2Talbe.end())
    {
        return iter->second->getRTableInfo().iBlindLevel > 0 ? iter->second->getRTableInfo().iBlindLevel : iter->second->getGTableInfo().iBlindLevel;
        /*if(roomType == 1 || roomType == 8)
        {
            return iter->second->getRTableInfo().iBlindLevel;
        }
        else if(roomType == 3)
        {
            return iter->second->getGTableInfo().iBlindLevel;
        }*/
    }

    return 0;
}

//玩家所在桌子椅子数
int CPlayerMng::getUTableMaxSeat(const long lPlayerID, int roomType) const
{
    auto iter = m_mapU2Talbe.find(lPlayerID);
    if (iter != m_mapU2Talbe.end())
    {
        return iter->second->getRTableInfo().iMaxSeatCount > 0 ? iter->second->getRTableInfo().iMaxSeatCount : iter->second->getTableInfo().iMaxSeatCount;
       /* if(roomType == 1 || roomType == 8)
        {
            return iter->second->getRTableInfo().iMaxSeatCount;
        }
        else if(roomType == 3)
        {
            return iter->second->getTableInfo().iMaxSeatCount;
        }*/
    }

    return 0;
}

//用户是否在U2Tmap中
bool CPlayerMng::isExistU2TableMap(const long lPlayerID) const
{
    auto iter = m_mapU2Talbe.find(lPlayerID);
    if (iter != m_mapU2Talbe.end())
        return true;

    return false;
}

/*
//通过桌子ID获取进入该桌的用户数量
int CPlayerMng::getUEnterCountByTID(const int iTableID) const
{
    int iEnterCount = 0;
    auto iter = m_mapU2Talbe.begin();
    for (; iter != m_mapU2Talbe.end(); ++iter)
    {
        if (iter->second != (long)NULL && iter->second->getTableID() == iTableID)
        {
            ++iEnterCount;
        }
    }

    return iEnterCount;
}

//通过桌子ID获取进入该桌的用户列表
vector<long> CPlayerMng::getUEnterListByTID(const int iTableID) const
{
    vector<long> vecUList;
    auto iter = m_mapU2Talbe.begin();
    for (; iter != m_mapU2Talbe.end(); ++iter)
    {
        if (iter->second != (long)NULL && iter->second->getTableID() == iTableID)
        {
            vecUList.push_back(iter->first);
        }
    }

    return vecUList;
}
*/

//通过Roomkey获取进入该桌的用户列表
vector<long> CPlayerMng::getUEnterListByKey(const std::string sRoomKey) const
{
    vector<long> vecUList;
    for (auto iter = m_mapU2Talbe.begin(); iter != m_mapU2Talbe.end(); ++iter)
    {
        if (iter->second && iter->second->getRoomKey() == sRoomKey)
        {
            vecUList.push_back(iter->first);
        }
    }

    return vecUList;
}

int CPlayerMng::getURobotListByRommID(const std::string sRoomID, /**out**/vector<long> &stRobotList) const
{
    for (const auto &umap : m_mapCPlayer)
    {
        URobot stURobot;
        getURobot(umap.first, stURobot);
        if (stURobot.bRobot && getRoomIDFromUMap(umap.first) == sRoomID)
        {
            stRobotList.push_back(umap.first);
        }
    }

    return 0;
}

//更新玩家上报
int CPlayerMng::updateUReportByUID(const long lPlayerID, const bool bIsReport)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    pPlayerPtr->setReport(bIsReport);
    return 0;
}

// 修改接口
int CPlayerMng::updateUStateByUID(const long lPlayerID, Eum_UState eUState)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    //for watch game
    auto eUCurrState = pPlayerPtr->getPlayerState();
    if (eUCurrState == E_PLAYER_WATCH)
    {
        if (eUState != E_PLAYER_OFFLIEN
                && eUState != E_PLAYER_NO_STATE
                && eUState != E_PLAYER_STATE_ERR
                && eUState != E_PLAYER_END)
        {
            return 0;
        }
    }

    pPlayerPtr->setPlayerState(eUState);
    return 0;
}

int CPlayerMng::updateAIActionByUID(const long lPlayerID, Eum_Action eAction)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }
    if(!isRobot(lPlayerID))
    {
        return RETCODE_ERR;
    }
   
    pPlayerPtr->setAIAction(eAction);
    return 0;
}

int CPlayerMng::updateURoomCardByUID(const long lPlayerID, const long iChangeValue)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateURoomCard(iChangeValue);
    return 0;
}

int CPlayerMng::updateMsgHeadByUID(const long lPlayerID, const HeadInfo &stHeadInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateMsgHead(stHeadInfo);
    return 0;
}

//更新用户账户信息
int CPlayerMng::updateUserBaseInfoExtByUid(const long lPlayerID, const JFGame::UserBaseInfoExt &stUserBaseInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateBaseInfoExt(stUserBaseInfo);
    return 0;
}

//更新快速开始用户排队信息
int CPlayerMng::updateQSUQueueByUID(const long lPlayerID, const UQueueInfo &stUQueueInfo)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateUQueue(stUQueueInfo);
    return 0;
}

int CPlayerMng::updateUClubRoomByUID(const long lPlayerID, const UClubRoom &stUClubRoom)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateUClubRoom(stUClubRoom);
    return 0;
}

int CPlayerMng::updateUGPSByUID(const long lPlayerID, const UGPS &stUGPS)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateUGPS(stUGPS);
    return 0;
}

int CPlayerMng::updataURobotByUID(const long lPlayerID, const URobot &stURobot)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateURobot(stURobot);
    return 0;
}

int CPlayerMng::updataUAchievementInfoByUID(const long lPlayerID, bool show)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateUAchievementInfo(show);
    return 0;
}

int CPlayerMng::updataULeagueInfoByUID(const long lPlayerID, int level)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updataULeagueInfoByUID(level);
    return 0;
}


int CPlayerMng::updateUGoldCoinByUID(const long lPlayerID, const long iChangeValue)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
        return RETCODE_ERR;

    pPlayerPtr->updateUGoldCoin(iChangeValue);
    return 0;
}

int CPlayerMng::updateUPointByUID(const long lPlayerID, const long iChangeValue)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    pPlayerPtr->updateUPoint(iChangeValue);
    return 0;
}

void CPlayerMng::setUBlindLevel(long lPlayerID, int level)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return ;
    }

    pPlayerPtr->setUBlindLevel(level);
    return ;
}

void CPlayerMng::setUMaxSeat(long lPlayerID, int seatnum)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return ;
    }

    pPlayerPtr->setUMaxSeat(seatnum);
    return ;
}

int CPlayerMng::getUBlindLevel(long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    return pPlayerPtr->getUBlindLevel();
}

int CPlayerMng::getUMaxSeat(long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    return pPlayerPtr->getUMaxSeat();
}

void CPlayerMng::setUpdateGoldFlag(long lPlayerID, bool flag)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return ;
    }

    pPlayerPtr->setUpdateGoldFlag(flag);
    return ;
}

bool CPlayerMng::getUpdateGoldFlag(long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    return pPlayerPtr->getUpdateGoldFlag();
}

bool CPlayerMng::getUAchievementInfoByUID(long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    return pPlayerPtr->getUAchievementInfo();
}

int CPlayerMng::getULeagueInfoByUID(long lPlayerID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    return pPlayerPtr->getULeagueInfo();
}


int CPlayerMng::updateURoomIDByUID(const long lPlayerID, const string sRoomID)
{
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (!pPlayerPtr)
    {
        return RETCODE_ERR;
    }

    pPlayerPtr->updateURoomID(sRoomID);
    return 0;
}

//从在线map移除用户
int CPlayerMng::eraseUser4Umap(long lPlayerID)
{
    if (!isExistPlayerMap(lPlayerID))
        return 0;

    if(isRobot(lPlayerID))
        return 0;
    //清理
    m_mapCPlayer.erase(lPlayerID);

    if (isExistPlayerMap(lPlayerID))
    {
        return RETCODE_ERR;
    }

    return 0;
}

// 删除接口 @注意在删除玩家信息时的顺序
int CPlayerMng::erasePlayerFromUMap(long lPlayerID)
{
    //无用户
    if (!isExistPlayerMap(lPlayerID))
    {
        LOG_DEBUG << "not exist user, uid: " << lPlayerID << endl;
        return 0;
    }

    //删除玩家桌子映射关系
    int iRet = erasePlayerFromU2TMap(lPlayerID);
    if (iRet != 0)
    {
        LOG_DEBUG << "erase user from table err, uid: " << lPlayerID << ", iRet : " << iRet << endl;
        return -1;
    }

    //
    LOG_DEBUG << "erase user, uid: " << lPlayerID << endl;
    //
    if(isRobot(lPlayerID))
        return 0;
    m_mapCPlayer.erase(lPlayerID);

    if (isExistPlayerMap(lPlayerID))
    {
        LOG_DEBUG << "user exist err, uid: " << lPlayerID << endl;
        return -2;
    }

    return 0;
}

//删除玩家桌子映射关系
int CPlayerMng::erasePlayerFromU2TMap(long lPlayerID, int iType)
{
    if (!isExistU2TableMap(lPlayerID))
    {
        LOG_DEBUG << "user not in table, uid: " << lPlayerID << endl;
        return 0;
    }

    int iRet = erasePlayerFormTable(lPlayerID, iType);
    if (iRet != 0)
    {
        LOG_DEBUG << "erase user from table err, uid: " << lPlayerID << ", iRet : " << iRet << endl;
        return -1;
    }

    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (pTablePtr)
    {
        pTablePtr->decEnterRoomUsers(lPlayerID);
    }

    LOG_DEBUG << "erase user from table , uid: " << lPlayerID << endl;
    m_mapU2Talbe.erase(lPlayerID);

    if (isExistU2TableMap(lPlayerID))
    {
        LOG_DEBUG << "user in table, uid: " << lPlayerID << endl;
        return -2;
    }

    updateUStateByUID(lPlayerID, E_PLAYER_NO_STATE);
    OuterFactorySingleton::getInstance()->asyncRequest2UserStateOffGame(lPlayerID);
    return 0;
}

// TODO
int CPlayerMng::erasePlayerFormTable(long lPlayerID, int iType)
{
    auto pTablePtr = findTablePtrByUID(lPlayerID);
    if (!pTablePtr)
    {
        LOG_DEBUG << "find table ptr empty, uid: " << lPlayerID << endl;
        return -1;
    }

    int iRet = pTablePtr->leftTable(lPlayerID, iType);
    if (iRet != 0)
    {
        LOG_DEBUG << "left table err, uid: " << lPlayerID << ", iRet : " << iRet << endl;
        return -2;
    }

    //更新用户状态信息
    updateUStateByUID(lPlayerID, E_PLAYER_ENTER_ROOM);
    return 0;
}

void CPlayerMng::reportRetrieveRobot(const long lPlayerID)
{
    /*    if (!isRobot(lPlayerID))
            return;

        ai::TRetrieveRobotReq req;
        req.vecUid.push_back(lPlayerID);
        OuterFactorySingleton::getInstance()->async2RetrieveRobot(req);*/
}

void CPlayerMng::reportExitRoom4Coro(const long lPlayerID)
{
    if (isUReport(lPlayerID) && !isRobot(lPlayerID))
    {
        updateUReportByUID(lPlayerID, false);
    }
}

//显示玩家信息
void CPlayerMng::displayUser(const long lPlayerID, const std::string str) const
{
    if (!isExistPlayerMap(lPlayerID))
    {
        return;
    }

    ostringstream os;
    os << "\n" << str << ", display user info = { ";
    auto pPlayerPtr = findPlayerPtrByUID(lPlayerID);
    if (pPlayerPtr)
    {
        std::string sUInfo = pPlayerPtr->display();
        os << sUInfo;
    }

    int iChairID = CHAIRID_ERR;
    int iTableID = TABLEID_ERR;
    if (isExistU2TableMap(lPlayerID))
    {
        iChairID = findCIDByUID(lPlayerID);
        iTableID = findTIDByUID(lPlayerID);
    }

    os << " { " << "tableid:" << iTableID << ", chairid:" << iChairID << " }";
    os << " }";
    LOG_DEBUG << os.str() << endl;
}

//显示用户
void CPlayerMng::showUInfo(const string &params, string &result) const
{
    long lPlayerID = S2I(params);
    if (lPlayerID == 0)
    {
        unsigned int iUCnt = m_mapCPlayer.size();
        result = "users total, number: " + I2S(iUCnt);
    }
    else
    {
        if (isExistPlayerMap(lPlayerID))
        {
            PlayerInfo stPlayerInfo;
            getUFromUMap(lPlayerID, stPlayerInfo);
            bool bIsRobot = isRobot(lPlayerID);
            bool bIsReport = isUReport(lPlayerID);
            int iTableID = findCIDByUID(lPlayerID);
            int iChairID = findTIDByUID(lPlayerID);
            int iMatchID = findMIDByUID(lPlayerID);
            string sRoomKey = findRoomKeyByUID(lPlayerID);

            result = "uid:" + I2S(stPlayerInfo.lPlayerID)
                     + ", uState:" + I2S(stPlayerInfo.eUState)
                     + ", bIsReport:" + I2S(bIsReport)
                     + ", bIsRobot:" + I2S(bIsRobot)
                     + "\r\ngender:" + I2S(stPlayerInfo.stUInfo.iPlayerGender)
                     + ", nick:" + stPlayerInfo.stUInfo.sNickName
                     + ", head:" + stPlayerInfo.stUInfo.sHeadStr
                     + "\r\nroomCard:" + I2S(stPlayerInfo.stUWealthInfo.iRoomCard)
                     + ", goldCoin:" + I2S(stPlayerInfo.stUWealthInfo.lGoldCoin)
                     + ", diamond:" + I2S(stPlayerInfo.stUWealthInfo.iDiamond)
                     + ", point:" + I2S(stPlayerInfo.stUWealthInfo.iPoint)
                     + "\r\nroomid:" + stPlayerInfo.stHeadInfo.sRoomID
                     + ", ip:" + stPlayerInfo.stHeadInfo.sPlayerIP
                     + "\r\nrouter:" + stPlayerInfo.stHeadInfo.sServantAddr
                     + "\r\ntableid:" + I2S(iTableID)
                     + ", chairid:" + I2S(iChairID)
                     + "\r\nmatchid:" + I2S(iMatchID)
                     + ", roomkey:" + sRoomKey + "\r\n";
        }
        else
        {
            result = "user not exist";
        }
    }
}

//显示桌子
void CPlayerMng::showTInfo(const string &params, string &result) const
{
    long lPlayerID = S2I(params);
    if (isExistPlayerMap(lPlayerID))
    {
        if (isExistU2TableMap(lPlayerID))
        {
            auto pTablePtr = findTablePtrByUID(lPlayerID);
            if (pTablePtr)
            {
                TableInfo stTableInfo = pTablePtr->getTableInfo();
                RTableConfInfo stRTableConfInfo = pTablePtr->getRTableInfo();
                std::string sC2U = "";
                for (auto iter = stTableInfo.mapChair2UID.begin(); iter != stTableInfo.mapChair2UID.end(); ++iter)
                {
                    sC2U = sC2U + "(" + I2S(iter->second) + ":" + I2S(iter->first) + ")  ";
                }

                result = "tableid:" + I2S(stTableInfo.iTableID)
                         + ", eTState:" + I2S(stTableInfo.eTState)
                         + "\r\nmaxSeatCount:" + I2S(stTableInfo.iMaxSeatCount)
                         + ", minSeatCount:" + I2S(stTableInfo.iMinSeatCount)
                         + ", enterRoomUsers:" + I2S(stTableInfo.iEnterRoomUsers)
                         + "\r\nchair2uid: " + sC2U
                         + "\r\nantiCheat:" + I2S(stTableInfo.bAntiCheat)
                         + ", roomkey:" + stRTableConfInfo.sRoomKey
                         + ", masterid:" + I2S(stRTableConfInfo.lMasterID) + "\r\n";
            }
        }
        else
        {
            result = "user not at the table";
        }
    }
    else
    {
        result = "user not exist";
    }
}

//显示所有用户信息
void CPlayerMng::showUserLog() const
{
    ostringstream oss;
    ostringstream osd;
    oss << "\nshow all uses lis = { ";

    vector<long> vecUList;
    for (auto iter = m_mapCPlayer.begin(); iter != m_mapCPlayer.end(); ++iter)
    {
        vecUList.push_back(iter->first);

        osd << "\n" << "user->" << iter->first << " = { ";
        osd << iter->second->display();

        int iChairID = CHAIRID_ERR;
        int iTableID = TABLEID_ERR;
        if (isExistU2TableMap(iter->first))
        {
            iChairID = findCIDByUID(iter->first);
            iTableID = findTIDByUID(iter->first);
        }

        osd << " { " << "tableid:" << iTableID << ", chairid:" << iChairID << " }";
        osd << " }\n";
    }

    sort(vecUList.begin(), vecUList.end());

    oss << "\n users sum:" << vecUList.size() << ", usersList = { ";
    for (auto it = vecUList.begin(); it != vecUList.end(); ++it)
    {
        oss << *it << ", ";
    }

    oss << " }\n\n";
    oss << osd.str();
    FDLOG_LOG_INFO << oss.str() << endl;
}

void CPlayerMng::serverRetSetNotify()
{
    for (auto iter = m_mapCPlayer.begin(); iter != m_mapCPlayer.end(); ++iter)
    {
        LOG_DEBUG << "serverRetSetNotify playID:" << iter->first << ", robot:" << isRobot(iter->first) << endl;
        XGameComm::TMsgRespServerReset notify;
        notify.set_iresultid(0);

        RespInfo stRespInfo;
        stRespInfo.lUID = iter->first;
        stRespInfo.vecMsgDataList.push_back(pbTobuffer(notify));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGamecomm::E_MSGID_SERVER_RESET_NOTIFY));
        OuterFactorySingleton::getInstance()->toClient(stRespInfo);

        /*//设置机器人下线
        if (isRobot(iter->first))*/
        {
            auto pPushServant = OuterFactorySingleton::getInstance()->getPushServerPrx(iter->first);

            HeadInfo stHeadInfo;
            getUHInfoFromUMap(iter->first, stHeadInfo);

            //设置机器人离线状态
            userstate::ReportOnlineStateReq req;
            req.uid = iter->first;
            req.state = userstate::E_ONLINE_STATE_OFFLINE;
            req.gwAddr = stHeadInfo.sServantAddr;
            req.gwCid = 9999;
            pPushServant->async_reportOnlineState(NULL, req);

            userstate::ReportGameStateReq req1;
            req1.uid = iter->first;;
            req1.state = userstate::E_GAME_STATE_OFF;
            pPushServant->async_reportGameState(NULL, req1);
        }
    }
}