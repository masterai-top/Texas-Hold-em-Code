#ifndef  __PLAYER_H__
#define  __PLAYER_H__

#include <string>
#include <vector>

//
#include "GameDataDef.h"
#include "servant/Application.h"
#include "JFGame.h"
#include "OuterFactoryImp.h"

// 玩家状态 --定义：状态是一个持续性过程
enum Eum_UState
{
    E_PLAYER_STATE_ERR   = -1, //错误状态
    E_PLAYER_NO_STATE    = 0,  //未定义   -初始化定义的
    E_PLAYER_LOGIN_GAME  = 1,  //登录游戏 -玩家进入游戏大厅
    E_PLAYER_ENTER_ROOM  = 2,  //进入房间 -玩家进入具体场次房间，而且已经分配了桌子号，但还玩家没有坐到具体椅子上
    E_PLAYER_SIT_TABLE   = 3,  //坐在桌子 -玩家已经分配椅子号，等待其他玩家就位，但游戏没有开始
    E_PLAYING_AGREED     = 4,  //玩家同意 -玩家点击准备
    E_PLAYING_GAME       = 5,  //正在游戏 -玩家正在游戏中
    E_PLAYER_OFFLIEN     = 6,  //玩家断线 -玩家杀进程、从游戏app退出、网络中断、游戏中强行退出
    E_PLAYER_WATCH       = 7,  //玩家观战
    E_PLAYER_MATCHING    = 8,  //玩家匹配中

    //后续补充旁观状态
    E_PLAYER_END,                   //结束
};

enum Eum_Action
{
    E_NO_ACTION = 0,
    E_WATCH = 1,
    E_EXIT_0 = 2,
    E_EXIT_1 = 3,
    E_EXIT_2 = 4,
    E_EXIT_3 = 5,
    E_EXIT,
};

//账户信息
typedef struct TUAccountInfo
{
    char userName[128];   //账号
    char deviceID[128];   //设备唯一码（android: imei, ios: idfa）
    char deviceType[128]; //机型信息
    int platform;         //平台类型
    int channnelID;       //渠道号
    int areaID;           //区域代码
    TUAccountInfo():  platform(0), channnelID(0), areaID(0)
    {
        memset(userName, 0, sizeof(userName));
        memset(deviceID, 0, sizeof(deviceID));
        memset(deviceType, 0, sizeof(deviceType));
    }
} UAccountInfo;

//财富信息
typedef struct TUWealthInfo
{
    long iRoomCard; // 房卡
    long lGoldCoin; // 金币
    long iDiamond;  // 钻石
    long iPoint;    // 积分
    int iBlindLevel;//匹配盲注等级
    int iMaxSeat;//匹配座位号
    int level; //等级
    int experience; //经验
    // 后续可补充其它财富信息
    TUWealthInfo(): iRoomCard(0), lGoldCoin(0), iDiamond(0), iPoint(0), iBlindLevel(0), iMaxSeat(0), level(0), experience(0) {}
} UWealthInfo;

//玩家信息
typedef struct TUInfo
{
    int  iPlayerGender;   //玩家性别 0-未知，1-男， 2-女
    int  iAiGameRound;
    int  iAiGamePoint;
    char sNickName[128];  //玩家昵称
    char sHeadStr[512];   //玩家头像地址
    char sSignature[128]; //玩家签名
    //后续可补充玩家信息，比如：会员,等级等
    TUInfo(): iPlayerGender(0), iAiGameRound(0), iAiGamePoint(0)
    {
        memset(sNickName, 0, sizeof(sNickName));
        memset(sHeadStr, 0, sizeof(sHeadStr));
        memset(sSignature, 0, sizeof(sSignature));
    }
} UInfo;

typedef struct TUSegmentInfo
{
    int iseason;
    int icfgId;
    int iGrade; //当前段位
    int iClass; //阶级（1-5）
    int iStar;   //星数
    int iIntegral; //升级积分
    int iUpgrade;  //是否晋级赛 1：晋级赛
    TUSegmentInfo(): iseason(0), icfgId(0), iGrade(0), iClass(0), iStar(0), iIntegral(0), iUpgrade(0) {}
} USegmentInfo;

typedef struct TUPropsInfo
{
    int iPropsID;
    int iPropsNum;
    long iPropsExpaire;
    TUPropsInfo(): iPropsID(0), iPropsNum(0), iPropsExpaire(0) {}
} UPropsInfo;

