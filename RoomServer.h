#ifndef _RoomServer_H_
#define _RoomServer_H_

#include <iostream>
#include "servant/Application.h"

#include "OuterFactoryImp.h"
#include "Room.h"
#include "AiMng.h"

//
using namespace tars;

/**
 * 服务实现
 */
class RoomServer : public Application
{
public:
    virtual ~RoomServer() {};

    /**
     *
    */
    virtual void initialize();

    /**
     *
    */
    virtual void destroyApp();

public:
    /*
    * 配置变更，重新加载配置
    */
    bool reloadSvrConfig(const string &command, const string &params, string &result);

    /**
     * 加载比赛场配置
    */
    bool reloadMatchConfig(const string &command, const string &params, string &result);

    bool statOnline(const string &command, const string &params, string &result);

    /*
    * 显示用户和桌子的日志
    */
    bool showRoomInfo(const string &command, const string &params, string &result);

    /*
    * 上报用户服务映射map
    */
    int roomStatusChange(const string &command, const string &params, string &result);

    /**
     * 开启某比赛场
    */
    bool startMatchGame(const string &command, const string &params, string &result);

    /**
     * 清理比赛场
    */
    bool cleanMatchGame(const string &command, const string &params, string &result);

    /**
     * 查看机器人
     */
    bool showRobot(const string &command, const string &params, string &result);

    /**
     * 加载机器人
     */
    bool loadRobot(const string &command, const string &params, string &result);

    /**
     * 清除机器人
     */
    bool cleanRobot(const string &command, const string &params, string &result);

    /**
     * 私人场结算
     */
    void cleanPRTable();

public:
    /**
     *
     */
    void initOuterFactory();

    /**
     *
     */
    void initGameServer();

    /**
     *
     */
    void showInfo(const string &params, string &result);

public:
    /**
     *
     */
    OuterFactoryImp *getOuterFactoryPtr() const
    {
        return m_pOuter;
    }

    /**
     *
     */
    CRoom *getRoomPtr() const
    {
        return m_pRoom;
    }

    /**
     *
     */
    CPlayerMng *getPlayerMngPtr() const
    {
        return m_pPlayerMng;
    }

private:
    // 房间对象指针
    CRoom *m_pRoom;
    // 外部接口对象指针
    OuterFactoryImp *m_pOuter;
    // 用户管理对象
    CPlayerMng *m_pPlayerMng;

    CAiMng *m_pAI;
};

//
extern RoomServer g_app;

//本地RoomObj具体地址
extern string g_sLocalRoomObj;

////////////////////////////////////////////

#endif


