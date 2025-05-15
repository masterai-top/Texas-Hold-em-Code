#ifndef _OUTER_FACTORY_IMP_H_
#define _OUTER_FACTORY_IMP_H_

#include <string>
#include <vector>

#include "globe.h"
#include "LogComm.h"
#include "OuterFactory.h"
#include "GameDataDef.h"

#include "JFGamePrProto.h"
#include "JFGameFsProto.h"
#include "JFGameKoProto.h"
#include "JFGameClProto.h"
#include "JFGameRgProto.h"
#include "Java2RoomProto.h"
#include "RoomSoProto.h"
#include "AIProto.h"
#include "XGameComm.pb.h"
#include "XGameQs.pb.h"
#include "XGameKo.pb.h"
#include "XGameAi.pb.h"
#include "XGameSNG.pb.h"
#include "XGameMJ.pb.h"

#include "Push.h"
#include "HallServant.h"
#include "ConfigServant.h"
#include "MatchServant.h"
#include "AIServant.h"
#include "GlobalServant.h"
#include "GameRecordServant.h"
#include "GMServant.h"
#include "PushServant.h"
#include "ActivityServant.h"
#include "Log2DBServant.h"
#include "OuterFactoryImp.h"
#include "SocialServant.h"
#include "GameRecordProto.h"

#include "util/tc_hash_fun.h"

//
using namespace tars;
using namespace config;
using namespace global;
using namespace hall;

class CKOTableMng;
class CQSTableMng;
class CAITableMng;
class SNGTableMng;
class CThirdPartyManager;
struct TSNGTableConfInfo;

//游戏类型
typedef enum eGameType
{
    GAME_TYPE_NONE = 0,
    GAME_TYPE_SNG = 1,
    GAME_TYPE_QS  = 2,
    GAME_TYPE_AI  = 3,
    GAME_TYPE_KO  = 4,
    GAME_TYPE_PR  = 5,
} GameType;

//取比赛信息请求参数
typedef struct TGetMatchInfoParam
{
    long lPlayerID;             // 玩家ID
    CKOTableMng *pKOTableMng;   // KO桌子管理指针
    CAITableMng *pAiTableMng;   // AI桌子管理指针
    SNGTableMng *pSNGTableMng;  // SNG桌子管理指针
    CQSTableMng *pQSTableMng;  // 经典场桌子管理指针
    int iMatchID;               // 比赛ID
    int iTableID;               // 桌子ID
    TGetMatchInfoParam() : lPlayerID(0), pKOTableMng(NULL), pAiTableMng(NULL), pSNGTableMng(NULL), pQSTableMng(NULL), iMatchID(0), iTableID(0)
    {

    }
} GetMatchInfoParam;

//桌子每日统计数据
typedef struct TTableDayStat
{
    long round_num; //开局次数
    long game_time;
    long robot_win;
    long sys_recyle;
    long sys_output;
    long total_win;
    long total_lose;
    std::map<long, long> m_UGame; //参与牌局
    std::map<long, long> m_UBet;  //入池
    std::set<long> s_Create; //房主
    TTableDayStat()
    {
        Clear();
    }

    void Clear()
    {
        round_num = 0;
        game_time = 0;
        robot_win = 0;
        sys_recyle = 0;
        sys_output = 0;
        total_win = 0;
        total_lose = 0;
        m_UBet.clear();
        m_UGame.clear();
        s_Create.clear();
    }
} TableDayStat;

//时区
#define ONE_DAY_TIME (24*60*60)
#define ZONE_TIME_OFFSET (8*60*60)

/**
 * 外部工具接口对象工厂
 */
class OuterFactoryImp
{
public:
    /**
     *
    */
    OuterFactoryImp();

    /**
     *
    */
    virtual ~OuterFactoryImp();

private:
    /**
     *
    */
    void createAllObject();

