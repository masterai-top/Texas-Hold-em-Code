#include "RoomServantImp.h"
#include "servant/Application.h"
#include "RoomServer.h"
#include "XGameComm.pb.h"
#include "XGameQs.pb.h"
#include "XGameKo.pb.h"
#include "XGameAi.pb.h"
#include "XGameSh.pb.h"
#include "XGameDDZ.pb.h"
#include "XGameMTT.pb.h"
#include "AsyncLoginRoomCallback.h"

//
using namespace std;

//////////////////////////////////////////////////////
void RoomServantImp::initialize()
{
    //
    getHandle()->setWaitTime(5);

    //初始化定时器
    initializeTimer();
}

//////////////////////////////////////////////////////
void RoomServantImp::destroy()
{
}

/**
* 初始化定时器
*/
void RoomServantImp::initializeTimer()
{
    m_bIsSetTimer = false;
    m_threadTimer.initialize(this);
    m_threadTimer.start();
}

/**
 * 处理终端请求
 */
tars::Int32 RoomServantImp::onRequest(tars::Int64 lUin, const std::string &sMsgPack, const std::string &sCurServrantAddr, const JFGame::TClientParam &stClientParam, const JFGame::UserBaseInfoExt &stUserBaseInfo, tars::TarsCurrentPtr current)
{
    int iRet = 0;

    int64_t tTime = TNOWMS;

    FUNC_ENTRY(lUin);

    __TRY__


    //请求日志
    ROLLLOG_DEBUG << "recv msg, uid : " << lUin << ", addr : " << stClientParam.sAddr << endl;
    RoomServant::async_response_onRequest(current, 0);

    //解码
    XGameComm::TPackage pkg;
    pbToObj(sMsgPack, pkg);
    ROLLLOG_DEBUG << "recv msg, pkg :" << logPb(pkg) << endl;

    //无数据
    if (pkg.vecmsghead_size() == 0)
    {
        ROLLLOG_DEBUG << "package empty." << endl;
        return -1;
    }

    //检测RoomID,不存在的roomid
    bool bIsExist = CRoomSingleton::getInstance()->isExistRID4Map(pkg.sroomid());
    if (!bIsExist)
    {
        LOG_ERROR << lUin << "|" << pkg.sroomid() << "|" << "roomid not exist, msgid : " << pkg.vecmsghead(0).nmsgid() << endl;
        return -2;
    }

    //消息分发处理
    for (int i = 0; i < pkg.vecmsghead_size(); ++i)
    {
        int64_t ms1 = TC_Common::now2ms();

        HeadInfo stHeadInfo;
        stHeadInfo.iGameID = pkg.igameid();
        stHeadInfo.sRoomID = pkg.sroomid();
        stHeadInfo.sRoomServerID = "1";
        stHeadInfo.sPlayerIP = stClientParam.sAddr;
        stHeadInfo.sServantAddr = sCurServrantAddr;

        auto pPlayer = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lUin);
        if (pPlayer)
        {
            pPlayer->updateMsgHead(stHeadInfo);
        }

        auto &head = pkg.vecmsghead(i);
        ROLLLOG_DEBUG << "recv msg, msgid :" << head.nmsgid() << ", lUin:" << lUin << endl;

        switch (head.nmsgid())
        {
        case XGameQs::E_QUICK_START_MSGID_LOGIN_ROOM_REQ:
        case XGameKo::E_KNOCKOUT_MSGID_LOGIN_ROOM_REQ:
        case XGameAi::E_AI_MSGID_LOGIN_ROOM_REQ:
        case XGamePr::E_PRIVATE_MSGID_LOGIN_ROOM_REQ:
        case XGameSh::E_SH_MSGID_LOGIN_ROOM_CS:
        case XGameDDZ::E_DDZ_MSGID_LOGIN_ROOM_CS:
        case XGameMJ::E_MJ_MSGID_LOGIN_ROOM_CS:
        case XGameSNG::E_SNG_MSGID_LOGIN_ROOM_CS:
        case XGameMtt::E_MTT_MSGID_LOGIN_ROOM_CS:
        {
            vector<char> vecOutBuffer;
            if (pkg.vecmsgdata_size() > i)
            {
                vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
            }
            iRet = doLoginReq(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
            break;
        }
        // case XGameKo::E_KNOCKOUT_MSGID_LOGIN_ROOM_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = doLoginReq(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        // case XGameAi::E_AI_MSGID_LOGIN_ROOM_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = doLoginReq(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        // case XGamePr::E_PRIVATE_MSGID_LOGIN_ROOM_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = doLoginReq(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        case XGameQs::Eum_Fs_Msgid::E_QUICK_START_MSGID_GET_TABLE_LIST_REQ:
        case XGameKo::Eum_Ko_Msgid::E_KNOCKOUT_MSGID_GET_TABLE_LIST_REQ:
        case XGamePr::Eum_Pr_Msgid::E_PRIVATE_MSGID_GET_TABLE_LIST_REQ:
        case XGamePr::Eum_Pr_Msgid::E_PRIVATE_MSGID_GET_TABLE_COUNT_REQ:
        case XGameAi::Eum_Ai_Msgid::E_AI_START_MSGID_GET_TABLE_LIST_REQ:
        case XGameSh::Eum_Sh_Msgid::E_SH_MSGID_ROOM_LIST_CS:
        case XGameDDZ::Eum_DDZ_Msgid::E_DDZ_MSGID_ROOM_LIST_CS:
        case XGameMJ::Eum_MJ_Msgid::E_MJ_MSGID_ROOM_LIST_CS:
        case XGameSNG::E_SNG_MsgId::E_SNG_MSGID_ROOM_LIST_CS:
        case XGameMtt::Eum_Mtt_Msgid::E_MTT_MSGID_ROOM_LIST_CS:
        case XGameMtt::Eum_Mtt_Msgid::E_MTT_MSGID_ROOM_INFO_CS:
        {
            vector<char> vecOutBuffer;
            if (pkg.vecmsgdata_size() > i)
            {
                vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
            }
            iRet = onGetTableList(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
            break;
        }
        // case XGameKo::Eum_Ko_Msgid::E_KNOCKOUT_MSGID_GET_TABLE_LIST_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = onGetTableList(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        // case XGamePr::Eum_Pr_Msgid::E_PRIVATE_MSGID_GET_TABLE_LIST_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = onGetTableList(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        // case XGameAi::Eum_Ai_Msgid::E_AI_START_MSGID_GET_TABLE_LIST_REQ:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = onGetTableList(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        // case XGameSh::Eum_Sh_Msgid::E_SH_MSGID_ROOM_LIST_CS:
        // {
        //     vector<char> vecOutBuffer;
        //     if (pkg.vecmsgdata_size() > i)
        //     {
        //         vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
        //     }
        //     iRet = onGetTableList(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stHeadInfo, stClientParam, stUserBaseInfo);
        //     break;
        // }
        default:
        {
            if (head.nmsgid() != XGameComm::Eum_Comm_Msgid::E_MSGID_GAME_SO_REQ             //1
                    && head.nmsgid() != XGameComm::Eum_Comm_Msgid::E_MSGID_CHAT_TE_REQ      //31
                    && head.nmsgid() != XGameComm::Eum_Comm_Msgid::E_MSGID_CHAT_AUDIO_REQ   //33
                    && head.nmsgid() != XGameComm::Eum_Comm_Msgid::E_MSGID_GPS_REQ)         //35 )
            {
                checkOnline(lUin, stHeadInfo);
            }

            vector<char> vecOutBuffer;
            if (pkg.vecmsgdata_size() > i)
            {
                vecOutBuffer.insert(vecOutBuffer.begin(), pkg.vecmsgdata(i).begin(), pkg.vecmsgdata(i).end());
            }

            iRet = doRequest(lUin, pkg.sroomid(), head.nmsgid(), vecOutBuffer, stClientParam, stUserBaseInfo);
            break;
        }
        }

        int64_t ms2 = TNOWMS;
        if ((ms2 - ms1) > COST_MS)
        {
            ROLLLOG_WARN << "@Performance, msgid:" << head.nmsgid() << ", costTime:" << (ms2 - ms1) << endl;
        }
    }

    //跟新玩家最新存活时间
    CPlayerMngSingleton::getInstance()->updateUserActiveTime(lUin);

    FUNC_EXIT((TNOWMS - tTime), pkg.vecmsghead(0).nmsgid(), iRet);
    __CATCH__
    return iRet;
}

//在线检查
int RoomServantImp::checkOnline(const long lPlayerID, const HeadInfo &stHeadInfo)
{
    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lPlayerID))
    {
        XGameComm::TPackage tPkg;
        tPkg.set_igameid(stHeadInfo.iGameID);
        tPkg.set_sroomid(stHeadInfo.sRoomID);

        auto ptuid = tPkg.mutable_stuid();
        ptuid->set_luid(lPlayerID);
        tPkg.add_vecmsgdata("");

        auto mh = tPkg.add_vecmsghead();
        mh->set_nmsgid(XGameComm::E_MSGID_USER_NOT_EXIST_NOTIFY);
        mh->set_nmsgtype(XGameComm::MSGTYPE_NOTIFY);

        if (stHeadInfo.sServantAddr != "")
        {
            LOG_DEBUG << lPlayerID << "|" << "toClientMsg, info = { uid:"
                      << lPlayerID << ", roomid:" << stHeadInfo.sRoomID << ", " << "msgid list = { " << XGameComm::E_MSGID_USER_NOT_EXIST_NOTIFY << "}"
                      << ", msgDataSize:" << tPkg.vecmsgdata_size() << " }" << endl;

            auto pPushPrx = Application::getCommunicator()->stringToProxy<JFGame::PushPrx>(stHeadInfo.sServantAddr);
            if (pPushPrx)
                pPushPrx->tars_hash(lPlayerID)->async_doPushBufByRoomID(NULL, tPkg.stuid().luid(), pbToString(tPkg), stHeadInfo.sRoomID, XGameComm::E_MSGID_USER_NOT_EXIST_NOTIFY);
            else
                LOG_ERROR << "pPushPrx is null" << endl;
        }
    }

    return 0;
}


