#ifndef __GAME_DATA_DEF_H__
#define __GAME_DATA_DEF_H__

#include <string>
#include <map>
#include <ITableGame.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEALTH_ERR -1
#define RETCODE_ERR -1

#define TABLEID_ERR -1
#define CHAIRID_ERR -1
#define PLAYERID_ERR -1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//桌子配置信息
typedef struct _TTableConfInfo
{
    int iGameID;                //游戏ID
    string sRoomID;             //房间ID
    int iAgentID;               //代理ID
    int iMaxTableCount;         //最大桌子数量
    int iMaxSeatCount;          //最大游戏玩家数
    int iMinSeatCount;          //最小可开始游戏玩家数
    int iBasePoint;             //桌子抵注
    int iInitPoint;             //游戏初始分数
    long lMinGold;              //最少携带金币，0表示无限制
    long lMaxGold;              //最大携带金币，0表示无限制
    long lTickoutGold;          //低于金币踢出
    string sTableRule;          //房间规则
    string sRoomName;           //房间名称
    int iReportTime;            //房间动态信息上报间隔时间
    int iAutoSitTime;           //快速开始排队间隔时
    int iServiceFee;            //服务费
    int iRecycLine;             //回收线
    int iProfit;                //盈利抽成的百分比
    long bigBlind;              //大盲注
    long smallBlind;            //小盲注
    int robotNum;               //机器人数量
    vector<char> sGameConfig;   //游戏玩法配置
    CreateGameFunc pfnInitGame; //创建游戏的函数
} TableConfInfo;

//所有游戏集合
typedef struct _TGameGroup
{
    map<std::string, TableConfInfo> mapGameGroup;    // 桌子配置集合 @param string-房间ID，TableConfInfo-房间配置信息
} GameGroup;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//消息头数据
typedef struct THeadInfo
{
    int         iGameID;             // 游戏ID
    std::string sRoomID;             // RoomID
    std::string sRoomServerID;       // RoomServerID
    std::string sPlayerIP;           // 客户端ip
    std::string sServantAddr;        // Router对象地址
    THeadInfo(): iGameID(0), sRoomID(""), sRoomServerID(""), sPlayerIP(""), sServantAddr("")
    {

    }
} HeadInfo;

//消息请求数据
typedef struct TReqInfo
{
    long         lUID;      // 用户ID
    short        nMsgID;    // 推送消息ID
    vector<char> vecMsgData;// 推送的数据
} ReqInfo;

//消息返回数据
typedef struct TRespInfo
{
    long                  lUID;          // 用户ID
    vector<short>         vecMsgIDList;  //推送消息ID
    vector<vector<char> > vecMsgDataList;//推送的数据
} RespInfo;

//请求机器人
typedef struct TReqRobot
{
    vector<char> vecMsgData;
} ReqRobot;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//RoomID分段, gameMode游戏模式枚举，可扩展
enum Eum_Game_Mode
{
    E_MODE_START          = 0, // 开始
    E_PRIVATE_ROOM_MODE   = 1, // 房卡模式
    E_AOF_MODE            = 2, // AOF
    E_QUICK_START_MODE    = 3, // 快速场
    E_KNOCKOUT_MODE       = 4, // 锦标赛
    E_SNG_MODE            = 5, // 单桌赛
    E_SUOHA_MODE          = 6, // SUOHA
    E_DDZ_MODE            = 7, // 斗地主
    E_ROOM_AI_MATCH       = 8, // AI比赛
    E_SHORT_DECK_MODE     = 9, // 短牌
    E_MJ_MODE             = 10, // 二人麻将
    E_MODE_END,                // 结束
};

//RoomID分段，screen游戏场次枚举，可扩展
enum Eum_Game_Screen
{
    E_SCREEN_START      = 0,    // 开始
    E_PRIMARY_SCREEN    = 1,    // 初级场
    E_MIDDLE_SCREEN     = 2,    // 中级场
    E_HIGH_SCREEN       = 3,    // 高级场
    E_ELITE_SCREEN      = 4,    // 精英级场
    E_SCREEN_END        = 9999,   // 结束
};

//RoomIDInfo结构
typedef struct _TRoomIDInfo
{
    Eum_Game_Mode eGameMode;     // 游戏模式
    Eum_Game_Screen eGameScreen; // 游戏场次
    int iGame;                   // 游戏
} RoomIDInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __GAME_DATA_DEF_H__