    /**
     *
    */
    void deleteAllObject();

public:
    //
    const JFGame::PushPrx getRouterPushPrx(const long uid);
    //
    const hall::HallServantPrx getHallServantPrx(const long uid);
    //
    const config::ConfigServantPrx getConfigServantPrx();
    //
    const match::MatchServantPrx getMatchServantPrx(const long uid);
    //
    const ai::AIServantPrx getAIServantPrx(const int type, const long uid);
    //
    const ai::AIServantPrx getAIServantPrx(const int type, const string &key);
    //
    const global::GlobalServantPrx getGlobalServantPrx(const long uid);
    //
    const push::PushServantPrx getPushServerPrx(const long uid);
    //
    const gamerecord::GameRecordServantPrx getGameRecordPrx(const long uid);
    //
    const gamerecord::GameRecordServantPrx getGameRecordPrx(const string &key);
    //
    const gm::GMServantPrx getGMServantPrx();
    //
    const activity::ActivityServantPrx getActivityServantPrx(const long uid);
    //
    const DaqiGame::Log2DBServantPrx getLog2DBServantPrx(const long uid);
    //
    const Social::SocialServantPrx getSocialServantPrx(const long uid);

public:
    //格式化时间
    string GetTimeFormat();

public:
    //加载配置
    void load();
    //加载比赛配置
    void loadMatchConfig();
    //读取所有配置
    void readAllConfig();
    //加载游戏配置
    void loadRoomConfig();
    //打印房间配置
    void printRoomConfig();
    //比赛配置
    void readAllMatchConfig();
    //打印所有比较配置
    void printAllMatchConfig();
    //锦标赛房间配置
    const ListAllMatchConfigResp &getAllMatchConfig();
    //锦标赛房间配置
    const MatchRoom *getMatchRoomConfig(const string &roomID, int matchID);
    //
    const map<int, MatchRoom> *getMatchRoomConfig(const string &roomID);
    //
    const map<string, MatchRoom> &getMatchRoomConfig();
    //增值服务配置
    void readBaseServiceConfig();
    //打印增值服务配置
    void printBaseServiceConfig();
    //获取增值服务配置
    const ListBaseServiceConfigResp &getBaseServiceConfig();
    //加载服务配置参数
    void readSystemConfig();
    //打印服务配置参数
    void printSystemConfig();
    //mtt邮件配置
    void readMatchMailConfig();
    //获取配牌开关
    int getDealFlag();
    //加载配牌数据
    void readDealCards();
    //打印配牌数据
    void printDealCards();
    //设备配牌数据
    void setDealCards(const gm::GetCardsResp &rsp);
    //获取配牌数据
    const gm::GetCardsResp &getDealCards();
    //设置游戏配置信息
    void setGameGroup(config::ListGameRoomServerResp &rspSysConf);
    //游戏库配置
    void readGameSo();
    //
    void printGameSo();
    //
    std::string &getGameSo();
    //
    int getTaskProcNum();
    //
    int getTaskProcTimeout();

public:
    //解析RoomID
    int parseRoomIDInfo(string sRoomID, /**out**/RoomIDInfo &stRoomIDInfo);
    //获取游戏模式:3-经典场,4-MTT,5-SNG
    int getGameModeFromRoomID(string sRoomID);
    //获取游戏人数
    int getPlayersNumFromRoomID(string sRoomID);
    //解析RoomServerID
    int64_t parseRoomServerID(const string sIP, const short nPort);
    //拆分字符串
    vector<string> split(const string &str, const string &pattern);
    //解析游戏模式
    Eum_Game_Mode parseGameMode(const std::string sRoomID);

public:
    //发送客户端
    int toClient(const RespInfo &stRespInfo);
    //发送给客户端
    int toClientPb(const RespInfo &stRespInfo);