//玩家信息
typedef struct TPlayerInfo
{
    long         lPlayerID;      //玩家ID
    Eum_UState   eUState;        //玩家状态
    Eum_Action   eAction;        //机器人后续操作
    UInfo        stUInfo;        //玩家个人信息
    UWealthInfo  stUWealthInfo;  //玩家财富信息
    HeadInfo     stHeadInfo;     //消息头信息
    UAccountInfo stUAccountInfo; //账户信息
    USegmentInfo stUSegmentInfo; //玩家段位信息
    map<int, map<int, UPropsInfo>> mUPropsInfo;// 道具信息
    TPlayerInfo(): lPlayerID(0), eUState(E_PLAYER_STATE_ERR)
    {
        mUPropsInfo.clear();
    }
} PlayerInfo;

//快速开始排队信息
typedef struct TUQueueInfo
{
    int     iWeight;        //权重
    time_t  tEnterTime;     //加入时间
    TUQueueInfo(): iWeight(0), tEnterTime(0) {}
} UQueueInfo;

//俱乐部房间信息
typedef struct TUClubRoom
{
    std::string sRoomKey;
    int iConfID;
    int iTableNO;
    TUClubRoom() : sRoomKey(""), iConfID(0), iTableNO(0)
    {

    }
} UClubRoom;

//地理信息
typedef struct TUGPS
{
    int iLongitude; //经度
    int iLatitude;  //纬度
    TUGPS(): iLongitude(0), iLatitude(0)
    {

    }
} UGPS;

//机器人信息
typedef struct TURobot
{
    bool bRobot;
    int  iBatchID;
    long lRobotID;
    TURobot(): bRobot(false), iBatchID(0), lRobotID(0)
    {

    }
} URobot;

/**
 * 比赛用户状态
*/
enum Eum_UMState
{
    E_MPLAYER_NO_STATE,             // 没有状态，初始化赋值
    E_MPLAYER_JOINMATCH,            // 玩家报名等待开赛状态
    E_MPLAYER_SIT,                  // 坐下状态
    E_MPLAYER_PLAY_GAME,            // 比赛进行中游戏中状态
    E_MPLAYER_WAIT_NEXTROUND,       // 等待下一轮状态
    E_MPLAYER_WAIT_ADDITIONAL,      // 增购状态
    E_MPLAYER_OFFLINE,              // 玩家断线状态
    E_MPLAYER_WAIT_KNOCK_OUT,       // 玩家等待出局
};

/**
 * 比赛场玩家信息
*/
typedef struct TKOPlayerInfo
{
    int         iMatchID;      //MatchID
    time_t      tEnterTime;    //报名时间
    Eum_UMState eState;        //玩家状态
    int         iPoint;        //积分
    int         iRanking;      //排名
    time_t      tKnockOutTime; //淘汰时间
    long        iGamePoint;    //游戏中积分
    long        iRankPoint;    //游戏中积分，用于排名
    long        iLastRankPoint;    //上一局游戏中积分
    TKOPlayerInfo() : iMatchID(0), tEnterTime(0), eState(E_MPLAYER_NO_STATE), iPoint(0),
        iRanking(0), tKnockOutTime(0), iGamePoint(0), iRankPoint(0), iLastRankPoint(0)
    {

    }
} KOPlayerInfo;

//私人场结算信息
typedef struct  TGameCalInfo
{
    int iBuyInCount;
    long lBuyInNum;
    long lWinNum;
    map<int, long> mRoundWinInfo;
    TGameCalInfo() : iBuyInCount(0), lBuyInNum(0), lWinNum(0)
    {
        mRoundWinInfo.clear();
    }

} GameCalInfo;

/**
 * 用户基础类
 */
class CPlayer: public TC_HandleBase
{
public:
    CPlayer(const long lPlayerID);
    virtual ~CPlayer();

public:
    // 查询接口
    long getPlayerID() const;
    //
    Eum_UState getPlayerState() const;