// 处理登录消息, 判断玩家是否在线。
// 1.不在线-插入map并通过infoServer拉取玩家个人信息
// 2.在线- 发送client重入消息，实现玩家吸入。
// 此方案有待商讨确认。
int RoomServantImp::doLoginReq(long lPlayerID, const short nMsgID, const HeadInfo &stHeadInfo)
{
    auto pPlayer = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lPlayerID);
    if (pPlayer)
    {
        pPlayer->updateMsgHead(stHeadInfo);
    }

    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lPlayerID))
    {
        auto pHallServant = g_app.getOuterFactoryPtr()->getHallServantPrx(lPlayerID);
        if (!pHallServant)
        {
            LOG_ERROR << "pHallServant is nullptr" << endl;
            return -1;
        }

        userinfo::GetUserBasicReq req;
        req.uid = lPlayerID;

        auto cb = new AsyncLoginRoomCallback(req);
        cb->servantImp = this;
        cb->loginType = 1;
        cb->stHeadInfo = stHeadInfo;
        cb->nMsgID = nMsgID;
        pHallServant->async_getUserBasic(cb, req);
        return 0;
    }

    LOG_DEBUG << lPlayerID << "|" << stHeadInfo.sRoomID << "|" << nMsgID << "|" << "login req msg, user online, recoming." << endl;

    //判断用户是否进入同一个场次
    std::string sExistRoomID = CPlayerMngSingleton::getInstance()->getRoomIDFromUMap(lPlayerID);
    if (sExistRoomID != stHeadInfo.sRoomID)
    {
        LOG_ERROR << lPlayerID << "|" << "user exist other RoomID, ReqRoomID : " << stHeadInfo.sRoomID << ", existRoomID : " << sExistRoomID << endl;
        toClientLoginMsg(lPlayerID, nMsgID, 3);
    }

    toClientLoginMsg(lPlayerID, nMsgID, 0);
    return 0;
}

