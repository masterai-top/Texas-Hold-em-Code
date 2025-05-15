#include "Player.h"

CPlayer::CPlayer(const long lPlayerID): m_lChatTime(0), m_bIsReport(false), m_bIsSetMsgHead(false)
{
    m_stPlayerInfo.lPlayerID = lPlayerID;
    m_stPlayerInfo.eUState = E_PLAYER_NO_STATE;
    m_stPlayerInfo.eAction = E_NO_ACTION;
    m_stPlayerInfo.stUWealthInfo.iRoomCard = 0;
    m_stPlayerInfo.stUWealthInfo.lGoldCoin = 0;
    m_stPlayerInfo.stUWealthInfo.iDiamond = 0;
    m_stPlayerInfo.stUWealthInfo.iPoint = 0;
    m_stPlayerInfo.stUInfo.iPlayerGender = 1;
    m_stPlayerInfo.stUSegmentInfo.iseason = 0;
    m_stPlayerInfo.stUSegmentInfo.icfgId = 0;
    m_stPlayerInfo.stUSegmentInfo.iGrade = 0;
    m_stPlayerInfo.stUSegmentInfo.iClass = 0;
    m_stPlayerInfo.stUSegmentInfo.iStar = 0;
    m_stPlayerInfo.stUSegmentInfo.iIntegral = 0;
    m_stPlayerInfo.stUSegmentInfo.iUpgrade = 0;
    m_stPlayerInfo.mUPropsInfo.clear();
    m_bUpdateGold = false;
    bAchievementInfo = false;
    m_iRound = 0;
    iLeagueLevel = 0;
}

CPlayer::~CPlayer()
{

}

long CPlayer::getPlayerID() const
{
    return m_stPlayerInfo.lPlayerID;
}

Eum_UState CPlayer::getPlayerState() const
{
    return m_stPlayerInfo.eUState;
}

Eum_Action CPlayer::getAIAction() const
{
    return m_stPlayerInfo.eAction;
}

std::string CPlayer::getPlayerIP() const
{
    return m_stPlayerInfo.stHeadInfo.sPlayerIP;
}

long CPlayer::getURoomCard() const
{
    return m_stPlayerInfo.stUWealthInfo.iRoomCard;
}

long CPlayer::getUGoldCoin() const
{
    return m_stPlayerInfo.stUWealthInfo.lGoldCoin;
}

long CPlayer::getUDiamond() const
{
    return m_stPlayerInfo.stUWealthInfo.iDiamond;
}

long CPlayer::getUPoint() const
{
    return m_stPlayerInfo.stUWealthInfo.iPoint;
}

long CPlayer::getUChatTime() const
{
    return m_lChatTime;
}

std::string CPlayer::getURoomID() const
{
    return m_stPlayerInfo.stHeadInfo.sRoomID;
}

PlayerInfo &CPlayer::getPlayerInfo()
{
    return m_stPlayerInfo;
}

UInfo &CPlayer::getUInfo()
{
    return m_stPlayerInfo.stUInfo;
}

UWealthInfo &CPlayer::getWealthInfo()
{
    return m_stPlayerInfo.stUWealthInfo;
}

UAccountInfo &CPlayer::getAccountInfo()
{
    return m_stPlayerInfo.stUAccountInfo;
}

HeadInfo &CPlayer::getUHeadInfo()
{
    return m_stPlayerInfo.stHeadInfo;
}

USegmentInfo &CPlayer::getUSegmentInfo()
{
    return m_stPlayerInfo.stUSegmentInfo;
}

UQueueInfo &CPlayer::getUQueue()
{
    return m_stUQueueInfo;
}

UClubRoom &CPlayer::getUClubRoom()
{
    return m_stUClubRoom;
}

KOPlayerInfo &CPlayer::getKOPlayerInfo()
{
    return m_stKOPlayerInfo;
}

UGPS &CPlayer::getUGPS()
{
    return m_stUGPS;
}

URobot &CPlayer::getURobot()
{
    return m_stURobot;
}

bool CPlayer::isUReport() const
{
    return m_bIsReport;
}

bool CPlayer::isUNoState() const
{
    return m_stPlayerInfo.eUState == E_PLAYER_NO_STATE;
}