    int toDelayClientPb(const RespInfo &stRespInfo, const string addr);
    //
    void logRobotWinStat(const string roomid, map<int, std::map<int, long>> &m_RobotWinStat);
    //
    void logDayStat(const string roomid, map<string, TableDayStat>  &dayStat, const std::map<long, long> &m_UGame = std::map<long, long>());
    //
    void logCoinChange(const long lPlayerID, const long changeCoin, const int changeType);
    //
    void logGameRoundInfo(const string &roomID, const int iTableID, long roundID, int maxSeat, int blindlevel, RoomSo::TGAME_GameFinish *pGameFinish, long sys_output = 0, long sys_recyle = 0);

public:
    //推送用户帐号信息
    int async2PushUserAccountInfo(const long lPlayerID);
    //同步玩家金币变化
    int sync2UserWealthToRoom(const long lPlayerID);
    //获取用户信息接口
    int async2GetUserBasic(const long lPlayerID);
    //用户道具信息
    int async2GetPropsInfo(const long lPlayerID);
    //
    int async2ChangePropsInfo(const long lPlayerID, int cost_num);
    //获取用户帐号接口
    int async2GetUserAccount(const long lPlayerID);
    //修改用户帐号接口
    int async2ModifyUAccount(const userinfo::ModifyUserAccountReq &req);
    int async2ModifyUserInfo(const userinfo::ModifyUserInfoReq &req);
    //经典场玩家行为信息
    int asyncReportQSUserActInfo(const RoomSo::TGAME_GameFinish &req);
    //比赛场玩家行为信息
    int asyncReportKOUserActInfo(const gamerecord::ReportKOUserActInfoReq &req);
    //上报游戏信息
    int asyncReportUGameInfo(const string &sRoomID, const string &sRoomKey, long lPlayerID, int iStatus, int iBlindLevel, long createID, int seatNum, long createTime, long endTime, int iPet, RoomSo::TGAME_PlayerCal &stCal);
    //AI场玩家行为信息
    int asyncReportAIUserActInfo(const RoomSo::TGAME_GameFinish &req, int blindlevel = 1);
    //异步调用用户游戏上线状态
    void asyncRequest2UserStateOnGame(long iUin, const string &sRoomID, int tableID, long gold = 0, int matchID = 0);
    //异步调用用户游戏下线状态
    void asyncRequest2UserStateOffGame(long iUin);
    //房间宕机，用于补偿玩家
    void asyncRequest2UserReturnCompensateGold(const string &sRoomID);
    //房间玩家集合
    void asyncRequest2RoomUsers(const string &sRoomID, const string &uids, map<int, std::map<int, long>> &m_RobotWinStat, long totalRobotWin = 0);
    //推送比赛开始消息
    void asyncRequest2PushMatchBegin(const push::PushMsgReq &msg);
    //推送邀请消息
    void asyncRequest2PushInvitePlayer(const push::PushMsgReq &msg);
    //获取比赛信息
    int asyncGetMatchUserInfo(const GetMatchInfoParam &reqGetMatchInfo, const match::MatchUserInfoReq &req);
    //取消比赛
    int asyncCancelMatch(const GetMatchInfoParam &reqGetMatchInfo, const match::CancelMatchReq &req);
    //清理比赛
    int asyncCleanMatch(const GetMatchInfoParam &reqGetMatchInfo, const match::CleanMatchReq &req);
    //检查重购资格
    int asyncRepurchaseCheck(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req);
    //检查增购资格
    int asyncAdditionalCheck(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req);
    //更新玩家晋级状态
    int asyncUpdateRise(const GetMatchInfoParam &reqGetMatchInfo, const match::UserSignUpInfoReq &req);
    //重购
    int asyncRepurchaseConsume(const GetMatchInfoParam &reqGetMatchInfo, const match::ConsumeBuyResReq &req);
    //增购
    int asyncAdditionalConsume(const GetMatchInfoParam &reqGetMatchInfo, const match::ConsumeBuyResReq &req);
    //上报用户信息
    int asyncReportUserInfo(const match::ReportUserGameInfoReq &req);
    //上报比赛信息
    int asyncReportMatchInfo(const int matchID, const match::ReportMatchInfoReq &req);
    //上报滚轮赛奖励
    int asyncReportMatchReward(const match::RewardGoods &req, int matchID, int tableID);
    //上报玩家淘汰信息
    int asyncReportUserKnockoutInfo(const match::ReportUserKnockoutInfoReq &req);
    //上报玩家淘汰信息
    int asyncReportUserKnockoutInfo(const int iMatchID, const long lPlayerID);
    //上报玩家状态信息
    int asyncReportUserStateInfo(const match::ReportUserStateReq &req);
    //上报玩家状态信息
    int asyncReportUserStateInfo(const int iMatchID, const long lPlayerID, match::UserState state);
    //发送奖励
    int asyncReward(const GetMatchInfoParam &reqGetMatchInfo, const match::RewardReq &req);
    //比赛报名
    int asyncSignUp(const GetMatchInfoParam &reqGetMatchInfo, const match::SignUpReq &req);
    //取消报名
    int asyncQuit(const GetMatchInfoParam &reqGetMatchInfo, const match::QuitReq &req);
    //检测用户是否在其它场次
    int asyncCheckUser(const long lPlayerID, const std::string sRoomID);
    //获取机器信息
    int async2GetRobot(const int type, const ai::TGetRobotReq &req);
    //机器人重生
    int async2RetrieveRobot(const int type, const string &sRoomID);
    //上报任务数据
    int async2TaskReport(const task::TaskReportReq &req);
    //上报任务数据
    int async2TaskCollect(const task::CollectReq &req);
    //上报任务收集数据
    int async2TaskCollectAI(const RoomSo::TGAME_GameFinish &req, const string &roomID);
    int async2TaskCollectAOF(const RoomSo::TGAME_GameFinish &req, const string &roomID);
    //参与经典场游戏
    int async2TaskCollectQS(const RoomSo::TGAME_GameFinish &req, const string &roomID);
    // 参与SNG比赛
    int async2TaskCollectSNG(long uid, const string &roomID);
    //参与比赛
    int async2TaskCollectKOPlayingIn(long uid, const string &roomID);
    //获得比赛冠军奖励
    int async2TaskCollectKOChampion(long uid, const string &roomID);
    //获得比赛场奖励筹码数量
    int async2TaskCollectKORewardChips(long uid, const string &roomID, const long &lChips);
    //保存活动金钱
    int async2ActivitySaveMoney(long lPlayerID, long bet);
    //日志入库
    void asyncLog2DB(const long uid, const DaqiGame::TLog2DBReq &req);
    //获取玩家赛季信息
    int asyncGetUserSegmentInfo(const userinfo::UserSeasonInfoGetReq &req);
    //更新玩家赛季信息
    int asyncUpdateUserSegmentInfo(const GetMatchInfoParam &reqGetMatchInfo, const string &reward, const userinfo::UserSeasonInfoUpdateReq &req);
    //更新玩家经验
    int asyncUpdateUserLevelExperience(const long lPlayerID, const int experience);
    //创建房间消耗
    int asyncCreateRoomCost(const long lPlayerID, const int props_id, const int number);
    //上报房间桌子状态
    int asyncReportRoomTableStat(const hall::RoomTableStat &data);
    //上报房间桌子信息
    int asyncReportRoomTableInfo(const push::RoomTableInfo &data);
    //请求机器人决策
    int asyncDoRobotDecide(const int type, const GetMatchInfoParam &reqGetMatchInfo, const RoomSo::TRobotDecideReq &req);
    //请求AI决策
    int asyncDoAiDecide(const int type, const ai::TAIBrainDecideReq &req);
    //请求AI转发消息
    int asyncDoAiForward(const int type, const ai::TAIMsgTransmitReq &req);
    //上报房间在线人数
    int asyncReportRoomOnlineCount(const string roomID, long smallBlind, long count);
    //获取机器人列表
    void getRobotList(vector<tars::Int64> &robotList);
    //盲注场广播推送
    int asyncBroadcastBlind(push::E_Reward_Mold_ID id, const RoomSo::TGAME_GameFinish &gameFinish, int iBlindLevel);
    //比赛场广播推送
    int asyncBroadcastMatch(push::E_Reward_Mold_ID id, long lPlayerID, int ranking, const TSNGTableConfInfo &tableInfo);
    //推送广播
    int asyncBroadcast(const push::RewardNotify &notify);
    //上报获得宝箱进度
    void asyncReportBoxProgressRate(const RoomSo::TGAME_GameFinish &req, int iBlindLevel, long lBigBlind);
    //mtt 奖励邮件
    int asyncMailNotifyReward(long lUid, int iRank, int iMatchID, const map<int, long>& mapReward);
    //mtt 退赛邮件
    int asyncMailNotifyQuit(long lUid, int iMatchID, const map<int, long>& mapReward);