//登录处理
int RoomServantImp::doLoginReq(long lPlayerID, const string sRoomID, const short nMsgID, const vector<char> &vecMsgData, const HeadInfo &stHeadInfo,
                               const JFGame::TClientParam &stClientParam, const JFGame::UserBaseInfoExt &stUserBaseInfo)
{
    auto pPlayer = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lPlayerID);
    if (pPlayer)
    {
        pPlayer->updateMsgHead(stHeadInfo);
    }

    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lPlayerID))
    {
        auto pHallServant = g_app.getOuterFactoryPtr()->getHallServantPrx(lPlayerID);
        if (!pHallServant)
        {
            LOG_ERROR << "pHallServant is nullptr" << endl;
            return -1;
        }

        userinfo::GetUserBasicReq req;
        req.uid = lPlayerID;

        auto cb = new AsyncLoginRoomCallback(req);
        cb->servantImp = this;
        cb->loginType = 2;
        cb->stHeadInfo = stHeadInfo;
        cb->nMsgID = nMsgID;
        cb->sRoomID = sRoomID;
        cb->stClientParam = stClientParam;
        cb->stUserBaseInfo = stUserBaseInfo;
        cb->vecMsgData.insert(cb->vecMsgData.end(), vecMsgData.begin(), vecMsgData.end());
        pHallServant->async_getUserBasic(cb, req);
        return 0;
    }
    else
    {
        LOG_DEBUG<<"player is playermap... lPlayerID: " << lPlayerID<< endl;
    }

    ReqInfo stReqInfo;
    stReqInfo.lUID = lPlayerID;
    stReqInfo.nMsgID = nMsgID;
    stReqInfo.vecMsgData = vecMsgData;
    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "find pTableMng error." << endl;
        return -1;
    }

    int iRet = pTableMng->onMessage(stReqInfo, stClientParam, stUserBaseInfo);
    if (iRet != 0 && nMsgID != XGameKo::E_KNOCKOUT_MSGID_LOGIN_ROOM_REQ)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "process msg error, ret : " << iRet << endl;
        return -2;
    }

    return 0;
}

