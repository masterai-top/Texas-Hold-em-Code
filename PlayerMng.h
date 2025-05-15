#ifndef  __PLAYER_MNG_H__
#define  __PLAYER_MNG_H__

#include "Player.h"
#include "Table.h"

/**
 * 座位信息描述
 */
typedef struct TUSitInfo
{
    long lPlayerID;         //玩家ID
    int iPlayerState;       //玩家状态
    int iTableID;           //桌子ID
    int iChairID;           //椅子ID
    std::string sPlayerIP;  //玩家IP
    std::string sNickName;  //玩家昵称
    std::string sHeadStr;   //玩家头像
    int iPlayerGender;      //玩家性别
} USitInfo;

/**
 *  用户管理类-以用户ID为主线
 */
class CPlayerMng
{
public:
    /**
     *
    */
    CPlayerMng();

    /**
     *
    */
    ~CPlayerMng();

public:
    // 增加接口
    int insertPlayer2UMap(const long lPlayerID);                                        //插入用户在线map，即在Room创建一个用户玩家
    int insertTable2UTMap(const long lPlayerID, CTablePtr pTablePtr);                   //玩家与桌子的映射关系
    int insertUInfo2CPlayer(const long lPlayerID, const UInfo &stUInfo);                //插入玩家个人信息
    int insertUWInfo2CPlayer(const long lPlayerID, const UWealthInfo &stUWealthInfo);   //插入玩家个人信息
    int insertUAInfo2CPlayer(const long lPlayerID, const UAccountInfo &stUAccountInfo); //插入玩家账户信息
    int insertTChairID2CTable(const long lPlayerID);                //插入玩家到具体椅子号
    int insertTChairID2CTable(const long lPlayerID, int iChairID);  //插入玩家到具体椅子号
    int insertUSegment2CPlayer(const long lPlayerID, const USegmentInfo &stUSegmentInfo);

    // 查询接口
    CPlayerPtr findPlayerPtrByUID(const long lPlayerID) const;  //通过玩家ID获取玩家对象指针
    CTablePtr findTablePtrByUID(const long lPlayerID) const;    //通过玩家ID获取桌子对象指针
    int findCIDByUID(const long lPlayerID) const;               //通过玩家ID获取桌子ID
    int findTIDByUID(const long lPlayerID) const;               //通过玩家ID获取椅子ID
    int findMIDByUID(const long lPlayerID) const;               //通过玩家ID获取MatchID
    std::string findRoomKeyByUID(const long lPlayerID) const;   //通过玩家ID获取Roomkey
    Eum_UState getUStateFromUMap(const long lPlayerID) const;   //通过玩家ID获取玩家状态
    Eum_Action getAIActionFromUMap(const long lPlayerID) const;
    std::string getUIPFromUMap(const long lPlayerID) const;     //通过玩家ID获取玩家IP
    int getURoomCardFromUMap(const long lPlayerID) const;       //通过玩家ID获取玩家房卡
    long getUGoldCoinFromUMap(const long lPlayerID) const;      //通过玩家ID获取玩家金币
    long getUPointFromUMap(const long lPlayerID) const;          //通过玩家ID获取玩家积分
    int getUDiamondFromUMap(const long lPlayerID) const;        //通过玩家ID获取玩家钻石
    int getUGameStatus(const long lPlayerID) const;             //通过玩家ID获取玩家游戏状态
    int getUWealthFromUMap(const long lPlayerID, /**out**/UWealthInfo &stUWealthInfo) const;    //通过玩家ID获取玩家财富
    int getUAccountFromUMap(const long lPlayerID, /**out**/UAccountInfo &stUAccountInfo) const; //通过玩家ID获取玩家账户
    int getUInfoFromUMap(const long lPlayerID, /**out**/UInfo &stUInfo) const;               //通过玩家ID获取玩家个人信息
    int getUHInfoFromUMap(const long lPlayerID, /**out**/HeadInfo &stHeadInfo) const;        //通过用户ID获取消息头信息
    int getUSegmentInfoFromUMap(const long lPlayerID, USegmentInfo &stUSegmentInfo) const;
    int getUPropsInfoFromUMap(const long lPlayerID,  map<int, map<int, UPropsInfo>> &mUPropsInfo) const;
    int getUFromUMap(const long lPlayerID, /**out**/PlayerInfo &stPlayerInfo) const;         //获取用户信息
    int getUBaseInfoExtFromUMap(const long lPlayerID, /**out**/UserBaseInfoExt &m_stUserBaseInfoExt) const;  //获取账户信息
    int getQSUQueue(const long lPlayerID, /**out**/UQueueInfo &stUQueueInfo) const;          //获取快速开始用户排队信息
    int getUClubRoom(const long lPlayerID, /**out**/UClubRoom &stUClubRoom) const;
    int getUGPS(const long lPlayerID, /**out**/UGPS &stUGPS) const;
    int getURobot(const long lPlayerID, /**out**/URobot &stURobot) const;
    int getUTableBlindLevel(const long lPlayerID, int roomType) const;
    int getUTableMaxSeat(const long lPlayerID, int roomType) const;
    Eum_Game_Mode getUModeFromU2TMap(const long lPlayerID) const;  //通过用户ID获取用户所在场次模式
    std::string getRoomIDFromUMap(const long lPlayerID) const;     //通过用户ID获取RoomID
    std::string getRoomIDFromU2TMap(const long lPlayerID) const;   //通过用户ID获取RoomID
    bool isRobot(const long lPlayerID) const;
    bool isUReport(const long lPlayerID) const;          //用户是否上报
    bool isUNoState(const long lPlayerID) const;         //用户是否是无状态
    bool isULoginSate(const long lPlayerID) const;       //用户是否是登录状态
    bool isUEterRoomState(const long lPlayerID) const;   //用户是否是进入房间状态
    bool isUSitTableState(const long lPlayerID) const;   //用户是否是坐桌状态
    bool isUAgreedState(const long lPlayerID) const;     //用户是否是准备状态
    bool isUPlayingState(const long lPlayerID) const;    //用户是否是游戏中状态
    bool isUOfflineState(const long lPlayerID) const;    //用户是否是离线状态
    bool isExistPlayerMap(const long lPlayerID) const;   //用户数是否在用户在线map
    bool isExistU2TableMap(const long lPlayerID) const;  //用户是否在U2Tmap中
    void eraseUserActiveTime(const long lPlayerID);
    void updateUserActiveTime(const long lPlayerID);
    //int getUEnterCountByTID(const int iTableID) const; //通过桌子ID获取进入该桌的用户数量
    //vector<long> getUEnterListByTID(const int iTableID) const; //通过桌子ID获取进入该桌的用户列表
    vector<long> getUEnterListByKey(const std::string sRoomKey) const; //通过Roomkey获取进入该桌的用户列表