bool CPlayer::isULoginSate() const
{
    return m_stPlayerInfo.eUState == E_PLAYER_LOGIN_GAME;
}

bool CPlayer::isUEterRoomState() const
{
    return m_stPlayerInfo.eUState == E_PLAYER_ENTER_ROOM;
}

bool CPlayer::isUSitTableState() const
{
    return m_stPlayerInfo.eUState == E_PLAYER_SIT_TABLE;
}

bool CPlayer::isUAgreedState() const
{
    return m_stPlayerInfo.eUState == E_PLAYING_AGREED;
}

bool CPlayer::isUPlayingState() const
{
    return m_stPlayerInfo.eUState == E_PLAYING_GAME;
}

bool CPlayer::isUOfflineState() const
{
    return m_stPlayerInfo.eUState == E_PLAYER_OFFLIEN;
}

const JFGame::UserBaseInfoExt CPlayer::getUserBaseInfoExt()
{
    return m_stUserBaseExt;
}

void CPlayer::setReport(const bool bIsReport)
{
    m_bIsReport = bIsReport;
}

void CPlayer::setPlayerState(Eum_UState eUState)
{
    m_stPlayerInfo.eUState = eUState;
}

void CPlayer::setAIAction(Eum_Action eAction)
{
    m_stPlayerInfo.eAction = eAction;
}

void CPlayer::setUChatTime(long lTime)
{
    m_lChatTime = lTime;
}

void CPlayer::setUSegmentInfo(const USegmentInfo &stUSegmentInfo)
{
    m_stPlayerInfo.stUSegmentInfo.iseason = stUSegmentInfo.iseason;
    m_stPlayerInfo.stUSegmentInfo.icfgId = stUSegmentInfo.icfgId;
    m_stPlayerInfo.stUSegmentInfo.iGrade = stUSegmentInfo.iGrade;
    m_stPlayerInfo.stUSegmentInfo.iClass = stUSegmentInfo.iClass;
    m_stPlayerInfo.stUSegmentInfo.iStar = stUSegmentInfo.iStar;
    m_stPlayerInfo.stUSegmentInfo.iIntegral = stUSegmentInfo.iIntegral;
    m_stPlayerInfo.stUSegmentInfo.iUpgrade = stUSegmentInfo.iUpgrade;
}

void CPlayer::setUSegmentInfo(int iIntegral)
{
    m_stPlayerInfo.stUSegmentInfo.iIntegral = iIntegral;
}

void CPlayer::setUInfo(const UInfo &stUInfo)
{
    m_stPlayerInfo.stUInfo.iPlayerGender = stUInfo.iPlayerGender > 0 ? stUInfo.iPlayerGender : 1;
    m_stPlayerInfo.stUInfo.iAiGameRound = stUInfo.iAiGameRound;
    m_stPlayerInfo.stUInfo.iAiGamePoint = stUInfo.iAiGamePoint;
    strncpy(m_stPlayerInfo.stUInfo.sNickName, stUInfo.sNickName, sizeof(m_stPlayerInfo.stUInfo.sNickName) - 1 );
    strncpy(m_stPlayerInfo.stUInfo.sHeadStr, stUInfo.sHeadStr, sizeof(m_stPlayerInfo.stUInfo.sHeadStr) - 1 );
    strncpy(m_stPlayerInfo.stUInfo.sSignature, stUInfo.sSignature, sizeof(m_stPlayerInfo.stUInfo.sSignature) - 1 );
}

void CPlayer::setUExperience(int level, int experience)
{
    m_stPlayerInfo.stUWealthInfo.level = level;
    m_stPlayerInfo.stUWealthInfo.experience = experience;
}

void CPlayer::setUWealthInfo(const UWealthInfo &stUWealthInfo)
{
    m_stPlayerInfo.stUWealthInfo.iRoomCard = stUWealthInfo.iRoomCard;
    m_stPlayerInfo.stUWealthInfo.lGoldCoin = stUWealthInfo.lGoldCoin;
    m_stPlayerInfo.stUWealthInfo.iDiamond = stUWealthInfo.iDiamond;
    m_stPlayerInfo.stUWealthInfo.iPoint = stUWealthInfo.iPoint;
    m_stPlayerInfo.stUWealthInfo.level = stUWealthInfo.level;
    m_stPlayerInfo.stUWealthInfo.experience = stUWealthInfo.experience;
}