//给客户端发送登录消息
int RoomServantImp::toClientLoginMsg(const long lPlayerID, const short nMsgID, const int iRetCode)
{
    RespInfo stRespInfo;
    stRespInfo.lUID = lPlayerID;
    switch (nMsgID)
    {
    case JFGamepr::E_PRIVATE_MSGID_LOGIN_ROOM_REQ:
    {
        JFGamepr::TPMsgRespLoginRoom resp;
        resp.iResultID = iRetCode;
        stRespInfo.vecMsgDataList.push_back(Encoding<JFGamepr::TPMsgRespLoginRoom>(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGamepr::E_PRIVATE_MSGID_LOGIN_ROOM_RESP));
        break;
    }
    case JFGamefs::E_FREE_SITTING_MSGID_LOGIN_ROOM_REQ:
    {
        JFGamefs::TFSMsgRespLoginRoom resp;
        resp.iResultID = iRetCode;
        stRespInfo.vecMsgDataList.push_back(Encoding<JFGamefs::TFSMsgRespLoginRoom>(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGamefs::E_FREE_SITTING_MSGID_LOGIN_ROOM_RESP));
        break;
    }
    case XGameQs::E_QUICK_START_MSGID_LOGIN_ROOM_REQ:
    {
        XGameQs::TQSMsgRespLoginRoom resp;
        resp.set_iresultid(iRetCode);
        stRespInfo.vecMsgDataList.push_back(pbTobuffer(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(XGameQs::Eum_Fs_Msgid::E_QUICK_START_MSGID_LOGIN_ROOM_RESP));
        OuterFactorySingleton::getInstance()->toClientPb(stRespInfo);
        return 0;
    }
    case JFGameko::E_KNOCKOUT_MSGID_LOGIN_ROOM_REQ:
    {
        JFGameko::TKOMsgRespLoginRoom resp;
        resp.iResultID = iRetCode;
        stRespInfo.vecMsgDataList.push_back(Encoding<JFGameko::TKOMsgRespLoginRoom>(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGameko::E_KNOCKOUT_MSGID_LOGIN_ROOM_RESP));
        break;
    }
    case JFGamecl::E_CLUB_MSGID_LOGIN_ROOM_REQ:
    {
        JFGamecl::TCMsgRespLoginRoom resp;
        resp.iResultID = iRetCode;
        stRespInfo.vecMsgDataList.push_back(Encoding<JFGamecl::TCMsgRespLoginRoom>(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGamecl::E_CLUB_MSGID_LOGIN_ROOM_RESP));
        break;
    }
    case JFGamerg::E_ROOM_GOLD_MSGID_LOGIN_ROOM_REQ:
    {
        JFGamerg::TRGMsgRespLoginRoom resp;
        resp.iResultID = iRetCode;
        stRespInfo.vecMsgDataList.push_back(Encoding<JFGamerg::TRGMsgRespLoginRoom>(resp));
        stRespInfo.vecMsgIDList.push_back(static_cast<short>(JFGamerg::E_ROOM_GOLD_MSGID_LOGIN_ROOM_RESP));
        break;
    }
    default:
    {
        LOG_ERROR << lPlayerID << "|" << nMsgID << "|" << "login room msgid error" << endl;
        return -2;
    }
    }

    OuterFactorySingleton::getInstance()->toClient(stRespInfo);
    return 0;
}

//请求处理
int RoomServantImp::doRequest(long lPlayerID, const string sRoomID, const short nMsgID, const vector<char> &vecMsgData,
                              const JFGame::TClientParam &stClientParam, const JFGame::UserBaseInfoExt &stUserBaseInfo)
{
    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lPlayerID) && nMsgID != XGameSh::E_SH_MSGID_ROOM_LIST_CS)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "user is not online." << endl;
        return -1;
    }

    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "find pTableMng error." << endl;
        return -2;
    }

    ReqInfo stReqInfo;
    stReqInfo.lUID = lPlayerID;
    stReqInfo.nMsgID = nMsgID;
    stReqInfo.vecMsgData = vecMsgData;
    int iRet = pTableMng->onMessage(stReqInfo, stClientParam, stUserBaseInfo);
    if (iRet != 0)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "process msg error, ret : " << iRet << endl;
        return -3;
    }

    return 0;
}