    void loadAlgConfig();
    std::string getAlgConfigPath();
    int getAiPlatformOpen();
    //
    std::string getAiPlatformToken();
    //
    std::string getAiPlatformAddr();
    //
    std::vector<int>& getSeatNumber();
    //
    int getAiPlatformPort();


public:
    /**
     * 游戏组配置信息
     */
    GameGroup &getGamegroup()
    {
        return m_stGameGroup;
    }

    /**
     * 获取配置文件对象
     */
    const tars::TC_Config &getConfig() const
    {
        return *_pFileConf;
    }


private:
    // RouterServer的对象名称
    string _sPushObj;
    JFGame::PushPrx _pushPrx;

    // HallServer的对象名称
    string _sHallServantObj;
    hall::HallServantPrx _hallServantPrx;

    //ConfigServer的对象名称
    string _sConfigServerObj;
    config::ConfigServantPrx _configServantPrx;

    // MatchServer的对象名称
    string _sMatchServerObj;
    match::MatchServantPrx _matchServantPrx;

    //AI
    string _sAIServantObj;
    //SNG
    string _sSNGServantObj;
    //QS
    string _sQSServantObj;
    //KO
    string _sKOServantObj;
    //PR
    string _sPRServantObj;
    //
    ai::AIServantPrx _AIServantPrx;