void CPlayer::setUAccountInfo(const UAccountInfo &stUAccountInfo)
{
    strncpy(m_stPlayerInfo.stUAccountInfo.userName, stUAccountInfo.userName, sizeof(m_stPlayerInfo.stUAccountInfo.userName) - 1 );
    strncpy(m_stPlayerInfo.stUAccountInfo.deviceID, stUAccountInfo.deviceID, sizeof(m_stPlayerInfo.stUAccountInfo.deviceID) - 1 );
    strncpy(m_stPlayerInfo.stUAccountInfo.deviceType, stUAccountInfo.deviceType, sizeof(m_stPlayerInfo.stUAccountInfo.deviceType) - 1 );
    m_stPlayerInfo.stUAccountInfo.platform = stUAccountInfo.platform;
    m_stPlayerInfo.stUAccountInfo.channnelID = stUAccountInfo.channnelID;
    m_stPlayerInfo.stUAccountInfo.areaID = stUAccountInfo.areaID;
}

void CPlayer::setPlayerInfo(const PlayerInfo &stPlayerInfo)
{
    m_stPlayerInfo.lPlayerID = stPlayerInfo.lPlayerID;
    m_stPlayerInfo.eUState = stPlayerInfo.eUState;

    m_stPlayerInfo.stUWealthInfo.iRoomCard = stPlayerInfo.stUWealthInfo.iRoomCard;
    m_stPlayerInfo.stUWealthInfo.lGoldCoin = stPlayerInfo.stUWealthInfo.lGoldCoin;
    m_stPlayerInfo.stUWealthInfo.iDiamond = stPlayerInfo.stUWealthInfo.iDiamond;
    m_stPlayerInfo.stUWealthInfo.iPoint = stPlayerInfo.stUWealthInfo.iPoint;

    m_stPlayerInfo.stUInfo.iPlayerGender = stPlayerInfo.stUInfo.iPlayerGender > 0 ? stPlayerInfo.stUInfo.iPlayerGender : 1;
    m_stPlayerInfo.stUInfo.iAiGameRound = stPlayerInfo.stUInfo.iAiGameRound;
    m_stPlayerInfo.stUInfo.iAiGamePoint = stPlayerInfo.stUInfo.iAiGamePoint;
    strncpy(m_stPlayerInfo.stUInfo.sNickName, stPlayerInfo.stUInfo.sNickName, sizeof(m_stPlayerInfo.stUInfo.sNickName) - 1);
    strncpy(m_stPlayerInfo.stUInfo.sHeadStr, stPlayerInfo.stUInfo.sHeadStr, sizeof(m_stPlayerInfo.stUInfo.sHeadStr) - 1);
    strncpy(m_stPlayerInfo.stUInfo.sSignature, stPlayerInfo.stUInfo.sSignature, sizeof(m_stPlayerInfo.stUInfo.sSignature) - 1);
}

void CPlayer::setKOPlayerInfo(const KOPlayerInfo &stPlayerInfo)
{
    m_stKOPlayerInfo.iMatchID = stPlayerInfo.iMatchID;
    m_stKOPlayerInfo.tEnterTime = stPlayerInfo.tEnterTime;
    m_stKOPlayerInfo.eState = stPlayerInfo.eState;
    m_stKOPlayerInfo.iPoint = stPlayerInfo.iPoint;
    m_stKOPlayerInfo.iRanking = stPlayerInfo.iRanking;
    m_stKOPlayerInfo.tKnockOutTime = stPlayerInfo.tKnockOutTime;
    m_stKOPlayerInfo.iGamePoint = stPlayerInfo.iGamePoint;
    m_stKOPlayerInfo.iRankPoint = stPlayerInfo.iRankPoint;
}

void CPlayer::updateURoomCard(long iChangeRoomCard)
{
    m_stPlayerInfo.stUWealthInfo.iRoomCard = m_stPlayerInfo.stUWealthInfo.iRoomCard + iChangeRoomCard;
}