//取桌子列表
int RoomServantImp::onGetTableList(long lPlayerID, const string sRoomID, const short nMsgID, const vector<char> &vecMsgData, const HeadInfo &stHeadInfo,
                                   const JFGame::TClientParam &stClientParam, const JFGame::UserBaseInfoExt &stUserBaseInfo)
{
    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lPlayerID))
    {
        CPlayerMngSingleton::getInstance()->insertPlayer2UMap(lPlayerID);
        // auto pPlayer = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lPlayerID);
        // if (!pPlayer)
        // {
        //     LOG_ERROR << "pPlayer is nullptr, PlayerId:" << lPlayerID << endl;
        //     return -3;
        // }

        // pPlayer->updateMsgHead(stHeadInfo);
        //OuterFactorySingleton::getInstance()->async2GetUserBasic(lPlayerID);
    }
    else
    {
        //判断用户是否进入同一个场次
        std::string sExistRoomID = CPlayerMngSingleton::getInstance()->getRoomIDFromUMap(lPlayerID);
        if (sExistRoomID != stHeadInfo.sRoomID)
        {
            LOG_ERROR << lPlayerID << "|" << "user exist other RoomID, ReqRoomID : " << sRoomID << ", existRoomID : " << sExistRoomID << endl;
            //toClientLoginMsg(lPlayerID, nMsgID, 3);
        }
    }

    auto pPlayer = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(lPlayerID);
    if (!pPlayer)
    {
        LOG_ERROR << "pPlayer is nullptr, PlayerId:" << lPlayerID << endl;
        return -3;
    }

    pPlayer->updateMsgHead(stHeadInfo);

    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
    if (pTableMng == nullptr)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "find pTableMng error." << endl;
        return -1;
    }

    ReqInfo stReqInfo;
    stReqInfo.lUID = lPlayerID;
    stReqInfo.nMsgID = nMsgID;
    stReqInfo.vecMsgData = vecMsgData;
    int iRet = pTableMng->onMessage(stReqInfo, stClientParam, stUserBaseInfo);
    if (iRet != 0)
    {
        LOG_ERROR << lPlayerID << "|" << sRoomID << "|" << nMsgID << "|" << "process msg error, ret : " << iRet << endl;
        return -2;
    }

    return 0;
}