    // ActivityServer的对象名称
    std::string _sActivityServantObj;
    activity::ActivityServantPrx _activityServantPrx;

    // PushServer的对象名称
    string _sPushServantObj;
    push::PushServantPrx _pushServantPrx;

    // RecordServer的对象名称
    string _sGameRecordObj;
    gamerecord::GameRecordServantPrx _recordServantPrx;

    // GMServer的对象名称
    string _sGMObj;
    gm::GMServantPrx _gmServantPrx;

    // GlobalServer的对象名称
    std::string _sGlobalServantObj;
    global::GlobalServantPrx _globalServantPrx;

    // Log2DBServer的对象名称
    std::string _sLog2DBServantObj;
    DaqiGame::Log2DBServantPrx _log2DBServantPrx;

    // SocialServer的对象名称
    std::string _sSocialServantObj;
    Social::SocialServantPrx _socialServantPrx;

private:
    //游戏配置信息
    GameGroup m_stGameGroup;
    //比赛配置信息
    ListAllMatchConfigResp listAllMatchConfigResp;
    //锦标赛房间配置
    map<string, map<int, MatchRoom>> mapMatchRoomConfig;  //roomid: {level: matchinfo}
    map<string, MatchRoom> mapOKMatchRoomConfig;  //roomid: {level: matchinfo}
    //增值服务配置
    ListBaseServiceConfigResp baseServiceConfig;
    //SO文件路径
    std::string gameSoPath;

    //algConfigPath
    std::string algConfigPath;
        //
    int aiPlatformOpen;
    //
    std::string aiPlatformToken;
    //
    std::string aiPlatformAddr;
    //
    int aiPlatformPort;
    //
    std::vector<int> vSeatNum;
    //mtt 房间名称
    std::map<int, string> mapMatchName;
    //mtt 邮件奖励模板
    string sMatchMailRewardContent;
    //mtt 邮件退赛模板
    string sMatchMailQuitContent;
private:
    //配牌数据
    gm::GetCardsResp getCardsResp;
    //配牌器开关(0-关闭，1-开启)
    int _dealFlag;
    //防止数据读写不一致
    wbl::ReadWriteLocker m_rwlock;
    //服务配置对象
    tars::TC_Config *_pFileConf;
    //每个时间片任务处理数量
    int _taskProcNum;
    //每个时间片任务超时警示（单位为毫秒）
    int _taskProcTimeout;
};

//singleton
typedef TC_Singleton<OuterFactoryImp, CreateStatic, DefaultLifetime> OuterFactorySingleton;

//编码 -- for test
template <typename T>
vector<char> Encoding(T &s)
{
    tars::TarsOutputStream<> jos;
    s.writeTo(jos);
    return jos.getByteBuffer();
}

//解码 -- for test
template <typename T>
T Decoding(vector<char> vc)
{
    T t;
    tars::TarsInputStream<> jis;
    jis.setBuffer(vc.data(), vc.size());
    t.readFrom(jis);
    return t;
}

#endif