    void clearAllPlayerMap();  //清理玩家

    const map<long, CPlayerPtr> &getCPlayer()
    {
        return m_mapCPlayer;
    }

    const map<long, CTablePtr> &getUTable()
    {
        return m_mapU2Talbe;
    }

    const map<long, long> &getUActiveTime()
    {
        return m_mapUActiveTime;
    }
    int getURobotListByRommID(const std::string sRoomID, /**out**/vector<long> &stRobotList) const;

    void setUBlindLevel(long lPlayerID, int level);
    int getUBlindLevel(long lPlayerID);
    void setUMaxSeat(long lPlayerID, int seatnum);
    int getUMaxSeat(long lPlayerID);
    void setUpdateGoldFlag(long lPlayerID, bool flag);
    bool getUpdateGoldFlag(long lPlayerID);
    bool getUAchievementInfoByUID(const long lPlayerID);
    int getULeagueInfoByUID(long lPlayerID);

    // 修改接口
    int updateUReportByUID(const long lPlayerID, const bool bIsReport);        //更新玩家上报
    int updateUStateByUID(const long lPlayerID, Eum_UState eUState);           //更新玩家状态
    int updateAIActionByUID(const long lPlayerID, Eum_Action eAction);
    int updateURoomCardByUID(const long lPlayerID, const long iChangeValue);    //更新玩家房卡
    int updateUGoldCoinByUID(const long lPlayerID, const long iChangeValue);    //更新玩家金币
    int updateUPointByUID(const long lPlayerID, const long iChangeValue);       //更新玩家积分
    int updateURoomIDByUID(const long lPlayerID, const std::string sRoomID);   //更新玩家所在RoomID @headinfo中的roomid
    int updateMsgHeadByUID(const long lPlayerID, const HeadInfo &stHeadInfo);  //更新用户所在消息头信息
    int updateUserBaseInfoExtByUid(const long lPlayerID, const JFGame::UserBaseInfoExt &stUserBaseInfo); //更新用户账户信息
    int updateQSUQueueByUID(const long lPlayerID, const UQueueInfo &stUQueueInfo); //更新快速开始用户排队信息
    int updateUClubRoomByUID(const long lPlayerID, const UClubRoom &stUClubRoom);
    int updateUGPSByUID(const long lPlayerID, const UGPS &stUGPS);
    int updataURobotByUID(const long lPlayerID, const URobot &stURobot);
    int updataUAchievementInfoByUID(const long lPlayerID, bool show = false);
    int updataULeagueInfoByUID(const long lPlayerID, int level);

    //结算
    int getUCalInfoByUID(const long lPlayerID, GameCalInfo &stUGameCalInfo) const;
    int updateUCalInfoByUID(const long lPlayerID, long roundID, GameCalInfo &stUGameCalInfo);
    void addPlayerAddr(const long lPlayerID);
    string getPlayerAddr(const long lPlayerID);
    void clearPlayerAddr();
    void delPlayerAddr(const long lPlayerID);

    // 删除接口 @注意在删除玩家信息时的顺序
    int eraseUser4Umap(long lPlayerID);        //从在线map移除用户
    int erasePlayerFromUMap(long lPlayerID);   //删除在线玩家
    int erasePlayerFromU2TMap(long lPlayerID, int iType = 0); //删除玩家桌子映射关系
    int erasePlayerFormTable(long lPlayerID, int iType = 0);  //删除玩家在桌子上的信息

    //辅助接口
    void reportRetrieveRobot(const long lPlayerID);
    void reportExitRoom4Coro(const long lPlayerID);
    void displayUser(const long lPlayerID, const std::string str) const; //显示玩家信息
    void showUInfo(const string &params, string &result) const; //显示用户
    void showTInfo(const string &params, string &result) const; //显示桌子
    void showUserLog() const; //显示所有用户信息
    void serverRetSetNotify();

private:
    map<long, CPlayerPtr> m_mapCPlayer;     // 在线玩家信息管理map @long-玩家ID
    map<long, CTablePtr>  m_mapU2Talbe;     // 玩家桌子信息映射map @long-玩家ID
    map<long, long> m_mapUActiveTime;
    map<long, string> m_mapCPlayerAddr; // 缓存玩家addr
};

////
typedef TC_Singleton<CPlayerMng, CreateStatic, DefaultLifetime> CPlayerMngSingleton;

#endif