/**
 * 玩家断线接口
 */
tars::Int32 RoomServantImp::onOffline(tars::Int64 lUin, const std::string &sRoomID, bool standup, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY(lUin);

    int iRet = 0;
    LOG_DEBUG << lUin << "|" << "offline msg, uid: " << lUin << ", sRoomID: " << sRoomID << endl;

    __TRY__

    if (!CPlayerMngSingleton::getInstance()->isExistPlayerMap(lUin))
    {
        return 0;
    }

    std::string sRoomID = CPlayerMngSingleton::getInstance()->getRoomIDFromUMap(lUin);
    if (sRoomID == "")
    {
        LOG_ERROR << lUin << "|" << "offline msg, find roomid is null." << endl;
        return -1;
    }

    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(sRoomID);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << lUin << "|" << sRoomID << "|" << "offline msg, find pTableMng error." << endl;
        return -2;
    }

    //
    int iRet = pTableMng->onOfflineMsg(lUin, standup);
    if (iRet != 0)
    {
        LOG_ERROR << lUin << "|" << sRoomID << "|" << "process offline msg error, ret : " << iRet << endl;
        return -3;
    }

    __CATCH__

    FUNC_EXIT("", "offline", iRet);

    return iRet;
}

tars::Int32 RoomServantImp::notifyRoom(const java2room::NotifyRoomReq &req, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY("");

    int iRet = 0;

    LOG_DEBUG << "recv java notify msg, req4Java : " << logTars(req) << endl;

    CTableMng *pTableMng = CRoomSingleton::getInstance()->findTMngByRID(req.roomId);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << req.roomId << "|" << req.messageId << "|" << "find pTableMng error." << endl;
        return -1;
    }

    ReqInfo stReqInfo;
    stReqInfo.lUID = 0;
    stReqInfo.nMsgID = req.messageId;
    tobuffer(req, stReqInfo.vecMsgData);

    //
    TClientParam clientparam;
    UserBaseInfoExt stUserBaseInfo;
    iRet = pTableMng->onMessage(stReqInfo, clientparam, stUserBaseInfo);
    if (iRet != 0)
    {
        LOG_ERROR << req.roomId << "|" << req.messageId << "|" << "process msg error, ret : " << iRet << endl;
    }

    FUNC_EXIT("", req.messageId, iRet);
    return 0;
}

/**
 * 拉取房间奖池数据
*/
tars::Int32 RoomServantImp::onLoadPrizePool(const string &roomid, tars::TarsCurrentPtr current)
{
    LOG_DEBUG << " load room prize . roomid:" << roomid << endl;

    CTableMng *pTableMng = CRoomSingleton::getInstance()->findTMngByRID(roomid);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << "roomid is not exist, req.sRoomID : " << roomid << endl;
        return -2;
    }

    //
    int iRet = pTableMng->LoadPrizePool(roomid);
    if (iRet != 0)
    {
        return -3;
    }

    return 0;
}