void CPlayer::updateUGoldCoin(long lChangeGoldCoin)
{
    m_stPlayerInfo.stUWealthInfo.lGoldCoin = m_stPlayerInfo.stUWealthInfo.lGoldCoin + lChangeGoldCoin;
    if (m_stPlayerInfo.stUWealthInfo.lGoldCoin < 0)
    {
        m_stPlayerInfo.stUWealthInfo.lGoldCoin = 0;
    }
}

void CPlayer::updateUDiamond(long lChangeDiamond)
{
    m_stPlayerInfo.stUWealthInfo.iDiamond = m_stPlayerInfo.stUWealthInfo.iDiamond + lChangeDiamond;
}

void CPlayer::updateUPoint(long lChangePoint)
{
    m_stPlayerInfo.stUWealthInfo.iPoint = m_stPlayerInfo.stUWealthInfo.iPoint + lChangePoint;
}

void CPlayer::setUBlindLevel(int level)
{
    m_stPlayerInfo.stUWealthInfo.iBlindLevel = level;
}

void CPlayer::setUMaxSeat(int seatnum)
{
    m_stPlayerInfo.stUWealthInfo.iMaxSeat = seatnum;
}

void CPlayer::updateMsgHead(const HeadInfo &stHeadInfo)
{
    //if (!m_bIsSetMsgHead)
    {
        m_bIsSetMsgHead = true;
        m_stPlayerInfo.stHeadInfo.iGameID = stHeadInfo.iGameID;
        m_stPlayerInfo.stHeadInfo.sRoomID = stHeadInfo.sRoomID;
        m_stPlayerInfo.stHeadInfo.sRoomServerID = stHeadInfo.sRoomServerID;
        m_stPlayerInfo.stHeadInfo.sPlayerIP = stHeadInfo.sPlayerIP;
        m_stPlayerInfo.stHeadInfo.sServantAddr = stHeadInfo.sServantAddr;
    }
}

void CPlayer::updateBaseInfoExt(const JFGame::UserBaseInfoExt &stUserBaseInfo)
{
    m_stUserBaseExt = stUserBaseInfo;
}

void CPlayer::updateURoomID(const std::string sRoomID)
{
    m_stPlayerInfo.stHeadInfo.sRoomID = sRoomID;
}

void CPlayer::updateUQueue(const UQueueInfo &stUQueueInfo)
{
    m_stUQueueInfo.iWeight = stUQueueInfo.iWeight;
    m_stUQueueInfo.tEnterTime = stUQueueInfo.tEnterTime;
}

void CPlayer::updateUClubRoom(const UClubRoom &stUClubRoom)
{
    m_stUClubRoom.sRoomKey = stUClubRoom.sRoomKey;
    m_stUClubRoom.iConfID = stUClubRoom.iConfID;
    m_stUClubRoom.iTableNO = stUClubRoom.iTableNO;
}

void CPlayer::updateUGPS(const UGPS &stUGPS)
{
    m_stUGPS.iLongitude = stUGPS.iLongitude;
    m_stUGPS.iLatitude = stUGPS.iLatitude;
}

void CPlayer::updateURobot(const URobot &stURobot)
{
    m_stURobot.bRobot = stURobot.bRobot;
    m_stURobot.iBatchID = stURobot.iBatchID;
    m_stURobot.lRobotID = stURobot.lRobotID;
}

void CPlayer::updateUAchievementInfo(bool show)
{
    bAchievementInfo = show;
}

void CPlayer::updataULeagueInfoByUID(int level)
{
    iLeagueLevel = level;
}

int CPlayer::getUBlindLevel()
{
    return m_stPlayerInfo.stUWealthInfo.iBlindLevel;
}

int CPlayer::getUMaxSeat()
{
    return m_stPlayerInfo.stUWealthInfo.iMaxSeat;
}

void CPlayer::setUpdateGoldFlag(bool flag)
{
    m_bUpdateGold = flag;
}