    Eum_Action getAIAction() const;
    //
    std::string getPlayerIP() const;
    //
    long getURoomCard() const;
    //
    long getUGoldCoin() const;
    //
    long getUDiamond() const;
    //
    long getUPoint() const;
    //
    long getUChatTime() const;
    //
    std::string getURoomID() const;
    //
    PlayerInfo &getPlayerInfo();
    //
    UInfo &getUInfo();
    //
    UWealthInfo &getWealthInfo();
    //
    UAccountInfo &getAccountInfo();
    //
    HeadInfo &getUHeadInfo();
    //
    USegmentInfo &getUSegmentInfo();
    //
    UQueueInfo &getUQueue();
    //
    UClubRoom &getUClubRoom();
    //
    KOPlayerInfo &getKOPlayerInfo();
    //
    UGPS &getUGPS();
    //
    URobot &getURobot();
    //
    bool isUReport() const;
    //
    bool isUNoState() const;
    //
    bool isULoginSate() const;
    //
    bool isUEterRoomState() const;
    //
    bool isUSitTableState() const;
    //
    bool isUAgreedState() const;
    //
    bool isUPlayingState() const;
    //
    bool isUOfflineState() const;
    //
    const JFGame::UserBaseInfoExt getUserBaseInfoExt();
    //
    void setReport(const bool bIsReport);
    //
    void setPlayerState(Eum_UState eUState);
    void setAIAction(Eum_Action eAction);
    //
    void setUChatTime(long lTime);
    //
    void setUSegmentInfo(int iIntegral);
    //
    void setUSegmentInfo(const USegmentInfo &stUSegmentInfo);
    //
    void setUInfo(const UInfo &stUInfo);
    //
    void setUExperience(int level, int experience);
    //
    void setUWealthInfo(const UWealthInfo &stUWealthInfo);
    //
    void setUAccountInfo(const UAccountInfo &stUAccountInfo);
    //
    void setPlayerInfo(const PlayerInfo &stPlayerInfo);
    //
    void setKOPlayerInfo(const KOPlayerInfo &stKOPlayerInfo);
    //
    void updateURoomCard(long iChangeRoomCard);
    //
    void updateUGoldCoin(long lChangeGoldCoin);
    //
    void updateUDiamond(long lChangeDiamond);
    //
    void updateUPoint(long iChangePoint);
    //
    void updateMsgHead(const HeadInfo &stHeadInfo);
    //
    void updateBaseInfoExt(const JFGame::UserBaseInfoExt &stUserBaseInfo);
    //
    void updateURoomID(const std::string sRoomID);
    //
    void updateUQueue(const UQueueInfo &stUQueueInfo);
    //
    void updateUClubRoom(const UClubRoom &stUClubRoom);
    //
    void updateUGPS(const UGPS &stUGPS);
    //
    void updateURobot(const URobot &stURobot);
    //
    void updateUAchievementInfo(bool show);
    //
    void updataULeagueInfoByUID(int level);
    //
    void setUBlindLevel(int level);
    //
    void setUMaxSeat(int seatnum);
    //
    int getUBlindLevel();

    int getUMaxSeat();

    int costPropsInfo(int iPropsID, int mult = 1, bool bCost = false);

    void updatePropsInfo(int type, const UPropsInfo &stUPropsInfo);

    map<int, map<int, UPropsInfo>>& getPropsInfo();

    //
    void setUpdateGoldFlag(bool flag);
    //
    bool getUpdateGoldFlag();
    //
    bool getUAchievementInfo()
    {
        return bAchievementInfo;
    }
    int getULeagueInfo()
    {
        return iLeagueLevel;
    }
    //
    bool isRobot();

    void updateUGameCalInfo(GameCalInfo &stUGameCal, long roundID);

    GameCalInfo& getUGameCalInfo();
    //
    std::string display();

    //
    void setRound(int round)
    {
        m_iRound = round;
    }
    int getRound() const
    {
        return m_iRound;
    }

private:
    //
    long m_lChatTime;
    //
    bool m_bIsReport;
    //
    bool m_bIsSetMsgHead;
    //
    PlayerInfo m_stPlayerInfo;
    //
    UQueueInfo m_stUQueueInfo;
    //
    UClubRoom m_stUClubRoom;
    //
    KOPlayerInfo m_stKOPlayerInfo;
    //
    UGPS m_stUGPS;
    //
    URobot m_stURobot;
    //
    JFGame::UserBaseInfoExt m_stUserBaseExt;
    //
    bool m_bUpdateGold;

    GameCalInfo m_stUCalInfo;

    bool bAchievementInfo;
    //参与玩牌局数
    int m_iRound;

    int iLeagueLevel;
};

//
typedef TC_AutoPtr<CPlayer> CPlayerPtr;

#endif