tars::Int32 RoomServantImp::onPushRobot(const robot::TPushRobotReq &req, robot::TPushRobotRsp &rsp, tars::TarsCurrentPtr current)
{
    LOG_DEBUG << "recv push robot notify msg, req4Robot:" << req.sRoomID << "|" << req.iBatchID << "|" << req.sServrantAddr << "|" << req.vecRobot.size() << "|" << endl;

    if (req.sServrantAddr == "" || req.sRoomID == "" || req.vecRobot.empty())
    {
        LOG_ERROR << "parameter err, req.sServrantAddr : " << req.sServrantAddr << ", req.sRoomID: " << req.sRoomID << ", req.vecRobot.size:" << req.vecRobot.size() << endl;
        rsp.iResult = -1;
        return -1;
    }

    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(req.sRoomID);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << "roomid is not exist, req.sRoomID : " << req.sRoomID << endl;
        rsp.iResult = -2;
        return -2;
    }

    int iRetCode = 0;
    vector<string> vecItems = tars::TC_Common::sepstr<string>(req.sRoomID, ":");
    vector<robot::TRobotInfo> vecRobot(req.vecRobot);
    for (const auto &robot : req.vecRobot)
    {
        //Player management pool
        CPlayerMngSingleton::getInstance()->insertPlayer2UMap(robot.lPlayerID);

        bool bResult = true;
        auto pRobot = CPlayerMngSingleton::getInstance()->findPlayerPtrByUID(robot.lPlayerID);
        if (pRobot)
        {
            URobot stURobot;
            stURobot.bRobot = true;
            stURobot.iBatchID = req.iBatchID;
            pRobot->updateURobot(stURobot);

            HeadInfo stHeadInfo;
            stHeadInfo.iGameID = S2I(vecItems[2]);;
            stHeadInfo.sRoomID = req.sRoomID;
            stHeadInfo.sRoomServerID = "1";
            stHeadInfo.sPlayerIP = robot.sPlayerIP;
            stHeadInfo.sServantAddr = req.sServrantAddr;
            pRobot->updateMsgHead(stHeadInfo);
            bResult = true;
        }
        else
        {
            bResult = false;
            LOG_ERROR << "pRobot is nullptr: lPlayerID=" << robot.lPlayerID << ", iRetCode=" << iRetCode << endl;
        }

        if (bResult)
        {
            iRetCode = OuterFactorySingleton::getInstance()->async2GetUserBasic(robot.lPlayerID);
            if (iRetCode != 0)
            {
                LOG_ERROR << "async2UserDetail() failed, RobotID: " << robot.lPlayerID << ", iRetCode:" << iRetCode << endl;
                bResult = false;
            }

            if (bResult)
            {
                //上报机器人在线状态
                userstate::ReportOnlineStateReq treq;
                treq.uid = robot.lPlayerID;
                treq.state = userstate::E_ONLINE_STATE_ONLINE;
                treq.gwAddr = req.sServrantAddr;
                auto pPushServant = OuterFactorySingleton::getInstance()->getPushServerPrx(robot.lPlayerID);
                if (pPushServant)
                    pPushServant->tars_hash(robot.lPlayerID)->async_reportOnlineState(NULL, treq);
                else
                    bResult = false;
            }
        }

        if (!bResult)
        {
            auto it = find(vecRobot.begin(), vecRobot.end(), robot);
            if (it != vecRobot.end())
                vecRobot.erase(it);

            CPlayerMngSingleton::getInstance()->eraseUser4Umap(robot.lPlayerID);
            CPlayerMngSingleton::getInstance()->erasePlayerFromUMap(robot.lPlayerID);
            CPlayerMngSingleton::getInstance()->erasePlayerFromU2TMap(robot.lPlayerID);
            CPlayerMngSingleton::getInstance()->erasePlayerFormTable(robot.lPlayerID);
            LOG_ERROR << "Failed to add robot: lPlayerID=" << robot.lPlayerID << ", iRetCode=" << iRetCode << endl;
        }
        else
        {
            LOG_WARN << "Add robot successfully: lPlayerID=" << robot.lPlayerID << ", iRetCode=" << iRetCode << endl;
        }
    }

    robot::TPushRobotReq reqRobot;
    reqRobot.sRoomID = req.sRoomID;
    reqRobot.iBatchID = req.iBatchID;
    reqRobot.sServrantAddr = req.sServrantAddr;
    reqRobot.vecRobot = vecRobot;

    ReqRobot stReqRobot;
    tobuffer(reqRobot, stReqRobot.vecMsgData);
    int iRet = pTableMng->onPushRobotMsg(stReqRobot);
    if (iRet == 0)
    {
        vector<long> stRobotList;
        CPlayerMngSingleton::getInstance()->getURobotListByRommID(req.sRoomID, stRobotList);

        //当前批次机器人列表
        vector<long> stCurBatchRobotList;
        for (auto &robot : stRobotList)
        {
            URobot stURobot;
            CPlayerMngSingleton::getInstance()->getURobot(robot, stURobot);
            if (req.iBatchID == stURobot.iBatchID)
            {
                robot::TRobot stRobot;
                stRobot.lPlayerID = robot;
                stCurBatchRobotList.push_back(robot);
                auto it = rsp.mapRobotBatch.find(stURobot.iBatchID);
                if (it == rsp.mapRobotBatch.end())
                {
                    vector<robot::TRobot> temp;
                    temp.push_back(stRobot);
                    rsp.mapRobotBatch.insert(pair<int, vector<robot::TRobot>>(stURobot.iBatchID, temp));
                }
                else
                {
                    it->second.push_back(stRobot);
                }
            }
        }

        //连续推送多个批次，每次校验机器人不在当前批次，该机器人下线
        for (const auto &r : stCurBatchRobotList)
        {
            auto it = std::find_if(vecRobot.begin(), vecRobot.end(), [r](const robot::TRobotInfo & rb)
            {
                return rb.lPlayerID == r;
            });

            if (it == vecRobot.end())
            {
                LOG_DEBUG << r << "|" << req.sRoomID << "|" << "not current batch, robot offline, uid:" << r << endl;
                pTableMng->onOfflineMsg(r, false);
            }
        }
    }
    else
    {
        rsp.iResult = -3;
        LOG_ERROR << "room load robots failed, roomId: " << req.sRoomID << endl;
    }

    ostringstream os;
    for (auto &batch : rsp.mapRobotBatch)
    {
        os << "{" << batch.first << "|" << batch.second.size() << "}";
    }

    LOG_INFO << "send push robot notify msg, resp4Robot:" << rsp.iResult << "|" << req.sRoomID << "|" << os.str() << "|" << endl;
    FDLOG_ROBOT_INFO << req.sRoomID << "|" << req.iBatchID << "|" << "add robot finish, req4RobotCount:" << req.vecRobot.size() << ", resp4RobotCount:" << os.str() << endl;
    return 0;
}