int CPlayer::costPropsInfo(int iPropsID, int mult, bool bCost)
{
    auto it = m_stPlayerInfo.mUPropsInfo.find(8);
    if (it == m_stPlayerInfo.mUPropsInfo.end())
    {
        return mult;
    }
    auto itt = it->second.find(iPropsID);
    if(itt == it->second.end())
    {
        return mult;
    }
    int cost_num = itt->second.iPropsExpaire >= TNOW ? 0 :(itt->second.iPropsNum >= mult ? mult : itt->second.iPropsNum);
    LOG_DEBUG <<"mult: "<< mult << ", cost_num: "<< cost_num << ", iPropsNum: "<< itt->second.iPropsNum << ", experience: "<< itt->second.iPropsExpaire << ", now: "<< TNOW << endl;
    if(bCost)
    {
        for(auto& prop : it->second)
        {
            prop.second.iPropsNum -= cost_num;
        }
        return cost_num;
    }
    
    return itt->second.iPropsExpaire >= TNOW ? 0 : (mult - cost_num);// 须花钱的数量
}

void CPlayer::updatePropsInfo(int type, const UPropsInfo &stUPropsInfo)
{
    auto it = m_stPlayerInfo.mUPropsInfo.find(type);
    if (it != m_stPlayerInfo.mUPropsInfo.end())
    {
        auto itt = it->second.find(stUPropsInfo.iPropsID);
        if(itt == it->second.end())
        {
            it->second[stUPropsInfo.iPropsID] = stUPropsInfo;
        }
        else
        {
           itt->second.iPropsNum = stUPropsInfo.iPropsNum;
            itt->second.iPropsExpaire = stUPropsInfo.iPropsExpaire; 
        }
        return;
    }
    map<int, UPropsInfo> info;
    info[stUPropsInfo.iPropsID] = stUPropsInfo;
    m_stPlayerInfo.mUPropsInfo.insert(std::make_pair(type, info));
}

map<int, map<int, UPropsInfo>>& CPlayer::getPropsInfo()
{
    return m_stPlayerInfo.mUPropsInfo;
}

bool CPlayer::getUpdateGoldFlag()
{
    return m_bUpdateGold;
}

bool CPlayer::isRobot()
{
    return m_stURobot.bRobot;
}

int iBuyInCount;
long lBuyInNum;
long lWinNum;
map<int, long> mRoundWinInfo;

void CPlayer::updateUGameCalInfo(GameCalInfo &stUGameCal, long roundID)
{
    if(stUGameCal.lBuyInNum > 0)
    {
        m_stUCalInfo.iBuyInCount += 1;
        m_stUCalInfo.lBuyInNum += stUGameCal.lBuyInNum;
    }
    m_stUCalInfo.lWinNum += stUGameCal.lWinNum;
    m_stUCalInfo.mRoundWinInfo.insert(std::make_pair(roundID, stUGameCal.lWinNum));
}

GameCalInfo &CPlayer::getUGameCalInfo()
{
    return m_stUCalInfo;
}

std::string CPlayer::display()
{
    ostringstream os;
    os  << "{ "
        << "uid:"           << m_stPlayerInfo.lPlayerID
        << ", uState:"      << m_stPlayerInfo.eUState
        << ", uReport:"     << m_bIsReport
        << " } { "
        << " gender:"       << m_stPlayerInfo.stUInfo.iPlayerGender
        << ", name:"        << m_stPlayerInfo.stUInfo.sNickName
        << ", head:"        << m_stPlayerInfo.stUInfo.sHeadStr
        << " } { "
        << "roomCard:"      << m_stPlayerInfo.stUWealthInfo.iRoomCard
        << ", goldCoin:"    << m_stPlayerInfo.stUWealthInfo.lGoldCoin
        << ", diamond:"     << m_stPlayerInfo.stUWealthInfo.iDiamond
        << ", point:"       << m_stPlayerInfo.stUWealthInfo.iPoint
        << " } { "
        << "gameid:"        << m_stPlayerInfo.stHeadInfo.iGameID
        << ", roomid:"      << m_stPlayerInfo.stHeadInfo.sRoomID
        << ", ip:"          << m_stPlayerInfo.stHeadInfo.sPlayerIP
        << ", serverid:"    << m_stPlayerInfo.stHeadInfo.sRoomServerID
        << ", routerAddr:"  << m_stPlayerInfo.stHeadInfo.sServantAddr
        << " } ";
    return os.str();
}