tars::Int32 RoomServantImp::onRoomAIDecide(const ai::TAIBrainDecideRsp &rsp, tars::TarsCurrentPtr current)
{
    auto pTableMng = CRoomSingleton::getInstance()->findTMngByRID(rsp.sRoomID);
    if (nullptr == pTableMng)
    {
        LOG_ERROR << "roomid is not exist, rsp.sRoomID : " << rsp.sRoomID << endl;
        return -1;
    }

    pTableMng->onRoomAIDecideMsg(rsp);
    return 0;
}

/**
 * 游戏定时器处理主循环
 */
tars::Int32 RoomServantImp::doCustomMessage(bool bExpectIdle)
{
    __TRY__

    CAIMngSingleton::getInstance()->loopThirdPartyMng();

    __CATCH__


    __TRY__

    static int64_t lastUpdateTime = 0;
    int64_t current = TNOWMS;
    int64_t interval = 100;
    if (current >= lastUpdateTime + interval)
    {
        lastUpdateTime = current;
        CheckTimer();
    }

    // if (m_bIsSetTimer)
    // {
    //     m_bIsSetTimer = false;
    //     CheckTimer();
    // }

    __CATCH__

    return 0;
}

/**
* 激活定时器
*/
void RoomServantImp::ActiveTimer()
{
    m_bIsSetTimer = true;
}

/**
* 处理定时器
*/
void RoomServantImp::CheckTimer()
{
    //每次最多处理任务数量
    const int taskNum = 200;
    //游戏定时器处理
    XTimer::onTickTock(taskNum);
    //游戏配牌处理
    getDealCards();
}

/**
 * 获取取配牌数据
*/
void RoomServantImp::getDealCards()
{
    static uint32_t counter = 0;
    static uint32_t interval = 6000;
    if (0 == ((counter++) % interval))
    {
        if (OuterFactorySingleton::getInstance()->getDealFlag())
        {
            OuterFactorySingleton::getInstance()->readDealCards();
        }
    }
}


